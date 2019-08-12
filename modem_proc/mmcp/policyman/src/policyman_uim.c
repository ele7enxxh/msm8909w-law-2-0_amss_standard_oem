/**
  @file policyman_uim.c

  @brief
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_uim.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_namedobj.h"
#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_plmn.h"
#include "policyman_policies.h"
#include "policyman_rules.h"
#include "policyman_set.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_uim.h"
#include "policyman_xml.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#include "simlock_modem_lib.h"
#include "policyman_svc_mode.h"
#include <stringl/stringl.h>

#define SIM_TYPE_TAG  "sim_type"
#define IMSI_M_DATA_LEN (MMGSDI_IMSI_LEN + 1)


/*=============================================================================
   Structures for MMGSDI
=============================================================================*/
typedef struct
{
  mmgsdi_session_type_enum_type session_type;  /* the key */ 
  sys_modem_as_id_e_type        asubs_id;      /* we assign this */
  mmgsdi_session_id_type        session_id;    /* comes from mmgsdi */
  mmgsdi_app_enum_type          app_type;      /* comes from mmgsdi */
  boolean                       in_use;        /* we manage. true if session open */ 
  boolean                       active;        /* we manage. true if session active */ 
} pm_mmgsdi_session_id_table_entry_t;

static boolean is_mmgsdi_subscription_ready_received = FALSE;

static pm_mmgsdi_session_id_table_entry_t pm_mmgsdi_session_id_table[] = {
  {MMGSDI_GW_PROV_PRI_SESSION, SYS_MODEM_AS_ID_1, 0, MMGSDI_APP_NONE, FALSE, FALSE},
  {MMGSDI_1X_PROV_PRI_SESSION, SYS_MODEM_AS_ID_1, 0, MMGSDI_APP_NONE, FALSE, FALSE},
  {MMGSDI_GW_PROV_SEC_SESSION, SYS_MODEM_AS_ID_2, 0, MMGSDI_APP_NONE, FALSE, FALSE},
  {MMGSDI_1X_PROV_SEC_SESSION, SYS_MODEM_AS_ID_2, 0, MMGSDI_APP_NONE, FALSE, FALSE},
};

/*=============================================================================
  Function Prototype
=============================================================================*/

static void pm_mmgsdi_reg_status_cb( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
);

static void pm_open_sessions(
  void
);

boolean sys_get_num_of_mnc_digits(
  mmgsdi_data_type * imsi_ptr,
  mmgsdi_data_type * ad_ptr,
  uint8            * num_mnc_digits_ptr
);

typedef void (*policyman_mmgsdi_callback_type) (
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

EXTERN void sd_decode_3gpp2_mcc(
  word       *decoded_mcc, 
  const word  encoded_mcc
);

/*=============================================================================
  APIs for UIM information
=============================================================================*/


/*-------- policyman_uim_init --------*/
boolean policyman_uim_init(
  policyman_uim_info_t  **pUimInfo 
)
{
  *pUimInfo = (policyman_uim_info_t *) policyman_mem_alloc(sizeof(policyman_uim_info_t));
  return (*pUimInfo != NULL);
}

/*-------- policyman_uim_deinit --------*/
void policyman_uim_deinit(
  policyman_uim_info_t  *pUimInfo
  )
{
  POLICYMAN_MEM_FREE_IF(pUimInfo);
}

/*-------- policyman_uim_info_deep_copy --------*/

policyman_uim_info_t * policyman_uim_info_deep_copy(
  policyman_uim_info_t  *pUimState
)
{
  policyman_uim_info_t *pUimInfo;
  
  pUimInfo = (policyman_uim_info_t *) policyman_mem_alloc(sizeof(policyman_uim_info_t));

  *pUimInfo = *pUimState;

  return pUimInfo;
}

/*-------- policyman_register_for_uim_events --------*/
void policyman_register_for_uim_events(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  is_mmgsdi_subscription_ready_received = FALSE;

  mmgsdi_status = mmgsdi_client_id_and_evt_reg( NULL,  // Not registering for card status events
                                                pm_mmgsdi_reg_status_cb,
                                                0 );
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1("mmgsdi_client_id_and_evt_reg failed %d", mmgsdi_status);
  }
}

/*=============================================================================
  Event callback definitions
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  mmgsdi_event_data_type   mmgsdi_event;
} policyman_uim_evt_cb_t;

typedef struct
{
   POLICYMAN_CMD_HDR;

   mmgsdi_return_enum_type status;
   mmgsdi_cnf_enum_type    data_type;
   mmgsdi_cnf_type         data;
} pm_mmgsdi_resp_t;

static mmgsdi_client_id_type pm_mmgsdi_client_id = 0;

/*===========================================================================
  FUNCTION PM_PROCESS_MMGSDI_REG_STATUS()

  DESCRIPTION
    MMGSDI process function
 ===========================================================================*/
void pm_process_mmgsdi_reg_status(
  policyman_cmd_t *pCmd
  )
{
  pm_mmgsdi_resp_t *pMmgsdi_resp = (pm_mmgsdi_resp_t *)pCmd;

  if (pMmgsdi_resp->status == MMGSDI_SUCCESS)
  {
    if (pMmgsdi_resp->data_type== MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)
    {
      pm_mmgsdi_client_id = pMmgsdi_resp->data.client_id_and_evt_reg_cnf.response_header.client_id;

      //open sessions irrespective of knowing the SIM inserted or not
      pm_open_sessions();
    }
  }
  else
  {
    switch (pMmgsdi_resp->data_type)
    {
      case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
        POLICYMAN_MSG_ERROR_2( "MMGSDI failed registering PM client ID & EVT registration.  Status %d, Confirmation %d",
                               pMmgsdi_resp->status,
                               pMmgsdi_resp->data_type );
        break;

      default:
        POLICYMAN_MSG_ERROR_2( "MMGSDI returned Failure async. Status %d, Confirmation %d",
                               pMmgsdi_resp->status,
                               pMmgsdi_resp->data_type );
        break;
    }
  }
}

/*===========================================================================
  FUNCTION pm_mmgsdi_ssn_tbl_get_entry_by_type()

  DESCRIPTION
 ===========================================================================*/
static pm_mmgsdi_session_id_table_entry_t * pm_mmgsdi_ssn_tbl_get_entry_by_type(
  mmgsdi_session_type_enum_type session_type
  )
{
  int                                  index;
  pm_mmgsdi_session_id_table_entry_t  *ssn_entry = NULL;


  for(index = 0; index < ARR_SIZE(pm_mmgsdi_session_id_table); index++)
  {
    if( pm_mmgsdi_session_id_table[index].session_type == session_type &&
        pm_mmgsdi_session_id_table[index].in_use == TRUE
      )
    {
      ssn_entry = &pm_mmgsdi_session_id_table[index];
      break;
    }
  }
  return ssn_entry;
}

/*===========================================================================
  FUNCTION pm_mmgsdi_ssn_tbl_get_entry_by_id()

  DESCRIPTION
 ===========================================================================*/
static pm_mmgsdi_session_id_table_entry_t * pm_mmgsdi_ssn_tbl_get_entry_by_id(
  mmgsdi_session_id_type        session_id
  )
{
  int                                  index;
  pm_mmgsdi_session_id_table_entry_t  *ssn_entry = NULL;


  for(index = 0; index < ARR_SIZE(pm_mmgsdi_session_id_table); index++)
  {
    if( (pm_mmgsdi_session_id_table[index].session_id == session_id) &&
        (pm_mmgsdi_session_id_table[index].in_use == TRUE)
      )
    {
      ssn_entry = &pm_mmgsdi_session_id_table[index];
      break;
    }
  }

  return ssn_entry;
}

/*===========================================================================
  FUNCTION print_session_info()
 ===========================================================================*/
static void print_session_info(
  pm_mmgsdi_session_id_table_entry_t *ssn_entry
  )
{
  POLICYMAN_MSG_MED_5( "session identity: session_type %d, subs %d, app_type %d, active %d, in_use %d",
                       ssn_entry->session_type,
                       ssn_entry->asubs_id,
                       ssn_entry->app_type,
                       ssn_entry->active,
                       ssn_entry->in_use );
}

/*===========================================================================
  Data Structure for READ CNF from MMGSDI
 ===========================================================================*/

#define PM_MMGSDI_RPM_AD_SIZE           4 /* EF-AD */


/*-------- sim_session_is_gw --------*/
STATIC boolean sim_session_is_gw(
  mmgsdi_session_type_enum_type session
)
{
  return    session == MMGSDI_GW_PROV_PRI_SESSION
         || session == MMGSDI_GW_PROV_SEC_SESSION;
}

/*-------- sim_session_is_1x --------*/
STATIC boolean sim_session_is_1x(
  mmgsdi_session_type_enum_type session
)
{
  return    session == MMGSDI_1X_PROV_PRI_SESSION
         || session == MMGSDI_1X_PROV_SEC_SESSION;
}

/*-------- policyman_get_imsi_file_id --------*/
STATIC mmgsdi_file_enum_type policyman_get_imsi_file_id(
  mmgsdi_app_enum_type app_type
)
{
  mmgsdi_file_enum_type fileId = MMGSDI_GSM_IMSI;

  switch (app_type)
  {
    case MMGSDI_APP_USIM:
      fileId = MMGSDI_USIM_IMSI;
      break;

    case MMGSDI_APP_CSIM:
      fileId = MMGSDI_CSIM_IMSI_M;
      break;

    case MMGSDI_APP_RUIM:
      fileId = MMGSDI_CDMA_IMSI_M;
      break;

    default:
      break;
  }

  return fileId;
}

/*-------- policyman_get_ad_file_id --------*/
STATIC mmgsdi_file_enum_type policyman_get_ad_file_id(
  mmgsdi_app_enum_type app_type
)
{
  mmgsdi_file_enum_type fileId = MMGSDI_GSM_AD;

  switch (app_type)
  {
    case MMGSDI_APP_USIM:
      fileId = MMGSDI_USIM_AD;
      break;

    default:
      break;
  }

  return fileId;
}


/*-------- policyman_uim_simlock_verified --------*/
STATIC boolean policyman_uim_simlock_verified(
  simlock_imsi_type   imsi
)
{
  simlock_result_enum_type simlock_verify_status;

  simlock_verify_status = simlock_verify_imsi(imsi);
  POLICYMAN_MSG_HIGH_1("simlock_verified status: %d", simlock_verify_status);

  return (   simlock_verify_status == SIMLOCK_SUCCESS
          || simlock_verify_status == SIMLOCK_UNSUPPORTED );

}

STATIC void policyman_uim_handle_imsi_update(
  mmgsdi_data_type        imsiData,
  sys_modem_as_id_e_type  subsId,
  policyman_set_t        *pItemSet
)
{
  simlock_imsi_type            imsi;
  boolean                      simlock_verified;
  boolean                      simlock_verified_old;
  policyman_svc_mode_item_t   *pSvcItem;

  /* check if this IMSI is successfully SIM Lock verified by simlock engine */
    imsi.imsi_len = memscpy( imsi.imsi, 
                             sizeof(imsi.imsi),
                             imsiData.data_ptr, 
                             imsiData.data_len);

  // Get existing simlock status
  simlock_verified_old = policyman_state_get_simlock_verified(subsId);

  simlock_verified = policyman_uim_simlock_verified(imsi);
  policyman_state_update_simlock_verified(subsId, simlock_verified);

  simlock_verified = policyman_state_get_simlock_verified(subsId);

  if (simlock_verified != simlock_verified_old)
  {
    if (simlock_verified)
    {
      pSvcItem = policyman_svc_mode_item_new(POLICYMAN_SVC_MODE_FULL, subsId);
    }
    else /* If SIM Lock not verified, restrict the subscription to limited mode */
    {
      pSvcItem = policyman_svc_mode_item_new(POLICYMAN_SVC_MODE_LIMITED, subsId);
    }

    policyman_set_add(pItemSet, &pSvcItem);
    ref_cnt_obj_release(pSvcItem);
  }
}

/*===========================================================================
 FUNCTION policyman_uim_set_imsi_plmn()

 DESCRIPTION
   Set IMSI PLMN into UIM state
===========================================================================*/
STATIC boolean policyman_uim_set_imsi_plmn(
  policyman_state_t  *pState,
  size_t              subsId,
  sys_plmn_id_s_type *pPlmn
)
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pState, subsId);

  if (!policyman_plmns_are_equal(&pUimInfo->plmn, pPlmn))
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "updating IMSI PLMN", subsId);
    pUimInfo->plmn = *pPlmn;
    changed = TRUE;
  }

  return changed;
}

/*-------- policyman_extract_plmn_from_imsi --------*/
STATIC void policyman_extract_plmn_from_imsi(
  mmgsdi_data_type      imsiData,
  size_t                 numMncDigits,
  sys_plmn_id_s_type   *pPlmn
)
{
  uint8   *imsi = (uint8 *)imsiData.data_ptr;

  pPlmn->identity[0] = (byte)(( ( imsi[1] & 0xF0 ) / 0x10 ) +
                              ( ( imsi[2] & 0x0F ) * 0x10 ));

  // Fill MCC Digit 3 and MNC Digit 3
  pPlmn->identity[1] = (byte)(( ( imsi[2] & 0xF0 ) / 0x10 ) +
                              ( ( imsi[4] & 0x0F ) * 0x10 ));

  // If 2 digits MNC is indicated in EF-AD then set MNC digit 3 to 'F'.
  if (numMncDigits == 2)
   {
    pPlmn->identity[1] |= 0xF0;
   }

  // Fill MNC Digit 1 and MNC Digit 2
  pPlmn->identity[2] = imsi[3];

  POLICYMAN_MSG_PLMN_0("IMSI PLMN: ", *pPlmn);
}

/*-------- policyman_uim_set_imsi_m_mcc --------*/
STATIC boolean policyman_uim_set_imsi_m_mcc(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId,
  sys_mcc_type            mcc
)
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo = policyman_state_get_uim_info(pState, subsId);

  if (pUimInfo->csim_mcc != mcc)
  {
    pUimInfo->csim_mcc = mcc;
    changed            = TRUE;
    POLICYMAN_MSG_HIGH_2("updating IMSI_M MCC %d for Sub %d", mcc, subsId);
  }

  return changed;
}
 
/*-------- policyman_uim_get_imsi_m_mcc --------*/
uint32 policyman_uim_get_imsi_m_mcc(
  policyman_state_t * pState,
  size_t              subsId
)
{
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pState, subsId);
  return pUimInfo->csim_mcc;
}

/*-------- policyman_process_imsi_m_bytes --------*/
STATIC sys_mcc_type policyman_process_imsi_m_bytes(
  sys_modem_as_id_e_type   asubsId,
  mmgsdi_data_type         imsiData
)
{
  uint8          *imsi_m           = (uint8 *)imsiData.data_ptr;
  sys_mcc_type    rawMcc     = 0;
  sys_mcc_type    decodedMcc = 0;
  boolean         imsi_has_changed;

   /* C.S0005 chapter 2.3.1 : 2 bits of 10th byte + 8 bits of 9th byte
     1. Represent the 3 digit MCC as D1D2D3 with a digit equal to 0 given a value of 10.
     2. Compute 100*D1 + 10*D2 + D3 - 111.
     3. Convert to binary and accordingly write in byte 9 & 10.
     e.x MCC 460, D1 = 4, D2 = 6, D3 = 0
          100*4+10*6+0 +10 - 111 = 359 which is 0x0167 in Hex
          hence byte 9 = 0x67, 10 = 0x01.
   */
  rawMcc =  (imsi_m[9] & 0x02) * 512
          + (imsi_m[9] & 0x01) * 256
          + (imsi_m[8] & 0xFF);

  sd_decode_3gpp2_mcc((word *)&decodedMcc, (word)rawMcc);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "processing IMSI_M data: rawMcc %d, decodedMCC %d",
                        asubsId,
                        rawMcc,
                        decodedMcc );

  return decodedMcc;

}


/*===========================================================================
  FUNCTION pm_mmgsdi_read()

  DESCRIPTION
 ===========================================================================*/
STATIC policyman_status_t pm_mmgsdi_read(
  mmgsdi_session_id_type     session_id,
  mmgsdi_file_enum_type      file_name,
  mmgsdi_data_type           read_buffer
)
{
  policyman_status_t        status        = POLICYMAN_STATUS_SUCCESS;
  mmgsdi_return_enum_type   mmgsdi_status;

  mmgsdi_status = mmgsdi_session_read_cache( session_id,
                                             file_name,
                                             read_buffer );

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_2( "mmgsdi_read_cache for file %d failed (mmgsdi_status %d)",
                           file_name,
                           mmgsdi_status );

    status = POLICYMAN_STATUS_ERR;
  }

  return status;
}

#define GOTO_DONE_IF_ERR(status)                         \
  if (status != POLICYMAN_STATUS_SUCCESS) { goto Done; } 

/*===========================================================================
  FUNCTION pm_read_imsi_hplmn()

  DESCRIPTION
 ===========================================================================*/
static policyman_status_t pm_read_imsi_hplmn(
  policyman_state_t                  *pState,
  pm_mmgsdi_session_id_table_entry_t *ssnEntry,
  policyman_set_t                    *pItemSet
)
{
  policyman_status_t                  status    = POLICYMAN_STATUS_ERR;
  mmgsdi_data_type                    imsiData;
  uint8                               imsi[MMGSDI_IMSI_LEN];
  mmgsdi_data_type                    adData;
  uint8                               ad[PM_MMGSDI_RPM_AD_SIZE];
  uint8                               num_mnc_digits;
  sys_plmn_id_s_type                  plmn;
  boolean                             imsi_has_changed = FALSE;

  imsiData.data_len = sizeof(imsi);
  imsiData.data_ptr = imsi;
  adData.data_len   = sizeof(ad);
  adData.data_ptr   = ad;

  /* Read EF-IMSI */
  status = pm_mmgsdi_read( ssnEntry->session_id,
                           policyman_get_imsi_file_id(ssnEntry->app_type),
                           imsiData );
  
  if (!POLICYMAN_SUCCEEDED(status))
  {
    goto Done;
  } 

  status = pm_mmgsdi_read( ssnEntry->session_id,
                           policyman_get_ad_file_id(ssnEntry->app_type),
                           adData );

  if (!POLICYMAN_SUCCEEDED(status))
  {
    goto Done;
  }  

  if (sys_get_num_of_mnc_digits(&imsiData, &adData, &num_mnc_digits))
  {
    /* Extract imsi plmn from imsi bytes */
    policyman_extract_plmn_from_imsi(imsiData, num_mnc_digits, &plmn);

    /* Set SIM refresh only after first time IMSI is set
    */
    if (policyman_state_check_precondition_met(pState, POLICYMAN_PRECOND_IMSI, ssnEntry->asubs_id))
    {
      policyman_uim_set_sim_refresh(pState, ssnEntry->asubs_id, TRUE);
    }
    policyman_state_set_precondition_met(pState, POLICYMAN_PRECOND_IMSI, ssnEntry->asubs_id);
	
    (void)policyman_uim_set_imsi_plmn(pState, ssnEntry->asubs_id, &plmn);

    policyman_uim_handle_imsi_update(imsiData, ssnEntry->asubs_id, pItemSet);
  }

Done:
  if (status != POLICYMAN_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1("EFS-IMSI/AD file read returned status %d", status);
  }

  return status;
} /* pm_read_imsi_hplmn() */

/*-------- pm_read_imsi_m --------*/
STATIC policyman_status_t pm_read_imsi_m(
  policyman_state_t                  *pState,
  pm_mmgsdi_session_id_table_entry_t *ssnEntry,
  policyman_set_t                    *pItemSet
)
{
  policyman_status_t   status = POLICYMAN_STATUS_ERR;
  sys_mcc_type         imsi_m_mcc = 0;

  if (  ssnEntry->app_type == MMGSDI_APP_CSIM
     || ssnEntry->app_type == MMGSDI_APP_RUIM
     )
  {
    mmgsdi_data_type     imsiData;
    uint8                imsi_m[IMSI_M_DATA_LEN];

    imsiData.data_len = sizeof(imsi_m);
    imsiData.data_ptr = imsi_m;

    status = pm_mmgsdi_read( ssnEntry->session_id,
                             policyman_get_imsi_file_id(ssnEntry->app_type),
                             imsiData );

    if (status != POLICYMAN_STATUS_SUCCESS)
    {
      POLICYMAN_MSG_ERROR_1("EFS-IMSI_M file read returned status %d", status);
    }
    else
    {
      imsi_m_mcc = policyman_process_imsi_m_bytes( ssnEntry->asubs_id,
                                                   imsiData );
	  
      policyman_state_set_precondition_met(pState, POLICYMAN_PRECOND_IMSI, ssnEntry->asubs_id);
      policyman_uim_set_imsi_m_mcc(pState, ssnEntry->asubs_id, imsi_m_mcc);

      policyman_uim_handle_imsi_update(imsiData, ssnEntry->asubs_id, pItemSet);
    }
  
  }

  return status;
} /* pm_read_imsi_m() */

/*-------- policyman_uim_get_subs_active --------*/
boolean policyman_uim_get_subs_active(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_uim_info_t *pUimInfo;

  pUimInfo  = policyman_state_get_uim_info(pState, subsId);
  return pUimInfo->subs_active;
}

/*-------- policyman_uim_set_subs_active --------*/
STATIC void policyman_uim_set_subs_active(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type subsId,
  boolean                newState
)
{  
  policyman_uim_info_t *pUimInfo;
  boolean               prevState;

  pUimInfo = policyman_state_get_uim_info(pState, subsId);
  prevState = pUimInfo->subs_active;

  if (prevState != newState)
  {
    pUimInfo->subs_active = newState;
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "active state updated from %d to %d",
                          subsId,
                          prevState,
                          newState );

  }  
}

/*-------- policyman_process_gw_subs --------*/
STATIC void policyman_process_gw_subs(
  policyman_cmd_t                    *pCmd,
  pm_mmgsdi_session_id_table_entry_t *ssnEntry
)
{
  policyman_status_t      status;
  policyman_set_t        *pItemSet;
  policyman_state_t      *pState;

  pState   = pCmd->pState;
  pItemSet = policyman_itemset_new();

  /* read IMSI PLMN for GW subs */
  status = pm_read_imsi_hplmn(pState, ssnEntry, pItemSet);

  if (POLICYMAN_SUCCEEDED(status))
  {
  policyman_state_set_precondition_met(pState, POLICYMAN_PRECOND_SIMTYPE, ssnEntry->asubs_id);
  policyman_uim_set_subs_active(pState, ssnEntry->asubs_id, TRUE);
  policyman_uim_set_sim_type(pState, ssnEntry->asubs_id, ssnEntry->app_type);
  }
  else
  {
    POLICYMAN_MSG_ERROR_1("Internal error in reading HPLMN: %d", status);
  }

  policyman_policies_run_policy_check(pState, pItemSet);
  policyman_cfgitem_update_items(pItemSet, pCmd);
  ref_cnt_obj_release(pItemSet);
}

/*-------- policyman_process_1x_subs --------*/
STATIC void policyman_process_1x_subs(
  policyman_cmd_t                    *pCmd,
  pm_mmgsdi_session_id_table_entry_t *ssnEntry
)
{
  policyman_status_t   status;
  policyman_set_t     *pItemSet;
  policyman_state_t   *pState;

  pItemSet = policyman_itemset_new();
  pState   = pCmd->pState;

  /* read IMSI_M PLMN for 1x subs */
  status = pm_read_imsi_m(pState, ssnEntry, pItemSet);

  if (POLICYMAN_SUCCEEDED(status))
  {
  policyman_state_set_precondition_met(pState, POLICYMAN_PRECOND_CDMASIMTYPE, ssnEntry->asubs_id);
  policyman_uim_set_subs_active(pState, ssnEntry->asubs_id, TRUE);
  (void)policyman_uim_set_cdma_sim_type(pState, ssnEntry->asubs_id, ssnEntry->app_type);
  }
  else
  {
    POLICYMAN_MSG_ERROR_1("Internal error %d in reading IMSI_M", status);
  }

  policyman_policies_run_policy_check(pState, pItemSet);
  policyman_cfgitem_update_items(pItemSet, pCmd);
  ref_cnt_obj_release(pItemSet);  
}

/*-------- policyman_process_subs_ready_event --------*/
STATIC void policyman_process_subs_ready_event(
  policyman_cmd_t                    *pCmd,
  pm_mmgsdi_session_id_table_entry_t *ssnEntry,
  mmgsdi_event_data_type             *pEvent
)
{
  if (is_mmgsdi_subscription_ready_received == FALSE)
  {
    is_mmgsdi_subscription_ready_received = TRUE;

    /* Print out all configuration database when receiving the first MMGSDI_SUBSCRIPTION_READY_EVT
        */
    POLICYMAN_MSG_MED_0("Received the first MMGSDI_SUB_READY_EVT, print all configuration database");
    policyman_cfgitem_display_all();
  }

  ssnEntry->app_type =  pEvent->data.subscription_ready.app_info.app_data.app_type;
  ssnEntry->active   = TRUE;

  if (sim_session_is_gw(ssnEntry->session_type))
  {
    policyman_process_gw_subs(pCmd, ssnEntry);
  }
  else if (sim_session_is_1x(ssnEntry->session_type))
  {
    policyman_process_1x_subs(pCmd, ssnEntry);
  }
}

/*-------- policyman_process_refresh_event --------*/
STATIC void policyman_process_refresh_event(
  policyman_cmd_t                    *pCmd,
  pm_mmgsdi_session_id_table_entry_t *ssnEntry
)
{
  policyman_status_t  status;
  policyman_set_t    *pItemSet;
  policyman_state_t  *pState;

  pItemSet = policyman_itemset_new();
  pState   = pCmd->pState;

  POLICYMAN_MSG_HIGH_1( "Received trigger for UIM refresh for session type %d",
                        ssnEntry->session_type );
  

  status = pm_read_imsi_hplmn(pState, ssnEntry, pItemSet);

  if (status != POLICYMAN_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1("Internal error in reading HPLMN: %d", status);
  }
  else
  {
    policyman_policies_run_policy_check(pState, pItemSet);
    policyman_cfgitem_update_items(pItemSet, pCmd);
  }

  ref_cnt_obj_release(pItemSet);
}

/*-------- policyman_uim_del_sub --------*/
boolean policyman_uim_del_sub(
  policyman_state_t             *pState,
  sys_modem_as_id_e_type        asubs_id,
  mmgsdi_session_type_enum_type session_type
)
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pState, asubs_id);

  if(pUimInfo != NULL)
  {
    if (session_type == MMGSDI_GW_PROV_PRI_SESSION || session_type == MMGSDI_GW_PROV_SEC_SESSION)
    {
      memset(&pUimInfo->plmn, 0, sizeof(sys_plmn_id_s_type));
      pUimInfo->sim_app_type = MMGSDI_APP_NONE;
      changed = TRUE;
      policyman_state_reset_precondition_met(pState, POLICYMAN_PRECOND_SIMTYPE, asubs_id);
    }
    else if (session_type == MMGSDI_1X_PROV_PRI_SESSION || session_type == MMGSDI_1X_PROV_SEC_SESSION)
    {
      pUimInfo->csim_mcc      = 0;
      pUimInfo->cdma_app_type = MMGSDI_APP_NONE;
      changed = TRUE;
      policyman_state_reset_precondition_met(pState, POLICYMAN_PRECOND_CDMASIMTYPE, asubs_id);
    }

    /* Precond IMSI is reset unconditionally on premise that we'll either have GW or 1x session removed */
    policyman_state_reset_precondition_met(pState, POLICYMAN_PRECOND_IMSI, asubs_id);
    policyman_state_set_simlock_verified(asubs_id, TRUE);
  }

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "deleted sub status %d", asubs_id, changed );
  return changed;
}

/*-------- pm_process_uim_evt --------*/
void pm_process_uim_evt(
  policyman_cmd_t *pCmd
)
{
  policyman_uim_evt_cb_t             *pUimCmd   = (policyman_uim_evt_cb_t   *)pCmd;
  mmgsdi_event_data_type             *pEvent    = &((policyman_uim_evt_cb_t *)pCmd)->mmgsdi_event;
  pm_mmgsdi_session_id_table_entry_t *ssnEntry = NULL;

  ssnEntry = pm_mmgsdi_ssn_tbl_get_entry_by_id(pEvent->session_id);

  POLICYMAN_MSG_HIGH_3( "PM got MMGSDI evt %d, session_id %d, ssn_entry 0x%0x",
                        pEvent->evt,
                        pEvent->session_id,
                        ssnEntry );

  if (NULL != ssnEntry)
  {
    print_session_info(ssnEntry);

    switch (pEvent->evt)
    {
      case MMGSDI_SUBSCRIPTION_READY_EVT:
        policyman_state_enter_crit_section();
        policyman_process_subs_ready_event(pCmd, ssnEntry, pEvent);
        policyman_state_leave_crit_section();
        break;

      case MMGSDI_SESSION_CHANGED_EVT:
        if (!pEvent->data.session_changed.activated
           && ssnEntry->active == TRUE
           )
        {
          ssnEntry->active = FALSE;

        policyman_state_enter_crit_section();
        policyman_uim_set_subs_active(pCmd->pState, ssnEntry->asubs_id, FALSE);
        policyman_uim_del_sub(pCmd->pState, ssnEntry->asubs_id, ssnEntry->session_type );
        policyman_state_leave_crit_section();

        policyman_state_handle_update(pCmd->pState);
        }
        break;

      case MMGSDI_REFRESH_EVT:
        policyman_state_enter_crit_section();
        policyman_process_refresh_event(pCmd, ssnEntry);
        policyman_state_leave_crit_section();
        break;

      default:
       break;
    }
  }
}

/*===========================================================================
  FUNCTION pm_mmgsdi_reg_status_cb()

  DESCRIPTION
 ===========================================================================*/
static void pm_mmgsdi_reg_status_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
  )
{
   pm_mmgsdi_resp_t *pCmd = NULL;

   if (NULL == data_ptr)
   {
     return;
   }

   pCmd = POLICYMAN_CMD_NEW( pm_mmgsdi_resp_t,
                             pm_process_mmgsdi_reg_status,
                             NULL,
                             NULL );

   pCmd->data_type = data_type;
   pCmd->status = status;
   pCmd->data = *data_ptr;

   policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
   ref_cnt_obj_release(pCmd);
}


/*===========================================================================
  FUNCTION pm_open_session_cb()

  DESCRIPTION
 ===========================================================================*/
void pm_open_session_cb( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
  )
{
  uint16 i = 0;
  uint16 j = 0;

  /* data_ptr is NULL for ICC card where open_session returns success */
  if (   (NULL == data_ptr)
      || (NULL == data_ptr->session_open_ext_cnf.session_info_ptr) )
  {
    return;
  }

  if ( status == MMGSDI_SUCCESS )
  {
    if (data_type == MMGSDI_SESSION_OPEN_EXT_CNF)
    {
      for (i = 0; i < data_ptr->session_open_ext_cnf.num_sessions; i++)
      {
        for (j = 0; j < ARR_SIZE(pm_mmgsdi_session_id_table); j++)
        {
          if(pm_mmgsdi_session_id_table[j].session_type == data_ptr->session_open_ext_cnf.session_info_ptr[i].session_type)
          {
            pm_mmgsdi_session_id_table[j].session_id = data_ptr->session_open_ext_cnf.session_info_ptr[i].session_id;
            pm_mmgsdi_session_id_table[j].app_type = data_ptr->session_open_ext_cnf.session_info_ptr[i].app_info.app_type;
            pm_mmgsdi_session_id_table[j].in_use = TRUE;

            POLICYMAN_MSG_HIGH_3( "num_sessions %d, session_id %d, session_type %d",
                               data_ptr->session_open_ext_cnf.num_sessions,
                               pm_mmgsdi_session_id_table[j].session_id,
                                  pm_mmgsdi_session_id_table[j].session_type );
          }
        }
      }
    }
  }
}

/*===========================================================================
  FUNCTION pm_open_sessions()

  DESCRIPTION
 ===========================================================================*/
static void pm_open_sessions(
  void
  )
{
  mmgsdi_return_enum_type result;
  uint32                  session_type_mask = ( MMGSDI_GW_PROV_PRI_SESSION_MASK
                                              | MMGSDI_1X_PROV_PRI_SESSION_MASK
                                              | MMGSDI_GW_PROV_SEC_SESSION_MASK
                                              | MMGSDI_1X_PROV_SEC_SESSION_MASK );

  result = mmgsdi_session_open_ext(pm_mmgsdi_client_id,
                                    session_type_mask,
                                    NULL, // Not registering for session events
                                    FALSE,
                                    pm_open_session_cb,
                                    (mmgsdi_client_data_type)session_type_mask );

  if ( MMGSDI_SUCCESS != result )
  {
    /* log that error and return */
    POLICYMAN_MSG_ERROR_1("PM session open failed with error %d", result);
  }
}


/*-------- policyman_uim_get_subs_plmn --------*/
static sys_plmn_id_s_type *policyman_uim_get_subs_plmn(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type subsId
  )
{
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pState, subsId);
  
  return &pUimInfo->plmn;
}


/*===========================================================================
 FUNCTION policyman_uim_get_imsi_mcc()

 DESCRIPTION
===========================================================================*/
uint32 policyman_uim_get_imsi_mcc(
  policyman_state_t *pInfo,
  size_t             subsId
  )
{
  return policyman_plmn_get_mcc(policyman_uim_get_subs_plmn(pInfo, subsId));
}


/*===========================================================================
 FUNCTION policyman_uim_get_imsi_plmn()

 DESCRIPTION
===========================================================================*/
boolean policyman_uim_get_imsi_plmn(
  policyman_state_t  *pInfo,
  size_t                subsId,
  sys_plmn_id_s_type    *pPlmn
  )
{
  if (subsId >= MAX_SIMS)
  { 
    return FALSE; 
  }

  *pPlmn = *policyman_uim_get_subs_plmn(pInfo, subsId);

  return TRUE;
}

/*===========================================================================
 FUNCTION policyman_uim_set_sim_type()

 DESCRIPTION
   Set SIM app type into UIM state
===========================================================================*/
boolean policyman_uim_set_sim_type(
  policyman_state_t    *pInfo,
  size_t                subsId,
  mmgsdi_app_enum_type sim_app_type
  )
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pInfo, subsId);

  if (sim_app_type != pUimInfo->sim_app_type)
  {
    pUimInfo->sim_app_type = sim_app_type;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "updated sim_app_type %d",
                          subsId,
                          sim_app_type );
    changed = TRUE;
  }

  return changed;
}

/*===========================================================================
 FUNCTION policyman_uim_get_sim_type()

 DESCRIPTION
   Get SIM app type from UIM state
===========================================================================*/
mmgsdi_app_enum_type policyman_uim_get_sim_type(
  policyman_state_t  *pInfo,
  size_t                subsId
  )
{
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pInfo, subsId);

  return pUimInfo->sim_app_type;
}

/*===========================================================================
 FUNCTION policyman_uim_set_cdma_sim_type()

 DESCRIPTION
   Set CDMA app type into UIM state
===========================================================================*/
boolean policyman_uim_set_cdma_sim_type(
  policyman_state_t    *pInfo,
  size_t                subsId,
  mmgsdi_app_enum_type sim_app_type
  )
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pInfo, subsId);

  if (sim_app_type != pUimInfo->cdma_app_type)
  {
    pUimInfo->cdma_app_type = sim_app_type;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "updated cdma_sim_app type %d",
                          subsId,
                          sim_app_type );
    changed = TRUE;
  }

  return changed;
}

/*===========================================================================
 FUNCTION policyman_uim_get_cdma_sim_type()

 DESCRIPTION
   Get CDMA SIM app type from UIM state
===========================================================================*/
mmgsdi_app_enum_type policyman_uim_get_cdma_sim_type(
  policyman_state_t     *pInfo,
  size_t                 subsId
)
{
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pInfo, subsId);

  return pUimInfo->cdma_app_type;
}

/*-------- policyman_uim_set_sim_refresh --------*/
void policyman_uim_set_sim_refresh(
  policyman_state_t *pState,
  size_t             subsId,
  boolean            simWasRefreshed
  )
{
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pState, subsId);

  pUimInfo->sim_refreshed = simWasRefreshed;
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "'SIM IMSI refreshed' flag set to %d",
                        subsId,
                        simWasRefreshed );
}

/*-------- policyman_uim_get_sim_refresh --------*/
boolean policyman_uim_get_sim_refresh(
  policyman_state_t *pState,
  size_t  subsId
  )
{
  policyman_uim_info_t *pUimInfo;

  pUimInfo = policyman_state_get_uim_info(pState, subsId);
  
  return pUimInfo->sim_refreshed;
}

/*-------- policyman_uim_sim_is_3gpp2 --------*/
boolean policyman_uim_sim_is_3gpp2(
  policy_execute_ctx_t *pCtx
)
{
  mmgsdi_app_enum_type simType;

  // subsId here is DDS subs
  simType= policyman_uim_get_cdma_sim_type(pCtx->pState, pCtx->asubs_id);

  return (  simType == MMGSDI_APP_RUIM 
         || simType == MMGSDI_APP_CSIM );
}

/*=============================================================================
  Conditions based on serving system.
=============================================================================*/

#define MAX_UIM_CONDITION_SET_SIZE   50


/*-----------------------------------------------------------------------------
  condition_imsi_mcc
-----------------------------------------------------------------------------*/


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_set_t *pMccSet;
  char const      *pSetName;
  boolean          checkIMSI_M;
} policyman_imsi_mcc_condition_t;

/*-------- policyman_str_to_session --------*/
STATIC mmgsdi_session_type_enum_type policyman_str_to_session(
  char const  *pStr
  )
{
  typedef struct
  {
    const char                    *pStr;
    mmgsdi_session_type_enum_type  session;
  } session_map;

  static session_map   map[] =
  {
    {"gw_pri",  MMGSDI_GW_PROV_PRI_SESSION},
    {"gw_sec",  MMGSDI_GW_PROV_SEC_SESSION},
    {"1x",      MMGSDI_1X_PROV_PRI_SESSION},
  };

  mmgsdi_session_type_enum_type  session = MMGSDI_MAX_SESSION_TYPE_ENUM;

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      session = map[i].session;
      break;
    }
  }

#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */

  return session;
}


/*-------- evaluate_imsi_mcc_in --------*/
static boolean evaluate_imsi_mcc_in(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_imsi_mcc_condition_t *pCond = (policyman_imsi_mcc_condition_t *) pCondition;
  policy_execute_ctx_t           *piCtx = (policy_execute_ctx_t *) pCtx;
  uint32                          uimMcc = 0;
  boolean                         result = FALSE;

  /*  If we don't have a set but *do* have a set name, try to find the named set.
   */

  if (pCond->pMccSet == NULL && pCond->pSetName != NULL)
  {    
    pCond->pMccSet = (policyman_set_t *) mre_named_object_find( pCond->pSetName,
                                                                POLICYMAN_NAMED_MCC_SET,
                                                                pCond->pPolicy );
    if (NULL != pCond->pMccSet)
    {
      ref_cnt_obj_add_ref(pCond->pMccSet);
    }
  }

  POLICYMAN_MEM_FREE_IF(pCond->pSetName);

  /*  Without a set, we can't work.
   */
  if (pCond->pMccSet == NULL)             
  { 
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "named PLMN set not found", CONDITION_SUBS(pCond));
    goto Done;
  }

  if (pCond->checkIMSI_M)
  {
    uimMcc = policyman_uim_get_imsi_m_mcc(piCtx->pState, subsId);
  }
  else
  { 
    uimMcc = policyman_uim_get_imsi_mcc(piCtx->pState, subsId);
  }

  result =  policyman_set_contains(pCond->pMccSet, &uimMcc);

Done:
  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX SRC_LINE "condition <imsi_mcc_in pmsubs=%d> with mcc %d returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        uimMcc,
                        result );
  return result;
}


/*-------- policyman_condition_imsi_mcc_evaluate --------*/
static boolean policyman_condition_imsi_mcc_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_imsi_mcc_in);
}


/*-------- policyman_condition_imsi_mcc_dtor --------*/
static void policyman_condition_imsi_mcc_dtor(
  void  *pObj
  )
{
  policyman_imsi_mcc_condition_t  *pCondition =  (policyman_imsi_mcc_condition_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pCondition->pMccSet);
  POLICYMAN_MEM_FREE_IF(pCondition->pSetName);
  policyman_condition_dtor(pCondition);
}


/*-------- policyman_condition_imsi_mcc_new --------*/
mre_status_t policyman_condition_imsi_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                    status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_imsi_mcc_condition_t  *pCondition = NULL;
  policyman_set_t                 *pMccSet = NULL;
  char const                      *pSetName = NULL;
  char const                      *pStr;
  char                            mccStr[4];
  sys_mcc_type                    mcc;
  policyman_subs_t                pm_subs;
  boolean                         checkIMSI_M = FALSE;
  mmgsdi_session_type_enum_type   session;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  /*  See if we need to check against 1x IMSI_M MCC.
   */
  pStr = policyman_xml_get_attribute(pElem, "session");

  if (pStr != NULL)
  {
    session = policyman_str_to_session(pStr);

    checkIMSI_M = (session == MMGSDI_1X_PROV_PRI_SESSION);
  }

  /*  See if there is a named MCC list to use for this condition.
   */
  pStr = policyman_xml_get_attribute(pElem, "list");

  if (pStr != NULL)
  {
    pSetName = policyman_str_dup(pStr);
  }
  else
  {
    /* No named set; try and get an inline list
     */
    pStr = policyman_xml_get_text(pElem);

    if (pStr != NULL)
    {
      /* Create a policyman set to hold the MCCs
       */
      pMccSet = policyman_set_new(sizeof(mcc), 5, MAX_UIM_CONDITION_SET_SIZE, NULL, NULL);

      /* Add the MCCs to the set
       */
      while (policyman_get_token(&pStr, mccStr, sizeof(mccStr)))
      {
        mcc = atoi(mccStr);
        if (mcc == 0)
        {
          REF_CNT_OBJ_RELEASE_IF(pMccSet);
          POLICYMAN_UT_MSG_2( SUBS_PREFIX "<imsi_mcc_in>: invalid MCC: %s",
                              POLICY_SUBS(pPolicy),
                              mccStr );
          goto Done;
        }

        policyman_set_add(pMccSet, &mcc);
      }
    }
  }

  /* If there is neither an inline or named list, bail.
   */
  if (pMccSet == NULL && pSetName == NULL)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "<imsi_mcc_in>: no inline MCC or named list", POLICY_SUBS(pPolicy));
    goto Done;
  }

  /* Create the condition
   */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_imsi_mcc_condition_t,
                                        pPolicy,
                                        policyman_condition_imsi_mcc_evaluate,
                                        policyman_condition_imsi_mcc_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->pMccSet     = pMccSet;
  pCondition->pSetName    = pSetName;
  pCondition->checkIMSI_M = checkIMSI_M;

  status = MRE_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;
  return status;
}



/*-----------------------------------------------------------------------------
  condition_imsi_plmn
-----------------------------------------------------------------------------*/


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  char const      *pSetName;
  policyman_set_t *pPlmnSet;
} policyman_imsi_plmn_condition_t;


/*-------- evaluate_imsi_plmn_in --------*/
static boolean evaluate_imsi_plmn_in(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_imsi_plmn_condition_t *pCond = (policyman_imsi_plmn_condition_t *) pCondition;
  policy_execute_ctx_t            *piCtx = (policy_execute_ctx_t *) pCtx;
  sys_plmn_id_s_type               uimPlmn;
  boolean                          result = FALSE;

  if (pCond->pPlmnSet == NULL && pCond->pSetName != NULL)
  {
    pCond->pPlmnSet = (policyman_set_t *) mre_named_object_find( pCond->pSetName,
                            POLICYMAN_NAMED_PLMN_SET,
                                                                 pCond->pPolicy );

    if (NULL != pCond->pPlmnSet)
    {
      ref_cnt_obj_add_ref(pCond->pPlmnSet);
    }
  }

  POLICYMAN_MEM_FREE_IF(pCond->pSetName);
  if (pCond->pPlmnSet == NULL)             
  { 
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "failed to get PLMN set", CONDITION_SUBS(pCond));
    goto Done; 
  }

  policyman_uim_get_imsi_plmn(piCtx->pState, subsId, &uimPlmn);

  result = policyman_set_contains(pCond->pPlmnSet, &uimPlmn);
  POLICYMAN_MSG_PLMN_0("IMSI PLMN: ", uimPlmn);

Done:

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <imsi_plmn_in pmsubs=%d> returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        result );
  return result;
}


/*-------- policyman_condition_imsi_plmn_evaluate --------*/
STATIC boolean policyman_condition_imsi_plmn_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_imsi_plmn_in);
}


/*-------- policyman_condition_imsi_plmn_dtor --------*/
static void policyman_condition_imsi_plmn_dtor(
  void  *pObj
  )
{
  policyman_imsi_plmn_condition_t *pCondition = (policyman_imsi_plmn_condition_t *)pObj;

  REF_CNT_OBJ_RELEASE_IF(pCondition->pPlmnSet);
  POLICYMAN_MEM_FREE_IF(pCondition->pSetName);
  policyman_condition_dtor(pCondition);
}


/*-------- policyman_condition_imsi_plmn_new --------*/
mre_status_t policyman_condition_imsi_plmn_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                     status;
  policyman_imsi_plmn_condition_t *pCondition = NULL;
  char const                      *pStr;
  policyman_subs_t                pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "<imsi_plmn> invalid 'subs' value for condition", POLICY_SUBS(pPolicy));
    goto Done;
  }

  status = MRE_STATUS_ERR_INVALID_CONDITION;

  /*  Allocate the condition
   */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_imsi_plmn_condition_t,
                                        pPolicy,
                                        policyman_condition_imsi_plmn_evaluate,
                                        policyman_condition_imsi_plmn_dtor,
                                        pm_subs,
                                        pElem );

  /*  See if there is a named PLMN list to use for this condition.
   */
  pStr = policyman_xml_get_attribute(pElem, "list");

  if (pStr != NULL)
  {
    pCondition->pSetName = policyman_str_dup(pStr);
    status = MRE_STATUS_SUCCESS;
  }
  else
  {
  /*  No name - read an inline list.
   */
  pStr = policyman_xml_get_text(pElem);
  if (pStr != NULL)
  {
      pCondition->pPlmnSet = policyman_plmn_list_read(pElem, pStr);
    if (pCondition->pPlmnSet != NULL)
    {
      status = MRE_STATUS_SUCCESS;
    }
  }
  }

  if (MRE_SUCCEEDED(status))
  {
    *ppCondition = (mre_condition_t *) pCondition;
  }
  else
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<imsi_plmn_in>: PLMN list required", POLICY_SUBS(pCondition));
    REF_CNT_OBJ_RELEASE_IF(pCondition);
  }

Done:

  return status;
}


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  mmgsdi_app_enum_type sim_app_type;
} policyman_sim_type_condition_t;


/*-------- evaluate_sim_type --------*/
static boolean evaluate_sim_type(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_sim_type_condition_t *pCond = (policyman_sim_type_condition_t*)pCondition;
  policy_execute_ctx_t           *pContext = (policy_execute_ctx_t *)pCtx;
  mmgsdi_app_enum_type            sim_app_type;
  mmgsdi_app_enum_type            cdma_app_type;
  boolean                         result = FALSE;

  sim_app_type  = policyman_uim_get_sim_type(pContext->pState, subsId);
  cdma_app_type = policyman_uim_get_cdma_sim_type(pContext->pState, subsId);

  switch(pCond->sim_app_type)
  {
    case MMGSDI_APP_USIM:
      result =  pCond->sim_app_type == sim_app_type;
      break;

    case MMGSDI_APP_CSIM:
    case MMGSDI_APP_RUIM:
      result =  pCond->sim_app_type == cdma_app_type;
      break;

   default:
    break;    
  }

  POLICYMAN_MSG_HIGH_6( SUBS_PREFIX SRC_LINE "condition <sim_type pmsubs=%d> with SIM type %d, CSIM type %d, returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        sim_app_type,
                        cdma_app_type,
                        result );
  return result;
}


static boolean policyman_condition_sim_type_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_sim_type);
}

static mmgsdi_app_enum_type policyman_str_to_sim_type(
  const char  *pSim_type
  )
{
  mmgsdi_app_enum_type sim_type = MMGSDI_APP_UNKNOWN;
  typedef struct
  {
    const char  *pStr;
    mmgsdi_app_enum_type      sim_type;
  } mask_map;

  static mask_map map[] =
  {
    {"3G",   MMGSDI_APP_USIM},
    {"2G",   MMGSDI_APP_SIM},
    {"CSIM", MMGSDI_APP_CSIM},
    {"RUIM", MMGSDI_APP_RUIM},
    {"NONE", MMGSDI_APP_NONE}
  };

  size_t  i;
  char    token[32];

  policyman_get_token(&pSim_type, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      sim_type = map[i].sim_type;
      break;
    }
  }

  return sim_type;
}


/*-------- policyman_condition_sim_type_new --------*/
mre_status_t policyman_condition_sim_type_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                    status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_sim_type_condition_t  *pCondition = NULL;
  mmgsdi_app_enum_type            simType;
  char const                      *pStr;
  policyman_subs_t                pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pStr = policyman_xml_get_text(pElem);

  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<sim_type> must specify SIM type", POLICY_SUBS(pPolicy));
    goto Done;
  }

  simType = policyman_str_to_sim_type(pStr);

  if (simType == MMGSDI_APP_UNKNOWN)
  {
    POLICYMAN_UT_MSG_2( SUBS_PREFIX "<sim_type> invalid SIM type: %s",
                        POLICY_SUBS(pPolicy),
                        pStr );
    goto Done;
  }

  /*  Allocate the condition
   */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_sim_type_condition_t,
                                        pPolicy,
                                        policyman_condition_sim_type_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->sim_app_type = simType;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-----------------------------------------------------------------------------
  condition_sim_refresh
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;
} policyman_sim_refresh_condition_t;


/*-------- evaluate_sim_refresh --------*/
static boolean evaluate_sim_refresh(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policy_execute_ctx_t *pUimCtx = (policy_execute_ctx_t *)pCtx;
  boolean               sim_refreshed;

  sim_refreshed = policyman_uim_get_sim_refresh(pUimCtx->pState, subsId);

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <sim_refresh pmsubs=%d> returns %d",
                        CONDITION_SUBS(pCondition),
                        LINE_NUM(pCondition),
                        subsId,
                        sim_refreshed );
  return sim_refreshed;
}


/*-------- policyman_condition_sim_refresh_evaluate --------*/
static boolean policyman_condition_sim_refresh_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_sim_refresh);
}


/*-------- policyman_condition_sim_refresh_new --------*/
mre_status_t policyman_condition_sim_refresh_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                       status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_sim_refresh_condition_t  *pCondition = NULL;
  policyman_subs_t                   pm_subs;
 
  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }
 
  /*  Allocate the condition
   */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_sim_refresh_condition_t,
                                        pPolicy,
                                        policyman_condition_sim_refresh_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );

  status = MRE_STATUS_SUCCESS;
 
  *ppCondition = (mre_condition_t *) pCondition;
 
 Done:

  return status;  
}

/*=============================================================================
  <sim_refresh_reset>
=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;
} uim_sim_refresh_action_t;


/*-------- execute_sim_refresh_reset --------*/
static boolean execute_sim_refresh_reset(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId 
  )
{
  policy_execute_ctx_t *pContext = (policy_execute_ctx_t *)pCtx;

  policyman_uim_set_sim_refresh(pContext->pState, subsId, FALSE);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "<sim_refresh_reset pmsubs=%d> done",
                        ACTION_SUBS(pAction),
                        LINE_NUM(pAction),
                        subsId );
  return TRUE;
}


/*-------- policyman_uim_sim_refresh_reset_execute --------*/
static boolean policyman_uim_sim_refresh_reset_execute(
  mre_action_t const  *pAction,
  void                      *pCtx
  )
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_sim_refresh_reset);
}


/*-------- policyman_uim_sim_refresh_reset_action_new --------*/
mre_status_t policyman_uim_sim_refresh_reset_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
  mre_status_t               status = MRE_STATUS_ERR_INVALID_ACTION;
  uim_sim_refresh_action_t  *pAction = NULL;
  policyman_subs_t           pm_subs = SUBS_THIS;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( uim_sim_refresh_action_t,
                                  pPolicy,
                                  policyman_uim_sim_refresh_reset_execute,
                                  policyman_action_dtor,
                                  pm_subs,
                                  pElem );
  
  status = MRE_STATUS_SUCCESS;

  *ppAction = (mre_action_t *) pAction;

Done:
  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "action_new <sim_refresh_reset pmsubs=%d> returned status %d",
                        POLICY_SUBS(pPolicy),
                        pm_subs,
                        status );
  return status;
}


/*-------- policyman_uim_fill_diag_session_info --------*/
void policyman_uim_fill_diag_session_info(
  size_t                          count,
  pm_dump_uim_session_info_t     *dump_uim_info
)
{
  size_t     idx;

  if (count > ARR_SIZE(pm_mmgsdi_session_id_table))
  {
    count = ARR_SIZE(pm_mmgsdi_session_id_table);
  }

  for (idx = 0; idx < count; idx++)
  {
    dump_uim_info[idx].session_type = pm_mmgsdi_session_id_table[idx].session_type;
    dump_uim_info[idx].asubs_id     = pm_mmgsdi_session_id_table[idx].asubs_id;
    dump_uim_info[idx].session_id   = pm_mmgsdi_session_id_table[idx].session_id;
    dump_uim_info[idx].app_type     = pm_mmgsdi_session_id_table[idx].app_type;
    dump_uim_info[idx].in_use       = pm_mmgsdi_session_id_table[idx].in_use;
    dump_uim_info[idx].active       = pm_mmgsdi_session_id_table[idx].active;
  }
}

/*-------- policyman_uim_fill_diag_uim_info --------*/
void policyman_uim_fill_diag_uim_info(
  policyman_uim_info_t  *src_uim_info,
  pm_dump_uim_info_t    *dump_uim_info
)
{
  dump_uim_info->plmn.identity[0] = src_uim_info->plmn.identity[0];
  dump_uim_info->plmn.identity[1] = src_uim_info->plmn.identity[1];
  dump_uim_info->plmn.identity[2] = src_uim_info->plmn.identity[2];
  dump_uim_info->sim_app_type     = src_uim_info->sim_app_type;
  dump_uim_info->cdma_app_type    = src_uim_info->cdma_app_type;
  dump_uim_info->sim_refreshed    = src_uim_info->sim_refreshed;
}


/*-------- session_availability_info_has_changed --------*/
STATIC boolean session_availability_info_has_changed(
  pm_mmgsdi_session_id_table_entry_t  *ssn_entry,
  policyman_session_avail_enum_t       notifyType,
  mmgsdi_app_enum_type                 appType
)
{
  return (   ((notifyType == SESSION_AVAILABLE)     && !ssn_entry->active)
          || ((notifyType == SESSION_AVAILABLE)     && (ssn_entry->app_type != appType))
          || ((notifyType == SESSION_NOT_AVAILABLE) &&  ssn_entry->active)
          ||  (notifyType == SESSION_REFRESHED) );
}

/*-------- policyman_report_session_availability --------*/
msgr_hdr_s * policyman_report_session_availability(
  policyman_session_avail_enum_t notifyType,
  mmgsdi_session_type_enum_type  sessionType,
  mmgsdi_app_enum_type           appType
)
{
  msgr_hdr_s                          *pMsg           = NULL;
  pm_mmgsdi_session_id_table_entry_t  *ssn_entry;
  boolean                              infoHasChanged = FALSE;

  ssn_entry = pm_mmgsdi_ssn_tbl_get_entry_by_type(sessionType);

  if (NULL == ssn_entry)
  {
    goto Done;
  }

  infoHasChanged = session_availability_info_has_changed(ssn_entry, notifyType, appType);

  if (infoHasChanged)
  {
    policyman_uim_evt_cb_t  *pCmd;

    pCmd = POLICYMAN_CMD_WITH_HOOK_NEW( policyman_uim_evt_cb_t,
                                        pm_process_uim_evt,
                                        NULL,
                                        NULL,
                                        policyman_cfgitem_update_notification_hook );

    pCmd->mmgsdi_event.session_id = ssn_entry->session_id;

    // Create MMGSDI Command based on CM info
    switch (notifyType)
    {
      case SESSION_AVAILABLE:
        pCmd->mmgsdi_event.evt = MMGSDI_SUBSCRIPTION_READY_EVT;
        pCmd->mmgsdi_event.data.subscription_ready.app_info.app_data.app_type = appType;
        break;

      case SESSION_NOT_AVAILABLE:
        pCmd->mmgsdi_event.evt = MMGSDI_SESSION_CHANGED_EVT;
        pCmd->mmgsdi_event.data.session_changed.activated = FALSE;
        break;

      case SESSION_REFRESHED:
        pCmd->mmgsdi_event.evt = MMGSDI_REFRESH_EVT;
        break;

      default:
        break;
    }

    POLICYMAN_MSG_HIGH_0("blocking caller of policyman_report_session_availability()");
    pCmd->signal = policyman_client_block_sig_get();
    policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
    policyman_client_block_signal_wait(pCmd->signal);

    pMsg = pCmd->pMsg;
    ref_cnt_obj_release(pCmd);
  }

Done:
  POLICYMAN_MSG_HIGH_6( "return from report_session_availability(notifyT %d, sessT %d, appT %d), ssn_entry 0x%0x, infoHasChanged %d, pMsg 0x%x",
                        notifyType,
                        sessionType,
                        appType,
                        ssn_entry,
                        infoHasChanged,
                        pMsg );
  return pMsg;
}
