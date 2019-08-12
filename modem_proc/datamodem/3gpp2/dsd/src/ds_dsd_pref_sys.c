/*===========================================================================
                     D S _ D S D _ P R E F _ S Y S . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2011 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_pref_sys.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    rs    Fixed KW errors
08/23/13    rs    Replaced memcpy with memscpy
08/06/13    rs    Changed strlcpy to proper format 
06/03/13    sd    ATCoP decoupling changes
11/11/11    sk    Created Module

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_dsd_ext.h"
#include "ds_dsd_pref_sys.h"
#include "ds_dsd_proc.h"
#include "ds_dsd_common.h"
#include "ds_dsd_efs_parser.h"
#include "ds_dsd_timer.h"
#include "data_msg.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*-------------------------------------------------------------------------
  Bit Masks to define Radio. Each client is a bit
-------------------------------------------------------------------------*/
#define DS_DSD_RADIO_MASK_LTE                 0x01
#define DS_DSD_RADIO_MASK_HDR                 0x02
#define DS_DSD_RADIO_MASK_1X                  0x04
#define DS_DSD_RADIO_MASK_GSM                 0x08
#define DS_DSD_RADIO_MASK_WCDMA               0x10
#define DS_DSD_RADIO_MASK_TDS                 0x20
#define DS_DSD_RADIO_MASK_WLAN                0x40

/*===========================================================================

                        GLOBAL VARIABLES

===========================================================================*/

ds_dsd_pref_notify_func_type notify_func = {NULL};

/*------------------------------------------------------------------------
  Recomputation flag and critical section
------------------------------------------------------------------------*/
rex_crit_sect_type      ds_dsd_recompute_critical_section = {{0}};

/* Current UE prefer mode  */
static cm_mode_pref_e_type pref_mode[DS3GSUBSMGR_SUBS_ID_COUNT] = {0};

/* LTE emergency call is going on or not*/
static boolean lte_emergency_call_in_prog[DS3GSUBSMGR_SUBS_ID_COUNT] = {0};
/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_UPDATE_PREF_ORDER_ON_FEATURE

DESCRIPTION   This function updates the priorities of radios pref in order  
              list based on feature definition. If a feature is not defined
              its priority will be set to MAX and it wont be considered as
              pref system.This function is called only during init.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Preferred order list is updated based on feature enabled.
===========================================================================*/
void ds_dsd_pref_sys_update_pref_order_on_feature
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint16                    i=0;
  boolean                   feature_avail = TRUE;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  dsd_pref_sys_info_type   *pref_sys_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d", subs_id);
    return;
  }
  pref_order_list = pref_sys_info->pref_ordered_list;

  for(i = 0; i<DS_DSD_MAX_RADIO; i++)
  {
    feature_avail = TRUE;
    switch(pref_order_list[i].radio_type)
    {
      case DS_DSD_RADIO_LTE:
        #ifndef FEATURE_DATA_LTE
        feature_avail = FALSE;
        #endif /*FEATURE_DATA_LTE*/
        break;
      case DS_DSD_RADIO_HDR:
        #ifndef FEATURE_HDR
        feature_avail = FALSE;
        #endif /*FEATURE_HDR*/
        break;
      case DS_DSD_RADIO_1X:
        #ifndef FEATURE_CDMA
        feature_avail = FALSE;
        #endif /*FEATURE_CDMA*/
        break;
      case DS_DSD_RADIO_GSM:
        #ifndef FEATURE_GSM
        feature_avail = FALSE;
        #endif /*FEATURE_GSM*/
        break;
      case DS_DSD_RADIO_TDSCDMA:
        #ifndef FEATURE_TDSCDMA
        feature_avail = FALSE;
        #endif /*FEATURE_TDSCDMA*/
        break;
      case DS_DSD_RADIO_WCDMA:
        #ifndef FEATURE_WCDMA
        feature_avail = FALSE;
        #endif /*FEATURE_WCDMA*/
        break;
      case DS_DSD_RADIO_WLAN:
        #ifndef FEATURE_DATA_WLAN_MAPCON
        feature_avail = FALSE;
        #endif /*FEATURE_DATA_WLAN_MAPCON*/
        break;
      default:
        break;
    }

    /*-------------------------------------------------------------------------
      If feature unavailable, set availability flag to FALSE and priority to
      least priority.
    -------------------------------------------------------------------------*/
    if(feature_avail == FALSE)
    {
      pref_order_list[i].priority   = DSD_PRIORITY_MAX;
      pref_order_list[i].available  = FALSE;
    }
  }
  return;
}/* ds_dsd_pref_sy_update_pref_order_on_feature() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_UPDATE_PREF_ORDER_ON_RADIO_MASK

DESCRIPTION   This function updates the preferred order list based on radio 
              mask.
  
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Preferred order list is updated based on radio mask.
===========================================================================*/
void ds_dsd_pref_sys_update_pref_order_on_radio_mask
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  uint16                      radio_mask
)
{
  uint16                    index=0;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  dsd_pref_sys_info_type   *pref_sys_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_HIGH,
            "ds_dsd_pref_sys_update_pref_order_on_radio_mask. radio_mask:0x%x",
            radio_mask);

  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d", subs_id);
    return;
  }
  pref_order_list = pref_sys_info->pref_ordered_list;

  /*---------------------------------------------------------------------------
    Logic to set the highest priority system based on mode preference
      1. If LTE is present, LTE will be set as the most preferred system
      2. Else if HDR is present, HDR will be set as the most preferred system
      3. Else if 1X is present, 1X will be set as the most preferred system
      4. Else if TDS is present, TDS will be set as the most preferred system
      5. Else if WCDMA is present, W will be set as the most preferred system
      6. Else if GSM is present, GSM will be set as the most preferred system
  ---------------------------------------------------------------------------*/
  if((radio_mask & DS_DSD_RADIO_MASK_LTE) != 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_LTE, subs_id);
    pref_order_list[index].priority = 0;
  }
  else if((radio_mask & DS_DSD_RADIO_MASK_HDR) != 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_HDR, subs_id);
    pref_order_list[index].priority = 0;
  }
  else if((radio_mask & DS_DSD_RADIO_MASK_1X) != 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index( DS_DSD_RADIO_1X, subs_id);
    pref_order_list[index].priority = 0;
  }
  else if((radio_mask & DS_DSD_RADIO_MASK_TDS) != 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_TDSCDMA, subs_id);
    pref_order_list[index].priority = 0;
  }
  else if((radio_mask & DS_DSD_RADIO_MASK_WCDMA) != 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_WCDMA, subs_id);
    pref_order_list[index].priority = 0;
  }
  else
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_GSM, subs_id);
    pref_order_list[index].priority = 0;
  }

  /*---------------------------------------------------------------------------
    Set the available flag and priority for each radio in preferred order list
  ---------------------------------------------------------------------------*/
  if((radio_mask & DS_DSD_RADIO_MASK_LTE) == 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_LTE, subs_id);
    pref_order_list[index].available = FALSE;
    pref_order_list[index].priority  = DSD_PRIORITY_MAX;
  }
  if((radio_mask & DS_DSD_RADIO_MASK_HDR) == 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_HDR, subs_id);
    pref_order_list[index].available = FALSE;
    pref_order_list[index].priority  = DSD_PRIORITY_MAX;
  }
  if((radio_mask & DS_DSD_RADIO_MASK_1X) == 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_1X, subs_id);
    pref_order_list[index].available = FALSE;
    pref_order_list[index].priority  = DSD_PRIORITY_MAX;
  }
  if((radio_mask & DS_DSD_RADIO_MASK_TDS) == 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_TDSCDMA, subs_id);
    pref_order_list[index].available = FALSE;
    pref_order_list[index].priority  = DSD_PRIORITY_MAX;
  }
  if((radio_mask & DS_DSD_RADIO_MASK_WCDMA) == 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_WCDMA, subs_id);
    pref_order_list[index].available = FALSE;
    pref_order_list[index].priority  = DSD_PRIORITY_MAX;
  }
  if((radio_mask & DS_DSD_RADIO_MASK_GSM) == 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_GSM, subs_id);
    pref_order_list[index].available = FALSE;
    pref_order_list[index].priority  = DSD_PRIORITY_MAX;
  }
  if((radio_mask & DS_DSD_RADIO_MASK_WLAN) == 0)
  {
    index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_WLAN, subs_id);
    pref_order_list[index].available = FALSE;
    pref_order_list[index].priority  = DSD_PRIORITY_MAX;
  }
  return;
}/* ds_dsd_pref_sys_update_pref_order_on_radio_mask() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_UPDATE_PREF_ORDER_FRM_MODE_PREF

DESCRIPTION   This function reads the mode preference and and updates the 
              preferred order list accordingly.
  
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Preferred order list is updated based on mode preference.
===========================================================================*/
void ds_dsd_pref_sys_update_pref_order_frm_mode_pref
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  cm_mode_pref_e_type         mode_pref
)
{
  uint16  radio_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_HIGH,
            "ds_dsd_pref_sys_update_pref_order_frm_mode_pref. mode_pref:%d",
            mode_pref);

  switch(mode_pref)
  {
    case CM_MODE_PREF_CDMA_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_1X;
      break;

    case CM_MODE_PREF_CDMA_WLAN:
    case CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_WLAN;
      break;

    case CM_MODE_PREF_CDMA_GSM:
    case CM_MODE_PREF_CDMA_GSM_AMPS:
    case CM_MODE_PREF_CDMA_GSM_GPS_AMPS:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_GSM;
      break;

    case CM_MODE_PREF_CDMA_GW_TDS:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_GSM|
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_TDS;
      break;

    case CM_MODE_PREF_HDR_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_HDR;
      break;

    case CM_MODE_PREF_CDMA_HDR_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_HDR_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR;
      break;

    case CM_MODE_PREF_HDR_WLAN:
      radio_mask = DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_WLAN;
      break;

    case CM_MODE_PREF_CDMA_HDR_WLAN:
    case CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_WLAN;
      break;

    case CM_MODE_PREF_CDMA_HDR_GW_TDS:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_TDS;
      break;

    case CM_MODE_PREF_CDMA_HDR_GSM_AMPS:
    case CM_MODE_PREF_CDMA_HDR_GSM_GPS_AMPS:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_GSM;
      break;

    case CM_MODE_PREF_GSM_ONLY:
    case CM_MODE_PREF_GSM_GPS:
      radio_mask = DS_DSD_RADIO_MASK_GSM;
      break;

    case CM_MODE_PREF_GSM_WLAN:
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WLAN;
      break;

#ifdef FEATURE_WCDMA
    case CM_MODE_PREF_WCDMA_ONLY:
    case CM_MODE_PREF_WCDMA_GPS:
      radio_mask = DS_DSD_RADIO_MASK_WCDMA;
      break;

    case CM_MODE_PREF_WCDMA_WLAN:
      radio_mask = DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_WLAN;
      break;

    case CM_MODE_PREF_GSM_WCDMA_ONLY:
    case CM_MODE_PREF_GW_GPS:	
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA;
      break;

    case CM_MODE_PREF_GW_WLAN:
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_WLAN;
      break;
#endif /* FEATURE_WCDMA */

    case CM_MODE_PREF_CDMA_HDR_GW:
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_1X  |
                   DS_DSD_RADIO_MASK_HDR;
      break;
	               
    case CM_MODE_PREF_CDMA_GW:
       radio_mask = DS_DSD_RADIO_MASK_1X |
                    DS_DSD_RADIO_MASK_GSM |
                    DS_DSD_RADIO_MASK_WCDMA;
       break;
    case CM_MODE_PREF_CDMA_HDR_GSM:
       radio_mask = DS_DSD_RADIO_MASK_1X |
                    DS_DSD_RADIO_MASK_HDR |
                    DS_DSD_RADIO_MASK_GSM;
       break;

   case CM_MODE_PREF_HDR_GSM:
      radio_mask = DS_DSD_RADIO_MASK_HDR |
                  DS_DSD_RADIO_MASK_GSM;
      break;
#ifdef FEATURE_TDSCDMA
    case CM_MODE_PREF_TDS_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_TDS;
      break;

    case CM_MODE_PREF_TDS_GSM:
    case CM_MODE_PREF_TDS_GSM_GPS:
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_TDS;
      break;

    case CM_MODE_PREF_TDS_GSM_WCDMA:
    case CM_MODE_PREF_TDS_GSM_WCDMA_GPS:
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_TDS;
      break;

    case CM_MODE_PREF_TDS_WCDMA:
      radio_mask = DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_TDS;
      break;
#endif /*FEATURE_TDSCDMA*/

#ifdef FEATURE_DATA_LTE
    case CM_MODE_PREF_LTE_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_GWL:
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_CDMA_LTE_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_HDR_LTE_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_GSM_LTE:
      radio_mask = DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_WCDMA_LTE:
      radio_mask = DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_CDMA_HDR_LTE_ONLY:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_CDMA_GSM_LTE:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_CDMA_WCDMA_LTE:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_HDR_GSM_LTE:
      radio_mask = DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_HDR_WCDMA_LTE:
      radio_mask = DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_TDS_GSM_LTE:
      radio_mask = DS_DSD_RADIO_MASK_TDS |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_TDS_GSM_WCDMA_LTE:
      radio_mask = DS_DSD_RADIO_MASK_TDS |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_TDS_LTE:
      radio_mask = DS_DSD_RADIO_MASK_TDS |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_CDMA_GSM_WCDMA_LTE:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_CDMA_HDR_GSM_TDS_LTE:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_TDS |
                   DS_DSD_RADIO_MASK_LTE;
      break;

    case CM_MODE_PREF_TDS_WCDMA_LTE:
      radio_mask = DS_DSD_RADIO_MASK_TDS|
                   DS_DSD_RADIO_MASK_WCDMA |
                   DS_DSD_RADIO_MASK_LTE;
      break;
     
    case CM_MODE_PREF_ANY_BUT_HDR:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_LTE |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_TDS |
                   DS_DSD_RADIO_MASK_WCDMA|
                   DS_DSD_RADIO_MASK_WLAN;
      break;

    case CM_MODE_PREF_ANY_BUT_HDR_WLAN:
     radio_mask = DS_DSD_RADIO_MASK_1X |
                  DS_DSD_RADIO_MASK_LTE |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_TDS |
                  DS_DSD_RADIO_MASK_WCDMA;
     break;

   case CM_MODE_PREF_ANY_BUT_WLAN:
     radio_mask = DS_DSD_RADIO_MASK_1X |
                  DS_DSD_RADIO_MASK_HDR |
                  DS_DSD_RADIO_MASK_LTE |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_TDS |
                  DS_DSD_RADIO_MASK_WCDMA;
     break;

   case CM_MODE_PREF_GWL_WLAN:
     radio_mask = DS_DSD_RADIO_MASK_LTE |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_WCDMA|
                  DS_DSD_RADIO_MASK_WLAN;
     break;

   case CM_MODE_PREF_ANY_BUT_TDS:
     radio_mask = DS_DSD_RADIO_MASK_1X |
                  DS_DSD_RADIO_MASK_HDR |
                  DS_DSD_RADIO_MASK_LTE |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_WCDMA |
                  DS_DSD_RADIO_MASK_WLAN;
     break;

   case CM_MODE_PREF_ANY_BUT_HDR_TDS:
     radio_mask = DS_DSD_RADIO_MASK_1X | 
                  DS_DSD_RADIO_MASK_LTE |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_WCDMA |
                  DS_DSD_RADIO_MASK_WLAN;
     break;

   case CM_MODE_PREF_ANY_BUT_CDMA_HDR:
     radio_mask = DS_DSD_RADIO_MASK_LTE |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_TDS |
                  DS_DSD_RADIO_MASK_WCDMA |
                  DS_DSD_RADIO_MASK_WLAN;
     break;
  
#endif /* FEATURE_DATA_LTE */

   case CM_MODE_PREF_ANY_BUT_LTE:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_TDS |
                   DS_DSD_RADIO_MASK_WCDMA|
                   DS_DSD_RADIO_MASK_WLAN;
      break;

   case CM_MODE_PREF_ANY_BUT_HDR_LTE:
     radio_mask = DS_DSD_RADIO_MASK_1X |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_TDS |
                  DS_DSD_RADIO_MASK_WCDMA|
                  DS_DSD_RADIO_MASK_WLAN;
     break;

   case CM_MODE_PREF_ANY_BUT_HDR_WLAN_LTE:
     radio_mask = DS_DSD_RADIO_MASK_1X |
                  DS_DSD_RADIO_MASK_GSM |
                  DS_DSD_RADIO_MASK_TDS |
                  DS_DSD_RADIO_MASK_WCDMA;
     break;

   case CM_MODE_PREF_ANY_BUT_LTE_TDS:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_HDR |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA|
                   DS_DSD_RADIO_MASK_WLAN;
      break;

   case CM_MODE_PREF_ANY_BUT_HDR_LTE_TDS:
      radio_mask = DS_DSD_RADIO_MASK_1X |
                   DS_DSD_RADIO_MASK_GSM |
                   DS_DSD_RADIO_MASK_WCDMA|
                   DS_DSD_RADIO_MASK_WLAN;
      break;

    case CM_MODE_PREF_AUTOMATIC:
    default:
      #ifdef FEATURE_DATA_LTE
      radio_mask |= DS_DSD_RADIO_MASK_LTE;
      #endif /*FEATURE_DATA_LTE*/

      #ifdef FEATURE_HDR
      radio_mask |= DS_DSD_RADIO_MASK_HDR;
      #endif /*FEATURE_HDR*/

      #ifdef FEATURE_CDMA
      radio_mask |= DS_DSD_RADIO_MASK_1X;
      #endif /*FEATURE_CDMA*/

      #ifdef FEATURE_GSM
      radio_mask |= DS_DSD_RADIO_MASK_GSM;
      #endif /*FEATURE_GSM*/

      #ifdef FEATURE_TDSCDMA
      radio_mask |= DS_DSD_RADIO_MASK_TDS;
      #endif /*FEATURE_TDSCDMA*/

      #ifdef FEATURE_WCDMA
      radio_mask |= DS_DSD_RADIO_MASK_WCDMA;
      #endif /*FEATURE_WCDMA*/

      #ifdef FEATURE_DATA_WLAN_MAPCON
      radio_mask |= DS_DSD_RADIO_MASK_WLAN;
      #endif /*FEATURE_DATA_WLAN_MAPCON*/
      break;
  }

  /*------------------------------------------------------------------------ 
  When pref mode does not have LTE, set LTE radio available if LTE emergency 
  call is in progress; set LTE radio unavailable if LTE emergency call is 
  not in progress.  
  ------------------------------------------------------------------------*/
  if( (!(radio_mask & DS_DSD_RADIO_MASK_LTE)) && 
       lte_emergency_call_in_prog[subs_id]   )
  {
    radio_mask |= DS_DSD_RADIO_MASK_LTE;
  }

  ds_dsd_pref_sys_update_pref_order_on_radio_mask(subs_id, radio_mask);
  return;
}/* ds_dsd_pref_sys_update_pref_order_frm_mode_pref() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_SET_BEST_SYSTEM

DESCRIPTION   This function sets the new best system.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_set_best_system
(
  dsd_radio_type              new_best_system,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  dsd_radio_type              old_pref_system;
  boolean                     pref_system_changed = FALSE;
  boolean                     notify_flag = FALSE;
  boolean                     mode_pref_flag = FALSE;
  dsd_radio_type              i;
  dsd_pref_sys_info_type     *pref_sys_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d",
              subs_id);
    return;
  }
  old_pref_system = pref_sys_info->best_radio;
  mode_pref_flag = pref_sys_info->mode_pref_force_notify;

  DATA_MSG3(MSG_LEGACY_HIGH,
            "ds_dsd_pref_sys_set_best_system: subs_id:%d new_system:%d, "
            "old_system:%d",
            subs_id, new_best_system, old_pref_system);

  /*------------------------------------------------------------------------
    Update database here for preferred system change.
  ------------------------------------------------------------------------*/
  if(new_best_system != pref_sys_info->best_radio)
  {
    pref_sys_info->best_radio = new_best_system;
    pref_system_changed = TRUE;
    if(new_best_system != DS_DSD_RADIO_OOS)
    {
      pref_sys_info->last_best_radio = new_best_system;
    }
  }

  /*------------------------------------------------------------------- 
    If pref system has changed then notify always
  --------------------------------------------------------------------*/
  if(pref_system_changed == TRUE)
  {
    DATA_MSG0(MSG_LEGACY_MED,"Pref sys changed. Notify");
    notify_flag = TRUE;
  }

  /*------------------------------------------------------------------- 
    If force notify flag is set has been set then notify always
  --------------------------------------------------------------------*/
  if((pref_sys_info->mode_pref_force_notify == TRUE) ||
     (pref_sys_info->dds_chg_force_notify == TRUE))
  {
    notify_flag = TRUE;

    /* reset the force notify due to mode preference change flag */
    pref_sys_info->mode_pref_force_notify = FALSE;

    /* reset the force notify due to mode preference change flag */
    pref_sys_info->dds_chg_force_notify = FALSE;

    /* reset current & last preferred system if new best system is OOS  */
    if(new_best_system == DS_DSD_RADIO_INVALID)
    {
      pref_sys_info->best_radio      = DS_DSD_RADIO_INVALID;
       /* Resetting last beat radio only if new mode preference doesnt have this
          Radio in available list */
      if(ds_dsd_pref_sys_get_radio_available_flag(
                                                pref_sys_info->last_best_radio,
                                                subs_id) == FALSE)
      {
        pref_sys_info->last_best_radio = DS_DSD_RADIO_INVALID;
      }
    }
  }

  /*------------------------------------------------------------------- 
    For all radios, check if any parameter has been changed.
    If any radio has changed then Notify. Else do not Notify
  --------------------------------------------------------------------*/
  for(i = DS_DSD_RADIO_MIN; i <= DS_DSD_RADIO_MAX; i++)
  {
    /* reset the mask */
    pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] =0x00;

    if(i != DS_DSD_RADIO_WLAN)
    {
      /* compare srv_status */
      if(TRUE == ds_dsd_common_cmp_radio_info(i,
                                              DS_DSD_RADIO_INFO_SRV_STATUS,
                                              subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_SRV_STATUS_CHANGE;
        notify_flag = TRUE;
      }

      /* compare srv_domain */
      if(TRUE == ds_dsd_common_cmp_radio_info(i,
                                              DS_DSD_RADIO_INFO_SRV_DOMAIN,
                                              subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_SRV_DOMAIN_CHANGE;
        notify_flag = TRUE;
      }

      /* compare rat mask */
      if(TRUE == ds_dsd_common_cmp_radio_info(i,
                                              DS_DSD_RADIO_INFO_RAT_MASK,
                                              subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_RAT_CHANGE;
        notify_flag = TRUE;
      }

      /* compare so mask */
      if(TRUE == ds_dsd_common_cmp_radio_info(i,
                                              DS_DSD_RADIO_INFO_SO_MASK,
                                              subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_SO_CHANGE;
        notify_flag = TRUE;
      }

      /* compare cell_srv indication */
      if(TRUE == ds_dsd_common_cmp_radio_info(i,
                                              DS_DSD_RADIO_INFO_CELL_SRV_IND,
                                              subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_CELL_SRV_CHANGE;
        notify_flag = TRUE;
      }

      /* compare sm throttle indication */
      if(TRUE == ds_dsd_common_cmp_radio_info(i,
                                              DS_DSD_RADIO_INFO_SM_THROTTLE_IND,
                                              subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_SM_THROTTLE_CHANGE;
        notify_flag = TRUE;
      }

      /* compare roam status */
      if(TRUE == ds_dsd_common_cmp_radio_info(i,
                                              DS_DSD_RADIO_INFO_ROAM_STATUS,
                                              subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_ROAM_STATUS_CHANGE;
        notify_flag = TRUE;
      }
    }

#ifdef FEATURE_DATA_WLAN_MAPCON
    else /* WLAN radio */
    {
      if(TRUE == ds_dsd_common_cmp_wlan_info(subs_id))
      {
        pref_sys_info->changed_radio_mask[i-DS_DSD_RADIO_MIN] |=
          NOTIFY_REASON_WLAN_CHANGE;
        notify_flag = TRUE;
      }
    }
#endif /*FEATURE_DATA_WLAN_MAPCON*/
  }

  if(notify_flag == TRUE)
  {
    notify_func(old_pref_system, new_best_system,
                                     mode_pref_flag, subs_id);
  }

  return;
}/* ds_dsd_pref_sys_set_best_system() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_REORDER_PREF_ORDER_LIST

DESCRIPTION   This function resets the preferred order list.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Resets the preferred order list to default values.
===========================================================================*/
void ds_dsd_pref_sys_reorder_pref_order_list
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  cm_mode_pref_e_type         mode_pref
)
{
  uint8                     i, j;
  uint8                     priority_index;
  dsd_pref_sys_order_type   temp = {0};
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  dsd_pref_sys_info_type   *pref_sys_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d",
              subs_id);
    return;
  }
  pref_order_list = pref_sys_info->pref_ordered_list;

  memset(pref_order_list, 0, sizeof(dsd_pref_sys_order_type)*DS_DSD_MAX_RADIO);
  /*------------------------------------------------------------------------
  Preferred system list. This is hardcoded order.
  !!! Do not change the order
  ------------------------------------------------------------------------*/
  pref_order_list[0].radio_type = DS_DSD_RADIO_LTE;
  pref_order_list[0].priority   = (uint8)1;
  pref_order_list[0].available  = TRUE;
  strlcpy(pref_order_list[0].tech_name_string,
          "LTE",
          sizeof(pref_order_list[0].tech_name_string));

  pref_order_list[1].radio_type = DS_DSD_RADIO_HDR;
  pref_order_list[1].priority   = (uint8)2;
  pref_order_list[1].available  = TRUE;
  strlcpy(pref_order_list[1].tech_name_string,
          "HDR",
          sizeof(pref_order_list[1].tech_name_string));

  pref_order_list[2].radio_type = DS_DSD_RADIO_1X;
  pref_order_list[2].priority   = (uint8)3;
  pref_order_list[2].available  = TRUE;
  strlcpy(pref_order_list[2].tech_name_string,
          "1X",
          sizeof(pref_order_list[2].tech_name_string));

  pref_order_list[3].radio_type = DS_DSD_RADIO_GSM;
  pref_order_list[3].priority   = (uint8)4;
  pref_order_list[3].available  = TRUE;
  strlcpy(pref_order_list[3].tech_name_string,
          "GSM",
          sizeof(pref_order_list[3].tech_name_string));

  pref_order_list[4].radio_type = DS_DSD_RADIO_WCDMA;
  pref_order_list[4].priority   = (uint8)5;
  pref_order_list[4].available  = TRUE;
  strlcpy(pref_order_list[4].tech_name_string,
          "WCDMA",
          sizeof(pref_order_list[4].tech_name_string));

  pref_order_list[5].radio_type = DS_DSD_RADIO_TDSCDMA;
  pref_order_list[5].priority   = (uint8)6;
  pref_order_list[5].available  = TRUE;
  strlcpy(pref_order_list[5].tech_name_string,
          "TDSCDMA",
          sizeof(pref_order_list[5].tech_name_string));


  pref_order_list[6].radio_type = DS_DSD_RADIO_WLAN;
  pref_order_list[6].priority   = (uint8)7;
  pref_order_list[6].available  = TRUE;
  strlcpy(pref_order_list[6].tech_name_string,
          "WLAN",
          sizeof(pref_order_list[6].tech_name_string));

  DATA_MSG0(MSG_LEGACY_MED,".......... HARD CODED PRIORITY ORDER.........");
  for(i = 0; i<DS_DSD_MAX_RADIO; i++)
  {
    DATA_MSG_SPRINTF_3(MSG_LEGACY_MED,
                       "RADIO : %s, PRIORITY: %d, AVAILABLE: %d",
                       pref_order_list[i].tech_name_string,
                       pref_order_list[i].priority,
                       pref_order_list[i].available);
  }

  /*------------------------------------------------------------------------
   Update the priorities of the radios in pref order list based on feature
   definitions. If a feature is not defined its priority will be set to MAX
   and it wont be considered as pref system.
  ------------------------------------------------------------------------*/
  ds_dsd_pref_sys_update_pref_order_on_feature(subs_id);

  /*------------------------------------------------------------------------ 
   Update the priorities of the radios using mode preference
  ------------------------------------------------------------------------*/
  ds_dsd_pref_sys_update_pref_order_frm_mode_pref(subs_id, mode_pref);
  
  DATA_MSG0(MSG_LEGACY_MED,"PRIORITY ORDER BEFORE SORTING....");
  for(i = 0; i<DS_DSD_MAX_RADIO; i++)
  {
    DATA_MSG_SPRINTF_3(MSG_LEGACY_MED,
                       "RADIO : %s, PRIORITY: %d, AVAILABLE: %d",
                       pref_order_list[i].tech_name_string,
                       pref_order_list[i].priority,
                       pref_order_list[i].available);

  }

  /*------------------------------------------------------------------------
  Use selection sort algorithm to sort the radios based on priority order.
  Since there are very few elements, the sorting performance is of no
  significance.
  ------------------------------------------------------------------------*/
  for(i= 0; i<DS_DSD_MAX_RADIO; i++ )
  {
    priority_index = i;
    for(j=i+1; j<DS_DSD_MAX_RADIO; j++)
    {
      if(pref_order_list[j].priority <
         pref_order_list[priority_index].priority)
      {
        priority_index = j;
      }
    }
    if(priority_index != i)
    {
      memscpy(&temp,
              sizeof(dsd_pref_sys_order_type),
              &pref_order_list[priority_index],
              sizeof(dsd_pref_sys_order_type));

      memscpy(&pref_order_list[priority_index],
              sizeof(dsd_pref_sys_order_type),
              &pref_order_list[i],
              sizeof(dsd_pref_sys_order_type));

      memscpy(&pref_order_list[i],
              sizeof(dsd_pref_sys_order_type),
              &temp,
              sizeof(dsd_pref_sys_order_type));
    }
  }

  DATA_MSG0(MSG_LEGACY_MED,
            "... PRIORITY ORDER AFTER EFS READING AND AFTER SORTING...");
  for(i = 0; i<DS_DSD_MAX_RADIO; i++)
  {
    DATA_MSG_SPRINTF_3(MSG_LEGACY_MED,
                       "RADIO : %s, PRIORITY: %d, AVAILABLE: %d",
                       pref_order_list[i].tech_name_string,
                       pref_order_list[i].priority,
                       pref_order_list[i].available);
  }
  return;
}/* ds_dsd_pref_sys_reorder_pref_order_list() */

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_GET_PREF_ORDER_INDEX

DESCRIPTION   This utility function returns the index in preferred order list  
              for the corresponding radio.

DEPENDENCIES  None.

RETURN VALUE  index.

SIDE EFFECTS  None.
===========================================================================*/
uint16 ds_dsd_pref_sys_get_pref_order_index
(
  dsd_radio_type              dsd_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint16                    indx = 0;
  uint16                    result = 0;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  dsd_pref_sys_info_type   *pref_sys_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d",
              subs_id);
    return DS_DSD_MAX_RADIO;
  }
  pref_order_list = pref_sys_info->pref_ordered_list;

  for(indx = 0; indx<DS_DSD_MAX_RADIO; indx++)
  {
    if(dsd_radio == pref_order_list[indx].radio_type)
    {
      result = indx;
      break;
    }
  }
  return result;
}/* ds_dsd_pref_sys_get_pref_order_index()*/

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_GET_CHANGED_FIELDS_MASK

DESCRIPTION   This function returns the changed fields mask for the radio.

DEPENDENCIES  None.

RETURN VALUE  Changed fields mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_pref_sys_get_changed_fields_mask
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint32                    changed_fields = 0x00;
  dsd_pref_sys_info_type   *pref_sys_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return changed_fields;
  }

  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d",
              subs_id);
    return changed_fields;
  }
  changed_fields = pref_sys_info->changed_radio_mask[radio-DS_DSD_RADIO_MIN];
  return changed_fields;
}/* ds_dsd_pref_sys_get_changed_fields_mask() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_GET_BEST_SYSTEM

DESCRIPTION   This function returns the best system

DEPENDENCIES  None.

RETURN VALUE  DSD best system.

SIDE EFFECTS  None.
===========================================================================*/
dsd_radio_type ds_dsd_pref_sys_get_best_system
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type  *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return DS_DSD_RADIO_OOS;
  }

  return dsd_proc_ptr->pref_sys_info.best_radio;
}/* ds_dsd_pref_sys_get_best_system() */


/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_GET_LAST_BEST_SYSTEM

DESCRIPTION   This function returns the best system.

DEPENDENCIES  None.

RETURN VALUE  DSD last best system.

SIDE EFFECTS  None.
===========================================================================*/
dsd_radio_type ds_dsd_pref_sys_get_last_best_system
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type  *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return DS_DSD_RADIO_OOS;
  }

  return dsd_proc_ptr->pref_sys_info.last_best_radio;
}/* ds_dsd_pref_sys_get_last_best_system() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_GET_PREF_SYSTEM

DESCRIPTION   This function returns the preferred system.

DEPENDENCIES  None.

RETURN VALUE  DSD preferred system.

SIDE EFFECTS  None.
===========================================================================*/
dsd_radio_type ds_dsd_pref_sys_get_pref_system
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  dsd_radio_type            ret_val = DS_DSD_RADIO_OOS;
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return DS_DSD_RADIO_OOS;
  }
  pref_order_list = dsd_proc_ptr->pref_sys_info.pref_ordered_list;

  /*----------------------------------------------------------------------
  Get the lock to make sure that write is not in progress. Else, spin on
  lock.
  -----------------------------------------------------------------------*/
  rex_enter_crit_sect( &ds_dsd_recompute_critical_section );

  if(ds_dsd_pref_sys_get_last_best_system(subs_id) == DS_DSD_RADIO_OOS)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "ds_dsd_last_best_radio = DS_DSD_RADIO_OOS. Return %d",
              pref_order_list[0].radio_type);

    ret_val = pref_order_list[0].radio_type;
  }
  else
  {
    if(dsd_proc_ptr->pref_sys_info.best_radio == DS_DSD_RADIO_OOS)
    {
      ret_val = ds_dsd_pref_sys_get_last_best_system(subs_id);
    }
    else
    {
      ret_val = dsd_proc_ptr->pref_sys_info.best_radio;
    }
  }
  rex_leave_crit_sect( &ds_dsd_recompute_critical_section );
  return ret_val;
}/* ds_dsd_pref_sys_get_pref_system() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_GET_RADIO_AVAILABLE_FLAG

DESCRIPTION   This function returns the availability of the radio for
                        recomputation of best system
 
DEPENDENCIES  None.

RETURN VALUE  available flag.

SIDE EFFECTS  None.
===========================================================================*/
boolean	ds_dsd_pref_sys_get_radio_available_flag
(
   dsd_radio_type              radio,
   ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8							   i;
  dsd_pref_sys_info_type		  *pref_sys_info_p = NULL;
  dsd_pref_sys_order_type		  *pref_ordered_list;
  boolean                          available = FALSE;
  /* -----------------------------------------------------------------  */
  
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_ERROR,"Invalid Radio: %d", radio);
    return available;
  }
  
  pref_sys_info_p = ds_dsd_proc_get_pref_sys_info(subs_id);
  
  if(pref_sys_info_p == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d",
              subs_id);
    return available;	
  }
  
  pref_ordered_list = pref_sys_info_p->pref_ordered_list;
  
  /* looping through the preferred list to get the available flag for particular Radio  */
  for(i = 0; i < DS_DSD_MAX_RADIO; i++)
  {
    if(pref_ordered_list[i].radio_type != radio)
    {
      continue;
    }
    else
    {
      available = pref_ordered_list[i].available;
      break;
    }
  }
  return available;
}/* ds_dsd_pref_sys_get_radio_available_flag */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_RECOMPUTE_BEST_SYSTEM

DESCRIPTION   This function runs the algorithm to compute best system.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_recompute_best_system
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_cmd_type                     *cmd_ptr = NULL;
  ds_dsd_recompute_cmd_info_type  *data_block_ptr = NULL;
  dsd_pref_sys_info_type          *pref_sys_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG0(MSG_LEGACY_LOW, "ds_dsd_pref_sys_recompute_best_system");

  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d",
              subs_id);
    return;
  }

  /*----------------------------------------------------------------------
  Use critical section to ensure that the any query will have to wait
  on this critical section.
  -----------------------------------------------------------------------*/
  rex_enter_crit_sect( &ds_dsd_recompute_critical_section );

  if(pref_sys_info->recompute_in_progress == TRUE)
  {
    DATA_MSG1(MSG_LEGACY_HIGH,
              "Recomputation in progress for subs_id:%d",
              subs_id);

    rex_leave_crit_sect( &ds_dsd_recompute_critical_section );
    return;
  }

  pref_sys_info->recompute_in_progress = TRUE;
  rex_leave_crit_sect( &ds_dsd_recompute_critical_section );

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_recompute_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  data_block_ptr = (ds_dsd_recompute_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  data_block_ptr->subs_id = subs_id;

  /*------------------------------------------------------------------
    Post the command on internal DS queue
  -------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_RECOMPUTE;
  ds_dsd_ext_put_cmd(DS_DSD_INTERNAL_CMD_QUEUE, subs_id, cmd_ptr);

  return ;
}/* ds_dsd_pref_sys_recompute_best_system() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_RECOMPUTE_CMD_PROCESSING

DESCRIPTION   This function runs the algorithm to compute best system.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_recompute_cmd_processing
(
  ds_cmd_type  *cmd_ptr
)
{
  uint8                            i;
  dsd_radio_type                   radio;
  ds_dsd_sm_state_enum_type        radio_state;
  boolean                          radio_sm_throttled;
  boolean                          is_remote;
  ds3gsubsmgr_subs_id_e_type       subs_id;
  dsd_pref_sys_info_type          *pref_sys_info = NULL;
  dsd_pref_sys_order_type         *pref_ordered_list;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "NULL ptr passed");
    return;
  }

  subs_id
    = ((ds_dsd_recompute_cmd_info_type*)(cmd_ptr->cmd_payload_ptr))->subs_id;

  DATA_MSG1(MSG_LEGACY_HIGH,
            "ds_dsd_pref_sys_recompute_cmd_processing: subs_id:%d",
            subs_id);

  pref_sys_info = ds_dsd_proc_get_pref_sys_info(subs_id);
  if(pref_sys_info == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Pref sys info ptr NULL for subs_id:%d",
              subs_id);
    return;
  }
  pref_ordered_list = pref_sys_info->pref_ordered_list;

  rex_enter_crit_sect( &ds_dsd_recompute_critical_section );
  pref_sys_info->recompute_in_progress = FALSE;
  rex_leave_crit_sect( &ds_dsd_recompute_critical_section );

  rex_enter_crit_sect( &ds_dsd_recompute_critical_section );
  for(i = 0; i < DS_DSD_MAX_RADIO; i++)
  {
    /*----------------------------------------------------------------------
      Skip WLAN radio for the best system computation
    -----------------------------------------------------------------------*/
    if ( pref_ordered_list[i].radio_type == DS_DSD_RADIO_WLAN )
    {
      continue;
    }

    /*----------------------------------------------------------------------
      If radio is not locally available (due to mode preference) and
      radio is not remote (applicable for fusion modem), then do not consider
      radio for recomputation of best system.
    -----------------------------------------------------------------------*/
    is_remote = ds_dsd_common_get_remote_flag(pref_ordered_list[i].radio_type,
                                              subs_id);

    if ( (pref_ordered_list[i].available == FALSE) && (is_remote == FALSE) )
    {
      DATA_MSG3(MSG_LEGACY_MED,
                "radio:%d available:%d and remote:%d. Skip for recomputation", 
                pref_ordered_list[i].radio_type,
                pref_ordered_list[i].available,
                is_remote);
      continue;
    }

    radio = pref_ordered_list[i].radio_type;
    radio_state = ds_dsd_common_get_sm_state(radio, subs_id);
    radio_sm_throttled = ds_dsd_common_is_sm_throttled(radio, subs_id);

    if((radio_state != DS_DSD_SM_STATE_INVALID) &&
       (radio_sm_throttled == FALSE) &&
       ((radio_state == DS_DSD_SM_STATE_IN_SVC) || 
        (radio_state == DS_DSD_SM_STATE_OOS_HYST)))
    {
      if(radio_state == DS_DSD_SM_STATE_OOS_HYST)
      {
        ds_dsd_pref_sys_set_best_system(DS_DSD_RADIO_OOS, subs_id);
      }
      else
      {
        ds_dsd_pref_sys_set_best_system(radio, subs_id);
      }
      break;
    }
  }

  if(i == DS_DSD_MAX_RADIO)
  {
    DATA_MSG0(MSG_LEGACY_LOW,"No radio. Last system is valid still");
    ds_dsd_pref_sys_set_best_system(DS_DSD_RADIO_OOS, subs_id);
  }
  rex_leave_crit_sect( &ds_dsd_recompute_critical_section );

  /* Log QXDM preferred radio event */
  ds_dsd_radio_event_gen(subs_id);

  /*--------------------------------------------------------------------
    Decrement DSD's command counter
  --------------------------------------------------------------------*/
  ds_dsd_ext_decr_cmd_counter(subs_id, DS_CMD_DSD_RECOMPUTE);

  return;
}/* ds_dsd_pref_sys_recompute_cmd_processing() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_MODE_PREF_CHG

DESCRIPTION   This function handles the mode pref change.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Updates the preferred order list based on new mode pref.
===========================================================================*/
void ds_dsd_pref_sys_mode_pref_chg
(
  cm_mode_pref_e_type           mode_pref,
  ds3gsubsmgr_subs_id_e_type    subs_id
)
{
  uint8                     i;
  uint8                     timer_id;
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Range check */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id) )
  {
    return;
  }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_pref_system_mode_pref_chg. mode_pref:%d subs_id:%d",
            mode_pref, subs_id);

  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return;
  }
  pref_order_list = dsd_proc_ptr->pref_sys_info.pref_ordered_list;

  /*--------------------------------------------------------------------
    Reset the preferred order list
  --------------------------------------------------------------------*/
  ds_dsd_pref_sys_reorder_pref_order_list(subs_id, mode_pref);

  /*--------------------------------------------------------------------
    Save current UE mode preference
  --------------------------------------------------------------------*/
  pref_mode[subs_id] = mode_pref;

  /*--------------------------------------------------------------------
    Set the force notify due to mode preference change flag
  --------------------------------------------------------------------*/
  dsd_proc_ptr->pref_sys_info.mode_pref_force_notify = TRUE;

  /*--------------------------------------------------------------------
    Check if any timers are running associated with the radio that
    has become unavailable (possibly due to mode pref change)
    If yes, reset it.
  --------------------------------------------------------------------*/
  for(i = 0; i<DS_DSD_MAX_RADIO; i++)
  {
    if( pref_order_list[i].available == FALSE )
    {
      /* Reset OOS timer of the radio if running */
      timer_id = ds_dsd_get_oos_timer_id(pref_order_list[i].radio_type);
      if( timer_id != DS_InvalidTimerID_T0 )
      {
        ds_dsd_reset_timer(timer_id, subs_id);
      }

      /* Reset detach timer of the radio if running */
      timer_id = ds_dsd_get_detach_timer_id(pref_order_list[i].radio_type);
      if( timer_id != DS_InvalidTimerID_T0 )
      {
        ds_dsd_reset_timer(timer_id, subs_id);
      }
    }
  }

  /*--------------------------------------------------------------------
    Recompute the preferred system
  --------------------------------------------------------------------*/
  ds_dsd_pref_sys_recompute_best_system(subs_id);

  return;
}/* ds_dsd_pref_sys_mode_pref_chg() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_LTE_EMERG_CALL_IN_PROG

DESCRIPTION   This function handles indications of LTE emergency call starts 
              and finishes.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_pref_sys_lte_emerg_call_in_prog
(
  boolean                       in_prog,
  ds3gsubsmgr_subs_id_e_type    subs_id
)
{
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_pref_sys_lte_emerg_call_in_prog. in_prog:%d subs_id:%d",
            in_prog, subs_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    ASSERT(0);
  }
 
  /*--------------------------------------------------------------------
  If LTE emergency call is in progress flag does not change, ignore.
  --------------------------------------------------------------------*/
  if( lte_emergency_call_in_prog[subs_id] == in_prog)
  {
    return;
  }
 
  /*--------------------------------------------------------------------
  Save LTE emergency call is in progress flag.
  --------------------------------------------------------------------*/
  lte_emergency_call_in_prog[subs_id] = in_prog;
    
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return;
  }

  /*--------------------------------------------------------------------
    Reset the preferred order list
  --------------------------------------------------------------------*/
  ds_dsd_pref_sys_reorder_pref_order_list(subs_id, pref_mode[subs_id]);

  /*--------------------------------------------------------------------
    Set the force notify due to mode preference change flag
  --------------------------------------------------------------------*/
  dsd_proc_ptr->pref_sys_info.mode_pref_force_notify = TRUE;

  /*--------------------------------------------------------------------
    Recompute the preferred system
  --------------------------------------------------------------------*/
    ds_dsd_pref_sys_recompute_best_system(subs_id);

  return;
}/* ds_dsd_pref_sys_lte_emerg_call_in_prog() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_DDS_CHG

DESCRIPTION   This function handles the DDS change.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_dds_chg
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_MED,
            "ds_dsd_pref_sys_dds_chg. subs_id:%d", subs_id);

  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return;
  }

  /*--------------------------------------------------------------------
    Set the force notify due to DDS change flag
  --------------------------------------------------------------------*/
  dsd_proc_ptr->pref_sys_info.dds_chg_force_notify = TRUE;

  /*--------------------------------------------------------------------
    Recompute the preferred system
  --------------------------------------------------------------------*/
  ds_dsd_pref_sys_recompute_best_system(subs_id);

  return;
}/* ds_dsd_pref_sys_dds_chg() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_REG_NOTIFY

DESCRIPTION   This function registers the notify function.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_reg_notify
(
  ds_dsd_pref_notify_func_type reg_func
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  notify_func = reg_func;
  return;
}/* ds_dsd_pref_sys_reg_notify() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_RESET_PREF_SYS_INFO

DESCRIPTION   This function resets the preferred system info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_reset_pref_sys_info
(
  ds3gsubsmgr_subs_id_e_type    subs_id
)
{
  ds_dsd_proc_type  *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return;
  }

  DATA_MSG1(MSG_LEGACY_MED, "ds_dsd_pref_sys_reset_pref_sys_info subs_id:%d",
            subs_id);

  dsd_proc_ptr->pref_sys_info.best_radio = DS_DSD_RADIO_OOS;
  dsd_proc_ptr->pref_sys_info.last_best_radio = DS_DSD_RADIO_OOS;
  dsd_proc_ptr->pref_sys_info.mode_pref_force_notify = FALSE;
  dsd_proc_ptr->pref_sys_info.dds_chg_force_notify = FALSE;

  memset(&(dsd_proc_ptr->pref_sys_info.changed_radio_mask), 0,
         (sizeof(uint32) * DS_DSD_MAX_RADIO));

  ds_dsd_pref_sys_reorder_pref_order_list(subs_id, CM_MODE_PREF_AUTOMATIC);

  return;
}/* ds_dsd_pref_sys_reset_pref_sys_info() */

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_INIT

DESCRIPTION   This function initializes the preferred system. This is during
              power up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&ds_dsd_recompute_critical_section, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect(&ds_dsd_recompute_critical_section);
  return;
}/* ds_dsd_pref_sys_init() */

