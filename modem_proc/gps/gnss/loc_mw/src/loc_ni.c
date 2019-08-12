/******************************************************************************
  @file:  loc_ni.c
  @brief: Location Middleware NI module

  DESCRIPTION
  This module handles the NI Call flows and mediates them to PDAPI.
  
  
  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  Copyright (c) 2013-2014 QUALCOMM Atheros Incorporated.
  All Rights Reserved. QUALCOMM Atheros Proprietary and Confidential
  -----------------------------------------------------------------------------
 ******************************************************************************/


/*=============================================================================
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_ni.c#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $
=============================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"
#include "queue.h"

#include "aries_os_api.h"
#include "pdapi.h"
#include "pdsm_atl.h"

#include "loc_api_2.h"
#include "loc_task.h"
#include "loc_client.h"
#include "loc_pd.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_pa.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"

#include "location_service_v02.h"


/* Mapping of PDAPI Data-Coding-Scheme to Loc-MW Enumerations */
static struct 
{
  loc_ni_datacoding_scheme_e_type                       e_LocDcs;
  pdsm_lcs_notify_verify_datacoding_scheme_e_type       e_PdsmDcs;
} z_LocNiDcsMap[]=                             
{
  { LOC_NI_PRESUPL_ISO646IRV,           PDSM_PRESUPL_ISO646IRV                },      
    
  { LOC_NI_PRESUPL_ISO8859,             PDSM_PRESUPL_ISO8859                  },      
    
  { LOC_NI_PRESUPL_UTF8,                PDSM_PRESUPL_UTF8                     },            
    
  { LOC_NI_PRESUPL_UTF16,               PDSM_PRESUPL_UTF16                    },         
    
  { LOC_NI_PRESUPL_UCS2,                PDSM_PRESUPL_UCS2                     },      
    
  { LOC_NI_PRESUPL_GSM_DEFAULT,         PDSM_PRESUPL_GSM_DEFAULT              },      
    
  { LOC_NI_PRESUPL_SHIFT_JIS,           PDSM_PRESUPL_SHIFT_JIS                },            
    
  { LOC_NI_PRESUPL_JIS,                 PDSM_PRESUPL_JIS                      },         

  { LOC_NI_PRESUPL_EUC,                 PDSM_PRESUPL_EUC                      },      
    
  { LOC_NI_PRESUPL_GB2312,              PDSM_PRESUPL_GB2312                   },      

  { LOC_NI_PRESUPL_CNS11643,            PDSM_PRESUPL_CNS11643                 },

  { LOC_NI_PRESUPL_KSC1001,             PDSM_PRESUPL_KSC1001                  },

  { LOC_NI_SS_GERMAN,                   PDSM_SS_GERMAN                       },

  { LOC_NI_SS_ENGLISH,                  PDSM_SS_ENGLISH                       },

  { LOC_NI_SS_ITALIAN,                  PDSM_SS_ITALIAN                       },      
    
  { LOC_NI_SS_FRENCH,                   PDSM_SS_FRENCH                        },            
    
  { LOC_NI_SS_SPANISH,                  PDSM_SS_SPANISH                       },         
  
  { LOC_NI_SS_DUTCH,                    PDSM_SS_DUTCH                         },      
    
  { LOC_NI_SS_SWEDISH,                  PDSM_SS_SWEDISH                       },      
  
  { LOC_NI_SS_DANISH,                   PDSM_SS_DANISH                        },
  
  { LOC_NI_SS_PORTUGUESE,               PDSM_SS_PORTUGUESE                    },
  
  { LOC_NI_SS_FINNISH,                  PDSM_SS_FINNISH                       },
  
  { LOC_NI_SS_NORWEGIAN,                PDSM_SS_NORWEGIAN                     },

  { LOC_NI_SS_GREEK,                    PDSM_SS_GREEK                         },      
    
  { LOC_NI_SS_TURKISH,                  PDSM_SS_TURKISH                       },            
    
  { LOC_NI_SS_HUNGARIAN,                PDSM_SS_HUNGARIAN                     },         
  
  { LOC_NI_SS_POLISH,                   PDSM_SS_POLISH                        },      
    
  { LOC_NI_SS_LANGUAGE_UNSPEC,          PDSM_SS_LANGUAGE_UNSPEC               },      
  
  { LOC_NI_SUPL_UTF8,                   PDSM_SUPL_UTF8                        },
  
  { LOC_NI_SUPL_UCS2,                   PDSM_SUPL_UCS2                        },
  
  { LOC_NI_SUPL_GSM_DEFAULT,            PDSM_SUPL_GSM_DEFAULT                 },
    
};  



// local variable to temporary store the notification text for UMTS CP 
static char umts_cp_notification_text[LOC_NI_MAX_CLIENT_NAME_LENGTH];


/* table mapping the QMI_LOC SUPL 2 GNSS tech masks to PDAPI
   pos tech mask */

static struct 
{
  qmiLocNiSuplVer2ExtGnssTypeMaskT_v02 qmiLoc_supl_ver2_ext_mask;
  uint32 pdapi_supl_ver2_ext_mask;
}gnss_type_conversion_table[] =
{
  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GPS_V02, 
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GPS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GLONASS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GLONASS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GALILEO_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GALILEO },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_SBAS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_SBAS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_QZSS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_QZSS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_MODERN_GPS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_MODERN_GPS } 
};

/* table mapping the QMI_LOC supported networks masks to PDAPI
   supported networks masks */
static struct  
{
  qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02 qmiLoc_supl_ver2_ext_mask;
  uint32 pdapi_supl_ver2_ext_mask;
}supported_networks_conversion_table[] = 
{
  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WLAN_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WLAN },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_GSM_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_GSM }, 

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WCDMA_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WCDMA },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_CDMA_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_CDMA },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HRDP_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_HRDP },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_UMB_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_UMB },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_LTE_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_LTE },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WIMAX_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WIMAX },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HISTORIC_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_HISTORIC },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_NONSVRV_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_NONSVRV }
};

#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_ALL \
    (QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WLAN_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_GSM_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WCDMA_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_CDMA_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HRDP_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_UMB_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_LTE_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WIMAX_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HISTORIC_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_NONSVRV_V02) 


static void loc_ni_event_cb
(
      void *user_data,
      pdsm_lcs_event_type  lcs_event,
      const pdsm_lcs_info_s_type *lcs_info_ptr
);

// NI cmd calback
static void loc_ni_cmd_cb
(
      void                      *data_block_ptr,
      pdsm_lcs_cmd_e_type        lcs_cmd,
      pdsm_lcs_cmd_err_e_type    lcs_cmd_error
);

// NI module used to process user reponse to the NI request
static int loc_ni_process_user_response
(
      loc_client_handle_type               client_handle,
      const loc_user_verify_s_type* user_resp_ptr
);

// Translate V1/V2 NI request from PDAPI to LOC API
static void  loc_ni_xlate_vx_req_from_pdapi_to_locapi
(
      const pdsm_lcs_vx_notify_verify_req_s_type* pdapi_vx_req_ptr,
      loc_ni_vx_notify_verify_req_s_type*      loc_vx_req_ptr
);

// Translate SUPL NI request from PDAPI to LOC API
static boolean  loc_ni_xlate_supl_req_from_pdapi_to_locapi
(
      const pdsm_lcs_supl_notify_verify_req_s_type* pdapi_supl_req_ptr,
      loc_ni_supl_notify_verify_req_s_type*         loc_supl_req_ptr
);

// Translate UMTS NI request from PDAPI tO LOC API
static void loc_ni_xlate_umts_req_from_pdapi_to_locapi
(
      const pdsm_lcs_umts_cp_notify_verify_req_s_type*  pdapi_umts_req_ptr,
      loc_ni_umts_cp_notify_verify_req_s_type*          loc_umts_req_ptr
);

// Translate V1/V2 NI request from LOC API to PD API
static void  loc_ni_xlate_vx_req_from_locapi_to_pdapi
(
      const loc_ni_vx_notify_verify_req_s_type*  loc_vx_req_ptr,
      pdsm_lcs_vx_notify_verify_req_s_type*     pdapi_vx_req_ptr
);

// Translate SUPL NI request from LOCAPI to PD API
static boolean  loc_ni_xlate_supl_req_from_locapi_to_pdapi
(
      const loc_ni_supl_notify_verify_req_s_type*  loc_supl_req_ptr,
      pdsm_lcs_supl_notify_verify_req_s_type*      pdapi_supl_req_ptr
);

// Translate UMTS NI request from PDAPI tO LOC API
static void loc_ni_xlate_umts_req_from_locapi_to_pdapi
(
      const loc_ni_umts_cp_notify_verify_req_s_type* loc_umts_req_ptr,
      pdsm_lcs_umts_cp_notify_verify_req_s_type*     pdapi_umts_req_ptr
);




//Translate the VX Version from LocApi to PDAPI 
static void loc_ni_xlate_vx_version_from_locapi_to_pdapi
(
  qmiLocVxVersionEnumT_v02  loc_vx_version,
  pdsm_lcs_agent_set_vx_config_s_type* pdapi_vx_version
);

//Translate the VX Version from PDAPI to LocAPI 
static void loc_ni_xlate_vx_version_from_pdapi_to_locapi
(
  pdsm_lcs_agent_version_e_type pdapi_vx_version,
  qmiLocVxVersionEnumT_v02*  loc_vx_version

);

/* Translate DCS from PDAPI to LocApi */
static boolean xlate_dcstype_from_pdsm_to_locapi (
            pdsm_lcs_notify_verify_datacoding_scheme_e_type pdsm_format,
            unsigned char *loc_format );

/* Translate DCS from LocAPI to PDAPI */
static boolean xlate_dcstype_from_locapi_to_pdapi (
            unsigned char loc_format ,
            pdsm_lcs_notify_verify_datacoding_scheme_e_type *pdsm_format);



// Initializet the NI module so that NI request will be processed by location module.
// This function will be called when first location client registers for NI event.
boolean loc_ni_init ()
{
   boolean                     ret_val = FALSE;
   pdsm_client_status_e_type   status;

   do
   {
      loc_middleware_data.ni_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_NI);

      if (loc_middleware_data.ni_client_id == -1)
      {
         LOC_MSG_ERROR ( "pdsm_client_init for NI failed", 0, 0, 0);
         break;
      }

      status= pdsm_client_act(loc_middleware_data.ni_client_id);

      if(status!= PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("pdsm_client_act failed, error code =%d", ret_val, 0, 0);
         break;
      }

      status= pdsm_client_lcs_reg(loc_middleware_data.ni_client_id,
                                    NULL,
                                    loc_ni_event_cb,
                                    PDSM_CLIENT_EVENT_REG,
                                    PDSM_LCS_EVENT_MASK,
                                    NULL
                                    );

      if(status!= PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("pdsm_client_lcs_reg failed, error code = %d", ret_val, 0, 0);
         break;
      }

      ret_val = TRUE;
   } while (0);

   // NI module data has been zero-initialized in the loc_task, no more special initialization is needed

   LOC_MSG_HIGH ("loc_ni_init returns =%d", ret_val, 0, 0);

   return ret_val;
}

// This function is used to process loc_ioctl that belongs to NI module.
int loc_ni_process_ioctl (loc_client_handle_type  client_handle,
                          loc_ioctl_e_type ioctl_type,
                          const loc_ioctl_data_u_type* ioctl_data)
{
   int ret_val = LOC_API_UNSUPPORTED;

   LOC_MSG_HIGH ("loc_ni_process_ioctl for type =%d", ioctl_type, 0, 0);

   if (ioctl_type == LOC_IOCTL_INFORM_NI_USER_RESPONSE)
   {
      if(NULL != ioctl_data)
      {
          ret_val = loc_ni_process_user_response (client_handle, &(ioctl_data->user_verify_resp));
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
   } 
   else if ( ioctl_type == LOC_IOCTL_SET_VX_CONFIG )
   {
     if(NULL != ioctl_data)
     {
       pdsm_lcs_agent_set_vx_config_s_type set_vx_config;
       memset ( &set_vx_config, 0, sizeof(set_vx_config) );
       
       ret_val = LOC_API_INVALID_PARAMETER;


       loc_ni_xlate_vx_version_from_locapi_to_pdapi (
                      ioctl_data->vx_config,
                      &set_vx_config );


       if ( TRUE == pdsm_lcs_set_vx_config(
                               PDSM_CLIENT_TYPE_NI,
                               set_vx_config ) )
       {
         ret_val = LOC_API_SUCCESS;
       }
     }
     else
     {
         ret_val = LOC_API_INVALID_PARAMETER;
     }
     if ( LOC_API_SUCCESS != ret_val ) 
     {
       LOC_MSG_ERROR("Could not set Vx Config error:%d",ret_val,0,0);
     }
   } 
   else if ( ioctl_type == LOC_IOCTL_GET_VX_CONFIG )
   {
      ret_val = LOC_API_INVALID_PARAMETER;
      if ( TRUE == pdsm_lcs_query_vx_config(
                              PDSM_CLIENT_TYPE_NI ) )
      {
        ret_val = LOC_API_SUCCESS;
      }

      if ( LOC_API_SUCCESS != ret_val ) 
      {
        LOC_MSG_ERROR("Could not set supl security error:%d",ret_val,0,0);
      }
   }

   return ret_val;
}

// Route user response for NI request to modem
static int loc_ni_process_user_response
(
      loc_client_handle_type        client_handle,
      const loc_user_verify_s_type* user_resp_ptr
)
{
   boolean ret_val = TRUE;
   pdsm_lcs_notification_resp_s_type resp_info;

   memset (&resp_info, 0, sizeof (pdsm_lcs_notification_resp_s_type));

   switch(user_resp_ptr->user_resp)
   {
   case LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT:
       resp_info.resp = PDSM_LCS_NOTIFY_VERIFY_ACCEPT;
   break;

   case LOC_NI_LCS_NOTIFY_VERIFY_DENY:
      resp_info.resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
   break;

   case LOC_NI_LCS_NOTIFY_VERIFY_NORESP:
      resp_info.resp = PDSM_LCS_NOTIFY_VERIFY_NORESP;
   break;

   default:
      resp_info.resp = PDSM_LCS_NOTIFY_VERIFY_NORESP;
      ret_val = FALSE;
   break;
   }

   switch(user_resp_ptr->ni_event_pass_back.event)
   {
   case LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ:
      resp_info.lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_VX_REQ;
      loc_ni_xlate_vx_req_from_locapi_to_pdapi (&(user_resp_ptr->ni_event_pass_back.payload.vx_req),
                                                &(resp_info.lcs_info.vx_req));
   break;

   case LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ:
      resp_info.lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_SUPL_REQ;
      ret_val = loc_ni_xlate_supl_req_from_locapi_to_pdapi (&(user_resp_ptr->ni_event_pass_back.payload.supl_req),
                                                  &(resp_info.lcs_info.supl_req));
   break;

   case LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ:
      resp_info.lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_UMTS_CP_REQ;
      loc_ni_xlate_umts_req_from_locapi_to_pdapi (&(user_resp_ptr->ni_event_pass_back.payload.umts_cp_req),
                                                  &(resp_info.lcs_info.umts_cp_req));
   break;

// earlier LocMW doesn't have this feature
#ifdef LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ
   case LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ:
      resp_info.lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_VX_SERVICE_INTERACTION;
      loc_ni_xlate_vx_req_from_locapi_to_pdapi (&(user_resp_ptr->ni_event_pass_back.payload.service_interaction_req.ni_vx_req),
                                                &(resp_info.lcs_info.vx_req));
   break;
#endif // #ifdef LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ

   default:
      ret_val = FALSE;
      break;
   }

   if (ret_val == TRUE) 
   {
      // Change summary Service interaction for SUPL MT PO and SUPL MT
      // SUPL MT PO:
      // 1: loc mw need to cancel the on-going MO session to allow SUPL MT PO to proceed. 
      // 2: If a SUPL MT is in progress, this will be killed by LSM
      // 3: LSM layer will make sure that in progress SUPL MT PO can not be killed
      //    by other incoming SUPL MT session or MO sessions   
      loc_pd_handle_service_interaction (&resp_info);

      ret_val = pdsm_lcs_agent_client_response(loc_ni_cmd_cb,
                                               (void *) client_handle,
                                               &resp_info,
                                               loc_middleware_data.ni_client_id
                                               );

      LOC_MSG_MED ("pdsm_lcs_agent_client_response returned %d", ret_val, 0, 0);
   }

   if (ret_val == TRUE)
   {
      return LOC_API_SUCCESS;
      // IOCTL status report will be issued in loc_ni_cmd_cb
   }
   else
   {
      return LOC_API_GENERAL_FAILURE;
   }
}

// NI event cb function: this is where the NI notify/verify request comes in
// Process the NI event (NI notification and verification event)
static void loc_ni_event_cb (void *user_data,
                             pdsm_lcs_event_type  lcs_event,
                             const pdsm_lcs_info_s_type *lcs_info_ptr)
{
   loc_cmd_s_type                cmd_data;
   loc_cmd_s_type*               cmd_ptr = &cmd_data;
   loc_ni_event_s_type*          loc_lcs_info_ptr  = NULL;
   boolean                       result = FALSE;
   loc_cmd_nty_client_s_type     *ptr_nty_client_data = NULL;
   pdsm_lcs_notification_resp_s_type * ptr_lcs_resp = NULL;

   LOC_MSG_HIGH ("loc_ni_event_cb, lcs_event = %d ", lcs_event, 0, 0);

   // Save the ni module data, to be used in following service interaction
   loc_middleware_data.ni_module_data.lcs_event = lcs_event;
   loc_middleware_data.ni_module_data.pdsm_lcs_info = lcs_info_ptr->lcs_info;

   do
   {
      // Initialize the cmd data 
      memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
      cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
      cmd_ptr->client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
      cmd_ptr->ptr_data = NULL;

      if (lcs_event == PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ)
      {
        ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
        if(NULL != ptr_nty_client_data)
        {
            cmd_ptr->ptr_data = (void *)ptr_nty_client_data;

              ptr_nty_client_data->event_type = LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST;
              loc_lcs_info_ptr  = &(ptr_nty_client_data->event_data.ni_request);

              loc_lcs_info_ptr->event = LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ;
              loc_ni_xlate_vx_req_from_pdapi_to_locapi (&(lcs_info_ptr->lcs_info.vx_req), &(loc_lcs_info_ptr->payload.vx_req));

              result = TRUE;
          }
          else
          {
              // memory allocation error
              result = FALSE;
          }         
      }
      else if (lcs_event == PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ)
      {
         // the typecast to loc_ni_notify_verify_e_type for 0 is just there to silence the compiler
         // what we need is to zero-initialize the struct
         loc_ni_supl_notify_verify_req_s_type supl_notify = {(loc_ni_notify_verify_e_type)0};
         loc_supl_req_e_type  supl_result = LOC_SUPL_REQ_DENY;
         if ( FALSE == 
                 loc_ni_xlate_supl_req_from_pdapi_to_locapi (&(lcs_info_ptr->lcs_info.supl_req), &supl_notify) )
         {
            result = FALSE;
            break;
         }

         supl_result = loc_pd_continue_processing_supl_request(&supl_notify);
         if(LOC_SUPL_REQ_DENY == supl_result)
         {
             // no need to continue processing this SUPL request
             // just ignore the whole event
             LOC_MSG_HIGH ("loc_ni_event_cb, silently deny SUPL NI request", 0, 0, 0);

             // instead of notifying the client, let's send an event to ourselves and deny it 
             // (switching thread context to avoid calling PDSM in a callback context, which could lead to deadlock) 
             ptr_lcs_resp = (pdsm_lcs_notification_resp_s_type *)loc_calloc(sizeof(pdsm_lcs_notification_resp_s_type));
             if(NULL != ptr_lcs_resp)
             {
                 // change command type from notifying the user to sending message back to SM
                 // just for this case
                 cmd_ptr->cmd_type = LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST;
                 cmd_ptr->ptr_data = (void *)ptr_lcs_resp;

                 ptr_lcs_resp->resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
                 ptr_lcs_resp->lcs_info_cmd_type = lcs_info_ptr->lcs_cb_cmd_type;
                 ptr_lcs_resp->lcs_info = lcs_info_ptr->lcs_info;

                 result = TRUE;
             }
             else
             {
                 // we are unable to reject the request, so SM probably will accept it by default after timeout
                 // there is nothing we can do
                 // memory allocation error
                 result = FALSE;
             }         
         }
         else if(LOC_SUPL_REQ_ACCEPT == supl_result)
         {
             LOC_MSG_HIGH ("loc_ni_event_cb, continue processing SUPL NI event", 0, 0, 0);
             ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
             if(NULL != ptr_nty_client_data)
             {
                 cmd_ptr->ptr_data = (void *)ptr_nty_client_data;

                 ptr_nty_client_data->event_type = LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST;
                 loc_lcs_info_ptr = &(ptr_nty_client_data->event_data.ni_request);

                 loc_lcs_info_ptr->event = LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ;
                 // copy the translated struct for notification to the client
                 loc_lcs_info_ptr->payload.supl_req = supl_notify;

                 result = TRUE;
             }
             else
             {
                 // we are unable to notify the client. SM probably will accept it after timeout by default
                 // memory allocation error
                 result = FALSE;
             }         
         }
         else
         {
              // LOC_SUPL_REQ_DROP, drop second SUPL silently
              LOC_MSG_HIGH ("loc_ni_event_cb, continue SUPL result = %d, drop SUPL", supl_result, 0, 0);
              result = FALSE;
         }

      }
      else if (lcs_event == PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ)
      {
        ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
        if(NULL != ptr_nty_client_data)
        {
            cmd_ptr->ptr_data = (void *)ptr_nty_client_data;

             ptr_nty_client_data->event_type = LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST;
             loc_lcs_info_ptr  = &(ptr_nty_client_data->event_data.ni_request);

             loc_lcs_info_ptr->event = LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ;
             loc_ni_xlate_umts_req_from_pdapi_to_locapi (&(lcs_info_ptr->lcs_info.umts_cp_req), &(loc_lcs_info_ptr->payload.umts_cp_req));

             result = TRUE;
         }
         else
         {
             // memory allocation error
             result = FALSE;
         }         
      }
// earlier LocMW releases does not have this feature
#ifdef LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ
      else if (lcs_event == PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION)
      {
         ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
         if(NULL != ptr_nty_client_data)
         {
             cmd_ptr->ptr_data = (void *)ptr_nty_client_data;

             ptr_nty_client_data->event_type = LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST;
             loc_lcs_info_ptr  = &(ptr_nty_client_data->event_data.ni_request);

             loc_lcs_info_ptr->event = LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ;
             loc_lcs_info_ptr->payload.service_interaction_req.service_interation_type = LOC_NI_SERVICE_INTERACTION_ONGOING_NI_INCOMING_MO;
             // In current LSM, the on-going vx ni event is not popuated properly, so we use the previously saved info
             if (loc_middleware_data.ni_module_data.lcs_event == PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ)
             {
                loc_ni_xlate_vx_req_from_pdapi_to_locapi (&(loc_middleware_data.ni_module_data.pdsm_lcs_info.vx_req), 
                                                          &(loc_lcs_info_ptr->payload.service_interaction_req.ni_vx_req));
             }

             // Now copy the info that are being populated properly in the event, this includes the notification/verification type and user response timeout
             switch(lcs_info_ptr->lcs_info.vx_req.notification_priv_type)
             {
               case PDSM_LCS_USER_NOTIFY_ONLY:
                 loc_lcs_info_ptr->payload.service_interaction_req.ni_vx_req.notification_priv_type = LOC_NI_USER_NOTIFY_ONLY;
               break;

               case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
                 loc_lcs_info_ptr->payload.service_interaction_req.ni_vx_req.notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
               break;

               case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
                 loc_lcs_info_ptr->payload.service_interaction_req.ni_vx_req.notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
               break;

               default:
                 loc_lcs_info_ptr->payload.service_interaction_req.ni_vx_req.notification_priv_type = LOC_NI_USER_NOTIFY_ONLY;
               break;
             }

             loc_lcs_info_ptr->payload.service_interaction_req.ni_vx_req.user_resp_timer_val = lcs_info_ptr->lcs_info.vx_req.user_resp_timer_val;

             result = TRUE;
         }
         else
         {
             // memory allocation error
             result = FALSE;
         }         
      }
#endif //#ifdef LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ
      /* In both cases, relay the vx-Config to the client, 
         The client ( QMI-LOC ) can verify if it the operation succeeded,
         Note: Had to do this as this is a limitation on PDAPI */
      else if ( (lcs_event == PDSM_LCS_EVENT_CLIENT_VX_CONFIG) ||
                (lcs_event == PDSM_LCS_EVENT_CLIENT_SET_VX_CONFIG) )
      {
         pdsm_lcs_agent_version_e_type vxVersion;
         uint32 q_payloadType;
         loc_ioctl_callback_data_u_type z_LocIoctlCbackData;
         
         if (lcs_event == PDSM_LCS_EVENT_CLIENT_VX_CONFIG)
         {
          vxVersion = lcs_info_ptr->lcs_info.vx_config.vxVersion;          
          q_payloadType = LOC_IOCTL_GET_VX_CONFIG;
         }
         else
         {
          vxVersion = lcs_info_ptr->lcs_info.set_vx_config.vxVersion;          
          q_payloadType = LOC_IOCTL_SET_VX_CONFIG;
         }
 
         
         loc_ni_xlate_vx_version_from_pdapi_to_locapi (
                      vxVersion, 
                      &(z_LocIoctlCbackData.vx_config));


         /* There is no status, We are gauranteed to get a callback, 
            if we get a callback, we pass the value that is returned,
            and hence reporting a success */
         loc_client_queue_ioctl_callback ((loc_client_handle_type) LOC_HANDLE_REGISTERED_CLIENTS,
                                          q_payloadType,
                                          LOC_API_SUCCESS ,
                                          &z_LocIoctlCbackData);
         result = TRUE;
      }      
      
      else
      {
         LOC_MSG_ERROR ("loc_ni_event_cb, unrecognized lcs_event = %d ", lcs_event, 0, 0);
         // Exit out of the loop
         break;
      }

      // Queue the cmd to notify the client who registers for those NI events
      if(TRUE == result)
      {
          LOC_MSG_HIGH ("loc_ni_event_cb, queue cmd", 0, 0, 0);
          if ( TRUE != loc_middleware_queue_cmd (cmd_ptr))
          {
              // reclaim resources allocated for the message
              if(NULL != cmd_ptr->ptr_data)
              {
                  loc_free(cmd_ptr->ptr_data);
              }
              ptr_nty_client_data = NULL;
              ptr_lcs_resp = NULL;
              result = FALSE;
          }
          else
          {
              // IPC sent, we're done.
              result = TRUE;
          }
      }
      else
      {
        LOC_MSG_HIGH ("loc_ni_event_cb, cannot be relayed to Loc-Clients", 0, 0, 0);
      }
   }while (0);

   if(TRUE != result)
   {
       LOC_MSG_ERROR ("loc_ni_event_cb, internal error", 0, 0, 0);
   }
}

// NI cmd callback
static void loc_ni_cmd_cb ( void                      *data_block_ptr,
                           pdsm_lcs_cmd_e_type        lcs_cmd,
                           pdsm_lcs_cmd_err_e_type    lcs_cmd_error)
{
   LOC_MSG_HIGH ("loc_ni_cmd_cb, cmd %d, cmd_err %d", lcs_cmd, lcs_cmd_error, 0);

   if (lcs_cmd == PDSM_LCS_CMD_NOTIFY_VERIFY_RESP)
   {
      if (lcs_cmd_error != PDSM_LCS_CMD_ERR_NOERR)
      {
         loc_client_queue_ioctl_callback ((loc_client_handle_type) data_block_ptr,
                                          LOC_IOCTL_INFORM_NI_USER_RESPONSE,
                                          LOC_API_GENERAL_FAILURE ,
                                          NULL);
      }
      else
      {

         loc_client_queue_ioctl_callback ((loc_client_handle_type) data_block_ptr,
                                          LOC_IOCTL_INFORM_NI_USER_RESPONSE,
                                          LOC_API_SUCCESS,
                                          NULL);
      }
   }
}

// Translate V1/V2 NI request from PDAPI to LOC API
static void  loc_ni_xlate_vx_req_from_pdapi_to_locapi
(
      const pdsm_lcs_vx_notify_verify_req_s_type* pdapi_vx_req_ptr,
      loc_ni_vx_notify_verify_req_s_type*      loc_vx_req_ptr
)
{
   switch(pdapi_vx_req_ptr->notification_priv_type)
   {
     case PDSM_LCS_USER_NOTIFY_ONLY:
       loc_vx_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_ONLY;
     break;

     case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
       loc_vx_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
     break;

     case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
       loc_vx_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
     break;

     default:
       loc_vx_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_ONLY;
     break;
   }

   loc_vx_req_ptr->pos_qos_incl = pdapi_vx_req_ptr->pos_qos_incl;
   loc_vx_req_ptr->pos_qos      = pdapi_vx_req_ptr->pos_qos;
   loc_vx_req_ptr->num_fixes    = pdapi_vx_req_ptr->num_fixes;
   loc_vx_req_ptr->tbf          = pdapi_vx_req_ptr->tbf;

   switch(pdapi_vx_req_ptr->pos_mode)
   {
     case PDSM_LCS_MS_ASSISTED_ONLY:
       loc_vx_req_ptr->pos_mode = LOC_VX_MS_ASSISTED_ONLY;
     break;

     case PDSM_LCS_MS_BASED_ONLY:
       loc_vx_req_ptr->pos_mode = LOC_VX_MS_BASED_ONLY;
     break;

     case PDSM_LCS_MS_ASSISTED_PREF_MSBASED_ALLWD:
       loc_vx_req_ptr->pos_mode = LOC_VX_MS_ASSISTED_PREF_MSBASED_ALLWD;
     break;

     case PDSM_LCS_MS_BASED_PREF_ASSISTED_ALLWD:
       loc_vx_req_ptr->pos_mode = LOC_VX_MS_BASED_PREF_ASSISTED_ALLWD;
     break;

     default:
       loc_vx_req_ptr->pos_mode = LOC_VX_MS_ASSISTED_ONLY;
     break;
   }

   loc_vx_req_ptr->encoding_scheme =
      (loc_ni_vx_requester_id_encoding_scheme_e_type) pdapi_vx_req_ptr->encoding_scheme;

   loc_vx_req_ptr->requester_id.requester_id_length =
      pdapi_vx_req_ptr->requester_id.requester_id_length;

   memscpy((void *)&(loc_vx_req_ptr->requester_id.requester_id[0]),
   	  sizeof(loc_vx_req_ptr->requester_id.requester_id),
          (void *)&(pdapi_vx_req_ptr->requester_id.requester_id[0]),
          sizeof(pdapi_vx_req_ptr->requester_id.requester_id));

   loc_vx_req_ptr->user_resp_timer_val =
      pdapi_vx_req_ptr->user_resp_timer_val;
}

static boolean xlate_formattype_from_pdsm_to_locapi (
            pdsm_lcs_supl_format_indicator_e_type pdsm_format,
            unsigned char *loc_format )
{
  boolean b_RetVal = TRUE;
  
  switch ( pdsm_format )
  {
   
   case PDSM_SUPL_FORMAT_LOGICAL_NAME:
     (*loc_format) = LOC_NI_SUPL_FORMAT_LOGICAL_NAME;
     break;
   case PDSM_SUPL_FORMAT_EMAIL_ADDRESS:
     (*loc_format) = LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS;
     break;
   case PDSM_SUPL_FORMAT_MSISDN:
     (*loc_format) = LOC_NI_SUPL_FORMAT_MSISDN;
     break;
   case PDSM_SUPL_FORMAT_URL:
     (*loc_format) = LOC_NI_SUPL_FORMAT_URL;
     break;
   case PDSM_SUPL_FORMAT_SIP_URL:
     (*loc_format) = LOC_NI_SUPL_FORMAT_SIP_URL;
     break;
   case PDSM_SUPL_FORMAT_MIN:
     (*loc_format) = LOC_NI_SUPL_FORMAT_MIN;
     break;
   case PDSM_SUPL_FORMAT_MDN:
     (*loc_format) = LOC_NI_SUPL_FORMAT_MDN;
     break;
   case PDSM_SUPL_FORMAT_IMSPUBLIC_IDENTITY:
     (*loc_format) = LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY;
     break;
   default:
       b_RetVal = FALSE; 
       LOC_MSG_ERROR("loc_ni_xlate_supl_req_from_pdapi_to_locapi invalid format: %d", 
                      pdsm_format, 0, 0);
       break;
  }

  return b_RetVal;
  
}


static boolean xlate_dcstype_from_pdsm_to_locapi (
            pdsm_lcs_notify_verify_datacoding_scheme_e_type pdsm_format,
            unsigned char *loc_format )
{
  boolean b_RetVal = FALSE;
  uint32  ctr = 0;
  

  
  for ( ctr = 0;
        ctr < ( ( sizeof( z_LocNiDcsMap ) ) / 
                          ( sizeof( z_LocNiDcsMap[0] ) ) ) ;
        ctr ++ )
  {
    if ( z_LocNiDcsMap[ctr].e_PdsmDcs ==  pdsm_format)
    { 
      /* Unfortunately LocAPI uses unsigned char, so type cast is required */
      (*loc_format) = (unsigned char ) z_LocNiDcsMap[ctr].e_LocDcs;
      b_RetVal = TRUE;
      break;
    }
  }


  return b_RetVal;
  
}


static boolean copy_requestor_id_pdsm_to_locapi(
      loc_ni_requestor_id_s_type* loc_req_id,
      const pdsm_lcs_requestor_id_s_type* pdsm_req_id)
{
  boolean b_RetVal = TRUE;
   if((NULL == loc_req_id) || (NULL == pdsm_req_id))
   {
       LOC_MSG_ERROR ( "copy_requestor_id_pdsm_to_locapi: invalid parameter", 0, 0, 0);
       b_RetVal = FALSE;
       return b_RetVal;
   }
   else
   {
       const size_t length = pdsm_req_id->string_len;

       b_RetVal =  xlate_formattype_from_pdsm_to_locapi ( 
               (pdsm_lcs_supl_format_indicator_e_type)pdsm_req_id->data_coding_scheme,
                &( loc_req_id->data_coding_scheme ) );
    
       // note: according to implementation in tm, string_len seems to be full size of the array, including any potential trailing zero(s)
       if((length <= sizeof(loc_req_id->requestor_id_string)) && (length <= sizeof(pdsm_req_id->requestor_id_string)))
       {
           loc_req_id->string_len = memscpy(loc_req_id->requestor_id_string, sizeof(loc_req_id->requestor_id_string),
		   	                                pdsm_req_id->requestor_id_string, length);
       }
       else
       {
           // error: the string length is longer than what we can handle. 
           // this shall not happen, because LocAPI should have the same or bigger space than PDAPI
           loc_req_id->string_len = 0;
           LOC_MSG_ERROR ( "copy_requestor_id_pdsm_to_locapi: invalid string length: %d", length, 0, 0);
           b_RetVal = FALSE;
       }
   }

   return b_RetVal;
}

static boolean copy_umts_cp_requestor_id_pdsm_to_locapi(
      loc_ni_requestor_id_s_type* loc_req_id,
      const pdsm_lcs_requestor_id_s_type* pdsm_req_id)
{
   boolean b_RetVal = TRUE;
   if((NULL == loc_req_id) || (NULL == pdsm_req_id))
   {
       LOC_MSG_ERROR ( "copy_umts_cp_requestor_id_pdsm_to_locapi: invalid parameter", 0, 0, 0);
       b_RetVal = FALSE;
       return b_RetVal;
   }
   else
   {
       const size_t length = pdsm_req_id->string_len;

       b_RetVal =  xlate_dcstype_from_pdsm_to_locapi ( 
               (pdsm_lcs_notify_verify_datacoding_scheme_e_type)pdsm_req_id->data_coding_scheme,
                &( loc_req_id->data_coding_scheme ) );
    
       // note: according to implementation in tm, string_len seems to be full size of the array, including any potential trailing zero(s)
       if((length <= sizeof(loc_req_id->requestor_id_string)) && (length <= sizeof(pdsm_req_id->requestor_id_string)))
       {
           loc_req_id->string_len = memscpy(loc_req_id->requestor_id_string, sizeof(loc_req_id->requestor_id_string),
		   	                                pdsm_req_id->requestor_id_string, length);
       }
       else
       {
           // error: the string length is longer than what we can handle. 
           // this shall not happen, because LocAPI should have the same or bigger space than PDAPI
           loc_req_id->string_len = 0;
           LOC_MSG_ERROR ( "copy_umts_cp_requestor_id_pdsm_to_locapi: invalid string length: %d", length, 0, 0);
           b_RetVal = FALSE;
       }
   }

   return b_RetVal;
}


static boolean copy_client_name_pdsm_to_locapi(
      loc_ni_supl_client_name_s_type* loc_client_name,
      const pdsm_lcs_client_name_s_type* pdsm_client_name)
{
    boolean b_RetVal = TRUE;
    
   if((NULL == loc_client_name) || (NULL == pdsm_client_name))
   {
       LOC_MSG_ERROR ( "copy_client_name_pdsm_to_locapi: invalid parameter", 0, 0, 0);
       b_RetVal = FALSE;
       return b_RetVal;
   }
   else
   {
       const size_t length = pdsm_client_name->string_len;
       
       b_RetVal =  xlate_formattype_from_pdsm_to_locapi ( 
               (pdsm_lcs_supl_format_indicator_e_type)pdsm_client_name->data_coding_scheme,
                &( loc_client_name->data_coding_scheme ) );
       // note: according to implementation in tm, string_len seems to be full size of the array, including any potential trailing zero(s)
       if((length <= sizeof(loc_client_name->client_name_string)) && (length <= sizeof(pdsm_client_name->client_name_string)))
       {
           loc_client_name->string_len = memscpy(loc_client_name->client_name_string, sizeof(loc_client_name->client_name_string),
		   	                                     pdsm_client_name->client_name_string, length);
       }
       else
       {
           // error: the string length is longer than what we can handle. 
           // this shall not happen, because LocAPI should have the same or bigger space than PDAPI
           loc_client_name->string_len = 0;
           b_RetVal = FALSE;           
           LOC_MSG_ERROR ( "copy_client_name_pdsm_to_locapi: invalid string length: %d", length, 0, 0);
       }
   }
   return b_RetVal;
}

static void copy_codeword_string_pdsm_to_locapi(
      loc_ni_codeword_string_s_type* loc_codeword,
      const pdsm_lcs_codeword_string_s_type* pdsm_codeword)
{
   (void) xlate_dcstype_from_pdsm_to_locapi(
                 (pdsm_lcs_notify_verify_datacoding_scheme_e_type)pdsm_codeword->data_coding_scheme,
                 &(loc_codeword->data_coding_scheme) );
   loc_codeword->string_len = memscpy(&loc_codeword->lcs_codeword_string, sizeof(loc_codeword->lcs_codeword_string),
   	       &pdsm_codeword->lcs_codeword_string, sizeof(pdsm_codeword->lcs_codeword_string));
}

static void copy_service_type_id_pdsm_to_locapi(
      loc_ni_service_type_id_s_type* loc_service_type_id,
      const pdsm_lcs_service_type_ID_s_type* pdsm_service_type_id)
{
   loc_service_type_id->lcs_service_type_id =
      pdsm_service_type_id->lcs_service_type_id;
}

static void copy_supl_qos_pdsm_to_locapi(
      loc_ni_supl_qop_s_type* loc_supl_qos,
      const pdsm_lcs_supl_qop_s_type* pdsm_supl_qos)
{
   loc_supl_qos->bit_mask = pdsm_supl_qos->bit_mask;
   loc_supl_qos->horacc = pdsm_supl_qos->horacc;
   loc_supl_qos->veracc = pdsm_supl_qos->veracc;
   loc_supl_qos->maxLocAge = pdsm_supl_qos->maxLocAge;
   loc_supl_qos->delay = pdsm_supl_qos->delay;
}

//Translate SUPL 2 extensions from PDAPI to QMI_LOC
static boolean convert_supl_ver_2_ext_from_pdsm( 
  qmiLocNiSuplVer2ExtStructT_v02* loc_supl_ver2_ext_ptr,
  const pdsm_lcs_supl_suplinit_ver2_ext_s_type* pdapi_supl_ver2_ext_ptr)
{
  int idx = 0;
  uint32 size = 0;

  if(NULL == loc_supl_ver2_ext_ptr || NULL == pdapi_supl_ver2_ext_ptr)
  {
    LOC_MSG_ERROR("copy_supl_ver_2_ext_from_pdsm, null input ptrs"
                  "loc_supl_ext = %p pdapi_supl_ext = %p\n", 
                  loc_supl_ver2_ext_ptr, pdapi_supl_ver2_ext_ptr, 0);
    return FALSE;
  }
  
  /* clear all the fields */
  memset(loc_supl_ver2_ext_ptr, 0, sizeof(*loc_supl_ver2_ext_ptr));

  /* if trigger type validity bit is set  then copy the information
     otherwise set the default trigger type of single shot*/
  if(pdapi_supl_ver2_ext_ptr->validity & PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID )
  {
    switch(pdapi_supl_ver2_ext_ptr->trigger_type)
    {
      case PDAPI_SUPLINIT_TRIGGER_TYPE_PERIODIC:
      {
        loc_supl_ver2_ext_ptr->triggerType = 
          eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_PERIODIC_V02;
        break;
      }

      case PDAPI_SUPLINIT_TRIGGER_TYPE_AREA_EVENT:
      {
        loc_supl_ver2_ext_ptr->triggerType = 
          eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_AREA_EVENT_V02;
        break;
      }

      default:
      {
        LOC_MSG_ERROR("copy_supl_ver_2_ext_from_pdsm: Unknown trigger type %d \n",
                      pdapi_supl_ver2_ext_ptr->trigger_type, 0, 0);
        return FALSE;
      }
    }
  }
  else
  {
    loc_supl_ver2_ext_ptr->triggerType = 
      eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_SINGLE_SHOT_V02;
  }
  
  /* convert the gnss type masks */

  size = sizeof(gnss_type_conversion_table) / 
      sizeof(gnss_type_conversion_table[0]);

  /* if the gnss pos tech validity bit is valid,
     copy the mask from PDAPI structure
     else indicate support for GPS only  */

  if(pdapi_supl_ver2_ext_ptr->validity & 
     PDAPI_SUPLINIT_VER2_EXT_BITMAP_GNSSPOSTECH_VALID)
  {
    for( idx = 0; idx < size; idx++ )
    {
      if( pdapi_supl_ver2_ext_ptr->gnss_pos_tech & 
          gnss_type_conversion_table[idx].pdapi_supl_ver2_ext_mask )
      {
        loc_supl_ver2_ext_ptr->gnssType |= 
          gnss_type_conversion_table[idx].qmiLoc_supl_ver2_ext_mask;
      }
    }
  }
  else
  {
    loc_supl_ver2_ext_ptr->gnssType |= 
      QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GPS_V02;
  }


  /* convert the supported network masks */

  size = sizeof(supported_networks_conversion_table) / 
      sizeof(supported_networks_conversion_table[0]);

  /* if the supported networks validity bit is valid,
     copy the mask from PDAPI structure
     else indicate support for the defined networks */

  if(pdapi_supl_ver2_ext_ptr->validity &   
     PDAPI_SUPLINIT_VER2_EXT_BITMAP_SUPTDNTWK_VALID)
  {
    for( idx = 0; idx < size; idx++ )
    {
      if( pdapi_supl_ver2_ext_ptr->supl_supported_ntwkinfo & 
          supported_networks_conversion_table[idx].pdapi_supl_ver2_ext_mask )
      {
        loc_supl_ver2_ext_ptr->supportedNetworksMask |= 
          supported_networks_conversion_table[idx].qmiLoc_supl_ver2_ext_mask;
      }
    }
  }
  else
  {
    loc_supl_ver2_ext_ptr->supportedNetworksMask = 
      QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_ALL;
  }

  return TRUE;
}


//Translate SUPL 2 extensions from QMI_LOC to PDAPI
static boolean convert_supl_ver_2_ext_to_pdsm( 
  pdsm_lcs_supl_suplinit_ver2_ext_s_type* pdapi_supl_ver2_ext_ptr,
  const qmiLocNiSuplVer2ExtStructT_v02* loc_supl_ver2_ext_ptr )
{
  int idx = 0;
  uint32 size = 0;

  if(NULL == loc_supl_ver2_ext_ptr || NULL == pdapi_supl_ver2_ext_ptr)
  {
    LOC_MSG_ERROR("copy_supl_ver_2_ext_to_pdsm, null input ptrs"
                  "loc_supl_ext = %p pdapi_supl_ext = %p\n", 
                  loc_supl_ver2_ext_ptr, pdapi_supl_ver2_ext_ptr, 0);
    return FALSE;
  }
  
  /* clear all the fields */
  memset(pdapi_supl_ver2_ext_ptr, 0, sizeof(*pdapi_supl_ver2_ext_ptr));

  /* convert trigger type to PDSM */
  switch(loc_supl_ver2_ext_ptr->triggerType)
  {
    case eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_PERIODIC_V02:
    {
      pdapi_supl_ver2_ext_ptr->trigger_type = 
        PDAPI_SUPLINIT_TRIGGER_TYPE_PERIODIC;
      pdapi_supl_ver2_ext_ptr->validity |=  
        PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID;
      break;
    }

    case eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_AREA_EVENT_V02:
    {
      pdapi_supl_ver2_ext_ptr->trigger_type = 
        PDAPI_SUPLINIT_TRIGGER_TYPE_AREA_EVENT;
      pdapi_supl_ver2_ext_ptr->validity |=  
        PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID;
      break;
    }

    case eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_SINGLE_SHOT_V02:
    {
      /* pdsm infers single shot if the the valdity bit is not set,
         just log and break */
      LOC_MSG_MED("convert_supl_ver_2_ext_to_pdsm: single shot trigger type",
                  0,0,0);
      break;
    }

    default:
    {
      LOC_MSG_ERROR("copy_supl_ver_2_ext_to_pdsm: Unknown trigger type %d \n",
                    loc_supl_ver2_ext_ptr->triggerType, 0, 0);
      return FALSE;
    }
  }
  
  /* convert the gnss type masks from QMI_LOC to PDAPI */
  size = sizeof(gnss_type_conversion_table) / 
      sizeof(gnss_type_conversion_table[0]);

  /* if the gnss type mask is not set to GPS only then
     set the validity bit for PDAPI pos tech and copy masks.
     PDAPI infers GPS pos tech from the absence of this field */

  if(QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GPS_V02 != 
     loc_supl_ver2_ext_ptr->gnssType)
  {
    pdapi_supl_ver2_ext_ptr->validity |= 
      PDAPI_SUPLINIT_VER2_EXT_BITMAP_GNSSPOSTECH_VALID;
    for( idx = 0; idx < size; idx++ )
    {
      if( loc_supl_ver2_ext_ptr->gnssType & 
          gnss_type_conversion_table[idx].qmiLoc_supl_ver2_ext_mask)
      {
        pdapi_supl_ver2_ext_ptr->gnss_pos_tech |= 
          gnss_type_conversion_table[idx].pdapi_supl_ver2_ext_mask ;
      }
    }
  }

  /* convert the supported network masks */

  size = sizeof(supported_networks_conversion_table) / 
      sizeof(supported_networks_conversion_table[0]);

  /* if the supported networks mask is set to ALL networks,
     do not set the validity bit for PDAPI supported networks,
     else convert the masks to PDAPI */

  if(QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_ALL != 
     loc_supl_ver2_ext_ptr->supportedNetworksMask)
  {
    pdapi_supl_ver2_ext_ptr->validity |= 
      PDAPI_SUPLINIT_VER2_EXT_BITMAP_SUPTDNTWK_VALID;
    for( idx = 0; idx < size; idx++ )
    {
      if(loc_supl_ver2_ext_ptr->supportedNetworksMask  & 
          supported_networks_conversion_table[idx].qmiLoc_supl_ver2_ext_mask )
      {
         pdapi_supl_ver2_ext_ptr->supl_supported_ntwkinfo |= 
           supported_networks_conversion_table[idx].pdapi_supl_ver2_ext_mask ;
      }
    }

  }
  return TRUE;
}

// Translate SUPL NI request from PDAPI to LOC API
static boolean  loc_ni_xlate_supl_req_from_pdapi_to_locapi
(
      const pdsm_lcs_supl_notify_verify_req_s_type*    pdapi_supl_req_ptr,
      loc_ni_supl_notify_verify_req_s_type*            loc_supl_req_ptr
)
{

  boolean b_RetVal = TRUE;

   switch(pdapi_supl_req_ptr->notification_priv_type)
   {
     case PDSM_LCS_NO_NOTIFY_NO_VERIFY:
       loc_supl_req_ptr->notification_priv_type = LOC_NI_USER_NO_NOTIFY_NO_VERIFY;
     break;

     case PDSM_LCS_USER_NOTIFY_ONLY:
       loc_supl_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_ONLY;
     break;

     case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
       loc_supl_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
     break;

     case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
       loc_supl_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
     break;

     case PDSM_LCS_PRIVACY_OVERRIDE:
       loc_supl_req_ptr->notification_priv_type = LOC_NI_USER_PRIVACY_OVERRIDE;
     break;

     default:
       loc_supl_req_ptr->notification_priv_type = LOC_NI_USER_NO_NOTIFY_NO_VERIFY;
     break;
   }


   loc_supl_req_ptr->flags = pdapi_supl_req_ptr->flags;

   loc_supl_req_ptr->supl_slp_session_id.presence = pdapi_supl_req_ptr->supl_slp_session_id.presence;

   memscpy ((void *)&(loc_supl_req_ptr->supl_slp_session_id.session_id[0]),
   	       sizeof(loc_supl_req_ptr->supl_slp_session_id.session_id),
           (void *)&(pdapi_supl_req_ptr->supl_slp_session_id.session_id[0]),
           sizeof(pdapi_supl_req_ptr->supl_slp_session_id.session_id));

   if(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_IPV4_ADDRESS)
   {
      loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_type = LOC_SERVER_ADDR_IPV4;
      loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv4.addr =
         pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs;

      loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv4.port =
         (uint16) pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id;
   }
   else if(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_URL_ADDRESS)
   {
      loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_type = LOC_SERVER_ADDR_URL;

      if((pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length <= 
          ((sizeof(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url)))) 
         &&
         (sizeof(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.addr) >= 
          sizeof(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url)))
      {
          loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.length =
             pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length;

          // note: it's not clear if url_length can be safely used as indicator of how many bytes should be copied
          memscpy(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.addr,
                 sizeof(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.addr),
                 pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url,
                 sizeof(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url));
      }
      else
      {
          loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.length = 0;

          LOC_MSG_ERROR("loc_ni_xlate_supl_req_from_pdapi_to_locapi invalid length: %d", 
                        pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length, 0, 0);
          b_RetVal = FALSE;
      }
   }
   else if(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_IPV6_ADDRESS)
   {
      loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_type = LOC_SERVER_ADDR_IPV6;

      memscpy( loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv6.addr,
	  	sizeof(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv6.addr),
        pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs, 
        sizeof(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs));

      loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv6.port =
         pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id;
   }

   memscpy((void *)&(loc_supl_req_ptr->supl_hash[0]), sizeof(loc_supl_req_ptr->supl_hash),
           (void *)&(pdapi_supl_req_ptr->supl_hash[0]), sizeof(pdapi_supl_req_ptr->supl_hash));

   loc_supl_req_ptr->datacoding_scheme = (loc_ni_datacoding_scheme_e_type)pdapi_supl_req_ptr->datacoding_scheme;

    switch ( pdapi_supl_req_ptr->pos_method )
   {
     case POSMETHOD_AGPS_SETASSISTED:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_AGPS_SETASSISTED;
       break;
       
     case POSMETHOD_AGPS_SETBASED:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_AGPS_SETBASED;
       break;
       
     case POSMETHOD_AGPS_SETASSISTED_PREF:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_AGPS_SETASSISTED_PREF;
       break;
       
     case POSMETHOD_AGPS_SETBASED_PREF:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_AGPS_SETBASED_PREF;
       break;
       
     case POSMETHOD_AUTONOMOUS_GPS:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_AUTONOMOUS_GPS;
       break;
       
     case POSMETHOD_AFLT:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_AFLT;
       break;
       
     case POSMETHOD_ECID:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_ECID;
       break;
       
     case POSMETHOD_EOTD:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_EOTD;
       break;
       
     case POSMETHOD_OTDOA:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_OTDOA;
       break;
   
     case POSMETHOD_NO_POSITION:
       loc_supl_req_ptr->pos_method = LOC_NI_POSMETHOD_NO_POSITION;
       break;
   
     default:
       b_RetVal = FALSE;

       LOC_MSG_ERROR("loc_ni_xlate_supl_req_from_pdapi_to_locapi invalid pos method: %d", 
                      pdapi_supl_req_ptr->pos_method, 0, 0);
       break;
   }


   LOC_MSG_MED("requestor ID len=%d", pdapi_supl_req_ptr->requestor_id.string_len, 0, 0);
   copy_requestor_id_pdsm_to_locapi(&loc_supl_req_ptr->requestor_id, &pdapi_supl_req_ptr->requestor_id);

   LOC_MSG_MED("client_name len=%d", pdapi_supl_req_ptr->client_name.string_len, 0, 0);
   copy_client_name_pdsm_to_locapi(&loc_supl_req_ptr->client_name, &pdapi_supl_req_ptr->client_name);

   copy_supl_qos_pdsm_to_locapi(&loc_supl_req_ptr->supl_qop, &pdapi_supl_req_ptr->supl_qop);

   loc_supl_req_ptr->user_response_timer = pdapi_supl_req_ptr->user_response_timer;

   /* set the valid flag to FALSE in case this field is not memset by the caller
      will be set to TRUE during conversion from PDAPI to LOC */
   loc_supl_req_ptr->supl_ver2_ext_valid = FALSE;

   if(pdapi_supl_req_ptr->flags & PDSM_LCS_SUPLINIT_VER2_EXT_PRESENT)
   {
     /* copy SUPL 2 extension from PDAPI to QMI_LOC */
     if( TRUE == 
          convert_supl_ver_2_ext_from_pdsm(&(loc_supl_req_ptr->supl_ver2_ext), 
                                        &(pdapi_supl_req_ptr->supl_init_ver2_ext)) )
     {
       loc_supl_req_ptr->supl_ver2_ext_valid = TRUE;
       LOC_MSG_MED("loc_ni_xlate_supl_req_from_pdapi_to_locapi, "
                   "supl 2 ext PDAPI input validitity = %u \n", 
                    pdapi_supl_req_ptr->supl_init_ver2_ext.validity, 
                    0,0 );
       LOC_MSG_MED("loc_ni_xlate_supl_req_from_pdapi_to_locapi, "
                   "supl 2 ext PDAPI input gnss pos tech = %u, supported networks =%u "
                    " trigger type =%d \n", pdapi_supl_req_ptr->supl_init_ver2_ext.gnss_pos_tech,
                    pdapi_supl_req_ptr->supl_init_ver2_ext.supl_supported_ntwkinfo,
                    pdapi_supl_req_ptr->supl_init_ver2_ext.trigger_type );
       LOC_MSG_MED("loc_ni_xlate_supl_req_from_pdapi_to_locapi, "
                   "supl 2 ext QMI_LOC output gnss type = %u, supported networks =%u "
                    " trigger type =%d \n", loc_supl_req_ptr->supl_ver2_ext.gnssType,
                    loc_supl_req_ptr->supl_ver2_ext.supportedNetworksMask,
                    loc_supl_req_ptr->supl_ver2_ext.triggerType);
     }
   }


   /* initialize supl_emergency information */
   loc_supl_req_ptr->supl_emergency_valid = FALSE;
   loc_supl_req_ptr->supl_emergency_notification.eslpUrl[0]='\0';

   /* copy the Emergency information */
   if(pdapi_supl_req_ptr->flags & PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT)
   {
     loc_supl_req_ptr->supl_emergency_valid = TRUE;

     if (TRUE == pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.b_eslp_address_present)
     {
       /* assuming that PDAPI always sends a URL type for ESLP and that the
         slp address is always null terminated */
       (void)GNSS_STRLCPY(
          loc_supl_req_ptr->supl_emergency_notification.eslpUrl,
          pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url,
          sizeof(loc_supl_req_ptr->supl_emergency_notification.eslpUrl));
     }
   }

   return b_RetVal;
}


// Translate UMTS NI request from PDAPI to LOC API
static void loc_ni_xlate_umts_req_from_pdapi_to_locapi
(
      const pdsm_lcs_umts_cp_notify_verify_req_s_type*  pdapi_umts_req_ptr,
      loc_ni_umts_cp_notify_verify_req_s_type*          loc_umts_req_ptr
)
{
   switch(pdapi_umts_req_ptr->notification_priv_type)
   {
     case PDSM_LCS_USER_NOTIFY_ONLY:
       loc_umts_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_ONLY;
     break;

     case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
       loc_umts_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
     break;

     case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
       loc_umts_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
     break;

     default:
       loc_umts_req_ptr->notification_priv_type = LOC_NI_USER_NOTIFY_ONLY;
     break;
   }

   loc_umts_req_ptr->invoke_id
     = pdapi_umts_req_ptr->invoke_id;

   loc_umts_req_ptr->flags
     = pdapi_umts_req_ptr->flags;

   loc_umts_req_ptr->notification_length
     = pdapi_umts_req_ptr->notification_length;

   memset (loc_umts_req_ptr->notification_text,
           0,
           sizeof (loc_umts_req_ptr->notification_text));

   if( (pdapi_umts_req_ptr->flags & PDSM_LCS_CLIENT_NAME_PRESENT) &&
       (pdapi_umts_req_ptr->notification_text != NULL)
     )
   {
      memscpy ((void*)&(loc_umts_req_ptr->notification_text[0]),
	  	      sizeof(loc_umts_req_ptr->notification_text),
              (void*)(pdapi_umts_req_ptr->notification_text),
              pdapi_umts_req_ptr->notification_length);
   }

   loc_umts_req_ptr->datacoding_scheme
     = (loc_ni_datacoding_scheme_e_type)pdapi_umts_req_ptr->datacoding_scheme;

   loc_umts_req_ptr->ext_client_address_data.ext_client_address_len =
      pdapi_umts_req_ptr->ext_client_address_data.ext_client_address_len;

   memscpy(loc_umts_req_ptr->ext_client_address_data.ext_client_address,
          sizeof loc_umts_req_ptr->ext_client_address_data.ext_client_address,
          pdapi_umts_req_ptr->ext_client_address_data.ext_client_address,
          sizeof pdapi_umts_req_ptr->ext_client_address_data.ext_client_address);

   switch ( pdapi_umts_req_ptr->location_type)
   {
    case PDSM_LOCATIONTYPE_CURRENT_LOCATION:
      loc_umts_req_ptr->location_type =   LOC_NI_LOCATIONTYPE_CURRENT_LOCATION;      
      break;
    case PDSM_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION:
      loc_umts_req_ptr->location_type = LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION;
      break;
    case PDSM_LOCATIONTYPE_INITIAL_LOCATION:
      loc_umts_req_ptr->location_type = LOC_NI_LOCATIONTYPE_INITIAL_LOCATION;
      break;

    default:      
      LOC_MSG_ERROR ( "xlate_umts_req_from_pdapi_to_locapi could not translate Location %d", 
                      pdapi_umts_req_ptr->location_type, 0, 0);
      break;
   }


   if (sizeof loc_umts_req_ptr->deferred_location ==
      sizeof pdapi_umts_req_ptr->deferred_location)
   {
   memscpy((void *)&(loc_umts_req_ptr->deferred_location),
          sizeof(loc_umts_req_ptr->deferred_location),
          (void *)&(pdapi_umts_req_ptr->deferred_location),
          sizeof(pdapi_umts_req_ptr->deferred_location));
   } else {
      LOC_MSG_ERROR("loc_ni_xlate_umts_req_from_pdapi_to_locapi, PDAPI and Loc API mismatch in deferred_location",0,0,0);
   }

   LOC_MSG_MED("requestor ID len=%d", pdapi_umts_req_ptr->requestor_id.string_len, 0, 0);
   copy_umts_cp_requestor_id_pdsm_to_locapi(&loc_umts_req_ptr->requestor_id, &pdapi_umts_req_ptr->requestor_id);

   LOC_MSG_MED("codeword_string len=%d", pdapi_umts_req_ptr->codeword_string.string_len, 0, 0);
   copy_codeword_string_pdsm_to_locapi(&loc_umts_req_ptr->codeword_string, &pdapi_umts_req_ptr->codeword_string);

   copy_service_type_id_pdsm_to_locapi(&loc_umts_req_ptr->service_type_id, &pdapi_umts_req_ptr->service_type_id);

   loc_umts_req_ptr->user_response_timer =
      pdapi_umts_req_ptr->user_response_timer;
}

// Translate V1/V2 NI request from Loc API to PD API
static void  loc_ni_xlate_vx_req_from_locapi_to_pdapi
(
      const loc_ni_vx_notify_verify_req_s_type*  loc_vx_req_ptr,
      pdsm_lcs_vx_notify_verify_req_s_type*      pdapi_vx_req_ptr
)
{
   switch(loc_vx_req_ptr->notification_priv_type)
   {
      case LOC_NI_USER_NOTIFY_ONLY:
         pdapi_vx_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_ONLY;
      break;

      case LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
         pdapi_vx_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
      break;

      case LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP: 
         pdapi_vx_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
      break;

      default:
         pdapi_vx_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_ONLY;
      break;
   }

   pdapi_vx_req_ptr->pos_qos_incl = loc_vx_req_ptr->pos_qos_incl;
   pdapi_vx_req_ptr->pos_qos      = loc_vx_req_ptr->pos_qos;
   pdapi_vx_req_ptr->num_fixes    = loc_vx_req_ptr->num_fixes;
   pdapi_vx_req_ptr->tbf          = loc_vx_req_ptr->tbf;

   switch(loc_vx_req_ptr->pos_mode)
   {
      case LOC_VX_MS_ASSISTED_ONLY:
       pdapi_vx_req_ptr->pos_mode = PDSM_LCS_MS_ASSISTED_ONLY;
      break;

     case LOC_VX_MS_BASED_ONLY:
       pdapi_vx_req_ptr->pos_mode = PDSM_LCS_MS_BASED_ONLY;
     break;

     case LOC_VX_MS_ASSISTED_PREF_MSBASED_ALLWD:
       pdapi_vx_req_ptr->pos_mode = PDSM_LCS_MS_ASSISTED_PREF_MSBASED_ALLWD;
     break;

     case LOC_VX_MS_BASED_PREF_ASSISTED_ALLWD:
       pdapi_vx_req_ptr->pos_mode = PDSM_LCS_MS_BASED_PREF_ASSISTED_ALLWD;
     break;

     default:
       pdapi_vx_req_ptr->pos_mode = PDSM_LCS_MS_ASSISTED_ONLY;
     break;
   }

   pdapi_vx_req_ptr->encoding_scheme = 
      (pdsm_lcs_requester_id_encoding_scheme_e_type) loc_vx_req_ptr->encoding_scheme;

   pdapi_vx_req_ptr->requester_id.requester_id_length = 
      loc_vx_req_ptr->requester_id.requester_id_length;

   memscpy((void *)&(pdapi_vx_req_ptr->requester_id.requester_id[0]),
           sizeof (pdapi_vx_req_ptr->requester_id.requester_id),
          (void *)&(loc_vx_req_ptr->requester_id.requester_id[0]),
           sizeof(loc_vx_req_ptr->requester_id.requester_id));

   pdapi_vx_req_ptr->user_resp_timer_val = loc_vx_req_ptr->user_resp_timer_val;
}

static boolean xlate_formattype_from_locapi_to_pdapi (
            unsigned char loc_format ,
            pdsm_lcs_supl_format_indicator_e_type *pdsm_format)
{
  boolean b_RetVal = TRUE;
  
  switch ( loc_format )
  {
   
   case LOC_NI_SUPL_FORMAT_LOGICAL_NAME:
     (*pdsm_format) = PDSM_SUPL_FORMAT_LOGICAL_NAME;
     break;
   case LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS:
     (*pdsm_format) = PDSM_SUPL_FORMAT_EMAIL_ADDRESS;
     break;
   case LOC_NI_SUPL_FORMAT_MSISDN:
     (*pdsm_format) = PDSM_SUPL_FORMAT_MSISDN;
     break;
   case LOC_NI_SUPL_FORMAT_URL:
     (*pdsm_format) = PDSM_SUPL_FORMAT_URL;
     break;
   case LOC_NI_SUPL_FORMAT_SIP_URL:
     (*pdsm_format) = PDSM_SUPL_FORMAT_SIP_URL;
     break;
   case LOC_NI_SUPL_FORMAT_MIN:
     (*pdsm_format) = PDSM_SUPL_FORMAT_MIN;
     break;
   case LOC_NI_SUPL_FORMAT_MDN:
     (*pdsm_format) = PDSM_SUPL_FORMAT_MDN;
     break;
   case LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY:
     (*pdsm_format) = PDSM_SUPL_FORMAT_IMSPUBLIC_IDENTITY;
     break;
   default:
       b_RetVal = FALSE; 
       LOC_MSG_ERROR("loc_ni_xlate_supl_req_from_locapi to pdapi invalid format: %d", 
                      loc_format, 0, 0);
       break;
  }

  return b_RetVal;
  
}

static boolean xlate_dcstype_from_locapi_to_pdapi (
            unsigned char loc_format ,
            pdsm_lcs_notify_verify_datacoding_scheme_e_type *pdsm_format)
{
  boolean b_RetVal = FALSE;
  uint32  ctr = 0;
  

  
  for ( ctr = 0;
        ctr < ( ( sizeof( z_LocNiDcsMap ) ) / 
                          ( sizeof( z_LocNiDcsMap[0] ) ) ) ;
        ctr ++ )
  {
    /* Type cast is required, as LocAPI uses unsigned char */
    if ( z_LocNiDcsMap[ctr].e_LocDcs == 
                    (loc_ni_datacoding_scheme_e_type) loc_format )
    {
      (*pdsm_format) = z_LocNiDcsMap[ctr].e_PdsmDcs;
      b_RetVal = TRUE;
      break;
    }
  }


  return b_RetVal;
  
}



// Sub-routines for xlate fields from loc api to pdapi
static boolean loc_ni_xlate_requestor_id_from_locapi_to_pdapi
(
      const loc_ni_requestor_id_s_type* loc_req_id,
      pdsm_lcs_requestor_id_s_type*     pdsm_req_id
)
{
  pdsm_lcs_supl_format_indicator_e_type e_PdsmFormat = PDSM_SUPL_FORMAT_OSS_UNKNOWN;
  boolean b_Retval = TRUE;
   
   b_Retval =  xlate_formattype_from_locapi_to_pdapi
     ( loc_req_id->data_coding_scheme,
       &e_PdsmFormat );
   pdsm_req_id->data_coding_scheme = (byte)e_PdsmFormat;
   
   pdsm_req_id->string_len = memscpy(&pdsm_req_id->requestor_id_string,
          sizeof (pdsm_req_id->requestor_id_string),
          &loc_req_id->requestor_id_string, loc_req_id->string_len);

   return b_Retval;
}

static boolean loc_ni_xlate_umts_cp_requestor_id_from_locapi_to_pdapi
(
      const loc_ni_requestor_id_s_type* loc_req_id,
      pdsm_lcs_requestor_id_s_type*     pdsm_req_id
)
{
  pdsm_lcs_notify_verify_datacoding_scheme_e_type e_PdsmDcs = {0};
  boolean b_Retval = TRUE;
   
   b_Retval =  xlate_dcstype_from_locapi_to_pdapi
     ( loc_req_id->data_coding_scheme,
       &e_PdsmDcs );
   pdsm_req_id->data_coding_scheme = (byte)e_PdsmDcs;
   
   pdsm_req_id->string_len = memscpy(&pdsm_req_id->requestor_id_string,
          sizeof (pdsm_req_id->requestor_id_string),
          &loc_req_id->requestor_id_string, loc_req_id->string_len);

   return b_Retval;
}


static boolean loc_ni_xlate_client_name_from_locapi_to_pdapi
( 
      const loc_ni_supl_client_name_s_type* loc_client_name,
      pdsm_lcs_client_name_s_type* pdsm_client_name
)
{
  pdsm_lcs_supl_format_indicator_e_type e_PdsmFormat = PDSM_SUPL_FORMAT_OSS_UNKNOWN;
  boolean b_Retval = TRUE;
   
  b_Retval =  xlate_formattype_from_locapi_to_pdapi
     ( loc_client_name->data_coding_scheme,
       &e_PdsmFormat );

   pdsm_client_name->data_coding_scheme = (byte)e_PdsmFormat;
   pdsm_client_name->string_len = memscpy (&pdsm_client_name->client_name_string,
           sizeof (pdsm_client_name->client_name_string),
           &loc_client_name->client_name_string, loc_client_name->string_len);
   return b_Retval;

}

static void loc_ni_xlate_codeword_string_from_locapi_to_pdapi
(
      const loc_ni_codeword_string_s_type* loc_codeword,
      pdsm_lcs_codeword_string_s_type* pdsm_codeword
)
{
  pdsm_lcs_notify_verify_datacoding_scheme_e_type e_PdsmDcs ={0};
  (void)  xlate_dcstype_from_locapi_to_pdapi
     ( loc_codeword->data_coding_scheme,
       &e_PdsmDcs );

   pdsm_codeword->data_coding_scheme = e_PdsmDcs;
   pdsm_codeword->string_len = memscpy(&pdsm_codeword->lcs_codeword_string,
          sizeof (pdsm_codeword->lcs_codeword_string),
          &loc_codeword->lcs_codeword_string, loc_codeword->string_len);
}

static void loc_ni_xlate_supl_qos_from_locapi_to_pdapi
(
      const loc_ni_supl_qop_s_type* loc_supl_qos,
      pdsm_lcs_supl_qop_s_type* pdsm_supl_qos
)
{
   pdsm_supl_qos->bit_mask  = loc_supl_qos->bit_mask;
   pdsm_supl_qos->horacc    = loc_supl_qos->horacc;
   pdsm_supl_qos->veracc    = loc_supl_qos->veracc;
   pdsm_supl_qos->maxLocAge = loc_supl_qos->maxLocAge;
   pdsm_supl_qos->delay     = loc_supl_qos->delay;
}

// Translate SUPL NI request from LOC API to PD API
static boolean  loc_ni_xlate_supl_req_from_locapi_to_pdapi
(
      const loc_ni_supl_notify_verify_req_s_type* loc_supl_req_ptr,
      pdsm_lcs_supl_notify_verify_req_s_type*     pdapi_supl_req_ptr
)
{

   boolean b_RetVal = TRUE;
   
   switch (loc_supl_req_ptr->notification_priv_type)
   {
     case LOC_NI_USER_NO_NOTIFY_NO_VERIFY :
        pdapi_supl_req_ptr->notification_priv_type = PDSM_LCS_NO_NOTIFY_NO_VERIFY;
     break;

     case LOC_NI_USER_NOTIFY_ONLY:
       pdapi_supl_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_ONLY;
     break;

     case LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP :
       pdapi_supl_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
     break;

     case LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP :
       pdapi_supl_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
     break;

     case LOC_NI_USER_PRIVACY_OVERRIDE :
       pdapi_supl_req_ptr->notification_priv_type = PDSM_LCS_PRIVACY_OVERRIDE;
     break;

     default:
       pdapi_supl_req_ptr->notification_priv_type = PDSM_LCS_NO_NOTIFY_NO_VERIFY;
     break;
   }

   pdapi_supl_req_ptr->flags = loc_supl_req_ptr->flags;

   pdapi_supl_req_ptr->supl_slp_session_id.presence = loc_supl_req_ptr->supl_slp_session_id.presence;

   memscpy ((void *)&(pdapi_supl_req_ptr->supl_slp_session_id.session_id[0]),
   	        sizeof (pdapi_supl_req_ptr->supl_slp_session_id.session_id),
           (void *)&(loc_supl_req_ptr->supl_slp_session_id.session_id[0]),
            sizeof (loc_supl_req_ptr->supl_slp_session_id.session_id));


   if (loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_type == LOC_SERVER_ADDR_IPV4)
   {
      pdapi_supl_req_ptr->supl_slp_session_id.slp_address.choice = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs =
          loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv4.addr;

      pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id =
          loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv4.port;
   }
   else if (loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_type == LOC_SERVER_ADDR_URL)
   {
      pdapi_supl_req_ptr->supl_slp_session_id.slp_address.choice = PDSM_SERVER_TYPE_URL_ADDRESS;
      pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length =
          loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.length;

      memscpy((void *)&(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url[0]),
             sizeof (pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url),
             (void *)&(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.addr[0]),
             sizeof(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.url.addr));
   }
   else if(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_type == LOC_SERVER_ADDR_IPV6)
   {
      pdapi_supl_req_ptr->supl_slp_session_id.slp_address.choice = PDSM_SERVER_TYPE_IPV6_ADDRESS;
      pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id =
          loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv6.port;

      memscpy((void *)&(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs),
	  	     sizeof(pdapi_supl_req_ptr->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs),
             (void *)&(loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv6.addr),
             sizeof (loc_supl_req_ptr->supl_slp_session_id.slp_address.addr_info.ipv6.addr));
   }


   memscpy((void *)&(pdapi_supl_req_ptr->supl_hash[0]), sizeof (pdapi_supl_req_ptr->supl_hash),
           (void *)&(loc_supl_req_ptr->supl_hash[0]), sizeof (loc_supl_req_ptr->supl_hash));

   pdapi_supl_req_ptr->datacoding_scheme = (pdsm_lcs_notify_verify_datacoding_scheme_e_type) loc_supl_req_ptr->datacoding_scheme;

   switch ( loc_supl_req_ptr->pos_method )
  {
    case LOC_NI_POSMETHOD_AGPS_SETASSISTED:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_AGPS_SETASSISTED;
      break;
      
    case LOC_NI_POSMETHOD_AGPS_SETBASED:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_AGPS_SETBASED;
      break;
      
    case LOC_NI_POSMETHOD_AGPS_SETASSISTED_PREF:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_AGPS_SETASSISTED_PREF;
      break;
      
    case LOC_NI_POSMETHOD_AGPS_SETBASED_PREF:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_AGPS_SETBASED_PREF;
      break;
      
    case LOC_NI_POSMETHOD_AUTONOMOUS_GPS:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_AUTONOMOUS_GPS;
      break;
      
    case LOC_NI_POSMETHOD_AFLT:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_AFLT;
      break;
      
    case LOC_NI_POSMETHOD_ECID:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_ECID;
      break;
      
    case LOC_NI_POSMETHOD_EOTD:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_EOTD;
      break;
      
    case LOC_NI_POSMETHOD_OTDOA:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_OTDOA;
      break;

    case LOC_NI_POSMETHOD_NO_POSITION:
      pdapi_supl_req_ptr->pos_method = POSMETHOD_NO_POSITION;
      break;

    default:
      b_RetVal = FALSE;
      break;
  }

   b_RetVal = loc_ni_xlate_requestor_id_from_locapi_to_pdapi (&loc_supl_req_ptr->requestor_id, &pdapi_supl_req_ptr->requestor_id);
   LOC_MSG_MED("requestor ID len=%d", pdapi_supl_req_ptr->requestor_id.string_len, 0, 0);

   if ( TRUE == b_RetVal ) 
  {
     b_RetVal = loc_ni_xlate_client_name_from_locapi_to_pdapi (&loc_supl_req_ptr->client_name, &pdapi_supl_req_ptr->client_name);
     LOC_MSG_MED("client_name len=%d", pdapi_supl_req_ptr->client_name.string_len, 0, 0);
  }
   
   loc_ni_xlate_supl_qos_from_locapi_to_pdapi (&loc_supl_req_ptr->supl_qop, &pdapi_supl_req_ptr->supl_qop);

   pdapi_supl_req_ptr->user_response_timer = loc_supl_req_ptr->user_response_timer;

   if(TRUE == loc_supl_req_ptr->supl_ver2_ext_valid)
   {
     pdapi_supl_req_ptr->flags |= PDSM_LCS_SUPLINIT_VER2_EXT_PRESENT;
     b_RetVal = convert_supl_ver_2_ext_to_pdsm(&(pdapi_supl_req_ptr->supl_init_ver2_ext),
                                               &(loc_supl_req_ptr->supl_ver2_ext) );

     LOC_MSG_MED("converted supl 2 ext, input QMI LOC : gnss type = %u,"
                  "supported networks = %u, trigger type = %d\n", 
                  loc_supl_req_ptr->supl_ver2_ext.gnssType, loc_supl_req_ptr->supl_ver2_ext.supportedNetworksMask,
                  loc_supl_req_ptr->supl_ver2_ext.triggerType);

     LOC_MSG_MED("converted supl 2 ext, output pdsm : validity = %u\n",
                  pdapi_supl_req_ptr->supl_init_ver2_ext.validity, 0, 0);

     LOC_MSG_MED("converted supl 2 ext, output pdsm : pos tech = %u supported networks = %u"
                  " trigger type = %d",  pdapi_supl_req_ptr->supl_init_ver2_ext.gnss_pos_tech,
                  pdapi_supl_req_ptr->supl_init_ver2_ext.supl_supported_ntwkinfo,
                  pdapi_supl_req_ptr->supl_init_ver2_ext.trigger_type );

   }

   if (TRUE == loc_supl_req_ptr->supl_emergency_valid)
   {
     pdapi_supl_req_ptr->flags |= PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT;
     pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.b_eslp_address_present = FALSE;
     if ('\0' != loc_supl_req_ptr->supl_emergency_notification.eslpUrl[0])
     {
       pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.b_eslp_address_present = TRUE;

       pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.e_slp_address.choice =
         PDSM_SERVER_TYPE_URL_ADDRESS;

       pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url_length =
         GNSS_STRLEN(loc_supl_req_ptr->supl_emergency_notification.eslpUrl);
       (void)GNSS_STRLCPY(
          pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url,
          loc_supl_req_ptr->supl_emergency_notification.eslpUrl,
          sizeof(
             pdapi_supl_req_ptr->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url));

     }
   }

   return b_RetVal;
}


// Translate UMTS NI request from LOC API to PD API
static void loc_ni_xlate_umts_req_from_locapi_to_pdapi
(
      const loc_ni_umts_cp_notify_verify_req_s_type*  loc_umts_req_ptr,
      pdsm_lcs_umts_cp_notify_verify_req_s_type*      pdapi_umts_req_ptr
)
{
   switch(loc_umts_req_ptr->notification_priv_type)
   {
     case LOC_NI_USER_NOTIFY_ONLY:
       pdapi_umts_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_ONLY;
     break;

     case LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
       pdapi_umts_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
     break;

     case LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
       pdapi_umts_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
     break;

     default:
       pdapi_umts_req_ptr->notification_priv_type = PDSM_LCS_USER_NOTIFY_ONLY;
     break;
   }

   pdapi_umts_req_ptr->invoke_id = loc_umts_req_ptr->invoke_id;

   pdapi_umts_req_ptr->flags = loc_umts_req_ptr->flags;

   memset (umts_cp_notification_text, 
           0, 
           sizeof (umts_cp_notification_text));

   memscpy (umts_cp_notification_text, sizeof (umts_cp_notification_text),
           loc_umts_req_ptr->notification_text, sizeof (loc_umts_req_ptr->notification_text));
           
   pdapi_umts_req_ptr->notification_length = loc_umts_req_ptr->notification_length;
   if (pdapi_umts_req_ptr->notification_length > sizeof (umts_cp_notification_text)) 
   {
      pdapi_umts_req_ptr->notification_length = sizeof (umts_cp_notification_text);
   }
   pdapi_umts_req_ptr->notification_text = umts_cp_notification_text;

   pdapi_umts_req_ptr->datacoding_scheme = (pdsm_lcs_notify_verify_datacoding_scheme_e_type) loc_umts_req_ptr->datacoding_scheme; 
   pdapi_umts_req_ptr->ext_client_address_data.ext_client_address_len =
       loc_umts_req_ptr->ext_client_address_data.ext_client_address_len;

   memscpy(pdapi_umts_req_ptr->ext_client_address_data.ext_client_address,
          sizeof (pdapi_umts_req_ptr->ext_client_address_data.ext_client_address),
          loc_umts_req_ptr->ext_client_address_data.ext_client_address,
          sizeof(loc_umts_req_ptr->ext_client_address_data.ext_client_address));

   switch ( loc_umts_req_ptr->location_type)
   {
    case LOC_NI_LOCATIONTYPE_CURRENT_LOCATION:
      pdapi_umts_req_ptr->location_type =   PDSM_LOCATIONTYPE_CURRENT_LOCATION;      
      break;
    case LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION:
      pdapi_umts_req_ptr->location_type = PDSM_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION;
      break;
    case LOC_NI_LOCATIONTYPE_INITIAL_LOCATION:
      pdapi_umts_req_ptr->location_type = PDSM_LOCATIONTYPE_INITIAL_LOCATION;
      break;

    default:      
      LOC_MSG_ERROR ( "xlate_umts_req_from_locapi_to_pdapi could not translate Location %d", 
                      loc_umts_req_ptr->location_type, 0, 0);
      break;
   }

   pdapi_umts_req_ptr->deferred_location.ms_available = loc_umts_req_ptr->deferred_location.ms_available;
   pdapi_umts_req_ptr->deferred_location.unused_bits = loc_umts_req_ptr->deferred_location.unused_bits;

   loc_ni_xlate_umts_cp_requestor_id_from_locapi_to_pdapi (&loc_umts_req_ptr->requestor_id, &pdapi_umts_req_ptr->requestor_id);
   LOC_MSG_MED("requestor ID len=%d", pdapi_umts_req_ptr->requestor_id.string_len, 0, 0);

   loc_ni_xlate_codeword_string_from_locapi_to_pdapi (&loc_umts_req_ptr->codeword_string, &pdapi_umts_req_ptr->codeword_string);
   LOC_MSG_MED("codeword_string len=%d", pdapi_umts_req_ptr->codeword_string.string_len, 0, 0);

   pdapi_umts_req_ptr->service_type_id.lcs_service_type_id = loc_umts_req_ptr->service_type_id.lcs_service_type_id;

   pdapi_umts_req_ptr->user_response_timer = loc_umts_req_ptr->user_response_timer;
}



static void loc_ni_xlate_vx_version_from_locapi_to_pdapi
(
  qmiLocVxVersionEnumT_v02  loc_vx_version,
  pdsm_lcs_agent_set_vx_config_s_type* pdapi_vx_version
)
{
  if ( NULL != pdapi_vx_version )
  {
    switch ( loc_vx_version )
    {
      case eQMI_LOC_VX_VERSION_V1_ONLY_V02:
          pdapi_vx_version->vxVersion= PDSM_LCS_AGENT_VERSION_V1_ONLY;
        break;
      
      case eQMI_LOC_VX_VERSION_V2_ONLY_V02:
        pdapi_vx_version->vxVersion= PDSM_LCS_AGENT_VERSION_V2_ONLY;
        break;
      default:
        LOC_MSG_ERROR ( "loc_ni_xlate_vx_version_from_locapi_to_pdapi could not translate %d", 
                        loc_vx_version, 0, 0);
        break;
    }
  }
}


static void loc_ni_xlate_vx_version_from_pdapi_to_locapi
(
  pdsm_lcs_agent_version_e_type pdapi_vx_version,
  qmiLocVxVersionEnumT_v02*  loc_vx_version
)
{
  if ( NULL != loc_vx_version )
  {
    switch ( pdapi_vx_version)
    {
      case PDSM_LCS_AGENT_VERSION_V1_ONLY :
          (*loc_vx_version) = eQMI_LOC_VX_VERSION_V1_ONLY_V02;
        break;
      
      case PDSM_LCS_AGENT_VERSION_V2_ONLY:
        (*loc_vx_version) = eQMI_LOC_VX_VERSION_V2_ONLY_V02;
        break;
      default:
        LOC_MSG_ERROR ( "loc_ni_xlate_vx_version_from_pdapi_to_locapi could not translate %d", 
                        loc_vx_version, 0, 0);
        break;
    }
  }
}



// NI cmd callback
static void loc_ni_silent_cmd_cb ( void                      *data_block_ptr,
                                   pdsm_lcs_cmd_e_type        lcs_cmd,
                                   pdsm_lcs_cmd_err_e_type    lcs_cmd_error)
{
   LOC_MSG_MED ("loc_ni_silent_cmd_cb, cmd %d, cmd_err %d", lcs_cmd, lcs_cmd_error, 0);
}

int loc_ni_process_cmd_request (loc_cmd_s_type * const ptr_cmd)
{
    boolean status = FALSE;
    int ret_val = LOC_API_GENERAL_FAILURE;
    
    if(NULL != ptr_cmd)
    {
        const loc_cmd_type_e_type cmd_type = ptr_cmd->cmd_type;
        const loc_client_handle_type client_handle = ptr_cmd->client_handle;
        pdsm_lcs_notification_resp_s_type * const ptr_resp = (pdsm_lcs_notification_resp_s_type *)ptr_cmd->ptr_data;

        switch (cmd_type)
        {
        case LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST:
            if(NULL != ptr_resp)
            {
                status = pdsm_lcs_agent_client_response(loc_ni_silent_cmd_cb,
                                                       (void *) client_handle,
                                                       ptr_resp,
                                                       loc_middleware_data.ni_client_id );
                LOC_MSG_MED ("pdsm_lcs_agent_client_response returned %d", status, 0, 0);
                if(TRUE == status)
                {
                    ret_val = LOC_API_SUCCESS;
                }
                else
                {
                    // something wrong with the response handling
                    ret_val = LOC_API_GENERAL_FAILURE;
                }
            }
            else
            {
                // internal error
                ret_val = LOC_API_GENERAL_FAILURE;
            }
            break;
        default:
            // internal error
            ret_val = LOC_API_UNSUPPORTED;
            break;
        }
    }
   else
   {
       // internal error
       ret_val = LOC_API_GENERAL_FAILURE;
   }

   if(LOC_API_SUCCESS != ret_val)
   {
       LOC_MSG_ERROR ("loc_ni_process_cmd_request internal error", 0, 0, 0);
   }
   return ret_val;
}

