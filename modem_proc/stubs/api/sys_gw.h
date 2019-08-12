
#ifndef SYS_GW_H
#define SYS_GW_H

/*===========================================================================

              S Y S T E M   H E A D E R   F I L E
 
===========================================================================*/

/** 
  @file sys_gw.h
  @brief 
  Definitions that are shared between the Call Manager,
  its clients, and the GSM/WCDMA protocol stacks.
*/

/*===========================================================================
Copyright (c) 2002 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/sys_gw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#include "cm_gw.h"          /* Definitions from NAS required by clients */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** @addtogroup SYS_GW System GSM/WCDMA Protocols
    @{ */

/** Maximum number of MBMS services that can be used per service request.
*/
#define SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ 2

/** Maximum number of MBMS services that can exist simultaneously.
*/
#define SYS_GW_MBMS_MAX_SERVICES 16

/** Maximum length of the User Zone (USZ) string used in GAN display mode.
*/
#define SYS_GW_GAN_USZ_STR_MAX_LENGTH 34

/** @} */ /* end_addtogroup SYS_GW */

/** @addtogroup SYS_GW System GSM/WCDMA Protocols
    @{ */

/** Multimedia Broadcast/Multicast Services (MBMS) enumerations.
    MBMS is specific to WCDMA.
*/
typedef enum
{
  /** @cond
  */
  SYS_GW_MBMS_SRV_STATUS_NONE = -1,  /* For internal use only */
  /** @endcond
  */

  SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_NETWORK_FAILURE,
    /**< MBMS service is unavailable due to network failure. */

  SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_SETUP_FAILURE,
    /**< MBMS service is unavailable due to setup failure on the UE side. */

  SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER,
    /**< MBMS service is unavailable due to some other failure. */

  SYS_GW_MBMS_SRV_STATUS_AVAILABLE,
    /**< MBMS service is available. */

  SYS_GW_MBMS_SRV_STATUS_MONITORED,
    /**< MBMS service is monitored. */

  SYS_GW_MBMS_SRV_STATUS_OTHER,
    /**< Reserved for future use. */
    
  SYS_GW_MBMS_SRV_STATUS_RESERVED1,
    /**< Reserved for future use. */

  /** @cond
  */  
  SYS_GW_MBMS_SRV_STATUS_MAX  /* For internal use only */
  /** @endcond
  */

} sys_gw_mbms_srv_status_e_type;
/*~ SENTINEL sys_gw_mbms_srv_status_e_type.SYS_GW_MBMS_SRV_STATUS_MAX */

/** MBMS mode of service.
*/
typedef enum
{
  /** @cond
  */
  SYS_GW_MBMS_SRV_MODE_NONE = -1,  /*For internal use only */
  /** @endcond
  */

  SYS_GW_MBMS_SRV_MODE_UNKNOWN,
    /**< Service mode is unknown. */

  SYS_GW_MBMS_SRV_MODE_BROADCAST,
    /**< Service mode is broadcast. */

  SYS_GW_MBMS_SRV_MODE_MULTICAST,
    /**< Service mode is multicast. */
  
  /** @cond
  */
  SYS_GW_MBMS_SRV_MODE_MAX    /*For internal use only */
  /** @endcond
  */

}sys_gw_mbms_srv_mode_e_type;
/*~ SENTINEL sys_gw_mbms_srv_mode_e_type.SYS_GW_MBMS_SRV_MODE_MAX */

/** MBMS type of service.
*/
typedef enum
{
  /** @cond
  */
  SYS_GW_MBMS_SRV_TYPE_NONE = -1,    /* For internal use only */
  /** @endcond
  */

  SYS_GW_MBMS_SRV_TYPE_UNKNOWN = 0,
    /**< Service type is unknown. */

  SYS_GW_MBMS_SRV_TYPE_STREAMING,
    /**< Service type is streaming. */

  SYS_GW_MBMS_SRV_TYPE_DOWNLOAD,
    /**< Service type is download. */

  SYS_GW_MBMS_SRV_TYPE_OTHER,
    /**< Service type is other. */
  
  /** @cond
  */
  SYS_GW_MBMS_SRV_TYPE_MAX    /* For internal use only */
  /** @endcond
  */

}sys_gw_mbms_srv_type_e_type;
/*~ SENTINEL sys_gw_mbms_srv_type_e_type.SYS_GW_MBMS_SRV_TYPE_MAX */

/** MBMS type of service.
*/
typedef enum
{
  /** @cond
  */
  SYS_GW_MBMS_ABORT_NONE = -1,    /* For internal use only */
  /** @endcond
  */

  SYS_GW_MBMS_ABORT_ALL,
    /**< Abort all. */

  SYS_GW_MBMS_ABORT_CONTEXT_ACT,
    /**< Abort context activation. */

  SYS_GW_MBMS_ABORT_CONTEXT_DEACT,
    /**< Abort context deactivation. */

  SYS_GW_MBMS_ABORT_TRAFFIC_ACT,
    /**< Abort traffic activation. */

  SYS_GW_MBMS_ABORT_TRAFFIC_DEACT,
    /**< Abort traffic deactivation. */

  /** @cond
  */
  SYS_GW_MBMS_ABORT_MAX    /* For internal use only */
  /** @endcond
  */

}sys_gw_mbms_abort_type_e_type;
/*~ SENTINEL sys_gw_mbms_abort_type_e_type.SYS_GW_MBMS_ABORT_MAX */

/** Failure reasons for MBMS context activation or deactivation.
*/
typedef enum
{
  /** @cond
  */
  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_NONE = -1,    /* For internal use only */
  /** @endcond
  */

  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_UNKNOWN,
    /**< Unknown reason. */

  SYS_GW_MBMS_CONTEXT_FAILURE_NO_NETWORK_RESPONSE,
    /**< No network response. */

  SYS_GW_MBMS_CONTEXT_FAILURE_INVALID_PDP_CONTEXT_STATE,
    /**< Invalid PDP context state. */

  SYS_GW_MBMS_CONTEXT_FAILURE_PDP_CONTEXT_ERROR,
    /**< PDP context error. */

  SYS_GW_MBMS_CONTEXT_FAILURE_APN_ERROR,
    /**< Access Point Name (APN) error. */

  SYS_GW_MBMS_CONTEXT_FAILURE_INSUFFICIENT_RESOURCES,
    /**< Insufficient resources. */

  SYS_GW_MBMS_CONTEXT_FAILURE_AUTHENTICATION_ERROR,
    /**<  Authentication error. */

  SYS_GW_MBMS_CONTEXT_FAILURE_ACT_DEACT_REJECTED,
    /**< Activate/deactivate rejected. */

  SYS_GW_MBMS_CONTEXT_FAILURE_DUPLICATE_MBMS,
    /**< Duplicate MBMS. */

  SYS_GW_MBMS_CONTEXT_FAILURE_CONTEXT_SYNC_ERROR,
    /**< Context sync error. */

  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_OTHER,
    /**< Other. */
  
  /** @cond
  */
  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_MAX    /* For internal use only */
  /** @endcond
  */

}sys_gw_mbms_context_failure_reason_e_type;
/*~ SENTINEL sys_gw_mbms_context_failure_reason_e_type.SYS_GW_MBMS_CONTEXT_FAILURE_REASON_MAX */

/** Type that identifies the specific MBMS request sent by
    the client.
*/
typedef enum 
{
  /** @cond
  */
  SYS_GW_MBMS_REQ_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_GW_MBMS_REQ_ACTIVATE,
    /**< MBMS service activate command. */

  SYS_GW_MBMS_REQ_DEACTIVATE,
    /**< MBMS service deactivate command. */

  SYS_GW_MBMS_REQ_SESSION_COMPLETE_IND,
    /**< MBMS session complete indicator. */

  SYS_GW_MBMS_REQ_ABORT,
    /**< MBMS abort request (to abort the MBMS context activation procedure
         in NAS). */

  SYS_GW_MBMS_REQ_SUSPEND,
    /**< MBMS service suspend command. */

  SYS_GW_MBMS_REQ_RESUME,
    /**< MBMS service resume command. */

  SYS_GW_MBMS_REQ_AVAIL_SERVICES_LIST,
    /**< MBMS available services list request. */

  SYS_GW_MBMS_REQ_GET_CAPABILITY ,
    /**< MBMS capability request. */

  SYS_GW_MBMS_REQ_SET_ATTR ,
    /**< MBMS set attribute. */
  
  /** @cond
  */
  SYS_GW_MBMS_REQ_MAX  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_gw_mbms_req_e_type;
/*~ SENTINEL sys_gw_mbms_req_e_type.SYS_GW_MBMS_REQ_MAX */

/** MBMS report type.
*/
typedef enum 
{
  /** @cond
  */
  SYS_GW_MBMS_RESP_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_GW_MBMS_RESP_ACTIVATE_CNF ,
    /**< MBMS service activate confirmation. */

  SYS_GW_MBMS_RESP_DEACTIVATE_CNF ,
    /**< MBMS service deactivate confirmation. */

  SYS_GW_MBMS_RESP_SERVICE_IND ,
    /**< MBMS service indication (change in status). */

  SYS_GW_MBMS_RESP_CAPABILITY_IND ,
    /**< MBMS capability indication from RRC or NAS. */

  SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND ,
    /**< MBMS context activated indicator, from NAS after it received
         a Temporary Mobile Group Identity (TMGI) for a service. */

  SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND ,
    /**< MBMS context deactivated indicator (e.g., MBMS context deactivation,
       PDP context deactivation) from NAS. */

  SYS_GW_MBMS_RESP_ABORT_CNF,
    /**< MBMS abort confirmation. */

  SYS_GW_MBMS_RESP_SUSPEND_CNF ,
    /**< MBMS service suspend confirmation. */

  SYS_GW_MBMS_RESP_RESUME_CNF ,
    /**< MBMS service resume confirmation. */

  SYS_GW_MBMS_RESP_AVAIL_SERVICES_LIST_CNF ,
    /**< MBMS available services list confirmation. */

  SYS_GW_MBMS_RESP_LL_READY_IND ,
    /**< MBMS available ready indication. */

  SYS_GW_MBMS_RESP_SET_ATTR ,
    /**< MBMS set attribute. */

  /** @cond
  */
  SYS_GW_MBMS_RESP_MAX  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_gw_mbms_resp_e_type;
/*~ SENTINEL sys_gw_mbms_resp_e_type.SYS_GW_MBMS_RESP_MAX */

/**  Enumeration of UMA-GAN client states.
*/
typedef enum sys_gw_gan_state_e {

  /** @cond
  */
  SYS_GW_GAN_STATE_NONE = -1,    /* internal */
  /** @endcond
  */

  SYS_GW_GAN_STATE_INIT,                 
  /**< GAN client is initialized. */
                                  
  SYS_GW_GAN_STATE_SHUTDOWN,
  /**< GAN client is shutdown. */
  
  /** @cond
  */
  SYS_GW_GAN_STATE_MAX    /* internal */
  /** @endcond
  */
} sys_gw_gan_state_e_type;
/*~ SENTINEL sys_gw_gan_state_e_type.SYS_GW_GAN_STATE_MAX */


/** Enumeration of UMA-GAN signal channel status.
*/
typedef enum sys_gw_gan_sig_chan_status_e {

  /** @cond
  */
  SYS_GW_GAN_SIG_CHAN_STATUS_NONE = -1,   /* internal */
  /** @endcond
  */

  SYS_GW_GAN_SIG_CHAN_STATUS_UP,                 
  /**< GAN service is possible, pending a successful
       GAN registration procedure. */
                                  
  SYS_GW_GAN_SIG_CHAN_STATUS_DOWN,
  /**< MS cannot attempt to register with the GAN client and
       no GAN service is possible. */
  
  /** @cond
  */
  SYS_GW_GAN_SIG_CHAN_STATUS_MAX    /* internal */
  /** @endcond
  */

} sys_gw_gan_sig_chan_status_e_type;
/*~ SENTINEL sys_gw_gan_sig_chan_status_e_type.SYS_GW_GAN_SIG_CHAN_STATUS_MAX */

/**
  UMA-GAN connection status.
*/
typedef enum sys_gw_gan_conn_status_e {

  /** @cond
  */
  SYS_GW_GAN_CONN_STATUS_NONE = -1,   /* internal */
  /** @endcond
  */

  /* 0 */
  SYS_GW_GAN_CONN_STATUS_AP_MS_NOT_ASSOCIATED = 0 ,
    /**< Access Point Mobile Station is not associated. */

  SYS_GW_GAN_CONN_STATUS_AP_MS_ASSOCIATED    = SYS_BM(1),
    /**< Access Point Mobile Station is associated. */

  SYS_GW_GAN_CONN_STATUS_CS_ROVE_IN_STARTED  = SYS_BM(2),
    /**< Circuit-Switched Rove-in (GERAN/UTRAN to GAN transition) started. */
  
  SYS_GW_GAN_CONN_STATUS_PS_ROVE_IN_STARTED  = SYS_BM(3),
    /**< Packet-Switched Rove-in started. */

  /* 4 */
  SYS_GW_GAN_CONN_STATUS_CS_HAND_IN_STARTED  = SYS_BM(4),
    /**< Circuit-Switched Handover-in (GERAN/UTRAN to GAN transition) started. */

  SYS_GW_GAN_CONN_STATUS_PS_HAND_IN_STARTED  = SYS_BM(5),
    /**< Packet-Switched Handover-in started. */

  SYS_GW_GAN_CONN_STATUS_CS_ROVE_OUT_STARTED = SYS_BM(6),
    /**< Circuit-Switched Rove-out (GAN to GERAN/UTRAN transition) started. */

  SYS_GW_GAN_CONN_STATUS_PS_ROVE_OUT_STARTED = SYS_BM(7),
    /**< Packet-Switched Rove-out started. */

   SYS_GW_GAN_CONN_STATUS_CS_HAND_OUT_STARTED =SYS_BM(8),
    /**< Circuit-Switched Handover-out (GAN to GERAN/UTRAN transition) started. */

  SYS_GW_GAN_CONN_STATUS_PS_HAND_OUT_STARTED = SYS_BM(9),
    /**< Packet-Switched Handover-out started. */

  SYS_GW_GAN_CONN_STATUS_CS_HAND_OUT_OVER   = SYS_BM(10),
    /**< Circuit-Switched Handover-out (GAN to GERAN/UTRAN transition) over. */

  SYS_GW_GAN_CONN_STATUS_PS_HAND_OUT_OVER   = SYS_BM(11),
    /**< Packet-Switched Handover-out over. */

  /** @cond
  */
  SYS_GW_GAN_CONN_STATUS_MAX   /* internal */
  /** @endcond
  */

} sys_gw_gan_conn_status_e_type;
/*~ SENTINEL sys_gw_gan_conn_status_e_type.SYS_GW_GAN_CONN_STATUS_MAX */

/** @} */ /* end_addtogroup SYS_GW */

/** @addtogroup SYS_GW System GSM/WCDMA Protocols
    @{ */

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/** @brief UMA-GAN mode display.

    Describes if WLAN or GERAN is the active RAT, and the parameters if WLAN
    is the active RAT.
*/
typedef struct sys_gw_gan_mode_display_s{

  boolean        is_mode_display_on;   /**< TRUE -- WLAN is the active RAT.\n
                                            FALSE -- GERAN is the active RAT. */

  /* The parameters below are valid only if 
  ** is_mode_display_on = TRUE
  */

  uint8  gan_usz_icon_indicator;              /**< GAN User Zone icon indicator. */
  uint8  gan_ms_location_status;              /**< GAN MS location status.  */
  uint8  usz_str[SYS_GW_GAN_USZ_STR_MAX_LENGTH]; /**< GAN User Zone string. */
}sys_gw_gan_mode_display_s_type;

/** @brief Unlicensed mobile access Generic Access Network error prompt.
*/
typedef struct sys_gw_gan_error_s{

  uint8   gan_error_category;     /**< GAN error category. */
  uint8   gan_error_codepoint;    /**< GAN-specific error within the
                                       category. */
} sys_gw_gan_error_s_type;


/** @brief MBMS logical channel ID.
*/
typedef struct
{
  boolean present;       /**< TRUE -- Logical channel ID is present.\n
                              FALSE -- Logical channel ID is not present. */
  uint32  log_chan_id;   /**< Logical channel ID.  Used between DS and RLC
                              on the traffic plane. */
}sys_gw_mbms_log_chan_id_s_type;


/** @brief MBMS session ID.
*/
typedef struct
{
  boolean present;    /**< TRUE -- Session ID is present.\n
                           FALSE --  Session ID is not present. */
  uint32  session_id; /**< Session id_type. */
}sys_gw_mbms_session_id_s_type;

/** @brief Temporary Mobile Group Identity structure in integer format.
*/
typedef struct
{ 
  boolean present;    /**< TRUE  --  TMGI is present.\n
                           FALSE --  TMGI is not present. */
  uint64  tmgi;       /**< TMGI in uint64 format. */
}sys_gw_mbms_tmgi_uint_s_type;

/** @brief MBMS service priority.
*/
typedef struct
{
  boolean present;     /**< TRUE --  Priority is present.\n
                            FALSE --  Priority is not present. */
  uint32   priority;   /**< Priority of the MBMS service request. */
}sys_gw_mbms_srv_priority_s_type;

/** @brief Parameters for the MBMS activate request for each service.
*/
typedef struct 
{
  sys_gw_mbms_tmgi_uint_s_type     tmgi;       /**< TMGI of the service. */
  sys_ip_address_s_type            mip;        /**< Multicast IP.    */
  uint32                           port;       /**< Port number. */
  sys_gw_mbms_srv_priority_s_type  priority;   /**< Service priority. */
  sys_gw_mbms_srv_mode_e_type      srv_mode;   /**< Service mode. */
  sys_gw_mbms_srv_type_e_type      srv_type;   /**< Service type. */
  uint32                           mbms_parm;  /**< Generic parameter. */
}sys_gw_mbms_activate_req_entry_s_type;

/** @brief MBMS activate request parameters for num_services elements.
*/
typedef struct 
{
  uint8                                  num_services;
    /**< Number of services in the array. */

  sys_gw_mbms_activate_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services to be activated. */

}sys_gw_mbms_activate_req_s_type;

/** @brief MBMS deactivate request parameters.
*/
typedef struct 
{
  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  uint32                           mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_deactivate_req_entry_s_type;

/** @brief MBMS deactivate request parameters for num_services MBMS
    services.
*/
typedef struct 
{

  uint8                                    num_services;
    /**< Number of services in the array. Setting this to 0xFF deactivates
         all active MBMS services. */

  sys_gw_mbms_deactivate_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services to be deactivated. */

}sys_gw_mbms_deactivate_req_s_type;

/** @brief Parameters for an MBMS session complete indication.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  sys_gw_mbms_session_id_s_type       session_id;
    /**< Completed session ID. */

  uint32                           mbms_parm;
    /**< Generic parameter. */
      
}sys_gw_mbms_session_complete_ind_s_type;

/** @brief Parameters for an MBMS abort request.
*/
typedef struct 
{
  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  sys_ip_address_s_type            mip;
    /**< Multicast IP. */

  uint32                           port;
    /**< Port number. */

  sys_gw_mbms_abort_type_e_type       abort_type;
    /**< Abort type. */

  uint32                           mbms_parm;
    /**< Generic parameter. */
      
}sys_gw_mbms_abort_req_entry_s_type;

/** @brief Abort request parameters for num_services MBMS services.
*/
typedef struct 
{

  uint8                               num_services;
    /**< Number of services in the array. Setting this to 0xFF aborts all
         active MBMS services. */

  sys_gw_mbms_abort_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services list. */
      
}sys_gw_mbms_abort_req_s_type;

/** @brief MBMS suspend request.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  uint32                           mbms_parm;
    /**< Generic parameter. */
      
}sys_gw_mbms_suspend_req_entry_s_type;

/** @brief Parameters for an MBMS suspend request command.
*/
typedef struct 
{

  uint8                                 num_services;
    /**< Number of services in the array. Setting this to 0xFF suspends
         all active MBMS services. */

  sys_gw_mbms_suspend_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services list. */
      
}sys_gw_mbms_suspend_req_s_type;

/** @brief MBMS resume request.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  uint32                           mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_resume_req_entry_s_type;

/** @brief Parameters for MBMS resume request command.
*/
typedef struct 
{

  uint8                                num_services;
    /**< Number of services in the array. Setting this to 0xFF resumes
         all suspended MBMS services. */

  sys_gw_mbms_resume_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services list. */
      
}sys_gw_mbms_resume_req_s_type;

/** @brief Parameters for the MBMS available services list request command.
*/
typedef struct 
{

  uint32 mbms_parm;
    /**< Place holder for future use. */

}sys_gw_mbms_avail_srv_list_req_s_type;

/** @brief Used by clients to query MBMS capability.
*/
typedef struct 
{

  uint32 mbms_parm;
    /**< Place holder for future use. */

} sys_gw_mbms_capability_req_s_type;

/** @brief Generic MBMS command.
*/
typedef struct 
{

  uint32 mbms_parm1;  /**< Generic parameter 1. */
  uint32 mbms_parm2;  /**< Generic parameter 2. */
  uint32 mbms_parm3;  /**< Generic parameter 3. */
  uint32 mbms_parm4;  /**< Generic parameter 4. */
  uint32 mbms_parm5;  /**< Generic parameter 5. */
  uint32 mbms_parm6;  /**< Generic parameter 6. */

}sys_gw_mbms_set_attr_req_s_type;

/** @brief MBMS activate confirmation.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  sys_ip_address_s_type            mip;
    /**< Multicast IP. */

  uint32                           port;
    /**< Port number. */

  boolean                          cnf_status;
    /**< Status of request. TRUE indicates success, FALSE indicates failure. */

  sys_gw_mbms_srv_status_e_type       srv_status;
    /**< Service status. */

  sys_gw_mbms_log_chan_id_s_type      log_chan_id;
    /**< Logical channel ID that was added. */

  uint32                           mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_resp_activate_cnf_entry_s_type;

/** @brief Parameters for MBMS activate confirmation reports.
*/
typedef struct 
{

  uint8                                  num_services;
    /**< Number of services in the array. */

  sys_gw_mbms_resp_activate_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services list. */

}sys_gw_mbms_resp_activate_cnf_s_type;

/** @brief Parameters for an MBMS deactivate confirmation report from GW.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  sys_ip_address_s_type            mip;
    /**< Multicast IP. */

  uint32                           port;
    /**< Port number. */

  boolean                          cnf_status;
    /**< Status of request. TRUE indicates success, FALSE indicates failure. */

  sys_gw_mbms_srv_status_e_type       srv_status;
    /**< Service status. */

  sys_gw_mbms_log_chan_id_s_type      log_chan_id;
    /**< Logical channel ID that was removed. */

  uint32                           mbms_parm; 
    /**< Generic parameter. */

}sys_gw_mbms_resp_deactivate_cnf_entry_s_type;

/** @brief Parameters for an MBMS deactivate confirmation.
*/
typedef struct 
{

  uint8                                   num_services;
    /**< Number of services in the array. */

  sys_gw_mbms_resp_deactivate_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services list. */

}sys_gw_mbms_resp_deactivate_cnf_s_type;

/** @brief MBMS service indication.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  sys_ip_address_s_type            mip;
    /**< Multicast IP. */

  uint32                           port;
    /**< Port number. */

  sys_gw_mbms_log_chan_id_s_type      log_chan_id;
    /**< Logical channel ID that was added or removed. */

  sys_gw_mbms_srv_status_e_type       srv_status;
    /**< Service status. */

  uint32                           mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_resp_service_ind_entry_s_type;

/** @brief Parameters for service indication reports.
*/
typedef struct sys_gw_mbms_resp_service_ind_s
{

  uint8                                 num_services;
    /**< Number of services in the array. */

  sys_gw_mbms_resp_service_ind_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ]; 
    /**< Services list. */

}sys_gw_mbms_resp_service_ind_s_type;

/*~ FIELD sys_gw_mbms_resp_service_ind_s.services VARRAY SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ
    LENGTH sys_gw_mbms_resp_service_ind_s.num_services */

/** @brief Parameters for capability indication.
*/
typedef struct 
{

  boolean network_capability;
    /**< Whether the core network is MBMS capable. */

  boolean cell_capability;
    /**< Whether the cell is MBMS capable. */

  uint32 mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_resp_capability_ind_s_type;

/** @brief Parameters for a context activated indication report.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type              tmgi;
    /**< TMGI reported by NAS for the MIP specified below. */

  sys_ip_address_s_type                  mip;
    /**< Multicast IP. */

  uint32                                 port;
    /**< Port number. */

  boolean                                act_status;
    /**< Context activation status. TRUE is successful, FALSE otherwise. */

  sys_gw_mbms_context_failure_reason_e_type context_failure_reason;
    /**< Failure reason for a context activation request if act_status is
	     FALSE. */

}sys_gw_mbms_resp_context_activated_ind_s_type;

/** @brief Parameters for context deactivation.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type               tmgi;
    /**< TMGI of the service. */

  sys_ip_address_s_type                   mip;
    /**< Multicast IP. */

  uint32                                  port;
    /**< Port number. */

  boolean                                 deact_status;
    /**< Context deactivation status. TRUE is successful, FALSE otherwise. */

  sys_gw_mbms_context_failure_reason_e_type  context_failure_reason;
    /**< Reason for the failure of a context deactivation request. Valid if
         deact_status is FALSE. */

}sys_gw_mbms_resp_context_deactivated_ind_s_type;

/** @brief MBMS abort confirmation.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi_uint;
    /**< TMGI of the service. */

  sys_ip_address_s_type            mip;
    /**< Multicast IP. */

  uint32                           port;
    /**< Port number. */

  boolean                          cnf_status;
    /**< Status of the request. TRUE indicates success, FALSE indicates
	     failure. */

}sys_gw_mbms_resp_abort_cnf_entry_s_type;

/** @brief Parameters for an MBMS abort confirmation.
*/
typedef struct 
{

  uint8                               num_services;
    /**< Number of services in the array. */
 
  sys_gw_mbms_resp_abort_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services list. */

}sys_gw_mbms_resp_abort_cnf_s_type;

/** @brief MBMS suspend confirmation.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  boolean                          cnf_status;
    /**< Status of request. TRUE indicates success, FALSE indicates
	     failure. */

  uint32                           mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_resp_suspend_cnf_entry_s_type;

/** @brief Parameters for an MBMS suspend confirmation.
*/
typedef struct 
{

  uint8 num_services;
    /**< Number of services in the array. */

  sys_gw_mbms_resp_suspend_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
    /**< Services list. */

}sys_gw_mbms_resp_suspend_cnf_s_type;

/** @brief MBMS resume confirmation.
*/
typedef struct 
{

  sys_gw_mbms_tmgi_uint_s_type        tmgi;
    /**< TMGI of the service. */

  boolean                          cnf_status;
    /**< Status of request. TRUE indicates success, FALSE indicates
	     failure. */

  sys_gw_mbms_srv_status_e_type       srv_status;
    /**< Service status after resuming. */

  uint32                           mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_resp_resume_cnf_entry_s_type;

/** @brief Struct to hold parameters for an MBMS resume confirmation.
*/
typedef struct 
{
  /** Number of services in the array. */
  uint8                                num_services;

  /** Services list. */
  sys_gw_mbms_resp_resume_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];      

}sys_gw_mbms_resp_resume_cnf_s_type;

/** @brief Parameters for available services list confirmation.
*/
typedef struct 
{

  boolean                          cnf_status;
    /**< Status of the request. TRUE indicates success, FALSE indicates
	     failure. */

  uint32                           service_id[SYS_GW_MBMS_MAX_SERVICES];
    /**< List of service IDs that are currently available in the cell. */

  uint32                           mbms_parm;
    /**< Generic parameter. */

}sys_gw_mbms_resp_avail_srv_list_cnf_s_type;

/** @brief MBMS response for setting its parameters.
*/
typedef struct 
{
  /* Generic parameters */
  uint32 mbms_parm1;   /**< Generic parameter 1. */
  uint32 mbms_parm2;   /**< Generic parameter 2. */
  uint32 mbms_parm3;   /**< Generic parameter 3. */
  uint32 mbms_parm4;   /**< Generic parameter 4. */
  uint32 mbms_parm5;   /**< Generic parameter 5. */
  uint32 mbms_parm6;   /**< Generic parameter 6. */

}sys_gw_mbms_resp_set_attr_s_type;

/** @brief Indicates the lower layer (RRC) is in a READY (IDLE)
    state to service MBMS requests.
*/
typedef struct 
{

  uint32 mbms_parm1;
    /**< Generic parameter. */

} sys_gw_mbms_resp_ll_ready_ind_s_type;

/** Status of a PS signal release. It must be identical to struct
    ps_sig_rel_status_T in sm_cm.h because the values coming from NAS are
    mapped to this by the CM.
*/
typedef enum
{
   PS_SIG_REL_SUCCESS, /**< &nbsp; */
   PS_SIG_REL_FAILURE  /**< &nbsp; */
} cm_ps_sig_rel_status_T;

/** @} */ /* end_addtogroup SYS_GW System GSM/WCDMA Protocols */

#endif /* #ifndef SYS_GW_H */
