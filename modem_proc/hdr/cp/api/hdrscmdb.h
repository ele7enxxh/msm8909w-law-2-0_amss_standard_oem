#ifndef HDRSCMDB_H
#define HDRSCMDB_H

/*===========================================================================

     H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

            D A T A B A S E   E X T E R N A L   I N T E R F A C E
                
DESCRIPTION
  This contains the declarations for the Session Configuration Manager
  Database.

Copyright (c) 2005 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmdb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/14   sju     Added check that AN should send SCC only for restore 
                   prior session with more than 1 personality.
03/21/13   cnx     Write NV when device is powerdown/reset.
01/30/13   vko     Optimize write to EFS 
07/27/11   cnx     Merged Optimized Handoff changes.
05/31/11   cnx     Added hdrscmdb_get_current_personality().
10/30/09   pba     Added ehrpd support.
01/12/09   etv     Disable DTXMode after connection close or fail.
12/04/08   pba     Lint and Compiler warning fixes
05/20/08   pba     Added hdrscmdb_set_amp_inconfig_to_inuse()
05/22/08   pba     Klockwork fixes.
08/08/07   pba     Added hdrscmdb_set_pktapp_protocols_to_default().
                   Added hdrscmdb_set_csna_protocol_to_default().
06/19/07   cc      Added DOrB RMAC4 support.
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
03/27/07   wsh     Added support for new prior session handling
03/21/07   pba     Added hdrscmdb_negotiated_session_is_valid().
03/06/07   wsh     Removal of RVCT compiler warnings
12/19/06   pba     Allow  only one personality at max to have non-default 
                   security layer protocol subtypes.
10/16/06   etv     Added support for LOG_HDR_SUBTYPE_STREAM_CONFIG_C log pkt.
10/04/06   pba     Since now we write only during graceful powerdwon, removed
                   code that delays write to EFS on back-to-back GAUP.
06/20/06   pba     Removed redundant AN auth status maintained by SCP.
                   Added changes to update config lock value in SCMDB.
06/05/06   pba     Added support to store AMP, LUP, FCP and DOS session data
                   in EFS.
06/05/06   pba     Updated hdrscmdb_personality_is_valid() routine to also be
                   used for checking validity of prior session personalities.
05/24/06   pba     Added hdrscmdb_get_negotiated_app_subtypes() routine to 
                   return a list with the names of all the negotiated app 
                   subtypes itself instead of a bit mask.
05/03/06   pba     Added support to avoid too many back-to-back writes to EFS.
04/27/06   pba     Added new API that update SCMDB's copy of inuse PCCC value.
04/21/06   hal     Extended existing APIs to include session change and session
                   close reasons
04/17/06   pba     Added APIs to support accepting/proposing only AT supported
                   stream configurations in multiple personality scenario.
03/22/06   pba     Added support for DeletePersonality.
02/01/06   pba     Fixes for HardLink subtype negotiation.
12/13/05   pba     Added support for multiple personality negotiation
11/07/05   pba     Code cleanup
11/04/05   etv     Added function for updating the default values
11/02/05   etv     Changed SCM design to propogate subtype info.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
                   for legacy protocols
08/22/05   pba     Initial SCM DB Module support
07/18/05   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "hdrhai.h"
#include "hdrerrno.h"

#include "hdrscp.h"
#include "hdramp.h"
#include "hdrlup.h"
#include "hdridlec.h"
#include "hdrscmidle.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/* Maximum number of personalities supported */
#define HDRSCMDB_MAX_PERSONALITIES 4


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  HDRSCMDB_SESSION_INVALID = 0,
  HDRSCMDB_SESSION_PREV_NO_SILENT_RESTORE,
  HDRSCMDB_SESSION_PREVIOUS,  
  HDRSCMDB_SESSION_CURRENT
} hdrscmdb_session_db_status_enum_type;


/*============================================================================

                      FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
FUNCTION HDRSCMDB_SET_SESSION_CONFIG_TOKEN

DESCRIPTION
  This function sets the inuse SessionConfigToken to the given value
  
DEPENDENCIES
  None.

PARAMETERS
  sess_cfg_token - Value to which inuse session config token have to set to
     
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_session_config_token(uint16 sess_cfg_token);

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_ATTRIB
  
DESCRIPTION
  This function gets the inuse value of the attribute ID. The value is 
  returned as pointer to the data item.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name  - The protocol that the attribute is for
  attrib_id      - AttributeID
  attrib_val_ptr - Pointer to the attibute value, for complex attribute this 
                  points to struct.
  subtype        - Negotiated subtype of the protocol
  
RETURN VALUE
  Returns the version number of the attribute.

SIDE EFFECTS
  None
=============================================================================*/
uint8 hdrscmdb_get_inuse_attrib
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         attrib_id,
  void                           *attrib_val_ptr,
  uint16                         subtype
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_CONFIG
  
DESCRIPTION
 This function returns the given protocol's inuse configuration parameters.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name    - The protocol that the attribute is for
  config_data_ptr  - Pointer to the protocol configuration structure where
                     the configuration data need to be copied to
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_inuse_config
(
  hdrhai_protocol_name_enum_type protocol_name,
  void                           *config_data_ptr
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_CONFIG_AND_VERNO
  
DESCRIPTION
 This function returns the given protocol's inuse configuration and the 
 version numbers for all attribute's of the protocol.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name     - The protocol that the attribute is for
  config_data_ptr   - Pointer to the protocol configuration structure where
                      the configuration data need to be copied to
  attrib_verno      - Pointer to attribute version number structure to which
                      version information need to be copied to
  attrib_verno_size - Size of protocol's attrib version number structure in
                      bytes
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_inuse_config_and_verno
(
  hdrhai_protocol_name_enum_type  protocol_name,
  void                           *config_data_ptr, 
  void                           *attrib_verno,
  uint16                          attrib_verno_size
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_RMAC_INUSE_CONFIG_AND_VERNO
  
DESCRIPTION
 This function returns the RMAC's inuse configuration and the version numbers
 for all attribute's of the protocol.

DEPENDENCIES
  None.

PARAMETERS
  cfg_data_ptr   - Pointer to the RMAC's configuration structure where
                   the configuration data need to be copied to
  cfg_data_size  - Size of configuration data                 
  attrib_verno   - Pointer to attribute version number structure to which
                   version information need to be copied to
  attrib_verno_size - Size of RMAC's attrib version number structure in
                      bytes
  subtype        - RMAC's subtype                      
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_rmac_inuse_config_and_verno
(
  void                           *cfg_data_ptr, 
  uint32                          cfg_data_size,
  void                           *attrib_verno,
  uint16                          attrib_verno_size,
  uint16                          subtype

);

/*=============================================================================
FUNCTION HDRSCMDB_GET_AMP_SESSION_INFO
  
DESCRIPTION
 This function copies AMP's inuse configuration information from SCMDB to the 
 location specified by the pointer passed.
  
DEPENDENCIES
  None.

PARAMETERS
  amp_cfg_ptr - Pointer to AMP's inuse configuration parameters.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_amp_session_info( hdramp_session_info_type *amp_cfg_ptr );

/*=============================================================================
FUNCTION HDRSCMDB_SET_AMP_SESSION_INFO
  
DESCRIPTION
 This function copies the AMP's configuration parameters to SCMDB.
 
DEPENDENCIES
  None.

PARAMETERS
 amp_cfg_ptr - Pointer to AMP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_amp_session_info
( 
  hdramp_session_info_type *amp_cfg_ptr
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_SCP_SESSION_INFO
  
DESCRIPTION
 This function copies the SCP's configuration parameters from SCMDB to the 
 location specified by the pointer passed.
 
DEPENDENCIES
  None.

PARAMETERS
 scp_cfg_ptr - Pointer to SCP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_scp_session_info(hdrscp_config_params_type *scp_cfg_ptr);

/*=============================================================================
FUNCTION HDRSCMDB_SET_SCP_SESSION_INFO
  
DESCRIPTION
  This function sets the SCP's session information to the vaules given
 
DEPENDENCIES
  None.

PARAMETERS
 scp_cfg_ptr - Pointer to SCP's inconfig configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_scp_session_info(hdrscp_config_params_type *scp_cfg_ptr);

/*=============================================================================
FUNCTION HDRSCMDB_GET_LUP_SESSION_INFO
  
DESCRIPTION
 This function copies LUP's inuse configuration information from SCMDB to the 
 location specified by the pointer passed.
  
DEPENDENCIES
  None.

PARAMETERS
  lup_cfg_ptr - Pointer to LUP's inuse configuration parameters.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_lup_session_info( hdrlup_session_info_type *lup_cfg_ptr );

/*=============================================================================
FUNCTION HDRSCMDB_SET_LUP_SESSION_INFO
  
DESCRIPTION
 This function copies the LUP's configuration parameters to SCMDB.
 
DEPENDENCIES
  None.

PARAMETERS
 lup_cfg_ptr - Pointer to LUP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_lup_session_info
( 
  hdrlup_session_info_type *lup_cfg_ptr
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_SCP_SESSION_INFO
  
DESCRIPTION
 This function copies the SCP's configuration parameters from SCMDB to the 
 location specified by the pointer passed.
 
DEPENDENCIES
  None.

PARAMETERS
 scp_config_ptr - Pointer to SCP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_scp_prev_session_info
(
  hdrscp_config_params_type *scp_cfg_ptr
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_PREVIOUS_CONFIG
  
DESCRIPTION
  This function returns the given protocol's previous configuration parameters.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name    - Protocol for which previous config is needed
  config_data_ptr  - Pointer to given protocol's configuration structure where
                     the configuration data need to be copied to.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_previous_config
(
  hdrhai_protocol_name_enum_type protocol_name,
  void                           *config_data_ptr
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_IDLE_SLOT_CYCLE_TIMEOUT
  
DESCRIPTION
  This function sets the inuse and negotiated value of IDLE slot cycle 
  timeout to the value given.

DEPENDENCIES
  None.

PARAMETERS
   t1 - System time at which to change from SlotCycle1 to SlotCycle2
   t2 - System time at which to change from SlotCcyle2 to SlotCycle3
  
RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrscmdb_set_idle_slot_cycle_timeout( uint64 t1, uint64 t2 );

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_ATTRIB

DESCRIPTION
  This function sets the inconfig instance of the specified attribute to the 
  value given.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name   - The protocol that the attribute belongs to
  attrib_id       - AttributeID
  value_id        - Value for simple attributes, ValueID for complex attributes
  complex_val_ptr - Pointer to complex data value, or NULL for simple attrib
  subtype         - Negotiated subtype of the protocol
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inconfig_attrib
( 
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         attrib_id,
  uint16                         value_id,
  void                           *complex_val_ptr,
  uint16                         subtype
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_GAUPED_ATTRIB

DESCRIPTION
  This function is called to update the database of gauped attribute
   
DEPENDENCIES
  None.

PARAMETERS
  protocol_name   - The protocol that the attribute is for
  attrib_id       - AttributeID
  value_id        - Value for simple attributes, ValueID for complex attributes
  complex_val_ptr - Pointer to complex data value, or NULL for simple attrib
  subtype         - Negotiated subtype of the protocol
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_gauped_attrib
( 
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         attrib_id,
  uint16                         value_id,
  void                           *complex_val_ptr,
  uint16                         subtype
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_GAUPED_CONFIG

DESCRIPTION
  This function is called to update the database of gauped config attributes

DEPENDENCIES
  None.

PARAMETERS
  protocol_name    - The protocol that the attribute is for
  config_data_ptr  - Pointer to the protocol's current configuration data
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_gauped_config
( 
  hdrhai_protocol_name_enum_type protocol_name,
  void                           *config_data_ptr
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_ATTRIB_TO_INUSE

DESCRIPTION
  This function sets the inconfig instance of the specified attribute to the
  inuse (fall-back) value.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - The protocol that the attribute is for
  attrib_id           - AttributeID
  subtype             - Negotiated subtype of the protocol
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inconfig_attrib_to_inuse
( 
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         attrib_id,
  uint16                         config_subtype
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_TO_INUSE

DESCRIPTION
  This function sets the value of the "in config" instances to the inuse 
  values e.g. at the beginning of negotiation.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inconfig_to_inuse( void );

/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  attrib_verno_val    - inuse version number for the attributes.
  attrib_verno_offset - Offset of each attribute in its version number list
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
);

/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_NN_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per flow attributes.
 (Example: RMAC's AssociatedFlowsNN)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_flownn          - Number of flows
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given flowNN 
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_flownn"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_nn_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_flownn,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
);

#ifdef FEATURE_HDR_REVB
/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_CC_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per Reverse Logical Channel attributes.
 (Example: RMAC4's MaxNumSubPacketsCC)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_channelcc       - Number of Reverse Logical Channels
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given CC 
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_channelcc"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_cc_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_channelcc,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
);
/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_XX_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per Configuration attributes.
 (Example: RMAC4's PilotStrengthConfigXX)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_configxx        - Number of configurations
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given configXX
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_configxx"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_xx_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_configxx,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
);
/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_NC_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per flow attributes.
 (Example: RMAC's AssociatedFlowsNN)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_flownn          - Number of flows
  num_channelcc       - Number of Reverse Logical Channels
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given NC
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_flownn" X "num_channelcc"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_nc_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_flownn,
  uint8                          num_channelcc,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
);
#endif /* FEATURE_HDR_REVB */
/*=============================================================================
FUNCTION HDRSCMDB_PROTOCOL_SUBTYPE_CHANGED
  
DESCRIPTION
  This function is called by SCP on subtype change for a given protocol during
  SCP subtype negotiation.

  When subtype changes to hardlink (0xFFFE), the inconfig instance of the
  protocol is set to the negotiated instance of the main personality.

  When subtype changes to other than hardlink, the inconfig instance is
  set to the default values of the new subtype.
  Note: Second case call is needed for protocols that store the config
  attributes as a union of config structs of subtypes in order to save
  space. For most protocols whose config struct includes a separate
  instance of config attributes for each subtype, this call is not needed.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol for which the subtype has changed. 
  subtype  - Negotiated subtype for the protocol.
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_protocol_subtype_changed
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         subtype
);

/*=============================================================================
FUNCTION HDRSCMDB_SESSION_CONFIG_TOKEN_CHANGED

DESCRIPTION

DEPENDENCIES
  Before calling this function current personality index in DB should be 
  updated by calling hdrscmdb_set_inuse_personality()

PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_session_config_token_changed
(
  hdrhai_session_change_reason_enum_type change_reason
);

/*=============================================================================
FUNCTION HDRSCMDB_PROCESS_CONFIG_COMPLETE

DESCRIPTION
  This function is called by SCP on receiving a soft config complete or
  config complete message.

DEPENDENCIES
  None.

PARAMETERS
  pindex_store - Personality index store in Soft CC message or Session config
                 token in Config complete message
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_process_config_complete( uint8 pindex_store );

/*=============================================================================
FUNCTION HDRSCMDB_PROCESS_PREVIOUS_SESSION_OPENED

DESCRIPTION
  This function is called by SCP when AT wants to restore its previous session.

DEPENDENCIES
  None.

PARAMETERS
  None  
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_process_previous_session_opened( void );

/*=============================================================================
FUNCTION HDRSCMDB_PROCESS_SESSION_CLOSED

DESCRIPTION
  This function is called by SMP when current session is closed.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_process_session_closed
( 
  hdrhai_session_close_reason_enum_type close_reason
);

/*=============================================================================
FUNCTION HDRSCMDB_POWERUP_INIT

DESCRIPTION
  This function does initialization of SCMDB module during powerup.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  TRUE if session is active and read from EFS succeeded, FALSE otherwise.

SIDE EFFECTS
  Modifies the database of configuration information.
=============================================================================*/
boolean hdrscmdb_powerup_init( void );

/*=============================================================================
FUNCTION HDRSCMDB_GET_MAIN_PERSONALITY_PS

DESCRIPTION
  This function returns the subtype of protocols in main personality.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_subtype - Array in which subtype of protocol's in main personality
                     is returned to the calling function.
  
RETURN VALUE
  E_SUCCESS if the main personality is valid, E_FAILURE otherwise.

SIDE EFFECTS
  None
=============================================================================*/
hdrerrno_enum_type hdrscmdb_get_main_personality_ps(uint16 *protocol_subtype);

/*=============================================================================
FUNCTION HDRSCMDB_GET_CURRENT_PERSONALITY

DESCRIPTION
 This function is a wrapper function of hdrscmdb_get_inuse_personality().
 It returns the inuse current personality index.
  
DEPENDENCIES
  None.

PARAMETERS
  None  
  
RETURN VALUE
  Current personality index aka session config token 

SIDE EFFECTS
  None 
=============================================================================*/
uint16 hdrscmdb_get_current_personality( void );

/*=============================================================================
FUNCTION HDRSCMDB_DELETE_PERSONALITIES

DESCRIPTION
  This function sets the specified personalities as deleted.
    
DEPENDENCIES
  None.

PARAMETERS
  personalities - Array of personality indexes to be deleted.
  personality_count - Number of personalities to delete.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_delete_personalities
( 
  uint8 personalities[], 
  uint8 personality_count 
);

/*=============================================================================
FUNCTION HDRSCMDB_PERSONALITY_IS_VALID

DESCRIPTION
  This function determines if the personality at given index is valid or not.
    
DEPENDENCIES
  None.

PARAMETERS
  personality_index - Index of the personality that need to checked.
  check_curr_session- If this is set to TRUE by the calling routine personality
          validity check is done for current session otherwise check is done in
          previous session.
  
RETURN VALUE
  TRUE if the personality is valid, FALSE otherwise.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_personality_is_valid
( 
  boolean check_curr_session,
  uint8    personality_index
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_NEGOTIATED_CONFIG

DESCRIPTION
  This function returns a pointer to the given protocol's negotiated
  configuration instance in a given personality.
    
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - The protocol whose configuration is requested.
  pindex        - Personality index from which protocol's configuration
                  data is to be retrieved.
  
RETURN VALUE
  Returns a pointer to protocol's negotiated configuration in database.

SIDE EFFECTS
  None
=============================================================================*/
void const* hdrscmdb_get_negotiated_config
(
  hdrhai_protocol_name_enum_type  protocol_name,
  uint16                          pindex
);

/*=============================================================================
FUNCTION HDRSCMDB_GET_NEGOTIATED_APP_SUBTYPES

DESCRIPTION
  This function returns the list of application subtypes that have been 
  negotiated.
  
DEPENDENCIES
  None.

PARAMETERS
  app_subtypes      - List of application subtypes that have been negotiated.  
                      This function fills up this array and passes it back to
                      the caller.
  num_app_subtypes  - Number of application subtypes that are included in
                      "app_subtypes" array
    
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_negotiated_app_subtypes
( 
  uint16 *app_subtypes, 
  uint8  *num_app_subtypes
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_CURRENT_PCCC_TO_DEFAULT

DESCRIPTION
  This function is called by IDLE to update the SCMDB's inuse copy of PCCC
  value. 
    
DEPENDENCIES
  None.
    
PARAMETERS
  curr_pccc_value - Current PCCC value.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_current_pccc_to_default( uint16 curr_pccc_value );

/*=============================================================================
FUNCTION HDRSCMDB_UPDATE_CONFIG_LOCK

DESCRIPTION
  This function updates the ConfigLock value in SCMDB.
  
DEPENDENCIES
  None.
    
PARAMETERS
  config_lock_val - Value of config lock
     
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_update_config_lock(boolean config_lock_val);

/*=============================================================================
FUNCTION HDRSCMDB_WRITE_ALL_CONFIG_DATA

DESCRIPTION
  This function writes the database of session information to EFS.

DEPENDENCIES
  None.

PARAMETERS
  None.
                          
RETURN VALUE
  E_SUCCESS if database was successfully written to EFS file. E_FAILURE 
  otherwise.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
hdrerrno_enum_type hdrscmdb_write_all_config_data( void );

/*=============================================================================
FUNCTION HDRSCMDB_LOG_PERS_SUBTYPE_STREAM_CONFIG

DESCRIPTION
  This function is used to log the protocol subtype information and stream 
  configuration for each of the negotiated personalities and the personality 
  index of the current personality.

DEPENDENCIES
  None.
    
PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_log_pers_subtype_stream_config( void );

/*=============================================================================
FUNCTION HDRSCMDB_NONDEFAULT_SECURITY_PS_IS_ALLOWED

DESCRIPTION
  This function checks if the AT is allowed to propose/accept non-default
  protocol subtype for a given security layer protocol.
  
DEPENDENCIES
  None.
    
PARAMETERS
  protocol_name - Name of the protocol
       
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_nondefault_security_ps_is_allowed
( 
  hdrhai_protocol_name_enum_type protocol_name 
);

/*=============================================================================
FUNCTION HDRSCMDB_NEGOTIATED_SESSION_IS_VALID

DESCRIPTION
  This function checks if the negotiated session is valid or not.

  Negotiated session is invalid if there are 
  1) More than one instance of MPA/EMPA.
  2) More than one instance of DPA and they are not hardlinked.
  3) More than one instance of non-default security protocol and they are not 
     hardlinked.

DEPENDENCIES
  None.
    
PARAMETERS
  None
       
RETURN VALUE
  TRUE if the session is valid, FALSE otherwise.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_negotiated_session_is_valid( void );

/*=============================================================================
FUNCTION HDRSCMDB_PRIOR_SESSION_IS_VALID

DESCRIPTION
  This function checks if previous session is valid

DEPENDENCIES
  None.
    
PARAMETERS
  None.
    
RETURN VALUE
  Return if previous session is valid.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_prior_session_is_valid( void );

/*=============================================================================
FUNCTION HDRSCMDB_SET_PRIOR_SESSION_VALIDITY

DESCRIPTION
  This function sets the validity of previous session. Simplying setting
  previous session to valid does not gurantee hdrscmdb_previous_session_is_valid()
  will return true. KA time must not have passed.

DEPENDENCIES
  None.
    
PARAMETERS
  prev_session_valid - whether current session is valid
    
RETURN VALUE
  None

SIDE EFFECTS
  Setting Previous session to Valid may invalidate current session.
=============================================================================*/
void hdrscmdb_set_prior_session_validity
(
  boolean prev_session_valid
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_CURRENT_SESSION_VALIDITY

DESCRIPTION
  This function sets the validity of current session

DEPENDENCIES
  None.
    
PARAMETERS
  Valid - whether current session is set to valid
    
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_current_session_validity
(
  boolean valid
);

/*=============================================================================
FUNCTION HDRSCMDB_IS_SILENT_RESTORE_ENABLED

DESCRIPTION
  This function checks if previous session can be silently restored.

DEPENDENCIES
  None.
    
PARAMETERS
  None.
    
RETURN VALUE
  True - prior session can be silently restored
  False - Otherwise

SIDE EFFECTS
  None
=============================================================================*/
extern boolean hdrscmdb_is_silent_restore_enabled( void );

/*=============================================================================
FUNCTION HDRSCMDB_DISABLE_SILENT_RESTORE

DESCRIPTION
  This function disables silent restore of prior session.

DEPENDENCIES
  None.
    
PARAMETERS
  None.
    
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_disable_silent_restore( void );

/*===========================================================================

FUNCTION HDRSCMDB_SET_IDLE_SLOTTED_MODE

DESCRIPTION
  This function updated the scmdb with given slottedmode value.

DEPENDENCIES
  None

PARAMETERS
   saved_slotted_mode_config - saved slottedmode attribute value 

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmdb_set_idle_scmidle_info
(
  hdridlec_saved_scmidle_info_type *saved_scmidle_info
);

/*=============================================================================
FUNCTION HDRSCMDB_SET_PKTAPP_PROTOCOLS_TO_DEFAULT

DESCRIPTION
  This function sets the inconfig instance of packet application 
  protocols (LUP, DOS, FCP, MRLP) to default values.

DEPENDENCIES
  None
 
PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hdrscmdb_set_pktapp_protocols_to_default( void );

/*=============================================================================
FUNCTION HDRSCMDB_SET_CSNA_PROTOCOL_TO_DEFAULT

DESCRIPTION
  This function sets the inconfig instance of CSNA protocol to default values.

DEPENDENCIES
  None
 
PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hdrscmdb_set_csna_protocol_to_default( void );

/*=============================================================================
FUNCTION HDRSCMDB_SET_AMP_INCONFIG_TO_INUSE

DESCRIPTION
  This function sets the value of the AMP "in config" instances to the inuse 
  values e.g. at the beginning of negotiation.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_amp_inconfig_to_inuse( void );

/*=============================================================================
FUNCTION HDRSCMDB_RESET_DTX_MODE

DESCRIPTION
  This function is called to reset the physical layer attribute dtx_mode. When
  AT goes back to idle from connected state or connection set up state dtx mode
  should be reset implicitly (that is without any AUR message from AN).
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_reset_dtx_mode( void );

/*=============================================================================
FUNCTION HDRSCMDB_SET_INUSE_TO_DEFAULT

DESCRIPTION
  This function sets the protocol's inuse configuration value to default
  values.

DEPENDENCIES
  None.

PARAMETERS
  None.
   
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inuse_to_default( void );


#ifdef FEATURE_EHRPD
/*=============================================================================
FUNCTION HDRSCMDB_AT_SESSION_HAS_EHRPD_PERSONALITY

DESCRIPTION
  This function checks whether AT has a valid eHRPD personality negotiated in 
  the current session.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  TRUE if AT has a valid eHRPD personality. FALSE othewise.

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrscmdb_at_session_has_ehrpd_personality( void );

#endif /* FEATURE_EHRPD */

/*=============================================================================
FUNCTION HDRSCMDB_DELETE

DESCRIPTION
  This function deletes the database information stored in EFS.

DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_delete( void );

/*=============================================================================
FUNCTION HDRSCMDB_EFS_IS_UP_TO_DATE

DESCRIPTION
  This function gets the variable, efs_is_up_to_date current value.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  TRUE - scmdb and the db information stored on EFS is in sync.
  FALSE - not in sync

SIDE EFFECTS
  NONE
=============================================================================*/
boolean hdrscmdb_efs_is_up_to_date( void );

#endif /* HDRSCMDB_H */

