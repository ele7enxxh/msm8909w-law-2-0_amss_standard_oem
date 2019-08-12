#ifndef _QM_EFS_H
#define _QM_EFS_H

/*===========================================================================

                        QM_EFS.H

DESCRIPTION

  QMI_MMODE header file for efs module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qm_efs.h#1 $
  $Change: 11985193 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/27/13    ld     Initial version
===========================================================================*/
#include "comdef.h"
#include "qm_comdef.h"
#include "qm_nv.h"
#include "qmi_voice_cm_if.h"
#include "sys.h"
#include "mcfg_fs.h"

//===========================================================================
// Function prototypes
//===========================================================================

/*===========================================================================
  FUNCTION QM_EFS_INIT()

  DESCRIPTION
    This function initializes the efs global variables.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    Initializes all three global variables in efs_global to zero.
===========================================================================*/
void qm_efs_init( void );

/*===========================================================================
  FUNCTION QM_EFS_ALWAYS_RETURN_PLMN()

  DESCRIPTION
    Caches and verifies the boolean value contained in the EFS_ALWAYS_RET_PLMN_FILE.

  PARAMETERS
    NONE

  RETURN VALUE
    TRUE  - File was read successfully and contained a valid TRUE  value.
    FALSE - File was read successfully and contained a valid FALSE value, or
            an error encounterred, such as the file does not exist, or
            we do not have permissions to access it, or it contained an invalid
            value.

  DEPENDENCIES
    None

  SIDE EFFECTS
    If the file has not yet been cached, reads the  sets the global variables in efs_global.always_return_plmn
===========================================================================*/
boolean qm_efs_always_return_plmn( void );

boolean qm_efs_is_auto_setup_response_disabled( void );

/*===========================================================================
  FUNCTION QM_EFS_IS_NO_WAIT_RSP_ENABLED()

  DESCRIPTION
        This function returns the status of no wait flag

  PARAMETERS
    NONE

  RETURN VALUE
    TRUE  - File was read successfully and contained a valid TRUE  value.
    FALSE - File was read successfully and contained a valid FALSE value, or
            an error encounterred, such as the file does not exist, or
            we do not have permissions to access it, or it contained an invalid
            value.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_efs_is_no_wait_rsp_enabled( void );

/*===========================================================================
  FUNCTION QM_EFS_CSG_SUPPORTED()

  DESCRIPTION
    This function determines if CSG is supported..

  PARAMETERS
    None

  RETURN VALUE
    boolean: whether CSG is supported

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_efs_csg_supported ( void );

/*===========================================================================

FUNCTION qm_efs_get_ui_tty_setting()

DESCRIPTION
  Read ui_tty_setting from EFS item

DEPENDENCIES
  None

RETURN VALUE
  boolean : 1 = Get successful
            0 = Error in Get

SIDE EFFECTS
  None.

===========================================================================*/
boolean qm_efs_get_ui_tty_setting(
uint8            *ui_tty_setting_ptr,
                /* Pointer to a ui_tty_setting data.*/
int32           ui_tty_setting_size
                /* Size of the data_ptr buffer.*/
);

/*===========================================================================

FUNCTION qm_efs_put_ui_tty_setting()

DESCRIPTION
  Write ui_tty_setting to EFS item

DEPENDENCIES
  None

RETURN VALUE
  boolean : 1 = Put successful
            0 = Error in Put

SIDE EFFECTS
  None.

===========================================================================*/
boolean qm_efs_put_ui_tty_setting(
uint8           ui_tty_setting
                /* UI TTY Setting to write */
);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
  FUNCTION QM_EFS_REFRESH_MCFG()

  DESCRIPTION:
  This function updates QMI nas EFS/NV settings after a sim swap

  INPUT/OUTPUT PARAMETERS:
  **TODO subs id** - Subscription ID specific to some multi-subs NV items

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None
===========================================================================*/
void qmi_efs_refresh_mcfg ( uint16 subs_index );
#endif // FEATURE_MODEM_CONFIG_REFRESH

/*===========================================================================
  FUNCTION QM_EFS_DRX_SCALING_ENABLED()

  DESCRIPTION:
  This function checks the status of EFS item 
  /nv/item_files/modem/mmode/qmi/enable_drx_scaling_skip_meas

  RETURN VALUE:
  MCFG_FS_STATUS_OK if no error occured in mcfg_fs_read(), other values
  means error occured.

  SIDE EFFECTS:
  None
===========================================================================*/
mcfg_fs_status_e_type qmi_efs_drx_scaling_enabled ( void );

/*===========================================================================
  FUNCTION QM_EFS_READ_DATA_ROAMING_STATUS()

  DESCRIPTION:
  This function reads EFS settings related to the data roaming status which will be read by 3GPP NAS during powerup.

  INPUT/OUTPUT PARAMETERS:
  **TODO subs id** - Subscription ID specific to some multi-subs NV items

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None
===========================================================================*/

boolean qm_efs_read_data_roaming_status( int32 as_id, sys_data_roaming_e_type *data_roam_status );

/*===========================================================================
  FUNCTION QM_EFS_MODEM_CENTRIC_SOLUTION_disabled()

  DESCRIPTION:
  This function checks the status of EFS item 
  /nv/item_files/modem/mmode/disable_global_mode

  RETURN VALUE:
  true if file is 1, false if not present or 0

  SIDE EFFECTS:
  None
===========================================================================*/
boolean qm_efs_modem_centric_solution_disabled ( void );

#endif // !_QM_EFS_H
