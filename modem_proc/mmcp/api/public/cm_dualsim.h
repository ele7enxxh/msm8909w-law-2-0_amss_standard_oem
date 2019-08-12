
#ifndef CM_DUALSIM_H
#define CM_DUALSIM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================================

       C A L L   M A N A G E R   D U A L S I M  H E A D E R   F I L E

===========================================================================*/

/**
@file cm_dualsim.h
@brief
This header file contains all the definitions necessary for
clients to interface with the Call Manager module. This header
file is a supplement to the original Call Manager header file
(cm.h) and contains additional information for
Dual-SIM/Dual-Standby (DS/DS) features.
                                                               */

/*==========================================================================
Copyright (c) 2002 - 2011 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/cm_dualsim.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/13   ss      FR1860 - T/G on DSDS
03/15/11   rm      DS-DS extensions for SAT-CC feature
09/15/09   sv      Initial Version
===========================================================================*/

/*-----------------------------------------------------------------------------

Common to all Call Manager Objects

-----------------------------------------------------------------------------*/
/* Defines that can be checked by CM clients to detect support
   for a feature. Added for client code to co-exist between
   different versions of CM. */

/** @ingroup COMMON
    @{
*/

/*---------------------------------------------------------------------------
** Includes
**---------------------------------------------------------------------------
*/

#include "comdef.h"     /* Definition for basic types and macros. */
#include "cm.h"         /* CM API header file. */
#include "sys.h"        /* System-wide common types. */
#include "sys_gw.h"     /* GSM/WCDMA-specific types. */
#include "mmgsdilib_common.h" /* Included for MMGSDI Session prototypes */
#include "sd.h" 

/** @} */ /* endgroup COMMON Common definitions */

/*---------------------------------------------------------------------------

Phone Object

---------------------------------------------------------------------------*/

/** @ingroup PHONE
    @{
*/


/*---------------------------------------------------------------------------
** Structures
**---------------------------------------------------------------------------
*/


/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION: cm_client_subs_reg

============================================================================*/

/**
   Allows a client to register the following subscription-related
   callbacks and events with the CM:
<ul>
   <li>Callback function to receive registered subscription events.</li>
   <li>Register and de-register subscription events.</li>
   <li>Callback function to receive other clients' phone command errors.</li>
</ul>

@dependencies Client must have already been initialized with cm_client_init.().

@return Request status.

@sideeffects None.

*/
cm_client_status_e_type cm_client_subs_reg(

  cm_client_id_type               client_id,
    /**< Requesting client. */

  cm_subs_event_f_type            *subs_event_func
    /**< Pointer to a callback function to notify the client of phone
         events. */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref_per_subs_1

============================================================================*/
/**
   Commands the CM to change the phone's system selection preferences
   (i.e., the mode, band, and roaming preferences).

@note The client-supplied callback function cmd_cb_func
      will be called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are notified of the
             corresponding CM event.

*/
extern boolean cm_ph_cmd_sys_sel_pref_per_subs_1(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type            subscription_id,
    /**< Subscription ID to which these preferences should apply. */

  cm_mode_pref_e_type               mode_pref,
    /**< Switch to this mode preferences. */

  cm_pref_term_e_type               term_pref,
    /**< Switch the preference for this term. */

  dword                             pref_duration,
    /**< Specify the duration in seconds for temporary term. */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /**< GW acquisition order preference. */

  cm_band_pref_e_type               band_pref,
    /**< Switch to this band preference. */

  cm_prl_pref_e_type                prl_pref,
    /**< Switch to this PRL preference. */

  cm_roam_pref_e_type               roam_pref,
    /**< Switch to this roam preference. */

  cm_hybr_pref_e_type               hybr_pref,
    /**< Switch to this hybrid preference. */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /**< Switch to this service domain (e.g., Circuit Switch).
         Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
         domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /**< Defines if the network should be selected automatically
         or manually. */

  const sys_plmn_id_s_type          *plmn_ptr,
    /**< If the network selection is set to manual, this specifies
         the PLMN ID. */

  uint16                             req_id
    /**< Request Id to uniquely identify request - response transactions 
         to be supplied by client. */

);

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref_per_subs_2

============================================================================*/
/**
   Commands the CM to change the phone's system selection preferences
   Supports TDS mode and contains field for band_pref fields for LTE
   and TDS
   (i.e., the mode, band, and roaming preferences).

@note The client-supplied callback function cmd_cb_func
      will be called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are notified of the
             corresponding CM event.

*/
extern boolean cm_ph_cmd_sys_sel_pref_per_subs_2(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type            subscription_id,
    /**< Subscription ID to which these preferences should apply. */

  cm_mode_pref_e_type               mode_pref,
    /**< Switch to this mode preferences. */

  cm_pref_term_e_type               term_pref,
    /**< Switch the preference for this term. */

  dword                             pref_duration,
    /**< Specify the duration in seconds for temporary term. */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /**< GW acquisition order preference. */

  cm_band_pref_e_type               band_pref,
    /**< Switch to this band preference. */

  cm_band_pref_e_type               lte_band_pref,
    /**< Switch to this band preference. */
    /* When LTE is suported on DSDS and DSDA
    ** this band_pref will be used 
    ** Send CM_BAND_PREF_NO_CHANGE for now 
    */

  cm_band_pref_e_type               tds_band_pref,
    /**< Switch to this band preference. */

  cm_prl_pref_e_type                prl_pref,
    /**< Switch to this PRL preference. */

  cm_roam_pref_e_type               roam_pref,
    /**< Switch to this roam preference. */

  cm_hybr_pref_e_type               hybr_pref,
    /**< Switch to this hybrid preference. */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /**< Switch to this service domain (e.g., Circuit Switch).
         Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
         domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /**< Defines if the network should be selected automatically
         or manually. */

  const sys_plmn_id_s_type          *plmn_ptr,
    /**< If the network selection is set to manual, this specifies
         the PLMN ID. */

  uint16                             req_id
    /**< Request Id to uniquely identify request - response transactions 
         to be supplied by client. */
    /** Pass 0 as in cm_ph_cmd_sys_sel_pref_per_subs_1 */

);

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref_per_subs

============================================================================*/
/**
   Commands the CM to change the phone's system selection preferences
   (i.e., the mode, band, and roaming preferences).

@note The client-supplied callback function cmd_cb_func
      will be called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are notified of the
             corresponding CM event.

*/
extern boolean cm_ph_cmd_sys_sel_pref_per_subs(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type            subscription_id,
    /**< Subscription ID to which these preferences should apply. */

  cm_mode_pref_e_type               mode_pref,
    /**< Switch to this mode preferences. */

  cm_pref_term_e_type               term_pref,
    /**< Switch the preference for this term. */

  dword                             pref_duration,
    /**< Specify the duration in seconds for temporary term. */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /**< GW acquisition order preference. */

  cm_band_pref_e_type               band_pref,
    /**< Switch to this band preference. */

  cm_prl_pref_e_type                prl_pref,
    /**< Switch to this PRL preference. */

  cm_roam_pref_e_type               roam_pref,
    /**< Switch to this roam preference. */

  cm_hybr_pref_e_type               hybr_pref,
    /**< Switch to this hybrid preference. */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /**< Switch to this service domain (e.g., Circuit Switch).
         Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
         domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /**< Defines if the network should be selected automatically
         or manually. */

  const sys_plmn_id_s_type          *plmn_ptr
    /**< If the network selection is set to manual, this specifies
         the PLMN ID. */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_wakeup_from_standby_per_subs

=============================================================================*/

/**
   Commands the CM to instruct lower layers to wake up from deep
   sleep (standby mode).

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
boolean cm_ph_cmd_wakeup_from_standby_per_subs(

  cm_ph_cmd_cb_f_type     cmd_cb_func,
    /**< Client callback function. */ 

  void                    *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type       client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type  subscription_id
    /**< Subscription ID of the stack to awaken. */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_acm_per_subs

============================================================================*/

/**
   Returns the current value of the Accumulated Call Meter
   (ACM).

@dependencies FEATURE_GSM or FEATURE_WCDMA must be defined.

@return The ACM value from the SIM.

@sideeffects None.

*/
unsigned long cm_ph_cmd_aoc_get_acm_per_subs(

  sys_modem_as_id_e_type                subscription_id
    /**< Subscription ID to use for retrieving the current ACM. */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_acmmax_per_subs

============================================================================*/

/**

   Returns the current maximum value of the ACM.

@dependencies FEATURE_GSM or FEATURE_WCDMA must be defined.

@return The ACMMax value from the SIM.

@sideeffects None.

*/
unsigned long cm_ph_cmd_aoc_get_acmmax_per_subs(

  sys_modem_as_id_e_type                subscription_id
  /**< Subscription ID to use for retrieving the current maximum ACM. */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_ccm_per_subs

============================================================================*/

/**

   Returns the value of the Current Call Meter (CCM).

@dependencies FEATURE_GSM or FEATURE_WCDMA must be defined.

@return The CCM value from the current or last call.

@sideeffects None.

*/
unsigned long cm_ph_cmd_aoc_get_ccm_per_subs(

  sys_modem_as_id_e_type            subscription_id
  /**< Subscription ID to use for retrieving the CCM value. */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_get_puct_per_subs

============================================================================*/

/**

   Returns the current value of the Price Unit Currency Conversion (PUCC).

@dependencies FEATURE_GSM or FEATURE_WCDMA must be defined.

@return The current PUCT value from the SIM.

@sideeffects None.

*/
unsigned long cm_ph_cmd_aoc_get_puct_per_subs(

  sys_modem_as_id_e_type    subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_aoc_reset_acm_per_subs

============================================================================*/

/**
   Sends a command to the CM task to reset the ACM to 0.

@dependencies FEATURE_GSM or FEATURE_WCDMA must be defined.

@return None.

@sideeffects None.

*/
boolean cm_ph_cmd_aoc_reset_acm_per_subs(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< Client callback function. */

  void                   *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type      client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type subscription_id
    /**< Subscription ID of the ACM to reset.
         Only valid in Dual Standby mode. In Single Standby mode, 
         it defaults to the active subscription. */
);

/*===========================================================================

FUNCTION cm_ph_cmd_aoc_set_acmmax_per_subs

===========================================================================*/

/**
   Sends a message to the CM to set the maximum value the ACM
   can achieve before calls are ended.

@dependencies FEATURE_WCDMA or FEATURE_GSM must be defined.

@return None.

@sideeffects None.

*/
boolean cm_ph_cmd_aoc_set_acmmax_per_subs(

  cm_ph_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function. */

  void                      *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type         client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type    subscription_id,
    /**< Subscription ID of the ACM to set.
         Only valid in Dual Standby mode. In Single Standby mode,
         it defaults to the active subscription. */

  uint32                    new_max
    /**< The new maximum value. */
);

/*===========================================================================

FUNCTION cm_ph_cmd_dual_standby_pref

===========================================================================*/

/**
   Commands the CM to avoid the system defined in avoid_type
   (avoid_time is no longer used).

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. CDMA
      only.
  
@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
boolean cm_ph_cmd_dual_standby_pref(

        cm_ph_cmd_cb_f_type                 cmd_cb_func,
            /**< Client callback function. */

        void                                *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type                    client_id,
            /**< Requesting client. */

        sys_modem_dual_standby_pref_e_type   standby_pref,
            /**< Standby preference of the phone. */

        sys_modem_as_id_e_type               active_subs,
            /**< The active subscription in Single Standby mode.
                 Only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY. */

        sys_modem_as_id_e_type               default_voice_subs,
            /**< Default subscription-ID to be used for Voice calls */

        sys_modem_as_id_e_type               default_data_subs,
          /**< Default subscription-ID to be used for Data calls */

        sys_modem_as_id_e_type               priority_subs,
          /**< Subscription-ID that is given priority to access Radio Resources 
          */
        cm_dds_cause_e_type                  dds_switch_type
);

/*===========================================================================

FUNCTION cm_ph_cmd_dual_standby_pref_1

===========================================================================*/

/**
   Commands the CM to avoid the system defined in avoid_type
   (avoid_time is no longer used).

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. CDMA
      only.
  
@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
boolean cm_ph_cmd_dual_standby_pref_1(

        cm_ph_cmd_cb_f_type                 cmd_cb_func,
            /**< Client callback function. */

        void                                *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type                    client_id,
            /**< Requesting client. */

        sys_modem_dual_standby_pref_e_type   standby_pref,
            /**< Standby preference of the phone. */

        uint8                                active_subs,
            /**< The active subscription in Single Standby mode.
                 Only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY. */

        sys_modem_as_id_e_type               default_voice_subs,
            /**< Default subscription-ID to be used for Voice calls */

        sys_modem_as_id_e_type               default_data_subs,
          /**< Default subscription-ID to be used for Data calls */

        sys_modem_as_id_e_type               priority_subs,
          /**< Subscription-ID that is given priority to access Radio Resources 
          */
       cm_dds_cause_e_type                  dds_switch_type
	             /**< DDS Switch type
          */
);

/*===========================================================================

FUNCTION: cm_ph_cmd_get_networks_per_subs_extn

===========================================================================*/

/**
  Commands the CM to retrieve either a list of preferred
  networks or a list of all available networks.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE The command was successfully sent to the CM.
@retval
FALSE The command was not successfully sent to the CM.

@sideeffects If the command is successful, CM clients receive
             the list of networks via the
             CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or
             CM_PH_EVENT_PREFERRED_NETWORKS_CONF event.

*/
boolean cm_ph_cmd_get_networks_per_subs_extn (

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /**< Client callback function. */

        void                        *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type            client_id,
            /**< Requesting client. */

        cm_network_list_type_e_type  list_type,
            /**< Indicates whether to retrieve the preferred or
                 available networks. */

        cm_mode_pref_e_type          network_type,
            /**< Indicates the desired network, such as WLAN or GW.
                 Only WLAN or GW is supported, and they can only be 
                 requested one at a time. */

        sys_modem_as_id_e_type       subscription_id,
            /**< Subscription ID to use to retrieve the GW network list.
                 Only valid when the network_type is GW and in Dual Standby mode. 
                 In Single Standby mode, a search can only be performed on active
                 subscriptions. */

         cm_band_pref_e_type               band_pref,
         /**< Switch to this band preference.*/

         cm_band_pref_e_type               lte_band_pref,
         /**< Switch to this LTE Band preference. */

         cm_band_pref_e_type               tds_band_pref
         /**< Switch to this TDS Band preference. */

);

/*===========================================================================

FUNCTION: cm_ph_cmd_get_networks_per_subs

===========================================================================*/

/**
  Commands the CM to retrieve either a list of preferred
  networks or a list of all available networks.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE The command was successfully sent to the CM.
@retval
FALSE The command was not successfully sent to the CM.

@sideeffects If the command is successful, CM clients receive
             the list of networks via the
             CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or
             CM_PH_EVENT_PREFERRED_NETWORKS_CONF event.

*/
boolean cm_ph_cmd_get_networks_per_subs (

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /**< Client callback function. */

        void                        *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type            client_id,
            /**< Requesting client. */

        cm_network_list_type_e_type  list_type,
            /**< Indicates whether to retrieve the preferred or
                 available networks. */

        cm_mode_pref_e_type          network_type,
            /**< Indicates the desired network, such as WLAN or GW.
                 Only WLAN or GW is supported, and they can only be 
                 requested one at a time. */

        sys_modem_as_id_e_type       subscription_id
            /**< Subscription ID to use to retrieve the GW network list.
                 Only valid when the network_type is GW and in Dual Standby mode. 
                 In Single Standby mode, a search can only be performed on active
                 subscriptions. */
);



/*===========================================================================

FUNCTION cm_ph_cmd_terminate_get_networks_per_subs

============================================================================*/

/**
  Command CM to terminate retrieval operation of list of all available
  networks.

@note The client supplied callback function, cmd_cb_func, will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been called.

@retval TRUE  = command was successfully sent to CM
@retval FALSE = command was not successfully sent to CM

@sideeffects    If command is successful, the previous command to retrieve the 
                list will be terminated.

*/
boolean cm_ph_cmd_terminate_get_networks_per_subs(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type            client_id,
    /* Requesting client */

  sys_modem_as_id_e_type       subscription_id
    /* Subscription ID to use for termination the GW network list request 
    ** Not used when terminating WLAN Network list request 
    */
);



/*===========================================================================

FUNCTION: cm_ph_cmd_set_preferred_networks_per_subs

============================================================================*/

/**
   Commands the CM to save the preferred networks to the
   SIM/USIM.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE The command was successfully sent to the CM.
@retval FALSE The command was not successfully sent to the CM.

@sideeffects None.

*/
boolean cm_ph_cmd_set_preferred_networks_per_subs(

        cm_ph_cmd_cb_f_type                   cmd_cb_func,
            /**< Client callback function. */

        void                                  *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type                     client_id,
            /**< Requesting client. */

        const sys_user_pref_plmn_list_s_type  *preferred_networks_ptr,
            /**< The preferred networks to be saved. */

        sys_modem_as_id_e_type                subscription_id
          /**< Subscription ID to use to retrieve the GW network list.
               Only valid when the network_type is GW and in Dual Standby mode. 
               In Single Standby mode, a search can only be performed on active
               subscriptions. */
);

/*===========================================================================

FUNCTION: cm_ph_net_list_copy_per_subs

===========================================================================*/

/**
   Copies network lists from phone event information to the
   client when requested by the client.

@dependencies None.

@return None.

@sideeffects None.

*/
void cm_ph_net_list_copy_per_subs
(
  cm_ph_net_lists_info_s_type   *info_ptr,
       /**< Pointer to the network list. */
       
  sys_modem_as_id_e_type    subscription_id
       /**< Active subscription ID. */
);


/*===========================================================================

FUNCTION: cm_ph_cmd_get_ph_info_per_subs

============================================================================*/

/**
   Gets a snapshot of the current phone information in the CM.
   The function submits a request for a snapshot of the phone
   object.

@note The client needs to register for CM_PH_EVENT_INFO to
      receive the response to this command. CM_PH_EVENT_INFO is
      sent only to the client that issues this command.

@note The client can call this function only after it has
      received a CM_PH_EVENT_INFO_AVAIL notification from CM.

@dependencies The CM must have already been initialized with
              cm_init_before_task_start() and
              cm_init_after_task_start().

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects None.

*/
extern boolean cm_ph_cmd_get_ph_info_per_subs(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< Client callback function. */

  void                   *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type      client_id,
    /**< Requesting client ID. */

  sys_modem_as_id_e_type subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_mm_ss_cmd_ps_data_available

===========================================================================*/

/**
   Informs the CM that data is available for transmission.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
boolean cm_mm_ss_cmd_ps_data_available_per_subs(

  cm_ss_cmd_cb_f_type            cmd_cb_func,
    /**< Client callback function. */

  void                           *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type              client_id,
    /**< Requesting client ID. */

  sys_modem_as_id_e_type         subscription_id
    /**< Active subscription ID. */
);


/** @} */

/*  endgroup PHONE Phone Related definitions
*/

/*-----------------------------------------------------------------------------

Supplementary Services Object

-----------------------------------------------------------------------------*/

/** @ingroup SSO
    @{
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION: cm_sups_cmd_register_cc_per_subs

=============================================================================*/

/**
   Registers the Supplementary Service (SUPS) command to the CM.
   The client ID should be from a registered
   CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_register_cc_per_subs(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /**< Client callback function. */

  void                            *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type               client_id,
    /**< Originating client. */

  const cm_reg_ss_params_s_type   *reg_cmd_params,
    /**< Pointer to register SS command data. */

  const cm_num_s_type             *dialed_digits,
    /**< Pointer to cm_num_s_type of the digits dialed. */

  boolean                         is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type          subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_sups_cmd_erase_cc_per_subs

===========================================================================*/

/**
   Erases the SUPS command to the CM. The
   client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_erase_cc_per_subs(

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
    /**< Client callback function. */

  void                                *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                   client_id,
    /**< Originating client. */

  const cm_erase_ss_params_s_type     *erase_cmd_params,
    /**< Pointer to erase SS command data. */

  const cm_num_s_type                 *dialed_digits,
    /**< Pointer to cm_num_s_type of the digits dialed. */

  boolean                             is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type              subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_sups_cmd_activate_cc_per_subs

============================================================================*/

/**
   Activates the SUPS command to the CM. The
   client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_activate_cc_per_subs(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /**< Client callback function. */

  void                            *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type               client_id,
    /**< Originating client. */

  const cm_act_ss_params_s_type   *act_cmd_params,
    /**< Pointer to activate SS command data. */

  const cm_num_s_type             *dialed_digits,
    /**< Pointer to cm_num_s_type of the digitis dialed. */

  boolean                         is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type          subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_sups_cmd_deactivate_cc_per_subs

=============================================================================*/

/**
   Deactivates the SUPS command to the CM. The
   client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_deactivate_cc_per_subs(

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Originating client. */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /**< Pointer to erase SS command data. */

  const cm_num_s_type               *dialed_digits,
    /**< Pointer to cm_num_s_type of the digits dialed. */

  boolean                           is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type            subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_sups_cmd_interrogate_cc_per_subs

===========================================================================*/

/**
   Enables the client to send an Interrogate SUPS request to the
   CM. The client ID should be from a
   registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_interrogate_cc_per_subs(

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
    /**< Client callback function. */

  void                                *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                   client_id,
    /**< Originating client. */

  const cm_interr_ss_params_s_type   *interr_cmd_params,
    /**< Pointer to interrogate SS command data. */

  const cm_num_s_type                *dialed_digits,
    /**< Pointer to cm_num_s_type of the digits dialed. */

  boolean                             is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type              subscription_id
    /**< Active subscription ID. */

);

/*===========================================================================

FUNCTION: cm_sups_cmd_process_uss_cc_per_subs

============================================================================*/

/**
   Processes an unstructured SUPS command to the CM. 
   The client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_process_uss_cc_per_subs(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /**< Client callback function. */

  void                            *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type               client_id,
    /**< Originating client. */

  const cm_uss_params_s_type      *process_uss_cmd_params,
    /**< Pointer to process Unstructured SS command data. */

  const cm_num_s_type             *dialed_digits,
    /**< Pointer to cm_num_s_type of the digits dialed. */

  boolean                         is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type          subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_sups_cmd_reg_password_cc_per_subs

===========================================================================*/

/**
   Registers the password SUPS command to the CM. 
   The client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_reg_password_cc_per_subs(

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Originating client. */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params,
    /**< Pointer to register password SS command data. */

  const cm_num_s_type              *dialed_digits,
    /**< Pointer to cm_num_s_type of the digits dialed. */

  boolean                           is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type            subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_sups_cmd_release_cc_per_subs

===========================================================================*/

/**
   Releases the SUPS command to the CM. The
   client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_release_cc_per_subs(

  cm_sups_cmd_cb_f_type                 sups_cmd_cb_func,
    /**< Client callback function. */

  void                                  *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                     client_id,
    /**< Originating client. */

  const cm_release_ss_params_s_type     *release_cmd_params,
    /**< Pointer to release SS command data. */

  const cm_num_s_type                   *dialed_digits,
    /**< Pointer to cm_num_s_type of the digits dialed. */

  boolean                               is_call_control_processing_complete,
    /**< Indicates if call control has been completed. */

  sys_modem_as_id_e_type                subscription_id
    /**< Active subscription ID. */
);

/*===========================================================================

FUNCTION: cm_sups_cmd_get_password_res_per_subs

============================================================================*/

/**
   Called by the client to send a Get Password Response to the
   CM. This response is sent when the mobile station receives a
   Get Password Indication from the network. The client ID
   should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_get_password_res_per_subs(

  cm_sups_cmd_cb_f_type                   sups_cmd_cb_func,
    /**< Client callback function. */

  void                                    *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                       client_id,
    /**< Originating client. */

  const cm_get_pwd_res_params_s_type      *get_pwd_res_params,
    /**< Pointer to Get Password response data. */

  sys_modem_as_id_e_type                  subscription_id
    /**< Active subscription ID. */

);


/*===========================================================================

FUNCTION: cm_sups_cmd_lcs_molr_per_subs

===========================================================================*/

/**
   Sends a Location Services (LCS) Mobile-Originated Location
   Request (MOLR) SUPS command to the CM. The client ID should
   be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
boolean cm_sups_cmd_lcs_molr_per_subs
(

        cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
            /**< Client callback function. */

        void                            *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type                client_id,
            /**< Originating client. */

        const cm_lcs_molr_params_s_type *lcs_molr_cmd_params,
            /**< Pointer to LCS MOLR SS command data. */

        sys_modem_as_id_e_type          subscription_id
            /**< Active subscription ID */

);


/*===========================================================================

FUNCTION: cm_sups_cmd_lcs_location_notification_res_per_subs

===========================================================================*/

/**
   Sends an LCS location notification response to the network.
   The client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
boolean cm_sups_cmd_lcs_location_notification_res_per_subs(

        cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
            /**< Client callback function. */

        void                              *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type                 client_id,
            /**< Originating client. */

        const cm_lcs_location_notification_res_params_s_type       
                                          *lcs_location_notification_res_params,
            /**< Pointer to the location notification response data. */

        sys_modem_as_id_e_type            subscription_id
            /**< Active subscription ID. */

);


/*===========================================================================

FUNCTION: cm_sups_cmd_lcs_molr_completed_per_subs

===========================================================================*/

/**
   Sends an LCS MOLR SUPS Completed command to the CM. The
   client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
boolean cm_sups_cmd_lcs_molr_completed_per_subs(

        cm_sups_cmd_cb_f_type                 sups_cmd_cb_func,
            /**< Client callback function. */

        void                                  *data_block_ptr,
            /**< Pointer to the client callback data block. */

        cm_client_id_type                     client_id,
            /**< Originating client. */

 const cm_lcs_molr_completed_params_s_type    *lcs_molr_completed_cmd_params,
            /**< Pointer to LCS MOLR Completed SS command data. */

        sys_modem_as_id_e_type                subscription_id
            /**< Active subscription ID. */
);


/*===========================================================================

FUNCTION: cm_sups_cmd_uss_notify_res_per_subs

===========================================================================*/

/**
   Called by the client to send an Unstructured Supplementary
   Service Notify Response to the CM. This response message is
   sent by the mobile station when an Unstructured Supplementary
   Service Notification received from the network has an error.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. If the
      command request is processed without errors,
      CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_uss_notify_res_per_subs(

  cm_sups_cmd_cb_f_type                     sups_cmd_cb_func,
    /**< Client callback function. */

  void                                      *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                         client_id,
    /**< Originating client. */

  const cm_uss_notify_res_params_s_type     *uss_notify_res_params,
    /**< Pointer to Unstructured SS Notify response data. */

  sys_modem_as_id_e_type                    subscription_id
    /**< Active subscription ID. */

);


/*===========================================================================

FUNCTION: cm_sups_cmd_uss_res_per_subs

===========================================================================*/

/**
   Sends an Unstructured SUPS response command to the CM.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status.

@dependencies cm_init_before_task_start() must have already been
              called. FEATURE_WCDMA or FEATURE_GSM must be
              defined.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_sups_cmd_uss_res_per_subs(

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
    /**< Client callback function. */

  void                                *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                   client_id,
    /**< Originating client. */

  const cm_uss_res_params_s_type      *uss_res_params,
    /**< Pointer to Unstructured SS response data. */

  sys_modem_as_id_e_type              subscription_id
    /**< Active subscription ID. */

);


/** @} */ /* endgroup SSO Supplementary Services Object */

/** @ingroup SMS
    @{
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION: cm_sms_cmd_mo_msg_per_subs

===========================================================================*/

/**
   Sends a mobile-originated SMS message.

@dependencies CM must have already been initialized with
              cm_init_before_task_start() and
              cm_init_after_task_start().\n Can only be used
              when CM_API_SMS_OVER_IP is defined.

@retval TRUE request sent.
@retval FALSE No buffer to send request.

@sideeffects None.

*/
extern boolean cm_sms_cmd_mo_msg_per_subs(

    cm_sms_cmd_cb_f_type    cmd_cb_func,
        /**< Client callback function. */

    void                    *data_block_ptr,
        /**< Pointer to the client callback data block. */

    cm_client_id_type       client_id,
        /**< Requesting client ID. */

    boolean                 l2_ack_requested,
        /**< l2 ACK is required or not. */

    const cm_sms_msg_type   *data,
        /**< Pointer to the SMS buffer. */

    cm_mode_pref_e_type     mode_pref,
        /**< The mode with which the SMS is sent. */

    sys_modem_as_id_e_type  subscription_id
        /**< Active subscription ID. */

); /* cm_sms_cmd_mo_msg_per_subs */

/** @} */ /* endgroup SMS API for Dual Standby */

/** @ingroup CALL
** @{
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/*==========================================================================

FUNCTION: cm_mm_call_cmd_orig_per_subs

==========================================================================*/

/**
   Commands the CM to originate a call. The client ID should be
   from a registered CM client. The activate_code field is only
   used for Over-the-Air Service Provisioning. (OTASP) calls
   (i.e., the call type is CM_CALL_TYPE_STD_OTASP or
   CM_CALL_TYPE_NON_STD_OTASP). For non-OTASP calls, this field
   should be set to CM_OTASP_ACT_CODE_NONE. For called_numbers,
   if the number->digit_mode is set to CM_DIGIT_MODE_4BIT_DTMF,
   the number->buf may contain only the 0-9, #, or * characters,
   and if the number->digit_mode is set to
   CM_DIGIT_MODE_8BIT_ASCII, the number buffer may contain any
   8-bit ASCII characters.

   If the call type is voice, the CM calls the application
   registered resolve call type callback function to determine
   if the dialed number is one of the special numbers for OTASP
   or Emergency. If it is a special number, the resolve call
   type function modifies the original call type accordingly. If
   no application registered a resolve call type callback
   function with the CM, the call type is not changed. For all
   call types, the CM calls the application registered call
   allowed callback function to determine if the dialed number
   is allowed. If no application registered a call allowed
   callback function with the CM, the call origination for all
   dialed numbers is always allowed. For targets supporting dual
   slots, the application registered resolve call type and call
   allowed callback functions are called as described above if
   there is no call control processing callback function
   registered. If there is a call control processing callback
   function registered, the CM calls the call control processing
   callback type function instead. This call control processing
   callback type function performs call type resolutions and
   call allowed checks. It also performs any call control that
   is required. In the case of dual slots in automatic mode,
   conflicts may arise during call control. The CM calls an
   application registered call control preferences callback type
   function to resolve any such conflicts.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. The
      error code is passed back to the client through the
      client-provided command callback function, cmd_cb_func. If
      the command request is processed without errors,
      CM_CALL_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called.

@return Call_id is returned in call_id_ptr.
@retval TRUE Request sent
@retval FALSE No buffer to send request

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_mm_call_cmd_orig_per_subs(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function. */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block. */

  cm_client_id_type           client_id,
    /**< Originating client. */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const sys_modem_as_id_e_type subscription_id,
    /**< Active subscription ID. */

  const cm_num_s_type         *calling_number,
    /**< Number of the mobile that is originating the call. */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called. */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination. */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to CDMA mode-specific data. */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode-specific data. */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode-specific data. */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned. */
);

/*===========================================================================

FUNCTION: cm_mm_call_cmd_sups_per_subs

===========================================================================*/

/**
   Commands the CM to send a supplementary service request to
   the Base Station (BS). All GW SUPS come with a valid call ID.
   The call ID passed has a range from 0 to CM_CALL_ID_MAX. The
   client ID should be from a registered CM client.

@note The client-supplied callback function cmd_cb_func will be
      called to notify the client of the command status. The
      error code is passed back to the client through the
      client-provided command-callback function, cmd_cb_func. If
      the command request is processed without errors,
      CM_CALL_CMD_ERR_NOERR is passed back to the client.

@dependencies cm_init_before_task_start() must have already been
              called.

@retval TRUE Request sent.
@retval FALSE No buffer to send request.

@sideeffects If the command is successful, CM clients are
             notified of the corresponding CM event.

*/
extern boolean cm_mm_call_cmd_sups_per_subs(

  cm_call_cmd_cb_f_type             cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Sending client. */

  cm_call_sups_type_e_type          sups_type,
    /**< Type of supplementary service. */

  const cm_call_sups_params_s_type  *sups_params_ptr,
    /**< Pointer to supplementary service-specific data. */

  sys_modem_as_id_e_type            subscription_id
    /**< Active subscription ID for origination. */

);

/*===========================================================================

FUNCTION: cm_mm_call_cmd_orig_cc_per_subs

===========================================================================*/
/**
  Commands the CM to originate a call. Call control will be done in the CM
  before originating the call. The client ID should be from a registered CM 
  client. Clients must check if CM_API_MODEM_CALL_CONTROL is defined 
  before calling this API. The call origination request needs the subs id
  on which it has to go.

  The client-supplied callback function cm_call_cmd_exten_cb_f_type
  will be called to notify the client of the command status. The error code is
  passed back to the client through the client-provided command callback 
  function cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@param[in] cmd_cb_func           Client callback function.   
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Originating client.
@param[in] call_type             Call type (e.g., voice, data).
@param[in] srv_type              Service type (e.g., automatic).
@param[in] calling_number        Number of the phone that is originating the call.
@param[in] called_number         Number dialed (phone that is being called).
@param[in] alpha_buf             Alpha tag to be displayed for the origination.
@param[in] cdma_orig_params_ptr  Pointer to CDMA mode-specific data.
@param[in] gw_cs_orig_params_ptr Pointer to GW CS mode-specific data.
@param[in] gw_ps_orig_params_ptr Pointer to GW PS mode-specific data.
@param[in] switch_out_call_id    Switch out call ID.
@param[in] orig_client_categ     Initiating client category.
@param[in] asubs_id              Subscription id on which the call should go
@param[out] return_call_id_ptr   Call ID will be allocated and returned.

@return 
  Switch_in Call_id will be returned in switch_in_call_id_ptr.
  - TRUE -- request sent.
  - FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_orig_cc_per_subs(

  cm_call_cmd_exten_cb_f_type       cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_call_type_e_type               call_type,
  cm_srv_type_e_type                srv_type,
  const cm_num_s_type               *calling_number,
  const cm_num_s_type               *called_number,
  const cm_orig_alpha_s_type        *alpha_buf,
  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
  cm_call_id_type                   switch_out_call_id,
  cm_orig_client_categ_e_type       orig_client_categ,
  const sys_modem_as_id_e_type      asubs_id,
  cm_call_id_type                   *return_call_id_ptr

);
/*~ FUNCTION cm_mm_call_cmd_orig_cc_per_subs */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */

/*===========================================================================

FUNCTION: cm_mm_call_cmd_orig_cc_per_subs_exten

===========================================================================*/
/**
  Commands the CM to originate a call. Call control will be done in the CM
  before originating the call. The client ID should be from a registered CM 
  client. Clients must check if CM_API_MODEM_CALL_CONTROL is defined 
  before calling this API. The call origination request needs the subs id
  on which it has to go.

  The client-supplied callback function cm_call_cmd_exten_cb_f_type
  will be called to notify the client of the command status. The error code is
  passed back to the client through the client-provided command callback 
  function cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@param[in] cmd_cb_func           Client callback function.   
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Originating client.
@param[in] call_type             Call type (e.g., voice, data).
@param[in] srv_type              Service type (e.g., automatic).
@param[in] calling_number        Number of the phone that is originating the call.
@param[in] called_number         Number dialed (phone that is being called).
@param[in] alpha_buf             Alpha tag to be displayed for the origination.
@param[in] cdma_orig_params_ptr  Pointer to CDMA mode-specific data.
@param[in] gw_cs_orig_params_ptr Pointer to GW CS mode-specific data.
@param[in] gw_ps_orig_params_ptr Pointer to GW PS mode-specific data.
@param[in] ip_orig_params_ptr Pointer to IP PS mode-specific data.
@param[in] switch_out_call_id    Switch out call ID.
@param[in] orig_client_categ     Initiating client category.
@param[in] asubs_id              Subscription id on which the call should go
@param[out] return_call_id_ptr   Call ID will be allocated and returned.

@return 
  Switch_in Call_id will be returned in switch_in_call_id_ptr.
  - TRUE -- request sent.
  - FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_orig_cc_per_subs_exten(

  cm_call_cmd_exten_cb_f_type       cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_call_type_e_type               call_type,
  cm_srv_type_e_type                srv_type,
  const cm_num_s_type               *calling_number,
  const cm_num_s_type               *called_number,
  const cm_orig_alpha_s_type        *alpha_buf,
  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
  const cm_ip_orig_params_s_type  *ip_orig_params_ptr,
  cm_call_id_type                   switch_out_call_id,
  cm_orig_client_categ_e_type       orig_client_categ,
  const sys_modem_as_id_e_type      asubs_id,
  cm_call_id_type                   *return_call_id_ptr

);
/*~ FUNCTION cm_mm_call_cmd_orig_cc_per_subs */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM ip_orig_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_activate_cc_exten_per_subs

===========================================================================*/
/**
  Sends an Activate Supplementary Services request. This is a common function
  that can be used to activate different Supplementary Services. The client ID
  should be from a registered CM client.

  Clients must specify the initiating client category and provide an
  appropriate callback function pointer. Clients must also check if
  CM_API_MODEM_CALL_CONTROL is defined before calling this API.

  The client-supplied callback function sups_cmd_cb_func will be called to
  notify the client of the command status. If the command request is
  processed without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the
  client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] act_cmd_params    Pointer to activate SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.
@param[in] is_call_control_processing_complete CC complete?.
@param[in] subscription_id   Subs id on which call has to go.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_activate_cc_exten_per_subs(

  cm_sups_cmd_exten_cb_f_type    sups_cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id,
  const cm_act_ss_params_s_type  *act_cmd_params,
  const cm_num_s_type            *dialed_digits,
  cm_orig_client_categ_e_type    orig_client_categ,
  boolean                        is_call_control_processing_complete,
  sys_modem_as_id_e_type         subscription_id

);
/*~ FUNCTION cm_sups_cmd_activate_cc_exten_per_subs */
/*~ PARAM act_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_register_cc_exten_per_subs

===========================================================================*/
/**
  Registers a SUPS command with the CM.

  This is a common function that can be used to register different Supplementary
  Services. The client ID should be from a registered CM client.

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide an
  appropriate callback function pointer.

  The client-supplied callback function sups_cmd_cb_func will be called to
  notify the client of the command status. If the command request is
  processed without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the
  client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] reg_cmd_params    Pointer to register SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.
@param[in] is_call_control_processing_complete CC complete?.
@param[in] subscription_id   Subs id on which call has to go.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_register_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type    sups_cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id,
  const cm_reg_ss_params_s_type  *reg_cmd_params,
  const cm_num_s_type            *dialed_digits,
  cm_orig_client_categ_e_type    orig_client_categ,
  boolean                        is_call_control_processing_complete,
  sys_modem_as_id_e_type         subscription_id

);
/*~ FUNCTION cm_sups_cmd_register_cc_exten_per_subs */
/*~ PARAM reg_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_reg_password_cc_exten_per_subs

===========================================================================*/
/**
  Sends a Register Password Supplementary Services request to the CM.

  This function is called to register a password for a Supplementary Service
  (e.g., register a password for call restriction service) or to set a new
  password (overwrite the existing password with a new password). The client ID
  should be from a registered CM client. 

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling
  this API. Clients must also specify the initiating client category and provide
  an appropriate callback function pointer.  
  
  The client-supplied callback function sups_cmd_cb_func will be called to
  notify the client of the command status. If the command request is
  processed without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the
  client.

@param[in] sups_cmd_cb_func   Client callback function.
@param[in] data_block_ptr     Pointer to the client callback data block.
@param[in] client_id          Originating client.
@param[in] reg_pwd_cmd_params Pointer to register password SUPS command data.
@param[in] dialed_digits      Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ  Initiating client category.
@param[in] is_call_control_processing_complete CC complete?.
@param[in] subscription_id   Subs id on which call has to go.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_reg_password_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type     sups_cmd_cb_func,
  void                            *data_block_ptr,
  cm_client_id_type               client_id,
  const cm_reg_pwd_params_s_type  *reg_pwd_cmd_params,
  const cm_num_s_type             *dialed_digits,
  cm_orig_client_categ_e_type     orig_client_categ,
  boolean                         is_call_control_processing_complete,
  sys_modem_as_id_e_type          subscription_id

);
/*~ FUNCTION cm_sups_cmd_reg_password_cc_exten_per_subs */
/*~ PARAM reg_pwd_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_process_uss_exten_per_subs

===========================================================================*/
/**
  Processes Unstructured SS commands to the CM.

  This command sends a mobile-initiated Unstructured Supplementary Services Data
  request. It enables the client to send a Process Unstructured Supplementary
  Services request to the CM. This function is used to originate an Unstructured
  SS Data operation by the mobile station. 

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide an
  appropriate callback function pointer.  

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client. 

@param[in] sups_cmd_cb_func       Client callback function.
@param[in] data_block_ptr         Pointer to the client callback data block.
@param[in] client_id              Originating client.
@param[in] process_uss_cmd_params Pointer to process Unstructured SUPS command
                                  data.
@param[in] dialed_digits          Pointer to a cm_num_s_type of the digits 
                                  dialed.
@param[in] orig_client_categ      Initiating client category.
@param[in] is_call_control_processing_complete CC complete?.
@param[in] subscription_id        Subs id on which call has to go.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_process_uss_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type    sups_cmd_cb_func,
  void                           *data_block_ptr,
  cm_client_id_type              client_id,
  const cm_uss_params_s_type     *process_uss_cmd_params,
  const cm_num_s_type            *dialed_digits,
  cm_orig_client_categ_e_type    orig_client_categ,
  boolean                        is_call_control_processing_complete,
  sys_modem_as_id_e_type         subscription_id

);
/*~ FUNCTION cm_sups_cmd_process_uss_exten_per_subs */
/*~ PARAM process_uss_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_deactivate_cc_exten_per_subs

===========================================================================*/
/**
  Deactivates a SUPS command to the CM.

  This function enables the client to send a Deactivate Supplementary 
  Services request to the CM. This is a common function that can be used to 
  deactivate different Supplementary Services. The client ID should be from a
  registered CM client.

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients also must specify the initiating client category and provide
  an appropriate callback function pointer. 

  The client-supplied callback function cmd_cb_func will be called to notify the
  client of the command status. If the command request is processed without
  errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] deact_cmd_params  Pointer to erase SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.
@param[in] is_call_control_processing_complete CC complete?.
@param[in] subscription_id   Subs id on which call has to go.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_deactivate_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type       sups_cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const cm_deact_ss_params_s_type   *deact_cmd_params,
  const cm_num_s_type               *dialed_digits,
  cm_orig_client_categ_e_type       orig_client_categ,
  boolean                           is_call_control_processing_complete,
  sys_modem_as_id_e_type            subscription_id

);
/*~ FUNCTION cm_sups_cmd_deactivate_cc_exten_per_subs */
/*~ PARAM deact_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_interrogate_cc_exten_per_subs

===========================================================================*/
/**
  Interrogates a SUPS command to the CM.

  This function enables the client to send an Interrogate Supplementary 
  Services request to the CM. The status of a Supplementary Service can be 
  checked using this command. The client ID should be from a registered 
  CM client. 

  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide
  an appropriate callback function pointer. 
  
  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client. 

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] interr_cmd_params Pointer to interrogate SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.
@param[in] is_call_control_processing_complete CC complete?.
@param[in] subscription_id   Subs id on which call has to go.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_interrogate_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type        sups_cmd_cb_func,
  void                               *data_block_ptr,
  cm_client_id_type                  client_id,
  const cm_interr_ss_params_s_type   *interr_cmd_params,
  const cm_num_s_type                *dialed_digits,
  cm_orig_client_categ_e_type        orig_client_categ,
  boolean                            is_call_control_processing_complete,
  sys_modem_as_id_e_type             subscription_id

);
/*~ FUNCTION cm_sups_cmd_interrogate_cc_exten_per_subs */
/*~ PARAM interr_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/*==========================================================================

FUNCTION: cm_sups_cmd_erase_cc_exten_per_subs

===========================================================================*/
/**
  Erases a SUPS command to the CM.

  This function enables the client to send an Erase Supplementary Services
  request to the CM. This is a common function that can be used to erase
  a previous registration of a Supplementary Service. The client ID should
  be from a registered CM client.
 
  Clients must check if CM_API_MODEM_CALL_CONTROL is defined before calling this
  API. Clients must also specify the initiating client category and provide
  an appropriate callback function pointer. 

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status. If the command request is processed
  without errors, CM_SUPS_CMD_ERR_NOERR is passed back to the client.

@param[in] sups_cmd_cb_func  Client callback function.
@param[in] data_block_ptr    Pointer to the client callback data block.
@param[in] client_id         Originating client.
@param[in] erase_cmd_params  Pointer to erase SUPS command data.
@param[in] dialed_digits     Pointer to a cm_num_s_type of the digits dialed.
@param[in] orig_client_categ Initiating client category.
@param[in] is_call_control_processing_complete CC complete?.
@param[in] subscription_id   Subs id on which call has to go.

@return 
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called. \n
  FEATURE_WCDMA or FEATURE_GSM must be defined.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_sups_cmd_erase_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type       sups_cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  const cm_erase_ss_params_s_type   *erase_cmd_params,
  const cm_num_s_type               *dialed_digits,
  cm_orig_client_categ_e_type       orig_client_categ,
  boolean                           is_call_control_processing_complete,
  sys_modem_as_id_e_type            subscription_id

);

/*===========================================================================

FUNCTION: cm_mm_call_cmd_get_call_info_list_per_subs

===========================================================================*/
/**
  Gets a snapshot of the state of all call objects maintained by the CM. This is
  a synchronous function. The client ID should be from a registered CM client.
  The client must register for CM_CALL_EVENT_INFO to receive a response to
  this command. The CM_CALL_EVENT_INFO is sent only to the client that issues
  this command.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Requesting client ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  The CM must have already been initialized with cm_init_before_task_start() and
  cm_init_after_task_start().

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
extern boolean cm_mm_call_cmd_get_call_info_list_per_subs(

  cm_call_cmd_cb_f_type  cmd_cb_func,
  void                   *data_block_ptr,
  cm_client_id_type      client_id,
  sys_modem_as_id_e_type asubs_id

);

/*~ FUNCTION cm_sups_cmd_erase_cc_exten_per_subs */
/*~ PARAM erase_cmd_params POINTER */
/*~ PARAM dialed_digits POINTER */

/** @} */ /* endgroup CALL API for Dual Standby */


/*==========================================================================

FUNCTION: cm_stats_set_modem_info_mask_per_subs

==========================================================================*/
/**
  Commands the CM to change the Modem Statistics information bitmask per subscription.

  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

@param[in] cmd_cb_func    Client callback function.
@param[in] data_block_ptr Pointer to the client callback data block.
@param[in] client_id      Originating client.
@param[in] module_type    Module for which the mask is being set.
@param[in] bit_mask       Bitmask value.
@param[in] subs_id        Subs id on which the mask is applicable.

@dependencies
  cm_init_before_task_start() must have already been called.

@return 
  TRUE -- request sent. \n
  FALSE -- No buffer to send request

@sideeffects
  If the command is successful, CM clients will be notified of the
  corresponding CM event.
*/
extern boolean cm_stats_set_modem_info_mask_per_subs (

  cm_stats_cmd_cb_f_type            cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  sys_modem_module_e_type           module_type,
  uint64                            bit_mask,
  sys_modem_as_id_e_type            subs_id

);
/*~ FUNCTION cm_stats_set_modem_info_mask_per_subs */




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CM_DUALSIM_H */

