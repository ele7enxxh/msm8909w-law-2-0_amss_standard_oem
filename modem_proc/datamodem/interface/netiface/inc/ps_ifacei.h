#ifndef PS_IFACEI_H
#define PS_IFACEI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ I F A C E I . H

DESCRIPTION
  Header file defining all of the internal data types

DEPENDENCIES
  None of these functions should EVER be called from Interrupt context!

Copyright (c) 2008-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_ifacei.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when      who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/13    rk     Removed feature mobileap.
04/13/12    pgm    Doing flow ctrl check on Phys link only for non-NAT ifaces 
                   as NAT ifaces dont have phys link and flow ctrl check fails
                   with NULL phys link.
08/16/09    pp     ps_iface_fill_next_hop_addr API introduced.
12/14/08    pp     Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ps_iface.h"
#include "ps_crit_sect.h"
#include "ps_iface_defs.h"
#include "ps_svc.h"
#ifdef FEATURE_DATA_RM_NET
#include "ps_tx_meta_info.h"
#endif /* FEATURE_DATA_RM_NET */
#include "ds_Utils_DebugMsg.h"
#include <timetick.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         INTERNAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  The PS iface Authentication credentials type
---------------------------------------------------------------------------*/
#define PS_AUTH_MAX_USER_ID_LEN   (255)
#define PS_AUTH_MAX_PASSWORD_LEN  (255)
typedef struct
{
  uint8  user_id_len;
  char   user_id[PS_AUTH_MAX_USER_ID_LEN];
  uint8  password_len;
  char   password[PS_AUTH_MAX_PASSWORD_LEN];
} ps_iface_auth_credential_type;

typedef struct
{
  ps_iface_auth_alg_pref_bitmask_e_type  alg_pref;
  ps_iface_auth_credential_type          auth_creds;
  void *                                 mode_spec_auth_info_ptr;
} ps_iface_auth_info_type;

/*===========================================================================

                         EXTERNAL MACROS

===========================================================================*/
#define PS_IFACE_GET_INPUT_F_PTR(ps_iface_ptr)                           \
  (PS_IFACE_GET_BASE_IFACE(ps_iface_ptr)->iface_private.ip_input)

#define PS_IFACE_GET_TIMETICK_MS() (uint32)timetick_get_ms()

/*===========================================================================

                         EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACE_INIT()

DESCRIPTION
  Initializes ps_iface module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_init
(
  void
);

/*===========================================================================
FUNCTION PS_IFACE_EVENT_INIT()

DESCRIPTION
  This function initializes the global queues for both ps_iface and
  ps_phys_link.  It also registers the ps_iface phys_link function on the
  global phys_link queue.

  Note: Currently, only the events in ps_iface need initialization. Hence,
  the implementation of this function resides in ps_ifacei_event.c.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_event_init
(
  void
);

/*===========================================================================
FUNCTION PS_IFACE_GET_REF_CNT()

DESCRIPTION
  This macro returns the interface reference count

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  the reference count for the passed in interface
===========================================================================*/
#define PS_IFACE_GET_REF_CNT  ps_iface_get_ref_cnt
INLINE uint32 ps_iface_get_ref_cnt
(
  ps_iface_type *iface_ptr
)
{
  uint32  ref_cnt = 0;
  uint8   index   = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    for (index = 0; index < PS_POLICY_MGR_IFACE_PRIORITY_MAX; index++)
    {
      ref_cnt += iface_ptr->iface_private.ref_cnt[index];
    }

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }

  return ref_cnt;

} /*  ps_iface_get_ref_cnt() */

/*===========================================================================
FUNCTION PS_IFACE_RESET_REF_CNT()

DESCRIPTION
  This macro resets the interface reference count to 0

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACE_RESET_REF_CNT  ps_iface_reset_ref_cnt
INLINE void ps_iface_reset_ref_cnt
(
  ps_iface_type *iface_ptr
)
{
  uint8   index   = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    for (index = 0; index < PS_POLICY_MGR_IFACE_PRIORITY_MAX; index++)
    {
      iface_ptr->iface_private.ref_cnt[index] = 0;
      iface_ptr->iface_private.priority_mask = 0;
    }

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }
} /*  ps_iface_reset_ref_cnt() */

/*===========================================================================
FUNCTION PS_IFACE_GET_APP_PRIORITY

DESCRIPTION
  Returns the first non-zero app priority field stored in an iface.

PARAMETERS
  iface_ptr - Pointer to ps iface control block.

RETURN VALUE
  app_priority stored in physical ifaces. In case of error, -1 is returned.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_get_app_priority
(
  ps_iface_type    *iface_ptr
);

/*===========================================================================
MACRO PS_IFACE_GET_BASE_IFACE()

DESCRIPTION
  If Inherit IP info of the iface is set then this macro drills down the
  chain of logical ifaces and return base iface, else it acts like NOOP
  returning the passed iface_ptr.

PARAMETERS
  iface_ptr:   any logical iface in a chain.

RETURN VALUE
  Same iface in case of error
  or base iface ptr of a logical iface chain.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_BASE_IFACE ps_iface_get_base_iface
INLINE ps_iface_type* ps_iface_get_base_iface
(
  ps_iface_type        *iface_ptr
)
{
  ps_iface_type *base_iface_ptr;

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid iface 0x%p", iface_ptr);
    ASSERT(0);
    return iface_ptr;
  }

 /*--------------------------------------------------------------------------
   If this is a logical iface chain, go to the base iface.
  -------------------------------------------------------------------------*/
  base_iface_ptr = iface_ptr;
  while (PS_IFACEI_IP_INFO_IS_INHERITED(base_iface_ptr))
  {
    base_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(base_iface_ptr);
    if (!PS_IFACE_IS_VALID(base_iface_ptr))
    {
      LOG_MSG_ERROR_1("Invalid associated iface 0x%p", base_iface_ptr);
      return iface_ptr;
    }
  }

  return base_iface_ptr;
} /* ps_iface_get_base_iface */

/** 
  @def   PS_IFACE_GET_PDN_TEARDOWN_LOCAL_ABORT_CONFIG(iface_ptr)
  @brief This macro will get the value of local abort flag
  
  @param
    iface_ptr[in]    pointer to iface
 
  @return
    pdn teardown config value

*/
#define PS_IFACE_GET_PDN_TEARDOWN_LOCAL_ABORT_CONFIG ps_iface_get_pdn_teardown_local_abort_config
INLINE boolean ps_iface_get_pdn_teardown_local_abort_config
(
  ps_iface_type *iface_ptr
)
{
  ps_iface_type * base_iface_ptr;
  
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);
    if (PS_IFACE_IS_VALID(base_iface_ptr))
    {
      return base_iface_ptr->iface_private.pdn_teardown_config.local_abort;
    }
  }
  
  return FALSE;
} /*  ps_iface_get_pdn_teardown_config() */

/** 
  @def   PS_IFACEI_INC_COOKIE(iface_ptr)
  @brief Increments the cookie of given iface 
 
  This method Increments the cookie of given iface
  
  @param
    iface_ptr[in]    pointer to iface
 
  @return
    cookie value after increment

*/
#define PS_IFACEI_INC_COOKIE( iface_ptr )     \
  if (PS_IFACE_IS_VALID(iface_ptr))           \
  {                                           \
    iface_ptr->iface_private.cookie++;        \
  }                
    
/**
  @brief  This macro returns whether the first packet drop in powersave mode is 
  reported or not.
  
  @return  TRUE      If Enabled.
           FALSE     If disabled.
  
  @code
  @endcode
*/
#define PS_IFACE_IS_FIRST_POWERSAVE_PACKET_DROP_REPORTED                       \
    ps_iface_is_first_powersave_packet_drop_reported
  
boolean ps_iface_is_first_powersave_packet_drop_reported
(
  void
);
  
/**
  @brief  This macro sets the powersave mode first packet drop reported setting for
  a given iface
  
  @param[in] report - TRUE if reported already
                      FALSE If not reported 

  @return  TRUE      If Enabled.
           FALSE     If disabled.

  @code
  @endcode
*/
#define PS_IFACE_SET_POWERSAVE_FIRST_PACKET_DROP_REPORTED(report)       \
        ps_iface_set_first_powersave_packet_drop_reported(report)
    
void ps_iface_set_first_powersave_packet_drop_reported
(
  boolean reported
);
    
/**
  @brief  This macro Returns ip_filters powersave mode configuration
  
  @param[in] iface_ptr - Pointer to iface control block

  @return    TRUE        If Enabled.
             FALSE       If disabled.

  @code
  @endcode
*/
#define PS_IFACE_GET_POWERSAVE_FILTERING_MODE(iface_ptr)                         \
  ( PS_IFACE_IS_VALID(iface_ptr)           \
      ?iface_ptr->iface_private.powersave_filtering_settings.enable_filter_mode  \
      :FALSE)        

/**
  @brief  This macro returns ip_filters power save AUTO EXIT mode configuration
  
  @param[in] iface_ptr - Pointer to iface control block
  
  @return    TRUE        If Enabled.
             FALSE       If disabled.
  
  @code
  @endcode
*/
#define PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE(iface_ptr)                   \
  ( PS_IFACE_IS_VALID(iface_ptr)                                                     \
      ?iface_ptr->iface_private.powersave_filtering_settings.enable_auto_exit_mode   \
      :FALSE)

/**
  @brief  This macro calls ioctl to enable/disable powersave mode
  
  @param[in]  iface_ptr - Pointer to iface control block
  @param[in]  enable    - TRUE for Enable
                          FALSE for Disable
  @param[out] ps_errno    Failure reason.
                          DS_EFAULT : Invalid arguments
                          DS_NOMEMORY : Out of memory
                          DS_EINVAL : Invalid Operation
  
  @code
  @endcode
*/
#define PS_IFACE_POWERSAVE_FILTERING_AUTO_EXIT_MODE_HDLR(iface_ptr, enable, ps_errno) \
  if( PS_IFACE_IS_VALID(iface_ptr))                                                   \
  {                                                                                   \
    if( -1 == ps_iface_ioctl( iface_ptr,                                              \
                              PS_IFACE_IOCTL_SET_POWERSAVE_FILTERING_MODE,            \
                              &enable,                                                \
                                 &ps_errno) )                                         \
    {                                                                                 \
      LOG_MSG_ERROR_1("Failed to set poser_save_ipfilters_mode errno%d",ps_errno);    \
    }                                                                                 \
  }                                                                                  

/**
  @brief  This macro resets the powersave filtering mode setting of a given
          iface to FALSE

  @param[in]  this_iface_ptr - Pointer to iface control block

  @code
  @endcode
*/
#define PS_IFACEI_RESET_POWERSAVE_FILTERING_MODE( this_iface_ptr )                    \
 (this_iface_ptr)->iface_private.powersave_filtering_settings.enable_filter_mode      \
   = FALSE;

/**
  @brief  This macro resets the powersave filtering auto exit mode setting of a
          given iface to FALSE

  @param[in]  this_iface_ptr - Pointer to iface control block

  @code
  @endcode
*/
#define PS_IFACEI_RESET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( this_iface_ptr )          \
 (this_iface_ptr)->iface_private.powersave_filtering_settings.enable_auto_exit_mode   \
   = FALSE;

/**
  @brief  This macro sets the low_latency timer handle

  @param[in]  timer_hdl - Timer handle allocated

  @code
  @endcode
*/
int ps_ifacei_set_low_latency_timer_handle
( 
  ps_iface_ipfltr_handle_type timer_hdl 
);

/**
  @brief  This macro retrieves low latency timer handle

  @code
  @endcode
*/
ps_iface_ipfltr_handle_type ps_ifacei_get_low_latency_timer_handle
(
  void
);

/*===========================================================================
MACRO PS_IFACE_COMMON_IS_FLOW_ENABLED()

DESCRIPTION
  This macro returns whether all of iface, flow, and phys_link in question
  are flow enabled.

PARAMETERS
  iface_ptr : pointer to the interface
  flow_ptr  : ptr to a flow

RETURN VALUE
  TRUE  : if flow is enabled on all the entities
  FALSE : otherwise
===========================================================================*/
#define PS_IFACE_COMMON_IS_FLOW_ENABLED  ps_iface_common_is_flow_enabled
INLINE boolean ps_iface_common_is_flow_enabled
(
  ps_iface_type  * ps_iface_ptr,
  ps_flow_type   * ps_flow_ptr
)
{
  ps_phys_link_type  * phys_link_ptr;
  boolean              ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
    if (!PS_IFACE_IS_VALID(ps_iface_ptr) || !PS_FLOW_IS_VALID(ps_flow_ptr))
    {
      break;
    }

    phys_link_ptr = PS_FLOWI_GET_PHYS_LINK(ps_flow_ptr);
    if (NULL != phys_link_ptr)
    {
      ret_val = (PS_IFACEI_FLOW_ENABLED(ps_iface_ptr) &&
                 PS_FLOWI_IS_TX_ENABLED(ps_flow_ptr) &&
                 PS_PHYS_LINK_FLOW_ENABLED(phys_link_ptr));
    }
    else
    {
      ret_val = (PS_IFACEI_FLOW_ENABLED(ps_iface_ptr) &&
                 PS_FLOWI_IS_TX_ENABLED(ps_flow_ptr));
    }
  } while (0);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return ret_val;
} /* ps_iface_common_is_flow_enabled() */

/*===========================================================================
MACRO PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED()

DESCRIPTION
  This macro returns whether all of iface, its default flow, and its primary
  phys_link are flow enabled.

PARAMETERS
  iface_ptr : pointer to the interface

RETURN VALUE
  TRUE  : if flow is enabled on all the entities
  FALSE : otherwise
===========================================================================*/
#define PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED                          \
  ps_iface_common_default_is_flow_enabled
INLINE boolean ps_iface_common_default_is_flow_enabled
(
  ps_iface_type  * ps_iface_ptr
)
{
  ps_flow_type       * ps_flow_ptr = NULL;
  ps_phys_link_type  * phys_link_ptr = NULL;
  boolean              ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
    if (!PS_IFACE_IS_VALID(ps_iface_ptr))
    {
      break;
    }

    ps_flow_ptr   = PS_IFACEI_GET_DEFAULT_FLOW(ps_iface_ptr);
    phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(ps_iface_ptr);

    ret_val =  (PS_IFACE_FLOW_ENABLED(ps_iface_ptr) &&
                PS_FLOW_IS_TX_ENABLED(ps_flow_ptr) &&
                PS_PHYS_LINK_FLOW_ENABLED(phys_link_ptr));
  } while (0);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return ret_val;
} /* ps_iface_common_default_is_flow_enabled() */

/*===========================================================================
FUNCTION PS_IFACEI_GET_SOURCE_IFACE()

DESCRIPTION
  This macro returns a ps_iface_ptr pointing to the source iface of the
  logical iface that is specified.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  Source ps_iface_ptr if passed in iface_ptr is a logical iface.
  Null: Otherwise.
===========================================================================*/
#define PS_IFACEI_GET_SOURCE_IFACE( ps_iface_ptr)                         \
  (PS_IFACE_IS_LOGICAL( ps_iface_ptr)                                     \
     ? (ps_iface_ptr)->iface_private.assoc_iface_ptr                      \
     : NULL)

/*===========================================================================
FUNCTION PS_IFACE_FLOW_CTRL_CMD_HANDLER()

DESCRIPTION
  This function processes PS_IFACE_FLOW_CTRL_CMD.

PARAMETERS
  ps_cmd       : Must be PS_IFACE_FLOW_CTRL_CMD.
  cmd_data_ptr : cmd data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_flow_ctrl_cmd_handler
(
  ps_cmd_enum_type    ps_cmd,
  void              * cmd_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_LOW_LATENCY_TIMER_EXPIRY_CB()

DESCRIPTION
  This function is invoked when the low latency traffic status timer is expired.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/**
  @brief  This function is invoked when the low latency traffic status timer
          is expired.

  @code
  @endcode
*/
void ps_ifacei_low_latency_timer_expiry_cb
(
  void * usr_data
);


/**
  @brief  This function starts/restarts the low latency traffic status timer.

  @code
  @endcode
*/
void ps_iface_low_latency_timer_start
(
  void
);

/*===========================================================================
FUNCTION PS_IFACE_GO_NULL_CMD_EX_HANDLER()

DESCRIPTION
  This function processes PS_IFACE_GO_NULL_CTRL_CMD.

PARAMETERS
  ps_cmd          : Must be PS_IFACE_GO_NULL_CTRL_CMD.
  ps_cmd_data_ptr : cmd data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_go_null_cmd_ex_handler
(
  ps_cmd_enum_type    ps_cmd,
  void              * ps_cmd_data_ptr
 );


/*===========================================================================
FUNCTION PS_IFACE_GO_NULL_CMD_EX()

DESCRIPTION
  This function is used to stop an iface. Its just work around called by 
  EPC framework only for issue where modehandler gets the bring up
  and tear down in different command.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_go_null_cmd_ex
(
  ps_iface_type *this_iface_ptr,
  void          *client_data_ptr
);

#ifdef FEATURE_DATA_RM_NET
/*===========================================================================
  FUNCTION PS_IFACE_FILL_NEXT_HOP_ADDR()

DESCRIPTION
  This function fills next hop address from either Pkt info or from iface
  pointer. Mainly used by Broadcast scenarios[Currently RmNet!]

  This function is introduced to avoid duplication in IFACE, IP Frag layers

DEPENDENCIES
  A valid Metainfo must be passed in which contain pkt info.
  A valid iface pointer must be passed in used in non-mcast scenarios.

PARAMETERS
  iface_ptr   - Iface pointer
  mi_ref_ptr  - Reference to meta info block
  errno       - Error value returned by the function

RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error

  Currently this function is used only for RmNet cases - can be opened for
  all - if required!
===========================================================================*/
int ps_iface_fill_next_hop_addr
(
  ps_iface_type                        *iface_ptr,
  ps_tx_meta_info_type                 *meta_info_ptr,
  errno_enum_type                      *ps_errno
);
#endif /* FEATURE_DATA_RM_NET */

#ifdef FEATURE_DATACOMMON_PS_IFACE_IO
int ps_ifacei_default_tx_cmd
(
  ps_iface_type        *this_iface_ptr,
  dsm_item_type       **pkt_ref_ptr,
  ps_tx_meta_info_type *meta_info_ptr,
  void                 *tx_cmd_info
);

int ps_ifacei_logical_default_tx_cmd
(
  ps_iface_type         *this_iface_ptr,
  dsm_item_type        **pkt_ref_ptr,
  ps_tx_meta_info_type  *meta_info_ptr,
  void                  *tx_cmd_info
);

/*===========================================================================
FUNCTION PS_IFACE_INPUT_LEGACY()

DESCRIPTION
   Interface client gives the protocol stack a packet to transmit.

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX/RX meta information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_input_legacy
(
  ps_iface_type         *this_iface_ptr,
  dsm_item_type        **pkt_ref_ptr,
  ps_meta_info_type_ex  *meta_info_ex_ptr
);

#endif /* FEATURE_DATACOMMON_PS_IFACE_IO */

extern int ps_iface_arbitration_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION ps_iface_log_qos_spec_params

DESCRIPTION
This function printf the qos spec params

PARAMETERS
None.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void ps_iface_log_qos_spec_params
(
  const qos_spec_type*         qos_spec
);

/*===========================================================================
FUNCTION ps_iface_log_qos_spec_filter_params

DESCRIPTION
This function prints the filter specifications

PARAMETERS
fltr_buf_ptr   - Pointer to PS QOS spec

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void ps_iface_log_qos_spec_filter_params
(
  const ip_filter_type    *fltr_buf_ptr        /* Pointer to PS QOS spec   */
);


/*===========================================================================
FUNCTION PS_IFACEI_COPY_SIP_SERV_ADDR()

DESCRIPTION
   Copyies sip serv address from one iface to another

PARAMETERS
  dst_iface_ptr   :  ptr to destination iface.
  src_iface_ptr   :  ptr to source iface.

RETURN VALUE
  0: copied successfully
 -1: failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ifacei_copy_sip_serv_addr
(
  ps_iface_type  *dst_iface_ptr,
  ps_iface_type  *src_iface_ptr
);

/**
  @brief  This function is used to get the powersave mode of ipfilters on the
          given interface.

  @param[in]  this_iface_ptr:   ptr to interface control block to operate on.
  @param[out] powersave_mode:   powersave filtering mode for a given iface
  @param[out] ps_errno          Failure reason.
                                DS_EFAULT : Invalid arguments
                                DS_NOMEMORY : Out of memory
                                DS_EINVAL : Invalid Operation

  @return     -1                On Failure.
               0                On Success.

  @code
  @endcode
*/
int ps_iface_get_powersave_filtering_mode
(
  ps_iface_type *this_iface_ptr,
  boolean       * powersave_mode,
  sint15        *ps_errno
);

/**
  @brief  This function is used to set the powersave mode of ipfilters on the
          given interface.

  @param[in]  this_iface_ptr:   ptr to interface control block to operate on.
  @param[out] powersave_mode:   powersave filtering mode for a given iface
  @param[out] ps_errno          Failure reason.
                                DS_EFAULT : Invalid arguments
                                DS_NOMEMORY : Out of memory
                                DS_EINVAL : Invalid Operation

  @return     -1                On Failure.
               0                On Success.

  @code
  @endcode
*/
int ps_iface_set_powersave_filtering_mode
(
  ps_iface_type * this_iface_ptr,
  boolean         powersave_mode,
  sint15        * ps_errno
);

/**
  @brief  This function is used to set the auto exit mode of ipfilters 
          installed.

  @param[in]  this_iface_ptr:   ptr to interface control block to operate on.
  @param[out] auto_exit_mode:   powersave auto_exit_mode mode for a given iface
  @param[out] ps_errno          Failure reason.
                                DS_EFAULT : Invalid arguments
                                DS_NOMEMORY : Out of memory
                                DS_EINVAL : Invalid Operation

  @return     -1                On Failure.
               0                On Success.

  @code
  @endcode
*/
int ps_iface_set_powersave_filtering_auto_exit_mode
(
  ps_iface_type *this_iface_ptr,
  boolean       auto_exit_mode,
  sint15        *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACEI_SET_UNBRIDGE_MODE()

DESCRIPTION
  This macro is used to set the software data path flag. 
  is_unbridged flag will set to true if software path is enabled  

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.
  flag : software data path enabled/disabled

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_UNBRIDGE_MODE(this_iface_ptr, is_sw_path_enabled)           \
  {                                                                              \
    (this_iface_ptr)->iface_private.is_unbridged = is_sw_path_enabled;           \
  }

/*===========================================================================
FUNCTION PS_IFACEI_GET_UNBRIDGE_MODE()

DESCRIPTION
  This macro gets the is_unbridged flag for an iface.

PARAMETERS
  this_iface: pointer to the interface in question.

RETURN VALUE
  None.
===========================================================================*/
#define PS_IFACEI_GET_UNBRIDGE_MODE(this_iface_ptr)                   \
  (this_iface_ptr)->iface_private.is_unbridged                       \

/*===========================================================================
FUNCTION PS_IFACEI_INC_UNBRIDGE_MODE_REF_CNT()

DESCRIPTION
  This macro is used to increment the unbridge mode reference count

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_INC_UNBRIDGE_MODE_REF_CNT(this_iface_ptr)           \
  {                                                                   \
     PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  \
    ((this_iface_ptr)->iface_private.unbridge_ref_count)++;           \
     PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                  \
  }

/*===========================================================================
FUNCTION PS_IFACEI_DEC_UNBRIDGE_MODE_REF_CNT()

DESCRIPTION
  This macro is used to decrement the unbridge mode reference count

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_DEC_UNBRIDGE_MODE_REF_CNT(this_iface_ptr)           \
  {                                                                   \
     PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  \
     if(((this_iface_ptr)->iface_private.unbridge_ref_count) > 0)     \
     {                                                                \
      ((this_iface_ptr)->iface_private.unbridge_ref_count)--;         \
     }                                                                \
     PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                  \
  }

/*===========================================================================
FUNCTION PS_IFACEI_GET_UNBRIDGE_MODE_REF_CNT()

DESCRIPTION
  This macro gets the unbridge_ref_count  for an iface.

PARAMETERS
  this_iface: pointer to the interface in question.

RETURN VALUE
  None.
===========================================================================*/
#define PS_IFACEI_GET_UNBRIDGE_MODE_REF_CNT(this_iface_ptr)            \
  (this_iface_ptr)->iface_private.unbridge_ref_count;

/*===========================================================================
FUNCTION PS_IFACEI_GET_UNBRIDGE_MODE_REF_CNT()

DESCRIPTION
  This macro set the unbridge_ref_count  for an iface.

PARAMETERS
  this_iface: pointer to the interface in question.

RETURN VALUE
  None.
===========================================================================*/
#define PS_IFACEI_SET_UNBRIDGE_MODE_REF_CNT(this_iface_ptr, unbridge_ref_cnt)            \
  (this_iface_ptr)->iface_private.unbridge_ref_count = unbridge_ref_cnt;

#ifdef __cplusplus
}
#endif

#endif /* PS_IFACEI_H */
