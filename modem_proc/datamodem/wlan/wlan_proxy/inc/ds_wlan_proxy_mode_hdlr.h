#ifndef DS_WLAN_PROXY_MODE_HDLR_H
#define DS_WLAN_PROXY_MODE_HDLR_H
/*===========================================================================

                      DS_WLAN_PROXY_MODE_HDLR.H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/wlan/wlan_proxy/inc/ds_wlan_proxy_mode_hdlr.h#1 $
  $Author: mplcsds1 $ 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/06/14    vl     Added support for state based setup timer values
12/11/14    op     Changes to support RM resync state
10/16/14    fn     Added support for thermal mitigation
05/19/14    ab     Added functions to call ps_sys apis for mac addr and ssid change
01/21/14    vl     Added Critical Section
12/19/13    ab     Added a timer value in WLAN CB 
11/07/12    SC     Created Module
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "dsutil.h"
#include "ps_iface_defs.h"
#include "ps_iface.h"
#include "ps_iface_ioctl.h"
#include "ps_acl.h"
#include "stm2.h"
#include "ds_wlan_proxy_mh_sm_defs.h"
#include "ps_sys_conf.h"
#include "ds_wlan_util.h"

#define DS_MAX_WLAN_PROXY_INSTS       2
#define DS_WLAN_PROXY_V4_INSTANCE_ID  0
#define DS_WLAN_PROXY_V6_INSTANCE_ID  1


typedef struct __ds_wlan_proxy_cb_s ds_wlan_proxy_cb_type;

struct __ds_wlan_proxy_cb_s
{
  ps_iface_type             local_iface;
  acl_type                  local_out_acl;
  ps_phys_link_type         dummy_phys_link;

  /*TODO: This instance id is added since we will have two instances one for v4 
    and one for v6. Check if this is really needed*/
  /* Instance id, one for V4 and one for V6*/
  uint8                     instance_id;

  /* Associated WLAN PROXY MH SM pointer*/
  stm_state_machine_t       * wlan_proxy_mh_sm_ptr;
  /*-------------------------------------------------------------------------
  
  -------------------------------------------------------------------------*/
  ds_wlan_proxy_cb_type   * this_wlan_proxy_cb_ptr;
  void                    * iface_up_buf;
  void                    * iface_down_buf;

  /*This is to store the timer value read from EFS*/
  uint32                  wifi_oos_linger_timer;

  /*This is to store the timer value read from EFS*/
  uint32                  bringup_retry_timer;

  /*This is to store the timer value read from EFS*/
  uint32                  wlan_proxy_sio_config_timer;

  /*This is to store the timer value read from EFS*/
  uint32                  wlan_proxy_rm_config_timer;
 
  /*-------------------------------------------------------------------------
  Store the acl_policy information
  -------------------------------------------------------------------------*/
  struct
  {
    boolean                    acl_info_valid;
    acl_policy_info_type       acl_policy_info;
  }acl_info;

  /*-------------------------------------------------------------------------
  Flag to indicate WLAN available event occurred. Currently set only in
  RM_CONFIG state
  -------------------------------------------------------------------------*/
  boolean             wlan_avail_event;
};

/*---------------------------------------------------------------------------
   Global Critical Section Macros
---------------------------------------------------------------------------*/
#define DS_WLAN_PROXY_GLOBAL_CRITICAL_SECTION_ENTER() \
  rex_enter_crit_sect(ds_wlan_proxy_get_global_crit_sect());

#define DS_WLAN_PROXY_GLOBAL_CRITICAL_SECTION_LEAVE() \
  rex_leave_crit_sect(ds_wlan_proxy_get_global_crit_sect());

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/*===========================================================================
MACRO         DS707_PDNCNTX_CB_IS_VALID

DESCRIPTION   This macro returns a boolean indicating whether the pdncntx_cb 
              ptr passed in is valid.

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdncntx_cb in question.

RETURN VALUE  TRUE: if is valid 
              FALSE: otherwise.
===========================================================================*/
#define DS_WLAN_PROXY_CB_IS_VALID(ds_wlan_proxy_cb_ptr)              \
  ((ds_wlan_proxy_cb_ptr != NULL) &&                                 \
  (ds_wlan_proxy_cb_ptr->this_wlan_proxy_cb_ptr == ds_wlan_proxy_cb_ptr))       

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_MODE_HDLR_INIT

DESCRIPTION   Initializes the wlan proxy mode handler module

PARAMETERS    None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_wlan_proxy_mode_hdlr_init(void);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_MODE_HDLR_DEINIT

DESCRIPTION   Cleanup the wlan proxy mode handler module

PARAMETERS    None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION  ds_wlan_proxy_process_nv_refresh_event()

DESCRIPTION  Process NV Refresh event
  
PARAMETERS  
  ds_wlan_nv_refresh_event_info_type* nv_refresh_event_info_ptr.

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None. 
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
void ds_wlan_proxy_process_nv_refresh_event
(
  ds_wlan_nv_refresh_event_info_type* nv_refresh_event_info_ptr
);
#endif

/*===========================================================================

FUNCTION ds_wlan_proxy_get_global_crit_sect

DESCRIPTION
  returns the global critical section

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  rex_crit_sect_type* - Pointer to global critical section

SIDE EFFECTS
  None

===========================================================================*/
rex_crit_sect_type* ds_wlan_proxy_get_global_crit_sect
(
  void
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_GET_CB_PTR_FROM_IFACE_PTR

DESCRIPTION   Given an iface ptr it returns a ptr to control block it is 
              attached to. If it doesnt belong to any cb then returns NULL 

PARAMETERS    ps_iface_type * : pointer to iface

RETURN VALUE  pointer wlan proxy cb if valid 
              NULL if invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds_wlan_proxy_cb_type* ds_wlan_proxy_get_cb_ptr_from_iface_ptr(
  ps_iface_type* iface_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_PROCESS_WLAN_RAT_CHANGE

DESCRIPTION   This function processes dsd change in WLAN availability or IP 
              addresses 

PARAMETERS    void

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_wlan_rat_change(void);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_CB_HDLR_PROCESS_BRINGUP

DESCRIPTION   This function process the cmd to bring up the control block 
              instance. 

PARAMETERS    ds_wlan_proxy_cb_type* - Ptr to the wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_bringup
(
	 ds_wlan_proxy_cb_type*  instance_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_CB_HDLR_PROCESS_TEARDOWN

DESCRIPTION   This function process the cmd to tear down the control block 
              instance. 

PARAMETERS    ds_wlan_proxy_cb_type* - Ptr to the wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_teardown
(
	 ds_wlan_proxy_cb_type*  instance_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_PROCESS_REV_IP_XPORT_SIO_CONFIG_COMPLETE

DESCRIPTION   This function process SIO configuration complete for reverse 
              SIO port for this IFACE. 

PARAMETERS    ds_wlan_proxy_cb_type* - Ptr to the wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_rev_ip_xport_sio_config_complete
(
  ds_wlan_proxy_cb_type*  instance_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_PROCESS_REV_IP_XPORT_WDS_CONFIG_COMPLETE

DESCRIPTION   This function process wds configuration complete for reverse 
              SIO port for this IFACE. 

PARAMETERS    ds_wlan_proxy_cb_type* - Ptr to the wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_rev_ip_xport_wds_config_complete
(
  ds_wlan_proxy_cb_type*  instance_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_PROCESS_THERMAL_MIT_TEARDOWN

DESCRIPTION   This function teardowns wlan_proxy iface gracefully for
              thermal mitigation

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_thermal_mit_teardown
(
  void* action
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_PROCESS_RM_DOWN

DESCRIPTION   This function processes RM down

PARAMETERS    ds_wlan_proxy_cb_type* - Ptr to the wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_rm_down
(
  ds_wlan_proxy_cb_type*  instance_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_PROCESS_WLAN_AVAILABLE

DESCRIPTION   This function processes WLAN available

PARAMETERS    ds_wlan_proxy_cb_type* - Ptr to the wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_wlan_available
(
  ds_wlan_proxy_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      ds_wlan_proxy_wlan_oos_linger_timer_cb

DESCRIPTION   This is a timer callback function which handles the expiry of
              wlan unavail timer

PARAMETERS    user_data - wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Will call tear down event
===========================================================================*/
void ds_wlan_proxy_wlan_oos_linger_timer_cb
(
  unsigned long user_data
);

/*===========================================================================
FUNCTION      ds_wlan_proxy_bringup_retry_timer_cb

DESCRIPTION   This is a timer callback function which handles the expiry of
              bringup retry timer. Upon expiry, wlan proxy will reprocess
              wlan availability event. That means, if wifi is available, it
              will bring up wlan proxy iface again. 

PARAMETERS    user_data - wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_bringup_retry_timer_cb
(
  unsigned long user_data
);

/*===========================================================================
FUNCTION      ds_wlan_proxy_call_setup_timer_cb

DESCRIPTION   This is a timer callback function which handles the expiry of
              call setup timer. Upon expiry, wlan proxy will post the 
              IFACE teardown event. 

PARAMETERS    user_data - wlan proxy cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_call_setup_timer_cb
(
  unsigned long user_data
);

/*===========================================================================
FUNCTION  ds_wlan_proxy_compare_ip_addr

DESCRIPTION   Returns TRUE if the two addresses are same and FALSE otherwise

PARAMETERS
  ps_ip_addr_type*  old_ip_addr
  ps_ip_addr_type*  new_ip_addr
  uint16*           errno_ptr

DEPENDENCIES  None

RETURN VALUE  TRUE  - Same IP addresses 
              FALSE - Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds_wlan_proxy_compare_ip_addr
(
  ps_ip_addr_type*  old_ip_addr,
  ps_ip_addr_type*  new_ip_addr,
  int16*            errno_ptr  
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_SET_MAC_ADDR

DESCRIPTION   This function stores the mac address to the iface using ps_sys 
              API 

PARAMETERS    ps_iface_type                         *iface_ptr   - iface ptr
              void*                                 mac_addr_ptr - mac addr

DEPENDENCIES  None

RETURN VALUE  boolean TRUE - if successful 
                      FALSE - otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_wlan_proxy_set_mac_addr
(
   ps_iface_type*   iface_ptr,
   void*            mac_addr_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_SET_SSID

DESCRIPTION   This function stores the ssid in the cache using ps_sys 
              API 

PARAMETERS    void*                          ssid 
              int16*                         ps_error 
              
DEPENDENCIES  None

RETURN VALUE  boolean TRUE - if successful 
                      FALSE - otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_wlan_proxy_set_ssid
(
   void*   ssid,
   int16*  ps_error
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_DSD_COMPARE_MAC_ADDR

DESCRIPTION  

PARAMETERS    mac_addr1_ptr, mac_addr2_ptr : Pointers to mac addresses
                                             to be compared.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_wlan_proxy_compare_mac_addr
(
  uint8*   mac_addr1_ptr,
  uint8*   mac_addr2_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_DSD_COMPARE_ssid

DESCRIPTION  

PARAMETERS    char* old_ssid
              char* new_ssid

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_wlan_proxy_dsd_compare_ssid
(
  char*   old_ssid,
  char*   new_ssid
);

/*===========================================================================
FUNCTION DS_WLAN_PROXY_DSD_IF_GET_MAC_ADDR()

DESCRIPTION
  This function returns MAC ADDR

PARAMETERS
  None

RETURN VALUE
  boolean - true if valid MAC Addr 
            false if invalid MAC Addr

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_wlan_proxy_get_mac_addr
(
  uint8*   mac_addr_ptr
);

/*===========================================================================
FUNCTION DS_WLAN_PROXY_GET_V4_IFACE()

DESCRIPTION
  This function returns V4 WLAN PROXY IFACE

PARAMETERS
  None

RETURN VALUE
  ps_iface_type *

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type* ds_wlan_proxy_get_v4_iface (void);

/*===========================================================================
FUNCTION DS_WLAN_PROXY_GET_V6_IFACE()

DESCRIPTION
  This function returns V6 WLAN PROXY IFACE

PARAMETERS
  None

RETURN VALUE
  ps_iface_type *

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type* ds_wlan_proxy_get_v6_iface (void);

/*===========================================================================
FUNCTION DS_WLAN_PROXY_PROCESS_DSD_INFO()

DESCRIPTION
  This function processes the information received in the dsd cb function

PARAMETERS
  void*    dsd_cback_info_ptr - Information received in the dsd cb function

RETURN VALUE 
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_wlan_proxy_process_dsd_info
(
  void*    dsd_cback_info_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_GENERATE_DS3G_WLAN_INFO_CHG_EV

DESCRIPTION   This function generates the ds3g event to notify its clients of what 
              has changed. The parameter indicates what all has changed and the fn
              get corresponding info from global cache and generates the event.

PARAMETERS    uint32  wlan_info_chg_mask

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_generate_ds3g_wlan_info_chg_ev
(
  void
);

#endif /*FEATURE_DATA_WLAN_MAPCON*/

#endif /*DS_WLAN_PROXY_MODE_HDLR_H*/
