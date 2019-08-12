#ifndef DS3GSTKPROC_H
#define DS3GSTKPROC_H
/*===========================================================================

           D A T A   S E R V I C E S   S T A C K   P R O C E S S O R
 
                              H E A D E R   F I L E

DESCRIPTION
  This file contains functions for processing CM SS event stacks.
  This header file is intended to be used internal to the 3G Dsmgr module only.

  Copyright (c) 2001-2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gstkproc.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/13   sd      TSTS changes for DS3G.
04/24/13   var     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sys.h"
#include "cm.h"
#include "dstask_v.h"
#include "ds3gsubsmgr.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
    DS3G radio enum type
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_RADIO_INDEX_MIN,
  DS3G_RADIO_INDEX_LTE = DS3G_RADIO_INDEX_MIN,
  DS3G_RADIO_INDEX_HDR,
  DS3G_RADIO_INDEX_1X,
  DS3G_RADIO_INDEX_GSM,
  DS3G_RADIO_INDEX_WCDMA,
  DS3G_RADIO_INDEX_TDS,
  DS3G_RADIO_INDEX_MAX
}ds3g_radio_index_type;

/*---------------------------------------------------------------------------
    DS3G SS stack enum type
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_SS_STACK_MIN,
  DS3G_SS_STACK_ID_1 = DS3G_SS_STACK_MIN,
  DS3G_SS_STACK_ID_2,
  DS3G_SS_STACK_MAX = DS3G_SS_STACK_ID_2
}ds3g_ss_stack_enum_type;

/*-------------------------------------------------------------------------
  Structure to store radio to stack information
-------------------------------------------------------------------------*/
typedef struct
{
  sys_sys_mode_e_type         sys_mode;
  uint8                       stack_id;
}ds3g_radio_to_stack_info_type;

/*-------------------------------------------------------------------------
  Structure to store radio to stack information
-------------------------------------------------------------------------*/
typedef struct
{
  ds3g_radio_to_stack_info_type  radio_info[DS3G_RADIO_INDEX_MAX];
}ds3g_radio_to_stack_table_type;

/*-------------------------------------------------------------------------
  Struct to cache stack info
-------------------------------------------------------------------------*/
typedef struct
{
  uint64                  changed_fields;
  uint64                  changed_fields2;
  boolean                 is_operational;
  sys_sys_mode_e_type     sys_mode;
  sys_srv_status_e_type   srv_status;
  sys_srv_domain_e_type   srv_domain;
  sys_srv_domain_e_type   effective_srv_domain;
  sys_sys_id_s_type       sys_id;
  cm_cell_srv_ind_s_type  cell_srv_ind;
} ds3g_cm_ss_stack_info_type;

/*-------------------------------------------------------------------------
  Struct to cache SS event info
-------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type      asubs_id;
  uint8                       number_of_stacks;
  uint64                      changed_fields_subs;
  ds3g_cm_ss_stack_info_type  stack_info[CM_NO_STACKS];
  boolean                     ps_data_suspend;
  boolean                     is_csfb_call_active;
  boolean                     bsr_in_progress;
  sys_sys_mode_e_type         target_rat;
  sys_div_duplex_e_type       div_duplex;
  boolean                     hdr_actv_ho_in_prog;
  boolean                     is_stack2_operational;
} ds3g_ss_event_info_type;

typedef enum
{
  DS3G_STKPROC_BSR_RESET,         /* BSR flag unset, no pending notification */
  DS3G_STKPROC_BSR_RESET_PENDING, /* BSR flag unset, pending notification */
  DS3G_STKPROC_BSR_SET,           /* BSR flag set */
}ds3g_stkproc_bsr_state_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GSTKPROC_INIT

DESCRIPTION   This function initializes ds3g stack processor.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_init(void);

/*===========================================================================
FUNCTION      DS3GSTKPROC_IGNORE_SRV_DOMAIN_CAMPED

DESCRIPTION   This function checks whether camped indication can be ignored.

PARAMETERS    srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If srv domain camped can be ignored
              FALSE: If srv domain camped can not be ignored

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_ignore_srv_domain_camped
(
  ds3g_cm_ss_stack_info_type  *stack_info_ptr,
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_IS_SRV_DOMAIN_PS_SUPPORTED

DESCRIPTION   This utility function checks if srv_domain is PS supported.

PARAMETERS    srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If the srv domain is PS supported
              FALSE: If the srv domain is not PS supported

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_is_srv_domain_ps_supported
(
  sys_srv_domain_e_type srv_domain
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_IS_SRV_DOMAIN_CS_SUPPORTED

DESCRIPTION   This utility function checks if srv_domain is PS supported.

PARAMETERS    srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If the srv domain is CS supported
              FALSE: If the srv domain is not CS supported

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_is_srv_domain_cs_supported
(
  sys_srv_domain_e_type srv_domain
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_CONVERT_SYS_MODE_TO_RADIO_INDEX

DESCRIPTION   This is a utility to translate sys mode to DS3G radio index.

PARAMETERS    sys_mode: sys_mode
 
DEPENDENCIES  None.

RETURN VALUE  ds3g radio index.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_radio_index_type ds3gstkproc_convert_sys_mode_to_radio_index
(
  sys_sys_mode_e_type sys_mode
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_CONVERT_RADIO_INDEX_TO_SYS_MODE

DESCRIPTION   This is a utility to translate DS3G radio index to sys mode.

PARAMETERS    index: ds3g radio index

DEPENDENCIES  None.

RETURN VALUE  sys_mode

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gstkproc_convert_radio_index_to_sys_mode
(
  ds3g_radio_index_type index
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_STACK_ID_FROM_RADIO

DESCRIPTION   returns the stack in which sys_mode was present

DEPENDENCIES  None.

RETURN VALUE  stack info.

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds3gstkproc_get_stack_id_from_radio
(
  sys_modem_as_id_e_type  asubs_id,
  sys_sys_mode_e_type     sys_mode
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_IS_DSD_NOTIFIED_BY_SS_EVENT

DESCRIPTION   This function returns the flag DSD has been notified by current 
              SS event.
  
PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  True:  DSD notified by ss_event.
              False: DSD not notified by ss_event.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gstkproc_is_dsd_notified_by_ss_event(void);

/*===========================================================================
FUNCTION      DS3GSTKPROC_SET_DSD_NOTIFIED_SS_EVENT

DESCRIPTION   This function sets the flag DSD notified by current SS event.
  
PARAMETERS    DSD notified by ss_event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_set_dsd_notified_by_ss_event
(
  boolean dsd_notified_by_ss_event
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_BSR_STATE

DESCRIPTION   This function returns the BSR state flag.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  bsr state.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_stkproc_bsr_state_type ds3gstkproc_get_bsr_state
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_SET_BSR_STATE

DESCRIPTION   This function sets the BSR state flag.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_set_bsr_state
(
  ds3gsubsmgr_subs_id_e_type   subs_id,
  ds3g_stkproc_bsr_state_type  bsr_state
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_SS_EVENT_INFO_INSTANCE

DESCRIPTION   This utility function will return a pointer to the ss event info
              for the passed sub id 

PARAMETERS    subs_id: ds3g data sub id.

DEPENDENCIES  None.

RETURN VALUE  pointer to ds3g_ss_event_info[subs_id] 
              NULL pointer if subs_id is invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds3g_ss_event_info_type* ds3gstkproc_get_ss_event_info_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_GET_STACK_INFO_FROM_STACK_ID

DESCRIPTION   This utility function returns pointer of cached stack_info for 
              the specified stack_id.

PARAMETERS    subs_id:  subs_id
              stack_id: CM stack id 
              stack_info_ptr: cached stack info pointer
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_get_stack_info_from_stack_id
(
  ds3gsubsmgr_subs_id_e_type    subs_id,
  uint8                         stack_id,
  ds3g_cm_ss_stack_info_type  **stack_info_ptr
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_CACHED_STACK_INFO

DESCRIPTION   This function parses the cached SS event stacks.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_cached_stack_info
(
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================
FUNCTION      DS3GSTKPROC_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION   This function process the SS event information.

PARAMETERS    ss_event_info_p: SS event info ptr.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gstkproc_process_cm_ss_srv_chg_cmd
(
  ds3g_ss_event_info_type  *ss_event_info_ptr
);

#endif /* DS3GSTKPROC_H */

