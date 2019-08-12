#ifndef CM_LOG_V_H
#define CM_LOG_V_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            CM Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging CM packets.

Copyright (c) 2002  - 2010 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmlog_v.h#1 $

when       who      what, where, why
--------   ---      --------------------------------------------------------- 
10/21/13   jvo      Modified cmlog_event_ssac_timer_s_type to match unaligned
                    qxdm event payload
08/06/13   xs       Add function cmlog_csfb_svlte_switch() to log switch between
                    SVLTE and CSFB
04/12/12   fj       Separate call_orig and call_connect log structure for DSDS 
                    and non-DSDS. 
05/22/12   ab       Reverted back the changes for WLAN as 
                    QXDM tool is not able to parse the CM SS event
01/06/12   gm       Data synchronization changes for freefloating platform
12/13/11   sg       Log packet updates TDS
07/24/11   aj       SC SVLTE 1.0 merge to mainline
04/29/11   rm       Adding req/resp packet for CDMA info
04/21/11   sv       Correct Lint Errors
04/13/11   rm       Moving LOG_CM_CALL_EVENT_C to cmlog.h 
12/23/10   sv       Integrate DSDS feature
07/12/10   mj       LTE log packet updates
11/16/09   ak       Rectified command value definitions.
11/04/09   ak       Added support for Ecall commands.
07/01/09   fj       Changes due to LTE merge.
05/27/09   aj       Adding support for updating clients with cdma local time
05/27/09   aj       Adding support for passing otasp status to clients
05/15/09   sg       Update SS EVENT log packet with new SS events added in CM
02/02/09   ks       Adding support for QDSP 6 based compiler
01/29/09   fj       Remove FEATURE_HDR_HYBRID for call events
03/17/08   am       Added two diag commands to support 
                    commands for CM_CALL_CMD_MODIFY_REQ / RSP
01/03/08   ka       Adding handset based plus code dialing feature.
04/15/07   rp       Removing FEATURE_MMGPS
11/17/06   jqi      Added support for HDR active protocol revision information.
11/09/06   rp       Added support for SS req/response packet
                    CM_GET_CELL_SERVICE_IND.
04/19/06   jqi      Added diag subsystem command(select system preference 
                    command2) to process 64-bit band preference. 
03/10/06   pk       Added support for IMSI and MCC for IS 95 systems. Changed
                    the changed fields of the ss log packet to 64 bits
01/05/06   pk       Modified the SS log packet structure. Replaced constants
                    with hard coded values, repalced boolean with uint8
12/10/05   ic       Lint cleanup
11/21/05   ic       Send EVENT_CM_OPERATIONAL_MODE on transition from 
                    SYS_OPRT_MODE_OFFLINE_CDMA to SYS_OPRT_MODE_RESET.
                    Fix the functionality that postpones sending of 
                    EVENT_CM_OPERATIONAL_MODE for SYS_OPRT_MODE_OFFLINE_CDMA 
                    until after TASK_OFFLINE_SIG from TMC comes (CR73050).
10/27/05   pk       Added support for SS log packet and its data structures
06/21/05   ic       Renamed DIAG_MAX_GSM_DIGITS to CM_DIAG_MAX_GSM_DIGITS  
06/10/05   sk       Moved user defined data types from cmlog.c to here.
01/24/05   ic       Hardcoded array sizes in declarations of phone and call 
                    event log packets
01/14/05   ic       Added comments about contents of num field in 
                    LOG_CM_CALL_EVENT_C_type declaration
01/06/05   ic       Mainlined FEATURE_CM_CALL_EVENT_LOGGING and 
                    FEATURE_CM_PH_EVENT_LOGGING. 
01/05/05   ic       Log packet version for call and phone event packets
12/20/04   ic       In LOG_CM_PH_EVENT_C_type and LOG_CM_CALL_EVENT_C_type
                    declarations changed all int to int32 
12/19/04   ic       Added support for sending LOG_CM_CALL_EVENT_C under
                    FEATURE_CM_CALL_EVENT_LOGGING
12/10/04   ic       Refinments for CR: 48569 - HDR-KDDI: Request for new EVENTs 
                    for DDTM mode (LOG_CM_PH_EVENT_C)
12/06/04   ic       Include log.h instead of log_dmss.h to correct RVCT 
                    compiler error on 6250 ("could not find log_dmss.h")
12/05/04   ic       Put definition of LOG_CM_PH_EVENT_C_type under 
                    FEATURE_CM_PH_EVENT_LOGGING
12/05/04   ic       Added support for sending LOG_CM_PH_EVENT_C  
08/10/04   ic       Removed cmlog_get_operating_mode() and cmlog_system_mode()
                    as they became obsolete. 
03/18/04   ka       Prototype for cmlog_set_cm_offline_signal_recvd ()
02/06/04   ws       Initial jaguar/mainline merge.
05/21/03   vt       Added new call events.
03/21/03   ATM      Added event logging for svc_cnf and nw_list_cnf
02/12/03   atm      Added pref logging and moved subsys command from UMTS to
                    CM.
01/21/03   ws       Updated copyright information for 2003
04/26/02   atm      Initial version.
===========================================================================*/

#include "mmcp_variation.h"
#include "cm.h"
#include "comdef.h" 
#include "sys.h"    
#include "sys_plmn_selection.h"    
#include "cmcall.h"
#include "log.h" 
#include "cmsoa.h"

#define CM_LOG_PACKET_CONTENTS_NOT_DEFINED   0
/* 
** Filler for log packet fields when there is
** nothing meaningful to write into them
*/

#define CM_LOG_PH_EVENT_LOG_PACKET_VERSION   6
/* 
** Version number for LOG_CM_PH_EVENT_C_type
** New fields are to be added at the end of the structure only
** (DIAG and tools requirement).
** Whenever a new field is added to the structure, the value of
** the constant below needs to be increased by 1 and
** DIAG and tools group need to be notified about the changes. 
*/

#define CM_LOG_CALL_EVENT_LOG_PACKET_VERSION 3
/* 
** Version number for LOG_CM_CALL_EVENT_C_type
** New fields are to be added at the end of the structure only
** (DIAG and tools requirement).
** Whenever a new field is added to the structure, the value of
** the constant below needs to be increased by 1 and
** DIAG and tools group need to be notified about the changes. 
*/

#define CM_LOG_SS_EVENT_LOG_PACKET_VERSION 8
/* 
** Version number for LOG_CM_SS_EVENT_C_type
** New fields are to be added at the end of the structure only
** (DIAG and tools requirement).
** Whenever a new field is added to the structure, the value of
** the constant below needs to be increased by 1 and
** DIAG and tools group need to be notified about the changes. 
*/

#define CM_LOG_SS_EVENT_MSIM_LOG_PACKET_VERSION 1

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/*
** Data structure used to log CM phone event SUBS_INFO
** We log current PH event along with what's in cm_subs_info_s_type
*/
LOG_RECORD_DEFINE(LOG_CM_SUBS_EVENT_C)

  byte                  subscription_id;
    /**< Active Subscription ID assigned to this subscription*/

  int32                 mode_pref;
    /* Switch to this mode preference       
    ** CM_MODE_PREF_GSM_ONLY
    ** CM_MODE_PREF_WCDMA_ONLY
    ** CM_MODE_PREF_GSM_WCDMA_ONLY
    ** CM_MODE_PREF_CDMA_ONLY
    ** CM_MODE_PREF_HDR_ONLY
    ** CM_MODE_PREF_CDMA_HDR_ONLY
    ** CM_MODE_PREF_PERSISTENT       
    ** CM_MODE_PREF_NO_CHANGE
    ** Only the above modes are allowed.
    */

  int32                 pref_term;
      /* Indicate the current mode preference term -
      ** one-call, power-cycle, etc.*/

  int32                 acq_order_pref;
    /* gw acquisition order preference */

  int32                 srv_domain_pref;
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  int64                 band_pref;
    /* Switch to this band preference */

  int32                 roam_pref;
      /* Indicate the current roam preference */

  int32                 hybr_pref;
    /* Indicate the hybr preference setting */

  int32                 network_sel_mode_pref;
    /* Defines whether the network should be selected automatically
    ** or manually */

  PACKED struct PACKED_POST
  {
    byte                 identity[3];
  } plmn;
    /* If network selection is set to manual, this specifies plmn id */

  int32                 prl_pref;
    /**< The current prl preferred setting */

  uint8                 sim_id[50];
    /**< SIM id of the card to which this subscription belongs */

  PACKED struct PACKED_POST
  {
    uint32                data_len;

    uint8                 data_ptr[32];
  } app_id;
    /**< Application ID (AID) assocuated with this subscription */

  uint16                             session_type;
    /**< UIM session type on which this subscription is currently provisioned */

  boolean                           is_operational;
    /**< Flag indicating if this subscription is in operation right now */

  boolean                           is_priority_subs;
    /**< Flag indicating if this subscription is the priority subscription*/

  boolean                           is_default_voice;
    /**< Flag indicating if this subscription is the default voice subs */

  boolean                           is_default_data;
    /**< Flag indicating if this subscription is the default data subs */


LOG_RECORD_END


/*
** Data structure used to log CM phone events
** We log current PH event along with what's in cm_ph_info_s_type
*/
LOG_RECORD_DEFINE(LOG_CM_PH_EVENT_C)

  uint8                  version;
    /* Version of the packet (CM_LOG_PH_EVENT_LOG_PACKET_VERSION)
    */ 

  int32                  ph_event;
    /* PH event, as defined by cm_ph_event_e_type
    */

  boolean                is_in_use;
    /* Indicate whether phone is currently in use 
    */

  int32                  oprt_mode;
    /* Indicate current operating mode (sys_oprt_mode_e_type) 
    */

  int32                  test_control_type;
    /* Indicate current test control type (cm_test_control_type_e_type) 
        */

  int32                  cdma_lock_mode;
    /* 
    ** Indicate whether phone is CDMA locked until power cycle 
    ** (cm_cdma_lock_mode_e_type) 
    */

  int32                  answer_voice;
    /* 
    ** Indicate whether phone should answer incoming
    ** voice calls as voice, fax or modem. CDMA only (cm_answer_voice_e_type) 
    */

  int32                  mode_pref;
    /* Indicate current mode preference (cm_mode_pref_e_type) 
    */

  int32                  pref_term;
    /* 
    ** Indicate the current preference term - one-call, power-cycle, etc.
    ** (cm_pref_term_e_type)
    */

  int32                  acq_order_pref;
    /* 
    ** The acquisition order preferred by the client. GSM/WCDMA only. 
    ** (cm_gw_acq_order_pref_e_type) 
    */

  int32                  srv_domain_pref;
    /* 
    ** The service domain preferred by the client.
    ** This field is set after a call to cm_ph_cmd_sys_sel_pref_new.
    ** GSM/WCDMA only (cm_srv_domain_pref_e_type). 
    */

  int64                  band_pref;
    /* Indicate current band preference (cm_band_pref_e_type)
     * for CDMA, GSM, WCDMA, systems
    */

  int32                  roam_pref;
    /* Indicate current roam preference (cm_roam_pref_e_type) 
    */

  int32                  nam_sel;
    /* Indicate the NAM selection. CDMA only (cm_nam_e_type) 
    */

  int32                  curr_nam;
    /* 
    ** Indicate the current NAM -
    ** needed when NAM selection is set to AUTO-NAM. CDMA only 
    ** (cm_nam_e_type)
    */

  int32                  packet_state;
    /* Indicate current packet state. CDMA only (cm_packet_state_e_type) 
    */

  int32                  hybr_pref;
    /* Indicate hybrid preference (cm_hybr_pref_e_type) 
    */

  int32                  network_sel_mode_pref;
    /* 
    ** The network selection mode selected by the client. GSM/WCDMA only 
    ** (cm_network_sel_mode_pref_e_type)
    */
    
  PACKED struct PACKED_POST
  {
    byte                 identity[3];
  } plmn;
    /* 
    ** The identifier of the PLMN to be selected manually. GSM/WCDMA only
    ** (sys_plmn_id_s_type) 
    */

  PACKED struct PACKED_POST
  {
    byte                 length;
      /* The number of PLMNs in the list below 
      */
    
    PACKED struct PACKED_POST
    {
        
      PACKED struct PACKED_POST
      {
        byte             identity[3];
      } plmn;
        /* (sys_plmn_id_s_type) 
        */ 
            
      uint16             access_tech;
        /* sys_access_tech_e_type
        */
            
    } info[ 50 ];
      /* 
      ** (sys_user_pref_plmn_list_info_s_type) 
      ** Hardcoded to the value of SYS_USER_PPLMN_LIST_MAX_LENGTH
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet. 
      */
        
  } user_pref_networks;
    /* 
    ** The user preferred network list. GSM/WCDMA only 
    ** (sys_user_pref_plmn_list_s_type)
    */

  PACKED struct PACKED_POST
  {
    
    uint32               length;
    /* The number of PLMNs in the list 
    */

    PACKED struct PACKED_POST
    {
     
      PACKED struct PACKED_POST
      {
        byte             identity[3];
      } plmn;
        /* The ID of the PLMN (sys_plmn_id_s_type)                                        
        */

      int32              rat;
        /* The radio access technology of the PLMN (sys_radio_access_tech_e_type)                   
        */

      int32              plmn_service_capability;
        /* 
        ** Indicates the type of service domain the PLMN can provide 
        ** (sys_srv_capability_e_type)
        */

      boolean            plmn_forbidden;
        /* Indicates if the PLMN is forbidden                        
        */

      int32              list_category;
        /* Indicates the type of PLMN (sys_detailed_plmn_list_category_e_type)                               
        */

      int32              signal_quality;
        /* The signal quality of the PLMN (sys_signal_quality_e_type)                            
        */

      int32              signal_strength;
        /* The signal strength of the PLMN (sys_signal_strength_type)                        
        */

    } info[ 40 ]; 
      /* 
      ** (sys_detailed_plmn_list_info_s_type)
      ** Hardcoded to the value of SYS_PLMN_LIST_MAX_LENGTH
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet.      
      */
        
  } available_networks;
    /* Available networks. GSM/WCDMA only (sys_detailed_plmn_list_s_type)
    */

  boolean                is_cdma_subscription_available;
    /* 
    ** Indicate whether the cdma subscription/provisioning information
    ** is available for consumption.                              
    */

  boolean                is_gwl_subscription_available;
    /* 
    ** Indicate whether the GSM/WCDMA/LTE subscription/provisioning 
    ** information is available for consumption.                              
    */

  int32                  rtre_config;
    /* 
    ** Current RTRE configuration - RUIM, NV, RUIM fallback NV. CDMA only 
    ** (cm_rtre_config_e_type)
    */

  int32                  rtre_control;
    /* Current RTRE control - RUIM or NV. CDMA only (cm_rtre_control_e_type)
    */

  int32                  orig_srv_domain_pref;
    /* 
    ** service domain preference for PS attach/detach commands.
    ** This is different from the phone service domain preference
    ** (cm_srv_domain_pref_e_type)
    */

  int32                  ddtm_pref;
    /* Data Dedicated Tranmission mode (DDTM) preference (cm_ddtm_pref_e_type).
    */

  boolean                cur_ddtm_status;
    /* ddtm status 
    */


  int32                  ddtm_act_mask;
    /* DDTM action mask settings (sys_ddtm_act_mask_e_type).
    */

  uint16                 ddtm_num_srv_opt;
    /* Number of service options specified in below list.
    */

  uint16                 ddtm_srv_opt_list[ 10 ];
    /* 
    ** List of service options whose pages have to be ignored provided
    ** ddtm_pref & ddtm_act_mask allows it (sys_srv_opt_type).
    ** Hardcoded to the value of SYS_DDTM_MAX_SO_LIST_SIZE
    ** at the time log packet was declared. 
    ** This is intentional as otherwise changes to the constant would
    ** change byte ordering in the log packet.    
    */

  int32                  cphs_allowed;
    /*
    ** Whether Common PCN Handset Specifications procedures are allowed
    ** FEATURE_ALS (cm_cphs_proc_e_type)
    */

  int32                  als_allowed;
    /*
    ** Whether ALS procedures are allowed.
    ** FEATURE_ALS (cm_als_proc_e_type)
    */

  int32                  line;
    /*
    ** Currently selected line (all MO voice calls will use it)
    ** FEATURE_ALS (cm_als_line_e_type)
    */

  int32                  line_switching;
    /*
    ** Whether the user is allowed to switch between lines
    ** FEATURE_ALS (cm_als_line_switching_e_type)
    */

  int32                  requesting_client_id;
    /* Requesting client (cm_client_id_type) 
    */

  PACKED struct PACKED_POST {

    uint8                type;
      /* Message type.
      */

    uint32               payload_len;
      /* Message payload length in bits max is SYS_MAX_IS2000_MSG_SIZE * 8
      */

    byte                 payload[ 256 ];
      /* Message payload.
      */
  } cdma_capability;
    /* 
    ** CDMA capability in the form of IS2000 Status Message 
    ** (sys_is2000_msg_s_type)
    ** FEATURE_UMTS_1X_HANDOVER_1XMSM || FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    ** otherwise filled with 0s.
    ** Hardcoded to the value of SYS_MAX_IS2000_MSG_SIZE
    ** at the time log packet was declared. 
    ** This is intentional as otherwise changes to the constant would
    ** change byte ordering in the log packet.      
    */

  int32                  msm_id;
    /* Identifies the MSM which generated this event (sys_msm_id_e_type).
    ** FEATURE_UMTS_1X_HANDOVER_1XMSM || FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    ** otherwise filled with 0s
    */


  int32                  orig_ddtm_act_mask;
    /* DDTM action mask settings (sys_ddtm_act_mask_e_type).
    */

  int64                  lte_band_pref;
  /* Indicate current LTE band preference (cm_band_pref_e_type) 
  */

  byte                   asubs_id;

  boolean                is_hybr_gw_subscription_available;
    /* 
    ** Indicate whether the GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              
    */

  boolean                is_hybr_gw3_subscription_available;
    /* 
    ** Indicate whether the GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              
    */

  byte                   standby_pref;
      /* Standby preference of the Phone */

  byte                   active_subs;
      /* The active subscription in Single Standby mode
      ** only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY
      */

  byte                   default_voice_subs;
      /* default voice subscription */

  byte                   default_data_subs;
      /* default data subscription */

  byte                   priority_subs;
      /* priority subscription */

  int64                  tds_band_pref;
     /* Indicate current TDS band preference (cm_band_pref_e_type) Used 
     ** with CM_BAND_PREF_TDS_* values.
     */      
  int64                  lte_band_pref_65_128;
  /**< Indicate current LTE band preference (cm_band_pref_e_type)
  */
  int64                  lte_band_pref_129_192;
  /**< Indicate current LTE band preference (cm_band_pref_e_type)
  */
  int64                  lte_band_pref_193_256;
  /**< Indicate current LTE band preference (cm_band_pref_e_type)
  */

LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CM_SS_MSIM_EVENT_C)
  uint8						version;
	  /* Version of the packet (CM_LOG_SS_MSIM_EVENT_LOG_PACKET_VERSION)
    */ 
  int32 					 ss_event;
			  /* SS event, as defined by cm_ss_event_e_type
			  */

  int32 				    asubs_id;
			  
  uint8                     number_of_stacks ;

  PACKED struct PACKED_POST
  {
    uint64                                  changed_fields ;
	uint64                                  changed_fields2 ;
	uint64                                  signal_strength_changed_fields ;
	boolean                                 is_operational ;
	
	int32                                   srv_status;
    /* The service status (SERVICE/NO SERVICE) of the system.
    ** Mask used is CM_SS_SRV_STATUS_MASK.
    */
    int32                                   true_srv_status ;
  /*CM_SS_EVT_TRUE_SRV_STATUS_MASK               changed_fields*/

    int32                                   srv_domain;
    /* The service domain registered on the system. /
    ** Mask used is CM_SS_SRV_DOMAIN_MASK.
    */
    
    int32                                   srv_capability;
    /* The system's service capability.
    ** Mask used is CM_SS_SRV_CAPABILITY_MASK.
    */
    int32                                   sys_mode;
    /* The system's mode.
    ** Mask used is CM_SS_SYS_MODE_MASK.
    */
    PACKED union PACKED_POST
  	{
  
      PACKED struct PACKED_POST
      {
        uint8                  bit_err_rate;
          /* Bit Error Rate / RX QUAL in GSM 
          */
  
        int32                  sim_state;
          /* Current state of the SIM.
          ** Mask used is CM_SS_SIM_STATE_MASK.
          ** sys_sim_state_e_type
          */
  
        uint8                plmn_forbidden;
          /* Indicates whether the PLMN is forbidden.
          ** Mask used is CM_SS_PLMN_FORBIDDEN_MASK.
          */
  
        PACKED struct PACKED_POST
        {
          /*
          ** Flags that indicate whether the particular mm information is
          ** available.
          **/
          uint8              plmn_avail;
          uint8              full_name_avail;
          uint8              short_name_avail;
          uint8              univ_time_and_time_zone_avail;
          uint8              time_zone_avail;
          uint8              lsa_identity_avail;
          uint8              daylight_saving_adj_avail;
  
          PACKED struct PACKED_POST
          {
            byte               identity[3];
          } plmn;
  
          PACKED struct PACKED_POST
          {
            uint8              length;
              /* Number of characters in the full name 
              */
  
            int32              coding_scheme;
              /* The coding scheme used to encode the full name of the network 
              ** sys_network_name_coding_scheme_type_e_type   
              */
  
            uint8            add_country_initials;
              /* Flag that indicates whether MS should add the country initials 
              ** to the full name of the network 
              */
  
            uint8              name[255];
              /* The full name of the network
              */
  
              /* 
              ** name 
              ** Hardcoded to the value of SYS_MAX_FULL_NAME_CHARS
              ** at the time log packet was declared. 
              ** This is intentional as otherwise changes to the constant would
              ** change byte ordering in the log packet.          
              */
  
          } full_name;
  
          PACKED struct PACKED_POST
          {
  
            uint8              length;
              /* Number of characters in the full name 
              */
  
            int32              coding_scheme;
              /* The coding scheme used to encode the full name of the network 
              ** sys_network_name_coding_scheme_type_e_type   
              */
  
            uint8            add_country_initials;
              /* Flag that indicates whether MS should add the country initials 
              ** to the full name of the network 
              */
  
            uint8              name[255];
              /* The full name of the network
              */
  
              /* 
              ** name 
              ** Hardcoded to the value of SYS_MAX_SHORT_NAME_CHARS
              ** at the time log packet was declared. 
              ** This is intentional as otherwise changes to the constant would
              ** change byte ordering in the log packet.          
              */
  
          } short_name;
  
          PACKED struct PACKED_POST
          {
  
             uint8             year;
             uint8             month;
             uint8             day;
             uint8             hour;
             uint8             minute;
             uint8             second;
             int               time_zone;
               /*  sys_time_zone_type
               */
  
          } univ_time_and_time_zone;
  
          int                  time_zone;
            /* sys_time_zone_type    
            */
          PACKED struct PACKED_POST
          {
            int32              type;
            /*  sys_lsa_identity_type_e_type
            */
  
            uint8              identity[3];
  
          } lsa_identity;
  
          int32                daylight_saving_adj;
            /* sys_daylight_saving_adj_e_type            
            */
  
        } mm_information;
  
          /* Mobility management information, i.e. network name, date,
          ** time and time zone of registered network */
  
      } gw_info;
  
          /* GSM/WCDMA SS information 
          */
  
      PACKED struct PACKED_POST
      {
  
  
        int32                  band_class;
          /* cellular or PCS 
             ** sys_band_class_e_type       
             */
     
        int32                  block_or_system;
          /* PCS frequency block: A to F Cellular System A or B
             ** sys_blksys_e_type  
             */
  
        uint16                 reg_zone;
          /* 12 bit registration zone 
               */
  
        uint16                 packet_zone;
          /* 8  bit packet zone, 0xffff means PACKET ZONE NONE.
             ** Mask used is CM_SS_PACKET_ZONE_MASK.
             */
  
        uint8                  bs_p_rev;
          /* 8  bit base station p_rev.
              ** Mask used is CM_SS_BS_P_REV_MASK.
              */
  
        uint8                  p_rev_in_use;
             /* 8  bit p_rev in use the lessor of BS p_rev and mobile p_rev.
                  ** Mask used is CM_SS_P_REV_IN_USE_MASK.
                  */
  
        uint8                  is_registered;
          /* is mobile registered on the current system */
  
        uint8                  ccs_supported;
          /* is concurrent services supported.
          */
  
        int32                  uz_id;
           /* User zone Identification of the selected user zone,
              ** == CM_INVALID_UZID, if no user zone selected. FEATURE_USERZONE.
              ** Mask used is CM_SS_UZ_CHANGED_MASK.
              */
  
        PACKED struct PACKED_POST
        {
          byte                 uz_name_encoding;
  
          byte                 uz_name_n_octets;
  
          byte                 uz_name[32];
  
  
           /* 
              ** uz_name 
              ** Hardcoded to the value of SYS_PUZL_MAX_UZ_NAME_LEN
              ** at the time log packet was declared. 
              ** This is intentional as otherwise changes to the constant would
              ** change byte ordering in the log packet.          
              */
  
        }uz_name;
          /* User zone name of the selected user zone.
             ** Valid only if uz_id != CM_INVALID_UZID. FEATURE_USERZONE.
             ** Mask used is CM_SS_UZ_CHANGED_MASK.
             */
     
        byte                   srch_win_n;
          /* Neighbour set window search size.
             ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
             */
  
        int32                  base_lat;
          /* Base station latitude[0.25sec and already sign extended].
            ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
            ** sys_base_lat_type  
            */
  
        int32                  base_long;
          /* Base station latitude[0.25sec and already sign extended].
             ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
             ** sys_base_long_type
             */
  
        word                   base_id;
          /* Base station Identification number.
              ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
              ** sys_base_id_type
              */
  
        PACKED struct
        {
          int32                  sys_mode;
          /* Indicates the system from which the time was obtained (sys_sys_mode_e_type) 
          */
          PACKED union
          {
            PACKED struct
            {
              uint8                lp_sec;
              int8                 daylt_savings;
              uint8                ltm_offset;
            }cdma_time;
  
            PACKED struct
            {
              uint8                lp_sec;
              int16                ltm_offset;
            }hdr_time;
  
          }time;
  
        }time_info;
        /* Time obtained from sync channel / sector parameter msg
          ** Mask used is CM_SS_CDMA_TIME_CHGD_MASK.
          */
  
      } cdma_info;
          /* CDMA SS information 
          */
      }mode_info ;

      
	  int32 					 roam_status;
		/* Indicate current roaming status.
		** Mask used is CM_SS_ROAM_STATUS_MASK.
		*/
	PACKED struct PACKED_POST
	{
    int32                    id_type;

    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        uint16               sid;
          /* The system ID 
          */

        uint16               nid;
          /* The network ID 
          */

      } is95;
        /* IS 95 sys id
        */

      byte                   is856[16];
        /* HDR sys id
        */

      PACKED struct PACKED_POST
      {
        byte                 identity[3];

      } plmn;
        /* UMTS (WCDMA/GSM) system ID.
        */

      PACKED struct PACKED_POST
      {
        uint8                len;
          /* Length of the SSID, if == 0, then SSID = broadcast SSID.
          */
        char                 ssid[32];
          /* SSID of the wlan system.
          */

          /* 
          ** ssid 
          ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.          
          */

      } ssid;
        /* WLAN system Id.
        */

      PACKED struct PACKED_POST
      {
        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        word                 lac;
          /* LAC
          */
      } plmn_lac;
        /* UMTS (WCDMA/GSM) PLMN and LAC ID  
        */
      }id;

    }sys_id;
		
	uint8					 is_sys_forbidden;
     /* Indicates whether the System is forbidden.
	 ** Mask used is CM_SS_SYS_FORBIDDEN_MASK.
	*/
	
	uint16 					rssi;
		   /* RSSI in positive dBm i.e. 75 means -75dBm. The range for
		   ** AMPS: 	 -110 to -89 dBm
		   ** 800 CDMA:  -105 to -90 dBm
		   ** 1900 CDMA: -108 to -93 dBm
		   ** GSM/WCDMA: -105 to -60 dBm
		   */

	int16					  ecio;
	int						  io;
	byte 					  sinr;
			 /* ECIO in negative 0.5 dBm i.e. 63 means -31.5dBm.
			 ** Io field is valid only when in HDR mode only operation
			 ** SINR range: from 0 to 8, Level 8 represents highest SINR
			 ** SINR applicable only when sys_mode = HDR.
			 */
		   
	int16 				   pathloss;
			   /**< Pathloss is applicable only in Connected mode (otherwise 0xFF)
			   */
			 
	int16 				   sir;
			   /**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
			   ** connected mode when DPCCH is active.
			   */
			 
	int16					   rssi2;
			   /**< The -ve RSSI value, coming from RRC layer
			   */
			 
	int16					   rscp;
			   /**< The range of RSCP would be from -28 to -121 dBm
			   */
			   
	boolean					 ecio_for_display;
			 /**< TRUE if UE is in Data call
			 */
			 
	int32					   psist;
			/* Indicate psist of the system.
			  ** Mask used is CM_SS_PSIST_CHGD_MASK.
			  ** cm_psist_e_type   
			*/
			 
	int32						active_band;
			  /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
			  ** using this structure.
			  **
			  */

	uint16						active_channel;
			  /**< Report active channel of the current serving system.
			  */
	PACKED struct PACKED_POST
    {

    uint8                  ss_thr_status;
      /* The throttle status of the current ss
      */

    dword                    thr_uptime;
      /* The throttling time period
      */

    unsigned int             idx;
      /* The index in the throttle_time table
      ** Used internally by CM to keep track of the next throttle time 
      ** increment
      */

    int32                    call_status;
      /* The call end reason
      ** cm_ss_orig_thr_reasons_e_type
      */

    } thr_status;
			  
	PACKED struct PACKED_POST
	{
	  uint32				   cell_id;  
			 /* Cell_id */
				
	}cell_info;  

    PACKED struct PACKED_POST
    {
    int32                       hs_ind;
      /**< High speed service indication (Over WCDMA) */
  
    int32                       dtm_supp;
      /**< Dual Transfer mode indication (Over GSM) */
  
    int32                       egprs_supp;
      /**< EGPRS indication  */
  
    int32                       hs_call_status;
      /**< Clients need to check for CM_API_CELL_SUPP_SERVICES
        ** and CM_API_CELL_SUPP_SERVICES_HS_CALL  before
          ** using this structure.
        */
  
    } cell_srv_ind;
	
	PACKED struct PACKED_POST
	{
	 int32					 reject_srv_domain;
	 /**<	Srv domain on which registration is rejected */
	
	 uint8					 reject_cause;
	 /**<  Registration Reject Cause */
	
	}reg_reject_info;
	
	int32	cipher_domain;
	  /* Can be used only if CM_CIPHERING_IND_SUPP is defined.
	  ** Ciphering on CS or PS domain or both.
	  ** sys_srv_domain_e_type
	  */
	
	int32 					 prev_idle_digital_mode;
		/* previous idle digital mode. FEATURE_HDR_HANDOFF.
		** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
		** sys_sys_mode_e_type
		*/
	  
	int32 					 cur_idle_digital_mode;
		/* current idle digital mode. FEATURE_HDR_HANDOFF.
		** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
		** sys_sys_mode_e_type
		*/
		
	uint8 								bcmcs_srv_supported ;
	/*CM_SS_EVT_BCMCS_SRV_SUPPORTED_MASK		changed_fields*/
			
    int32                               bcmcs_srv_status ;

	
    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        word                             lac ;
		
		uint8							 rac_or_mme_code ;

		PACKED struct PACKED_POST
        {
         uint32                         csg_id ;

		 uint8                          hybrid_cell ;
        } csg_info ;
		
      } gsm_mode_info ;
  	  
      PACKED struct PACKED_POST
      {
       
	   uint16						   cdma_reg_prd ;
	   /*NO MASK*/
	   
	   boolean						   is_colloc ;
	   /*NO MASK*/
	   
		 uint16 					   frame_err_rate ;
	   /*CM_SS_EVT_FER_MASK 				 changed_fields */
	   
      } cdma_mode_info ;
  
   	 PACKED struct PACKED_POST
   	 {
   	   int32 					       hdr_active_prot ;

	   uint8                           subnet_mask_len ;

	   int32 					       hdr_ln_status  ;
	   
	   uint16						   hdr_packet_err_rate ;
	   
	   int32 					       hdr_personality  ;
	   
	   uint8                           is_colloc ;
	   
	   uint8                           hdr_hybrid ;
	   
   	 } hdr_mode_info ;
   
   	 PACKED struct PACKED_POST
   	 {
   	   word                           lac ;
	   
	   uint8						   rac_or_mme_code ;
	   
	   PACKED struct PACKED_POST
	   {
		uint32						   csg_id ;
	   
		uint8						   hybrid_cell ;
	   } csg_info ;
   	 } wcdma_mode_info ;
   
   	 PACKED struct PACKED_POST
   	 {
		 int16							   rsrp ;
		 int8							   rsrq ;
		 word                              lac ;
		 word                              tac ;		 
		 uint8							   rac_or_mme_code ;
		 
		 PACKED struct PACKED_POST
		 {
		  uint8						 embms_coverage_status ;
		 
		  int16 						 trace_id ;
		 } embms_info ;

		 
		 PACKED struct PACKED_POST
		 {
		  uint32						 csg_id ;
		 
		  uint8 						 hybrid_cell ;
		 } csg_info ;

		 uint8                           voice_support_on_lte ;
   	 } lte_mode_info ;
   
   	 PACKED struct PACKED_POST
   	 {
   	    word                           lac ;
		
		uint8							rac_or_mme_code ;
		
		PACKED struct PACKED_POST
		{
		 uint32 						csg_id ;
		
		 uint8							hybrid_cell ;
		} csg_info ;
   	 } tds_mode_info ;
    }rat_mode_info ;
	
  } stack_info[2];

  
	uint64								changed_fields_subs;
	
	uint8 								is_csfb_call_active ;
	
	dword								curr_uptime ;
	
	uint8 								bsr_in_progress ;

	int32                               eutra_detect_status ;
	
	int                                 requesting_client_id ;
	
	int32                               msm_id ;

   PACKED struct PACKED_POST
   {
    int32                     reg_status;
    /**< IP app registration status
    */

    int                       cause;
    /**< Cause given when app registration fails or deregisters
    */

    PACKED struct PACKED_POST
    {
     boolean                  present;
    /**< TRUE   - Warning is present
    ** FALSE  - Warning is not present
    */

    int                       cause;
    /**< Warning code - SIP RFC 3261, sect 20.43
    */

    char                      hostname[20];
    /**< Null terminated host name that
    ** generates the warning
    */

    char                      description[20];
    /**< Null terminated Warning text.
    */
    
    }warn_info;
    /**< Warning information given by ipapp
    */

  uint64                      call_type_mask[CMIPAPP_CAP_ARR_MAX];
    /**< Array of Call tpyes that ipapp can service
    */

  int32                       sys_mode[CMIPAPP_CAP_ARR_MAX];
    /**< System modes app can support for calls
    ** Can be used only if CM_API_SMS_OVER_IP is defined.
    ** corresponding to the call_type (one to one)
    */

   }ipapp_info ;

   uint16                    current_mcc ;
   
   uint16                    home_mcc ;

   int32                     plus_dial_setting ;

   PACKED struct PACKED_POST
   {
    int32                    gan_state;
   /* Indicate GAN state .
   ** Mask used is CM_SS_GAN_STATE_MASK.
   ** sys_gw_gan_state_e_type
   */
    int32                    gan_conn_status;
   /* Indicate GAN connection status .
   ** Mask used is CM_SS_GAN_CONN_STATUS_MASK.
   ** sys_gw_gan_conn_status_e_type
   */
    int32                    gan_sig_chan_status;
   /* Indicate GAN signal channel status .
   ** Mask used is CM_SS_GAN_SIG_CHAN_STATUS_MASK.
   ** sys_gw_gan_sig_chan_status_e_type
   */
    PACKED struct PACKED_POST
    {
       boolean               is_mode_display_on;
 
       uint8                 gan_usz_icon_indicator;

       uint8                 gan_ms_location_status;

       uint8                 usz_str[34];

     }gan_mode_display;
   /* Indicate GAN mode display - whether GERAN or WLAN .
   ** Mask used is CM_SS_GAN_MODE_DISPLAY_MASK .
   ** sys_gw_gan_mode_display_s_type
   */        
    PACKED struct PACKED_POST
    {
        uint8                 gan_error_category;

        uint8                 gan_error_codepoint;

     }gan_error;
   /* Indicate GAN errors
   ** Mask used is CM_SS_GAN_ERROR_PROMPT_MASK  .
   ** sys_gw_gan_error_s_type
   */ 
    }gan_info;

   
   int32                     target_rat ;

   int32 				     irat_ctxt_target_rat ;
   
   int32 				     gwl_irat_asubs_id ;

   int32 				     voice_domain ;
   
   int32 				     sms_domain ;
   
   int32 				     div_duplex ;
   
   int32 				     def_roam_ind ;
   
   uint8                     ps_data_suspend ;
   
   uint16					 sys_sel_pref_req_id ;


   LOG_RECORD_END
/*
** Data structure used to log CM SS events
** We log current SS event along with what's in cm_ss_info_s_type
*/
LOG_RECORD_DEFINE(LOG_CM_SS_EVENT_C)

  uint8                      version;
    /* Version of the packet (CM_LOG_SS_EVENT_LOG_PACKET_VERSION)
    */ 

  int32                      ss_event;
    /* SS event, as defined by cm_ss_event_e_type
    */

  uint64                     changed_fields;
    /* Bit mask indicating which of the following serving system fields
    ** were changed since last notification.  Refer to the #defines
    ** (CM_SS_*_MASK) above. */

  int32                      srv_status;
    /* The service status (SERVICE/NO SERVICE) of the system.
    ** Mask used is CM_SS_SRV_STATUS_MASK.
    */

  int32                      srv_domain;
    /* The service domain registered on the system. /
    ** Mask used is CM_SS_SRV_DOMAIN_MASK.
    */

  int32                      srv_capability;
    /* The system's service capability.
    ** Mask used is CM_SS_SRV_CAPABILITY_MASK.
    */

  int32                      sys_mode;
    /* The system's mode.
    ** Mask used is CM_SS_SYS_MODE_MASK.
    */

  int32                      roam_status;
    /* Indicate current roaming status.
    ** Mask used is CM_SS_ROAM_STATUS_MASK.
    */

  PACKED struct PACKED_POST
  {
    int32                    id_type;

    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        uint16               sid;
          /* The system ID 
          */

        uint16               nid;
          /* The network ID 
          */

      } is95;
        /* IS 95 sys id
        */

      byte                   is856[16];
        /* HDR sys id
        */

      PACKED struct PACKED_POST
      {
        byte                 identity[3];

      } plmn;
        /* UMTS (WCDMA/GSM) system ID.
        */

      PACKED struct PACKED_POST
      {
        uint8                len;
          /* Length of the SSID, if == 0, then SSID = broadcast SSID.
          */
        char                 ssid[32];
          /* SSID of the wlan system.
          */

          /* 
          ** ssid 
          ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.          
          */

      } ssid;
        /* WLAN system Id.
        */

      PACKED struct PACKED_POST
      {
        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        word                 lac;
          /* LAC
          */
      } plmn_lac;
        /* UMTS (WCDMA/GSM) PLMN and LAC ID  
        */
    }id;

  } sys_id;

    /*
    ** Type for system ID.
    */

  uint8                    is_sys_forbidden;
    /* Indicates whether the System is forbidden.
    ** Mask used is CM_SS_SYS_FORBIDDEN_MASK.
    */

  uint8                    is_colloc;
    /* Indicates whether the system is collocated.
    ** Mask used is CM_SS_SYS_ID_MASK.
    */

  uint16                     rssi;
    /* RSSI in positive dBm i.e. 75 means -75dBm. The range for
    ** AMPS:      -110 to -89 dBm
    ** 800 CDMA:  -105 to -90 dBm
    ** 1900 CDMA: -108 to -93 dBm
    ** GSM/WCDMA: -105 to -60 dBm
    */

  int16                      ecio;
  int                        io;
  byte                       sinr;
    /* ECIO in negative 0.5 dBm i.e. 63 means -31.5dBm.
    ** Io field is valid only when in HDR mode only operation
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** SINR applicable only when sys_mode = HDR.
    */

  uint8                    ps_data_suspend;
    /* Indicates that the current PS data service has been suspended.
    ** Mask used is CM_SS_PS_DATA_SUSPEND_MASK.
    */

  uint8                    hdr_hybrid;
    /* system is hybrid or not. FEATURE_HDR_HYBRID */

  int32                      hdr_srv_status;
     /* Indicate current service state - FEATURE_HDR_HYBRID.
     ** Mask used is CM_SS_HDR_SRV_STATUS_MASK.
     ** sys_srv_status_e_type   
     */

  int32                      hdr_roam_status;
     /* Indicate current hybrid hdr roaming status - FEATURE_HDR_HYBRID.
     ** Mask used is CM_SS_HDR_ROAM_STATUS_MASK.
     ** sys_roam_status_e_type 
     */

  uint16                     hdr_rssi;
    /* HDR RSSI in positive dBm i.e. 75 means -75dBm - FEATURE_HDR_HYBRID
    ** The range is -105 to -90 dBm
    */

  int16                      hdr_ecio;
  int                        hdr_io;
  byte                       hdr_sinr;
    /*
    ** HDR Ec/Io and Io values (used when mobile is in
    ** hybrid operation).
    ** Ec/Io range: from 5 ( -31.5 dB ) to 512 ( 0 dB )
    ** Io    range: from -106 dBm to -21 dBm
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** ( FEATURE_HDR )
    */

  int32                      prev_idle_digital_mode;
    /* previous idle digital mode. FEATURE_HDR_HANDOFF.
    ** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
    ** sys_sys_mode_e_type
    */

  int32                      cur_idle_digital_mode;
    /* current idle digital mode. FEATURE_HDR_HANDOFF.
    ** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
    ** sys_sys_mode_e_type
    */

  PACKED union PACKED_POST
  {

    PACKED struct PACKED_POST
    {
      uint8                  bit_err_rate;
        /* Bit Error Rate / RX QUAL in GSM 
        */

      int32                  sim_state;
        /* Current state of the SIM.
        ** Mask used is CM_SS_SIM_STATE_MASK.
        ** sys_sim_state_e_type
        */

      uint8                plmn_forbidden;
        /* Indicates whether the PLMN is forbidden.
        ** Mask used is CM_SS_PLMN_FORBIDDEN_MASK.
        */

      PACKED struct PACKED_POST
      {
        /*
        ** Flags that indicate whether the particular mm information is
        ** available.
        **/
        uint8              plmn_avail;
        uint8              full_name_avail;
        uint8              short_name_avail;
        uint8              univ_time_and_time_zone_avail;
        uint8              time_zone_avail;
        uint8              lsa_identity_avail;
        uint8              daylight_saving_adj_avail;

        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        PACKED struct PACKED_POST
        {
          uint8              length;
            /* Number of characters in the full name 
            */

          int32              coding_scheme;
            /* The coding scheme used to encode the full name of the network 
            ** sys_network_name_coding_scheme_type_e_type   
            */

          uint8            add_country_initials;
            /* Flag that indicates whether MS should add the country initials 
            ** to the full name of the network 
            */

          uint8              name[255];
            /* The full name of the network
            */

            /* 
            ** name 
            ** Hardcoded to the value of SYS_MAX_FULL_NAME_CHARS
            ** at the time log packet was declared. 
            ** This is intentional as otherwise changes to the constant would
            ** change byte ordering in the log packet.          
            */

        } full_name;

        PACKED struct PACKED_POST
        {

          uint8              length;
            /* Number of characters in the full name 
            */

          int32              coding_scheme;
            /* The coding scheme used to encode the full name of the network 
            ** sys_network_name_coding_scheme_type_e_type   
            */

          uint8            add_country_initials;
            /* Flag that indicates whether MS should add the country initials 
            ** to the full name of the network 
            */

          uint8              name[255];
            /* The full name of the network
            */

            /* 
            ** name 
            ** Hardcoded to the value of SYS_MAX_SHORT_NAME_CHARS
            ** at the time log packet was declared. 
            ** This is intentional as otherwise changes to the constant would
            ** change byte ordering in the log packet.          
            */

        } short_name;

        PACKED struct PACKED_POST
        {

           uint8             year;
           uint8             month;
           uint8             day;
           uint8             hour;
           uint8             minute;
           uint8             second;
           int               time_zone;
             /*  sys_time_zone_type
             */

        } univ_time_and_time_zone;

        int                  time_zone;
          /* sys_time_zone_type    
          */
        PACKED struct PACKED_POST
        {
          int32              type;
          /*  sys_lsa_identity_type_e_type
          */

          uint8              identity[3];

        } lsa_identity;

        int32                daylight_saving_adj;
          /* sys_daylight_saving_adj_e_type            
          */

      } mm_information;

        /* Mobility management information, i.e. network name, date,
        ** time and time zone of registered network */

    } gw_info;

        /* GSM/WCDMA SS information 
        */

    PACKED struct PACKED_POST
    {


      int32                  band_class;
        /* cellular or PCS 
        ** sys_band_class_e_type       
        */

      int32                  block_or_system;
        /* PCS frequency block: A to F Cellular System A or B
        ** sys_blksys_e_type  
        */

      uint16                 reg_zone;
        /* 12 bit registration zone 
        */

      uint16                 packet_zone;
        /* 8  bit packet zone, 0xffff means PACKET ZONE NONE.
        ** Mask used is CM_SS_PACKET_ZONE_MASK.
        */

      uint8                  bs_p_rev;
        /* 8  bit base station p_rev.
        ** Mask used is CM_SS_BS_P_REV_MASK.
        */

      uint8                  p_rev_in_use;
        /* 8  bit p_rev in use the lessor of BS p_rev and mobile p_rev.
        ** Mask used is CM_SS_P_REV_IN_USE_MASK.
        */

      uint8                  is_registered;
        /* is mobile registered on the current system */

      uint8                  ccs_supported;
        /* is concurrent services supported.
        */

      int32                  uz_id;
         /* User zone Identification of the selected user zone,
         ** == CM_INVALID_UZID, if no user zone selected. FEATURE_USERZONE.
         ** Mask used is CM_SS_UZ_CHANGED_MASK.
         */

      PACKED struct PACKED_POST
      {
        byte                 uz_name_encoding;

        byte                 uz_name_n_octets;

        byte                 uz_name[32];


         /* 
         ** uz_name 
         ** Hardcoded to the value of SYS_PUZL_MAX_UZ_NAME_LEN
         ** at the time log packet was declared. 
         ** This is intentional as otherwise changes to the constant would
         ** change byte ordering in the log packet.          
         */

      }uz_name;
        /* User zone name of the selected user zone.
        ** Valid only if uz_id != CM_INVALID_UZID. FEATURE_USERZONE.
        ** Mask used is CM_SS_UZ_CHANGED_MASK.
        */

      byte                   srch_win_n;
        /* Neighbour set window search size.
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        */

      int32                  base_lat;
        /* Base station latitude[0.25sec and already sign extended].
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_lat_type  
        */

      int32                  base_long;
        /* Base station latitude[0.25sec and already sign extended].
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_long_type
        */

      word                   base_id;
        /* Base station Identification number.
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_id_type
        */

      PACKED struct
      {
        int32                  sys_mode;
        /* Indicates the system from which the time was obtained (sys_sys_mode_e_type) 
        */
        PACKED union
        {
          PACKED struct
          {
            uint8                lp_sec;
            int8                 daylt_savings;
            uint8                ltm_offset;
          }cdma_time;

          PACKED struct
          {
            uint8                lp_sec;
            int16                ltm_offset;
          }hdr_time;

        }time;

      }time_info;
      /* Time obtained from sync channel / sector parameter msg
      ** Mask used is CM_SS_CDMA_TIME_CHGD_MASK.
      */

    } cdma_info;
        /* CDMA SS information 
        */
  }mode_info;

  /* Mode specific serving system information */

 
  /* FEATURE_BCMCS
  ** These fields specify whether lower layer
  ** supports broadcast/multicast as per
  ** High Rate Broadcast-Multicast Packet data Air
  ** Interface Specification C.S0054 and
  ** BCMCS Framework Draft Document 0.1.3 XP0019
  */

  uint8                    hybr_hdr_bcmcs_srv_supported;
    /* Specify whether HDR is configured to support BCMCS service.
    ** Mask used is CM_SS_HYBR_HDR_BCMCS_SRV_SUPPORTED_MASK.
    */

  uint8                    ss_main_bcmcs_srv_supported;
    /* Specify whether MAIN is configured to support BCMCS service.
    ** Mask used is CM_SS_MAIN_BCMCS_SRV_SUPPORTED_MASK.
    */

  int                        requesting_client_id;
    /* Requesting client */

  int32                      msm_id;
     /* Identifies the MSM which generated this event.
     ** sys_msm_id_e_type       
     */

  /* Clients check CM_API_WLAN for WLAN support for following
  **
  ** wlan_rssi, wlan_srv_status, wlan_bss_info, wlan_stats
  */
  uint16                     wlan_rssi;

  int32                      wlan_srv_status;
     /* Indicate current WLAN service state
     ** Mask used is CM_SS_WLAN_SRV_STATUS_MASK.
     ** sys_srv_status_e_type  
     */

  PACKED struct PACKED_POST
  {

    word                     chan;
      /* Channel on which this BSS provides service.
      ** sys_chan_type 
      */

    int32                    band;
      /* Band in which this BSS provides service.
      ** sys_band_class_e_type
      */

    int32                    bss_type;
      /* Type of BSS - Infrastructure or independent.
      ** sys_wlan_bss_e_type
      */

    uint64                   bssid;
      /*  BSS Id.
      **  sys_wlan_bssid_type      
      */

    PACKED struct PACKED_POST
    {
      uint8                  len;
        /* Length of the SSID, if == 0, then SSID = broadcast SSID.
        */

      char                   ssid[32];
        /* SSID of the wlan system.
        */

        /* 
        ** ssid
        ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
        ** at the time log packet was declared. 
        ** This is intentional as otherwise changes to the constant would
        ** change byte ordering in the log packet.          
        */
    } ssid;
    /* SS Id.
    */

    uint16                   rssi;
      /* Measured RSSI of the BSS.
      */

    int32                    tech;
      /* WLAN technology in use.
      ** sys_tech_e_type
      */
  } wlan_bss_info;

    /* 802.11 (WLAN) System Information
    */

  PACKED struct PACKED_POST
  {
    int32                    current_xmit_rate;
      /* Xmit rate of the last packet successfully transmitted 
      ** sys_wlan_rate_e_type
      */

    uint32                   total_tx_bytes;
      /* Number of bytes transmitted over the WLAN interface
      */

    uint32                   total_rx_bytes;
      /* Number of bytes received over the WLAN interface     
      */

  } wlan_stats;
 
    /* WLAN stats such as Current Transmit Rate, TX/RX bytes.
    */

  int32   cipher_domain;
    /* Can be used only if CM_CIPHERING_IND_SUPP is defined.
    ** Ciphering on CS or PS domain or both.
    ** sys_srv_domain_e_type
    */

  PACKED struct PACKED_POST
  {

    uint8                  ss_thr_status;
      /* The throttle status of the current ss
      */

    dword                    thr_uptime;
      /* The throttling time period
      */

    unsigned int             idx;
      /* The index in the throttle_time table
      ** Used internally by CM to keep track of the next throttle time 
      ** increment
      */

    int32                    call_status;
      /* The call end reason
      ** cm_ss_orig_thr_reasons_e_type
      */

  } main_thr_status;

    /* The DCTM status, indicates the current throttle status of the MAIN 
    ** system
    */

  PACKED struct PACKED_POST
  {
    uint8                  ss_thr_status;
      /* The throttle status of the current ss
      */

    dword                    thr_uptime;
      /* The throttling time period
      */

    unsigned int             idx;
      /* The index in the throttle_time table
      ** Used internally by CM to keep track of the next throttle time 
      ** increment
      */

    int32                    call_status;
      /* The call end reason
      ** cm_ss_orig_thr_reasons_e_type
      */

  }  hdr_thr_status;

    /* The DCTM status, indicates the current throttle status of the HDR 
    ** system
    */

  PACKED struct PACKED_POST
  {
    int32                    id_type;

    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        uint16               sid;
          /* The system ID 
          */

        uint16               nid;
          /* The network ID 
          */

      } is95;
        /* IS 95 sys id
        */

      byte                   is856[16];
        /* HDR sys id
        */

      PACKED struct PACKED_POST
      {
        byte                 identity[3];

      } plmn;
        /* UMTS (WCDMA/GSM) system ID.
        */

      PACKED struct PACKED_POST
      {
        uint8                len;
          /* Length of the SSID, if == 0, then SSID = broadcast SSID.
          */
        char                 ssid[32];
          /* SSID of the wlan system.
          */

          /* 
          ** ssid 
          ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.          
          */

      } ssid;
        /* WLAN system Id.
        */

      PACKED struct PACKED_POST
      {
        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        word                 lac;
          /* LAC
          */
      } plmn_lac;
        /* UMTS (WCDMA/GSM) PLMN and LAC ID  
        */
    }id;
  } hdr_sys_id;

    /* The ID of the HDR sub system.
    ** Mask used is CM_SS_HDR_SYS_ID_MASK.
    */

  dword                     curr_uptime;
    /* Current uptime
    */

  int32                     psist;
   /* Indicate psist of the system.
   ** Mask used is CM_SS_PSIST_CHGD_MASK.
   ** cm_psist_e_type   
   */

  int32                     hdr_active_prot;
   /* Indicate HDR active protocol revision value.
   ** Mask used is CM_SS_HDR_ACTIVE_PROT_MASK.
   ** sys_active_prot_e_type
   */

   /* Below fields added for version = 5 and upwards */

  int32                     true_srv_status;
    /* The true service status (LIMITED/LIMITED_REGIONAL) of the system.
    ** Mask used is CM_SS_TRUE_SRV_STATUS_MASK.
    */

  uint64                    signal_strength_changed_fields;
    /**< Bit mask indicating which of the following signal strength fields
     ** were changed since last notification. Refer to the #defines
     ** (CM_SS_*_MASK) above. */

   int16                    pathloss;
    /**< Pathloss is applicable only in Connected mode (otherwise 0xFF)
    */

   int16                    sir;
    /**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
    ** connected mode when DPCCH is active.
    */

  int16                     rssi2;
    /**< The -ve RSSI value, coming from RRC layer
    */

  int16                     rscp;
    /**< The range of RSCP would be from -28 to -121 dBm
    */

  boolean                   ecio_for_display;
    /**< TRUE if UE is in Data call
    */

  int16                     gw_ecio;
    /**< ECIO for WCDMA only
    ** Ec/Io is measured in -0.5 dB.
    ** What WCDMA L1 sends to RRC is in 0.5dB (positive value)
    ** For GSM it is defaulted to CMSS_ECIO_VALUE_INVALID_SIGNAL
    ** Clients should check for CM_API_GW_ECIO to use this field
    */

   PACKED struct PACKED_POST
   {
     uint32                 cell_id;  
       /* Cell_id */
     
   }cell_info;  
  
   PACKED struct PACKED_POST
   {
    int32                   reject_srv_domain;
    /**<   Srv domain on which registration is rejected */

    uint8                   reject_cause;
    /**<  Registration Reject Cause */
  
   }reg_reject_info;

  PACKED struct PACKED_POST
  {
    int32                    gan_state;
   /* Indicate GAN state .
   ** Mask used is CM_SS_GAN_STATE_MASK.
   ** sys_gw_gan_state_e_type
   */
    int32                    gan_conn_status;
   /* Indicate GAN connection status .
   ** Mask used is CM_SS_GAN_CONN_STATUS_MASK.
   ** sys_gw_gan_conn_status_e_type
   */
    int32                    gan_sig_chan_status;
   /* Indicate GAN signal channel status .
   ** Mask used is CM_SS_GAN_SIG_CHAN_STATUS_MASK.
   ** sys_gw_gan_sig_chan_status_e_type
   */
    PACKED struct PACKED_POST
    {
       boolean               is_mode_display_on;
 
       uint8                 gan_usz_icon_indicator;

       uint8                 gan_ms_location_status;

       uint8                 usz_str[34];

     }gan_mode_display;
   /* Indicate GAN mode display - whether GERAN or WLAN .
   ** Mask used is CM_SS_GAN_MODE_DISPLAY_MASK .
   ** sys_gw_gan_mode_display_s_type
   */        
    PACKED struct PACKED_POST
    {
        uint8                 gan_error_category;

        uint8                 gan_error_codepoint;

    }gan_error;
   /* Indicate GAN errors
   ** Mask used is CM_SS_GAN_ERROR_PROMPT_MASK  .
   ** sys_gw_gan_error_s_type
   */ 
   }gan_info;
 
   /* Below fields added for version = 6 and upwards */
  
  PACKED struct PACKED_POST
  {
  int32                       hs_ind;
    /**< High speed service indication (Over WCDMA) */

  int32                       dtm_supp;
    /**< Dual Transfer mode indication (Over GSM) */

  int32                       egprs_supp;
    /**< EGPRS indication  */

  int32                       hs_call_status;
    /**< Clients need to check for CM_API_CELL_SUPP_SERVICES
  ** and CM_API_CELL_SUPP_SERVICES_HS_CALL  before
    ** using this structure.
  */

  } cell_srv_ind;



   int32                      hybr_hdr_bcmcs_srv_status;
    /**< Specify whether BCMCS service is available on HDR.
    ** Mask used is CM_SS_HYBR_HDR_BCMCS_SRV_STATUS_MASK.
    */

    int32                     ss_main_bcmcs_srv_status;
    /**< Specify whether BCMCS service is available on MAIN.
    ** Mask used is CM_SS_MAIN_BCMCS_SRV_STATUS_MASK.
    */
  

   PACKED struct PACKED_POST
   {
    int32                     reg_status;
    /**< IP app registration status
    */

    int                       cause;
    /**< Cause given when app registration fails or deregisters
    */

    PACKED struct PACKED_POST
    {
     boolean                  present;
    /**< TRUE   - Warning is present
    ** FALSE  - Warning is not present
    */

    int                       cause;
    /**< Warning code - SIP RFC 3261, sect 20.43
    */

    char                      hostname[20];
    /**< Null terminated host name that
    ** generates the warning
    */

    char                      description[20];
    /**< Null terminated Warning text.
    */
    
    }warn_info;
    /**< Warning information given by ipapp
    */

  uint64                      call_type_mask[CMIPAPP_CAP_ARR_MAX];
    /**< Array of call types that ipapp can service
    */

  int32                       sys_mode[CMIPAPP_CAP_ARR_MAX];
    /**< System modes app can support for corresponding call_type
    ** Can be used only if CM_API_SMS_OVER_IP is defined.
    */

   }ipapp_info;


  int32                       active_band;
    /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
    ** using this structure.
    **
    */

  uint16                      active_channel;
    /**< Report active channel of the current serving system.
    */

  int32                       hybrid_active_band;
    /**< Report active band of hybrid system 
    */

  uint16                      hybrid_active_channel;
    /**< Report active channel of the current serving system.
    */

  int32                       hdr_ln_status;
    /**< HDR LN status
    */

   PACKED struct PACKED_POST
   {
   boolean                    present; 
    /* present flag if this structure
    ** is populated
    */

   uint16                     sid;
    /* Sid that is in conflict with
    ** countries
    */

   uint8                      country_code_len;
    /* Length of country_code_list[]
    */

   uint16                     country_code_list [5];
    /* Array of mobile country codes that conflict
    ** with given sid
    */
   
   }country_list;


  uint16                      current_mcc;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries country code of current serving country.
    */


  uint16                      home_mcc;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries home country code.
    */


  int32                       plus_dial_setting;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries NV setting corresp to plus dial
    */

  int32                       def_roam_ind;
   /** Default roaming indicator from PRL
   */


  /* Below fields added for version = 7 and upwards */

  byte                        asubs_id;
    /**< Subscription ID corresponding to the Main GW service 
    ** Mask used is CM_SS_SUBS_ID_MASK 
    ** This is only valid when the sys_mode is GSM/WCDMA 
    */

  boolean                    is_main_operational;
    /**< Operating status for HYBR_GW subscription. Decided by selected dual 
    **  standby preference
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
  **                     HYBRID GW SS INFO
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint64                     hybr_gw_changed_fields;
    /* Bit mask indicating which of the following serving system fields
    ** were changed since last notification.  Refer to the #defines
    ** (CM_SS_*_MASK) above. */

  boolean                    is_hybr_gw_operational;
    /**< Operating status for HYBR_GW subscription. Decided by selected dual 
        standby preference */

  byte                       gw_asubs_id;
    /**< Subscription ID corresponding to the Hybrid GW service 
    ** Mask used is CM_SS_GW_SUBS_ID_MASK 
    ** This is only valid when the sys_mode is GSM/WCDMA 
    */

  int32                     gw_active_band;
    /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
    ** using this structure.
    **
    */

  int16                     gw_active_channel;
    /**< Report active channel of the current serving system.
    */

  int32                     gw_srv_status;
    /**< The service status (SERVICE/NO SERVICE) of the system.
    ** Mask used is CM_SS_GW_SRV_STATUS_MASK.
    */

  int32                     true_gw_srv_status;
    /**< The true service status (LIMITED/LIMITED_REGIONAL) of the system.
    ** Mask used is CM_SS_TRUE_GW_SRV_STATUS_MASK.
    */

  int32                     gw_srv_domain;
    /**< The service domain registered on the system. /
    ** Mask used is CM_SS_SRV_DOMAIN_MASK.
    */

  int32                     gw_srv_capability;
    /**< The system's service capability.
    ** Mask used is CM_SS_SRV_CAPABILITY_MASK.
    */

  int32                     gw_sys_mode;
    /**< The system's mode.
    ** Mask used is CM_SS_SYS_MODE_MASK.
    */

  int32                     gw_roam_status;

		
		/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 	
		**										 HYBRID3 GW SS INFO
		**- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		
		uint64										 hybr_gw3_changed_fields;
			/* Bit mask indicating which of the following serving system fields
			** were changed since last notification.	Refer to the #defines
			** (CM_SS_*_MASK) above. */
		
		boolean 									 is_hybr_gw3_operational;
			/**< Operating status for HYBR_GW subscription. Decided by selected dual 
					standby preference */
		
		byte											 gw3_asubs_id;
			/**< Subscription ID corresponding to the Hybrid GW service 
			** Mask used is CM_SS_GW_SUBS_ID_MASK 
			** This is only valid when the sys_mode is GSM/WCDMA 
			*/
		
		int32 										gw3_active_band;
			/**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
			** using this structure.
			**
			*/
		
		int16 										gw3_active_channel;
			/**< Report active channel of the current serving system.
			*/
		
		int32 										gw3_srv_status;
			/**< The service status (SERVICE/NO SERVICE) of the system.
			** Mask used is CM_SS_GW_SRV_STATUS_MASK.
			*/
		
		int32 										true_gw3_srv_status;
			/**< The true service status (LIMITED/LIMITED_REGIONAL) of the system.
			** Mask used is CM_SS_TRUE_GW_SRV_STATUS_MASK.
			*/
		
		int32 										gw3_srv_domain;
			/**< The service domain registered on the system. /
			** Mask used is CM_SS_SRV_DOMAIN_MASK.
			*/
		
		int32 										gw3_srv_capability;
			/**< The system's service capability.
			** Mask used is CM_SS_SRV_CAPABILITY_MASK.
			*/
		
		int32 										gw3_sys_mode;
			/**< The system's mode.
			** Mask used is CM_SS_SYS_MODE_MASK.
			*/
		
		int32 										gw3_roam_status;

    /**< Indicate current roaming status.
    ** Mask used is CM_SS_ROAM_STATUS_MASK.
    */

  PACKED struct PACKED_POST
  {
    int32                    id_type;

    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        uint16               sid;
          /* The system ID 
          */

        uint16               nid;
          /* The network ID 
          */

      } is95;
        /* IS 95 sys id
        */

      byte                   is856[16];
        /* HDR sys id
        */

      PACKED struct PACKED_POST
      {
        byte                 identity[3];

      } plmn;
        /* UMTS (WCDMA/GSM) system ID.
        */

      PACKED struct PACKED_POST
      {
        uint8                len;
          /* Length of the SSID, if == 0, then SSID = broadcast SSID.
          */
        char                 ssid[32];
          /* SSID of the wlan system.
          */

          /* 
          ** ssid 
          ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.          
          */

      } ssid;
        /* WLAN system Id.
        */

      PACKED struct PACKED_POST
      {
        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        word                 lac;
          /* LAC
          */
      } plmn_lac;
        /* UMTS (WCDMA/GSM) PLMN and LAC ID  
        */
    }id;

			} gw3_sys_id;
				/**< The ID of the selected system.
				** Mask used is CM_SS_SYS_ID_MASK.
				*/

  PACKED struct PACKED_POST
  {
    int32                    id_type;

    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        uint16               sid;
          /* The system ID 
          */

        uint16               nid;
          /* The network ID 
          */

      } is95;
        /* IS 95 sys id
        */

      byte                   is856[16];
        /* HDR sys id
        */

      PACKED struct PACKED_POST
      {
        byte                 identity[3];

      } plmn;
        /* UMTS (WCDMA/GSM) system ID.
        */

      PACKED struct PACKED_POST
      {
        uint8                len;
          /* Length of the SSID, if == 0, then SSID = broadcast SSID.
          */
        char                 ssid[32];
          /* SSID of the wlan system.
          */

          /* 
          ** ssid 
          ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.          
          */

      } ssid;
        /* WLAN system Id.
        */

      PACKED struct PACKED_POST
      {
        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        word                 lac;
          /* LAC
          */
      } plmn_lac;
        /* UMTS (WCDMA/GSM) PLMN and LAC ID  
        */
    }id;

  } gw_sys_id;
    /**< The ID of the selected system.
    ** Mask used is CM_SS_SYS_ID_MASK.
    */

  boolean                    gw_is_sys_prl_match;
    /**< Indicates whether the System is in PRL
    ** Mask used is CM_SS_SYS_PRL_MATCH_MASK.
    */

  boolean                    gw_is_sys_forbidden;
    /**< Indicates whether the System is forbidden.
    ** Mask used is CM_SS_SYS_FORBIDDEN_MASK.
    */

  uint16                     gw_rssi;
    /**< RSSI in positive dBm i.e. 75 means -75dBm. The range for
    ** AMPS:      -110 to -89 dBm
    ** 800 CDMA:  -105 to -90 dBm
    ** 1900 CDMA: -108 to -93 dBm
    ** GSM/WCDMA: -105 to -60 dBm
    */

  int                        gw_io;
  byte                       gw_sinr;
    /**< ECIO in negative 0.5 dBm i.e. 63 means -31.5dBm.
    ** Io field is valid only when in HDR mode only operation
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** SINR applicable only when sys_mode = HDR.
    */

  int16                      gw_pathloss;
    /**< Pathloss is applicable only in Connected mode (otherwise 0xFF)
    */

  int16                      gw_sir;
    /**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
    ** connected mode when DPCCH is active.
    */

  int16                      gw_rssi2;
    /**< The -ve RSSI value, coming from RRC layer
    */

  int16                      gw_rscp;
    /**< Signal strength in RSSI, change for WCDMA only  (otherwise 0xFF)
    */

  boolean                    gw_ecio_for_display;
    /**< TRUE if UE is in a data call, else FALSE
    */

	boolean 									 gw3_is_sys_prl_match;
			/**< Indicates whether the System is in PRL
			** Mask used is CM_SS_SYS_PRL_MATCH_MASK.
			*/
		
  boolean 									 gw3_is_sys_forbidden;
			/**< Indicates whether the System is forbidden.
			** Mask used is CM_SS_SYS_FORBIDDEN_MASK.
			*/
		
  uint16										 gw3_rssi;
			/**< RSSI in positive dBm i.e. 75 means -75dBm. The range for
			** AMPS:			-110 to -89 dBm
			** 800 CDMA:	-105 to -90 dBm
			** 1900 CDMA: -108 to -93 dBm
			** GSM/WCDMA: -105 to -60 dBm
			*/
		
		int 											 gw3_io;
		byte											 gw3_sinr;
			/**< ECIO in negative 0.5 dBm i.e. 63 means -31.5dBm.
			** Io field is valid only when in HDR mode only operation
			** SINR range: from 0 to 8, Level 8 represents highest SINR
			** SINR applicable only when sys_mode = HDR.
			*/
		
		int16 										 gw3_pathloss;
			/**< Pathloss is applicable only in Connected mode (otherwise 0xFF)
			*/
		
		int16 										 gw3_sir;
			/**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
			** connected mode when DPCCH is active.
			*/
		
		int16 										 gw3_rssi2;
			/**< The -ve RSSI value, coming from RRC layer
			*/
		
		int16 										 gw3_rscp;
			/**< Signal strength in RSSI, change for WCDMA only  (otherwise 0xFF)
			*/
		
		boolean 									 gw3_ecio_for_display;
			/**< TRUE if UE is in a data call, else FALSE
			*/

  PACKED struct PACKED_POST
  {
    uint32                 cell_id;  
      /* Cell_id */

  }gw3_cell_info;  
    /**< The CELL_INFO of the selected system.
    **  Mask used is CM_SS_CELL_INFO_MASK
    */

  PACKED struct PACKED_POST
  {
    uint32                 cell_id;  
      /* Cell_id */

  }gw_cell_info;  
    /**< The CELL_INFO of the selected system.
    **  Mask used is CM_SS_CELL_INFO_MASK
    */

  PACKED union PACKED_POST
  {

    PACKED struct PACKED_POST
    {
      uint8                  bit_err_rate;
        /* Bit Error Rate / RX QUAL in GSM 
        */

      int32                  sim_state;
        /* Current state of the SIM.
        ** Mask used is CM_SS_SIM_STATE_MASK.
        ** sys_sim_state_e_type
        */

      uint8                plmn_forbidden;
        /* Indicates whether the PLMN is forbidden.
        ** Mask used is CM_SS_PLMN_FORBIDDEN_MASK.
        */

      PACKED struct PACKED_POST
      {
        /*
        ** Flags that indicate whether the particular mm information is
        ** available.
        **/
        uint8              plmn_avail;
        uint8              full_name_avail;
        uint8              short_name_avail;
        uint8              univ_time_and_time_zone_avail;
        uint8              time_zone_avail;
        uint8              lsa_identity_avail;
        uint8              daylight_saving_adj_avail;

        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        PACKED struct PACKED_POST
        {
          uint8              length;
            /* Number of characters in the full name 
            */

          int32              coding_scheme;
            /* The coding scheme used to encode the full name of the network 
            ** sys_network_name_coding_scheme_type_e_type   
            */

          uint8            add_country_initials;
            /* Flag that indicates whether MS should add the country initials 
            ** to the full name of the network 
            */

          uint8              name[255];
            /* The full name of the network
            */

            /* 
            ** name 
            ** Hardcoded to the value of SYS_MAX_FULL_NAME_CHARS
            ** at the time log packet was declared. 
            ** This is intentional as otherwise changes to the constant would
            ** change byte ordering in the log packet.          
            */

        } full_name;

        PACKED struct PACKED_POST
        {

          uint8              length;
            /* Number of characters in the full name 
            */

          int32              coding_scheme;
            /* The coding scheme used to encode the full name of the network 
            ** sys_network_name_coding_scheme_type_e_type   
            */

          uint8            add_country_initials;
            /* Flag that indicates whether MS should add the country initials 
            ** to the full name of the network 
            */

          uint8              name[255];
            /* The full name of the network
            */

            /* 
            ** name 
            ** Hardcoded to the value of SYS_MAX_SHORT_NAME_CHARS
            ** at the time log packet was declared. 
            ** This is intentional as otherwise changes to the constant would
            ** change byte ordering in the log packet.          
            */

        } short_name;

        PACKED struct PACKED_POST
        {

           uint8             year;
           uint8             month;
           uint8             day;
           uint8             hour;
           uint8             minute;
           uint8             second;
           int               time_zone;
             /*  sys_time_zone_type
             */

        } univ_time_and_time_zone;

        int                  time_zone;
          /* sys_time_zone_type    
          */
        PACKED struct PACKED_POST
        {
          int32              type;
          /*  sys_lsa_identity_type_e_type
          */

          uint8              identity[3];

        } lsa_identity;

        int32                daylight_saving_adj;
          /* sys_daylight_saving_adj_e_type            
          */

      } mm_information;

        /* Mobility management information, i.e. network name, date,
        ** time and time zone of registered network */

    } gw_info;

        /* GSM/WCDMA SS information 
        */

    PACKED struct PACKED_POST
    {


      int32                  band_class;
        /* cellular or PCS 
        ** sys_band_class_e_type       
        */

      int32                  block_or_system;
        /* PCS frequency block: A to F Cellular System A or B
        ** sys_blksys_e_type  
        */

      uint16                 reg_zone;
        /* 12 bit registration zone 
        */

      uint16                 packet_zone;
        /* 8  bit packet zone, 0xffff means PACKET ZONE NONE.
        ** Mask used is CM_SS_PACKET_ZONE_MASK.
        */

      uint8                  bs_p_rev;
        /* 8  bit base station p_rev.
        ** Mask used is CM_SS_BS_P_REV_MASK.
        */

      uint8                  p_rev_in_use;
        /* 8  bit p_rev in use the lessor of BS p_rev and mobile p_rev.
        ** Mask used is CM_SS_P_REV_IN_USE_MASK.
        */

      uint8                  is_registered;
        /* is mobile registered on the current system */

      uint8                  ccs_supported;
        /* is concurrent services supported.
        */

      int32                  uz_id;
         /* User zone Identification of the selected user zone,
         ** == CM_INVALID_UZID, if no user zone selected. FEATURE_USERZONE.
         ** Mask used is CM_SS_UZ_CHANGED_MASK.
         */

      PACKED struct PACKED_POST
      {
        byte                 uz_name_encoding;

        byte                 uz_name_n_octets;

        byte                 uz_name[32];


         /* 
         ** uz_name 
         ** Hardcoded to the value of SYS_PUZL_MAX_UZ_NAME_LEN
         ** at the time log packet was declared. 
         ** This is intentional as otherwise changes to the constant would
         ** change byte ordering in the log packet.          
         */

      }uz_name;
        /* User zone name of the selected user zone.
        ** Valid only if uz_id != CM_INVALID_UZID. FEATURE_USERZONE.
        ** Mask used is CM_SS_UZ_CHANGED_MASK.
        */

      byte                   srch_win_n;
        /* Neighbour set window search size.
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        */

      int32                  base_lat;
        /* Base station latitude[0.25sec and already sign extended].
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_lat_type  
        */

      int32                  base_long;
        /* Base station latitude[0.25sec and already sign extended].
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_long_type
        */

      word                   base_id;
        /* Base station Identification number.
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_id_type
        */

      PACKED struct
      {
        int32                  sys_mode;
        /* Indicates the system from which the time was obtained (sys_sys_mode_e_type) 
        */
        PACKED union
        {
          PACKED struct
          {
            uint8                lp_sec;
            int8                 daylt_savings;
            uint8                ltm_offset;
          }cdma_time;

          PACKED struct
          {
            uint8                lp_sec;
            int16                ltm_offset;
          }hdr_time;

        }time;

      }time_info;
      /* Time obtained from sync channel / sector parameter msg
      ** Mask used is CM_SS_CDMA_TIME_CHGD_MASK.
      */

    } cdma_info;
        /* CDMA SS information 
        */
  }gw_mode_info;

	PACKED union PACKED_POST
	{
		
				PACKED struct PACKED_POST
				{
					uint8 								 bit_err_rate;
						/* Bit Error Rate / RX QUAL in GSM 
						*/
		
					int32 								 sim_state;
						/* Current state of the SIM.
						** Mask used is CM_SS_SIM_STATE_MASK.
						** sys_sim_state_e_type
						*/
		
					uint8 							 plmn_forbidden;
						/* Indicates whether the PLMN is forbidden.
						** Mask used is CM_SS_PLMN_FORBIDDEN_MASK.
						*/
		
					PACKED struct PACKED_POST
					{
						/*
						** Flags that indicate whether the particular mm information is
						** available.
						**/
						uint8 						 plmn_avail;
						uint8 						 full_name_avail;
						uint8 						 short_name_avail;
						uint8 						 univ_time_and_time_zone_avail;
						uint8 						 time_zone_avail;
						uint8 						 lsa_identity_avail;
						uint8 						 daylight_saving_adj_avail;
		
						PACKED struct PACKED_POST
						{
							byte							 identity[3];
						} plmn;
		
						PACKED struct PACKED_POST
						{
							uint8 						 length;
								/* Number of characters in the full name 
								*/
		
							int32 						 coding_scheme;
								/* The coding scheme used to encode the full name of the network 
								** sys_network_name_coding_scheme_type_e_type 	
								*/
		
							uint8 					 add_country_initials;
								/* Flag that indicates whether MS should add the country initials 
								** to the full name of the network 
								*/
		
							uint8 						 name[255];
								/* The full name of the network
								*/
		
								/* 
								** name 
								** Hardcoded to the value of SYS_MAX_FULL_NAME_CHARS
								** at the time log packet was declared. 
								** This is intentional as otherwise changes to the constant would
								** change byte ordering in the log packet.					
								*/
		
						} full_name;
		
						PACKED struct PACKED_POST
						{
		
							uint8 						 length;
								/* Number of characters in the full name 
								*/
		
							int32 						 coding_scheme;
								/* The coding scheme used to encode the full name of the network 
								** sys_network_name_coding_scheme_type_e_type 	
								*/
		
							uint8 					 add_country_initials;
								/* Flag that indicates whether MS should add the country initials 
								** to the full name of the network 
								*/
		
							uint8 						 name[255];
								/* The full name of the network
								*/
		
								/* 
								** name 
								** Hardcoded to the value of SYS_MAX_SHORT_NAME_CHARS
								** at the time log packet was declared. 
								** This is intentional as otherwise changes to the constant would
								** change byte ordering in the log packet.					
								*/
		
						} short_name;
		
						PACKED struct PACKED_POST
						{
		
							 uint8						 year;
							 uint8						 month;
							 uint8						 day;
							 uint8						 hour;
							 uint8						 minute;
							 uint8						 second;
							 int							 time_zone;
								 /*  sys_time_zone_type
								 */
		
						} univ_time_and_time_zone;
		
						int 								 time_zone;
							/* sys_time_zone_type 	 
							*/
						PACKED struct PACKED_POST
						{
							int32 						 type;
							/*	sys_lsa_identity_type_e_type
							*/
		
							uint8 						 identity[3];
		
						} lsa_identity;
		
						int32 							 daylight_saving_adj;
							/* sys_daylight_saving_adj_e_type 					 
							*/
		
					} mm_information;
		
						/* Mobility management information, i.e. network name, date,
						** time and time zone of registered network */
		
				} gw_info;
		
						/* GSM/WCDMA SS information 
						*/
		
				PACKED struct PACKED_POST
				{
		
		
					int32 								 band_class;
						/* cellular or PCS 
						** sys_band_class_e_type			 
						*/
		
					int32 								 block_or_system;
						/* PCS frequency block: A to F Cellular System A or B
						** sys_blksys_e_type	
						*/
		
					uint16								 reg_zone;
						/* 12 bit registration zone 
						*/
		
					uint16								 packet_zone;
						/* 8	bit packet zone, 0xffff means PACKET ZONE NONE.
						** Mask used is CM_SS_PACKET_ZONE_MASK.
						*/
		
					uint8 								 bs_p_rev;
						/* 8	bit base station p_rev.
						** Mask used is CM_SS_BS_P_REV_MASK.
						*/
		
					uint8 								 p_rev_in_use;
						/* 8	bit p_rev in use the lessor of BS p_rev and mobile p_rev.
						** Mask used is CM_SS_P_REV_IN_USE_MASK.
						*/
		
					uint8 								 is_registered;
						/* is mobile registered on the current system */
		
					uint8 								 ccs_supported;
						/* is concurrent services supported.
						*/
		
					int32 								 uz_id;
						 /* User zone Identification of the selected user zone,
						 ** == CM_INVALID_UZID, if no user zone selected. FEATURE_USERZONE.
						 ** Mask used is CM_SS_UZ_CHANGED_MASK.
						 */
		
					PACKED struct PACKED_POST
					{
						byte								 uz_name_encoding;
		
						byte								 uz_name_n_octets;
		
						byte								 uz_name[32];
		
		
						 /* 
						 ** uz_name 
						 ** Hardcoded to the value of SYS_PUZL_MAX_UZ_NAME_LEN
						 ** at the time log packet was declared. 
						 ** This is intentional as otherwise changes to the constant would
						 ** change byte ordering in the log packet. 				 
						 */
		
					}uz_name;
						/* User zone name of the selected user zone.
						** Valid only if uz_id != CM_INVALID_UZID. FEATURE_USERZONE.
						** Mask used is CM_SS_UZ_CHANGED_MASK.
						*/
		
					byte									 srch_win_n;
						/* Neighbour set window search size.
						** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
						*/
		
					int32 								 base_lat;
						/* Base station latitude[0.25sec and already sign extended].
						** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
						** sys_base_lat_type	
						*/
		
					int32 								 base_long;
						/* Base station latitude[0.25sec and already sign extended].
						** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
						** sys_base_long_type
						*/
		
					word									 base_id;
						/* Base station Identification number.
						** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
						** sys_base_id_type
						*/
		
					PACKED struct
					{
						int32 								 sys_mode;
						/* Indicates the system from which the time was obtained (sys_sys_mode_e_type) 
						*/
						PACKED union
						{
							PACKED struct
							{
								uint8 							 lp_sec;
								int8								 daylt_savings;
								uint8 							 ltm_offset;
							}cdma_time;
		
							PACKED struct
							{
								uint8 							 lp_sec;
								int16 							 ltm_offset;
							}hdr_time;
		
						}time;
		
					}time_info;
					/* Time obtained from sync channel / sector parameter msg
					** Mask used is CM_SS_CDMA_TIME_CHGD_MASK.
					*/
		
				} cdma_info;
						/* CDMA SS information 
						*/
			}gw3_mode_info;

    /**< Mode specific serving system information */

  PACKED struct PACKED_POST
  {
  int32                       hs_ind;
    /**< High speed service indication (Over WCDMA) */

  int32                       dtm_supp;
    /**< Dual Transfer mode indication (Over GSM) */

  int32                       egprs_supp;
    /**< EGPRS indication  */

  int32                       hs_call_status;
    /**< Clients need to check for CM_API_CELL_SUPP_SERVICES
  ** and CM_API_CELL_SUPP_SERVICES_HS_CALL  before
    ** using this structure.
  */

  } gw_cell_srv_ind;

	PACKED struct PACKED_POST
  {
  int32                       hs_ind;
    /**< High speed service indication (Over WCDMA) */

  int32                       dtm_supp;
    /**< Dual Transfer mode indication (Over GSM) */

  int32                       egprs_supp;
    /**< EGPRS indication  */

  int32                       hs_call_status;
    /**< Clients need to check for CM_API_CELL_SUPP_SERVICES
  ** and CM_API_CELL_SUPP_SERVICES_HS_CALL  before
    ** using this structure.
  */

  } gw3_cell_srv_ind;
    /**< Clients need to check for CM_API_CELL_SUPP_SERVICES before
    ** using this structure.
    **
    ** Structure to hold information about services in current cell.
    ** Information tied to CM_SS_SRV_IND_MASK.
    */

  boolean                    gw_ps_data_suspend;
    /**< Indicates that the current PS data service has been suspended.
    ** Mask used is CM_SS_PS_DATA_SUSPEND_MASK.
    */

	boolean                    gw3_ps_data_suspend;
    /**< Indicates that the current PS data service has been suspended.
    ** Mask used is CM_SS_PS_DATA_SUSPEND_MASK.
    */

  PACKED struct PACKED_POST
  {
   int32                   reject_srv_domain;
   /**<   Srv domain on which registration is rejected */

   uint8                   reject_cause;
   /**<  Registration Reject Cause */

  }gw_reg_reject_info;
    /**< Registration reject information
    */

	PACKED struct PACKED_POST
  {
   int32                   reject_srv_domain;
   /**<   Srv domain on which registration is rejected */

   uint8                   reject_cause;
   /**<  Registration Reject Cause */

  }gw3_reg_reject_info;
    /**< Registration reject information
    */

  int32                     gw_cipher_domain;
    /**< Can be used only if CM_CIPHERING_IND_SUPP is defined.
    ** Ciphering on CS or PS domain or both.
    */

	int32                     gw3_cipher_domain;
    /**< Can be used only if CM_CIPHERING_IND_SUPP is defined.
    ** Ciphering on CS or PS domain or both.
    */

LOG_RECORD_END

/*lint -e717 */             /* DIAG uses a do ... while (0) construct. */
/*lint -e754 */             /* Local structure member for not referenced */
/*lint -e778 */             /* Re reference NONE+1, which evaluates to 0 */
/*===========================================================================
                        TYPE DEFINITIONS FOR EVENTS
===========================================================================*/
#ifdef CM_GW_SUPPORTED
/* Constants used for processing USSD messages */
#define DEFAULT_ALPHABET             0x00
#define ALPHABET_MASK                0xF0
#define USS_DEF_ALPHABET_LANG_UNSPEC 0x0F
#endif /*CM_GW_SUPPORTED */


#ifdef CM_GW_SUPPORTED 
#define CM_LOG_DATA_OFFSET 12 /* Offset in log packet to write USSD indication type */
#define CM_RESPONSE_NOTIFY_DATA_LENGTH_OFFSET 13 /* Offset to write data length in log packet */
#define CM_RESPONSE_NOTIFY_DATA_OFFSET 14 /* Offset to write actual data in log packet */
#define CM_SIZE_RESPONSE_NOTIFY_HEADER 2 /* Size reqd for indication type and data len in log packet */
#endif /* CM_GW_SUPPORTED */


#define CMLOG_MAX_EVENT_PAYLOAD 8

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/

/* Type for CM test client.
*/
typedef struct cmlog_client_s {

  cm_client_id_type                client;

  sys_oprt_mode_e_type             oprt_mode;
    /* Indicates current operating mode */

  sys_sys_mode_e_type              sys_mode;
    /* Indicates current system mode */

  cm_mode_pref_e_type              mode_pref;
    /* Indicates current mode preference */

  cm_gw_acq_order_pref_e_type      acq_order_pref;
    /* The acquisition order preferred by the client. */

  cm_srv_domain_pref_e_type        srv_domain_pref;
    /* The service domain preferred by the client.
    ** This field is set after a call to cm_ph_cmd_sys_sel_pref_new. */

  cm_band_pref_e_type              band_pref;
    /* Indicates current band preference, CDMA, GW, WLAN systems  */

  cm_roam_pref_e_type              roam_pref;
    /* Indicates current roam preference */

  cm_hybr_pref_e_type              hybr_pref;
    /* Indicates hybrid preference */

  cm_network_sel_mode_pref_e_type  network_sel_mode_pref;
    /* The network selection mode selected by the client */

  sys_srv_status_e_type            srv_status;
    /* Indicates current service status */

  boolean                          offline_signal_recvd;
    /* Indicates if CM received TASK_OFFLINE_SIG signal from TMC */

  cm_cell_srv_ind_s_type           cell_srv_ind;
    /* Indicates the CELL SERVICE Type */

  cm_band_pref_e_type              lte_band_pref;
    /* Indicates current band preference LTE systems */

  cm_band_pref_e_type              tds_band_pref;
    /* Indicates current band preference TD-SCDMA systems */

  #ifdef FEATURE_MMODE_DUAL_SIM
  sys_sys_mode_e_type              gw_sys_mode;
    /* Indicate current system mode */

  cm_mode_pref_e_type              gw_mode_pref;
    /* Indicate current mode preference */

  cm_gw_acq_order_pref_e_type      gw_acq_order_pref;
    /* The acquisition order preferred by the client. */

  cm_srv_domain_pref_e_type        gw_srv_domain_pref;
    /* The service domain preferred by the client.
    ** This field is set after a call to cm_ph_cmd_sys_sel_pref_new. */

  cm_band_pref_e_type              gw_band_pref;
    /* FEATURE_SD20 - Indicate current band preference */

  cm_roam_pref_e_type              gw_roam_pref;
    /* FEATURE_SD20 - Indicate current band preference */

  cm_hybr_pref_e_type              gw_hybr_pref;
    /* Indicate hybrid preference */

  cm_network_sel_mode_pref_e_type  gw_network_sel_mode_pref;
    /* The network selection mode selected by the client */

  sys_srv_status_e_type            gw_srv_status;
    /* Indicate current service status */

  cm_cell_srv_ind_s_type           gw_cell_srv_ind;
    /* Indicates the CELL SERVICE Type */
  #endif /* FEATURE_MMODE_DUAL_SIM */

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sys_sys_mode_e_type              gw3_sys_mode;
    /* Indicate current system mode */

  cm_mode_pref_e_type              gw3_mode_pref;
    /* Indicate current mode preference */

  cm_gw_acq_order_pref_e_type      gw3_acq_order_pref;
    /* The acquisition order preferred by the client. */

  cm_srv_domain_pref_e_type        gw3_srv_domain_pref;
    /* The service domain preferred by the client.
    ** This field is set after a call to cm_ph_cmd_sys_sel_pref_new. */

  cm_band_pref_e_type              gw3_band_pref;
    /* FEATURE_SD20 - Indicate current band preference */

  cm_roam_pref_e_type              gw3_roam_pref;
    /* FEATURE_SD20 - Indicate current band preference */

  cm_hybr_pref_e_type              gw3_hybr_pref;
    /* Indicate hybrid preference */

  cm_network_sel_mode_pref_e_type  gw3_network_sel_mode_pref;
    /* The network selection mode selected by the client */

  sys_srv_status_e_type            gw3_srv_status;
    /* Indicate current service status */

  cm_cell_srv_ind_s_type           gw3_cell_srv_ind;
    /* Indicates the CELL SERVICE Type */
  #endif /* FEATURE_MMODE_TRIPLE_SIM */

  /* CDMA specific info */
  sys_channel_num_type       active_channel;
  sys_band_class_e_type      band_class;
  uint16                     packet_zone;
  sys_roam_status_e_type     roam_status;
  sys_base_lat_type          latitude;
  sys_base_long_type         longitude;
  cm_country_code_type       mcc;

} cmlog_client_s_type;

typedef struct cmlog_call_orig_dsds_s {
  uint8 asubs_id;
  uint8 call_id;
  uint8 call_type;
  uint8 srv_type;
} cmlog_call_orig_dsds_s_type;

typedef struct cmlog_call_orig_s {
  uint8 call_id;
  uint8 call_type;
  uint8 srv_type;
} cmlog_call_orig_s_type;

typedef struct cmlog_call_orig_2_s {
  uint8 call_id;
  uint8 call_type;
  uint8 srv_type;
  uint8 client_id;
  uint8 subs_id;
} cmlog_call_orig_2_s_type;

typedef struct cmlog_call_connect_dsds_s {
  uint8 asubs_id;
  uint8 call_id;
  uint8 call_type;
  uint8 sys_mode;
} cmlog_call_connect_dsds_s_type;

typedef struct cmlog_call_connect_s {
  uint8 call_id;
  uint8 call_type;
  uint8 sys_mode;
} cmlog_call_connect_s_type;

typedef struct cmlog_call_connect_2_s {
  uint8 call_id;
  uint8 call_type;
  uint8 sys_mode;
  uint8 client_id;
  uint8 subs_id;
} cmlog_call_connect_2_s_type;

typedef struct cmlog_call_end_s {
  uint8 num_calls;
  uint8 call_id;
} cmlog_call_end_s_type;

typedef struct cmlog_call_end_2_s {
  uint8 num_calls;
  uint8 call_id;
  uint8 client_id;
  uint8 subs_id;
} cmlog_call_end_2_s_type;

typedef struct cmlog_call_client_id_s {
  uint8 client_id;
  uint8 reserved1;
  uint8 reserved2;
} cmlog_call_client_id_s_type;

typedef struct cmlog_srv_confirmed_s {
  uint8                         asubs_id;
  sys_plmn_service_state_s_type srv_info;
} cmlog_srv_confirmed_s_type;

/* Holds call_state, call_type and call_id for allocated call_id's
*/
typedef struct cmlog_call_state_s {

  cm_call_id_type            call_id;
        /* call id */

  cm_call_state_e_type       call_state;
        /* Call state - idle, orig, etc. */

  cm_call_type_e_type        call_type;
        /* Call state - idle, orig, etc. */

  boolean                    is_ps_call_dormant;
        /* Gives status of ps call in converation
        ** Only applicable to ps calls.
        ** FALSE - PS call is active
        ** TRUE  - PS call is dormant
        */

  sys_modem_as_id_e_type     asubs_id;

} cmlog_call_state_s_type;

/* Type for logging ssac call events 
*/
typedef struct {
  uint8    ssac_result; 
  /* Indicate the ssac result - accept or reject the call 
   ** 0 - REJECT
   ** 1 - ACCEPT
   */

  uint8    mmtel_type;
  /* mmtel_type indicating voice/video over LTE 
   ** 0 - NONE,
   ** 1 - VOICE,  
   ** 2 - VIDEO,
   ** 3 - VOICE_VIDEO,
   */

  uint8    ssac_param_barring_factor;   
  /* barring factor for ssac_call_type */

  uint16    ssac_param_barring_time;
  /* barring time for ssac_call_type */
} cmlog_event_ssac_call_s_type;

/* Type for logging ssac timer events 
*/
typedef struct {
  uint8   timer_action;  
  /* Indicate to start/stop/cancel the ssac backoff timer 
   ** TIMER_NONE,
   ** TIMER_START,
   ** TIMER_STOP,
   ** TIMER_CANCEL,
   */

  uint8   mmtel_type;
  /* mmtel_type indicating voice/video over LTE 
   ** 0 - NONE,
   ** 1 - VOICE,  
   ** 2 - VIDEO,
   ** 3 - VOICE_VIDEO,
   */

  uint8   backoff_timer_byte_0;
  uint8   backoff_timer_byte_1;
  uint8   backoff_timer_byte_2;
  uint8   backoff_timer_byte_3;
  /* Indicates the backoff timer value.  Only applicable to start timer.
   ** For timer_action other than TIMER_START, the value will be 0.
   ** Note: These bytes are specified individually to force packing
   **       this struct to 6 bytes.  To copy a uint32 to this location,
   **       use memscpy.
   */
   
} cmlog_event_ssac_timer_s_type;


/*=========================================================================*/


/*===========================================================================
FUNCTION  cmlog_init

DESCRIPTION
  This function should be called at phone startup.
  It registers the subsystem command handlers with diag.

RETURN VALUE
  None

===========================================================================*/
void cmlog_init ( void );

#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)
/*===========================================================================
FUNCTION  cmlog_call_orig_connected

DESCRIPTION
   This function logs the dynamic switch event between CSFB and SVLTE

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmlog_csfb_svlte_switch(
  cmsoa_sv_switch_action_e_type dyn_sw
    /*dynamic switch action type*/
);
#endif

/*===========================================================================
FUNCTION  cmlog_enter_emergency_cb

DESCRIPTION
  This function should be called when CM enters emergency callback state.
  It will send an event packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_enter_emergency_cb ( void );


/*===========================================================================
FUNCTION  cmlog_exit_emergency_cb

DESCRIPTION
  This function should be called when CM exits emergency callback state.
  It will send an event packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_exit_emergency_cb ( void );


/*===========================================================================
FUNCTION  cmlog_call_state

DESCRIPTION
  This function should be called when call state info changes.
  It will send an event packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  Local Copy of the call state is updated

===========================================================================*/
void cmlog_call_state ( cm_ss_e_type      ss );

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================
FUNCTION  cmlog_get_system_mode_per_subs
===========================================================================*/
byte cmlog_get_system_mode_per_subs(

  sys_modem_as_id_e_type      asubs_id

);

/*===========================================================================
FUNCTION  cmlog_get_call_state_per_subs
===========================================================================*/
byte cmlog_get_call_state_per_subs (

  sys_modem_as_id_e_type   asubs_id
  
);
#endif /*FEATURE_MMODE_DUAL_SIM*/

/*===========================================================================
FUNCTION  cmlog_plmn_list_event_per_subs

DESCRIPTION
  This function sends a diag event when the PLMN NETWORK LIST arrives.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmlog_plmn_list_event_per_subs
(
      const sys_detailed_plmn_list_s_type *plmn_list_ptr,
        /* Pointer to a structure containing the list of available PLMNs.
        */

      cm_ss_e_type                  ss
);

/*===========================================================================
FUNCTION  cmlog_plmn_list_event

DESCRIPTION
  This function sends a diag event when the PLMN NETWORK LIST arrives.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmlog_plmn_list_event
(
        sys_detailed_plmn_list_s_type *plmn_list_ptr
            /* Pointer to a structure containing the list of available PLMNs.
            */
);

/*===========================================================================
FUNCTION  cmlog_get_call_state_list

DESCRIPTION
  Returns pointer to call_state_list static data. This data needs to be used
  in cmdiag module, so the global datum is converted to static and provided
  is through an API.

RETURN VALUE
  Pointer to type cmlog_call_state_s_type

SIDE EFFECTS
  None

===========================================================================*/
cmlog_call_state_s_type* cmlog_get_call_state_list( void );

/*===========================================================================
FUNCTION  cmlog_get_diag_client

DESCRIPTION
  Returns pointer to diag client global static data defined in cmlog.c

RETURN VALUE
  Pointer to type cmlog_client_s_type

SIDE EFFECTS
  None

===========================================================================*/
cmlog_client_s_type* cmlog_get_diag_client( void );



/*===========================================================================
FUNCTION  cmlog_service_cnf_event

DESCRIPTION
       This function sends a diag event when a SERVICE CONFIRMATION arrives.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmlog_service_cnf_event
(
    sys_plmn_service_state_s_type   *srv_info_ptr,
       /* Pointer to the service state info received in the report.
       */

    sd_ss_e_type                    ss
       /* Stack on which the service confirmation is recieved
       */
);

#if defined FEATURE_HDR_HYBRID && defined FEATURE_GPSONE
/*===========================================================================
FUNCTION  cmlog_block_hdr_orig_during_gps

DESCRIPTION
   This function logs the block HDR orig during GPS event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_block_hdr_orig_during_gps( void );

/*===========================================================================
FUNCTION  cmlog_allow_hdr_orig_during_gps

DESCRIPTION
   This function logs the allow HDR orig during GPS event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_allow_hdr_orig_during_gps( void );

#endif // defined FEATURE_HDR_HYBRID && defined FEATURE_GPSONE

/*===========================================================================
FUNCTION cmlog_call_orig_start

DESCRIPTION
   This function logs the call orig start event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_start(
  const cmcall_s_type  *call_ptr
    /*  pointer for the call object */
);


/*===========================================================================
FUNCTION  cmlog_call_orig_redial

DESCRIPTION
   This function logs the call orig redial event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_redial(
  cm_call_end_e_type    end_status,
    /* the call end status */
  cmcall_act_e_type     redial_act,
    /* the redial action */
  dword                 sr_left,
    /* number of silent redial left */
  unsigned int          sr_max_redial
    /* maximun number of silent redial attempts */
);


/*===========================================================================
FUNCTION  cmlog_call_orig_send_hdr_orig

DESCRIPTION
   This function logs the orig sent to HDR event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_send_hdr_orig( void );


/*===========================================================================
FUNCTION  cmlog_call_orig_send_mc_orig

DESCRIPTION
   This function logs the orig sent to MC event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_send_mc_orig( void );


/*===========================================================================
FUNCTION  cmlog_call_orig_end

DESCRIPTION
   This function logs the call orig end event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_end(
  cm_call_end_e_type         end_status
    /* call end status */
);


/*===========================================================================
FUNCTION  cmlog_call_orig_connected

DESCRIPTION
   This function logs the call orig connected event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_connected(
  cm_call_type_e_type         call_type,
    /* origination call type */
  cm_srv_type_e_type          srv_type,
    /* origination srv type */
  word                        srv_opt
    /* origination srv opt */
);

/*===========================================================================
FUNCTION  cmlog_set_cm_offline_signal_recvd

DESCRIPTION

  CM calls this after receiving TASK_OFFLINE_SIG signal from TMC. 
  This happens some time after cm_ph_cmd_oprt_mode() with 
  SYS_OPRT_MODE_OFFLINE_CDMA had been processed by CM.
  Call on log object oprt mode event handler to force sending of DIAG 
  EVENT_CM_OPERATIONAL_MODE event now that we got TASK_OFFLINE_SIG 
  from TMC 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  cmlog_set_cm_offline_signal_recvd(void);

#ifdef CM_GW_SUPPORTED 

/*===========================================================================
Function: cmlog_cm_send_call_information_log
Description:

       This function sends log packets containing call info.
============================================================================*/
void cmlog_cm_send_call_information_log(void);

#endif /* CM_GW_SUPPORTED */

/*===========================================================================
FUNCTION  cmlog_ssac_call_event_handler

DESCRIPTION
   This function logs the ssac call event when voice/video over LTE call is made.

RETURN VALUE
  None

SIDE EFFECTS
  None        

===========================================================================*/
void cmlog_ssac_call_event_handler(

  boolean                     ssac_call_result,
  /* ssac result - accept or reject the call */

  cmcall_ssac_mmtel_e_type    mmtel_type,
  /* voice/video over LTE */
  
  cmcall_ssac_s_type          *ssac_info
  /* containing cm ssac info, ie. ssac params, uptime, etc */

);

/*===========================================================================
FUNCTION  cmlog_ssac_timer_event_handler

DESCRIPTION
   This function logs the ssac timer event such as start/stop/reset ssac timer.

RETURN VALUE
  None

SIDE EFFECTS
  None        

===========================================================================*/
void cmlog_ssac_timer_event_handler(

  cmcall_ssac_timer_action_e_type    timer_action,
  /* start/stop/cancel timer */

  cmcall_ssac_mmtel_e_type           mmtel_type,
  /* voice/video over LTE */
  
  uint32                             timer
  /* backoff timer if timer_action == start.  0 for all the other timer_action values */

);

/*===========================================================================
FUNCTION  cmlog_call_event_report_payload

Description:

       Generate DIAG events corresponding to given call event

===========================================================================*/
void cmlog_call_event_report_payload(

  cm_call_event_e_type          call_event,

  const cm_mm_call_info_s_type  *call_info_ptr
);


#endif /* CM_LOG_V_H */
