#ifndef DSHDR_PDN_CONTEXT_H
#define DSHDR_PDN_CONTEXT_H

/*===========================================================================

          

DESCRIPTION
  This module is responsible for starting/stopping Adaptive Receive Diversity
  Algorithm (ARDA). 
  
Copyright (c) 2010-2014 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pdn_context.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:03 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/14   jc      Added new iface down masks.
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
05/02/12   jz      Set ip_ver_capability according to profile specifies
10/28/11   ls      PDN throttle info notification through SYS IOCTL
12/07/11   vpk     Add support for 5 PDNs
07/21/11   vpk     Fix for closing other interfaces during DUN call bringup
06/06/11   ash     Added support for PPP Partial Context
03/15/11   mg      Global variable cleanup
07/18/10   mg      PDN throttling feature for eHRPD
05/13/10   ssh     Added support for PDN Inactivity timer
03/12/10   sc      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_sys.h"

#include "ds707_data_session_profile.h"
#include "dstask_v.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ds707_sys_chg_hdlr.h"
#include "dsgen_iface6_hdlr.h"
/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/

typedef struct ds707_iface_table_type_s ds707_iface_table_type;
typedef struct ds707_pdn_context_cb_type_s ds707_pdn_context_cb_type;
typedef struct ds707_pdn_context_cb_ptr_type_s ds707_pdn_context_cb_ptr_type;

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
#define DS707_PDNCNTX_MAX_EHRPD_APN DATA_SESSION_EHRPD_NO_VALID_PROFILE
typedef struct ds707_pdn_throttle_info_type_s ds707_pdn_throttle_info_type;
typedef struct ds707_pdn_throttle_info_ptr_type_s ds707_pdn_throttle_info_ptr_type;
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
/*===========================================================================
                          VARIABLES AND MACRO DEFINITIONS
==========================================================================*/

/*---------------------------------------------------------------------------
  Maximum number of pkt data calls supportable by IS707/IS2000 packet data
  module.  
---------------------------------------------------------------------------*/  
#ifdef FEATURE_EHRPD
   /* 8 PDNs, need to be same as MAX_IFACES */
   #define DS707_MAX_PDN_INSTANCES                        (PS_SYS_MAX_AVAIL_PDNS*2)
  #else
  #ifdef FEATURE_DATA_PS_IPV6
    #define DS707_MAX_PDN_INSTANCES                        2
  #else
    #define DS707_MAX_PDN_INSTANCES                        1
  #endif
#endif

#define DS707_MAX_IFACE_PER_PDN_CONTEXT                    2 /* IPv4, IPv6 */
#define DS707_PDN_CONTEXT_IPV4_IFACE                       0 /* IPv4 */
#define DS707_PDN_CONTEXT_IPV6_IFACE                       1 /* IPv6 */

#ifdef FEATURE_EHRPD
  /* 8 PDNs * 2 ifaces per PDN */
  #define DS707_MAX_IFACES                                 (PS_SYS_MAX_AVAIL_PDNS*2)
#else
  #define DS707_MAX_IFACES                                  2
#endif

typedef enum
{
  DS707_PDNCNTX_LINK_UP     = 0,
  DS707_PDNCNTX_LINK_DOWN   = 1,
  DS707_PDNCNTX_PPP_CLOSE   = 2, 
  DS707_PDNCNTX_INVALID_ACTION
} ds707_pdncntx_ppp_event_action;

/*---------------------------------------------------------------------------
  Structure to store iface related information.
---------------------------------------------------------------------------*/
struct ds707_iface_table_type_s
{
  ds707_iface_table_type*     this_iface_tbl_entry;
  ps_iface_type               ps_iface;
  acl_type                    acl;
#ifdef FEATURE_DATA_IS707
  dssnet4_sm_cb_type          dssnet4_sm_cb;
#endif /* FEATURE_DATA_IS707 */
#ifdef FEATURE_DATA_PS_IPV6
  dssnet6_sm_cb_type          dssnet6_sm_cb;
  ip6_sm_type                 ip6_sm;
#endif
  rex_timer_type              conn_delay_timer;
  void*                       iface_up_buf_ptr;
  void*                       iface_down_buf_ptr;
  void*                       iface_routable_buf_ptr;
  ds707_pdn_context_cb_type*  pdn_cntx_ptr; /*pointer to pdn_cb when attached to a PDN*/
};

typedef enum
{
  DS707_PDNCNTX_IFACE_DOWN_NONE = 0,
  DS707_PDNCNTX_IFACE_DOWN_V4   = 1,
  DS707_PDNCNTX_IFACE_DOWN_V6   = 2
} ds707_pdn_context_iface_down_mask;

/*---------------------------------------------------------------------------
  Structure to store PDN context control block
---------------------------------------------------------------------------*/
struct ds707_pdn_context_cb_type_s
{
  ds707_pdn_context_cb_type*  this_pdn;
  ds707_iface_table_type*     ds707_iface_tbl[DS707_MAX_IFACE_PER_PDN_CONTEXT];
#ifdef FEATURE_EHRPD
  ppp_ncp_handle_type         ncp_handle;
  boolean                     ncp_started;
  boolean                     ncp_resync_started;
  rex_timer_type              inactivity_timer;

  /* Flag to indicate call bring up in tunnel mode*/
  boolean                     call_bringup_in_tunnel_mode; 
#endif
  ds707_data_session_info_type 
                              data_profile;
  boolean                     ppp_resync_lock;
#ifdef FEATURE_EPC_HANDOFF
  void                       *handover_ip_config_handle;/*ip config handover*/
  boolean                    handover_attach;  /* flag to identify handover*/
#endif
#ifdef FEATURE_CALL_THROTTLE
  /* flag to indicate if throttle indication is sent to CM thru DCTM. This is
     needed, since both dssnet get the failure with reason in PPP down, but only
     one indication should be given to DCTM */
  boolean                   dctm_ind_posted; 
#endif
#ifdef FEATURE_DATA_FALLBACK_LIST
  /* flag to indicate whether pdn fallback count has been added when a certain PDN
   	 fails to establish. This flag should set to TRUE when increased, and be cleared
   	 when this PDN context is cleared*/
  boolean                   pdn_fb_cnt_added;
#endif /*FEATURE_DATA_FALLBACK_LIST*/
  /* flag to indicate which ifaces have been brought down for a given PDN */
  ds707_pdn_context_iface_down_mask   iface_down_flag;

  ds707_sys_info_enum_type  orig_sys;
};

/*---------------------------------------------------------------------------
  Structure with pointer type for PDN control block. Array of this struct
  stores PDN control block info. When a call is requested memory will be
  allocated to the pointer for the PDN.
---------------------------------------------------------------------------*/
struct ds707_pdn_context_cb_ptr_type_s 
{
  ds707_pdn_context_cb_type *ds707_pdn_context_cb_ptr;
};

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE

/*---------------------------------------------------------------------------
  IP type 
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_PDNCNTX_IP_TYPE_V4 = 0,
  DS707_PDNCNTX_IP_TYPE_V6 = 1,
  DS707_PDNCNTX_IP_TYPE_INVALID
} ds707_pdncntx_iptype_enum_type;

/*---------------------------------------------------------------------------
  Type of throttling - disallowed or failure
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW = 0,
  DS707_PDNCNTX_THROTTLE_TYPE_FAILURE  = 1,
  DS707_PDNCNTX_THROTTLE_TYPE_INFINITE = 2,
  DS707_PDNCNTX_THROTTLE_TYPE_INVALID
} ds707_pdncntx_throttle_enum_type;

/*---------------------------------------------------------------------------
  Structure to store PDN throttle info related to a particular IP type
---------------------------------------------------------------------------*/
typedef struct
{
  rex_timer_type  throttle_timer;   /* timer for throttling */
  uint8           failure_count;      /* # of times failure has occurred */
  ds707_pdncntx_throttle_enum_type throttle_type; /* Reason for throttling */
  ps_iface_net_down_reason_type throttle_reason;  /* Stores throttle reason*/
  boolean throttled_in_tunnel_mode; /* Flag to indicate if throttling was in
                                      tunnel/direct mode*/

} ds707_pdn_throttle_iptype_info;

/*---------------------------------------------------------------------------
  Structure to store the PDN throttling information.
---------------------------------------------------------------------------*/
struct ds707_pdn_throttle_info_type_s 
{
  ds707_pdn_throttle_info_type *this_throttle_info_ptr;
  char            apn_string[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN]; 
  uint8           apn_string_len;
  ds707_pdn_throttle_iptype_info ipv4_throttle_info; /* Contains data related 
                                                        to IPV4 throttling */
  ds707_pdn_throttle_iptype_info ipv6_throttle_info; /* Contains data related
                                                        to IPV6 throttling */
};

/*---------------------------------------------------------------------------
  Structure with pointer type for PDN throttling. Array of this struct stores
  throttling info. When a PDN is throttled memory will be allocated to
  the pointer.
---------------------------------------------------------------------------*/
struct ds707_pdn_throttle_info_ptr_type_s
{
  ds707_pdn_throttle_info_type *ds707_pdn_throttle_info_ptr; 
};

#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */

/*===========================================================================
MACRO         DS707_IFACE_TBL_IS_VALID

DESCRIPTION   This macro returns a boolean indicating whether the iface_tble 
              ptr passed in is valid.

PARAMETERS    ds707_iface_table_type* : pointer to iface_tbl_entry.

RETURN VALUE  TRUE: if is valid 
              FALSE: otherwise.
===========================================================================*/
#define DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr)               \
  ((iface_tbl_ptr != NULL) &&                                 \
  (iface_tbl_ptr->this_iface_tbl_entry == iface_tbl_ptr))     


/*===========================================================================
MACRO         DS707_PDNCNTX_CB_IS_VALID

DESCRIPTION   This macro returns a boolean indicating whether the pdncntx_cb 
              ptr passed in is valid.

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdncntx_cb in question.

RETURN VALUE  TRUE: if is valid 
              FALSE: otherwise.
===========================================================================*/
#define DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr)              \
  ((pdncntx_cb_ptr != NULL) &&                                 \
  (pdncntx_cb_ptr->this_pdn == pdncntx_cb_ptr))                

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
/*===========================================================================
MACRO         DS707_THROTTLE_INFO_PTR_IS_VALID

DESCRIPTION   This macro returns a boolean indicating whether the 
              throttle_info_ptr passed in is valid.

PARAMETERS    ds707_pdn_throttle_info_type* : pointer to throttle_info.

RETURN VALUE  TRUE: if is valid 
              FALSE: otherwise.
===========================================================================*/
#define DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr)    \
  ((throttle_info_ptr != NULL) &&                              \
   (throttle_info_ptr->this_throttle_info_ptr == throttle_info_ptr))

#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_GET_PDNCNTX_CB_FROM_INSTANCE

DESCRIPTION   Given the pdn cb table index, returns the pdn cb tbl pointer 

PARAMETERS    None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_pdn_context_cb_type* ds707_get_pdncntx_cb_from_instance(
  uint8 x
);

/*===========================================================================
FUNCTION      DS707_GET_IFACE_TBL_FROM_INSTANCE

DESCRIPTION   Given the iface tbl index, returns the iface tbl pointer. 

PARAMETERS    Index to iface table

RETURN VALUE  Iface_tbl_ptr

SIDE EFFECTS  None
===========================================================================*/
ds707_iface_table_type* ds707_get_iface_tbl_from_instance(
  uint8 x
);

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_THROTTLE_INFO_PTR

DESCRIPTION   This function checks if the APN passed through the profile has 
              valid throttle info table entry. If it is, the throttle table
              entry ptr is returned.

PARAMETERS    ds707_data_session_profile_info_type*: pointer to the profile

RETURN VALUE  ds707_pdn_throttle_info_type* : if pdn is throttled then the 
                                              ptr to the table entry,
                                              NULL otherwise 

SIDE EFFECTS  None
===========================================================================*/
ds707_pdn_throttle_info_type* ds707_pdncntx_get_throttle_info_ptr( 
  ds707_data_session_profile_info_type  *data_sess_profile_info_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V4_THROTTLING_TIMER_CB

DESCRIPTION   This function handles expiry of v4 throttling timer for a PDN
               
PARAMETERS    user_data ptr : pointer to throttle info type

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_v4_throttling_timer_cb(
  unsigned long user_data
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V6_THROTTLE_TIMER_CB

DESCRIPTION   This function handles expiry of v4 throttling timer for a PDN

PARAMETERS    user_data ptr : pointer to throttle info type

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_v6_throttling_timer_cb(
  unsigned long user_data
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CLEAR_THROTTLE_COUNT

DESCRIPTION   This function clears the count of that iptype to 0. This is 
              called when there is a successful data connection on that 
              iptype

PARAMETERS    ds707_pdn_throttle_info_type* :pointer to throttle info type
              ds707_pdncntx_iptype_enum_type: ip type (v4/v6)

RETURN VALUE  None 
 
SIDE EFFECTS  None 
===========================================================================*/
void ds707_pdncntx_clear_throttle_count( 
  ds707_pdn_throttle_info_type*  throttle_info_ptr,
  ds707_pdncntx_iptype_enum_type ip_type
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_THROTTLE_PDN_IPTYPE

DESCRIPTION   This function increments the failure count for that ip type, 
              stores the fail reason, gets the timer value depending
              on the throttle_type, and starts the throttling timer.

PARAMETERS    ds707_data_session_profile_info_type*: pointer to the profile
              ds707_pdncntx_iptype_enum_type: ip_type
              ds707_pdncntx_throttle_enum_type:  throttle type (disallow/failure)
              ps_iface_net_down_reason_type: fail_reason (reason for throttling)

RETURN VALUE  None 
 
SIDE EFFECTS  None 
===========================================================================*/
void ds707_pdncntx_throttle_pdn_iptype( 
  ds707_data_session_profile_id  data_sess_profile_id,
  ds707_pdncntx_iptype_enum_type ip_type,
  ds707_pdncntx_throttle_enum_type throttle_type,
  ps_iface_net_down_reason_type fail_reason 
); 

/*===========================================================================
FUNCTION      DS707_PDNCNTX_THROTTLING_SET_ACL_DENY_REASON

DESCRIPTION   Set acl deny reason for v4 or V6 IFACE when currrent iface is 
              throttled. 

DEPENDENCIES  None

RETURN VALUE  ACL deny reason is returned.  
 
SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_throttling_set_acl_deny_reason
(
  ds707_data_session_profile_info_type * data_sess_prof_ptr,
  ps_iface_addr_family_type              ip_family             
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V4_THROTTLING_SET_PS_CALL_END_REASON

DESCRIPTION   Set netdown reason for v4 IFACE when PDN throttling happens

DEPENDENCIES  None

RETURN VALUE  None 
 
SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_v4_throttling_set_ps_call_end_reason
(
  ds707_pdn_throttle_info_type    * throttle_info_ptr,
  ds707_iface_table_type          * iface_tbl_ptr              
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_V6_THROTTLING_SET_PS_CALL_END_REASON

DESCRIPTION   Set netdown reason for v6 IFACE when PDN throttling happens

DEPENDENCIES  None

RETURN VALUE  None 
 
SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_v6_throttling_set_ps_call_end_reason
(
  ds707_pdn_throttle_info_type    * throttle_info_ptr,
  ds707_iface_table_type          * iface_tbl_ptr              
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IS_IFACE_THROTTLED

DESCRIPTION   This function checks if the APN passed through the profile is 
              being throttled or not. Throttle will be judged per iface.
              For dualIP, if v4 iface is throttled, companion v6 iface may not
              be throttled.

PARAMETERS    data_sess_profile_info_ptr 
              IP family type: v4/v6 

RETURN VALUE  TRUE: if current iface is throttled 
              FALSE: otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdncntx_is_iface_throttled(
  ds707_data_session_profile_info_type  *data_sess_profile_info_ptr,
  ps_iface_addr_family_type              ip_family
);

#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_ALLOCATE_PDNCNTX_CB

DESCRIPTION   This function allocates a PDN context cb 

PARAMETERS    None

RETURN VALUE  pointer PDN cntx 

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdn_context_cb_type* ds707_pdncntx_allocate_pdncntx_cb(void);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PDNCNTX_CB_PTR_FROM_IFACE_PTR

DESCRIPTION   Given an iface ptr it returns a ptr to pdn context it is 
              attached to. If it doesnt belong to any pdn then returns NULL 

PARAMETERS    ps_iface_type * : pointer to iface

RETURN VALUE  pointer PDN cntx if valid 
              NULL if invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdn_context_cb_type* ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
  ps_iface_type* iface_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_V4_IFACE

DESCRIPTION   Given a PDN context cb ptr get the V4 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  iface ptr 

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_v4_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_V4_IFACE

DESCRIPTION   Given a PDN context cb ptr set the V4 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
              ds707_iface_table_type *    : pointer to iface tbl entry

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_v4_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  ds707_iface_table_type*    iface_tbl_ptr
);

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_V6_IFACE

DESCRIPTION   Given a PDN context cb ptr get the V6 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  iface ptr 

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_v6_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_V6_IFACE

DESCRIPTION   Given a PDN context cb ptr set the V6 iface.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
              ds707_iface_table_type*     : pointer to iface tbl entry 

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_v6_iface(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  ds707_iface_table_type*    iface_tbl_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IP6_SM_PTR

DESCRIPTION   Given the iface tbl pointer, returns the IP6 pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  ip6_sm_type *

SIDE EFFECTS  None.
===========================================================================*/
ip6_sm_type* ds707_pdncntx_get_ip6_sm_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DSSNET6_SM_PTR

DESCRIPTION   Given the iface tbl pointer, returns the DSSNET6 pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  dssnet6_sm_cb_type *

SIDE EFFECTS  None.
===========================================================================*/
dssnet6_sm_cb_type* ds707_pdncntx_get_dssnet6_sm_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_TBL_INDEX_FROM_IFACE

DESCRIPTION   Given an iface ptr it returns an index of iface tbl entry it 
              belongs to. 

PARAMETERS    ps_iface_type*

RETURN VALUE  int - positive value if success 
              -1  - if error 

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_get_iface_tbl_index_from_iface(
  ps_iface_type* iface_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_TBL_ENTRY_FROM_IFACE

DESCRIPTION   Given an iface ptr it returns a ptr to iface tbl entry it 
              belongs to. This is set to iface_ptr->client_data_ptr at
              powerup

PARAMETERS    ps_iface_ptr * : pointer to iface

RETURN VALUE  pointer iface_tbl if valid 
              NULL if invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds707_iface_table_type* ds707_pdncntx_get_iface_tbl_entry_from_iface(
  ps_iface_type* iface_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_PTR_FROM_IFACE_TBL_INDEX

DESCRIPTION   Given the iface tbl index, returns the ps_iface pointer.

PARAMETERS    uint8 : iface table index

RETURN VALUE  ps_iface_type* : pointer to iface

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(
  uint8 iface_tbl_index
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_IFACE_PTR_FROM_IFACE_TBL

DESCRIPTION   Given the iface tbl ptr, returns the ps_iface pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  ps_iface_type* : pointer to iface

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_pdncntx_get_iface_ptr_from_iface_tbl(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PDNCNTX_CB_PTR

DESCRIPTION   Given an iface table ptr it returns a ptr to pdn context it is 
              attached to. If it doesnt belong to any pdn then returns NULL 

PARAMETERS    ds707_iface_table_type* : pointer to iface tbl entry

RETURN VALUE  pointer PDN cntx if valid 
              NULL if invalid 

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdn_context_cb_type* ds707_pdncntx_get_pdncntx_cb_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DSSNET4_SM_PTR

DESCRIPTION   Given the iface tble pointer, returns the DSSNET4 SM 
              instance Ptr.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  dssnet4_sm_cb_type* : pointer to dssnet4 

SIDE EFFECTS  None.
===========================================================================*/
dssnet4_sm_cb_type* ds707_pdncntx_get_dssnet4_sm_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);
#endif /* FEATURE_DATA_IS707 */
  
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ACL_PTR

DESCRIPTION   Given the iface tbl pointer, returns the ACL pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  pointer to acl_type

SIDE EFFECTS  None.
===========================================================================*/
acl_type* ds707_pdncntx_get_acl_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);

/*===========================================================================
FUNCTION      DS707_GET_NUM_DS_IFACES_IN_USE

DESCRIPTION   This function returns the total number of ds707 ifaces that 
              are currently being used

PARAMETERS    None

RETURN VALUE  uint8 - NUmber of ds707 ifaces in use

SIDE EFFECTS  None
===========================================================================*/
uint8 ds707_get_num_active_ds_ifaces(void);
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_CONN_DELAY_TIMER_PTR

DESCRIPTION   Given the iface tbl pointer, returns the connection delay timer 
              pointer.

PARAMETERS    ds707_iface_table_type* : pointer to iface table type

RETURN VALUE  rex_timer_type *

SIDE EFFECTS  None.
===========================================================================*/
rex_timer_type* ds707_pdncntx_get_conn_delay_timer_ptr(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DATA_SESSION_INFO_PTR

DESCRIPTION   Given the pdncntx pointer, returns the data sesssion info
              pointer.

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  ds707_data_session_info_type *

SIDE EFFECTS  None.
===========================================================================*/
ds707_data_session_info_type* ds707_pdncntx_get_data_session_info_ptr(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_REQUESTING_PROFILE

DESCRIPTION   Given a PDN context cb ptr get the requesting profile

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  TRUE  if requesting profile ID is valid 
              FALSE if PDN CB or profile ID is invalid 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_requesting_profile(
  ds707_pdn_context_cb_type*              pdncntx_cb_ptr,
  ds707_data_session_profile_info_type**  data_sess_profile_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_CURRENT_PROFILE

DESCRIPTION   Given a PDN context cb ptr get the current profile

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  TRUE  if requesting profile ID is valid 
              FALSE if PDN CB or profile ID is invalid 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_current_profile(
  ds707_pdn_context_cb_type*              pdncntx_cb_ptr,
  ds707_data_session_profile_info_type**  data_sess_profile_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PROFILE

DESCRIPTION   Given a PDN context cb ptr get the profile ptr (either current
              or requesting profile whichever is valid). Currently used only
              with MPPM.

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb

RETURN VALUE  profile id 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_get_profile
(
  ds707_pdn_context_cb_type             *pdncntx_cb_ptr,
  ds707_data_session_profile_info_type **data_sess_prof_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_PPP_RESYNC_LOCK

DESCRIPTION   Sets ppp_resync_lock for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb
              boolean                     : ppp_resync_lock

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ppp_resync_lock(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  boolean                    ppp_resync_lock
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PPP_RESYNC_LOCK

DESCRIPTION   Gets ppp_resync_lock for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb
              
RETURN VALUE  boolean 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_ppp_resync_lock(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_PDN_CB_PTR

DESCRIPTION   Set the iface table entry's pdn context control block pointer. 

PARAMETERS    ds707_iface_table_type* : pointer to iface table entry for which 
                                        pdn cb pointer needs to be set
              ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_pdn_cb_ptr( 
  ds707_iface_table_type      *ds707_pdn_other_iface_tbl_ptr, 
  ds707_pdn_context_cb_type   *ds707_pdn_cntx_cb_ptr 
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_EVENT_CB

DESCRIPTION   This function is called when iface_down_ev event is generated 
              for registerd ifaces.
              -Mark this iface table entry as NULL.
              -If both iface tbl entries for this pdn are NULL, then
              mark the PDN out of use.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_iface_event_cb
(
  ps_iface_type                *this_iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_data_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_PROCESS_PS_CMD

DESCRIPTION   This function processes the PS commands received by the DS task.
              The  commands that can be received by the mode specific handler
              are as follows:

	          DS707_IFACE_IN_USE_CMD: Command to notify active iface use by app(s)
              DS707_IFACE_OUTOF_USE_CMD: Command to notify 
	 
PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_process_cmd(
  const ds_cmd_type *cmd_ptr                       
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_REGISTER_IFACE_EV

DESCRIPTION   Register for iface up, down and routable events for this iface

PARAMETERS    ps_iface_type * : iface_ptr

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_register_iface_ev(
  ds707_iface_table_type* ds707_iface_tbl_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_ARE_ALL_PDNS_INVALID

DESCRIPTION   This function checks if all PDN contexts are invalid

DEPENDENCIES  None

RETURN VALUE  TRUE: If all PDNs are invalid
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdnctx_are_all_pdns_invalid
(
  void
);

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_NCP_HANDLE

DESCRIPTION   Sets ncp_handle for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb
              ppp_ncp_handle_type        : ncp handle

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ncp_handle(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  ppp_ncp_handle_type        ncp_handle
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_NCP_HANDLE

DESCRIPTION   Gets ncp_handle for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb
              
RETURN VALUE  ppp_ncp_handle_type        : ncp handle

SIDE EFFECTS  None.
===========================================================================*/
ppp_ncp_handle_type ds707_pdncntx_get_ncp_handle(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_GRACEFUL_PPP_CLOSE

DESCRIPTION   Sets graceful ppp close flag for all the ifaces attached to 
              this PDN CB.

PARAMETERS    ds707_pdn_context_cb_type* : pdn context cb ptr
              boolean                    : graceful_ppp_close flag

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_graceful_ppp_close(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  boolean                    graceful_ppp_close
);
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_NCP_STARTED

DESCRIPTION   Sets ncp_started boolean for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pdn context cb ptr
              boolean                    : ncp_started flag

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ncp_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  boolean                    ncp_started
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_NCP_STARTED

DESCRIPTION   Gets ncp_started boolean for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_ncp_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_NCP_RESYNC_STARTED

DESCRIPTION   When NCP goes into resync this flag is set. When the ifaces 
              come up/go down after resync, the flag is unset.

PARAMETERS    ds707_pdn_context_cb_type* : pdn context cb ptr
              boolean                    : ncp_resync_started

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_ncp_resync_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr,
  boolean                    ncp_resync_started
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_NCP_RESYNC_STARTED

DESCRIPTION   Gets ncp_resync_started boolean for the given pdn_cntx_ptr

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_ncp_resync_started(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_PROCESS_VSNCP_STARTED

DESCRIPTION   When VSNCP is started, in dual ip case, call active_bringup 
              on the other iface for that PDN 

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_process_vsncp_started(
  ds707_pdn_context_cb_type  *pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CONFIGURE_VSNCP

DESCRIPTION   Configures the VSNCP Parameters for the given PDN Context Block

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdncntx_configure_vsncp(
  ds707_pdn_context_cb_type* pdn_context_cb,
  ppp_vsncp_3gpp2_config_type* ncp_config_ptr,
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr 
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CHECK_COMPATIBILITY

DESCRIPTION   Checks if the pdn context cb ptr is of the same config that is 
              passed in the policy. 

PARAMERTERS   ds707_pdn_context_cb_type* : pointer to pdn context cb 
              acl_policy_info_type*      : pointer to policy info 

RETURN VALUE  boolean: TRUE if compatible. 
                       FALSE otherwise 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_check_compatibility( 
  ds707_pdn_context_cb_type *pdncntx_cb_ptr, 
  acl_policy_info_type      *policy_info_ptr 
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMEOUT_CB

DESCRIPTION   Posts a command to DS Task on PDN inactivity timer expiry

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timeout_cb(
  uint32 pdn_context_cb
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMER_PROCESS_CMD

DESCRIPTION   Handles expiry of inactivity timer for a PDN connection

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timer_process_cmd(
  ds_cmd_type * cmd_ptr
);
/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMER_RESTART

DESCRIPTION   Restarts the inactivity timer for a PDN connection

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timer_restart(
  ds707_pdn_context_cb_type * pdn_context_cb
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_INACTIVITY_TIMER_START

DESCRIPTION   Starts the inactivity timer for a PDN connection

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_inactivity_timer_start(
  ds707_pdn_context_cb_type * pdn_context_cb
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_TEARDOWN_ALL_PDNS

DESCRIPTION   This function triggers teardown of all PDN connections. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_teardown_all_pdns
(
  ps_iface_net_down_reason_type end_reason
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_TEARDOWN_IFACE

DESCRIPTION   This triggers teardown of given PDN.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_teardown_iface
(
  ps_iface_type                 *iface_ptr,
  ps_iface_net_down_reason_type  end_reason
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_ARE_ALL_PDNS_DOWN

DESCRIPTION   This function checks if all PDN contexts are down

DEPENDENCIES  None

RETURN VALUE  TRUE: If all PDNs are down 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pdnctx_are_all_pdns_down
(
  void
);

#ifdef FEATURE_DATA_EHRPD_DUAL_IP
/*===========================================================================
FUNCTION      DS707_PDNCNTX_ALLOCATE_IFACE_TBL_ENTRY

DESCRIPTION   This function allocates a iface table entry

PARAMETERS    None

RETURN VALUE  pointer to iface table entry 

SIDE EFFECTS  None.
===========================================================================*/
ds707_iface_table_type*    ds707_pdncntx_allocate_iface_tbl_entry(void);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_IN_USE_CMD

DESCRIPTION   Post a command to indicate the active use of the iface 
              by application.

PARAMETERS    ps_iface_type * : Iface_ptr 

DEPENDENCIES  None.

RETURN VALUE  int 0 - success

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_iface_in_use_cmd
(
  ps_iface_type    *iface_ptr,
  void             *not_used
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_OUT_OF_USE_CMD

DESCRIPTION   Post a command to indicate the iface is not in use by 
              any application.

PARAMETERS    ps_iface_type * : iface_ptr

DEPENDENCIES  None.

RETURN VALUE  int 0 - Success

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_iface_out_use_cmd
(
  ps_iface_type    *iface_ptr,
  void             *not_used
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_HANDLE_PPP_EV

DESCRIPTION   From dssnet this function is called to resolve what action 
              should be taken for ppp_event for this iface. We need to
              resolve 2 PPP events - SUBSCRIPTION_LIMITED_TO_V4 and
              SUBSCRIPTION_LIMITED_TO_V6 depending on the address family of
              the iface for dual ip

PARAMETERS    ds707_pdn_context_cb_type* : pointer to pdn context cb 
              ps_iface_type*             : pointer to iface
              event                      : ppp event

RETURN VALUE  ds707_pdncntx_ppp_event_action.

SIDE EFFECTS  None.
===========================================================================*/
ds707_pdncntx_ppp_event_action ds707_pdncntx_handle_ppp_ev(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr, 
  ps_iface_type             *iface_ptr, 
  ppp_event_e_type           ppp_event
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_FREE_V6_IFACE

DESCRIPTION   This function frees the V6 iface of the pdn context cb. This 
              is needed if a dual IP call is attempted before session
              negotiation and it goes on hrpd system, then we need to free
              the V6 iface, if app request is V4.
              

PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_free_v6_iface(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_FREE_V4_IFACE

DESCRIPTION   This function frees the V4 iface of the pdn context cb. This 
              is needed if a dual IP call is attempted before session
              negotiation and it goes on hrpd system, then we need to free
              the V4 iface, if app request is V6.
              

PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_free_v4_iface(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
);

#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_EPC_HANDOFF_CTXT

DESCRIPTION   Handles the IOCTL to get epc handoff ctxt. It returns the IP
              config details of the passed IFACE and companion IFACE (in 
              case of dual IP connection) and bearer type (single IP or dual
              IP)

PARAMETERS    argval_ptr: Pointer to acl_handoff_ctxt_type that contains IP
              config of this IFACE, companion IFACE (in case of dual IP) and
              bearer type (single IP or dual IP bearer)

DEPENDENCIES

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_get_epc_handoff_ctxt(
  ps_iface_type             *iface_ptr,
  void                      *argval_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_EPC_HANDOFF_CTXT

DESCRIPTION   Handles the IOCTL to set epc handoff ctxt. It updates the IP
              config for the passed IFACE and companion IFACE (in case of
              dual IP connection)
    
PARAMETERS    argval_ptr: Pointer to acl_handoff_ctxt_type that contains IP
              config for this IFACE and companion IFACE (in case of dual IP)

DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_set_epc_handoff_ctxt(
  ps_iface_type             *iface_ptr,
  void                      *argval_ptr
);
#endif /*FEATURE_EPC_HANDOFF*/

#ifdef FEATURE_CALL_THROTTLE
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_DCTM_IND_POSTED

DESCRIPTION   Returns the boolean which indicates whether DCTM is 
              posted to CM  
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb

RETURN VALUE   boolean: dctm_ind_posted

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_dctm_ind_posted(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_DCTM_IND_POSTED

DESCRIPTION   Set the boolean, to the value passed. This flag indicates 
              whether DCTM is posted to CM
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb 
              boolean: to set the dctm_ind_posted flag 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_dctm_ind_posted(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  boolean                    dctm_posted
);
#endif /* FEATURE_CALL_THROTTLE */

/*===========================================================================
FUNCTION      DS707_PDNCTX_PPP_EVENT_HDL

DESCRIPTION   Called by device manager to indicate about LCP/AUTH events.
    
PARAMETERS    protocol        - Protocol type
              ppp_event       - PPP event
              ppp_fail_reason - Failure reason

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_ppp_event_hdl
(
  ppp_protocol_e_type    protocol,
  ppp_event_e_type       ppp_event,
  ppp_fail_reason_e_type ppp_fail_reason
);

/*===========================================================================
FUNCTION DS707_PDNCTX_POST_IFACE_DOWN

DESCRIPTION
  This function does additional handling before v4/v6 iface goes down. It 
  sees that both the v4/v6 ifaces go down together to avoid race conditions

PARAMETERS  iface_ptr   - IFACE pointer
            down_reason - Network down reason

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS  
  None
===========================================================================*/
void ds707_pdncntx_post_iface_down
(
  ps_iface_type                 *iface_ptr,
  ps_iface_net_down_reason_type  down_reason
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_CURRENT_PROFILE_ID

DESCRIPTION   This function sets the current data session profile id in 
              data_session_info in the pdn cb

PARAMETERS    pdn_cb_ptr *: pointer to PDN context cb
              ds707_data_session_profile_id : 
                    Current data session profile id for the iface.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_set_current_profile_id
(
  ds707_pdn_context_cb_type     *pdn_cb_ptr,
  ds707_data_session_profile_id  current_profile_id,
  ds_profile_subs_id_enum_type   subs_id
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_REQUESTING_PROFILE_ID

DESCRIPTION   This function sets the requesting data session profile id in 
              data_session_info in the pdn cb

PARAMETERS    pdn_cb_ptr *: pointer to PDN context cb
              ds707_data_session_profile_id : 
                   Requesting data session profile id for the iface.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_set_requesting_profile_id
(
  ds707_pdn_context_cb_type     *pdn_cb_ptr,
  ds707_data_session_profile_id  requesting_profile_id,
  ds_profile_subs_id_enum_type   subs_id
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_IFACE_LINGER_STATUS

DESCRIPTION   This function sets the linger status in the data_session_info 
              in pdn cb

PARAMETERS    pdn_cb_ptr *: pointer to PDN context cb
              boolean        :  TRUE - IFACE lingering else FALSE
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_set_iface_linger_status
(
  ds707_pdn_context_cb_type  *pdn_cb_ptr,
  boolean                     is_iface_lingering
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ACTIVE_IFACE

DESCRIPTION   Returns an active iface
    
PARAMETERS    None.

RETURN VALUE  ps_iface_type *

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type *ds707_pdncntx_get_active_iface
(
  void
);
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ACTIVE_PDN_COUNT

DESCRIPTION   Returns current active PDN count
    
PARAMETERS    None.

RETURN VALUE  uint8: active pdn count

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds707_pdncntx_get_active_pdn_count(void);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IS_LAST_ACTIVE_PDN

DESCRIPTION   Returns if the input pdn is the last active pdn
    
PARAMETERS    None.

RETURN VALUE  TRUE: input pdn is the last PDN 
              FALSE: input pdn is not the last PDN 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_is_last_active_pdn(
                        ds707_pdn_context_cb_type *curr_pdn_cntx_ptr
                        );

/*===========================================================================
FUNCTION DS707_PDNCTX_DUN_CLEANUP

DESCRIPTION
  This function handles cleanup for dun calls

PARAMETERS  this_iface_ptr - IFACE pointer

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS  
  None
===========================================================================*/
void ds707_pdncntx_dun_cleanup
(
  ps_iface_type   *this_iface_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SYS_PDN_THROTTLE_INFO_NOTIFICATION

DESCRIPTION   Generate system PDN throttle events 
    
PARAMETERS    None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_sys_pdn_throttle_info_notification
(
  void 
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SYS_IOCTL_EHRPD_PDN_THROTTLE_INFO

DESCRIPTION   This function will be called by sys ioctl to get PDN throttle 
              info
    
PARAMETERS    argval_ptr: this pointer is pointing to the caller for getting
                          PDN throttle info

RETURN VALUE   0 on success
              -1 on Failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds707_pdncntx_sys_ioctl_ehrpd_pdn_throttle_info
(
  void            *argval_ptr,
  int16           *ps_errno
);

/*===========================================================================
FUNCTION      DS707_GET_PDNCNTX_CB_FROM_REQ_PROFILE_APN

DESCRIPTION   Given the requesting profile, returns the pdn cb tbl pointer 
              associated to it 

PARAMETERS    ds707_data_session_profile_id : 
                    Requesting data session profile id.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_pdn_context_cb_type* ds707_get_pdncntx_cb_from_req_profile_apn(
  ds707_data_session_profile_info_type *data_sess_profile_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_CLEAR_PDN_THROTTLED_IN_TUNNEL_MODE

DESCRIPTION   Function to clear PDNs that were throttled due to non-explicit 
              Network Reject in Tunnel Mode
              The Following Fail reasons will be cleared from the throttle
              table
              1. PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_TIMEOUT
              2. PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE
    
PARAMETERS    None

RETURN VALUE  None 
 
SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_clear_pdn_throttled_in_tunnel_mode
(
  void
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_RESET_CALL_BRINGUP_IN_TUNNEL_MODE

DESCRIPTION   For a given PDN context cb ptr resets the 
              bringup_in_tunnel_mode flag to FALSE

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_reset_call_bringup_in_tunnel_mode
(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_CALL_BRINGUP_MODE

DESCRIPTION   For a given PDN context cb ptr set the bringup flag to 
              indicate call bring up in tunnel mode depending on the S101
              state

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 


RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_call_bringup_mode
(
  ds707_pdn_context_cb_type* pdncntx_cb_ptr
);

#ifdef FEATURE_DATA_FALLBACK_LIST
/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_PDN_FB_CNT_ADDED

DESCRIPTION   Set the pdn_fb_cnt_added flag, to indicated whether a pdn fallback 
              count has been increased during this PDN session. 
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb 
              boolean: to set the pdn_fb_cnt_added flag 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_pdn_fb_cnt_added(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  boolean                    pdn_fb_cnt_added
);
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_PDN_FB_CNT_ADDED

DESCRIPTION   Set the pdn_fb_cnt_added flag, to indicated whether a pdn fallback 
              count has been increased during this PDN session. 
    
PARAMETERS    ds707_pdn_context_cb_type *: pointer to pdn context cb 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pdncntx_get_pdn_fb_cnt_added(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
);
#endif /*FEATURE_DATA_FALLBACK_LIST*/
/*===========================================================================
FUNCTION      DS707_PDNCNTX_READ_PDN_OVERRIDE_NV

DESCRIPTION   Read the NV record for disable pdn type override.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pdncntx_read_pdn_override_nv(void);

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_PDNCNTX_IFACE_RECONFIG

DESCRIPTION   Function to reconfigure (resync) NCP connection
    
PARAMETERS    iface_ptr: iface to be reconfigured
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_iface_reconfig
(
  ps_iface_type            *iface_ptr
);
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_V6_REM_RTR_LIFETIME

DESCRIPTION   Handles the IOCTL to retrive remaining IPv6 router lifetime.
    
PARAMETERS    iface_ptr: iface associated IPv6 PDN 
              argnal_ptr: void pointer to return value
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pdncntx_get_v6_rem_rtr_lifetime
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr
);
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DS707_PDNCNTX_SET_ORIG_SYS

DESCRIPTION   Given a PDN context cb ptr set the origination system

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
             
RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pdncntx_set_orig_sys
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  ds707_sys_info_enum_type   last_good_sys
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_GET_ORIG_SYS

DESCRIPTION   Given a PDN context cb ptr get the origination system

PARAMETERS    ds707_pdn_context_cb_type * : pointer to pdn context cb 
              
RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
ds707_sys_info_enum_type ds707_pdncntx_get_orig_sys
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION      DS707_PDNCNTX_IS_CALL_ALLOWED

DESCRIPTION   Utility function to see if the current serving system is 
              compatible with the system stored in PDN CB during call orig
              
DEPENDENCIES  None

RETURN VALUE  TRUE  - if the current serving system & stored orig system 
                      PDN CB are compatible
              FALSE - otherwise

SIDE EFFECTS  None 
  
===========================================================================*/
boolean ds707_pdncntx_is_call_allowed 
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  ps_iface_net_down_reason_type *call_end_reason
);

/*===========================================================================
FUNCTION      DS707_PDNCTX_RESET_THROTTLE_ON_SIM_SWAP

DESCRIPTION   Clear the throttling information on hotswap
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None 
  
===========================================================================*/
void ds707_pdnctx_reset_throttle_on_sim_swap();

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION DS707_PDNCNTX_CB_IS_DEFAULT_PDN

DESCRIPTION
  This function returns if the PDN CB is default PDN
  
PARAMETERS  
  pointer to the PDN CB in question
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE:  if this PDN CB is default PDN
  FALSE: otherwise.
==========================================================================*/
boolean ds707_pdncntx_is_default_pdn
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr
);

/*===========================================================================
FUNCTION DS707_PDNCNTX_CB_IS_DEFAULT_APN

DESCRIPTION
  This function returns if the APN in profile is default APN
  
PARAMETERS  
  pointer to the profile in question
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE:  if this APN is default APN
  FALSE: otherwise.
==========================================================================*/
boolean ds707_pdncntx_is_default_apn
(
  ds707_data_session_profile_info_type *data_sess_prof_ptr
);

/*===========================================================================
FUNCTION DS707_PDNCNTX_STORE_DEFAULT_PDN_AND_APN

DESCRIPTION
  This function stores the default PDN and APN inicated by HSGW
  
PARAMETERS  
  pointer to the APN
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_pdncntx_set_default_pdn_and_apn
(
  ds707_pdn_context_cb_type *pdncntx_cb_ptr,
  char                      *apn,
  uint8                      apn_len
);

/*===========================================================================
FUNCTION DS707_PDNCNTX_RESET_DEFAULT_PDN

DESCRIPTION
  This function clears the default PDN context in UE
  
PARAMETERS  
  None.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_pdncntx_reset_default_pdn
(
  void
);
#endif /* FEATURE_EHRPD */
#endif /*DSHDR_PDN_CONTEXT_H*/
