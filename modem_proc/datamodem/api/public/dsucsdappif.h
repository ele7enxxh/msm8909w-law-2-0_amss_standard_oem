#ifndef DSUCSDAPPIF_H
#define DSUCSDAPPIF_H
/*===========================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

                  A P P L I C A T I O N   I N T E R F A C E

                          H E A D E R   F I L E
============================================================================*/
/**
  @file dsucsdappif.h
  @brief
  This header file contains interfaces that are required by UMTS circuit- 
  switched data applications. Currently, the interfaces support only 
  (external) videotelephony. They do not yet support external 
  asynchronous or synchronous CS data calls.
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the Modem_Data_mainpage file. 
		 To change any of the the file/group text for the PDF, edit the 
		 Modem_Data_mainpage file, or contact Tech Pubs.

         The above description for this file is part of the 
		 "dsucsdappif" group description in the Modem_Data_mainpage 
		 file. 
*/
/*============================================================================  
  Copyright (c) 2003 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsucsdappif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/12/10   km      Added Doxygen tags
02/08/10   sa      Moved ds_ucsd_api_get_calltype to dsucsdappif_apis.h 
09/09/09   bs      Removed private header inclusions.
02/20/08   sa      Deleted WM UCSD API's for CMI de-featurization.
12/10/08   bs      Fixed compilation issues.
10/03/08   sa      Added back WM UCSD API's for backward compatibility.
09/01/08   sa      Added modifications for CMI defeaturization.
08/08/08   sa      Added support for Modem restart and removed featurization
                   from ds_ucsd_datapath_type.
04/01/08   sa      Added prototype ds_ucsd_api_get_calltype().
10/22/07   ar      Add nw_initiated flag to ds_ucsd_call_modify_info_type.
06/27/07   ar      Add types for multiple client support for unified API.
12/14/06   ar      Add deregister interface for WindowsMobile client. 
11/15/06   ar      Export UCSD API for WindowsMobile access.
09/01/06   ar      Added support for multiprocessor build.
02/15/06   ar      Remove obsolete call orig callback.
02/09/06   ar      Lint corrections.
10/26/05   ar      Add support for multiple call instances.
08/25/05   ar      Add support for multimedia service change and fallback.
06/16/05   ar      Add support for asynchronous call setup response. 
05/20/05   ar      Added ds_ucsd_call_progress_cb_type for CM progress event.
11/12/04   dwp     Add support for VT two stage dialing via User to User IE.
11/03/04   dwp     Add support for sending reject cause for incoming call.
07/16/04   ar      Added supplementary services dial modifer to API.
03/15/04   ar      Added prototype ds_ucsd_lookup_cm_callid()
03/15/04   srk     Added propation of reject reason and cc_cause to the 
                   application.
01/27/04   vsk     changed feautre FEATURE_RLC_ENHANCED_CB to 
                   FEATURE_RLC_SDU_RCVD_ENHANCED_CB
12/29/03   vsk     ds_ucsd_rlc_dl_rx_cb_type prototype change to account for 
                   RLC callback change
08/13/03   dwp     Add support for Call Confirmation callback for VT.
06/20/03   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "sys.h"
#include "cm.h"
#include "sio.h"
#include "dsm.h"
#include "dsucsd.h"
#include "cm_gw.h"

/** @addtogroup dsucsdappif
@{ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Macro to define supported call modes (normal or SCUDIF w/bearer 
precedence order) */
/** Invalid call mode. */
#define DSUCSD_CALL_MODE_INVALID      (0x0) 
/** Data call mode. */
#define DSUCSD_CALL_MODE_DATA         (0x1)
/** Voice call mode. */
#define DSUCSD_CALL_MODE_DATA_VOICE (0x2)
/** Voice and data call mode. */
#define DSUCSD_CALL_MODE_VOICE_DATA (0x4) 
/** Maximum number of API clients. */
#define DSUCSD_API_MAX_CLIENTS    (2)      
/** Maximum value of the client ID. */
#define DSUCSD_MAX_CLIENT_ID      (DSUCSD_API_MAX_CLIENTS-1)
/** Invalid client ID. */
#define DSUCSD_INVALID_CLIENT_ID   (-1) 
/** Maximum number of supported RLP sets. */
#define DSUCSD_MAX_RLP_SETS (3)         

/** @} */ /* end_addtogroup_dsucsdappif */
/** @addtogroup dsucsdappif
@{ */

/** Indicates whether the API call is a command message or response message. */
typedef enum
{
   DS_UCSD_MSG_COMMAND,         /**< Call is command message. */
   DS_UCSD_MSG_RESPONSE         /**< Call is response message. */
} ds_ucsd_msg_type; 

/** @brief Keeps the call control-related termination parameters. */
typedef struct
{
  boolean     cc_cause_present; /**< Flags the presence/validity of the 
                                     remaining fields in this structure. */
  uint8       cc_cause;         /**< Call control cause as defined 
                                     in GSM 04.08 Table 10.86 constants, 
                                     which are defined in cause.h. */
} ds_ucsd_cc_cause_param_type;

/** @brief Keeps the incoming call answer parameters. */
typedef struct
{
  boolean     reject;          /**< Flags whether the incoming call is  
                                    accepted or rejected. */
  ds_ucsd_cc_cause_param_type  cause_info; /**< Call control cause 
                                                information. */
} ds_ucsd_answer_param_type;

/** @brief Keeps the call rejection parameters. */
typedef struct 
{
   boolean                         present;    /**< Tags the presence of the 
                                                    reject parameter field; 
                                                    reads the remaining fields 
                                                    only when this field is 
                                                    set to TRUE. */
   reject_type_enum_T              rej_type;   /**< Reject subtype enumeration 
                                                    defined in sys_cnst.h. */
   uint8                           rej_reason; /**< GSM 04.08 reject reason 
                                                    values defined in 
                                                    cause.h. */
} ds_ucsd_call_reject_param_type;

/** @brief Detailed call end parameters. This provides more detail on a call 
  termination event. */
typedef struct 
{
  cm_call_end_e_type              end_cause; /**< Call termination reason. */
  ds_ucsd_cc_cause_param_type     cc_param;  /**< Call control cause 
                                                  parameters. */
  ds_ucsd_call_reject_param_type  rej_param; /**< call rejection parameters. */
} ds_ucsd_call_end_param_type;

/** 
  Called by Radio Link Control (RLC) when data is available in the 
  downlink (DL) queue for further processing by the upper layers. 
   
  @param lc_id [out]    Logical channel ID. 
  @param num_sdus [out] Number of Service Data Units (SDUs) being enqueued
                        onto the DL WM. 
  @param context [out]  Context. 
   
  @return 
  None. 

  @dependencies 
  None. 
*/
typedef void (*ds_ucsd_rlc_dl_rx_cb_type)
(
  uint8               lc_id,     /* Logical channel ID   */  
  uint8               num_sdus,  /* Number of SDU being enqueued onto DL WM */
  void               *context    /*  context */
);

/** Definitions to manage API client registration. */
typedef int8   ds_ucsd_client_id_type;

/** @brief Structure for passing data path parameters. */
typedef struct 
{
  dsm_watermark_type        *ul_wm_ptr;      /**< Pointer to the UL L2 WM. */
  dsm_watermark_type        *dl_wm_ptr;      /**< Pointer to the DL L2 WM. */
  ds_ucsd_rlc_dl_rx_cb_type  rlc_dl_rx_cb;   /**< Called by RLC when
                                                  RLC receives the DL data. 
                                                  This callback can be NULL. */
  sio_rx_func_ptr_type       sio_ul_rx_cb;   /**< Called by SIO when
                                                  SIO receives the UL data. 
                                                  This callback can be NULL. */
} ds_ucsd_dpath_info_type;

/** @brief Structure for V.42bis data compression parameters.  */
typedef struct ds_ucsd_v42bis
{
  boolean  present;           /**< Validity of the structure. */
  uint8    direction;         /**< Desired direction of operation of the data
							       compression function; from the Data 
                                   Terminal Equipment (DTE) perspective. */
  uint8    negotiation;       /**< Specifies whether or not the Data 
                                   Communication Equipment (DCE) should continue
                                   to operate if the desired result is 
                                   not obtained. */
  uint16   max_dict;          /**< Specifies the maximum number of dictionary 
                                   entries that should be negotiated (may be 
                                   used by the DTE to limit the codeword size 
                                   transmitted based on its knowledge of the 
                                   nature of the data to be transmitted). */
  uint8    max_string;        /**< Specifies the maximum string length to be 
                                   negotiated. */
} ds_ucsd_v42bis_type;

/** @brief Structure for RLP parameters.  */
typedef struct ds_ucsd_rlp
{
  boolean                 present;    /**< Validity of the structure. */
  uint8                   version;    /**< RLP version. */
  uint16                  iws;        /**< IWF-to-MS window size. */
  uint16                  mws;        /**< MS-to-IWF window size. */
  uint16                  T1;         /**< Acknowledgement timer T1. */
  uint16                  N2;         /**< Retransmission attempts N2. */
  uint16                  T4;         /**< Resequencing timer. */
} ds_ucsd_rlp_type;


/** @brief Structure for nontransparent call parameters for query results. */
typedef struct ds_ucsd_rlp_sets /* For query results */
{
  uint8                num_sets;                      /**< Number of valid 
                                                           RLP sets. */
  ds_ucsd_rlp_type     rlp_sets[DSUCSD_MAX_RLP_SETS]; /**< RLP data 
                                                           information. */
  ds_ucsd_v42bis_type  v42_info;                      /**< V.42bis data 
                                                           information. */
} ds_ucsd_rlp_sets_type;

/** @brief Structure for nontransparent information. */
typedef struct ds_ucsd_nt_info
{
  ds_ucsd_rlp_type     rlp_info;   /**< RLP data information. */
  ds_ucsd_v42bis_type  v42_info;   /**< V.42bis data information. */
} ds_ucsd_nt_info_type;

/**  @brief Structure for call statistics reporting.  */
typedef struct ds_ucsd_call_stats
{
  boolean  present;                /**< Validity of the structure. */
  boolean  call_active;            /**< Whether or not the call is active. */
  uint32   tx_data_counter;        /**< Transmit data counter. */
  uint32   rx_data_counter;        /**< Receive data counter. */
} ds_ucsd_call_stats_type;


/** @name Data Path Specification

  @note1hang The Watermark (WM) information member is <i>not</i> used 
  in a remoted interface.
*/
/*  Structures & enum for data path specification. */
/** @{ */
/** Data path mode. */
typedef enum ds_ucsd_datapath_mode
{
  DS_UCSD_DPATH_MODE_NULL,          /**< Data path mode is NULL. */
  DS_UCSD_DPATH_MODE_SIO,           /**< Data path mode is SIO. */
  DS_UCSD_DPATH_MODE_WM             /**< Data path mode is WM. */
} ds_ucsd_datapath_mode_type;

/** @brief Serial I/O (SIO) port information.  */
typedef struct sio_info_s {
  sio_port_id_type             sio_port;      /**< SIO or SMD port. */
} ds_ucsd_sio_info_type;

/** @brief Data path and channel-related information. */
typedef struct ds_ucsd_datapath
{
  ds_ucsd_ota_channel_params_T    *chan_info; /**< Access stratum 
                                                   parameters. */
  ds_ucsd_datapath_mode_type       mode;      /**< Data path mode (SIO|WM). */
  /** @brief Data path information. */
  union dpathinfo {
    ds_ucsd_sio_info_type          sio_info;  /**< SIO or Shared Memory 
											       Device (SMD) port. */
    void                          *wm_info;   /**< Tx/Rx watermarks. */
  } info; 
} ds_ucsd_datapath_type;
/*~ FIELD ds_ucsd_datapath.chan_info POINTER */
/*~ FIELD dpathinfo.wm_info POINTER */
/*~ FIELD ds_ucsd_datapath.info DISC ds_ucsd_datapath.mode */
/*~   CASE DS_UCSD_DPATH_MODE_SIO dpathinfo.sio_info */
/*~   CASE DS_UCSD_DPATH_MODE_WM  dpathinfo.wm_info  */

/** @} */ /* end_name_group Data Path Specification */


/** @brief Used to indicate to the application that a 
  call is connected. */
typedef struct ds_ucsd_call_connect_info
{
  uint8                         inst_id;       /**< CS data call instance. */
  ds_ucsd_call_type             call_type;     /**< CS data call type. */
  boolean                       modify_allowed;/**< Whether or not in-call 
                                                    modification is 
                                                    permitted. */ 
  ds_ucsd_datapath_type        *datapath;      /**< Data path information. */
} ds_ucsd_call_connect_info_type;
/*~ FIELD ds_ucsd_call_connect_info.datapath POINTER */

/** @brief Used to indicate to the application that a
  call is ended. */
typedef struct ds_ucsd_call_end_info
{ 
  uint8                        inst_id;    /**< CS data call instance. */  
  ds_ucsd_call_end_param_type *end_param;  /**< Provides more information 
                                           about the call termination. The 
                                           application should copy 
                                           this structure if it wants to 
                                           keep the information. The pointer
                                           will be reused immediately 
                                           after the callback is returned. */
} ds_ucsd_call_end_info_type;
/*~ FIELD ds_ucsd_call_end_info.end_param POINTER */

/** @brief Used to indicate to the application that there is
  a setup request for an incoming call. */
typedef struct ds_ucsd_call_setup_info
{
  uint8                       inst_id;         /**< CS data call instance. */
  ds_ucsd_call_type           call_type;       /**< CS data call type. */
  boolean                     modify_allowed;  /**< Whether or not in-call 
                                                    modification is 
                                                    permitted. */ 
} ds_ucsd_call_setup_info_type;

/** @brief Used to indicate to the application that there is
  an incoming call. */
typedef struct ds_ucsd_call_incoming_info
{
  uint8                       inst_id;           /**< CS data call instance. */  
  sys_sys_mode_e_type         network_mode;      /**< Which network the 
                                                      system is camped on: 
                                                      WCDMA or GSM. */
  uint8                       speed;             /**< +CBST speed. */
  uint8                       name;              /**< Synchronous or 
                                                      asynchronous. */
  uint8                       connection_element; /**< Transparent or 
                                                       nontransparent. */
  uint8                       waiur;              /**< Wanted air interface
                                                       rate. */
  uint8                      *uus_data;           /**< UUS1 payload. */
  uint8                       uus_data_len;       /**< Length of uus_data. */
  uint8                      *caller_number;      /**< Caller's number. */
  uint8                       caller_number_len;  /**< Length of caller's 
                                                      number. */
} ds_ucsd_call_incoming_info_type;
/*~ FIELD ds_ucsd_call_incoming_info.uus_data VARRAY LENGTH ds_ucsd_call_incoming_info.uus_data_len */
/*~ FIELD ds_ucsd_call_incoming_info.caller_number VARRAY LENGTH ds_ucsd_call_incoming_info.caller_number_len */

/** @brief Used to indicate to the application that the
  originated call has received a progress indication. */
typedef struct ds_ucsd_call_progress_info
{
  uint8                         inst_id;       /**< CS data call instance. */  
  ds_ucsd_call_type             call_type;     /**< CS data call type. */
  uint8                         progress_info; /**< CM progress information. */  
  ds_ucsd_datapath_type        *datapath;      /**< Data path information. */
} ds_ucsd_call_progress_info_type;
/*~ FIELD ds_ucsd_call_progress_info.datapath POINTER */

/** @brief Used to indicate to the application that the
  originated call has received a setup confirmation. */
typedef struct ds_ucsd_call_confirm_info
{
  uint8                       inst_id;         /**< CS data call instance. */  
  ds_ucsd_call_type           call_type;       /**< CS data call type. */
  boolean                     modify_allowed;  /**< Whether or not in-call 
                                                    modification is 
                                                    permitted. */ 
} ds_ucsd_call_confirm_info_type;


/** @brief Used to indicate to the application that 
  there is a modify transaction for an existing call. */
typedef struct ds_ucsd_call_modify_info
{
  uint8                         inst_id;       /**< CS data call instance. */
  ds_ucsd_call_type             call_type;     /**< Type of call. */
  boolean                       status;        /**< Transaction status. */
  boolean                       nw_initiated;  /**< Network or remote party. */
  byte                          rej_cause;     /**< Rejection cause. */
  ds_ucsd_datapath_type        *datapath;      /**< Data path information. */
} ds_ucsd_call_modify_info_type;

/*~ FIELD ds_ucsd_call_modify_info.datapath POINTER */

/** @brief Call event. */
typedef struct ds_ucsd_call_event
{
  cm_call_event_e_type                call_event; /**< CM call event. */

  /** @brief Event payload. */
  union event_info
  {
    ds_ucsd_call_confirm_info_type    confirm;   /**< Call Confirmed event. */
    ds_ucsd_call_progress_info_type   progress;  /**< Call Progress event. */
    ds_ucsd_call_connect_info_type    connect;   /**< Call Connect event. */
    ds_ucsd_call_setup_info_type      setup;     /**< Call SetupIND event. */
    ds_ucsd_call_incoming_info_type   incoming;  /**< Call Incoming event. */
    ds_ucsd_call_end_info_type        end;       /**< Call End event. */
    ds_ucsd_call_modify_info_type     modify;    /**< Call Modify event. */
  } event_info; 
} ds_ucsd_call_event_type;
/*~ FIELD ds_ucsd_call_event.event_info DISC ds_ucsd_call_event.call_event */
/*~   CASE  CM_CALL_EVENT_CALL_CONF             event_info.confirm */
/*~   CASE  CM_CALL_EVENT_PROGRESS_INFO_IND     event_info.progress */
/*~   CASE  CM_CALL_EVENT_CONNECT               event_info.connect */
/*~   CASE  CM_CALL_EVENT_SETUP_IND             event_info.setup */
/*~   CASE  CM_CALL_EVENT_INCOM                 event_info.incoming */
/*~   CASE  CM_CALL_EVENT_END                   event_info.end */
/*~   DEFAULT                                   event_info.modify */


/** 
  Event notification callback function for CS data application.
  
  @param event_ptr [in]     Call notification event.
  @param user_info_ptr [in] User information pointer.
  
  @return
  None. 

  @dependencies
  None. 
*/
typedef void (*ds_ucsd_call_event_cb_type)
( 
  const ds_ucsd_call_event_type *event_ptr,
  void                          *user_info_ptr 
);
/*~ PARAM IN event_ptr POINTER */
/*~ PARAM IN user_info_ptr POINTER */

/** @} */ /* end_addtogroup_dsucsdappif */

#endif /* DSUCSDAPPIF_H */
