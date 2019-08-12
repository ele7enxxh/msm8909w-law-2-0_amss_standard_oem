#ifndef HDRSCP_H
#define HDRSCP_H

/*===========================================================================

         S E S S I O N   C O N F I G U R A T I O N   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Session Configuration Protocol (SCP)

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrscp.h_v   1.12   14 Mar 2003 11:13:16   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
11/18/14   vko     Do not close ehrpd session immediately during hotswap
04/06/12   cnx     Force to close HPRD session after LTE attach.
12/05/11   cnx     Support hdrcp_config_info.conf.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/24/11   smd     Moved more to SU API header files.
09/24/11   cnx     Added hdrscp_is_irat_enabled() and hdrscp_update_subtype_defaults().
09/01/11   cnx     Refeaturized 'query data's eHRPD capability'.
08/26/11   cnx     Renamed ehrpd_apns to data_ehrpd_capability.
07/27/11   cnx     Merged Optimized Handoff changes.
06/03/11   cnx     Added ehrpd_was_advertised flag in SCP configurable parameters.
                   Supported eHRPD availability related functions.
04/26/11   sju     Added API to provide current HDR revision.
06/04/10   wsh     Restructured hdrscp data type.
03/22/10   pba     Added support for eHRPD session negotiation based on 
                   confirming availability of USIM App
10/30/09   pba     Added ehrpd support.
03/16/09   etv     Added hdrscp_at_is_dorb_capable.
01/23/09   wsh     Fixed crash in logging
12/23/08   pba     Support for Quick Idle Protocol
08/07/08   wsh     Fixed OTA msg parsed w/ wrong subtype during negotiation
04/09/08   sju     Added hdrscp_get_logging_subtype()
10/17/07   pba     Updates based on code review comments.
03/27/07   wsh     Added support for new prior session handling
03/07/07   yll     Removed SCP custom config.
01/12/06   pba     Reject Rev-A default subtype attributes when Phy is not ST2
01/08/07   yll     Added support for CSNA.
06/21/06   etv     Added hdrscmovhd module to support OMP config attribs.
06/20/06   pba     Removed redundant AN auth status maintained by SCP.
                   Config Lock is hardlinked attribute of SCP instead of per
                   personality.
05/24/06   pba     Added hdrscp_config_subtype_is_hardlink() routine.
05/15/06   pba     Code cleanup
05/10/06   yll/ksu EMPA: NV item for runtime control
03/22/06   pba     Added support for ConfigurationLock.
02/01/06   pba     Fixes for HardLink subtype negotiation.
01/14/06   dna     Allow hardlink subtype for all protocols
12/13/05   pba     Changes to support multiple personality negotiation
12/06/05   etv     Added subtype defn for Generic Multi-mode Capab. Disc prot.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/19/05   mpa     Added hdrscp_is_configuring().
08/22/05   pba     Added SCM support.
07/07/05   dna     Added support for Enhanced Idle State protocol
07/07/05   mpa     Added support to propose all non-default subtypes during 
                   AT initiated config and an NV item to selectively enable/
                   disable support for non-default protocol subtypes.
11/04/04   mpa     Added RMAC subtype 1 support.
10/19/04   mpa     Moved hdrscp_session_is_open() to hdrsmp.
07/02/04   mpa     Added support for new BCMCS protocol.
03/12/03   mpa     Replaced failed reason and deactivate enum types with
                   hdrhai_session_close_reason_enum_type
02/05/02   vas     Changed AN authentication status from boolean to enum. 
01/17/02   dna     Remove hdrscp_init()
12/03/01   vas     Changed hdrscp_set_an_auth_status() prototype to match 
                   that for commands.
12/01/01   vas     Added new functions & support for maintaining HDR AN 
                   authentication status.
11/30/01   om      Added security layer support
01/02/01   nmn     Initial Revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "hdrhai.h"
#include "dsm.h"
#include "comdef.h"
#include "hdrerrno.h"
#include "hdrind.h"
#include "sys.h"
#include "nv.h" /* for nv_hdrscp_bcmcs_config_enum_type */
#include "hdrcp_api.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_EHRPD
#define HDRSCP_EHRPD_NETWORK_SETTING_FILENAME "/nv/item_files/modem/hdr/cp/scp/ehrpdnetworksetting"
#endif /* FEATURE_EHRPD */

/* Values of all protocol's defined subtypes */
#define HDRSCP_DEFAULT_PROTOCOL 0x0000U
#define HDRSCP_HARDLINK 0x0FFFEU

/* Security Layer protocols subtypes */
#define HDRSCP_DEFAULT_SEC 0x0000U
#define HDRSCP_GENERIC_SECURITY 0x0001U
#define HDRSCP_DEFAULT_KEP 0x0000U
#define HDRSCP_DH_KEY_EXCHANGE 0x0001U
#define HDRSCP_DEFAULT_AUTH 0x0000U
#define HDRSCP_SHA1_AUTH 0x0001U
#define HDRSCP_DEFAULT_ECP 0x0000U

/* Connection Layer protocols subtypes */
#define HDRSCP_DEFAULT_OVHD 0x0000U
#define HDRSCP_DEFAULT_ALMP 0x0000U
#define HDRSCP_DEFAULT_INIT 0x0000U
#define HDRSCP_DEFAULT_IDLE 0x0000U
#define HDRSCP_QUICK_IDLE 0x0002U
#define HDRSCP_DEFAULT_CON 0x0000U
#define HDRSCP_DEFAULT_RUP 0x0000U
#define HDRSCP_MC_RUP 0x0001U
#define HDRSCP_DEFAULT_PCP 0x0000U

/* Session protocols subtypes */
#define HDRSCP_DEFAULT_SMP 0x0000U
#define HDRSCP_DEFAULT_AMP 0x0000U
#define HDRSCP_DEFAULT_SCP 0x0000U
#define HDRSCP_DEFAULT_MMCDP 0x0000U
#define HDRSCP_GENERIC_MMCDP 0x0001U

#define HDRSCP_DEFAULT_SLP 0x0000U

#define HDRSCP_DEFAULT_BCMCS 0x0000U
#define HDRSCP_GENERIC_BCMCS 0x0001U

#ifdef FEATURE_LTE_TO_HDR_OH
#define HDRSCP_IRAT_OVHD 0x0001U
#define HDRSCP_IRAT_INIT 0x0001U
#define HDRSCP_IRAT_IDLE 0x0003U
#define HDRSCP_IRAT_QUICK_IDLE 0x0004U
#define HDRSCP_IRAT_RUP 0x0002U
#define HDRSCP_IRAT_MC_RUP 0x0003U
#define HDRSCP_DEFAULT_SAP 0x0000U
#define HDRSCP_IRAT_SAP 0x0001U
#endif /* FEATURE_LTE_TO_HDR_OH */

/* structure for SCP indication data */
typedef struct
{
  uint8  type;                      /* failed type */
  uint16 subtype;                   /* failed subtype */
  uint8  attribute_id_length;       /* failed attribute ID length (bytes)*/ 
  uint16 attribute_id;              /* failed attribute ID */
  hdrhai_session_close_reason_enum_type reason; /* failure reason */
} hdrscp_scp_fail_ind_type;

/*----------------------------------------------------------
Bit mask for custom config of application subtypes (NV Item) 
----------------------------------------------------------*/
typedef struct
{
  /*----------------------------
  0 - Enable, 1 - Disallow
  ----------------------------*/
  uint32 enable_mfpa   :1;  /* Bit 0: SN Multiflow Packet Application */
  uint32 enable_empa   :1;  /* Bit 1: SN Enhanced Multiflow Packet Application */
  uint32 enable_csna   :1;  /* Bit 2: Circuit Services Notification Application */
  uint32 enable_mmpa   :1;  /* Bit 3: SN MultiCarrier Multiflow Packet 
                                      Application */
  uint32 enable_aempa  :1;  /* Bit 4: Alternate EMPA */
  uint32 enable_ammpa  :1;  /* Bit 5: Alternate MMPA */
  /* Bit 6-31: Reserved */
  uint32 reserved     :27;
} hdrscp_app_subtype_enable_mask_type;

/* number of bytes in the SCP Attribute ID */
#define HDRSCP_ATTRIBUTE_ID_LENGTH 2

/* configurable parameters for SCP */
typedef struct
{
  uint16 config_token;
  uint16 subtype[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];
  uint16 support_gaup_session_config_token;
  uint16 personality_count;
  uint16 support_config_lock;
  boolean config_is_locked; 
#ifdef FEATURE_EHRPD
  boolean ehrpd_was_advertised;
#endif /* FEATURE_EHRPD */

} hdrscp_config_params_type;

/* configurable parameters for SCP that are hardlinked */
typedef struct
{
  uint16 config_token;
  uint16 support_gaup_session_config_token;
  uint16 personality_count;
  uint16 support_config_lock;
  boolean config_is_locked;
#ifdef FEATURE_EHRPD
  boolean ehrpd_was_advertised;
#endif /* FEATURE_EHRPD */

} hdrscp_hardlink_config_params_type;

/* configurable parameters for SCP that are per personality */
typedef struct
{
  uint16 subtype[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];

} hdrscp_per_personality_config_params_type;

/*-- definitions for scp_data_type --*/

/* This enum defines the three possibilities of the session status */
typedef enum
{
  HDRSCP_SESSION_INACTIVE, /* Inactive - there is no session */
  HDRSCP_SESSION_DEFAULT,  /* Default - there is a session, but no negotiation
                           has been completed */
  HDRSCP_SESSION_ACTIVE    /* Active - there is a session, and all parameters
                           have had at least one chance to be negotitated */
} hdrscp_session_status_enum_type;

/* This enum defines the current state of the negotiation */
typedef enum
{
  HDRSCP_NEG_NONE, 
  HDRSCP_NEG_PRIOR_SESSION,
  HDRSCP_NEG_PRIOR_SESSION_COMPLETE,
  HDRSCP_NEG_AN_MAIN_PERSONALITY,
  HDRSCP_NEG_AT_MAIN_PERSONALITY,
  HDRSCP_NEG_AN_SUBSEQUENT_PERSONALITY,
  HDRSCP_NEG_AT_SUBSEQUENT_PERSONALITY,
  HDRSCP_NEG_COMPLETE
} hdrscp_negotiation_state_enum_type;

/* methods to restore prior session */
typedef enum
{
  HDRSCP_PS_RESTORE_CONFIGREQ,
  /* Using ConfigurationRequest/Resp to restore prior session */
  HDRSCP_PS_RESTORE_GAUP,
  /* Using GAUP */
  HDRSCP_PS_RESTORE_SILENT
  /* Silent restore of prior session */
} hdrscp_ps_restore_process_enum_type;

#define HDRSCP_MAX_SUBTYPES 5

#define HDRSCP_MAX_CFG_REQ 2

typedef struct
{
  uint16 type;          /* Protocol Type */
  uint16 num_subtypes;  /* Number of subtypes */
  uint16 subtype[HDRSCP_MAX_SUBTYPES];  /* Supported protocol subtypes */
} hdrscp_subtype_entry_type;

/* session parameters */
typedef struct
{
  hdrscp_config_params_type current;
  hdrscp_config_params_type negotiated;
} hdrscp_session_params_type;

typedef struct
{
  /* The following are used to match the ConfigRequest message to the
     corresponding response message */
  uint8  transaction_id;
  uint16 attrib_id;      

}hdrscp_config_req_resp_map_type;

/* This structure contains all information related to a sent Configuration
   Request Message */
typedef struct
{
  /* This list has an entry for each of the protocol. For each protocol it
     lists the supported subtypes that were included in the ConfigRequest
     message */
  hdrscp_subtype_entry_type ptype_attrib[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];

  /* Flag indicating if prior session attrib was included in AT_INIT
     SCP's configuration request */
  boolean sent_previous_session_attrib;
  /* Sent value ID for the prior session attribute */
  uint8 value_id;      

  /* Flag indicating if personality count attrib was included in AT_INIT
     SCP's configuration request */
  boolean sent_personality_count_attrib;
 
  /* Sent Transaction ID */
  uint8 transaction_id;

  /* Flags whose each of the bit position indicate if AT has received SLP ACK
     for corresponding configuration request */
  uint8 awaiting_slp_ack;

  /* Flags whose each of the bit position indicate if AT has received L3 ACK
     for corresponding configuration request */
  uint8 awaiting_l3_ack;

  /* Number of SCP configuration Request messages sent at a given time. SCP 
     sends two separate ConfigReqs while proposing PriorSession */
  uint8 num_cfg_req_sent;

  hdrscp_config_req_resp_map_type mapping_req_resp[HDRSCP_MAX_CFG_REQ];

  /* Flag indicating if prior session was accepted */
  boolean prior_session_is_accepted;

} hdrscp_config_req_info_type;

/* Fields of SCC/CC message */
typedef struct
{
  uint8  trans_id;
  uint8  personality_index_store;
  uint8  continue_config;
  uint8  commit;
  uint16 sct;

}hdrscp_config_complete_msg_type;

typedef struct
{
  uint8 trans_id;
  boolean awaiting_slp_callback;
} hdrscp_config_complete_info_type;

typedef struct
{
  /* Number of failed connection setups attempts */
  uint8 con_fail_retry_count;

  /* Indicates whether one of the connection setup failures was due to the
     network sending a ConnectionDeny message in response to our connection
     request.  (In AT_INIT state) This is tied to the conn failure retry count
     because we expect that after the max number of retries, we will close the
      we received a ConnectionDeny in this round, we will count it towards
     avoiding the system. */
  boolean rxed_con_deny;
} hdrscp_config_conn_setup_info_type;

typedef enum
{
  HDRSCP_REV0_PROTOCOLS_ONLY,
    /* AT supports only Rev 0 protocols */

  HDRSCP_REVA_PROTOCOLS_WITH_MFPA,
    /* AT supports only Rev 0, Rev A protocols and MFPA */

  HDRSCP_REVA_PROTOCOLS_WITH_MFPA_AND_EMPA,
    /* AT supports only Rev 0, Rev A protocols, MFPA and EMPA */

  HDRSCP_REVB_PROTOCOLS_WITH_MMPA,
    /* AT supports Rev 0, Rev A, Rev B protocols, MFPA, EMPA and MMPA */
    /* MMPA is superset of MFPA and EMPA, so by supporting MMPA 
       we support also EMPA and MFPA */

  HDRSCP_REVA_PROTOCOLS_WITH_EHRPD,
    /* AT supports only Rev 0, Rev A protocols, MFPA, EMPA and AEMPA */

  HDRSCP_REVB_PROTOCOLS_WITH_EHRPD,
    /* AT supports Rev 0, Rev A, Rev B protocols, MFPA, EMPA, MMPA and AEMPA */

  HDRSCP_REVA_PROTOCOLS_WITH_EHRPD_AND_IRAT,
    /* AT supports only Rev 0, Rev A protocols, MFPA, EMPA, AEMPA and IRAT */

  HDRSCP_REVB_PROTOCOLS_WITH_EHRPD_AND_IRAT
    /* AT supports Rev 0, Rev A, Rev B protocols, MFPA, EMPA, MMPA, AEMPA and IRAT */
} hdrscp_force_at_config_type;

/*---------------------------------------------------------
   Run-Time enabled protocol subtype selection (NV Item) 
---------------------------------------------------------*/
typedef struct
{
                                            
    /*----------------------------------------------------------------------
       0 - Disabled, 1 - Enabled
    ----------------------------------------------------------------------*/
    uint32 sub2_phys_layer :1;  /* Bit 0: Subtype 2 Physical Layer */
    uint32 enh_cmac        :1;  /* Bit 1: Enhanced CCMAC           */
    uint32 enh_amac        :1;  /* Bit 2: Enhanced ACMAC           */
    uint32 enh_fmac        :1;  /* Bit 3: Enhanced FTCMAC          */
    uint32 rmac_sub3       :1;  /* Bit 4: Subtype 3 RTCMAC         */  
    uint32 rmac_sub1       :1;  /* Bit 5: Subtype 1 RTCMAC         */
    uint32 enh_idle        :1;  /* Bit 6: Enhanced Idle            */
    uint32 gmmcdp          :1;  /* Bit 7: Generic MultiMode Capab Disc prot */

    uint32 sub3_phys_layer :1;  /* Bit 8: Subtype 3 Physical Layer */
    uint32 mc_fmac         :1;  /* Bit 9: MultiCarrier FTCMAC      */
    uint32 mc_rmac         :1;  /* Bit 10: MultiCarrier RTCMAC     */
    uint32 mc_rup          :1;  /* Bit 11: MultiCarrier RUP        */
    uint32 quick_idle      :1;  /* Bit 12: Quick Idle              */

    uint32 irat_sap        :1;  /* Bit 13: IRAT SAP */
    /* Bit 14-31: Reserved      */
    uint32 reserved        :18;

} hdrscp_prot_subtype_enable_mask_type;

/*---------------------------------------------------------
 Run-Time enabled Broadcast protocol negotiation (NV Item) 
---------------------------------------------------------*/
typedef struct
{
  uint32 gen_broadcast_is_enabled : 1;  
    /* Bit 0: Enable Generic broadcast (subtype 1) */

  uint32 reserved                 : 31;
    /* Bit 1-31: Reserved*/

} hdrscp_bcmcs_enable_mask_type;

typedef struct 
{ 
  boolean                              config_is_custom;
    /* 0 - propose/accept all supported subtypes, 1 - use custom config below. */

  hdrscp_prot_subtype_enable_mask_type prot_subtype_mask;
  /* HDR Protocol Subtypes Enable Bitmask */

  hdrscp_bcmcs_enable_mask_type        bcmcs_subtype_mask;
  /* HDR Broadcast Subtypes Enable Bitmask */

  hdrscp_app_subtype_enable_mask_type  app_subtype_mask;
  /* Stream Applications Subtypes Enable Bitmask */

} hdrscp_permitted_subtypes_type;

typedef struct
{
  boolean                            config_requested;
  uint32                             neg_flags;
    /* Each bit in this mask represents one of the protocols which may have
       sent a Configuration Request Message. If there are ever more than 32
       protocols, the type of this will have to expand to a type with more 
       bits. */
  hdrscp_config_req_info_type        config_req_info;
  hdrscp_session_params_type         session_params;
  hdrscp_session_status_enum_type    session_status;
    /* Status of AT's current session */

  boolean                            ps_gaup_failed;
    /* Flag indicating if we have tried ps gaup and failed on this
       subnet. It is reset when moved to a different subnet */

  hdrscp_negotiation_state_enum_type negotiation_state;
  hdrscp_config_complete_info_type   config_complete;
  hdrscp_config_conn_setup_info_type config_conn_setup;
  hdrscp_permitted_subtypes_type     permitted_subtypes;
    /* Custom configuration of protocol and app subtypes */
  boolean                            session_cfg_token_changed;
    /* Flag to indicate if session configuration token changed
       while in Connected state */

  nv_hdrscp_bcmcs_config_enum_type bcmcs_config_from_nv;
    /* BCMCS Configuration read from NV item */

  hdrscp_force_at_config_type        force_at_config; 
    /* Which configuration the AT should use */

#ifdef FEATURE_EHRPD
  boolean ehrpd_auth_in_usim;

  boolean ehrpd_credentials_are_provisioned;
    /* Flag indicating whether eHRPD credentials are provisioned */
  boolean data_ehrpd_capability;
    /* Flag indicating whether upper layer's eHRPD capability is valid */
  boolean data_ehrpd_capability_initialized;
	/* Flag indicating whether data_ehrpd_capability has been initialized */
  boolean ehrpd_network_setting;
    /* Flag indicating whether network setting of eHRPD is True or False */
  boolean ehrpd_no_check;
    /* Flag indicating if ehrpd needs to be checked during hotswap */ 
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_LTE_TO_HDR_OH
  boolean awaiting_prereg_allowed;
    /* Flag indicating whether we are waiting for PrePreAllowed == 1 */
#endif /* FEATURE_LTE_TO_HDR_OH*/

} hdrscp_type;

/*===========================================================================

FUNCTION HDRSCP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSCP.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdrscp_msg_cb
( 
  dsm_item_type * msg_ptr 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRSCP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdrscp_ind_cb
( 
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_ACTIVATE

DESCRIPTION
  This function commands SCP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrscp_activate
(
  hdrhai_protocol_name_enum_type sender 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_DEACTIVATE

DESCRIPTION
  This function commands SCP to deactivate.  

DEPENDENCIES
  None

PARAMETERS
  deactivate_reason - the reason SCP is being deactivated
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrscp_deactivate
(
  hdrhai_session_close_reason_enum_type deactivate_reason,
  hdrhai_protocol_name_enum_type sender 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_POWERUP_INIT

DESCRIPTION
  This function commands SCP to Initialize.  This function is called 
    instead of hdrscp_init at powerup.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - Session is open
  FALSE - Session is not open

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrscp_powerup_init( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_CONFIG_SUBTYPE

DESCRIPTION
  This accessor function returns the InConfig subtype of the requested 
    protocol

DEPENDENCIES
  None

PARAMETERS
  protocol - protocol type for which the subtype is returned

RETURN VALUE
  subtype value of the requested protocol

SIDE EFFECTS
  None

===========================================================================*/

extern uint16 hdrscp_get_config_subtype
(
  hdrhai_protocol_name_enum_type protocol
);

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRSCP_CONFIG_SUBTYPE_IS_HARDLINK

DESCRIPTION
  This function finds if a given protocol's configured subtype is hardlink.
  
DEPENDENCIES
  None.

PARAMETERS
  Protocol - Protocol Type for which subtype need to be checked.

RETURN VALUE
  TRUE if protocol subtype is hardlink, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_config_subtype_is_hardlink
(
  hdrhai_protocol_name_enum_type protocol
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_CURRENT_SUBTYPE

DESCRIPTION
  This accessor function returns the InUse subtype of the requested protocol

DEPENDENCIES
  None

PARAMETERS
  protocol - Protocol type for which the subtype is returned

RETURN VALUE
  Subtype value of the requested protocol

SIDE EFFECTS
  None

===========================================================================*/

extern uint16 hdrscp_get_current_subtype
(
  hdrhai_protocol_name_enum_type protocol
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_RESTORE_FROM_EFS

DESCRIPTION
  This boolean accessor function tells the caller whether they should
  restore data from EFS or use default values.  It is intended to be used
  only during initialization.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - caller should restore the previous session's data from EFS
  FALSE - caller should use default values for session-related data

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrscp_restore_from_efs
(
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_SESSION_CONFIG_TOKEN

DESCRIPTION
  This accessor function returns the session configuration token.

DEPENDENCIES
  None

PARAMETERS
  sender - the protocol giving this command (only for software trace)

RETURN VALUE
  value of the session configuration token (specified in the forward
    Configuration Complete message)

SIDE EFFECTS
  None

===========================================================================*/
extern uint16 hdrscp_get_session_config_token
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSCP_GET_APP_SUBTYPE_CONFIG

DESCRIPTION
  This accessor function returns the configuration for app subtypes.

DEPENDENCIES
  Must be called after hdrscp powerup initialization.

PARAMETERS
  app_subtype_mask - App subtype config mask

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_get_app_subtype_config 
(
  hdrscp_app_subtype_enable_mask_type *app_subtype_mask
);

/*===========================================================================

FUNCTION HDRSCP_IS_CONFIGURING

DESCRIPTION
  Indicates whether SCP is in session configuration
 
DEPENDENCIES
  Must be called after hdrscp powerup initialization.

PARAMETERS
  None

RETURN VALUE
  TRUE  - SCP is in session configuration
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrscp_is_configuring( void );

/*===========================================================================

FUNCTION HDRSCP_SET_HARDLINK_ATTRIBS_TO_DEFAULT

DESCRIPTION
  This function initializes SCP's hardlinked configuration parameters
  to default.

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to SCP's current configuration structure in SCMDB.
                     
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_set_hardlink_attribs_to_default
( 
  hdrscp_hardlink_config_params_type *current_config
);

/*===========================================================================

FUNCTION HDRSCP_SET_PER_PERSONALITY_ATTRIBS_TO_DEFAULT

DESCRIPTION
  This function initializes SCP's per personality configuration parameters
  to default.

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to SCP's current configuration structure in SCMDB.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_set_per_personality_attribs_to_default
( 
  hdrscp_per_personality_config_params_type *current_config
);

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRSCP_IS_PERSONALITY_IRAT

DESCRIPTION
  This function checks whether SCP's current personality is IRAT.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE - current personality is IRAT
  FALSE - current personality is not IRAT 

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrscp_is_personality_irat( void );


/*===========================================================================
FUNCTION HDRSCP_IS_IRAT_ENABLED

DESCRIPTION
  This function tells whether IRAT subtypes are enabled or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - IRAT subtypes are enabled.
  FALSE - IRAT subtypes are disabled.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean hdrscp_is_irat_enabled( void );


/*===========================================================================
FUNCTION HDRSCP_UPDATE_SUBTYPE_DEFAULTS

DESCRIPTION
  This function resets the defaults for protocol subtype to subtype0.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern void hdrscp_update_subtype_defaults( void );
#endif /* FEATURE_LTE_TO_HDR_OH */


/*===========================================================================

FUNCTION HDRSCP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of SCP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of SCP

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrscp_get_inconfig_handle(void);

/*===========================================================================

FUNCTION hdrscp_config_is_locked

DESCRIPTION
  This function checks if AT is allowed to send and receive configuration
  message.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Returns the value of ConfigLock in Open state and TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscp_config_is_locked(void);

/*===========================================================================

FUNCTION HDRSCP_PHONE_IS_SET_TO_REV0_MODE

DESCRIPTION
  This function checks if the phone is provisioned to be in Rev0 mode.
 
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE if both PhyLayer Subtype 2/Subtpe 3 are disabled. 
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_phone_is_set_to_rev0_mode(void);

/*===========================================================================

FUNCTION HDRSCP_PHONE_IS_SET_TO_REVA_MODE

DESCRIPTION
  This function checks if the phone is provisioned to be in RevA mode.
 
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Returns TRUE if PhyLayer Subtype 2 is enabled. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_phone_is_set_to_revA_mode(void);

/*===========================================================================

FUNCTION HDRSCP_GENERATE_PRIOR_SESSION_ATTRIBUTE

DESCRIPTION
  Local helper function to configure the previous session

DEPENDENCIES
  Message ID and Transaction ID have already been packed into send_msg_ptr

PARAMETERS
  send_msg_ptr - pointer to the dsm item where the previous session info
                 is to be packed
  aoffset      - offset of this attribute in the dsm item
  send_on_tc   - prior session attribute to be send on traffic channel

RETURN VALUE
  Number of bits in the previous session attribute added to the send_msg_ptr

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrscp_generate_prior_session_attribute
(
  dsm_item_type * send_msg_ptr,
  uint16 aoffset,
  boolean send_on_tc
);

/*===========================================================================

FUNCTION HDRSCP_POWER_DOWN

DESCRIPTION
  This function is called during AT power down and saves the session
  status appropriately
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscp_power_down( void );

/*===========================================================================

FUNCTION HDRSCP_SILENT_RESTORE_PREV_SESSION

DESCRIPTION
  This function silently restore prior session

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscp_silent_restore_prior_session( void );

/*===========================================================================

FUNCTION HDRSCP_GET_LOGGING_SUBTYPE

DESCRIPTION
  This accessor function is used by SLP to get the configured subtype of the 
  requested protocol. If active personality switch is in progress, this function
  returns the configured subtype that the AT is going to commit after connection 
  is closed, otherwise, it returns the configured subtype. 

  NOTE:- If in the current round of negotiating a personality, the protocol 
  subtype is configured as hardlink this function would return the subtype
  of the protocol in main personality.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol type for which the subtype is returned
  prot_inst - Instance (InUse/InConfig) of the protocol to log

RETURN VALUE
  Subtype value of the requested protocol

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrscp_get_logging_subtype
(
  hdrhai_protocol_name_enum_type protocol,
  hdrhai_protocol_instance_enum_type prot_inst,
  hdrhai_channel_enum_type channel
);

/*===========================================================================

FUNCTION HDRSCP_AT_IS_DORB_CAPABLE

DESCRIPTION
  This function returns whether DORB is enabled in the NV item 
  NV_HDRSCP_FORCE_AT_CONFIG_I. 

  Please note that this function doesn't return whether DORB is negotiated. A
  return value of TRUE doesn't mean that DORB is negotiated. Despite AT's 
  capability, DORB configuration could still be rejected by AN.
  
DEPENDENCIES
  Assumes that the global variable force_at_config is already initialized.
  Must only be called after the NV item NV_HDRSCP_FORCE_AT_CONFIG_I has been
  read.

PARAMETERS
  TRUE - If AT is capable of DORB
  FALSE - Otherwise

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrscp_at_is_dorb_capable ( void );

#ifdef FEATURE_EHRPD
/*===========================================================================

FUNCTION HDRSCP_EHRPD_CREDENTIALS_ARE_PROVISIONED

DESCRIPTION
  This function returns whether eHRPD credentials are provisioned in the NV
  or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if eHRPD credentials are provisioned. FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_credentials_are_provisioned( void );

/*=============================================================================
FUNCTION HDRSCP_UPDATE_EHRPD_CREDENTIALS

DESCRIPTION
  This function posts a command to SCP to process update to eHRPD credentials.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrscp_update_ehrpd_credentials
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSCP_SET_EHRPD_CREDENTIALS_ARE_VALID

DESCRIPTION
  This function is used to set the flag whether eHRPD credentials are 
  provisioned or not.

DEPENDENCIES
  None.

PARAMETERS
  ehrpd_credentials_available - Flag indicating whether eHRPD credentials 
  are available

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_ehrpd_credentials_are_valid
( 
  boolean ehrpd_credentials_available 
);

/*===========================================================================

FUNCTION HDRSCP_EHRPD_AUTH_IS_IN_USIM

DESCRIPTION
  This function is used to check the flag whether eHRPD credentials are 
  in USIM.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_auth_is_in_usim ( void );


/*===========================================================================

FUNCTION HDRSCP_SET_DATA_EHRPD_CAPABILITY_INTERNAL

DESCRIPTION
  This function sets the flag which tells eHRPD capability (e.g. APN,
  authentication) are valid or not from upper layer (data service)'s point of
  view.
  This function only sets the value and does not check whether it is needed
  to close session or not.

DEPENDENCIES
  None

PARAMETERS
  TRUE - upper layer's eHRPD capability is valid. 
  FALSE - upper layer's eHRPD capability is invalid.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_data_ehrpd_capability_internal
( 
  boolean data_ehrpd_capability
);

/*===========================================================================

FUNCTION HDRSCP_LTE_IS_ATTACHED

DESCRIPTION
  This function resets ehrpd_was_advertised flag after LTE successful attach
  so that session will be renegotiated if current session is HRPD.
 
  This function should be called only after a successful LTE attach.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_lte_is_attached( void ); 


/*===========================================================================

FUNCTION HDRSCP_SET_DATA_EHRPD_CAPABILITY

DESCRIPTION
  This function sets the flag which tells eHRPD capability (e.g. APN,
  authentication) are valid or not from upper layer (data service)'s point of
  view.

DEPENDENCIES
  None

PARAMETERS
  TRUE - upper layer's eHRPD capability is valid. 
  FALSE - upper layer's eHRPD capability is invalid.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_data_ehrpd_capability
( 
  boolean data_ehrpd_capability
);

/*===========================================================================

FUNCTION HDRSCP_EHRPD_IS_ALLOWED

DESCRIPTION
  This function tells whether eHRPD capability should be advertised based on
  all the condition (APNs, eHRPD network setting, eHRPD credentials) when this
  funtion is called.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - eHRPD is allowed. 
  FALSE - eHRPD is not allowed.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_is_allowed( void );

/*===========================================================================

FUNCTION HDRSCP_EHRPD_WAS_ADVERTISED

DESCRIPTION
  This function tells whether eHRPD capability was advertised when negotiating
  ATSupportedApplicationSubtypes attribute.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - eHRPD has been advertised. 
  FALSE - eHRPD was not advertised.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_was_advertised( void );

/*===========================================================================

FUNCTION HDRSCP_EHRPD_STATUS_IS_VALID

DESCRIPTION
  This function checks whether eHRPD status of current session is valid.
 
  AT's eHRPD   Current active  has eHRPD advertised        Action
  capability	 personality   when session negotiated 
    True          HRPD             True	          Retain the current DO session
    True          HRPD             False          Close the current session
    True          eHRPD            True           Retain the current DO session
    True          eHRPD            False                    NA
    False         HRPD             True	          Retain the current DO session
    False         HRPD             False          Retain the current DO session
    False         eHRPD            True           Close the current session
    False         eHRPD            False                    NA

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - Session is valid.
  FALSE - eHRPD availability of current session is invalid. Session needs to be
          closed.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_status_is_valid( void );

/*===========================================================================

FUNCTION HDRSCP_CHECK_OK_TO_OPEN_STATE

DESCRIPTION
  This function checks whether the current eHPRD availability before SCP 
  goes to OPEN state.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - SCP is OK to go to OPEN state.
  FALSE - eHRPD status may have changed and current session has to be closed.
          SCP won't go to OPEN state.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_check_ok_to_open_state( void );

/*===========================================================================

FUNCTION HDRSCP_SET_EHRPD_NO_CHECK

DESCRIPTION
  This function is used to set/reset the flag, which indicates whether eHRPD 
  validation should be made, when there is a trigger.

DEPENDENCIES
  None.

PARAMETERS
  Value - Flag indicating if ehrpd check needs to be performed or not when
          there is any trigger to validate ehrpd.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_ehrpd_no_check
( 
  boolean value 
);
#endif /* FEATURE_EHRPD */


/*===========================================================================

FUNCTION HDRSCP_GET_CURRENT_HDR_REV

DESCRIPTION
  This function returns current HDR revision 
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  SYS_ACTIVE_PROT_NONE - The protocol revision is unknown (no session)
  SYS_ACTIVE_PROT_HDR_REL0 - HDR release 0
  SYS_ACTIVE_PROT_HDR_RELA - HDR release A
  SYS_ACTIVE_PROT_HDR_RELB - HDR release B

SIDE EFFECTS
  None.

===========================================================================*/

sys_active_prot_e_type hdrscp_get_current_hdr_rev( void );


/*===========================================================================

FUNCTION HDRSCP_SESSION_NEGOTIATION_IN_PROGRESS

DESCRIPTION
  This function checks if session negotiation is in progress or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE -if session negotiation is in progress
  FALSE - session negotiation NOT in progress

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_session_negotiation_in_progress(void);

#endif               /* HDRSCP_H */
