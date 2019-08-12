/*===========================================================================
                     D S _ D S D _ C O M M O N . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c) 2011 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_common.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module 
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "data_msg.h"
#include "ds_dsd_common.h"
#include "ds_dsd_proc.h"
#include "ds_dsd_pref_sys.h"
#include "ds_dsd_wlanSm.h"
#include "ds_dsd_timer.h"
#include "ds_dsd_ext.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/
#define IGNORE_SAME_RADIO_ACQ_IF_ATTACH(subs_id, sm_radio)                    \
          ( (ds_dsd_ext_get_trat(subs_id)) &&                                 \
           !(ds_dsd_common_hysteresis_supported(DS_DSD_RADIO_LTE, sm_radio)) )

#define IGNORE_DIFF_RADIO_ACQ_IF_ATTACH(subs_id, sm_radio, source_radio)      \
          ( (ds_dsd_ext_get_trat(subs_id)) &&                                 \
            (sm_radio == DS_DSD_RADIO_LTE) &&                                 \
           !(ds_dsd_common_hysteresis_supported(sm_radio, source_radio)) )

/*===========================================================================

                      LOCAL FUNCTIONS DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_START_ACQ_HYST_TIMER

DESCRIPTION   Utility to start the acquisition hysteresis timer.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  ACQ hysteresis timer started.
===========================================================================*/
void ds_dsd_common_start_acq_hyst_timer
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW, "ds_dsd_common_start_acq_hyst_timer. Radio %d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      timer_id = DS_LTEAcqHystTimerID_T5;
      break;

    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDRAcqHystTimerID_T3;
      break;

    case DS_DSD_RADIO_1X:
      timer_id = DS_1xAcqHystTimerID_T1;
      break;

    case DS_DSD_RADIO_WLAN:
      timer_id = DS_WLANAcqHystTimerID_T7;
      break;

    case DS_DSD_RADIO_GSM:
    case DS_DSD_RADIO_WCDMA:
    default:
      DATA_MSG0(MSG_LEGACY_LOW,"Not supported");
      break;
  }

  if(timer_id != DS_InvalidTimerID_T0)
  {
    ds_dsd_timer_start_timer(timer_id, subs_id);
  }
  return;
}/* ds_dsd_common_start_acq_hyst_timer() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_STOP_ACQ_HYST_TIMER

DESCRIPTION   Utility to stop Acq hysteresis timer
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  ACQ hysteresis timer stopped.
===========================================================================*/
void ds_dsd_common_stop_acq_hyst_timer
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW, "ds_dsd_common_stop_acq_hyst_timer. Radio %d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      timer_id = DS_LTEAcqHystTimerID_T5;
      break;

    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDRAcqHystTimerID_T3;
      break;

    case DS_DSD_RADIO_1X:
      timer_id = DS_1xAcqHystTimerID_T1;
      break;

    case DS_DSD_RADIO_WLAN:
      timer_id = DS_WLANAcqHystTimerID_T7;
      break;

    case DS_DSD_RADIO_GSM:
    case DS_DSD_RADIO_WCDMA:
    default:
      DATA_MSG0(MSG_LEGACY_LOW,"Not supported");
      break;
  }

  if(timer_id != DS_InvalidTimerID_T0)
  {
    ds_dsd_timer_stop_timer(timer_id, subs_id);
  }
  return;
}/* ds_dsd_common_stop_acq_hyst_timer() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_START_OOS_HYST_TIMER

DESCRIPTION   Utility to start Out of service hysteresis timer.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  OOS hysteresis timer started.
===========================================================================*/
void ds_dsd_common_start_oos_hyst_timer
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW,"ds_dsd_common_start_oos_hyst_timer. Radio %d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      timer_id = DS_LTEOOSHystTimerID_T6;
      break;

    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDROOSHystTimerID_T4;
      break;

    case DS_DSD_RADIO_1X:
      timer_id = DS_1xOOSHystTimerID_T2;
      break;

    case DS_DSD_RADIO_WLAN:
    case DS_DSD_RADIO_GSM:
    case DS_DSD_RADIO_WCDMA:
    default:
      DATA_MSG0(MSG_LEGACY_LOW,"Not supported");
      break;
  }

  if(timer_id != DS_InvalidTimerID_T0)
  {
    ds_dsd_timer_start_timer(timer_id, subs_id);
  }
  return;
}/* ds_dsd_common_start_oos_hyst_timer() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_STOP_OOS_HYST_TIMER

DESCRIPTION   Utility to stop oos hysteresis timer.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  OOS hysteresis timer stopped.
===========================================================================*/
void ds_dsd_common_stop_oos_hyst_timer
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW, "ds_dsd_common_stop_oos_hyst_timer. Radio %d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      timer_id = DS_LTEOOSHystTimerID_T6;
      break;

    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDROOSHystTimerID_T4;
      break;

    case DS_DSD_RADIO_1X:
      timer_id = DS_1xOOSHystTimerID_T2;
      break;

    case DS_DSD_RADIO_WLAN:
    case DS_DSD_RADIO_GSM:
    case DS_DSD_RADIO_WCDMA:
    default:
      DATA_MSG0(MSG_LEGACY_LOW,"Not supported");
      break;
  }

  if(timer_id != DS_InvalidTimerID_T0)
  {
    ds_dsd_timer_stop_timer(timer_id, subs_id);
  }
  return;
}/* ds_dsd_common_stop_oos_hyst_timer() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_START_HAND_DOWN_TIMER

DESCRIPTION   Utility to start hand down timer_id.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Hand down timer running.
===========================================================================*/
void ds_dsd_common_start_hand_down_timer
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW, "ds_dsd_common_start_hand_down_timer. Radio %d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDRHandDownTimerID_T9;
      break;
    default:
      DATA_MSG0(MSG_LEGACY_LOW,"Not supported");
      break;
  }

  if(timer_id != DS_InvalidTimerID_T0)
  {
    ds_dsd_timer_start_timer(timer_id, subs_id);
  }
  return;
}/* ds_dsd_common_start_hand_down_timer() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_STOP_HAND_DOWN_TIMER

DESCRIPTION   Utility to sotp hand down timer.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Hand down timer stops.
===========================================================================*/
void ds_dsd_common_stop_hand_down_timer
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW, "ds_dsd_common_stop_hand_down_timer. Radio %d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDRHandDownTimerID_T9;
      break;
    default:
      DATA_MSG0(MSG_LEGACY_LOW,"Not supported");
      break;
  }

  if(timer_id != DS_InvalidTimerID_T0)
  {
    ds_dsd_timer_stop_timer(timer_id, subs_id);
  }
  return;
}/* ds_dsd_common_stop_hand_down_timer() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_HANDLE_NW_DETACH_HYST_TIMER

DESCRIPTION   Utility to start or stop the network detach hysteresis timer.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Network detach hysteresis timer started.
===========================================================================*/
void ds_dsd_common_handle_nw_detach_hyst_timer
(
  boolean                     start_flag,
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds_dsd_common_handle_nw_detach_hyst_timer start_flag:%d Radio:%d",
            start_flag,radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      timer_id = DS_LTENwDetachHystTimerID_T12;
      break;

    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDRNwDetachHystTimerID_T11;
      break;

    default:
      DATA_MSG0(MSG_LEGACY_LOW,"Not supported");
      break;
  }

  if(timer_id != DS_InvalidTimerID_T0)
  {
    if(start_flag)
    {
      ds_dsd_timer_start_timer(timer_id, subs_id);
    }
    else
    {
      ds_dsd_timer_stop_timer(timer_id, subs_id);
    }
  }
  return;
}/* ds_dsd_common_handle_nw_detach_hyst_timer() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_RADIO_LESS_PREF

DESCRIPTION   This is a utility to find if priority of the radio.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if target radio is less preferred than compared radio.
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_radio_less_pref
(
  dsd_radio_type              target_radio,
  dsd_radio_type              compared_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8                     target_radio_index = DS_DSD_MAX_RADIO;
  uint8                     compared_radio_index = DS_DSD_MAX_RADIO;
  boolean                   ret_val = FALSE;
  uint8                     i;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW, "ds_dsd_common_radio_less_pref: radio: %d",
            target_radio,compared_radio);

  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return FALSE;
  }
  pref_order_list = dsd_proc_ptr->pref_sys_info.pref_ordered_list;

  for(i=0; i<DS_DSD_MAX_RADIO; i++)
  { 
    if(pref_order_list[i].radio_type == target_radio)
    {
      target_radio_index = i;
      break;
    }
  }

  for(i=0; i<DS_DSD_MAX_RADIO; i++)
  {
    if(pref_order_list[i].radio_type == compared_radio)
    {
      compared_radio_index = i;
      break;
    }
  }

  if(target_radio_index >= compared_radio_index)
  {
    ret_val = TRUE;
    DATA_MSG2(MSG_LEGACY_LOW,
              "target_radio:%d: less pref than compared_radio :%d", 
              target_radio,compared_radio);
  }
  else
  {
    DATA_MSG2(MSG_LEGACY_LOW,
              "target_radio:%d: more pref than compared_radio :%d", 
              target_radio,compared_radio);
  }
  return ret_val;
}/* ds_dsd_common_radio_less_pref() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_HYSTERESIS_SUPPORTED

DESCRIPTION   This is a utility function and has a table of hysteris sypported 
              between two different radio. This is static configuration and
              varies with RF configuration.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if two supplied radios support hysteresis.
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_hysteresis_supported
(
  dsd_radio_type radio1,
  dsd_radio_type radio2
)
{
  boolean hysteresis_supported = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds_dsd_common_hysteresis_supported: radio1:%d, radio2:%d",
            radio1,radio2);

  switch(radio1)
  {
    case DS_DSD_RADIO_LTE:
      switch(radio2)
      {
        case DS_DSD_RADIO_LTE:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_HDR:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_1X:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_GSM:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WCDMA:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WLAN:
          hysteresis_supported = TRUE;
          break;
        default:
          hysteresis_supported = FALSE;
          break;
      }
      break;

    case DS_DSD_RADIO_HDR:
      switch(radio2)
      {
        case DS_DSD_RADIO_LTE:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_HDR:
          hysteresis_supported = TRUE; 
          break;
        case DS_DSD_RADIO_1X:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_GSM:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WCDMA:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WLAN:
          hysteresis_supported = TRUE;
          break;
        default:
          hysteresis_supported = FALSE;
          break;
      }
      break;

    case DS_DSD_RADIO_1X:
      switch(radio2)
      {
        case DS_DSD_RADIO_LTE:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_HDR:
          hysteresis_supported = TRUE; 
          break;
        case DS_DSD_RADIO_1X:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_GSM:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WCDMA:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WLAN:
          hysteresis_supported = TRUE;
          break;
        default:
          hysteresis_supported = FALSE;
          break;
      }
      break;

    case DS_DSD_RADIO_GSM:
      switch(radio2)
      {
        case DS_DSD_RADIO_LTE:
          hysteresis_supported = FALSE; 
          break;
        case DS_DSD_RADIO_HDR:
          hysteresis_supported = FALSE; 
          break;
        case DS_DSD_RADIO_1X:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_GSM:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_WCDMA:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WLAN:
          hysteresis_supported = TRUE;
          break;
        default:
          hysteresis_supported = FALSE;
          break;
      }
      break;

    case DS_DSD_RADIO_WCDMA:
      switch(radio2)
      {
        case DS_DSD_RADIO_LTE:
          hysteresis_supported = FALSE; 
          break;
        case DS_DSD_RADIO_HDR:
          hysteresis_supported = FALSE; 
          break;
        case DS_DSD_RADIO_1X:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_GSM:
          hysteresis_supported = FALSE;
          break;
        case DS_DSD_RADIO_WCDMA:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_WLAN:
          hysteresis_supported = TRUE;
          break;
        default:
          hysteresis_supported = FALSE;
          break;
      }
      break;

    case DS_DSD_RADIO_WLAN:
      switch(radio2)
      {
        case DS_DSD_RADIO_LTE:
          hysteresis_supported = TRUE; 
          break;
        case DS_DSD_RADIO_HDR:
          hysteresis_supported = TRUE; 
          break;
        case DS_DSD_RADIO_1X:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_GSM:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_WCDMA:
          hysteresis_supported = TRUE;
          break;
        case DS_DSD_RADIO_WLAN:
          hysteresis_supported = TRUE;
          break;
        default:
          hysteresis_supported = FALSE;
          break;
      }
      break;

    default:
      break;
  }
  return hysteresis_supported;
}/* ds_dsd_common_hysteresis_supported()*/


/*===========================================================================
FUNCTION      DS_DSD_COMMON_HYSTERESIS_SUPPORTED_TO_ANY_RADIO

DESCRIPTION   This utility finds if the supplied Radio supports hysteresis 
              with any radio in the modem. The radio do not have to be in
              available state.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if there is any radio to which radio supports hysteresis
              FALSE otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_hysteresis_supported_to_any_radio
(
  dsd_radio_type              acq_radio, 
  dsd_radio_type             *high_priority_hysteresis_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                   ret_val = FALSE;
  uint8                     i;
  dsd_radio_type            dsd_radio;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds_dsd_common_hysteresis_supported_to_any_radio: acq_radio: %d",
            acq_radio);

  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL || high_priority_hysteresis_radio == NULL)
  {
    DATA_MSG2(MSG_LEGACY_ERROR,"dsd proc ptr: %d, prio hysteresis ptr: %d,",
                              dsd_proc_ptr,high_priority_hysteresis_radio);
    return FALSE;
  }
  pref_order_list = dsd_proc_ptr->pref_sys_info.pref_ordered_list;

  for(i= 0; i<DS_DSD_MAX_RADIO; i++)
  { 
    if(acq_radio == pref_order_list[i].radio_type)
    {
      continue;
    }
    dsd_radio = pref_order_list[i].radio_type;
    {
      if(ds_dsd_common_hysteresis_supported(dsd_radio, acq_radio) == TRUE)
      {
        ret_val = TRUE; 
        *high_priority_hysteresis_radio = dsd_radio;
        break;
      }
    }
  }

  if(i == DS_DSD_MAX_RADIO)
  {
    ret_val = FALSE;
  }
  return ret_val;
}/* ds_dsd_common_hysteresis_supported_to_any_radio() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_HYSTERESIS_SUPPORTED_TO_ANY_AVAILABLE_RADIO

DESCRIPTION   This utility finds if the supplied radio supports hysteresis 
              with any available radio at present.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if there is at least one radio which supports hysteresis.
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_hysteresis_supported_to_any_available_radio
(
  dsd_radio_type              acq_radio,
  dsd_radio_type             *high_priority_hysteresis_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                   ret_val = FALSE;
  uint8                     i;
  dsd_radio_type            dsd_radio;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds_dsd_common_hysteresis_supported_to_any_available_radio: "
            "acq_radio: %d",
            acq_radio);

  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return FALSE;
  }
  pref_order_list = dsd_proc_ptr->pref_sys_info.pref_ordered_list;

  if(high_priority_hysteresis_radio == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Not supplied memory");
    return FALSE;
  }

  for(i= 0; i<DS_DSD_MAX_RADIO; i++)
  { 
    if(acq_radio == pref_order_list[i].radio_type)
    {
      continue;
    }
    dsd_radio = pref_order_list[i].radio_type;
    {
      if(ds_dsd_common_hysteresis_supported(dsd_radio, acq_radio) == TRUE)
      {
        ret_val = TRUE; 
        *high_priority_hysteresis_radio = dsd_radio;
        break;
      }
    }
  }

  if(i == DS_DSD_MAX_RADIO)
  {
    ret_val = FALSE;
  }
  return ret_val;
}/* ds_dsd_common_hysteresis_supported_to_any_available_radio() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_STABILIZATION_REQ

DESCRIPTION   This is radio configuration definition to see if the stabilization
              is required to this radio.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if Stabilization required.
              FALSE otherwise.

SIDE EFFECTS  None
===========================================================================*/
boolean ds_dsd_common_stabilization_req
(
  dsd_radio_type              acq_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW, "ds_dsd_common_stabilization_req: acq_radio: %d",
            acq_radio);

  if(acq_radio == DS_DSD_RADIO_HDR)
  {
    /*------------------------------------------------------------------------
    If the state is OOS_HYST, there is no need to go through stabilization. 
    If the system is lost again, it will come back to the same state. So, the 
    data system broadcasted is still HDR.
    ------------------------------------------------------------------------*/
    if(ds_dsd_common_get_sm_state(acq_radio, subs_id) == DS_DSD_SM_STATE_OOS_HYST)
    {
      DATA_MSG0(MSG_LEGACY_LOW,"Stabilization NOT Required");
      ret_val = FALSE;
    }
    else
    {
      DATA_MSG0(MSG_LEGACY_LOW,"Stabilization Required");
      ret_val = TRUE;
    }
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Stabilization NOT Required for Radio: %d",
              acq_radio);
  }
  return ret_val;
}/* ds_dsd_common_stabilization_req()*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_SHOULD_GO_ACQ_HYSTERESIS

DESCRIPTION   This is an algorithm to see if the acquired radio should go 
              through acquisition hysteresis.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if should go through Acquisition hysteresis 
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_should_go_acq_hysteresis
(
  dsd_radio_type              acq_radio,
  boolean                    *hand_up,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean         ret_val = TRUE;
  dsd_radio_type  high_priority_hysteresis_radio  = DS_DSD_RADIO_INVALID;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds_dsd_common_should_go_acq_hysteresis: acq_radio:%d",
            acq_radio);

  if(ds_dsd_pref_sys_get_best_system(subs_id) == DS_DSD_RADIO_OOS)
  {
    if(ds_dsd_pref_sys_get_last_best_system(subs_id) == DS_DSD_RADIO_OOS)
    {
      DATA_MSG0(MSG_LEGACY_LOW,"Initial..");
      if(TRUE == ds_dsd_common_hysteresis_supported_to_any_radio(
                   acq_radio, &high_priority_hysteresis_radio, subs_id))
      {
        DATA_MSG1(MSG_LEGACY_LOW,"Hysteresis supported highest system = %d", 
                  high_priority_hysteresis_radio);
        if(TRUE == ds_dsd_common_radio_less_pref(
                     acq_radio, high_priority_hysteresis_radio, subs_id))
        {
          ret_val = TRUE;
        }
        else
        {
          ret_val = FALSE;
        }
      }
      else
      {
        ret_val = FALSE;
      }
    }
    else
    {
      if(TRUE == ds_dsd_common_hysteresis_supported(
                   acq_radio, ds_dsd_pref_sys_get_last_best_system(subs_id)))
      {

        if(acq_radio == ds_dsd_pref_sys_get_last_best_system(subs_id))
        {
          ret_val = FALSE;
        }
        else if(FALSE == ds_dsd_common_radio_less_pref(
                           acq_radio, 
                           ds_dsd_pref_sys_get_last_best_system(subs_id),
                           subs_id))
        {
          if(ds_dsd_common_stabilization_req(acq_radio, subs_id) == TRUE)
          {
            ret_val = TRUE;
          }
          else
          {
            ret_val = FALSE;
          }
        }
        else
        {
          ret_val = TRUE;
        }
      }
      else
      {
        if(TRUE == ds_dsd_common_hysteresis_supported_to_any_radio(
                     acq_radio, &high_priority_hysteresis_radio, subs_id))
        {
          if(TRUE == ds_dsd_common_radio_less_pref(acq_radio,
                                                   high_priority_hysteresis_radio,
                                                   subs_id))
          {
            ret_val = TRUE;
          }
          else
          {
            ret_val = FALSE;
          }
        }
        else
        {
          ret_val = FALSE;
        }
      }
    }
  }
  else
  {
    if(ds_dsd_common_hysteresis_supported(
         acq_radio, ds_dsd_pref_sys_get_best_system(subs_id)) == TRUE)
    {
      if(acq_radio == ds_dsd_pref_sys_get_best_system(subs_id))
      {
        ret_val = FALSE;
      }
      else if(TRUE == ds_dsd_common_radio_less_pref(
                        acq_radio,
                        ds_dsd_pref_sys_get_best_system(subs_id), subs_id))
      {
        ret_val = TRUE;
      }
      else
      {
        if(ds_dsd_common_stabilization_req(acq_radio, subs_id) == TRUE)
        {
          ret_val = TRUE;
        }
        else
        {
          ret_val = FALSE;
        }
      }
    }
    else
    {
      if(TRUE == ds_dsd_common_hysteresis_supported_to_any_radio(
                   acq_radio, &high_priority_hysteresis_radio, subs_id))
      {
        if(TRUE == ds_dsd_common_radio_less_pref(
                     acq_radio, high_priority_hysteresis_radio, subs_id))
        {
          ret_val = TRUE;
        }
        else
        {
          ret_val = FALSE;
        }
      }
      else
      {
        ret_val = FALSE;
      }
    }
  }

  if(ret_val == TRUE)
  {
    DATA_MSG0(MSG_LEGACY_LOW,".............. YES.....");
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_LOW,".............. NO......");
  }
  return ret_val;
}/* ds_dsd_common_should_go_acq_hysteresis() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_SHOULD_GO_OOS_HYSTERESIS

DESCRIPTION   This is a utility function to test if the Radio should go
              through hysteresis.
 
DEPENDENCIES  TRUE if radio supports OOS hystereis.
              FALSE otherwise.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_should_go_oos_hysteresis
(
  dsd_radio_type              oos_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                   ret_val = FALSE;
  dsd_radio_type            high_priority_hysteresis_radio = DS_DSD_RADIO_INVALID;
  uint16                    radio_index;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds_dsd_common_should_go_oos_hysteresis: oos_radio: %d",
            oos_radio);

  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return FALSE;
  }
  pref_order_list = dsd_proc_ptr->pref_sys_info.pref_ordered_list;

  radio_index = ds_dsd_pref_sys_get_pref_order_index(oos_radio, subs_id);

  if( pref_order_list[radio_index].available == FALSE )
  {
    ret_val = FALSE;
  }
  else if(ds_dsd_common_hysteresis_supported_to_any_available_radio(
      oos_radio, &high_priority_hysteresis_radio, subs_id) == TRUE)
  {
    // Comment this, should not affect other radio
    if(high_priority_hysteresis_radio == DS_DSD_RADIO_WLAN)
    {
      DATA_MSG0(MSG_LEGACY_MED,"High priority available radio is WLAN, "
                "do not go to OOS hysteresis");
      ret_val = FALSE;
    }
    else if(TRUE == ds_dsd_common_radio_less_pref(
                      oos_radio, high_priority_hysteresis_radio, subs_id))
    {
      ret_val = FALSE;
    }
    else
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
}/* ds_dsd_common_should_go_oos_hysteresis() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_DCTM_THROTTLE_SUPPORTED

DESCRIPTION   Utility function to find if DCTM throttle supported.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE is supported.
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_dctm_throttle_supported
(
  dsd_radio_type radio
)
{
  boolean   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds_dsd_common_dctm_throttle_supported: radio:%d, aux radio:%d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_HDR:
      ret_val = TRUE;
      break;
    case DS_DSD_RADIO_1X:
      ret_val = TRUE;
      break;
    case DS_DSD_RADIO_GSM:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WCDMA:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WLAN:
      ret_val = FALSE;
      break;
    default:
      ret_val = FALSE;
      break;
  }
  return ret_val;
}/* ds_dsd_common_dctm_throttle_supported()*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_HANDOFF_THROTTLE_SUPPORTED

DESCRIPTION   Utility function to find if handoff throttle supported.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE is supported.
              FALSE otherwise.

SIDE EFFECTS  NONE
===========================================================================*/
boolean ds_dsd_common_handoff_throttle_supported
(
  dsd_radio_type radio
)
{
  boolean   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_MED,
            "ds_dsd_common_handoff_throttle_supported: radio:%d",
            radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE: 
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_HDR:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_1X:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_GSM:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WCDMA:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WLAN:
      ret_val = TRUE;
      break;
    default:
      ret_val = FALSE;
      break;
  }
  return ret_val;
}/* ds_dsd_common_handoff_throttle_supported() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_NW_DETACHED_THROTTLE_SUPPORTED

DESCRIPTION   Utility function to find if throttle supported on Network 
              Attach and detach.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE is supported.
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_nw_detached_throttle_supported
(
  dsd_radio_type radio, 
  dsd_radio_type aux_radio 
)
{
  boolean   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds_dsd_common_nw_detached_throttle_supported: radio:%d, "
            "aux radio:%d",
            radio, aux_radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      ret_val = TRUE;
      break;
    case DS_DSD_RADIO_HDR:
      ret_val = TRUE;
      break;
    case DS_DSD_RADIO_1X:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_GSM:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WCDMA:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WLAN:
      ret_val = FALSE;
      break;
    default:
      ret_val = FALSE;
      break;
  }
  return ret_val;
}/* ds_dsd_common_nw_detached_throttle_supported()*/

/*===========================================================================
FUNCTION      DS_DSD_AVOID_HAND_DOWN_THROTTLE_SUPPORTED

DESCRIPTION   Utility function to find if call connected throttle is 
              supported on this radio. This is configuration dependent
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if supported 
              FALSE otherwise.

SIDE EFFECTS  None
===========================================================================*/
boolean ds_dsd_avoid_hand_down_throttle_supported
(
  dsd_radio_type radio, 
  dsd_radio_type aux_radio 
)
{
  boolean   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,"ds_dsd_avoid_hand_down_throttle_supported: "
            "radio: %d, aux radio: %d",
            radio,
            aux_radio);
  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_HDR:
      if(aux_radio == DS_DSD_RADIO_1X)
      {
        ret_val = TRUE;
      }
      break;
    case DS_DSD_RADIO_1X:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_GSM:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WCDMA:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WLAN:
      ret_val = FALSE;
      break;
    default:
      ret_val = FALSE;
      break;
  }
  return ret_val;
}/* ds_dsd_avoid_hand_down_throttle_supported()*/

/*===========================================================================
FUNCTION      ds_dsd_common_nw_detach_hyst_supported

DESCRIPTION   Utility function to find if NW detach hysteresis throttle is 
              supported on this radio. This is configuration dependent.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if supported.
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_nw_detach_hyst_supported
(
  dsd_radio_type              radio,
  dsd_radio_type              aux_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                   ret_val = FALSE;
  uint16                    lte_index, hdr_index;
  dsd_pref_sys_order_type  *pref_order_list = NULL;
  ds_dsd_proc_type         *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds_dsd_common_nw_detach_hyst_supported: radio: %d, aux radio: %d",
            radio,aux_radio);

  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return FALSE;
  }
  pref_order_list = dsd_proc_ptr->pref_sys_info.pref_ordered_list;

  lte_index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_LTE, subs_id);
  hdr_index = ds_dsd_pref_sys_get_pref_order_index(DS_DSD_RADIO_HDR, subs_id);

  switch(radio)
  {
    case DS_DSD_RADIO_1X:
      if(aux_radio == DS_DSD_RADIO_LTE)
      {
        /* LTE detach hysteresis supported only if LTE is available in
           mode preference */
        if(pref_order_list[lte_index].available == TRUE)
        {
          ret_val = TRUE;
        }
      }
      else if(aux_radio == DS_DSD_RADIO_HDR)
      {
        /* HDR detach hysteresis supported only if HDR is available in
           mode preference */
        if(pref_order_list[hdr_index].available == TRUE)
        {
          ret_val = TRUE;
        }
      }
      break;

    default:
      break;
  }
  return ret_val;
} /* ds_dsd_common_nw_detach_hyst_supported() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_CALL_CONNECTED_THROTTLE_SUPPORTED

DESCRIPTION   Utility function to find if call connected throttle is 
              supported on this radio. This is configuration dependent.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if supported.
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_call_connected_throttle_supported
(
  dsd_radio_type radio, 
  dsd_radio_type aux_radio 
)
{
  boolean   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds_dsd_common_call_connected_throttle_supported: radio:%d, "
            "aux radio:%d",
            radio, aux_radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_HDR:
      if(aux_radio == DS_DSD_RADIO_1X)
      {
        ret_val = TRUE;
      }
      break;
     case DS_DSD_RADIO_1X:
       if(aux_radio == DS_DSD_RADIO_HDR)
       {
         ret_val = TRUE;
       }
      break;
    case DS_DSD_RADIO_GSM:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WCDMA:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WLAN:
      ret_val = FALSE;
      break;
    default:
      ret_val = FALSE;
      break;
  }
  return ret_val;
}/* ds_dsd_common_call_connected_throttle_supported()*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_HAND_DOWN_THROTTLE_SUPPORTED

DESCRIPTION   Utility function to find if throttle is supported between 
              two radios.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE: if supported.
              FALSE: otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_hand_down_throttle_supported
(
  dsd_radio_type radio, 
  dsd_radio_type aux_radio 
)
{
  boolean   ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds_dsd_common_hand_down_throttle_supported: radio: %d, "
            "aux radio: %d",
            radio, aux_radio);

  switch(radio)
  {
    case DS_DSD_RADIO_LTE:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_HDR:
      if(aux_radio == DS_DSD_RADIO_1X)
      {
        ret_val = TRUE;
      }
      break;
    case DS_DSD_RADIO_1X:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_GSM:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WCDMA:
      ret_val = FALSE;
      break;
    case DS_DSD_RADIO_WLAN:
      ret_val = FALSE;
      break;
    default:
      ret_val = FALSE;
      break;
  }
  return ret_val;
}/* ds_dsd_common_hand_down_throttle_supported()*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_SUB_STATE_TRANSITION

DESCRIPTION   This is the transition function for sub state machine
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_common_sub_state_transition
(
  dsd_event_type              new_event,
  ds_dsd_sm_type             *ds_dsd_sm,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              radio,
  dsd_radio_type              aux_radio
)
{
  ds_dsd_sub_sm_state_enum_type     old_state;
  ds_dsd_sub_sm_type      * ds_dsd_sub_sm;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,"Sub state transition:event:%d, radio:%d",
            new_event,radio);

  if(ds_dsd_sm == NULL || ds_dsd_sm->sub_sm_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_MED," ds_dsd_sm : %d ",ds_dsd_sm);
    return;
  }

  ds_dsd_sub_sm = ds_dsd_sm->sub_sm_ptr;
  
  ds_dsd_sub_sm->state_prev = ds_dsd_sub_sm->state;
  old_state = ds_dsd_sub_sm->state;
  switch(new_event)
  {
    /*-----------------------------------------------------------------------
      Handling OOS Hysteresis Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_OOS_HYST_TIMER_EXPIRED_EV:
      /*---------------------------------------------------------------------
       This case is used to stop Handdown timer if running when HDR OOS
       Hysteresis Timer Expires
      ---------------------------------------------------------------------*/
      /* Check if Handdown throttle is set. If set stop handdown timer */
      if((ds_dsd_sub_sm->throttle_register) & (1<< DS_DSD_HAND_DOWN_THROTTLE))
      {
        ds_dsd_sub_sm->throttle_register &= 
          (~(1<<DS_DSD_HAND_DOWN_THROTTLE));
        ds_dsd_common_stop_hand_down_timer(ds_dsd_sm->radio, subs_id);

        if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&&
           (ds_dsd_sub_sm->throttle_register == 0))
        {
          DATA_MSG0(MSG_LEGACY_LOW,
                    "Un-Throttled by: DSD_HAND_DOWN_TIMER_EXPIRED_EV");
          ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
          ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Hand Down Event
    -----------------------------------------------------------------------*/
    case DSD_HAND_DOWN_EV:
      if(ds_dsd_common_hand_down_throttle_supported(radio,aux_radio) 
                                                     == TRUE)
      {
        ds_dsd_sub_sm->throttle_register |= 
          (1<<DS_DSD_HAND_DOWN_THROTTLE);
        if(old_state == DS_DSD_SM_SUB_STATE_UNTHROTTLE)
        {
          DATA_MSG0(MSG_LEGACY_LOW,"Throttled by: DSD_HAND_DOWN_EV");
          ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_THROTTLE;
          ds_dsd_common_start_hand_down_timer(radio, subs_id);
          ds_dsd_pref_sys_recompute_best_system(subs_id);
          ds_dsd_throttle_event_gen(ds_dsd_sm->radio, TRUE);
        }
        else
        {
          DATA_MSG1(MSG_LEGACY_LOW,"Already throttled: 0x%x",
                    ds_dsd_sub_sm->throttle_register);
        }
      }
      else
      {
        DATA_MSG2(MSG_LEGACY_LOW,
                  "Hand Down Throttle not supported: s.radio:%d,t.radio:%d",
                  radio,
                  aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Call Connected Event
    -----------------------------------------------------------------------*/
    case DSD_CALL_CONNECTED_EV:
      if(ds_dsd_sm->radio == radio)
      {
        if(old_state == DS_DSD_SM_SUB_STATE_THROTTLE)
        {
          DATA_MSG0(MSG_LEGACY_LOW,"Un-Throttled by: DSD_CALL_CONNECTED_EV");
          ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
          if((ds_dsd_sub_sm->throttle_register) & 
                  (1<< DS_DSD_HAND_DOWN_THROTTLE))
          {
            ds_dsd_common_stop_hand_down_timer(ds_dsd_sm->radio, subs_id);
          }
          /*------------------------------------------------------------- 
            Reset throttle Register. Throttle due to call connect does not
            need special processing.
          --------------------------------------------------------------*/
          ds_dsd_sub_sm->throttle_register = 0;
          ds_dsd_pref_sys_recompute_best_system(subs_id);
          ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
        }
        else
        {
          DATA_MSG0(MSG_LEGACY_LOW,
                    "Not Throttled. No action on:DSD_CALL_CONNECTED_EV");
        }
      }
      else 
      {
        if(ds_dsd_common_call_connected_throttle_supported(ds_dsd_sm->radio, radio) 
                                                                    == TRUE)
        {
          ds_dsd_sub_sm->throttle_register |= 
            (1<<DS_DSD_CALL_CONNECTED_THROTTLE);
          if(old_state == DS_DSD_SM_SUB_STATE_UNTHROTTLE)
          {
            DATA_MSG0(MSG_LEGACY_LOW,"Throttled by: DSD_CALL_CONNECTED_EV");
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_THROTTLE;
            ds_dsd_pref_sys_recompute_best_system(subs_id);
            ds_dsd_throttle_event_gen(ds_dsd_sm->radio, TRUE);
          }
          else
          {
            DATA_MSG1(MSG_LEGACY_LOW,"Already throttled: 0x%x",
                      ds_dsd_sub_sm->throttle_register);
          }
        }
        else
        {
          DATA_MSG2(MSG_LEGACY_LOW,
                    "Connected Throttle not supported: s.radio:%d,t.radio:%d",
                    radio,
                    aux_radio);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Call Ended Event
    -----------------------------------------------------------------------*/
    case DSD_CALL_ENDED_EV:
      if(TRUE == ds_dsd_common_call_connected_throttle_supported(
                   ds_dsd_sm->radio, radio))
      {
        ds_dsd_sub_sm->throttle_register &= 
          (~(1<<DS_DSD_CALL_CONNECTED_THROTTLE));

        if(TRUE == ds_dsd_avoid_hand_down_throttle_supported(ds_dsd_sm->radio,
                                                             radio))
        {
          ds_dsd_sub_sm->throttle_register |= 
            (1<<DS_DSD_AVOID_HAND_DOWN_THROTTLE);
          ds_dsd_timer_start_timer(DS_HDRHandUpAvoidTimerID_T10, subs_id);
        }
        else
        {
          if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&&
             (ds_dsd_sub_sm->throttle_register == 0))
          {
            DATA_MSG0(MSG_LEGACY_LOW,"Un-Throttled by: DSD_CALL_ENDED_EV");
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
            ds_dsd_pref_sys_recompute_best_system(subs_id);
            ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
          }
          else
          {
            if(ds_dsd_sub_sm->throttle_register != 0)
            {
              DATA_MSG1(MSG_LEGACY_LOW,"Still throttled: 0x%x",
                        ds_dsd_sub_sm->throttle_register);
            }
          }
        }
      }
      else
      {
        DATA_MSG2(MSG_LEGACY_LOW,
                  "Connected Throttle not supported:s.radio:%d,t.radio:%d",
                  radio,
                  aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Hand Down Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_HAND_DOWN_TIMER_EXPIRED_EV:
      ds_dsd_sub_sm->throttle_register &= (~(1<<DS_DSD_HAND_DOWN_THROTTLE));
      if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&&
         (ds_dsd_sub_sm->throttle_register == 0))
      {
        DATA_MSG0(MSG_LEGACY_LOW,
                  "Un-Throttled by: DSD_HAND_DOWN_TIMER_EXPIRED_EV");
        ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
        ds_dsd_pref_sys_recompute_best_system(subs_id);
        ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
      }
      else
      {
        if(ds_dsd_sub_sm->throttle_register != 0)
        {
          DATA_MSG1(MSG_LEGACY_LOW,"Still throttled: 0x%x",
                    ds_dsd_sub_sm->throttle_register);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Avoid Hand Down Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_HAND_DOWN_AVOID_TIMER_EXPIRED_EV:
      ds_dsd_sub_sm->throttle_register &= 
        (~(1<<DS_DSD_AVOID_HAND_DOWN_THROTTLE));
      if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&&
         (ds_dsd_sub_sm->throttle_register == 0))
      {
        DATA_MSG0(MSG_LEGACY_LOW,
                  "Un-Throttled by: DSD_HAND_DOWN_AVOID_TIMER_EXPIRED_EV");
        ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
        if(ds_dsd_sm->state == DS_DSD_SM_STATE_IN_SVC)
        {
          ds_dsd_pref_sys_recompute_best_system(subs_id);
          ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
        }
      }
      else
      {
        if(ds_dsd_sub_sm->throttle_register != 0)
        {
          DATA_MSG1(MSG_LEGACY_LOW,"Still throttled: 0x%x",
                    ds_dsd_sub_sm->throttle_register);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling NW detach Hyst Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_NW_DETACH_HYST_TIMER_EXPIRED_EV: 
      ds_dsd_sub_sm->throttle_register &= 
        (~(1<<DS_DSD_NW_DETACHED_HYST_THROTTLE));

      if(old_state == DS_DSD_SM_SUB_STATE_THROTTLE )
      {
        if(ds_dsd_sub_sm->throttle_register == 0)
        {
          DATA_MSG0(MSG_LEGACY_LOW,
                    "Un-Throttled by: DSD_NW_DETACH_HYST_TIMER_EXPIRED_EV");
          ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
          if(ds_dsd_sm->state == DS_DSD_SM_STATE_IN_SVC)
          {
            ds_dsd_pref_sys_recompute_best_system(subs_id);
            ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
          }
        }
        else
        {
          if(ds_dsd_sub_sm->throttle_register != 0)
          {
            DATA_MSG1(MSG_LEGACY_LOW,"Still throttled: 0x%x",
                      ds_dsd_sub_sm->throttle_register);
          }
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Network Detached Event
    -----------------------------------------------------------------------*/
    case DSD_NW_DETACHED_EV:
      if(ds_dsd_sm->radio == radio)
      {
        if(TRUE == ds_dsd_common_nw_detached_throttle_supported(
                     ds_dsd_sm->radio, radio))
        {
          ds_dsd_sub_sm->throttle_register |= (1<<DS_DSD_NW_DETACHED_THROTTLE);
          if(old_state == DS_DSD_SM_SUB_STATE_UNTHROTTLE)
          {
            DATA_MSG0(MSG_LEGACY_LOW,
                      "Throttled by: DS_DSD_NW_DETACHED_THROTTLE");
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_THROTTLE;
            ds_dsd_pref_sys_recompute_best_system(subs_id);
            ds_dsd_throttle_event_gen(ds_dsd_sm->radio, TRUE);
          }
          else
          {
            DATA_MSG1(MSG_LEGACY_LOW,"Already throttled: 0x%x",
                      ds_dsd_sub_sm->throttle_register);
          }
        }
        else
        {
          DATA_MSG2(MSG_LEGACY_LOW,
                    "NW detach Throttle not supported:s.radio:%d,t.radio:%d",
                    radio,
                    aux_radio);
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Handle LTE attach detach or HDR session close event when 1x SM is
          IN_SVC. 1x SM will be throttled and a hysteresis timer will be
          started.
        -------------------------------------------------------------------*/
        if(TRUE == ds_dsd_common_nw_detach_hyst_supported(ds_dsd_sm->radio,
                                                          radio,
                                                          subs_id))
        {
          /* Throttle only when SM state is in IN_SVC. */
          if(ds_dsd_sm->state == DS_DSD_SM_STATE_IN_SVC)
          {
            DATA_MSG0(MSG_LEGACY_LOW,
                      "Throttled by: DS_DSD_NW_DETACHED_HYST_THROTTLE");
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_THROTTLE;
            ds_dsd_sub_sm->throttle_register |= 
              (1<<DS_DSD_NW_DETACHED_HYST_THROTTLE);
            ds_dsd_common_handle_nw_detach_hyst_timer(TRUE,radio, subs_id);
          }
        }
        else
        {
          DATA_MSG2(MSG_LEGACY_LOW,"NW detach hyst throttle not supported:"
                    "s.radio: %d, t.radio: %d",
                    radio,
                    aux_radio);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling EPS context transfer, NW Attaching and NW Attached Event
    -----------------------------------------------------------------------*/
    case DSD_EPS_CNTXT_TX_EV:
    case DSD_NW_ATTACHED_EV:
    case DSD_NW_ATTACHING_EV:
      if(ds_dsd_sm->radio == radio)
      {
        if(TRUE == ds_dsd_common_nw_detached_throttle_supported(
                     ds_dsd_sm->radio, radio))
        {
          ds_dsd_sub_sm->throttle_register &= 
              (~(1<<DS_DSD_NW_DETACHED_THROTTLE));
          if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&&
             (ds_dsd_sub_sm->throttle_register == 0))
          {
            DATA_MSG0(MSG_LEGACY_LOW,
                      "Un-Throttled by: DS_DSD_NW_DETACHED_THROTTLE");
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;

            /*--------------------------------------------------------------
            As part of UNTHROTTLING event, of sub state machine, see if there 
            is any orthogonal system to move out of IN_SVC and OOS_HYST to OOS.
            ---------------------------------------------------------------*/
            if(ds_dsd_sm->state == DS_DSD_SM_STATE_IN_SVC)
            {
              ds_dsd_pref_sys_recompute_best_system(subs_id);
              if((new_event == DSD_NW_ATTACHED_EV)  ||
                 (new_event == DSD_NW_ATTACHING_EV) ||
                 (new_event == DSD_EPS_CNTXT_TX_EV))
              {
                ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
              }
            }
          }
          else
          {
            if((old_state == DS_DSD_SM_SUB_STATE_UNTHROTTLE )&&
               (ds_dsd_sub_sm->throttle_register == 0))
            {
              DATA_MSG0(MSG_LEGACY_LOW,
                        "Already Un-Throttled by: DS_DSD_NW_DETACHED_THROTTLE");
            }
            if(ds_dsd_sub_sm->throttle_register != 0)
            {
              DATA_MSG1(MSG_LEGACY_LOW,"Still throttled: 0x%x",
                        ds_dsd_sub_sm->throttle_register);
            }
          }
        }
        else
        {
          DATA_MSG2(MSG_LEGACY_LOW,
                    "NW Attach throttle not supported:s.radio: %d, t.radio: %d",
                    radio,
                    aux_radio);
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Handle LTE attach init/attach complete or HDR session open event
          when 1x Nw detach hysteresis timer is running.
        -------------------------------------------------------------------*/
        if(TRUE == ds_dsd_common_nw_detach_hyst_supported(ds_dsd_sm->radio,
                                                          radio,
                                                          subs_id))
        {
          ds_dsd_sub_sm->throttle_register &= 
              (~(1<<DS_DSD_NW_DETACHED_HYST_THROTTLE));
          ds_dsd_common_handle_nw_detach_hyst_timer(FALSE,radio, subs_id);

          if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&&
             (ds_dsd_sub_sm->throttle_register == 0))
          {
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
            ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
          }
        }
        else
        {
          DATA_MSG2(MSG_LEGACY_LOW,"NW detach hyst throttle not supported:"
                    "s.radio: %d, t.radio: %d",
                    radio,
                    aux_radio);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling DCTM Throttle Event
    -----------------------------------------------------------------------*/
    case DSD_DCTM_THROTTLE_EV:
      if(ds_dsd_sm->radio == radio)
      {
        if(ds_dsd_common_dctm_throttle_supported(radio) == TRUE)
        {
          ds_dsd_sub_sm->throttle_register |= (1<<DS_DSD_DCTM_THROTTLE);
          if(old_state == DS_DSD_SM_SUB_STATE_UNTHROTTLE)
          {
            DATA_MSG0(MSG_LEGACY_LOW,"Throttled by: DSD_DCTM_THROTTLE_EV");
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_THROTTLE;
            ds_dsd_pref_sys_recompute_best_system(subs_id);
            ds_dsd_throttle_event_gen(ds_dsd_sm->radio, TRUE);
          }
          else
          {
            DATA_MSG1(MSG_LEGACY_LOW,"Already throttled: 0x%x",
                      ds_dsd_sub_sm->throttle_register);
          }
        }
        else
        {
          DATA_MSG1(MSG_LEGACY_LOW,"Dctm throttle not supported: radio:%d",
                    radio);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling DCTM Unthrottle Event
    -----------------------------------------------------------------------*/
    case DSD_DCTM_UNTHROTTLE_EV:
      if(ds_dsd_sm->radio == radio)
      {
        if(ds_dsd_common_dctm_throttle_supported(radio) == TRUE)
        {
          ds_dsd_sub_sm->throttle_register &= 
              (~(1<<DS_DSD_DCTM_THROTTLE));
          if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&&
             (ds_dsd_sub_sm->throttle_register == 0))
          {
            DATA_MSG0(MSG_LEGACY_LOW,"Un-Throttled by: DSD_DCTM_UNTHROTTLE_EV");
            ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;

            if(ds_dsd_sm->state == DS_DSD_SM_STATE_IN_SVC)
            {
              ds_dsd_pref_sys_recompute_best_system(subs_id);
              ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
            }
          }
          else
          {
            if(ds_dsd_sub_sm->throttle_register != 0)
            {
              DATA_MSG1(MSG_LEGACY_LOW,"Still throttled: 0x%x",
                        ds_dsd_sub_sm->throttle_register);
            }
          }
        }
        else
        {
          DATA_MSG1(MSG_LEGACY_LOW,"Dctm throttle not supported: radio:%d",
                    radio);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Emergency Call Connected Event
    -----------------------------------------------------------------------*/
    case DSD_EMERGENCY_CALL_CONNECTED_EV:
      ds_dsd_sub_sm->throttle_register |= (1<<DS_DSD_EMERGENCY_CALL_THROTTLE);
      if(old_state == DS_DSD_SM_SUB_STATE_UNTHROTTLE)
      {
        DATA_MSG0(MSG_LEGACY_LOW,
                  "Throttled by: DS_DSD_EMERGENCY_CALL_THROTTLE");
        ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_THROTTLE;
        ds_dsd_pref_sys_recompute_best_system(subs_id);
        ds_dsd_throttle_event_gen(ds_dsd_sm->radio, TRUE);
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_LOW,"Already throttled:0x%x",
                  ds_dsd_sub_sm->throttle_register);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Emergency Call Ended Event
    -----------------------------------------------------------------------*/
    case DSD_EMERGENCY_CALL_ENDED_EV:
      ds_dsd_sub_sm->throttle_register 
        &= (~(1<<DS_DSD_EMERGENCY_CALL_THROTTLE));

      if((old_state == DS_DSD_SM_SUB_STATE_THROTTLE )&& 
         (ds_dsd_sub_sm->throttle_register == 0))
      {
        DATA_MSG0(MSG_LEGACY_LOW,
                  "Un-Throttled by: DS_DSD_EMERGENCY_CALL_THROTTLE");
        ds_dsd_sub_sm->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;

        if(ds_dsd_sm->state == DS_DSD_SM_STATE_IN_SVC)
        {
          ds_dsd_pref_sys_recompute_best_system(subs_id);
          ds_dsd_throttle_event_gen(ds_dsd_sm->radio, FALSE);
        }
      }
      else
      {
        if(ds_dsd_sub_sm->throttle_register != 0)
        {
          DATA_MSG1(MSG_LEGACY_LOW,"Still throttled:0x%x",
                    ds_dsd_sub_sm->throttle_register);
        }
      }
      break;

    default:
      DATA_MSG2(MSG_LEGACY_LOW,"Unhandled EV in sub sm : 0x%x, event: %d",
                ds_dsd_sub_sm, 
                new_event);
      break;
  }
  return;
}/* ds_dsd_common_sub_state_transition()*/

/*===========================================================================

                        PUBLIC FUNCTIONS DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_SM_STATE

DESCRIPTION   This function returns the state of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  State of a State Machine.

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_sm_state_enum_type ds_dsd_common_get_sm_state
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_sm_state_enum_type  state = DS_DSD_SM_STATE_INVALID;
  ds_dsd_sm_type            *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return state;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return state;
  }
  state = dsd_radio_sm_instance->state;

  DATA_MSG2(MSG_LEGACY_LOW,"ds_dsd_common_get_sm_state: radio:%d, state:%d",
            radio,state);
  return state;
}/* ds_dsd_common_get_sm_state() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_IS_SM_THROTTLED

DESCRIPTION   Utility to find if the state machine is throttled
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if throttled.
              FALSE otherwise.

SIDE EFFECTS  None
===========================================================================*/
boolean ds_dsd_common_is_sm_throttled
(
  dsd_radio_type              radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                       ret_val = TRUE;
  ds_dsd_sub_sm_state_enum_type throttled = DS_DSD_SM_SUB_STATE_INVALID; 
  ds_dsd_sm_type               *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_MED,"Invalid Radio: %d", radio);
    return TRUE;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return TRUE;
  }

  if(dsd_radio_sm_instance->sub_sm_ptr == NULL)
  {
    return FALSE;
  }

  throttled = dsd_radio_sm_instance->sub_sm_ptr->state;
  if(throttled == DS_DSD_SM_SUB_STATE_THROTTLE )
  {
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }

  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_common_is_sm_throttled: radio:%d throttled:%d",
            radio, ret_val);
  return ret_val;
}/* ds_dsd_common_is_sm_throttled() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_REMOTE_FLAG

DESCRIPTION   This function returns the is_remote of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  Returns SO MASK.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_get_remote_flag
(
  dsd_radio_type                radio,
    ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean          is_remote = FALSE;
  ds_dsd_sm_type  *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return is_remote;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return is_remote;
  }
  is_remote = dsd_radio_sm_instance->is_remote;

  DATA_MSG2(MSG_LEGACY_LOW,"ds_dsd_common_get_remote_flag radio:%d remote:%d",
            radio, is_remote);
  return is_remote;
}/*ds_dsd_common_get_remote_flag()*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_SRV_STATUS

DESCRIPTION   This function returns the srv status of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  Service status.

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_status_e_type ds_dsd_common_get_radio_srv_status
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  sys_srv_status_e_type      srv_status = SYS_SRV_STATUS_NO_SRV;
  ds_dsd_sm_type            *dsd_radio_sm_instance = NULL;
  ds_dsd_sm_state_enum_type  wlan_sm_state;
  boolean                    wlan_throttled;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return srv_status;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return srv_status;
  }

  /* Special handling for WLAN */
  if(radio == DS_DSD_RADIO_WLAN)
  {
    if(!previous)
    {
      wlan_sm_state  = ds_dsd_common_get_sm_state(radio, subs_id);
      wlan_throttled = ds_dsd_common_is_sm_throttled(radio,subs_id);

      if((wlan_throttled == FALSE) &&
         ((wlan_sm_state == DS_DSD_SM_STATE_IN_SVC) ||
          (wlan_sm_state == DS_DSD_SM_STATE_OOS_HYST)))
      {
        srv_status = SYS_SRV_STATUS_SRV;
      }
    }
  }
  else
  {
    if(previous)
    {
      srv_status = dsd_radio_sm_instance->radio_info_prev.srv_status;
    }
    else
    {
      srv_status = dsd_radio_sm_instance->radio_info.srv_status;
    }
  }

  return srv_status;
}/* ds_dsd_common_get_radio_srv_status() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_SRV_DOMAIN

DESCRIPTION   This function returns the service domain of a state machine 
              belonging to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  service domain.

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_domain_e_type ds_dsd_common_get_radio_srv_domain
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  sys_srv_domain_e_type  srv_domain = SYS_SRV_DOMAIN_NO_SRV;
  ds_dsd_sm_type        *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return srv_domain;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return srv_domain;
  }

  if(previous)
  {
    srv_domain = dsd_radio_sm_instance->radio_info_prev.srv_domain;
  }
  else
  {
    srv_domain = dsd_radio_sm_instance->radio_info.srv_domain;
  }

  return srv_domain;
}/* ds_dsd_common_get_radio_srv_domain() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_RAT_MASK

DESCRIPTION   This function returns the rat mask of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  RAT mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_common_get_radio_rat_mask
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint32           rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
  ds_dsd_sm_type  *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return rat_mask;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return rat_mask;
  }

  if(previous)
  {
    rat_mask = dsd_radio_sm_instance->radio_info_prev.rat_mask;
  }
  else
  {
    rat_mask = dsd_radio_sm_instance->radio_info.rat_mask;
  }

  return rat_mask;
}/* ds_dsd_common_get_radio_rat_mask() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_SO_MASK

DESCRIPTION   This function returns the so mask of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  SO mask.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_common_get_radio_so_mask
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint32           so_mask = 0;
  ds_dsd_sm_type  *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return so_mask;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return so_mask;
  }

  if(previous)
  {
    so_mask = dsd_radio_sm_instance->radio_info_prev.so_mask;
  }
  else
  {
    so_mask = dsd_radio_sm_instance->radio_info.so_mask;
  }

  return so_mask;
}/* ds_dsd_common_get_radio_so_mask() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_CELL_SRV_IND

DESCRIPTION   This function returns the cell serv info of a state machine 
              belonging to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  Cell service info.

SIDE EFFECTS  None.
===========================================================================*/
cm_cell_srv_ind_s_type ds_dsd_common_get_radio_cell_srv_ind
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  cm_cell_srv_ind_s_type  cell_srv_ind;
  ds_dsd_sm_type         *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&cell_srv_ind, 0, sizeof(cm_cell_srv_ind_s_type));
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return cell_srv_ind;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return cell_srv_ind;
  }

  if(previous)
  {
    cell_srv_ind = dsd_radio_sm_instance->radio_info_prev.cell_srv_ind;
  }
  else
  {
    cell_srv_ind = dsd_radio_sm_instance->radio_info.cell_srv_ind;
  }

  return cell_srv_ind;
}/* ds_dsd_common_get_radio_cell_srv_ind() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_GET_RADIO_ROAM_STATUS

DESCRIPTION   This function returns the roam status of a state machine belonging 
              to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  roaming status.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_get_radio_roam_status
(
  dsd_radio_type              radio,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean          roaming_status = FALSE;
  ds_dsd_sm_type  *dsd_radio_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((radio == DS_DSD_RADIO_INVALID) || (radio >DS_DSD_RADIO_MAX))
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Invalid Radio: %d", radio);
    return roaming_status;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return roaming_status;
  }

  if(previous)
  {
    roaming_status = dsd_radio_sm_instance->radio_info_prev.roaming_status;
  }
  else
  {
    roaming_status = dsd_radio_sm_instance->radio_info.roaming_status;
  }

  return roaming_status;
}/* ds_dsd_common_get_radio_roam_status() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_CMP_RADIO_INFO

DESCRIPTION   This function compares the previous and current information 
              values of the state machine belonging to the radio system 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:   value changed.
              FALSE:  value unchanged.

SIDE EFFECTS  Once compared current value will be copied to previous value.
===========================================================================*/
boolean ds_dsd_common_cmp_radio_info
(
  dsd_radio_type               radio,
  ds_dsd_radio_info_enum_type  param,
  ds3gsubsmgr_subs_id_e_type   subs_id
)
{
  boolean                 ret_val = FALSE;
  ds_dsd_sm_type         *dsd_radio_sm_instance = NULL;
  ds_dsd_radio_info_type *dsd_radio_info = NULL;
  ds_dsd_radio_info_type *dsd_radio_info_prev = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( ((radio == DS_DSD_RADIO_INVALID) || (radio > DS_DSD_RADIO_MAX)) ||
      ((param < DS_DSD_RADIO_INFO_SRV_STATUS) || (param > DS_DSD_RADIO_INFO_MAX)))
  {
    DATA_MSG2(MSG_LEGACY_ERROR," Radio: %d , param: %d", radio,param);
    return FALSE;
  }

  dsd_radio_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(dsd_radio_sm_instance == NULL)
  {
    return FALSE;
  }
  dsd_radio_info = &(dsd_radio_sm_instance->radio_info);
  dsd_radio_info_prev = &(dsd_radio_sm_instance->radio_info_prev);

  switch(param)
  {
    case DS_DSD_RADIO_INFO_SRV_STATUS:
      if(radio == DS_DSD_RADIO_LTE)
      {
        if(dsd_radio_info_prev->srv_status == dsd_radio_info->srv_status)
        {
          ret_val = FALSE;
          break;
        }
        if((dsd_radio_info_prev->srv_status == SYS_SRV_STATUS_LIMITED &&
            dsd_radio_info->srv_status      == SYS_SRV_STATUS_NO_SRV) ||
           (dsd_radio_info_prev->srv_status == SYS_SRV_STATUS_NO_SRV &&
            dsd_radio_info->srv_status      == SYS_SRV_STATUS_LIMITED) ||
           (dsd_radio_info->srv_status      == SYS_SRV_STATUS_SRV &&
            dsd_radio_info->rat_mask        == PS_SYS_RAT_3GPP_NULL_BEARER)
          )
        {
          ret_val = FALSE;
        }
        else
        {
          ret_val = TRUE;
        }
        /* already compared, copy curr value to prev value */
        dsd_radio_info_prev->srv_status = dsd_radio_info->srv_status;
      }
      else
      {
        if(dsd_radio_info_prev->srv_status != dsd_radio_info->srv_status)
        {
          ret_val = TRUE;
          /* already compared, copy curr value to prev value */
          dsd_radio_info_prev->srv_status = dsd_radio_info->srv_status;
        }
      }
      break;

    case DS_DSD_RADIO_INFO_SRV_DOMAIN:
      if(dsd_radio_info_prev->srv_domain != dsd_radio_info->srv_domain)
      {
        ret_val = TRUE;
        /* already compared, copy curr value to prev value */
        dsd_radio_info_prev->srv_domain = dsd_radio_info->srv_domain;
      }
      break;

    case DS_DSD_RADIO_INFO_RAT_MASK:
      if(dsd_radio_info_prev->rat_mask != dsd_radio_info->rat_mask)
      {
        ret_val = TRUE;
        /* already compared, copy curr value to prev value */
        dsd_radio_info_prev->rat_mask = dsd_radio_info->rat_mask;
      }
      break;

    case DS_DSD_RADIO_INFO_SO_MASK:
      if(dsd_radio_info_prev->so_mask != dsd_radio_info->so_mask)
      {
        ret_val = TRUE;
        /* already compared, copy curr value to prev value */
        dsd_radio_info_prev->so_mask = dsd_radio_info->so_mask;
      }
      break;

    case DS_DSD_RADIO_INFO_CELL_SRV_IND:
      if(0 != memcmp(&(dsd_radio_info_prev->cell_srv_ind),
                     &(dsd_radio_info->cell_srv_ind),
                     sizeof(cm_cell_srv_ind_s_type)))
      {
        ret_val = TRUE;
        /* already compared, copy curr value to prev value */
        memscpy(&(dsd_radio_info_prev->cell_srv_ind),
                sizeof(cm_cell_srv_ind_s_type),
                &(dsd_radio_info->cell_srv_ind),
                sizeof(cm_cell_srv_ind_s_type));
      }
      break;

    case DS_DSD_RADIO_INFO_SM_THROTTLE_IND:
      if(dsd_radio_sm_instance->sub_sm_ptr != NULL)
      {
        if((dsd_radio_sm_instance->sub_sm_ptr->state_prev) 
            != (dsd_radio_sm_instance->sub_sm_ptr->state))
        {
          ret_val = TRUE;
          /* already compared, copy curr value to prev value */
          dsd_radio_sm_instance->sub_sm_ptr->state_prev = 
            dsd_radio_sm_instance->sub_sm_ptr->state;
        }
      }
      break;

    case DS_DSD_RADIO_INFO_ROAM_STATUS:
      if(dsd_radio_info_prev->roaming_status != dsd_radio_info->roaming_status)
      {
        ret_val = TRUE;
        /* already compared, copy curr value to prev value */
        dsd_radio_info_prev->roaming_status = dsd_radio_info->roaming_status;
      }
      break;

    default:
      break;
  }
  return ret_val;
}/* ds_dsd_cmp_radio_srv_status() */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_DSD_COMMON_CMP_WLAN_INFO

DESCRIPTION   This function compares the previous and current information 
              values of the WLAN state machine.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:   value changed.
              FALSE:  value unchanged.

SIDE EFFECTS  Once compared current value will be copied to previous value.
===========================================================================*/
boolean ds_dsd_common_cmp_wlan_info
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                  ret_val = FALSE;
  boolean                  result = FALSE;
  ds_dsd_sm_type          *wlan_sm_instance = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
    Get statemachine pointer
  ---------------------------------------------------------------------*/
  wlan_sm_instance = ds_dsd_proc_get_radio_sm_instance(subs_id, DS_DSD_RADIO_WLAN);
  if(wlan_sm_instance == NULL)
  {
    return FALSE;
  }

  if(wlan_sm_instance->state != wlan_sm_instance->state_prev)
  {
    /*------------------------------------------------------------------------
     If wlan sm state moves to IN_SRV (unthrottled) or OOS, notify;
     If wlan is IN_SRV, but addr change, notify.
    ------------------------------------------------------------------------*/
    if(((wlan_sm_instance->state == DS_DSD_SM_STATE_IN_SVC) &&
        (wlan_sm_instance->sub_sm_ptr->state ==DS_DSD_SM_SUB_STATE_UNTHROTTLE))
       || (wlan_sm_instance->state == DS_DSD_SM_STATE_OOS))
    {
      ret_val = TRUE;
      /* already compared, copy curr value to prev value */
      wlan_sm_instance->state_prev = wlan_sm_instance->state;

      memscpy(&(wlan_sm_instance->radio_info_prev.wlan_addr),
              sizeof(ds_dsd_ext_wlan_addr_type),
              &(wlan_sm_instance->radio_info.wlan_addr),
              sizeof(ds_dsd_ext_wlan_addr_type));
    }
  }
  else
  {
    if((wlan_sm_instance->state == DS_DSD_SM_STATE_IN_SVC) &&
       (wlan_sm_instance->sub_sm_ptr->state ==DS_DSD_SM_SUB_STATE_UNTHROTTLE))
    {
      if(TRUE == ds_dsd_wlansm_compare_address(
                   &(wlan_sm_instance->radio_info_prev.wlan_addr),
                   &(wlan_sm_instance->radio_info.wlan_addr),
                   &result))
      {
        if(result == FALSE )
        {
          ret_val = TRUE;
          /* already compared, copy curr value to prev value */
          memscpy(&(wlan_sm_instance->radio_info_prev.wlan_addr),
                  sizeof(ds_dsd_ext_wlan_addr_type),
                  &(wlan_sm_instance->radio_info.wlan_addr),
                  sizeof(ds_dsd_ext_wlan_addr_type));
        }
      }
    }
  }
  return ret_val;
}/* ds_dsd_common_cmp_wlan_info() */
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_DSD_COMMON_RESET_SM_INSTANCE

DESCRIPTION   This function resets the state-machine's structure to its 
              default value.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_common_reset_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              radio
)
{
  ds_dsd_sm_type   *radio_sm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_MED, "ds_dsd_common_reset_sm_instance Radio:%d", radio);

  radio_sm_ptr = ds_dsd_proc_get_radio_sm_instance(subs_id, radio);
  if(radio_sm_ptr == NULL)
  {
    DATA_MSG2(MSG_LEGACY_ERROR, "radio_sm no present for subs_id: radio:%d",
              subs_id, radio);
    return;
  }

  radio_sm_ptr->radio               = radio;
  radio_sm_ptr->state               = DS_DSD_SM_STATE_OOS;
  radio_sm_ptr->state_prev          = DS_DSD_SM_STATE_OOS;
  radio_sm_ptr->is_remote           = FALSE;

  /* If sub-statemachine is present, initialize */
  if(radio_sm_ptr->sub_sm_ptr != NULL)
  {
    if((radio_sm_ptr->radio == DS_DSD_RADIO_1X) ||
       (radio_sm_ptr->radio == DS_DSD_RADIO_WLAN))
    {
      radio_sm_ptr->sub_sm_ptr->state = DS_DSD_SM_SUB_STATE_UNTHROTTLE;
    }
    else
    {
      radio_sm_ptr->sub_sm_ptr->state = DS_DSD_SM_SUB_STATE_THROTTLE;
    }
  }

  memset(&(radio_sm_ptr->radio_info),0,sizeof(ds_dsd_radio_info_type));
  radio_sm_ptr->radio_info.rat_mask      = PS_SYS_RAT_3GPP_NULL_BEARER;
  radio_sm_ptr->radio_info.roaming_status = FALSE;
  radio_sm_ptr->radio_info.cell_srv_ind.hs_ind = SYS_HS_IND_NONE;
  radio_sm_ptr->radio_info.cell_srv_ind.dtm_supp = SYS_DTM_SUPPORT_NONE;
  radio_sm_ptr->radio_info.cell_srv_ind.egprs_supp = SYS_EGPRS_SUPPORT_NONE;
  radio_sm_ptr->radio_info.cell_srv_ind.hs_call_status = SYS_HS_IND_NONE;

  memset(&(radio_sm_ptr->radio_info_prev),0,sizeof(ds_dsd_radio_info_type));
  radio_sm_ptr->radio_info_prev.rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
  radio_sm_ptr->radio_info_prev.roaming_status = FALSE;
  radio_sm_ptr->radio_info_prev.cell_srv_ind.hs_ind = SYS_HS_IND_NONE;
  radio_sm_ptr->radio_info_prev.cell_srv_ind.dtm_supp = SYS_DTM_SUPPORT_NONE;
  radio_sm_ptr->radio_info_prev.cell_srv_ind.egprs_supp 
    = SYS_EGPRS_SUPPORT_NONE;
  radio_sm_ptr->radio_info_prev.cell_srv_ind.hs_call_status = SYS_HS_IND_NONE;

#ifdef FEATURE_DATA_WLAN_MAPCON
  radio_sm_ptr->radio_info.acq_wqe_support = FALSE;
  radio_sm_ptr->radio_info.lost_wqe_support = FALSE;

  memset(&(radio_sm_ptr->radio_info.wlan_addr), 0,
         sizeof(ds_dsd_ext_wlan_addr_type));

  radio_sm_ptr->radio_info_prev.acq_wqe_support = FALSE;
  radio_sm_ptr->radio_info_prev.lost_wqe_support = FALSE;

  memset(&(radio_sm_ptr->radio_info_prev.wlan_addr), 0,
         sizeof(ds_dsd_ext_wlan_addr_type));
#endif /* FEATURE_DATA_WLAN_MAPCON */

  return;
}/* ds_dsd_common_reset_sm_instance() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_CREATE_SM_INSTANCE

DESCRIPTION   This function creates an instance of DSD statemachine.
 
DEPENDENCIES  None.

RETURN VALUE  If success, return pointer to the created SM instance 
              If failure, return NULL 

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_common_create_sm_instance
(
  boolean  create_sub_sm
)
{
  ds_dsd_sm_type     *sm_ptr = NULL;
  ds_dsd_sub_sm_type *sub_sm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  sm_ptr = (ds_dsd_sm_type*)modem_mem_alloc(sizeof(ds_dsd_sm_type),
                                            MODEM_MEM_CLIENT_DATA);

  if(sm_ptr != NULL)
  {
    memset(sm_ptr, 0 , sizeof(ds_dsd_sm_type));

    /* create sub-statemachine */
    if(create_sub_sm == TRUE)
    {
      sub_sm_ptr 
        = (ds_dsd_sub_sm_type*)modem_mem_alloc(sizeof(ds_dsd_sub_sm_type),
                                               MODEM_MEM_CLIENT_DATA);
      if(sub_sm_ptr == NULL)
      {
        /* free memory allocated for main statemachine */
        modem_mem_free(sm_ptr, MODEM_MEM_CLIENT_DATA);
        sm_ptr = NULL;
      }
      else
      {
        memset(sub_sm_ptr, 0 , sizeof(ds_dsd_sub_sm_type));
        sm_ptr->sub_sm_ptr = sub_sm_ptr;
      }
    }
  }
  return sm_ptr;
}/* ds_dsd_common_create_sm_instance() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_CONVERT_DSD_RADIO_TO_RADIO_SM

DESCRIPTION   This function converts DSD radio to radio SM type
 
DEPENDENCIES  None.

RETURN VALUE  RADIO SM enum

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_radio_sm_enum_type ds_dsd_common_convert_dsd_radio_to_radio_sm
(
  dsd_radio_type  dsd_radio
)
{
  ds_dsd_radio_sm_enum_type radio_sm = DS_DSD_RADIO_SM_COUNT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(dsd_radio)
  {
    case DS_DSD_RADIO_LTE:
      radio_sm = DS_DSD_RADIO_SM_LTE;
      break;

    case DS_DSD_RADIO_HDR:
      radio_sm = DS_DSD_RADIO_SM_HDR;
      break;

    case DS_DSD_RADIO_1X:
      radio_sm = DS_DSD_RADIO_SM_1X;
      break;

    case DS_DSD_RADIO_GSM:
      radio_sm = DS_DSD_RADIO_SM_GSM;
      break;

    case DS_DSD_RADIO_TDSCDMA:
      radio_sm = DS_DSD_RADIO_SM_TDSCDMA;
      break;

    case DS_DSD_RADIO_WCDMA:
      radio_sm = DS_DSD_RADIO_SM_WCDMA;
      break;

    case DS_DSD_RADIO_WLAN:
      radio_sm = DS_DSD_RADIO_SM_WLAN;
      break;

    default:
      radio_sm = DS_DSD_RADIO_SM_COUNT;
      break;
  }
  return radio_sm;
}/* ds_dsd_common_convert_dsd_radio_to_radio_sm() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_VALIDATE_EVENT

DESCRIPTION   This function validates DSD event from event dispatcher
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:  Success
              FALSE: Failure

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_common_validate_event
(
  dsd_event_type              notified_event,
  dsd_event_client_type       client,
  dsd_event_client_type       client_to_compare,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  void                       *aux_info_ptr
)
{
  boolean ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  do
  {
    /*---------------------------------------------------------------------
      Validate Client
    ---------------------------------------------------------------------*/
    if((client != client_to_compare) ||(aux_info_ptr == NULL) )
    {
      DATA_MSG2(MSG_LEGACY_ERROR, "Wrong client:%d , aux info ptr: %d",
                 client,aux_info_ptr);
      ret_val = FALSE;
      break;
    }

    /*---------------------------------------------------------------------
      Validate Event
    ---------------------------------------------------------------------*/
    if((notified_event == DS_DSD_INVALID_EVENT) ||
       (notified_event >  DS_DSD_MAX_EVENT))
    {
      DATA_MSG2(MSG_LEGACY_ERROR, "Invalid event:%d for client:%d", 
                notified_event, client);
      ret_val = FALSE;
      break;
    }

    /*---------------------------------------------------------------------
      Validate DS SUBS ID
    ---------------------------------------------------------------------*/
    if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
    {
      ret_val = FALSE;
      break;
    }
  }while(0);
  return ret_val;
}/* ds_dsd_common_validate_event() */

/*===========================================================================
FUNCTION      DS_DSD_COMMON_STATE_TRANSITION

DESCRIPTION   This is the transition function for the generic state machine.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  The state is moved to new state.
===========================================================================*/
void ds_dsd_common_state_transition
(
  dsd_event_type              new_event,
  ds_dsd_sm_type             *ds_dsd_sm,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              radio,
  dsd_radio_type              aux_radio
)
{
  boolean        hand_up;
  uint8          timer_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_LOW,
            "Main state transition: radio:%d, event: %d",radio,new_event);

  if(ds_dsd_sm == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"NULL ds_dsd_sm");
    return;
  }

  switch(new_event)
  {
    /*-----------------------------------------------------------------------
      Handling Radio Acquired Event
    -----------------------------------------------------------------------*/
    case DSD_ACQ_RADIO_EV:
      if(ds_dsd_sm->radio == radio)
      {
        DATA_MSG1(MSG_LEGACY_LOW,"Same Radio acquired:%d", radio);
        if( IGNORE_SAME_RADIO_ACQ_IF_ATTACH(subs_id, ds_dsd_sm->radio) )
        {
          DATA_MSG0(MSG_LEGACY_HIGH,
                    "Ignore radio acquired since lte attach in progress");
          break;
        }

        switch(ds_dsd_sm->state)
        {
          case DS_DSD_SM_STATE_OOS:
            if(FALSE == ds_dsd_common_should_go_acq_hysteresis(
                          ds_dsd_sm->radio, &hand_up, subs_id))
            {
              DATA_MSG2(MSG_LEGACY_LOW,
                        "Radio %d, 0x%x: DS_DSD_OOS --> DS_DSD_IN_SVC",
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_IN_SVC;

              if(ds_dsd_sm->sub_sm_ptr && 
                 ds_dsd_sm->sub_sm_ptr->state == DS_DSD_SM_SUB_STATE_THROTTLE)
              {
                DATA_MSG0(MSG_LEGACY_LOW,"Do not recompute best system yet");
              }
              else
              {
                ds_dsd_pref_sys_recompute_best_system(subs_id);
              }
            }
            else
            {
              /*------------------------------------------------------------- 
                Look at algorithm
              --------------------------------------------------------------*/
              DATA_MSG2(MSG_LEGACY_LOW,
                        "Radio %d, 0x%x: DS_DSD_OOS --> DS_DSD_ACQ_HYST",
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_ACQ_HYST;
              ds_dsd_common_start_acq_hyst_timer(ds_dsd_sm->radio, subs_id);

              /*------------------------------------------------------------- 
                Also recompute in case of acq hysteresis so radios are
                reported as available when acquired
              --------------------------------------------------------------*/
              ds_dsd_pref_sys_recompute_best_system(subs_id);
            }
            break;

          case DS_DSD_SM_STATE_ACQ_HYST:
            DATA_MSG0(MSG_LEGACY_LOW,"Should not get this event.");
            break;

          case DS_DSD_SM_STATE_IN_SVC:
            DATA_MSG0(MSG_LEGACY_LOW,
                      "Recomputing since parameters might have changed");
            ds_dsd_pref_sys_recompute_best_system(subs_id);
            break;

          case DS_DSD_SM_STATE_OOS_HYST:
            if(FALSE == ds_dsd_common_should_go_acq_hysteresis(
                          ds_dsd_sm->radio, &hand_up, subs_id))
            {  
              DATA_MSG2(MSG_LEGACY_LOW,
                        "Radio %d, 0x%x: DS_DSD_OOS_HYST --> DS_DSD_IN_SVC",
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_IN_SVC;
              ds_dsd_common_stop_oos_hyst_timer(ds_dsd_sm->radio, subs_id);

              if(ds_dsd_sm->sub_sm_ptr && 
                 ds_dsd_sm->sub_sm_ptr->state == DS_DSD_SM_SUB_STATE_THROTTLE)
              {
                DATA_MSG0(MSG_LEGACY_LOW,"Do not recompute best system yet");
              }
              else
              {  
                ds_dsd_pref_sys_recompute_best_system(subs_id);
              }
            }
            else
            {
              DATA_MSG2(MSG_LEGACY_HIGH,
                        "Radio %d, 0x%x: DS_DSD_OOS_HYST --> DS_DSD_ACQ_HYST",
                        ds_dsd_sm->radio,ds_dsd_sm);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_ACQ_HYST;
              ds_dsd_common_stop_oos_hyst_timer(ds_dsd_sm->radio, subs_id);
              ds_dsd_common_start_acq_hyst_timer(ds_dsd_sm->radio, subs_id);
            }
            break;

          default:
            DATA_MSG1(MSG_LEGACY_LOW,"Invalid state %d",ds_dsd_sm->state);
            break;
        }
      }
      else
      {
        DATA_MSG2(MSG_LEGACY_LOW,"Diff Radio acquired:%d, SM Radio: %d ", 
                  radio,
                  ds_dsd_sm->radio);
        if( IGNORE_DIFF_RADIO_ACQ_IF_ATTACH(subs_id, ds_dsd_sm->radio, radio) )
        {
          DATA_MSG0(MSG_LEGACY_HIGH,
                    "Ignoring radio acquired since lte attach in progress");
          break;
        }

        switch(ds_dsd_sm->state)
        {
          case DS_DSD_SM_STATE_OOS:
            DATA_MSG0(MSG_LEGACY_LOW,"TODO: Ignore");
            break;

          case DS_DSD_SM_STATE_ACQ_HYST:
            if(ds_dsd_common_hysteresis_supported(radio, ds_dsd_sm->radio) == FALSE)
            {
              DATA_MSG4(MSG_LEGACY_LOW,
                        "Hysteresis not supported. radio1:%d, SM radio2: %d " 
                        "Radio %d, 0x%x: DS_DSD_ACQ_HYST --> DS_DSD_OOS",
                        radio, 
                        ds_dsd_sm->radio,
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_common_stop_acq_hyst_timer(ds_dsd_sm->radio, subs_id);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_OOS;
              ds_dsd_pref_sys_recompute_best_system(subs_id);
            }
            break;

          case DS_DSD_SM_STATE_IN_SVC:
            if((ds_dsd_common_hysteresis_supported(radio, ds_dsd_sm->radio) == FALSE)&&
               (ds_dsd_common_is_sm_throttled(radio, subs_id) == FALSE))
            {
              DATA_MSG4(MSG_LEGACY_LOW,
                        "Hysteresis not supported. radio1:%d, SM radio2: %d " 
                        "Radio %d, 0x%x: DS_DSD_IN_SVC --> DS_DSD_OOS",
                        radio, 
                        ds_dsd_sm->radio,
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_OOS;
            }
            break;

          case DS_DSD_SM_STATE_OOS_HYST:
            if((ds_dsd_common_hysteresis_supported(radio,ds_dsd_sm->radio) == FALSE) &&
               (ds_dsd_common_is_sm_throttled(radio, subs_id) == FALSE))
            {
              DATA_MSG4(MSG_LEGACY_LOW,
                        "Hysteresis not supported. radio1:%d, SM radio2: %d "
                        "Radio %d, 0x%x: DS_DSD_OOS_HYST --> DS_DSD_OOS",
                        radio, 
                        ds_dsd_sm->radio,
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_common_stop_oos_hyst_timer(ds_dsd_sm->radio, subs_id);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_OOS;
            }
            else
            {
              DATA_MSG2(MSG_LEGACY_LOW,
                        "Hysteresis supported. radio1:%d, SM radio2: %d",
                        radio,
                        ds_dsd_sm->radio);
            }
            break;

          default:
            DATA_MSG1(MSG_LEGACY_LOW,"Invalid state %d",ds_dsd_sm->state);
            break;
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Radio Lost Event
    -----------------------------------------------------------------------*/
    case DSD_OOS_RADIO_EV:
      if(ds_dsd_sm->radio == radio)
      {
        DATA_MSG1(MSG_LEGACY_LOW,"Same Radio lost :%d", radio);
        switch(ds_dsd_sm->state)
        {
          case DS_DSD_SM_STATE_OOS:
            DATA_MSG0(MSG_LEGACY_LOW,"Ignore");
            break;

          case DS_DSD_SM_STATE_ACQ_HYST:
            DATA_MSG2(MSG_LEGACY_LOW,
                      "Radio %d, 0x%x: DS_DSD_ACQ_HYST --> DS_DSD_OOS",
                      ds_dsd_sm->radio,
                      ds_dsd_sm);
            ds_dsd_common_stop_acq_hyst_timer(ds_dsd_sm->radio, subs_id);
            ds_dsd_sm->state_prev = ds_dsd_sm->state;
            ds_dsd_sm->state = DS_DSD_SM_STATE_OOS;
            break;

          case DS_DSD_SM_STATE_IN_SVC:
            if(TRUE == ds_dsd_common_should_go_oos_hysteresis(ds_dsd_sm->radio,
                                                              subs_id))
            {
              DATA_MSG2(MSG_LEGACY_LOW,
                        "Radio %d, 0x%x: DS_DSD_IN_SVC --> DS_DSD_OOS_HYST",
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_common_start_oos_hyst_timer(ds_dsd_sm->radio, subs_id);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_OOS_HYST;
              ds_dsd_pref_sys_recompute_best_system(subs_id);
            }
            else
            {
               DATA_MSG2(MSG_LEGACY_LOW,
                         "Radio %d, 0x%x: DS_DSD_IN_SVC --> DS_DSD_OOS",
                         ds_dsd_sm->radio,
                         ds_dsd_sm);
               ds_dsd_sm->state_prev = ds_dsd_sm->state;
               ds_dsd_sm->state = DS_DSD_SM_STATE_OOS;
               ds_dsd_pref_sys_recompute_best_system(subs_id);
            }
            break;

          case DS_DSD_SM_STATE_OOS_HYST:
            DATA_MSG0(MSG_LEGACY_LOW,
                      "Impossible. Ignore");
            break;

          default:
            DATA_MSG1(MSG_LEGACY_LOW, "Invalid state 0x%x", 
                      ds_dsd_sm->state);
            break;
        }
      }
      else
      {
        DATA_MSG2(MSG_LEGACY_LOW,
                  "Diff Radio lost:%d, SM Radio: %d ",
                  radio,ds_dsd_sm->radio);
        switch(ds_dsd_sm->state)
        {
          case DS_DSD_SM_STATE_OOS:
            DATA_MSG0(MSG_LEGACY_LOW,"Ignore");
            break;

          case DS_DSD_SM_STATE_ACQ_HYST:
            break;

          case DS_DSD_SM_STATE_IN_SVC:
            DATA_MSG0(MSG_LEGACY_LOW,"Ignore");
            break;

          case DS_DSD_SM_STATE_OOS_HYST:
            if(FALSE == ds_dsd_common_should_go_oos_hysteresis(ds_dsd_sm->radio,
                                                               subs_id))
            {
              DATA_MSG2(MSG_LEGACY_LOW,"DSD Radio %d, 0x%x Transition: "
                        "DS_DSD_OOS_HYST --> DS_DSD_OOS",
                        ds_dsd_sm->radio,
                        ds_dsd_sm);
              ds_dsd_common_stop_oos_hyst_timer(ds_dsd_sm->radio, subs_id);
              ds_dsd_sm->state_prev = ds_dsd_sm->state;
              ds_dsd_sm->state = DS_DSD_SM_STATE_OOS;
              ds_dsd_pref_sys_recompute_best_system(subs_id);
            }
            break;

          default:
            DATA_MSG1(MSG_LEGACY_LOW,"Invalid state 0x%x", ds_dsd_sm->state);
            break;
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Acquisition Hysteresis Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_ACQ_HYST_TIMER_EXPIRED_EV:
      if(ds_dsd_sm->state == DS_DSD_SM_STATE_ACQ_HYST )
      {
        DATA_MSG2(MSG_LEGACY_LOW,"DSD Radio %d, 0x%x Transition: "
                  "DS_DSD_ACQ_HYST --> DS_DSD_IN_SVC",
                  ds_dsd_sm->radio,
                  ds_dsd_sm);
        ds_dsd_sm->state_prev = ds_dsd_sm->state;
        ds_dsd_sm->state = DS_DSD_SM_STATE_IN_SVC;
        if(ds_dsd_sm->sub_sm_ptr && 
           ds_dsd_sm->sub_sm_ptr->state == DS_DSD_SM_SUB_STATE_THROTTLE)
        {
          DATA_MSG0(MSG_LEGACY_LOW,"Do not recompute best system yet");
        }
        else
        {
          ds_dsd_pref_sys_recompute_best_system(subs_id);
        }
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_LOW,
                  "DSD_ACQ_HYST_TIMER_EXPIRED on state: %d. Ignore", 
                  ds_dsd_sm->state);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling OOS Hysteresis Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_OOS_HYST_TIMER_EXPIRED_EV:
      if(ds_dsd_sm->state == DS_DSD_SM_STATE_OOS_HYST )
      {
        DATA_MSG2(MSG_LEGACY_LOW,"DSD Radio %d, 0x%x Transition: "
                  "DS_DSD_OOS_HYST --> DS_DSD_OOS",
                  ds_dsd_sm->radio,
                  ds_dsd_sm);
        ds_dsd_sm->state_prev = ds_dsd_sm->state;
        ds_dsd_sm->state = DS_DSD_SM_STATE_OOS;

        ds_dsd_common_sub_state_transition(new_event,
                                           ds_dsd_sm,
                                           subs_id,
                                           radio,
                                           aux_radio);
        ds_dsd_pref_sys_recompute_best_system(subs_id);
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_LOW,
                  "DSD_OOS_HYST_TIMER_EXPIRED_EV on state: %d. Ignore", 
                  ds_dsd_sm->state);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Call Connected Event
    -----------------------------------------------------------------------*/
    case DSD_CALL_CONNECTED_EV: 
      if(radio == ds_dsd_sm->radio)
      {
        if(ds_dsd_sm->state == DS_DSD_SM_STATE_ACQ_HYST)
        {
          DATA_MSG2(MSG_LEGACY_LOW,"DSD Radio %d, 0x%x Transition: "
                    "DS_DSD_ACQ_HYST --> DS_DSD_IN_SVC",
                    ds_dsd_sm->radio,ds_dsd_sm);
          ds_dsd_common_stop_acq_hyst_timer(ds_dsd_sm->radio, subs_id);
          ds_dsd_sm->state_prev = ds_dsd_sm->state;
          ds_dsd_sm->state = DS_DSD_SM_STATE_IN_SVC;

          if(ds_dsd_sm->sub_sm_ptr && ds_dsd_sm->sub_sm_ptr->state 
             == DS_DSD_SM_SUB_STATE_THROTTLE)
          {
            DATA_MSG0(MSG_LEGACY_LOW,"Do not recompute best system yet");
          }
          else
          {
            ds_dsd_pref_sys_recompute_best_system(subs_id);
          }
        }
      }
      ds_dsd_common_sub_state_transition(new_event,
                                         ds_dsd_sm,
                                         subs_id,
                                         radio,
                                         aux_radio);
      break;

    /*-----------------------------------------------------------------------
      Handling Call Ended Event
    -----------------------------------------------------------------------*/
    case DSD_CALL_ENDED_EV: 
      if(radio != ds_dsd_sm->radio)
      {
        ds_dsd_common_sub_state_transition(new_event,
                                           ds_dsd_sm,
                                           subs_id,
                                           radio,
                                           aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Handdown Event
    -----------------------------------------------------------------------*/
    case DSD_HAND_DOWN_EV: 
      if(radio == ds_dsd_sm->radio)
      {
         ds_dsd_common_sub_state_transition(new_event,
                                            ds_dsd_sm,
                                            subs_id,
                                            radio,
                                            aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Handdown Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_HAND_DOWN_TIMER_EXPIRED_EV:
      if(radio == ds_dsd_sm->radio)
      {
         ds_dsd_common_sub_state_transition(new_event,
                                            ds_dsd_sm,
                                            subs_id,
                                            radio,
                                            aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Handdown Avoid Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_HAND_DOWN_AVOID_TIMER_EXPIRED_EV:
      if(radio == ds_dsd_sm->radio)
      {
         ds_dsd_common_sub_state_transition(new_event,
                                            ds_dsd_sm,
                                            subs_id,
                                            radio,
                                            aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Network Detach Hysteresis Timer Expired Event
    -----------------------------------------------------------------------*/
    case DSD_NW_DETACH_HYST_TIMER_EXPIRED_EV:
      if(radio == ds_dsd_sm->radio)
      {
         ds_dsd_common_sub_state_transition(new_event,
                                            ds_dsd_sm,
                                            subs_id,
                                            radio,
                                            aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Network Detached Event
    -----------------------------------------------------------------------*/
    case DSD_NW_DETACHED_EV:
      ds_dsd_common_sub_state_transition(new_event,
                                         ds_dsd_sm,
                                         subs_id,
                                         radio,
                                         aux_radio);
      break;

    /*-----------------------------------------------------------------------
      Handling EPS context transfer, Network Attaching and Network Attached Event
    -----------------------------------------------------------------------*/
    case DSD_EPS_CNTXT_TX_EV:
    case DSD_NW_ATTACHING_EV:
    case DSD_NW_ATTACHED_EV:
      ds_dsd_common_sub_state_transition(new_event,
                                         ds_dsd_sm,
                                         subs_id,
                                         radio,
                                         aux_radio);
      ds_dsd_pref_sys_recompute_best_system(subs_id);
      break;

    /*-----------------------------------------------------------------------
      Handling DCTM Throttle/Unthrottle Event
    -----------------------------------------------------------------------*/
    case DSD_DCTM_THROTTLE_EV:
    case DSD_DCTM_UNTHROTTLE_EV:
      if(radio == ds_dsd_sm->radio)
      {
         ds_dsd_common_sub_state_transition(new_event,
                                            ds_dsd_sm,
                                            subs_id,
                                            radio,
                                            aux_radio);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Network Param Change Event, LTE EX SO Event
    -----------------------------------------------------------------------*/
    case DSD_NETWORK_PARM_CHANGE_EV:
    case DSD_LTE_EX_SO_CHANGED_EV:
      if(radio == ds_dsd_sm->radio)
      {
        ds_dsd_pref_sys_recompute_best_system(subs_id);
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Voice/Emergency  Call Connected/orig Event.  
       Handling is same for voice and emergency given events 
    -----------------------------------------------------------------------*/
    case DSD_VOICE_CALL_ORIG_EV:
    case DSD_VOICE_CALL_CONNECTED_EV:
    case DSD_EMERGENCY_CALL_ORIG_EV:
    case DSD_VOICE_CALL_UP_ON_OTHER_SUB_EV:
      /* Stop OOS hysteresis timer if running */
      timer_id = ds_dsd_get_oos_timer_id(ds_dsd_sm->radio);
      if(ds_dsd_timer_running(timer_id, subs_id) == TRUE)
      {
        ds_dsd_timer_stop_timer(timer_id, subs_id);
      }

      /* Stop detach hysteresis timer if running */
      timer_id = ds_dsd_get_detach_timer_id(ds_dsd_sm->radio);
      if(ds_dsd_timer_running(timer_id, subs_id) == TRUE)
      {
        ds_dsd_timer_stop_timer(timer_id, subs_id);
      }

      break;

    /*-----------------------------------------------------------------------
      Handling Voice Call Ended Event
    -----------------------------------------------------------------------*/
    case DSD_VOICE_CALL_ENDED_EV:
    case DSD_VOICE_CALL_DOWN_ON_OTHER_SUB_EV:
      /* Re-start OOS hysteresis timer if stopped and SM is in OOS_HYST */
      timer_id = ds_dsd_get_oos_timer_id(ds_dsd_sm->radio);
      if((ds_dsd_timer_running(timer_id, subs_id) == FALSE) &&
         (ds_dsd_sm->state == DS_DSD_SM_STATE_OOS_HYST))
      {
        ds_dsd_timer_start_timer(timer_id, subs_id);
      }

      /* Re-start detach hysteresis timer if stopped and
         sub-SM is detach throttled */
      timer_id = ds_dsd_get_detach_timer_id(ds_dsd_sm->radio);
      if( ds_dsd_timer_running(timer_id, subs_id) == FALSE )
      {
        if((ds_dsd_sm->sub_sm_ptr != NULL) &&
           (ds_dsd_sm->sub_sm_ptr->state == DS_DSD_SM_SUB_STATE_THROTTLE) &&
           ((ds_dsd_sm->sub_sm_ptr->throttle_register & 
             (1 << DS_DSD_NW_DETACHED_THROTTLE)) != 0))
        {
          ds_dsd_timer_start_timer(timer_id, subs_id);
        }
      }
      break;

    /*-----------------------------------------------------------------------
      Handling Emergency Call Connected Event
    -----------------------------------------------------------------------*/
    case DSD_EMERGENCY_CALL_CONNECTED_EV:
      /* Stop OOS hysteresis timer if running */
      timer_id = ds_dsd_get_oos_timer_id(ds_dsd_sm->radio);
      if(ds_dsd_timer_running(timer_id, subs_id) == TRUE)
      {
        ds_dsd_timer_stop_timer(timer_id, subs_id);
      }
      
      /* Stop detach hysteresis timer if running */
      timer_id = ds_dsd_get_detach_timer_id(ds_dsd_sm->radio);
      if(ds_dsd_timer_running(timer_id, subs_id) == TRUE)
      {
        ds_dsd_timer_stop_timer(timer_id, subs_id);
      }

      ds_dsd_common_sub_state_transition(new_event,
                                         ds_dsd_sm,
                                         subs_id,
                                         radio,
                                         aux_radio);
      break;

    /*-----------------------------------------------------------------------
      Handling Emergency Call Connected Event
    -----------------------------------------------------------------------*/
    case DSD_EMERGENCY_CALL_ENDED_EV:
      /* Re-start OOS hysteresis timer if stopped and SM is in OOS_HYST */
      timer_id = ds_dsd_get_oos_timer_id(ds_dsd_sm->radio);
      if((ds_dsd_timer_running(timer_id, subs_id) == FALSE) &&
         (ds_dsd_sm->state == DS_DSD_SM_STATE_OOS_HYST))
      {
        ds_dsd_timer_start_timer(timer_id, subs_id);
      }

      /* Re-start detach hysteresis timer if stopped and
         sub-SM is detach throttled */
      timer_id = ds_dsd_get_detach_timer_id(ds_dsd_sm->radio);
      if( ds_dsd_timer_running(timer_id, subs_id) == FALSE )
      {
        if((ds_dsd_sm->sub_sm_ptr != NULL) &&
           (ds_dsd_sm->sub_sm_ptr->state == DS_DSD_SM_SUB_STATE_THROTTLE) &&
           ((ds_dsd_sm->sub_sm_ptr->throttle_register & 
             (1 << DS_DSD_NW_DETACHED_THROTTLE)) != 0))
        {
          ds_dsd_timer_start_timer(timer_id, subs_id);
        }
      }

      ds_dsd_common_sub_state_transition(new_event,
                                         ds_dsd_sm,
                                         subs_id,
                                         radio,
                                         aux_radio);
      break;

    /*-----------------------------------------------------------------------
      Handling Roaming Status Change Event
    -----------------------------------------------------------------------*/
    case DSD_ROAM_STATUS_CHANGED_EV:
      if(radio == ds_dsd_sm->radio)
      {
        ds_dsd_pref_sys_recompute_best_system(subs_id);
      }
      break;

    default:
      DATA_MSG1(MSG_LEGACY_MED,"Invalid new_event :%d", new_event);
      break;
  }

  return;
}/* ds_dsd_common_state_transition() */

