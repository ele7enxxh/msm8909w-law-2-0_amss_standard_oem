#ifndef DSUCSDMPSHIM_H
#define DSUCSDMPSHIM_H
/*===========================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

                  M U L T I P R O C E S S O R   S H I M

                          H E A D E R   F I L E

DESCRIPTION

  This file contains the interfaces for UMTS Circuit-Switched Data
  multiprocessor (MP) shim.
  
Copyright (c) 2006 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dsucsdmpshim.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/29/11   SA      Added support for TD-SCDMA.
04/05/11   SS      Q6 free floating support.
01/06/11   SA      Fix to set the correct SIO packet size for GSM async call.
09/09/09   BS      Removed private header inclusions.
03/04/09   SA      AU level CMI modifications.
03/25/08   SA      Moved ds_ucsd_mpshim_configure_l2_watermarks to internal header.
03/14/06   AR      Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )

#include "sys.h"
#include "cm.h"
#include "dsm.h"
#include "dsucsd.h"
#include "dsgcsd_task.h"
#include "dsucsdappif.h"
#include "dsucsdhdlr.h"

#ifdef FEATURE_DATA_WCDMA_CS
#include "rrcdata.h"
#endif /* FEATURE_DATA_WCDMA_CS */

#ifdef FEATURE_TDSCDMA
#ifndef FEATURE_SEGMENT_LOADING
//#include "tdsrrcdata.h"
#include "tdsrrcdataext.h"
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_TDSCDMA */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION ds_ucsd_mpshim_send_gcsd_msg

DESCRIPTION
  This function sends a command message to the GCSD task.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Command is enqueued to GCSD task.

===========================================================================*/
void
ds_ucsd_mpshim_send_gcsd_msg
(
  ds_gcsd_cmd_enum_type   cmd_id,
  cm_call_id_type         call_id
);

#ifdef FEATURE_DATA_WCDMA_CS

/*===========================================================================

FUNCTION ds_ucsd_mpshim_register_wcdma_rlc

DESCRIPTION
  This function registers the interface with WCDMA Layer 2 RLC.
  The watermarks are configured based on the type of call.
        
DEPENDENCIES
   None
  
RETURN VALUE
   TRUE for successful processing; FALSE otherwise.
    
SIDE EFFECTS
   Commands queued to L2 UL/DL tasks.

===========================================================================*/
boolean
ds_ucsd_mpshim_register_wcdma_rlc
(
  const rrc_user_plane_lc_info_type  *lc_info_ptr,
  const ds_ucsd_dpath_info_type      *dpath_ptr,
  sys_modem_as_id_e_type              subs_id
);


/*===========================================================================

FUNCTION ds_ucsd_deregister_wcdma_rlc_interface

DESCRIPTION
   This function deregisters the interface with WCDMA RLC.
        
DEPENDENCIES
   None
  
RETURN VALUE
   TRUE on successful processing; FALSE otherwise.
    
SIDE EFFECTS
   Command sent to L2 UL & DL tasks

===========================================================================*/
boolean
ds_ucsd_mpshim_deregister_wcdma_rlc 
(
  sys_modem_as_id_e_type subs_id 
);

#endif /* FEATURE_DATA_WCDMA_CS*/

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION ds_ucsd_mpshim_register_tdscdma_rlc

DESCRIPTION
  This function registers the watermarks with 
  TD-SCDMA RLC for uplink & downlink data
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE -- Sucess
  FALSE -- Failure
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ucsd_mpshim_register_tdscdma_rlc
(
  const tdsrrc_user_plane_lc_info_type  *lc_info_ptr,
  const ds_ucsd_dpath_info_type         *dpath_ptr,
  sys_modem_as_id_e_type                 subs_id
);

/*===========================================================================
FUNCTION ds_ucsd_mpshim_deregister_tdscdma_rlc

DESCRIPTION
  This function deregisters the watermarks with 
  TDRLC for UL & DL data

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- Success
  FALSE -- Failure

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ucsd_mpshim_deregister_tdscdma_rlc
(
  sys_modem_as_id_e_type subs_id
);
#endif /* FEATURE_TDSCDMA */

#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_rlc_datapath

DESCRIPTION
  This function returns the elements for the datapath to WCDMA RLC
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void
ds_ucsd_mpshim_get_rlc_datapath
(
  ds_ucsd_dpath_info_type * datapath_ptr
);
#endif /* (FEATURE_DATA_WCDMA_CS) || (FEATURE_TDSCDMA) */

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_rlp_dc_state

DESCRIPTION
  This function returns the RLP data compression state (direction) 
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
byte
ds_ucsd_mpshim_get_rlp_dc_state( void );


/*===========================================================================

FUNCTION ds_ucsd_mpshim_connect_data_path

DESCRIPTION
  This function connects the data path with L2.

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_mpshim_connect_data_path
(
  const dsucsd_call_descriptor_type  *call_desc_ptr,   /* Call descriptor */
  const ds_ucsd_ota_channel_params_T *chan_parms_ptr   /* RLC OTA info    */
);

/*===========================================================================

FUNCTION ds_ucsd_mpshim_disconnect_data_path

DESCRIPTION
  This function disconnects the data path with L2.

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_mpshim_disconnect_data_path
(
  const sys_sys_mode_e_type          network_mode
);

/*===========================================================================

FUNCTION ds_ucsd_mpshim_call_cleanup

DESCRIPTION
  This function does stack cleanup on call termaination.

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_mpshim_call_cleanup
(
  const dsucsd_call_descriptor_type *call_desc_ptr,     /* Call descriptor */
  const boolean                      call_was_connected  /* State flag     */
);


#if defined(FEATURE_DATA_WCDMA_CS) && defined(FEATURE_DATA_UCSD_HANDOVER)

/*===========================================================================

FUNCTION ds_ucsd_mpshim_nt_handover_change_config

DESCRIPTION
  This function changes the NT call configuration for WCDMA or GSM
  target mode.  The OTA callback functions for interfacing with lower
  layers are set based on handover direction/reason.  

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if processing successful; FALSE otherwise
    
SIDE EFFECTS
  None

===========================================================================*/
boolean
ds_ucsd_mpshim_nt_handover_change_config
(
  const dsucsd_call_descriptor_type   *call_desc_ptr,   /* Call descriptor */
  const sync_enum_T                    direction,       /* HO direction    */
  const ds_ucsd_ota_channel_params_T  *chan_params_ptr  /* RLC OTA info    */
);

/*===========================================================================

FUNCTION ds_ucsd_mpshim_nt_handover_wrapup

DESCRIPTION
  This function performs wrapup for interRAT handover processing.  If
  there was an error indicated, call teardown is triggered.  It
  changes the NT call configuration if neceaasy to ensure the lower
  layers will clean up correctly.  GCSD task is signaled to kickstart
  RLP is new mode is WCDMA.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Command enqueued to GCSD task.

===========================================================================*/
void
ds_ucsd_mpshim_nt_handover_wrapup
(
  const dsucsd_call_descriptor_type   *call_desc_ptr,   /* Call descriptor */
  const sync_enum_T                    direction,       /* HO direction    */
  const boolean                        success          /* Status flag     */ 
);

#endif /* FEATURE_DATA_WCDMA_CS && FEATURE_DATA_UCSD_HANDOVER */


/*===========================================================================

FUNCTION ds_ucsd_mpshim_init

DESCRIPTION
  This funciton initializes the module at power up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_mpshim_init( void );

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_tps_from_sio_or_fps_wm

DESCRIPTION
  This funciton returns a pointer to the tps_from_sio_or_fps_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_tps_from_sio_or_fps_wm(void);

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_tps_to_sio_or_fps_wm

DESCRIPTION
  This funciton returns a pointer to the tps_to_sio_or_fps_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_tps_to_sio_or_fps_wm(void);

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_fps_from_sio_wm

DESCRIPTION
  This funciton returns a pointer to the fps_from_sio_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_fps_from_sio_wm(void);

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_fps_to_sio_wm

DESCRIPTION
  This funciton returns a pointer to the fps_to_sio_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_fps_to_sio_wm(void);

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

#endif /* DSUCSDAPPIF_H */
