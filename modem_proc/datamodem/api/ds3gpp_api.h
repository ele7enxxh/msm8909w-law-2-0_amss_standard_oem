#ifndef DS3GPP_H
#define DS3GPP_H
/*===========================================================================

               3 G P P   P A C K E T - S W I T C H E D   D A T A

                    E X T E R N A L   H E A D E R   F I L E

  DESCRIPTION
  This file contains external data declarations and function prototypes for 
  3gpp Packet-Switched Data Services.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2010-2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ds3gpp_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/14/12   nd      Sharing API to RRC for PCH indication to applications.
01/25/11   ss      SU API defeaturization and removal of customer.h.
12/29/10   ua      Added API, ds3gpp_is_service_req_allowed_on_plmn.
07/26/10   yz      First revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys.h"
#include "cm_gw.h"
#include "ps_iface_defs.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  RRC states in connected mode. PCH states are URA_PCH and CELL_PCH out of PCH
  states are CELL_FACH and CELL_DCH.
---------------------------------------------------------------------------*/
typedef enum ds_3gpp_pch_event_e{
  DS_3GPP_EVENT_OUT_OF_PCH,
             /* UTRAN moves UE out of PCH states(URA_PCH and CELL_PCH) */
  DS_3GPP_EVENT_IN_PCH
             /* UTRAN moves UE into PCH states(URA_PCH and CELL_PCH) */
} ds_3gpp_pch_event_e_type;

typedef struct
{
 ds_3gpp_pch_event_e_type  pch_event;
 sys_sys_mode_e_type       call_mode;
 sys_modem_as_id_e_type    subs_id; 
}ds_3gpp_rrc_pch_ind_proc_cmd_type;

typedef struct
{
  uint32 rab_id;
  uint32 ul_data_rate;
  sys_modem_as_id_e_type subs_id;
}ds_3gpp_ul_rab_info_type;

/*---------------------------------------------------------------------------
  This structure is used to read the NV item(NV:global_throttling). Other
  modem subsystems (e.g: NAS) can read this NV item using this struct.
---------------------------------------------------------------------------*/
typedef struct
{ 
  uint8   version ;          /* version of the NV item*/
  boolean global_throttling; /* NV item value */
  uint8 reserve_bytes[2];    /* reserve bytes for future use*/
}ds_3gpp_global_throttle_s;

/*---------------------------------------------------------------------------
 type of the notification other modules making to DS 
---------------------------------------------------------------------------*/
typedef enum 
{
  DS_3GPP_INVALID_IND = -1,
  DS_3GPP_RAU_IND= 0
}ds_3gpp_notify_ind_e_type;

/*---------------------------------------------------------------------------
 RAU events that other modules can notify
---------------------------------------------------------------------------*/
typedef enum 
{
DS_3GPP_INVALID_EVT = -1,
DS_3GPP_RAU_COMPLETE_EVT, //? Currently we need only this event. Added other for future cases.
DS_3GPP_RAU_STARTED,
DS_3GPP_RAU_FAILED
}ds_3gpp_rau_event_e_type;

typedef enum 
{
  DS_3GPP_INVALID_RAU_TYPE = -1,
  DS_3GPP_PERIODIC_TYPE,
  DS_3GPP_RA_CAHNGE_TYPE,
  DS_3GPP_OTHER_TYPE,
  DS_3GPP_RAU_MAX

}ds_3gpp_rau_type;

typedef void (*ds_3gpp_bearer_err_ind_cb_type)
(
  uint8 eps_bearer_id,  /*EPS bearer ID  */
  boolean prev_resync_passed /* Previous sync flag passed */
);

typedef struct
{
  sys_modem_as_id_e_type     subs_id;
  sys_sys_mode_e_type        sys_mode;
  ds_3gpp_rau_event_e_type   event;
  ds_3gpp_rau_type           rau_type;
}ds_3gpp_rau_event_s_type;

typedef struct 
{
  uint8	version ;		   /* version of the NV item*/
  boolean bcm_info_validation_enabled; /* NV item value */
  uint8 reserve_bytes[2];    /* reserve bytes for future use*/
}ds_3gpp_bcm_info_validation_s;

typedef struct
{
  ds_3gpp_bearer_err_ind_cb_type pdpc_cb_fptr;
  uint8   threshhold_pkt_cnt;
}ds_3gpp_bearer_err_info_type;


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_3GPP_NOTIFY_IND

DESCRIPTION
   This is a generic function, used to notify DS module.

PARAMETERS
 indication type and 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_notify_ind(ds_3gpp_notify_ind_e_type indication, void *notify_ind_data_ptr);

/*===========================================================================
FUNCTION DS3GPP_PLMN_BLOCKED_FORCE_LTE_PREF

DESCRIPTION
   This is called by lower layers during powere off to see if any PLMN is blocked.
   lower layer will set their next pref sys based on the PLMN blocked status

PARAMETERS
  subs_id 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If any PLMN is blocked
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_plmn_blocked_force_lte_pref
(
  sys_modem_as_id_e_type	  subs_id
);

/*===========================================================================
FUNCTION DS3GPP_IS_PS_DATA_AVAILABLE

DESCRIPTION
   This function returns TRUE if data is available in any of the uplink
   buffers, otherwise, it returns FALSE.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ps_data_available(void);

/*===========================================================================
FUNCTION DS3GPP_PER_SUBS_IS_PS_DATA_AVAILABLE

DESCRIPTION
  This function returns TRUE if data is available in any of the uplink
  buffers, otherwise, it returns FALSE.

PARAMETERS 
   Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_per_subs_is_ps_data_available
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_IS_UE_INIT_QOS_SUPPORTED

DESCRIPTION
   This function returns TRUE if umts UE initiated QOS support
   is available, otherwise, it returns FALSE.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If UE initiated UMTS QOS support is available
  FALSE - If UE initiated UMTS QOS support is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ue_init_qos_supported(void);

/*===========================================================================
FUNCTION DS3GPP_IS_SERVICE_REQ_ALLOWED_ON_PLMN

DESCRIPTION
  This function is used to check if the service request throttling timer is 
  currently running on the PLMN passed into the function.

  If the throttling algorithm has never been kicked off in this PLMN or if the
  throttling timer is not running currently for the PLMN, the UE can send the 
  service request over the air.

PARAMETERS
  plmn_id - The PLMN id in which this the service request is to be sent over air

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If it is ok to send Service request over the air for this PLMN
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_service_req_allowed_on_plmn(sys_plmn_id_s_type plmn_id);


/*===========================================================================
FUNCTION DS3GPP_PER_SUBS_IS_SERVICE_REQ_ALLOWED_ON_PLMN

DESCRIPTION
  This function is used to check if the service request throttling timer is 
  currently running on the PLMN passed into the function.

  If the throttling algorithm has never been kicked off in this PLMN or if the
  throttling timer is not running currently for the PLMN, the UE can send the 
  service request over the air.

PARAMETERS
  plmn_id - The PLMN id in which this the service request is to be sent over air
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If it is ok to send Service request over the air for this PLMN
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_per_subs_is_service_req_allowed_on_plmn
(
  sys_plmn_id_s_type          plmn_id,
  sys_modem_as_id_e_type      subs_id
);
/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PCH_STATE

DESCRIPTION
  This function is used by RRC in WCDMA and TDSCDMA mode to indicate when
  UTRAN moves UE to PCH states: CELL_PCH and URA_PCH. Same needs to be
  propagated to applications. It will be useful for saving power.

PARAMETERS
  sys_mode  - system network mode, PCH states are applicable only in case of
             TDSCDMA and WCDMA.
  pch_event - In or out of PCH state.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_update_pch_state
(
  sys_sys_mode_e_type sys_mode,
  ds_3gpp_pch_event_e_type pch_event
);

/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PCH_STATE_SUB

DESCRIPTION
  This function is used by RRC in WCDMA and TDSCDMA mode to indicate when
  UTRAN moves UE to PCH states: CELL_PCH and URA_PCH. Same needs to be
  propagated to applications. It will be useful for saving power.

PARAMETERS
  subs_id   - Subscription ID
  sys_mode  - system network mode, PCH states are applicable only in case of
             TDSCDMA and WCDMA.
  pch_event - In or out of PCH state.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_update_pch_state_sub
(
  sys_modem_as_id_e_type   subs_id,
  sys_sys_mode_e_type      sys_mode,
  ds_3gpp_pch_event_e_type pch_event  
);

/*==============================================================================

FUNCTION:  ds_eps_bearer_reenqueue_pkts

DESCRIPTION
  This API will be used to traverse the DSM chain(head_dsm_ptr) and
  enqueue each packet to WM (wm_ptr).It will also increase the dne
  count if required based on the num_pkt_bytes

PARAMETERS
    wm_ptr : WM to which pkts need to be re-enqueued
    num_pkt_byes: Number of bytes to be enqueued
    head_dsm_ptr: This will be a chain of DSM items

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

*/

void ds_eps_bearer_reenqueue_pkts
(
  dsm_watermark_type* wm_ptr,     
  uint32 num_pkt_bytes,               
  dsm_item_type* head_dsm_ptr    
);

/*===========================================================================
FUNCTION DS_TDSCDMA_CONFIGURE_UL_DATA_RATE

DESCRIPTION
  This function is used by RRC in TDSCDMA mode to indicate the updated
  data rate on a particular rab_id/nsapi changes. Based on which
  wm levels are adjusted on that bearer

PARAMETERS
  rab_id  - Identifier used to uniquely identify a data bearer
  ul_data_rate - Updated data rate of the bearer
  subs_id      - Subscription on which this call is made

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds_tdscdma_configure_ul_data_rate
(
   uint32 rab_id,  
   uint32  ul_data_rate,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_COMPARE_PDP_ADDRESS

DESCRIPTION
  This function is used by NAS to compare the pdp_address when network
  sends MODIFY_REQUEST with pdp_address included. DS will compare the
  pdp_address passed by NAS against the pdp_address which is configured on
  V4/V6 interfaces.

PARAMETERS
  identifier   - Identifier used to uniquely identify a PDP/PDN Context.
                 (For W/G/TD it would be nsapi, For LTE it would be eps_bid)
  sys_mode     - Current sys_mode.
  subs_id      - Subscription on which the PDP/PDN is active.
  *pdp_address - Structure containing the pdp_address and pdp_type_num 
                 given by network, which would be used for comparison.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  if PDP address sent by network is same as the one configured.
  FALSE - if PDP address sent by network is different than the one
          configured.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_compare_pdp_address
(
  uint8 identifier,
  sys_sys_mode_e_type sys_mode,
  sys_modem_as_id_e_type subs_id,
  pdp_address_T *pdp_address
);

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_multi_pdn_same_apn

DESCRIPTION
  This function gets the multi pdn same apn nv value for each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether Multi Pdn same apn nv is enabled.

SIDE EFFECTS
===========================================================================*/

boolean ds3gpp_per_subs_get_config_multi_pdn_same_apn
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_umts_nw_init_qos_support

DESCRIPTION
  This function gets the umts_nw_init_qos_support NV item: 70331
  modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether UMTS NW Init QoS is enabled or not

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3gpp_per_subs_get_config_umts_nw_init_qos_support
(
  sys_modem_as_id_e_type subs_id
);

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION DS_3GPP_GET_BEARER_ERR_INFO_PTR

DESCRIPTION
  This function returns pointer to to bearer error cache variable

PARAMETERS
None
  
DEPENDENCIES
  None.

RETURN VALUE
  Cache Pointer
  
SIDE EFFECTS
  None.
  
===========================================================================*/
ds_3gpp_bearer_err_info_type* ds_3gpp_get_bearer_err_info_ptr (void);

/*===========================================================================
FUNCTION DS_3GPP_GET_BEARER_ERR_THRESHHOLD_CNT

DESCRIPTION
  This function returns threshhold count value for malformed packets 
   cached at DS 

PARAMETERS
None
  
DEPENDENCIES
  None.

RETURN VALUE
  Threshhold packet count
  
SIDE EFFECTS
  None.
  
===========================================================================*/
uint8 ds_3gpp_get_bearer_err_threshhold_cnt (void);

/*===========================================================================
FUNCTION DS_3GPP_REG_PDCP_BEARER_ERR_CB

DESCRIPTION
  This function is exposed to pdcp to register a callback function to be 
  triggered when malformed packet count on particular bearer exceeds 
  threshhold limit.

PARAMETERS
  pdcp_cb_ptr - callback ptr paseed from PDCP
  threshhold count- malformed packet limit
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
boolean ds_3gpp_reg_pdcp_bearer_err_cb 
( 
  ds_3gpp_bearer_err_ind_cb_type pdcp_cb_fptr, 
  uint8   threshhold_pkt_count 
);

#endif /* FEATURE_DATA_LTE */

#endif /* DS3GPP_H */
