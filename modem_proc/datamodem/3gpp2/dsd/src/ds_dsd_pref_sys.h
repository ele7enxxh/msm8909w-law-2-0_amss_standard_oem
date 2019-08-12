#ifndef DS_DSD_PREF_SYS_H
#define DS_DSD_PREF_SYS_H
/*===========================================================================

                      D S _ D S D _ P R E F _ S Y S . H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_pref_sys.h#1 $
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
#include "comdef.h"
#include "customer.h"
#include "ds_dsd_common.h"
#include "ds3gsubsmgr.h"
#include "cm.h"

#define MAX_RADIO_TECH_NAME_SIZE 8
#define DSD_PRIORITY_MAX         255

/*------------------------------------------------------------------------
  Preferred system list sructure
------------------------------------------------------------------------*/
typedef struct
{
  dsd_radio_type  radio_type;
  uint8           priority;
  boolean         available;
  char            tech_name_string[MAX_RADIO_TECH_NAME_SIZE];
}dsd_pref_sys_order_type;

/*------------------------------------------------------------------------
  DSD Preferred system info type
------------------------------------------------------------------------*/
typedef struct
{
  dsd_radio_type            best_radio;
  dsd_radio_type            last_best_radio;
  uint32                    changed_radio_mask[DS_DSD_MAX_RADIO];
  boolean                   recompute_in_progress;
  boolean                   mode_pref_force_notify;
  boolean                   dds_chg_force_notify;
  dsd_pref_sys_order_type   pref_ordered_list[DS_DSD_MAX_RADIO];
}dsd_pref_sys_info_type;

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef void (*ds_dsd_pref_notify_func_type)
                                          (dsd_radio_type,
                                          dsd_radio_type,
                                           boolean,
                                           ds3gsubsmgr_subs_id_e_type);

/*---------------------------------------------------------------------------
  Recompute cmd info structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds3gsubsmgr_subs_id_e_type  subs_id;
}ds_dsd_recompute_cmd_info_type;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_GET_PREF_ORDER_INDEX

DESCRIPTION   This utility function returns the index in preferred order list  
              for the corresponding radio.

PARAMETERS    dsd_radio.
 
DEPENDENCIES  None.

RETURN VALUE  index.

SIDE EFFECTS  None.
===========================================================================*/
uint16 ds_dsd_pref_sys_get_pref_order_index
(
  dsd_radio_type              dsd_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_MODE_PREF_CHG

DESCRIPTION   This function handles the mode pref change.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Updates the preferred order list based on new mode pref.
===========================================================================*/
void ds_dsd_pref_sys_mode_pref_chg
(
  cm_mode_pref_e_type         mode_pref,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

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
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS_DSD_PREF_SYS_INIT

DESCRIPTION   This function initializes the preferred system. This is during 
              power up. 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_pref_sys_init(void);

#endif /* DS_DSD_PREF_SYS_H */
