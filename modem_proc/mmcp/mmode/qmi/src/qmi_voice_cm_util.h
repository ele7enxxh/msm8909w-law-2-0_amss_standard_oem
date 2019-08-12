#ifndef QMI_VOICE_CM_UTIL_H
#define QMI_VOICE_CM_UTIL_H

/*===========================================================================

                         Q M I _ V O I C E _ C M _ U T I L . H

DESCRIPTION

 The Data Services Qualcomm Voice CM Utils header.


EXTERNALIZED FUNCTIONS


Copyright (c) 2004-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_cm_util.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/25/09    sk     Initial revision.
===========================================================================*/


#include "comdef.h"
#include "cm.h"
#include "qmi_voice_cm_if.h"
#include "qmi_voice_call_list.h"
#include "IxErrno.h"

#include "fs_public.h"  /* Interface to EFS services */
#include "fs_errno.h"   /* EFS error numbers */

#include "mcfg_fs.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define QMI_VOICE_CM_UTIL_CONN_ID_INVALID 0

/* USSD GSM 7-bit coding scheme mask */
#define QMI_VOICE_CM_SUPS_USSD_DCS_7_BIT_MASK         0x00
/* USSD UCS2 coding preceded by language preference scheme mask */
#define QMI_VOICE_CM_SUPS_USSD_DCS_UCS2_LANG_IND_MASK 0x01
/* USSD 8-bit coding scheme mask */
#define QMI_VOICE_CM_SUPS_USSD_DCS_8_BIT_MASK         0x04
/* USSD UCS2 coding scheme mask */
#define QMI_VOICE_CM_SUPS_USSD_DCS_UCS2_MASK          0x08

/**< Numbering plan identification. */
#define QMI_VOICE_CM_UTIL_NUM_PLAN_MASK            0x0F

/* Type of number */
#define QMI_VOICE_CM_UTIL_TON_MASK                 0x70
/**< Type of Number (octet 3). */

#define QMI_VOICE_CM_UTIL_PRESENTATION_IND_MASK   0x60
  /**< Presentation indicator  (octet 3a). */

/* Screening Indicator */
#define QMI_VOICE_CM_UTIL_SCREENING_IND_MASK       0x03
  /**< Screening indicator (octet 3a). */

#define QMI_VOICE_CM_UTIL_CM_EXTENSION_BIT           0x80
  /**< Indicates octet 3a of the BCD number IE
      (calling party/called party/redirecting party information element)
  */

#define QMI_VOICE_TIMEPAUSE_CHAR      'T'  /**< Character in the dial string used to indicate
                                          a time pause. */

#define QMI_VOICE_HARDPAUSE_CHAR      'H'  /**< Character in the dial string used to indicate
                                          a hard pause.
@note On a UIM card, this is converted to a TIMEPAUSE character before saving. */

#define QMI_VOICE_WILD_CHAR           '?'
  /**< Character in the dial string used to indicate a wildcard character. */

typedef struct {
  sys_sys_mode_e_type     sys_mode;
  sys_srv_status_e_type   srv_status;
  sys_srv_status_e_type   hdr_srv_status;
  boolean                 hdr_hybrid;
} qmi_voice_cm_util_ss_info_s;

/* Maximum length of file name for EFS files  (including full path)-
** to be increased if filepath length increases
*/
#define QMI_VOICE_EFS_FILENAME_MAX_LEN         60

/** EFS id enumeration
 *  NOTE: Add new enum values to the end of this table. It is
 *  very important to keep the order of this table consistent
 *  with the order in cmnv_efs_id_to_path_mapping_tbl[] to
 *  maintain correct operation
 *   */
typedef enum
{
  QMI_VOICE_EFS_ID_NONE = -1,          
    /**< internal use only */

  QMI_VOICE_DOMAIN_PREF_F_NAME,

  QMI_VOICE_UI_TTY_SETTING_PRI_F_NAME,

  QMI_VOICE_EFS_ID_MAX

} qmi_voice_efs_id_e_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


void qmi_voice_cm_util_init
(
  void
);

void qmi_voice_cm_util_copy_alpha_info_from_cm
(
  qmi_voice_cm_if_alpha_id_s *dest,
  cm_alpha_s_type *src
);

void qmi_voice_cm_util_copy_alpha_ident_from_cm
(
  voice_alpha_ident_type_v02 *dest,
  cm_alpha_s_type            *src
);

void qmi_voice_cm_util_copy_cc_alpha_info_from_cm
(
  qmi_voice_cm_if_cc_alpha_id_s *dest,
  cm_alpha_s_type *src
);


void qmi_voice_cm_util_process_cnap_info( const cm_mm_call_info_s_type * call_info_ptr, boolean report_change);

int qmi_voice_cm_util_convert_2s_complement_to_int(byte);

boolean qmi_voice_cm_util_is_digits_dtmf(uint8 cnt, const uint8 *digit_buf);

IxErrnoType qmi_voice_cm_util_allocate_conn_id
(
  uint8* conn_id
);

IxErrnoType qmi_voice_cm_util_deallocate_conn_id
(
  uint8 conn_id
);

cm_ussd_dcs_e_type qmi_voice_cm_util_get_ussd_dcs
(
  byte uss_data_coding_scheme
);

void qmi_voice_cm_util_get_num_info_from_bcd_num
(
  cm_num_s_type *num,
  cm_called_party_bcd_no_s_type *bcd_num
);

boolean qmi_voice_cm_util_get_ss_info
(
  qmi_voice_cm_util_ss_info_s  *ss_info,
  uint8 asubs_id
);


const char *   qmi_voice_util_map_efs_id_to_filename(
qmi_voice_efs_id_e_type          efs_id
/* passing the efs Id to map  */
);

boolean qmi_voice_cm_util_efs_write(

qmi_voice_efs_id_e_type          efs_id,
            /* passing the efs Id to write.       */
const byte                      *data_ptr,
            /* Pointer to a data buffer to write to EFS.      */
int32                            size
            /* Size of the data_ptr buffer.            */
);

boolean qmi_voice_cm_util_efs_write_per_subs(

qmi_voice_efs_id_e_type          efs_id,
            /* passing the efs Id to write.       */
const byte                       *data_ptr,
            /* Pointer to a data buffer to write to EFS.      */
int32                            size,
            /* Size of the data_ptr buffer.            */
qmi_voice_cm_if_as_id_e_type     as_id
            /* subscription to write EFS file to */
);

boolean qmi_voice_cm_util_efs_read(

qmi_voice_efs_id_e_type          efs_id,
            /* passing the efs Id to write.       */
byte                            *data_ptr,
       /* Pointer to a data buffer to store EFS data.      */
int32                            size
            /* Size of the data_ptr buffer.            */
);

boolean qmi_voice_cm_util_efs_read_per_subs(

qmi_voice_efs_id_e_type          efs_id,
/* passing the efs Id to write.*/
byte                             *data_ptr,
/* Pointer to a data buffer to store EFS data.*/
int32                            size,
/* Size of the data_ptr buffer.*/
qmi_voice_cm_if_as_id_e_type     as_id
/* subscription to read EFS file from */
);

mcfg_fs_sub_id_e_type qmi_voice_cm_util_map_as_id_to_mcfg_id(
  qmi_voice_cm_if_as_id_e_type    as_id
);

void qmi_voice_cm_util_set_ss_info
(
  cm_mm_msim_ss_info_s_type* ss_info_ptr
);

boolean qmi_voice_cm_util_number_match
(
  const byte *sim_num,    
  byte sim_num_len,
  const byte *dialed_num, 
  byte dialed_num_len
);

qmi_voice_cm_if_vs_version_e_type qmi_voice_cm_util_map_cm_to_qmi_vs_version
(
  cm_ip_vs_version_e_type version
);

qmi_voice_call_list_state_e_type qmi_voice_cm_util_map_cm_to_qmi_call_state
(
  cm_call_state_e_type cm_state,
  cm_call_substate_type cm_substate
);

cm_ip_vs_version_e_type qmi_voice_cm_util_map_qmi_to_cm_vs_version
(
  qmi_voice_cm_if_vs_version_e_type  version
);

qmi_voice_cm_if_call_state_e_type qmi_voice_cm_util_map_call_state
(
  qmi_voice_call_list_state_e_type state
);

qmi_voice_cm_if_call_type_e_type qmi_voice_cm_util_map_cm_to_qmi_call_type
(
  cm_call_type_e_type type,
  cm_call_mode_info_e_type mode_info_type
);

uint8 qmi_voice_cm_util_map_sys_mode_to_call_mode
(
  sys_sys_mode_e_type mode
);

void qmi_voice_util_map_qmi_to_cm_call_state_substate
(
  cm_call_state_e_type *cm_state,
  cm_call_substate_type *cm_sub_state,
  qmi_voice_cm_if_call_state_e_type qmi_state,
  qmi_voice_cm_if_call_substate_e_type qmi_sub_state
);

cm_call_type_e_type qmi_voice_cm_util_map_qmi_to_cm_call_type
(
  qmi_voice_cm_if_call_type_e_type type
);

uint32 qmi_voice_cm_util_map_qmi_to_cm_call_reject_cause
(
  qmi_voice_cm_if_call_reject_cause_e_type  qmi_rej_cause
);

uint32 qmi_wstrlen(const uint16 *ucs2_str);

uint32 qmi_voice_cm_util_map_cm_to_qmi_restrict_cause
(
  cm_call_restrict_cause_e_type  cm_cause
);

qmi_voice_cm_if_failure_cause_e_type qmi_voice_cm_util_map_cm_to_qmi_ip_sups_failure_cause
(
  cm_call_end_e_type  cm_fail_cause,
  boolean *is_found
);

qmi_voice_cm_if_call_modified_cause_e_type qmi_voice_cm_util_map_cm_to_qmi_ip_call_modified_cause
(
  cm_call_modify_cause_e_type cm_call_modified_cause,
  boolean *is_found
);

void qmi_voice_cm_util_copy_sups_failure_string
(
  voice_set_sups_service_resp_msg_v02 *qmi_struct,
  cm_ip_sups_error_s_type *cm_struct
);

boolean qmi_voice_cm_util_is_sys_mode(uint16 mode_mask, uint8 asubs_id);

qmi_voice_cm_if_sys_mode_e_type qmi_voice_cm_util_map_cm_to_qmi_sys_mode
(
  sys_sys_mode_e_type  cm_sys_mode
);

sys_sys_mode_e_type qmi_voice_cm_util_map_qmi_to_cm_sys_mode
(
  qmi_voice_cm_if_sys_mode_e_type  qmi_sys_mode
);

uint8 qmi_voice_cm_util_map_cm_to_qmi_alerting_type
(
  cm_srvcc_alert_media_type cm_alt_type
);

cm_srvcc_alert_media_type qmi_voice_cm_util_map_qmi_to_cm_alerting_type
(
   uint8 qmi_alerting_type
);

boolean qmi_voice_cm_util_copy_barr_num_list_info_from_cm_to_qmi
(
  ie_call_barring_num_info_T *cm_data, qmi_voice_cm_if_icb_sc_s *qmi_data, uint8 copy_len
);

boolean qmi_voice_cm_util_copy_barr_num_list_from_qmi_to_cm
(
  qmi_voice_cm_if_icb_ind_s *qmi_data, cm_called_party_bcd_no_s_type *cm_data, uint8 copy_len
);

cm_call_end_e_type qmi_voice_cm_util_map_qmi_to_cm_ip_call_reject_cause
(
  qmi_voice_cm_if_call_reject_cause_e_type  qmi_rej_cause
);

boolean qmi_voice_util_check_if_gsm7_ext_set
(
 uint16 *utf16_data,
 uint8   utf16_data_len,
 uint8  *gsm7_data,
 uint8   gsm7_data_len,
 uint8   num_chars,
 uint8  *gsm7_ext_data,
 uint8  *gsm7_ext_str_len
);


void qmi_voice_cm_util_fill_number_and_length_with_plus_sign
(
  uint8                *dest_num_buf_ptr,
  uint32               dest_num_buf_len,
  uint8                *dest_len_ptr,
  const cm_num_s_type  *src_num_s_ptr
);

boolean qmi_voice_cm_util_map_cm_to_qmi_ho_type
(
  sys_voice_handover_e_type cm_ho_type,
  voice_handover_type_enum_v02 *qmi_ho_type_ptr
);

#endif /* QMI_VOICE_CM_UTIL_H */

