#ifndef DS_DSD_PROC_H
#define DS_DSD_PROC_H
/*===========================================================================

                      D S _ D S D _ P R O C . H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_proc.h#1 $
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
#include "comdef.h"
#include "customer.h"
#include "ds_dsd_common.h"
#include "ds_dsd_pref_sys.h"
#include "ds_dsd_timer.h"
#include "ds_dsd_ext.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------
  Macro to convert SUBS ID to Mask value
-------------------------------------------------------------------------*/
#define CONVERT_SUBS_ID_TO_MASK( val )   ((uint32)1<<(int)(val))

/*-------------------------------------------------------------------------
  DSD processor structure
    radio_on_mask:     mask of active radio statemachines
    radio_sm:          pointer to radio statemachines
    pref_sys_info:     Preferred system info
    timer_list:        List of DSD timers
    epc_present_flag:  Flag to indicate if epc is active
-------------------------------------------------------------------------*/
typedef struct
{
  uint32                    radio_active_mask;
  ds_dsd_sm_type           *radio_sm[DS_DSD_RADIO_SM_COUNT];
  dsd_pref_sys_info_type    pref_sys_info;
  ds_dsd_timer_list_type    timer_list;
  dsd_ext_notif_info_type   ext_notif_info;
  boolean                   epc_present_flag;
}ds_dsd_proc_type;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_PROC_INIT

DESCRIPTION   This function initializes DSD processor and all DSD 
              statemachines.

DEPENDENCIES  Event Dispatcher module is initialized already.

RETURN VALUE  None.

SIDE EFFECTS  Variables are initialized to default value. 
===========================================================================*/
void ds_dsd_proc_init(void);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS_DSD_PROC_CREATE_RADIO_SM_INSTANCE

DESCRIPTION   This function creates an instance of radio SM
 
DEPENDENCIES  None.

RETURN VALUE  If success, return TRUE
              If failure, return FALSE 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_proc_create_radio_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dsd_radio_type              dsd_radio
);

#endif /* DS_DSD_PROC_H */
