/*===========================================================================
                     D S _ D S D _ P R O C . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2013-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_proc.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/29/13    vtk    Created Module
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "data_msg.h"
#include "ds3gsubsmgr.h"
#include "ds_dsd_1xSm.h"
#include "ds_dsd_gsmSm.h"
#include "ds_dsd_lteSm.h"
#include "ds_dsd_hdrSm.h"
#include "ds_dsd_tdscdmaSm.h"
#include "ds_dsd_wcdmaSm.h"
#include "ds_dsd_wlanSm.h"
#include "ds_dsd_proc.h"
#include "ds_dsd_pref_sys.h"
#include "ds_dsd_timer.h"
#include "modem_mem.h"

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/
uint32                      dsd_subs_active_mask = 0;
ds_dsd_proc_type           *dsd_proc[DS3GSUBSMGR_SUBS_ID_COUNT] = {NULL};

/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_PROC_INIT

DESCRIPTION   This function initializes DSD processor and all DSD 
              statemachines.

DEPENDENCIES  Event Dispatcher module is initialized already.

RETURN VALUE  None.

SIDE EFFECTS  Variables are initialized to default value. 
===========================================================================*/
void ds_dsd_proc_init(void)
{
  ds3gsubsmgr_subs_id_e_type  indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize Subscription */
  dsd_subs_active_mask = 0x00;

  /* Initialize all DSD proc instances to NULL */
  for(indx = DS3GSUBSMGR_SUBS_ID_MIN; indx < DS3GSUBSMGR_SUBS_ID_MAX; indx++)
  {
    dsd_proc[indx] = NULL;
  }

  return;
}/* ds_dsd_proc_init() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_IS_DSD_PROC_ACTIVE

DESCRIPTION   This function return if DSD processor is active.for subs_id.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  DSD proc active for subs_id.
              FALSE: DSD proc inactive for subs_id.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_proc_is_dsd_proc_active
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return (dsd_subs_active_mask & CONVERT_SUBS_ID_TO_MASK(subs_id));
}/* ds_dsd_proc_is_dsd_proc_active() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_IS_RADIO_SM_ACTIVE

DESCRIPTION   This function return if radio sm is active.for subs_id.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  radio sm is active for subs_id.
              FALSE: radio sm is inactive for subs_id 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_proc_is_radio_sm_active
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              dsd_radio
)
{
  ds_dsd_radio_sm_enum_type   radio_sm;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(ds_dsd_proc_is_dsd_proc_active(subs_id) == FALSE)
  {
    return FALSE;
  }

  radio_sm = ds_dsd_common_convert_dsd_radio_to_radio_sm(dsd_radio);

  return 
    (dsd_proc[subs_id]->radio_active_mask & CONVERT_RADIO_SM_TO_MASK(radio_sm));
}/* ds_dsd_proc_is_radio_sm_active() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_GET_DSD_PROC_INSTANCE

DESCRIPTION   This function returns the DSD proc instance pointer 
              corresponding to subs_id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_proc_type* ds_dsd_proc_get_dsd_proc_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return NULL;
  }
  return dsd_proc[subs_id];
}/* ds_dsd_proc_get_dsd_proc_instance_ptr() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_GET_RADIO_SM_INSTANCE

DESCRIPTION   This function returns the DSD proc pointer corresponding to 
              subs_id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_proc_get_radio_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              dsd_radio
)
{
  ds_dsd_proc_type           *dsd_proc_ptr = NULL;
  ds_dsd_radio_sm_enum_type   radio_sm;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    return NULL;
  }

  radio_sm = ds_dsd_common_convert_dsd_radio_to_radio_sm(dsd_radio);
  if(!VALIDATE_DSD_RADIO_SM(radio_sm))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid radio_sm:%d. return", radio_sm);
    return NULL;
  }
  return dsd_proc_ptr->radio_sm[radio_sm];
}/* ds_dsd_proc_get_radio_sm_instance() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_GET_PREF_SYS_INFO

DESCRIPTION   This function returns the pref sys info pointer corresponding 
              to subs_id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
dsd_pref_sys_info_type* ds_dsd_proc_get_pref_sys_info
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return NULL;
  }
  return &(dsd_proc_ptr->pref_sys_info);
}/* ds_dsd_proc_get_pref_sys_info() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_GET_TIMER_LIST

DESCRIPTION   This function returns the timer info pointer corresponding 
              to subs_id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_timer_list_type* ds_dsd_proc_get_timer_list
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return NULL;
  }
  return &(dsd_proc_ptr->timer_list);
}/* ds_dsd_proc_get_timer_list() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_GET_EXT_NOTIF_INFO

DESCRIPTION   This function returns the ext notification info pointer 
              corresponding to subs_id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
dsd_ext_notif_info_type* ds_dsd_proc_get_ext_notif_info
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return NULL;
  }
  return &(dsd_proc_ptr->ext_notif_info);
}/* ds_dsd_proc_get_ext_notif_info() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_GET_EPC_PRESENT_FLAG

DESCRIPTION   This function returns the epc present flag corresponding 
              to subs_id.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If EPC present for subs_id.
              False: If EPC not present for subs_id.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_proc_get_epc_present_flag
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "DSD proc ptr NULL for subs_id:%d", subs_id);
    return FALSE;
  }
  return dsd_proc_ptr->epc_present_flag;
} /* ds_dsd_proc_get_epc_present_flag() */


/*===========================================================================
FUNCTION      DS_DSD_PROC_CREATE_DSD_PROC_INSTANCE

DESCRIPTION   This function creates an instance of DSD processor.
 
DEPENDENCIES  None.

RETURN VALUE  If success, return TRUE
              If failure, return FALSE 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_proc_create_dsd_proc_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_proc_type  *dsd_proc_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return FALSE;
  }

  dsd_proc_ptr = (ds_dsd_proc_type*)modem_mem_alloc(sizeof(ds_dsd_proc_type),
                                                    MODEM_MEM_CLIENT_DATA);
  if(dsd_proc_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_ERROR,
              "Failed to create DSD proc for subs_id:%d. assert",
              subs_id);
    ASSERT(0);
    return FALSE;
  }

  /* reset DSD proc and set active mask */
  memset(dsd_proc_ptr, 0 , sizeof(ds_dsd_proc_type));
  dsd_proc[subs_id] = dsd_proc_ptr;
  dsd_subs_active_mask |= CONVERT_SUBS_ID_TO_MASK(subs_id);

  /* reset pref sys info */
  ds_dsd_pref_sys_reset_pref_sys_info(subs_id);

  /* reset timer info */
  ds_dsd_timer_init_timers(subs_id);

  DATA_MSG1(MSG_LEGACY_MED, 
            "ds_dsd_proc_create_dsd_proc_instance success subs_id:%d",
            subs_id);
  return TRUE;
}/* ds_dsd_proc_create_dsd_proc_instance() */

/*===========================================================================
FUNCTION      DS_DSD_PROC_CREATE_RADIO_SM_INSTANCE

DESCRIPTION   This function creates an instance of radio SM.
 
DEPENDENCIES  None.

RETURN VALUE  If success, return TRUE
              If failure, return FALSE 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_proc_create_radio_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              dsd_radio
)
{
  ds_dsd_radio_sm_enum_type   radio_sm;
  ds_dsd_proc_type           *dsd_proc_ptr = NULL;
  ds_dsd_sm_type             *sm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  if(dsd_proc_ptr == NULL)
  {
    return FALSE;
  }

  radio_sm = ds_dsd_common_convert_dsd_radio_to_radio_sm(dsd_radio);
  switch(radio_sm)
  {
    case DS_DSD_RADIO_SM_LTE:
      sm_ptr = ds_dsd_lteSm_create_sm_instance(subs_id);
      break;

    case DS_DSD_RADIO_SM_HDR:
      sm_ptr = ds_dsd_hdrSm_create_sm_instance(subs_id);
      break;

    case DS_DSD_RADIO_SM_1X:
      sm_ptr = ds_dsd_1xSm_create_sm_instance(subs_id);
      break;

    case DS_DSD_RADIO_SM_GSM:
      sm_ptr = ds_dsd_gsmSm_create_sm_instance(subs_id);
      break;

    case DS_DSD_RADIO_SM_TDSCDMA:
      sm_ptr = ds_dsd_tdscdmaSm_create_sm_instance(subs_id);
      break;

    case DS_DSD_RADIO_SM_WCDMA:
      sm_ptr = ds_dsd_wcdmaSm_create_sm_instance(subs_id);
      break;

#ifdef FEATURE_DATA_WLAN_MAPCON
    case DS_DSD_RADIO_SM_WLAN:
      sm_ptr = ds_dsd_wlanSm_create_sm_instance(subs_id);
      break;
#endif /*FEATURE_DATA_WLAN_MAPCON*/

    default:
      break;
  }

  if(sm_ptr == NULL)
  {
    DATA_MSG2(MSG_LEGACY_ERROR,
              "Failed to create radio SM for subs_id:%d. dsd_radio:%d assert",
              subs_id, dsd_radio);
    ASSERT(0);
    return FALSE;
  }

  dsd_proc_ptr->radio_active_mask |= CONVERT_RADIO_SM_TO_MASK(radio_sm);
  dsd_proc_ptr->radio_sm[radio_sm] = sm_ptr;

  /* reset statemachine */
  ds_dsd_common_reset_sm_instance(subs_id, dsd_radio);

  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_proc_create_radio_sm_instance success subs_id:%d dsd_radio:%d "
            "ret_val:%d",
            subs_id, dsd_radio);
  return TRUE;
}/* ds_dsd_proc_create_radio_sm_instance() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

