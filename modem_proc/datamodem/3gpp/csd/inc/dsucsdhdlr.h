#ifndef DSUCSDHDLR_H
#define DSUCSDHDLR_H
/*===========================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

                M O D E - S P E C I F I C   H A N D L E R S

                          H E A D E R   F I L E

DESCRIPTION
  This header file contains interfaces that are needed by the DSTASK.

  
Copyright (c) 2003 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dsucsdhdlr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/11   ttv     GSDI to MMGSDI_SESSION api migration changes.
04/05/11   ss      Q6 free floating support.
03/25/08   sa      Added ds_ucsd_mpshim_configure_l2_watermarks from public header.
06/27/07   ar      Add multiple client support for unified API.
03/25/03   dgy     Initial version. Merged GSM and WCDMA CS Data mode-
                   specific handlers. Added support for WCDMA Asynchronous
                   CS Data.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "IWCDMA.h"
#include "ITDSCDMA.h"
#endif /* FEATURE_SEGMENT_LOADING  */

#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )

#include "dsucsdi.h"
#include "ds3gmmgsdiif.h"

/* Marco to map CM callID to UCSD call instance */
#define DSUCSD_XLATE_CALLID( call_id ) \
      ds_ucsd_get_call_inst_from_call_id(call_id)

/* Marco to return call instance record based on table index */
#define DSUCSD_GET_CALL_INFO( index ) \
      ds_ucsd_get_call_tbl(index)

/* Marco to signify validity of CS radio(sys_mode) */
#define INVALID_3GPP_CS_SYS_MODE( ds_ucsd_network_mode ) \
      (ds_ucsd_network_mode != SYS_SYS_MODE_TDS && \
       ds_ucsd_network_mode != SYS_SYS_MODE_WCDMA && \
       ds_ucsd_network_mode != SYS_SYS_MODE_GSM)

/* DS_UCSD_APP_ORIG_CALL_CMD command type */
typedef struct
{
  ds_ucsd_client_id_type  client_id;      /* UCSD client identifier        */
  uint8                inst_id;           /* CSData call instance ID       */
  ds_ucsd_call_type    call_type;         /* Type of call                  */
  uint8                call_mode;         /* Single/dual mode flag         */
  uint8                speed;             /* Equivalent to +CBST Speed     */
  uint8                name;              /* Synchronous or Asynchronous   */
  uint8                ce;                /* Transparent or Non-Transparent*/
  ds_ucsd_data_rate_T  ota_rate;          /* OTA data rate                 */
  uint8                waiur;             /* Equivalent to +CHSN WAIUR.    */
  const uint8         *dial_string;       /* Null-terminated dial string   */
  dsm_watermark_type  *ul_rlc_wm_ptr;     /* Pointer to UL RLC WM          */
  dsm_watermark_type  *dl_rlc_wm_ptr;     /* Pointer to DL RLC WM          */
  ds_ucsd_rlc_dl_rx_cb_type rlc_dl_rx_cb; /* Function called by RLC        **
                                          ** when RLC receives DL data.    **
                                          ** This callback can be NULL     */
  ds_ucsd_dial_modifier_info_type modifiers;/* SupSrv dial modifiers       */
  ds_ucsd_uus_info_type uus_info;          /* User to User IE              */
} ds_ucsd_app_orig_call_cmd_type;

/* DS_UCSD_APP_ANSWER_CALL_CMD command type */
typedef struct
{
  ds_ucsd_client_id_type    client_id;    /* UCSD client identifier        */
  uint8                inst_id;           /* CSData call instance ID       */
  ds_ucsd_call_type    call_type;         /* Type of call                  */
  dsm_watermark_type  *ul_rlc_wm_ptr;     /* Pointer to UL RLC WM          */
  dsm_watermark_type  *dl_rlc_wm_ptr;     /* Pointer to DL RLC WM          */
  ds_ucsd_rlc_dl_rx_cb_type rlc_dl_rx_cb; /* Function called by RLC        **
                                          ** when RLC receives DL data.    **
                                          ** This callback can be NULL     */
  ds_ucsd_answer_param_type answer_params; /* Incoming call answer info    */
} ds_ucsd_app_answer_call_cmd_type;

/* DS_UCSD_APP_CONFIRM_CALL_CMD command type */
typedef struct
{
  ds_ucsd_client_id_type    client_id;     /* UCSD client identifier       */
  uint8                     inst_id;       /* CSData call instance ID     */
  ds_ucsd_call_type         call_type;     /* Type of call                */
  ds_ucsd_answer_param_type result_params; /* Call setup result info      */
} ds_ucsd_app_confirm_call_cmd_type;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/* DS_UCSD_APP_END_CALL_CMD command type */
typedef struct
{
  ds_ucsd_client_id_type    client_id;     /* UCSD client identifier       */
  uint8                     inst_id;           /* CSData call instance ID       */
  ds_ucsd_call_type         call_type;         /* Type of call                  */
} ds_ucsd_app_end_call_cmd_type;

#ifdef FEATURE_MMGSDI_SESSION_LIB
typedef struct
{
  mmgsdi_cnf_enum_type  cnf_type;
  mmgsdi_cnf_type       cnf_data;
  uint8                 read_cnf_data[MAX_MMGSDI_BUFFER_LEN];
} ds_ucsd_mmgsdi_session_cnf_info;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION ds_ucsd_hdlr_init

DESCRIPTION
   This function will be called on power up initialization to initialize
   the UCSD subtask.  A call to this function will be placed in  
   dsi_task_init().  In addition to local initializations, this function
   will register the UCSD mode specific handler functions with 3G DSMGR. 
   
         
DEPENDENCIES
   None
  
RETURN VALUE
   Signal mask of signals that the UCSD mode-specific subtask will expect/
   handle.
    
SIDE EFFECTS
   None

===========================================================================*/
rex_sigs_type
ds_ucsd_hdlr_init (void);



/*===========================================================================

FUNCTION ds_ucsd_cc_process_cmds

DESCRIPTION
   This function will be from the main DS task dispather to send
   commands to the UCSD mode-specific subtask.  Currently the only
   command that is handled is the UCSD protocol stack disconnect
   indication.  This in turn sends a command to the 3GDSMR if there
   is a call active. 
   
   Note that the command buffer is not put back on the free queue here...
   that is done by the caller of this function.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void
ds_ucsd_process_cmds
(
  ds_cmd_type  *cmd_ptr
);


/*===========================================================================

FUNCTION ds_ucsd_cc_process_signals

DESCRIPTION
   This function will be called from the main DS task dispather to send
   signals to the UCSD mode-specific subtask.  Currently there are no
   UCSD mode specific signals. 
   
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/ 
void
ds_ucsd_process_signals
(
   rex_sigs_type set_sigs 
);


/*===========================================================================

FUNCTION ds_ucsd_dial_string_handler

DESCRIPTION
   This function is the UMTS CS dial string handler that is called
   by ATCOP when a UMTS CS call is to be initiated. 
   
        
DEPENDENCIES
   None
  
RETURN VALUE
   An abort function callback
    
SIDE EFFECTS
   None

===========================================================================*/
dsat_call_abort_cb_type
ds_ucsd_dial_string_handler
( 
   const uint8                    *dial_string, 
   boolean                        digit_mode,
  dsat_dial_modifier_info_type    *modifiers,
  sys_modem_as_id_e_type          subs_id
);

/*===========================================================================


FUNCTION ds_ucsd_validate_cbst_values


DESCRIPTION
   This function validates the +CBST values against the speed, name,
   and connection element list supported within the CS protocol stack.


DEPENDENCIES
   None


RETURN VALUE
   TRUE if the parameter set is valid.  FALSE otherwise.


SIDE EFFECTS
   None


===========================================================================*/
boolean ds_ucsd_validate_cbst_values
(
   uint32 speed,
   uint32 name,
   uint32 ce
);


/*===========================================================================


FUNCTION ds_ucsd_validate_chsn_values

DESCRIPTION
  This function validates the +CHSN values against the WAIUR list
  supported within the CS protocol stack.

DEPENDENCIES
   None

RETURN VALUE
   TRUE if the parameter set is valid.  FALSE otherwise.

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_validate_chsn_values
(
  uint8 connection_element,
  uint8 waiur
);

/*===========================================================================

FUNCTION ds_ucsd_get_data_counters

DESCRIPTION
  This function returns the Tx/Rx data counters for a specified call.

DEPENDENCIES
  None
  
RETURN VALUE
  boolean:  The return value indicates if metrics are valid.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_get_data_counters
(
  uint8   inst_id,
  uint32  *rx_from_network_byte_cnt, 
  uint32  *tx_to_network_byte_cnt
);

#ifdef FEATURE_TDSCDMA
/*===========================================================================

FUNCTION ds_ucsd_update_network_mode

DESCRIPTION
  This function updates the ds_ucsd_network_mode mode for a call when
  we handle cm connected command.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_update_network_mode
(
  sys_sys_mode_e_type call_mode                        /* network mode */
);
#endif /* FEATURE_TDSCDMA */
/*===========================================================================

FUNCTION ds_ucsd_find_call_instance

DESCRIPTION
  This function finds a previously allocated record in the call
  instance table based on internal CS Data call instance ID.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Index of the call table array element if found,
  MAX_UCSD_CALL_TBL_ENTRIES otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
uint8 ds_ucsd_find_call_instance
(
  uint8    inst_id                              /* CSData call instance ID */
);

/*===========================================================================
FUNCTION ds_ucsd_get_call_tbl

DESCRIPTION
   This function is used to access ds_ucsd_call_tbl.

DEPENDENCIES
   None
  
RETURN VALUE
   ds_ucsd_call_tbl_entry_type *
    
SIDE EFFECTS
   None

===========================================================================*/
ds_ucsd_call_tbl_entry_type * ds_ucsd_get_call_tbl(uint8 index);

/*===========================================================================
FUNCTION ds_ucsd_get_call_inst_from_call_id

DESCRIPTION
   This function returns the UCSD call_isnt corresponding to the 
   cm call_id.

DEPENDENCIES
   None
  
RETURN VALUE
   uint8
    
SIDE EFFECTS
   None

===========================================================================*/
uint8 ds_ucsd_get_call_inst_from_call_id
(
  cm_call_id_type  cm_call_id
);

/*===========================================================================
FUNCTION ds_ucsd_set_call_inst

DESCRIPTION
   This function sets the UCSD call_isnt corresponding to the 
   cm call_id.

DEPENDENCIES
   None
  
RETURN VALUE
   boolean
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_set_call_inst
(
  cm_call_id_type cm_call_id,
  uint8          call_inst
);

/*===========================================================================
FUNCTION ds_ucsd_get_network_mode

DESCRIPTION
   This function returns the CS data network mode.

DEPENDENCIES
   None
  
RETURN VALUE
   sys_sys_mode_e_type
    
SIDE EFFECTS
   None

===========================================================================*/
sys_sys_mode_e_type ds_ucsd_get_network_mode(void);

/*===========================================================================
FUNCTION ds_ucsd_set_network_mode

DESCRIPTION
   This function sets the UCSD network mode.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_ucsd_set_network_mode
(
  sys_sys_mode_e_type network_mode
);

/*===========================================================================
FUNCTION ds_ucsd_get_last_call_info

DESCRIPTION
   This function accesses the UCSD last_call_info struct.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
ds_ucsd_last_call_info_type * ds_ucsd_get_last_call_info(void);

/*===========================================================================
FUNCTION ds_ucsd_get_sync_test_app_enabled

DESCRIPTION
   This function returns the value of ds_ucsd_get_sync_test_app_enabled flag.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_sync_test_app_enabled(void);

/*===========================================================================
FUNCTION ds_ucsd_is_incoming_internal_call_enabled

DESCRIPTION
   This function returns the value of 
   ds_ucsd_is_incoming_internal_call_enabled flag.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_incoming_internal_call_enabled(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION ds_ucsd_get_ucsd_subs_id

DESCRIPTION
   This function returns the value of ds_ucsd_subs_id

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
sys_modem_as_id_e_type ds_ucsd_get_ucsd_subs_id(void);

#endif /* FEATURE_DUAL_SIM */

/*===========================================================================

FUNCTION ds_ucsd_mpshim_configure_l2_watermarks

DESCRIPTION
  This function configure watermarks for raw data calls.  The
  watermarks are configured based on the type of call.  Watermarks for
  other call types (e.g. VT) are configured by the external client.

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_mpshim_configure_l2_watermarks
(
  ds_ucsd_call_tbl_entry_type    *call_info_ptr          /* Call instance  */
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION DS_UCSD_READ_CALLING_PARTY_SUBADDRESS

DESCRIPTION
  This function starts the calling party subaddress reading process.

PARAMETERS
  session_id : Session ID.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_read_calling_party_subaddress
(
  mmgsdi_session_id_type   session_id
);

/*===========================================================================
FUNCTION DS_UCSD_INVALIDATE_SUB_ADDR_INFO()

DESCRIPTION
  This function invalidates the mmgsdi info, because some error occured.

PARAMETERS
  session_type : mmgsdi session type

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_invalidate_sub_addr_info
(
  mmgsdi_session_type_enum_type session_type
);


/*===========================================================================
  FUNCTION DS_UCSD_PROCESS_MMGSDI_CMD()

  DESCRIPTION
    This function is caleld when DS_UCSD_MMGSDI_CMD is received.

  PARAMETERS
    cmd_ptr : command info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_ucsd_process_mmgsdi_cmd
(
  ds_cmd_type  *cmd_ptr
);
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */


#endif /* DSUCSDHDLR_H */
