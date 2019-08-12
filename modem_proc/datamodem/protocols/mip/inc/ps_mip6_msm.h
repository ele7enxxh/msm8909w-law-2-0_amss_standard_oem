#ifndef PS_MIP6_MSM_H
#define PS_MIP6_MSM_H
/*===========================================================================

                           P S _ M I P 6 _ M S M . H
                   
DESCRIPTION
  The header file for MIP6 Meta State Machine.

EXTERNALIZED FUNCTIONS

Copyright (c) 2007-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/ps_mip6_msm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#ifdef FEATURE_DATA_PS_MIPV6
#include "ps_iface.h"
#include "ps_acl.h"
#include "ps_in.h"

/*===========================================================================
  Forward declarations
===========================================================================*/
struct ps_mip6_msm_cb_s;

typedef struct ps_mip6_msm_cb_s ps_mip6_msm_type;

/*===========================================================================
  Constants 
===========================================================================*/
#define MIP6_MAX_IFACES 1

#define MIP6_MAX_BINDINGS 1
#define MIP6_MAX_HA_ADDRS 2

#define MIP6_MAX_HOME_PREFIX_LEN     64
#define MIP6_DEFAULT_HOME_PREFIX_LEN MIP6_MAX_HOME_PREFIX_LEN

/*===========================================================================
  Declarations
===========================================================================*/
typedef enum
{
  MIP6_MIN_STATE                = 0,
  MIP6_DOWN                     = 0,
  MIP6_SIP6_IFACE_CONFIGURING   = 1,
  MIP6_BOOTSTRAPPING            = 2,
  MIP6_IPSEC_TPORT_CONFIGURING  = 3,
  MIP6_IPSEC_TUNNEL_CONFIGURING = 4,
  MIP6_REGISTERING              = 5,
  MIP6_UP                       = 6,
  MIP6_HANDOFF_REG              = 7,
  MIP6_DEREGISTERING            = 8,
  MIP6_GOING_DOWN               = 9,
  MIP6_MAX_STATE
} ps_mip6_state_type;

typedef enum
{
  MIP6_MSM_MIN_EV                      = 0,
  MIP6_MSM_START_EV                    = 0,
  MIP6_MSM_BOOTSTRAP_SUCCESS_EV        = 1,
  MIP6_MSM_BOOTSTRAP_FAIL_EV           = 2,
  MIP6_MSM_SIP6_IFACE_UP_EV            = 3,
  MIP6_MSM_SIP6_IFACE_UNAVAILABLE_EV   = 4,
  MIP6_MSM_SIP6_IFACE_CONFIGURING_EV   = 5,
  MIP6_MSM_SIP6_IFACE_PREFIX_UPDATE_EV = 6,
  MIP6_MSM_IPSEC_TPORT_IFACE_UP_EV     = 7,
  MIP6_MSM_IPSEC_TPORT_IFACE_DOWN_EV   = 8,
  MIP6_MSM_IPSEC_TUNNEL_IFACE_UP_EV    = 9, 
  MIP6_MSM_IPSEC_TUNNEL_IFACE_DOWN_EV  = 10,
  MIP6_MSM_RSM_UP_EV                   = 11,
  MIP6_MSM_RSM_DOWN_EV                 = 12,
  MIP6_MSM_CLOSE_EV                    = 13,  
  MIP6_MSM_MAX_EV
} ps_mip6_msm_event_type;

#ifdef FEATURE_SEC_IPSEC 
typedef struct
{ 
  ps_iface_type *iface_ptr;               /* IPsec iface handle            */
  void          *esp_sa_ptr;              /* Pointer to the associated SA  */

  void          *iface_up_cbuf;           /* Iface UP ev buf               */
  void          *iface_down_cbuf;         /* Iface DOWN ev buf             */
} mip6_ipsec_sa_type;
#endif /* FEATURE_SEC_IPSEC */

struct ps_mip6_msm_cb_s 
{
  ps_mip6_state_type   state;               /* Current MSM state          */

  boolean              initial_registration;/* First reg attempt with new 
                                             bootstrap info               */

  struct ps_in6_addr   ha_addrs[MIP6_MAX_HA_ADDRS];/* List of available HAs */
  uint16               curr_ha_index;       /* Index of currently used HA   */
  uint16               home_prefix_len;     /* Length of Home prefix        */
  boolean              visited_nw_info;   /* HA info is from visited network*/

  ps_iface_type        mip6_logical_iface;  /* MIPv6 logical iface handle   */
  struct ps_in6_addr   home_addr;           /* Home address                 */

  acl_policy_info_type req_policy_info;     /* Requested policy info        */

  struct
  {
    ps_iface_type      *iface_ptr;        /* physical iface handle         */
    void               *iface_up_cbuf;    /* Iface UP ev buf               */
    void               *iface_down_cbuf;    /* Iface DOWN ev buf           */
    void               *iface_configuring_cbuf;/* Iface configuring ev buf */
    void               *iface_extended_ip_config_cbuf;/* extended ip cfg buf */
    void               *iface_prefix_update_cbuf;/* Iface prefix update ev buf*/
    void               *iface_flow_enabled_cbuf; /* Iface flow enabled ev buf*/
    void               *iface_flow_disabled_cbuf;/* Iface flow disabled ev buf*/
  } sip6_iface;

#ifdef FEATURE_SEC_IPSEC 
  boolean            ipsec_on_payload;

  /* MIP6 EAP-AKA credential ???*/
  mip6_ipsec_sa_type ipsec_tport_sa;/* IPsec transport SA info for signaling */
  mip6_ipsec_sa_type ipsec_tun_sa;  /* IPsec tunnel SA for bearer traffic  */
#endif /* FEATURE_SEC_IPSEC */

  list_type          binding_list; /* Associated RSM's  */
  
  ps_mip6_msm_type   *this_msm_ptr;       /* self pointer                  */
};

extern ps_mip6_msm_type mip6_msm_instance[MIP6_MAX_IFACES];
/*===========================================================================
  Macros
===========================================================================*/
#define MIP6_INST_INITIALIZED(inst_ptr) ((inst_ptr != NULL) && \
          (inst_ptr->this_msm_ptr==inst_ptr))

/*===========================================================================
  External function declarations
===========================================================================*/
/*===========================================================================
FUNCTION  PS_MIP6_INIT()

DESCRIPTION
  Initialize MIP6 related data strcutures.

DEPENDENCIES
  IPsec ifaces must have been created and initialized.

RETURN VALUE
  0 on Success.
  1 on Failure.

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_init
(
  void
);

/*===========================================================================
FUNCTION 
  PS_MIP6_MSM_POST_EVENT

DESCRIPTION
  This functions posts the MIP6 MSM events in the DS command queue to be
  handled by MSM later when scheduled.

DEPENDENCIES

RETURN VALUE
  -1 on failure
  0  on success

SIDE EFFECTS

===========================================================================*/
void ps_mip6_msm_post_event
(
  ps_mip6_msm_type       *instance,
  ps_mip6_msm_event_type event,
  void                   *event_data
);

/*===========================================================================
FUNCTION  PS_MIP6_CONFIG_INITED()

DESCRIPTION
  This function is used to indicate if MIPv6 configuration has been
  initialized successfully - this is relevant if MIPv6 is REQUIRED.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if MIPv6 is REQUIRED and configuration has not been initialized.
  TRUE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_mip6_config_inited
(
  void
);

/*int ps_mip6_update_tx_meta_info
(
  ps_mip6_msm_type  *mip6_msm_ptr,
  ps_meta_info_type *meta_info_ptr
);*/

#endif /* FEATURE_DATA_PS_MIPV6 */
#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6*/
#endif /* PS_MIP6_MSM_H */
