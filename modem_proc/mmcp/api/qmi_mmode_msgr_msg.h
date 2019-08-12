#ifndef QMI_MMODE_MSGR_MSG_H
#define QMI_MMODE_MSGR_MSG_H
/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/qmi_mmode_msgr_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/14/12   ndanturi      Provided config request support to CMSDS
==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "cm.h"
#include <msgr.h>
#include <msgr_umid.h>


/*==============================================================================
                           Module definitions
==============================================================================*/
#define MSGR_MODULE_NAS    0x03
#define MSGR_QMI_NAS       MSGR_TECH_MODULE( MSGR_TECH_QMI, MSGR_MODULE_NAS )

#define MSGR_MODULE_VOICE    0x09
#define MSGR_QMI_VOICE       MSGR_TECH_MODULE( MSGR_TECH_QMI, MSGR_MODULE_VOICE )

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  MMODE_QMI_VOICE_TTY_MODE_FULL = 0,  /* TTY Full */
  MMODE_QMI_VOICE_TTY_MODE_VCO,  /* TTY Voice Carryover */
  MMODE_QMI_VOICE_TTY_MODE_HCO,  /* TTY Hearing Carryover */
  MMODE_QMI_VOICE_TTY_MODE_OFF  /* TTY Off */
} mmode_qmi_voice_tty_mode_e_type;

/** @brief Message structure for the data associated 
  with a tty mode changed ind
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  mmode_qmi_voice_tty_mode_e_type tty_mode; 

} mmode_qmi_voice_tty_mode_change_ind_msg_type;


typedef enum
{
  MMODE_QMI_UPDATE_TYPE_FULL = 0,
  MMODE_QMI_UPDATE_TYPE_PARTIAL = 1
} mmode_qmi_voice_update_type_e_type;

typedef enum
{
    MMODE_QMI_CALL_STATUS_NO_CHANGE = 0,
    MMODE_QMI_CALL_STATUS_PENDING = 1,
    MMODE_QMI_CALL_STATUS_DIALING_OUT = 2,
    MMODE_QMI_CALL_STATUS_DIALING_IN = 3,
    MMODE_QMI_CALL_STATUS_ALERTING = 4,
    MMODE_QMI_CALL_STATUS_ON_HOLD = 5,
    MMODE_QMI_CALL_STATUS_CONNECTED = 6,
    MMODE_QMI_CALL_STATUS_MUTED_VIA_FOCUS = 7,
    MMODE_QMI_CALL_STATUS_DISCONNECTING = 8,
    MMODE_QMI_CALL_STATUS_DISCONNECTED = 9
} mmode_qmi_voice_call_status_e_type;


typedef enum
{
    MMODE_QMI_DISC_METHOD_NO_CHANGE = 0,
    MMODE_QMI_DISC_METHOD_DEPARTED = 1,
    MMODE_QMI_DISC_METHOD_BOOTED = 2,
    MMODE_QMI_DISC_METHOD_FAILED = 3,
    MMODE_QMI_DISC_METHOD_BUSY = 4
} mmode_qmi_voice_disconnection_method_e_type;

typedef uint32 mmode_qmi_call_attribute_type_mask;

#define VOICE_CALL_ATTRIB_TX ((mmode_qmi_call_attribute_type_mask)0x01)
#define VOICE_CALL_ATTRIB_RX ((mmode_qmi_call_attribute_type_mask)0x02)
#define VOICE_CALL_ATTRIB_NO_CHANGE ((mmode_qmi_call_attribute_type_mask)0x80)

#define MMODE_QMI_NUM_CONF_PARTICIPANTS_MAX 10
#define MMODE_QMI_CONF_DISPLAY_TEXT_MAX_LEN 64
#define MMODE_QMI_CONF_URI_MAX_LEN 128
#define QMI_VOICE_VICE_XML_MAX_LEN 2048

typedef struct
{
    //number of utf-16 chars in user_uri
    uint8  user_uri_len;

    // Used to differentiate each individual call
    uint16 user_uri[MMODE_QMI_CONF_URI_MAX_LEN];

    // Call status
    mmode_qmi_voice_call_status_e_type status;

    // Audio attribute
    mmode_qmi_call_attribute_type_mask audio_attrib;

    // Video attribute
    mmode_qmi_call_attribute_type_mask video_attrib;

    // disconnection method
    mmode_qmi_voice_disconnection_method_e_type disconnection_method;

    //length of disconnection info
    uint8  disconnection_info_len;

    // disconnection info
    uint8 disconnection_info[MMODE_QMI_CONF_DISPLAY_TEXT_MAX_LEN];
} mmode_qmi_conference_participant_info;

typedef struct
{
    mmode_qmi_voice_update_type_e_type type;

    // Number of calls in the conference
    uint32 conference_call_count;

    // Array of call Information.
    // Although for UE there is only one call established between the device and network,
    // the "call" here refer to each participant in the conference.
    mmode_qmi_conference_participant_info  call_info[MMODE_QMI_NUM_CONF_PARTICIPANTS_MAX];

} mmode_qmi_conference_call_info_s;

/** @brief Message structure for the data associated 
  with command to send the parsed conference participants info to qmi-voice clients
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  mmode_qmi_conference_call_info_s conf_call_info; 

} mmode_qmi_voice_conf_participants_info_cmd_msg_type;

/** @brief Message structure for the data associated 
  with command to send the tty mode info to qmi-clients
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  mmode_qmi_voice_tty_mode_e_type tty_mode; 

} mmode_qmi_voice_tty_mode_info_cmd_msg_type;

typedef enum{
  QMI_MMODE_MSGR_SEARCH_MODE_HOME = 0,
  QMI_MMODE_MSGR_SEARCH_MODE_ALL = 1
} qmi_mmode_msgr_csg_search_mode_e_type ;

typedef struct 
{
    uint64  umts_band_pref;
    uint64  lte_band_pref;
    uint32  search_timer;
    uint32  list_type;
    uint32  network_type;
    uint8  search_mode;
    uint8  asubs_id;
    uint8  select_config;
} qmi_mmode_msgr_csg_search_select_config_s_type;

typedef struct
{
  uint8 asubs_id;
} qmi_mmode_msgr_csg_immd_search_select_cmd_msg_type;

typedef struct
{
  msgr_hdr_s  msg_hdr;
 /**< Message header */
  qmi_mmode_msgr_csg_search_select_config_s_type csg_search_select_config_req;
} mmode_qmi_nas_csg_search_select_config_cmd_msg_type;


typedef struct
{
  msgr_hdr_s  msg_hdr;
 /**< Message header */
  qmi_mmode_msgr_csg_immd_search_select_cmd_msg_type csg_immediate_search_select_req;
} mmode_qmi_nas_csg_immediate_search_select_cmd_msg_type;


typedef enum
{
  MMODE_QMI_AUDIO_PASSIVE_SESSION_START  = 0,
  MMODE_QMI_AUDIO_PASSIVE_SESSION_STOP = 1
} mmode_qmi_voice_audio_passive_session_e_type;

typedef enum {

  MMODE_QMI_SYS_MODEM_AS_ID_1,
    /**< Subscription ID 1 */

  MMODE_QMI_SYS_MODEM_AS_ID_2,
    /**< Subscription ID 2 */
  
  MMODE_QMI_SYS_MODEM_AS_ID_3,
    /**< Subscription ID 3 */
} mmode_qmi_voice_as_id_e_type;

typedef enum
{
  MMODE_QMI_VOICE_RAT_LTE  = 0x04,
  MMODE_QMI_VOICE_RAT_WLAN = 0x07
} mmode_qmi_voice_rat_e_type;

typedef enum
{
  MMODE_QMI_CONF_PARTICIPANT_ADD  = 0,
  MMODE_QMI_CONF_PARTICIAPNT_REMOVE = 1
} mmode_qmi_voice_conf_participant_operation_e_type;


typedef struct
{
    mmode_qmi_voice_as_id_e_type asid;
    mmode_qmi_voice_audio_passive_session_e_type audio_session;
    mmode_qmi_voice_rat_e_type  rat;
} mmode_qmi_audio_rat_change_info_s;

/** @brief Message structure for the data associated 
  with command to send the audio session rat change info from IMS to QMI to AP.
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  mmode_qmi_audio_rat_change_info_s audio_rat_change; 

} mmode_qmi_voice_audio_session_rat_change_cmd_msg_type;


typedef struct
{
  /*CM call ID for qmi-voice to retrieve the call context*/
  cm_call_id_type   cm_call_id;

  /* Participant details, Length of the URI and ASCI URI string */
  uint8             participant_uri_len;
  uint8             participant_uri[MMODE_QMI_CONF_URI_MAX_LEN];

  /* Operation performed */
  mmode_qmi_voice_conf_participant_operation_e_type operation;
 
  /*Is it Expilicit Call Transfer*/
  boolean is_qmi_voice_transfer;
 
  /* SIP code indication status of participant (e.g 180, 200) */
  uint16            status_sip_code; 
} mmode_qmi_participant_status_info_s;


/** @brief Message structure for the data associated 
  with command to send the add-participant operation status from IMS to QMI to AP.
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  mmode_qmi_participant_status_info_s participant_status; 

} mmode_qmi_voice_participant_status_cmd_msg_type;

 /** @brief Message structure for the data associated 
  with command to send VICE dialog info from IMS
  to AP
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  uint8 *vice_xml;

} mmode_qmi_voice_viceDialog_info_ind_msg_type;


/*==============================================================================
                           Msg UMID definitions
==============================================================================*/

/*! @brief UMIDs of the messages sent by the QMI to other modules
*/
enum
{
  MMODE_QMI_VOICE_IND_FIRST = MSGR_UMID_BASE(MSGR_QMI_VOICE, MSGR_TYPE_IND),
  
  MSGR_DEFINE_UMID(QMI, VOICE, IND, VOICE_TTY_MODE_CHANGE_IND, 0x01, mmode_qmi_voice_tty_mode_change_ind_msg_type),

 /* QMI_VOICE_CONF_PARTICIPANTS_INFO_CMD  - Interface to pass conference participants info */
  MSGR_DEFINE_UMID(QMI, VOICE, CMD, CONF_PARTICIPANTS_INFO, 0x02, mmode_qmi_voice_conf_participants_info_cmd_msg_type),

  /* QMI_VOICE_TTY_MODE_INFO_CMD  - Interface to pass tty info from lower layers*/
  MSGR_DEFINE_UMID(QMI, VOICE, CMD, TTY_MODE_INFO, 0x03, mmode_qmi_voice_tty_mode_info_cmd_msg_type),

  /* QMI_NAS_CSG_SEARCH_SELECTION_CONFIG_CMD  - Interface to pass csg search selection configuration to thin UI*/
  MSGR_DEFINE_UMID(QMI, NAS, CMD, CSG_SEARCH_SELECTION_CONFIG, 0x04, mmode_qmi_nas_csg_search_select_config_cmd_msg_type),

  /* QMI_NAS_CSG_IMMEDIATE_SEARCH_SELECTION_CMD  - Interface to pass immediate csg search selection request to thin UI*/
  MSGR_DEFINE_UMID(QMI, NAS, CMD, CSG_IMMEDIATE_SEARCH_SELECTION, 0x05, mmode_qmi_nas_csg_immediate_search_select_cmd_msg_type),

  /* QMI_VOICE_AUDIO_RAT_CHANGE_INFO_CMD  - Interface to pass tty info from lower layers*/
  MSGR_DEFINE_UMID(QMI, VOICE, CMD, AUDIO_RAT_CHANGE_INFO, 0x09, mmode_qmi_voice_audio_session_rat_change_cmd_msg_type),

  /* QMI_VOICE_UI_TTY_MODE_SETTING_CMD  - Interface to pass ui tty info to lower layers*/
  MSGR_DEFINE_UMID(QMI, VOICE, CMD, UI_TTY_MODE_SETTING, 0x0A, mmode_qmi_voice_tty_mode_info_cmd_msg_type),

  /* QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_CMD  - Interface to pass add-participant operation status to apps*/
  MSGR_DEFINE_UMID(QMI, VOICE, CMD, CONF_PARTICIPANT_STATUS_INFO, 0x0B, mmode_qmi_voice_participant_status_cmd_msg_type),
   
/* QMI_VOICE_VICE_DIALOG_IND  - Interface to vice dialog info to apps from IMS */
  MSGR_DEFINE_UMID(QMI, VOICE, IND, VICE_DIALOG, 0x0D, mmode_qmi_voice_viceDialog_info_ind_msg_type),

  MMODE_QMI_VOICE_IND_MAX,
  MMODE_QMI_VOICE_IND_LAST = MMODE_QMI_VOICE_IND_MAX - 1
};

#endif /* QMI_MMODE_MSGR_MSG_H */


