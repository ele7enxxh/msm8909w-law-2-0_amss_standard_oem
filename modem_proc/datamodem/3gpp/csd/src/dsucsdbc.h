#ifndef DSUCSDBC_H
#define DSUCSDBC_H

/*===========================================================================

              UCSD Bearer Capability Header File

DESCRIPTION
  This file contains the implementation of the UMTS Circuit Switched Data
  Bearer Capability functions

Copyright (c) 2003 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdbc.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/11   TTV     GSDI to MMGSDI_SESSION api migration changes.
09/18/06   AR      Add support for bearer swap/strip for SCUDIF calls.
09/05/06   AR      Added ds_ucsd_validate_repeat_indicator prototype.
11/21/05   AR      Lint corrections.
09/15/05   AR      Add support for service change & fallback.
04/28/05   AR      Add support for exported ATCOP +CV120 parameters.
12/17/04   AR      Add explicit flag for use of LLC in SETUP message.
03/25/03   dgy     Initial version. Merged GSM and WCDMA CS Data mode-
                   specific handlers. Added support for WCDMA Asynchronous
                   CS Data.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#include "cm.h"
#include "nas_exp.h"
#include "ds_cap.h"
#include "dsucsdnt.h"
#include "dsucsdi.h"
#include "ds3gmmgsdiif.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*bearer capability definitions
*******************************/
#define OTHER_RATE_ADAPTION_NOT_APPLICABLE      0xFF

/*define structure to store low layer compatibility*/
#define DSUCSD_NULL_INFO  0
#define DSUCSD_LOW_LAYER_COMPATIBILITY           18
#define DSUCSD_MAX_BCD_PHONE_NUM_LEN        27
#define DSUCSD_MAX_SUBADDRESS_LEN           22

typedef struct
{
   uint8 user_rate;
   uint8 connection_element;
   uint8 sync_async;
} ds_ucsd_supported_capability_length_T;

#ifdef FEATURE_MMGSDI_SESSION_LIB
typedef struct
{
  /* Valid flag - data present is valid or not */
  boolean valid;
  /* session_type - session_type for which sub_addr info belong to */
  mmgsdi_session_type_enum_type session_type;
  /* bcd_num_len - length of the BCD number */
  uint8   bcd_num_len;
  /* sub_addr_len - length of the sub address */
  uint8   sub_addr_len;
  /* bcd_number - stores the BCD number from the SIM */
  uint8   sim_bcd_number[DSUCSD_MAX_BCD_PHONE_NUM_LEN];
  /* sim_subaddress - stores the subaddress */
  uint8   sim_subaddress[DSUCSD_MAX_SUBADDRESS_LEN];
}dsucsd_sim_addr_info_type;

typedef enum
{
  DS_UCSD_READ_FILE_ATTR       = 0,
  DS_UCSD_READ_MSISDN          = 1,
  DS_UCSD_READ_EXT             = 2,
}ds_ucsd_client_ref_e_type;

#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION ds_ucsd_bc_init_orig_params

DESCRIPTION
   Copies default capability and other origination information
   to the input orig_params structure.  The information copied
   depends on user preference setting passed in via the chosen_capability
   parameter.  In addition, the bearer capability, low layer compatibility
   and high layer compatibility information filled in to the orig_params
   structure is copied to the input negotiated_capability structure if
   this parameter is not set to NULL. 
   
   For dual mode calls, it is assumed the preferred mode is Data while
   the alternate mode is Voice.  The call_type parameter indicates the
   modes required for the call.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Changes the orig_params and possibly the negotiated_capability structure
  information.
===========================================================================*/
extern void ds_ucsd_bc_init_orig_params 
(
   ds_ucsd_call_tbl_entry_type       *call_info_ptr,
   const uint8                       *dial_str,                             
   const uint8                        dial_str_len,           
   const sys_sys_mode_e_type          network_mode,
   const ds_ucsd_nt_rlp_parameters_T *rlp_parameters,   
   cm_gw_cs_orig_params_s_type       *orig_params,          
   ds_ucsd_chosen_capability_T       *chosen_capability,
   ds_ucsd_negotiated_capability_T   *negotiated_capability
);


/*===========================================================================

FUNCTION ds_ucsd_bc_check_bc

DESCRIPTION
   Checks the input bearer capabilities and low level compatibilities 
   (contained in the call_info parameter) and determines if the
   all parameters are supported.   If all the parameters are supported (i.e. the
   function returns TRUE), the
   negotiated_capabilities will contain the set of capabilities for the call.
   If the parameters are not supported, the info.gsm_wcdma_call_info.cc_cause
   field of call_info will be updated with a rejection cause value.
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if all of the parameters are supported, FALSE if 
  not. 
    
SIDE EFFECTS
  Changes the call_info and possibly the negotiated_capability structure
  information.
===========================================================================*/
extern boolean ds_ucsd_bc_check_bc 
(
  cm_call_mode_info_s_type           *call_info,
  const cm_bearer_capability_s_type  *data_bc_ptr,
  sys_sys_mode_e_type                 network_mode,
  ds_ucsd_negotiated_capability_T    *negotiated_capability
  );


/*===========================================================================

FUNCTION ds_ucsd_bc_check_and_negotiate_bc

DESCRIPTION
   Checks the input bearer capabilities and low level compatibilities 
   (contained in the call_info parameter) and determines if the
   non-negotiable parameters are supported, and also negotiates any
   negotiable parameters that are not supported.  If any negotiation
   is required, the bc_changed parameter will be set to TRUE, and the
   negotiated_capabilities parameter will contain the negotiated set
   of capabilities.  If negotiation is not required bc_changed will be
   set to FALSE, and negotiated_capabilities will contain the set
   of capabilities for the call.
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if all of the non-negotiable parameters are supported, FALSE if 
  not. 
    
SIDE EFFECTS
  Changes the orig_params and possibly the negotiated_capability structure
  information.
===========================================================================*/
extern boolean ds_ucsd_bc_check_and_negotiate_bc 
(
   cm_call_mode_info_s_type           *call_info,
   const cm_bearer_capability_s_type  *data_bc_ptr,
   ds_ucsd_chosen_capability_T        *chosen_capability,
   ds_ucsd_negotiated_capability_T    *negotiated_capability,
   ds_ucsd_nt_rlp_parameters_T        *rlp_parameters,
   const ds_ucsd_bearer_service_type  *service_ptr,
   boolean                            *bc_changed,
   boolean                             chosen_capability_is_valid,
   sys_sys_mode_e_type                 network_mode,
   sys_modem_as_id_e_type              subs_id
);



/*===========================================================================

FUNCTION ds_ucsd_print_bc_struct

DESCRIPTION
  This function dumps the passed bearer capability information
  element via Diag debug messages.
  
DEPENDENCIES
  None

RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_print_bc_struct
(
   const cm_bearer_capability_s_type *bc
);



/*===========================================================================

FUNCTION ds_ucsd_determine_call_type

DESCRIPTION
  This function determine the internal CS Data call type based on the
  setup paramaters.  The supported call types are defined in
  ds_ucsd_call_type.

  Only single-mode calls (one bearer capability defined) are supported
  at this time.
  
DEPENDENCIES
  NV items for internal multimedia call routing and synchronous call test 
  applicaiotn must be read before calling this routine.
  
RETURN VALUE
  TRUE if operation successful; FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_call_type
ds_ucsd_determine_call_type
(
  const cm_bearer_capability_s_type  *bc1_ptr,     /* Preferred mode BCIE */
  const cm_bearer_capability_s_type  *bc2_ptr,     /* Secondary mode BCIE */
  const cm_repeat_indicator_s_type   *bc_ri_ptr,   /* BC Repeat Indicator */
  const cm_bearer_capability_s_type **data_bc_pptr,/* Data mode BCIE      */
  const cm_bearer_capability_s_type **voice_bc_pptr/* Voice mode BCIE     */
);


#ifdef FEATURE_MULTIMEDIA_FALLBACK 
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */



/*===========================================================================

FUNCTION ds_ucsd_validate_repeat_indicator

DESCRIPTION
  This function validates the SETUP message repeat indicator.
  Alternating modes are not supported at this time EXCEPT for service
  change & fallack for UDI VT calls if SCUDIF is enabled.

DEPENDENCIES
   None

RETURN VALUE
   TRUE if the repeat indicator is supported, FALSE otherwise.

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_validate_repeat_indicator
(
  const cm_call_mode_info_s_type     *mode_info_ptr,
  const cm_bearer_capability_s_type  *data_bc_ptr,
  cm_setup_res_params_s_type         *setup_res_params
);



/*===========================================================================

FUNCTION  ds_ucsd_bc_init

DESCRIPTION
  This function performs powerup initialization steps for this module.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_bc_init( void );

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION DS_UCSD_GET_SUB_ADDR_INFO()

DESCRIPTION
  This function returns the calling party subaddress info.

PARAMETERS
  sub_id          : subscription info.
  sub_address info: sub address info pointer to store the result

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DUAL_SIM
void ds_ucsd_get_sub_addr_info
(
  sys_modem_as_id_e_type     sub_id,
  dsucsd_sim_addr_info_type  *mmgsdi_sim_addr_info
);
#else
void ds_ucsd_get_sub_addr_info
(
  dsucsd_sim_addr_info_type  *mmgsdi_sim_addr_info
);
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */


#endif  /* DSUCSDBC_H */


