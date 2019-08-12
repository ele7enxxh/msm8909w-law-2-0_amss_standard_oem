#ifndef _DS_QMI_NAS_UTILS_H
#define _DS_QMI_NAS_UTILS_H

/*===========================================================================

                         DS_QMI_NAS_UTILS.H

DESCRIPTION

 The Qualcomm Network Access Services Utility header file.

Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_utils.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/04/10    hs     Initial version
===========================================================================*/

#include "qmi_nas.h"
#include "qmi_nas_internal.h"
#include "qmi_nas_msg.h"

/*===========================================================================
Macro QMI_NAS_HTONS()
===========================================================================*/
/**
  Converts host-to-network short integer. Handles potential byte order
  differences between different computer architectures and different network
  protocols.

  @param[in] x Unsigned short integer value to be converted.

  @return
  The network byte-ordered value.
*/
#define qmi_nas_htons(x)                                                      \
  (((((uint16)(x) & 0x00FF) << 8) | (((uint16)(x) & 0xFF00) >> 8)))

/*===========================================================================
  FUNCTION QMI_NAS_BINARY_TO_ASCII()

  DESCRIPTION
    This function is used to convert a binary value to an ASCII string 
    
  PARAMETERS
    value        : binary value to convert
    buffer       : buffer to put value in
    buf_len      : length of buffer

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_binary_to_ascii(uint32 value, char* buffer, uint16 buf_len);

/*===========================================================================
  FUNCTION QMI_NAS_ASCII_TO_BINARY()

  DESCRIPTION
    This function is used to convert an ASCII string to binary values
    
  PARAMETERS
    value        : binary value to convert
    buffer       : buffer to put value in
    buf_len      : length of buffer

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qmi_nas_ascii_to_binary(uint8* buffer, uint16 buf_len);

/*===========================================================================
  FUNCTION QMI_NAS_STRING_IS_NUMERIC()

  DESCRIPTION
    This function is used to verify a character string is composed entirely
    of ASCII characters
    
  PARAMETERS
    value        : binary value to convert
    buffer       : buffer to put value in
    buf_len      : length of buffer

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_string_is_numeric(uint8* buffer, uint16 buf_len);

/*===========================================================================
  FUNCTION qmi_nas_byte_array_compare()

  DESCRIPTION
    comparison function for qsort() used for char array.
    
  PARAMETERS
    check stdlib.h

  RETURN VALUE
    -1 if *p_a < *p_b, 0 if *p_a == *p_b, 1 if *p_a > *p_b
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_nas_byte_array_compare( const void *p_a, const void *p_b );

/*===========================================================================
  FUNCTION qmi_nas_int16_array_compare()

  DESCRIPTION
    comparison function for qsort() used for (singend) int16 array.
    
  PARAMETERS
    check stdlib.h

  RETURN VALUE
    -1 if *p_a < *p_b, 0 if *p_a == *p_b, 1 if *p_a > *p_b
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_nas_int16_array_compare( const void *p_a, const void *p_b );
/*===========================================================================
  FUNCTION qmi_nas_sys_mode_2_radio_if()

  DESCRIPTION
    convert sys_mode enum to radio_if enum
    
  PARAMETERS
    cm_mm_msim_ss_info_s_type, sys_sys_mode_e_type, if the stack needs to be in service

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_sys_mode_2_radio_if( cm_mm_msim_ss_info_s_type *p_info, enum qm_stack_index_e stack, boolean needs_to_be_in_service );

/*===========================================================================
  FUNCTION qmi_nas_radio_if_2_sys_radio_access()

  DESCRIPTION
    convert radio_if enum to sys_radio_access_tech enum
    
  PARAMETERS
    radio_if to translate

  RETURN VALUE
    sys_radio_access_tech_e_type
===========================================================================*/
sys_radio_access_tech_e_type qmi_nas_radio_if_2_sys_radio_access( nas_radio_if_enum_v01 radio_if );

/*===========================================================================
  FUNCTION qmi_nas_sys_radio_access_2_radio_if()

  DESCRIPTION
    convert sys_radio_access_tech enum to radio if enum
    
  PARAMETERS
    sys_radio_access to translate

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_sys_radio_access_2_radio_if( sys_radio_access_tech_e_type sys_radio_access );

/*===========================================================================
  FUNCTION qmi_nas_get_supported_technology_from_nv_pref_mode()

  DESCRIPTION
    split up nv pref_mode into access technologies
    
  PARAMETERS
    nv_pref_mode, access_tech_s container to hold split information

  RETURN VALUE
    TRUE if something understood, FALSE otherwise
===========================================================================*/
boolean qmi_nas_get_supported_technology_from_nv_pref_mode( nv_mode_enum_type pref_mode, struct access_tech_s *p );

/*===========================================================================
  FUNCTION qmi_nas_cm_bc_2_qmi_bc()

  DESCRIPTION
    convert sys_band_class_e_type to enum qmi_nas_bc_e
    
  PARAMETERS
    cm band class enum

  RETURN VALUE
    corresponding qmi band class enum value or;
    QMI_NAS_BC_MAX if error
===========================================================================*/
enum qmi_nas_bc_e qmi_nas_cm_bc_2_qmi_bc( sys_band_class_e_type cm_bc );

/*===========================================================================
  FUNCTION qmi_nas_cm_2_qmi_hdr_personality()

  DESCRIPTION
    convert sys_personality_e_type to enum qmi_nas_hdr_personality_e
    
  PARAMETERS
    sys_personality_e_type

  RETURN VALUE
    corresponding qmi_nas_hdr_personality_e
===========================================================================*/
enum qmi_nas_hdr_personality_e qmi_nas_cm_2_qmi_hdr_personality( sys_personality_e_type cm_val );

/*===========================================================================
  FUNCTION qmi_nas_strnlen()

  DESCRIPTION
    QMI_NAS local version of strnlen
    
  PARAMETERS
    - src_ptr  : pointer to start of the source buffer to calculate str len
    - nax_len  : maximum length to check for

  RETURN VALUE
   length of the string in the src buffer
===========================================================================*/
uint32 qmi_nas_strnlen ( const char *src_ptr, uint32 max_len);

/*===========================================================================
  FUNCTION convert_endianness16()

  DESCRIPTION
    Convert endianness of 16bit character string by swapping first and second
    byte of each character

  PARAMETERS
    buf: ptr to buffer that will be converted.
    size: size of 'buf' in terms of number of bytes. 'size' is expected to be
          an even number. if not, last byte at the end will be left untouched
===========================================================================*/
void convert_endianness16( unsigned char *buf, unsigned int size );

/*===========================================================================
  FUNCTION qmi_nas_eutra_status_cm_2_qmi()

  DESCRIPTION
    convert sys_eutra_status_enum_type to nas_eutra_cell_status_enum_type_v01
    
  PARAMETERS
    cm eutra status enum

  RETURN VALUE
    corresponding qmi eutra status enum value
===========================================================================*/
nas_eutra_cell_status_enum_type_v01 qmi_nas_eutra_status_cm_2_qmi( sys_eutra_status_enum_type cm_eutra_status );

/*===========================================================================
  FUNCTION qmi_nas_validate_cmd_buf_p()

  DESCRIPTION
    Function to validate returned cmd_buf_p is still allocated in client.

  PARAMETERS
    cmd_buf_p: pointer to queue element for comparison
    nas_state: pointer to qmi_nas state
===========================================================================*/
boolean qmi_nas_validate_cmd_buf_p( qmi_cmd_buf_type *cmd_buf_p, qmi_nasi_state_type * qmi_nas_state );

/*===========================================================================
  FUNCTION qmi_nas_rat_qmi_to_mmgsdi()

  DESCRIPTION
    Translate QMI_NAS RAT to MMGSDI RAT enum type

  PARAMETERS
    qmi_rat: QMI_NAS Radio access tech
===========================================================================*/
mmgsdi_rat_enum_type qmi_nas_rat_qmi_to_mmgsdi( enum qmi_nas_radio_if_e qmi_rat );

/*===========================================================================
  FUNCTION qmi_nas_rat_mmgsdi_to_qmi()

  DESCRIPTION
    Translate MMGSDI RAT enum type to QMI_NAS RAT

  PARAMETERS
    mmgsdi_rat: MMGSDI Radio access tech
===========================================================================*/
enum qmi_nas_radio_if_e qmi_nas_rat_mmgsdi_to_qmi( mmgsdi_rat_enum_type mmgsdi_rat );

#ifdef FEATURE_MMODE_REPORT_POWERSAVE
/*===========================================================================
  FUNCTION qmi_nas_get_srv_status_from_cmss_event()

  DESCRIPTION
    Function to get srv_status for different RATs by looking up CMSS Event.

  PARAMETERS
    p_info: pointer to latest cm ss event
    stack_index: stack to look into in latest cm ss event
    sys_mode: sys_mode/RAT for which we need to determine service status
===========================================================================*/
nas_service_status_enum_type_v01 qmi_nas_get_srv_status_from_cmss_event( cm_mm_msim_ss_info_s_type *p_info, uint8 stack_index, sys_sys_mode_e_type sys_mode );
#endif // FEATURE_MMODE_REPORT_POWERSAVE

/*===========================================================================
  FUNCTION qmi_nas_check_sys_info_chg_against_rpt_limits()

  DESCRIPTION
    Function to determine if fields under limited reporting changed.

  PARAMETERS
    
===========================================================================*/
boolean qmi_nas_check_sys_info_chg_against_rpt_limits( nas_sys_info_ind_msg_v01 *new_ind, nas_sys_info_ind_msg_v01 *old_ind, uint64 sys_info_limits );

/*===========================================================================
  FUNCTION qmi_nas_rat_cm_scan_to_mmgsdi()

  DESCRIPTION
    Translate CM Scan Result RAT to MMGSDI RAT enum type

  PARAMETERS
    cm_rat: CM Radio access tech
===========================================================================*/
mmgsdi_rat_enum_type qmi_nas_rat_cm_scan_to_mmgsdi( sys_radio_access_tech_e_type cm_rat );

/*===========================================================================
  FUNCTION qmi_nas_rat_cm_mode_to_mmgsdi()

  DESCRIPTION
    Translate CM sys mode RAT to MMGSDI RAT enum type

  PARAMETERS
    cm_rat: CM Radio access tech
===========================================================================*/
mmgsdi_rat_enum_type qmi_nas_rat_cm_mode_to_mmgsdi( sys_sys_mode_e_type cm_rat );

/*===========================================================================
FUNCTION  QMI_NAS_PRINT_CLIENT_STATE

DESCRIPTION
  Prints the current status of qmi_nas_state for all valid clients

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None
==============================================================================*/
void qmi_nas_print_client_state( qmi_nasi_state_type * qmi_nas_state );

/*===========================================================================
  FUNCTION qmi_nas_qmi_sys_lte_bc_2_lte_bc()

  DESCRIPTION
    convert sys_sband_lte_e_type to nas_active_band_enum_v01 
    
  PARAMETERS
    qmi band class enum

  RETURN VALUE
    corresponding sys band class enum value or;
    SYS_SBAND_LTE_EUTRAN_BAND_MAX if error
===========================================================================*/
nas_active_band_enum_v01  qmi_nas_qmi_sys_lte_bc_2_lte_bc( sys_sband_lte_e_type sys_lte_bc );

/*===========================================================================
  FUNCTION qmi_nas_radio_if_2_sys_mode()

  DESCRIPTION
    Converts nas_radio_if_enum_v01 type to sys_sys_mode_e_type
 
  PARAMETERS
    radio_if: RAT to convert
===========================================================================*/
sys_sys_mode_e_type qmi_nas_radio_if_2_sys_mode( nas_radio_if_enum_v01 radio_if );

/*===========================================================================
  FUNCTION qmi_nas_convert_pref_to_mask()

  DESCRIPTION
    Converts ims preference to preference mask
 
  PARAMETERS
    old_mask: old mask value to add on to
    call_type_mask: call type for preference
    is_ims_pref: whether IMS is preferred
===========================================================================*/
uint64 qmi_nas_convert_pref_to_mask( uint64 old_mask, cm_call_type_mask_type call_type_mask, cm_call_type_mask_type ims_pref_mask );

/*===========================================================================
  FUNCTION qmi_nas_map_voice_sms_status_cm_to_qmi()

  DESCRIPTION
    Maps CM domain status to QMI_NAS enum type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_voice_sms_status_e_type qmi_nas_map_voice_sms_status_cm_to_qmi( sys_cm_domain_sel_domain_e_type domain );

/*===========================================================================
  FUNCTION qmi_nas_update_global_ims_pref()

  DESCRIPTION
    Update global ims preference information based on SRV_CHANGED event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_update_global_ims_pref( cm_mm_msim_ss_info_s_type *p_ss_info );

/*===========================================================================
  FUNCTION qmi_nas_qmi_lte_bc_2_sys_lte_bc()

  DESCRIPTION
    convert nas_active_band_enum_v01 to sys_sband_lte_e_type 
    
  PARAMETERS
    qmi band class enum

  RETURN VALUE
    corresponding sys band class enum value or;
    SYS_SBAND_LTE_EUTRAN_BAND_MAX if error
===========================================================================*/
sys_sband_lte_e_type qmi_nas_qmi_lte_bc_2_sys_lte_bc( nas_active_band_enum_v01 qmi_lte_bc );

/*===========================================================================
  FUNCTION qmi_nas_map_display_bit_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI display bit value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_tri_state_boolean_type_v01 qmi_nas_map_display_bit_mmgsdi_to_qmi( mmgsdi_eons_rplmn_display_bit_enum_type display_bit );

/*===========================================================================
  FUNCTION qmi_nas_map_home_status_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI home status value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_tri_state_boolean_type_v01 qmi_nas_map_home_status_mmgsdi_to_qmi( mmgsdi_eons_roaming_status_enum_type home_status );

/*===========================================================================
  FUNCTION qmi_nas_map_lang_id_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI language ID value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_lang_id_enum_v01 qmi_nas_map_lang_id_mmgsdi_to_qmi( mmgsdi_plmn_lang_type plmn_lang );

/*===========================================================================
  FUNCTION qmi_nas_map_mmgsdi_session_to_cm_asubs()

  DESCRIPTION
    Map MMGSDI session to CM asubs id

  RETURN VALUE
    asubs id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sys_modem_as_id_e_type qmi_nas_map_mmgsdi_session_to_cm_asubs( mmgsdi_session_type_enum_type session );

/*===========================================================================
  FUNCTION qmi_nas_sys_rat_2_radio_if()

  DESCRIPTION
    convert radio access tech enum to radio_if enum
    
  PARAMETERS
    

  RETURN VALUE
    enum qmi_nas_radio_if_e
===========================================================================*/

enum qmi_nas_radio_if_e qmi_nas_rat_2_radio_if( sys_radio_access_tech_e_type rat );


/*===========================================================================
  FUNCTION qmi_nas_map_cm_asubs_to_mmgsdi_session()

  DESCRIPTION
    Map CM asubs id to MMGSDI session

  RETURN VALUE
    asubs id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_session_type_enum_type qmi_nas_map_cm_asubs_to_mmgsdi_session( sys_modem_as_id_e_type asubs_id );

/*===========================================================================
  FUNCTION qmi_nas_map_nw_name_source_mmgsdi_to_qmi()

  DESCRIPTION
    Map MMGSDI network name source value to QMI_NAS return enum

  RETURN VALUE
    QMI_NAS enum

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
nas_nw_name_source_enum_type_v01 qmi_nas_map_nw_name_source_mmgsdi_to_qmi( mmgsdi_eons_name_source_enum_type nw_name_source );
#endif // !_DS_QMI_NAS_UTILS_H
