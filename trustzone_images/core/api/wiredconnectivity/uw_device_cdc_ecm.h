/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file uw_device_cdc_ecm.h

Contains the declarations of the Communication Device Class/Ethernet Control 
Model (CDC/ECM) function.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_device_cdc_ecm.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/23/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com) 
**************************************************************************/ 


#ifndef _UW_DEVICE_CDC_ECM_H_
#define _UW_DEVICE_CDC_ECM_H_

#include <uw_device_cdc.h>


/** @ingroup hsusb_jungo_api 
@{ */

/*
 * Host to Device - Requests
 */

/** MAC address size.
*/
#define MAC_ADDR_SIZE 6

/** MAC address.
*/
typedef struct
{
    char addr[MAC_ADDR_SIZE];   /**< 6-byte Device's MAC address. */

} mac_addr_t;

typedef jresult_t (*ecm_set_mc_filter_cb)(mac_addr_t *mc_filters,
    juint16_t nfilters, cdc_appctx_t ctx);

/** ECM filter.
*/
typedef struct
{
    juint16_t mask_size; /**< Mask size in bytes. */
    juint8_t *mask;      /**< Filter mask. */
    juint8_t *pattern;   /**< Pattern to search. */
} ecm_pm_filter_t;

typedef jresult_t (*ecm_set_pm_filter_cb)(juint16_t filter_id,
    juint16_t filter_size, ecm_pm_filter_t *pm_filter, cdc_appctx_t ctx);

typedef jresult_t (*ecm_get_pm_filter_cb)(juint16_t filter_id,
    jbool_t *is_active, cdc_appctx_t ctx);

/** ECM bitmask.
*/
enum ecm_pf_bitmask_t
{
    ECM_PF_PROMISCUOUS          = 0x0001,   /**< &nbsp; */
    ECM_PF_ALL_MULTICAST        = 0x0002,   /**< &nbsp; */
    ECM_PF_DIRECTED             = 0x0004,   /**< &nbsp; */
    ECM_PF_BROADCAST            = 0x0008,   /**< &nbsp; */
    ECM_PF_MULTICAST            = 0x0010    /**< &nbsp; */
};

typedef jresult_t (*ecm_set_pkt_filter_cb)(juint16_t pf_bitmask,
    cdc_appctx_t ctx);

/* HSU Fix: Adding support for SET_CONTROL_LINE_STATE on ECM */
typedef jresult_t (*ecm_set_control_line_state_cb)(juint16_t ctrl_sig_bitmap,
                                                   cdc_appctx_t ctx);
/* End of HSU fix*/

/** ECM statistics.
*/
typedef enum
{
    ECM_FS_NONE                 = 0x00000000L,   /**< &nbsp; */
    ECM_FS_XMIT_OK              = 0x00000001L,   /**< &nbsp; */
    ECM_FS_RCV_OK               = 0x00000002L,   /**< &nbsp; */
    ECM_FS_XMIT_ERR             = 0x00000004L,   /**< &nbsp; */
    ECM_FS_RCV_ERR              = 0x00000008L,   /**< &nbsp; */
    ECM_FS_RCV_NO_BUFFER        = 0x00000010L,   /**< &nbsp; */
    ECM_FS_DIRECTED_BYTES_XMIT  = 0x00000020L,   /**< &nbsp; */
    ECM_FS_DIRECTED_FRAMES_XMIT = 0x00000040L,   /**< &nbsp; */
    ECM_FS_MCAST_BYTES_XMIT     = 0x00000080L,   /**< &nbsp; */
    ECM_FS_MCAST_FRAMES_XMIT    = 0x00000100L,   /**< &nbsp; */
    ECM_FS_BCAST_BYTES_XMIT     = 0x00000200L,   /**< &nbsp; */
    ECM_FS_BCAST_FRAMES_XMIT    = 0x00000400L,   /**< &nbsp; */
    ECM_FS_DIRECTED_BYTES_RCV   = 0x00000800L,   /**< &nbsp; */
    ECM_FS_DIRECTED_FRAMES_RCV  = 0x00001000L,   /**< &nbsp; */
    ECM_FS_MCAST_BYTES_RCV      = 0x00002000L,   /**< &nbsp; */
    ECM_FS_MCAST_FRAMES_RCV     = 0x00004000L,   /**< &nbsp; */
    ECM_FS_BCAST_BYTES_RCV      = 0x00008000L,   /**< &nbsp; */
    ECM_FS_BCAST_FRAMES_RCV     = 0x00010000L,   /**< &nbsp; */
    ECM_FS_RCV_CRC_ERR          = 0x00020000L,   /**< &nbsp; */
    ECM_FS_XMIT_Q_LEN           = 0x00040000L,   /**< &nbsp; */
    ECM_FS_RCV_ERR_ALIGN        = 0x00080000L,   /**< &nbsp; */
    ECM_FS_XMIT_ONE_COLLISION   = 0x00100000L,   /**< &nbsp; */
    ECM_FS_XMIT_MULT_COLLISIONS = 0x00200000L,   /**< &nbsp; */
    ECM_FS_XMIT_DEFERRED        = 0x00400000L,   /**< &nbsp; */
    ECM_FS_XMIT_MAX_COLLISIONS  = 0x00800000L,   /**< &nbsp; */
    ECM_FS_RCV_OVERRUN          = 0x01000000L,   /**< &nbsp; */
    ECM_FS_XMIT_UNDERRUN        = 0x02000000L,   /**< &nbsp; */
    ECM_FS_XMIT_HB_FAILURE      = 0x04000000L,   /**< &nbsp; */
    ECM_FS_XMIT_CRS_LOST        = 0x08000000L,   /**< &nbsp; */
    ECM_FS_XMIT_LATE_COLLISIONS = 0x10000000L    /**< &nbsp; */
} ecm_enum_stats_t;

typedef jresult_t (*ecm_get_stats_cb)(ecm_enum_stats_t feature, juint32_t *count,
    cdc_appctx_t ctx);

/*
 * Device to Host - Notifications
 */

/** ECM connection spoed information.
*/
typedef struct
{
    juint32_t upstream_bitrate;     /**< Upstream bitrate. */
    juint32_t downstream_bitrate;   /**< Downstream bitrate. */
} ecm_conn_speed_t;

jresult_t ecm_notify_conn_speed_change(cdc_handle_t handle,
    ecm_conn_speed_t *conn_speed);

jresult_t ecm_notify_network_connection(cdc_handle_t handle, jbool_t link_up);

/*
 * Initialization
 */

/** ECM callbacks structure.
*/
typedef struct
{
    cdc_callbacks_t             basic;    /**< Basic CDC handler. */

    ecm_set_mc_filter_cb        set_multicast_filter_h;
      /**< ECM host-to-device request handler. */

    ecm_set_pm_filter_cb        set_power_filter_h;
      /**< ECM host-to-device request handler. */

    ecm_get_pm_filter_cb        get_power_filter_h;
      /**< ECM host-to-device request handler. */

    ecm_set_pkt_filter_cb       set_packet_filter_h;
      /**< ECM host-to-device request handler. */

    ecm_get_stats_cb            get_stats_h;
      /**< ECM host-to-device request handler. */

/* HSU Fix: Adding support for SET_CONTROL_LINE_STATE on ECM */
    ecm_set_control_line_state_cb       set_control_line_state_h;
      /**< ECM host-to-device request handler. */
/* End of HSU fix */

    cdc_io_complete_cb          get_power_filter_complete_h;
      /**< ECM request completion handler. */

    cdc_io_complete_cb          get_stats_complete_h;
      /**< ECM request completion handler. */

    cdc_io_complete_cb          notify_net_conn_complete_h;
      /**< ECM notification completion handler. */

    cdc_io_complete_cb          notify_speed_change_complete_h; 
      /**< ECM notification completion handler. */
   
} ecm_callbacks_t;

/** ECM initialization information.
*/
typedef struct
{
    cdc_init_info_t basic;            /**< CDC initialization information. */
#define MAC_ADDR_STR_SIZE 13          /**< Size of the MAC address when 
                                           represented as a string. */
    char mac_addr[MAC_ADDR_STR_SIZE]; /**< String: xxxxxxxxxxxx. */
    juint32_t eth_statistics;         /**< Filter
									       (defined in ecm_enum_stats_t). */
    juint8_t num_power_filters;       /**< Number of power filters. */
     
    /** Multicast filter configuration.
	*/
    struct
    {
        juint_t perfect_filtering : 1;  /**< Perfect filtering. */
        juint_t num_filters : 15;       /**< Number of filters. */
    } mc_filters;
} ecm_init_info_t;

jresult_t ecm_init(cdc_handle_t handle, ecm_callbacks_t *cbs,
    ecm_init_info_t *info, cdc_appctx_t *ctx);

/** @} */ /* end_group hsusb_jungo_api */
    
#endif
