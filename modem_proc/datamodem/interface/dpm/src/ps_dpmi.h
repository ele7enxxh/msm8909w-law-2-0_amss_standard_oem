#ifndef _PS_DPMI
#define _PS_DPMI
/*===========================================================================

                     PS_DPM . H
DESCRIPTION
Header file describing global DPM config definitions.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpmi.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    bvd     Initial Version
=========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_dpm.h"
#include "ps_ifacei.h"
#include "ps_metai_info.h"
#include "ds_flow_control.h"
#include "ps_ifacei_utils.h"
#include "ps_system_heap.h"
#include "ps_sys_ioctl.h"
#include "ps_rm_defs.h"
#include "ps_dpm_dpl.h"
#include "ps_dpm_opt_defs.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_dpm_clat.h"
#endif /* FEATURE_DATA_PS_464XLAT */

#define PS_DPM_MAX_UID           32
#define PS_DPM_MAX_BEARERS        8
#define PS_DPM_MAX_PDN_SESSIONS  16 //MSR TODO rename
#define PS_DPM_UL_MAX_SIGNALS    PS_DPM_MAX_PDN_SESSIONS
#define PS_DPM_DL_MAX_SIGNALS    PS_DPM_MAX_BEARERS

/**
  @brief DPM state machine
*/
typedef enum
{
  PS_DPM_DISABLED      = 0,
  PS_DPM_RM_CONFIGURED = 1,
  PS_DPM_UM_CONFIGURED = 2,
  PS_DPM_UM_RM_CONFIGURED
} ps_dpm_state_enum_type;

typedef enum
{
  PS_DPM_QOS_FORMAT_NONE   = 0,
  PS_DPM_QOS_FORMAT_6_BYTE = 1,
  PS_DPM_QOS_FORMAT_8_BYTE = 2
} ps_dpm_qos_format;

/**
  @brief Data type that holds DPM info per bearer
*/
typedef struct
{
  ps_phys_link_type   * ps_phys_link_ptr;
  dsm_watermark_type  * l2_to_ps_wm_ptr;
  q_type                hw_to_ps_wm_q;
  dsm_watermark_type    hw_to_ps_wm;
  int32                 handle;
  ps_sig_enum_type      dpm_dl_sig;
  uint8                 uid;
  uint32                cont_err_pkt_cnt;
  uint32                cont_valid_pkt_cnt;
  boolean               hfn_resync_flag;
  boolean               is_bridged;
} ps_dpm_bearer_info_type;

typedef struct ps_dpm_rm_info_s ps_dpm_rm_info_type;

/**
  @brief Data type that holds DPM info which is per PDN
  @see   ps_dpm_global_info
*/
typedef struct
{
  ps_iface_type       * v4_iface_ptr;
  ps_iface_type       * v6_iface_ptr;

  /*-------------------------------------------------------------------------
    Holds array of Bearers per pdn
  -------------------------------------------------------------------------*/
  ps_dpm_bearer_info_type  * bearer_ptr_arr[PS_DPM_MAX_BEARERS];

  ps_dpm_rm_info_type      * rm_info_ptr;

  int32                      handle;
  ps_sys_rat_ex_enum_type    rat;
  boolean                    is_bridged_with_hw;
  uint8                      num_bearer;
 
  uint8                      all_bearers_flushed;

  boolean                    is_hw_bridging_allowed;
} ps_dpm_um_info_type;

struct ps_dpm_rm_info_s
{
  int32                  handle;
  ps_iface_type        * v4_iface_ptr;
  ps_iface_type        * v6_iface_ptr;
  
  ps_iface_type        * v4_logical_iface_ptr;
  ps_iface_type        * v6_logical_iface_ptr;
  
  sio_port_id_type       sio_port_id;
  sio_stream_id_type     sio_stream_id;
  dsm_watermark_type   * sio_tx_wm_ptr;
  dsm_watermark_type   * sio_rx_wm_ptr;

  ps_phys_link_type    * v4_phys_link_ptr;  
  ps_phys_link_type    * v6_phys_link_ptr;
 
  /*-------------------------------------------------------------------------
    Stores the Outstanding Pkt in data path when flow is controlled.
    Packet will be routed to the right PDN by storing it here.
  -------------------------------------------------------------------------*/
  dsm_item_type       * rx_pkt;

  ps_rm_sig_enum_type                dpm_ul_sig;

  rmnet_sm_link_prot_e_type          link_prot;
  uint8                              data_format;
  boolean                            te_flow_control; //Indicates whether TE will do flow control
                                                      //only but no QOS header addition.
  ps_dpm_qos_format                  qos_header_format;
  rmnet_data_agg_enum_type           data_agg_protocol;
  boolean                            is_eth_addr_configured;

  ps_dpm_um_info_type  * v4_um_info_ptr;
  ps_dpm_um_info_type  * v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    LAN LLC Parameters
  -------------------------------------------------------------------------*/
  lan_lle_enum_type    v4_lan_llc_instance;
  lan_lle_enum_type    v6_lan_llc_instance;

  llc_frm_packed_mode_zero_type * v4_eth_hdr_ptr;
  llc_frm_packed_mode_zero_type * v6_eth_hdr_ptr;


  void * v4_iface_flow_enabled_buf_ptr;
  void * v6_iface_flow_enabled_buf_ptr;

  void * v4_flow_tx_enabled_buf_ptr;
  void * v6_flow_tx_enabled_buf_ptr;

  void * v4_phys_link_flow_enabled_buf_ptr;
  void * v6_phys_link_flow_enabled_buf_ptr;
  
  void * v4_handoff_success_buf_ptr;
  void * v6_handoff_success_buf_ptr;  
  
  void * v6_prefix_updated_buf_ptr;
  
  /*-------------------------------------------------------------------------
    Prefix rule handle associated with v6 calls
  -------------------------------------------------------------------------*/  
  void * v6_prefix_comp_ipfltr_hdl;
};

/**
  @brief Data type that holds global stats which consists total packets count
         including uplink and downlink on all the RAT's.
         also holds a variable which will have the current number
         clients registered for data activity resumed event.
*/
typedef struct
{
  ps_sys_ioctl_global_stats_type global_stats;
  uint8                          reg_clnt_cnt;
}ps_dpm_global_stats_info_type;


/**
  @brief Data type that holds global DPM info for all existing UID. There
         is one unique entry per tuple {bearer id, subscription id and
         iface_ptr}
  @see   ps_dpm_info
*/
typedef struct
{
  int32                  num_dpm_um_info_ptr;
  int32                  num_dpm_rm_info_ptr;
  ps_dpm_um_info_type  * dpm_um_info_ptr_arr[PS_DPM_MAX_PDN_SESSIONS];
  ps_dpm_rm_info_type  * dpm_rm_info_ptr_arr[PS_DPM_MAX_PDN_SESSIONS];
  ps_dpm_global_stats_info_type   dpm_global_stats[PS_SYS_SUBS_MAX-1];
} ps_dpm_global_info_type;

/**
  @brief Data type associated with bearer tech change event in DPM
*/
typedef struct
{
  sio_stream_id_type             sio_stream_id;
  ps_iface_type                * rm_iface_ptr;
} ps_dpm_handoff_success_ev_info_type;


/**
  @brief Data type associated with prefix added event in DPM
*/
typedef struct
{
  sio_stream_id_type             sio_stream_id;
  ps_iface_type                * um_iface_ptr;
  ps_iface_prefix_update_enum_type     prefix_update_kind;
} ps_dpm_prefix_updated_ev_info_type;


/**
  @brief Data type associated with PS_DPM_RESUME_HW_PATH_PER_PDN_CMD
*/
typedef struct
{
  ps_iface_type     * iface_ptr;
} ps_dpm_resume_hw_path_per_pdn_info_type;

/**
  @brief Data type associated with PS_DPM_SUSPEND_HW_PATH_PER_PDN_CMD
*/
typedef struct
{
  ps_iface_type     * iface_ptr;
} ps_dpm_suspend_hw_path_per_pdn_info_type;

/**
  @brief DPM command type
*/
typedef enum
{
  PS_DPM_UM_REG_BEARER_CMD          =  0,
  PS_DPM_UM_REG_IFACE_CMD           =  1,
  PS_DPM_RM_REG_IFACE_CMD           =  2,
  PS_DPM_UM_DEREG_BEARER_CMD        =  3,
  PS_DPM_UM_DEREG_IFACE_CMD         =  4,
  PS_DPM_RM_DEREG_IFACE_CMD         =  5,
  PS_DPM_UM_REREG_BEARER_CMD        =  6,
  PS_DPM_RM_CONFIG_ETH_HDR_CMD      =  7,
  PS_DPM_SUSPEND_HW_PATH_CMD        =  8,
  PS_DPM_RESUME_HW_PATH_CMD         =  9,
  PS_DPM_UM_REG_TLB_BEARER_CMD      =  10,
  PS_DPM_HANDOFF_SUCCESS_EV_CMD        =  11,
  PS_DPM_HW_TO_PS_WMK_GONE_EMPTY_CMD =  12,
  PS_DPM_SIO_TX_WMK_GONE_EMPTY_CMD  = 13,
  PS_DPM_SUSPEND_HW_PATH_PER_PDN_CMD =  14,
  PS_DPM_RESUME_HW_PATH_PER_PDN_CMD =  15,
  PS_DPM_PREFIX_UPDATED_EV_CMD        =  16,
#ifdef FEATURE_DATA_PS_464XLAT
  PS_DPM_CLAT_PDN_CONTEXT_EV_CMD        =  17,
  PS_DPM_CLAT_GLOBAL_CONTEXT_EV_CMD     =  18,
  PS_DPM_CLAT_PDN_REREG_EV_CMD          =  19,
#endif /* FEATURE_DATA_PS_464XLAT */
  PS_DPM_MAX_CMD
} ps_dpm_cmd_enum_type;

/**
  @brief Data Type describing DPM cmd info
*/
//MSR TODO What are the tradeoffs of doing this?
typedef struct
{
  ps_dpm_cmd_enum_type  dpm_cmd;

  union
  {
    ps_dpm_um_reg_bearer_info_type      um_reg_bearer_info;
    ps_dpm_um_reg_iface_info_type       um_reg_iface_info;
    ps_dpm_rm_reg_iface_info_type       rm_reg_iface_info;
    ps_dpm_um_dereg_bearer_info_type    um_dereg_bearer_info;
    ps_dpm_um_dereg_iface_info_type     um_dereg_iface_info;
    ps_dpm_rm_dereg_iface_info_type     rm_dereg_iface_info;
    ps_dpm_um_rereg_bearer_info_type    um_rereg_bearer_info;
    ps_dpm_rm_eth_hdr_info_type         rm_eth_hdr_info;
    ps_dpm_um_reg_tlb_bearer_info_type  um_reg_tlb_bearer_info;
    ps_dpm_handoff_success_ev_info_type handoff_success_ev_info;
    int32                               hw_to_ps_wmk_empty_info;
    int32                               sio_tx_wmk_empty_info;
    ps_dpm_suspend_hw_path_per_pdn_info_type suspend_hw_path_per_pdn_info;
    ps_dpm_resume_hw_path_per_pdn_info_type  resume_hw_path_per_pdn_info;
    ps_dpm_prefix_updated_ev_info_type      prefix_updated_ev_info;
#ifdef FEATURE_DATA_PS_464XLAT
    ps_dpm_clat_reg_pdn_cntxt_info_type     clat_pdn_cntxt_ev_info;
    ps_dpm_clat_global_cntxt_info_type      clat_global_context_ev_info;
    ps_dpm_clat_reg_pdn_cntxt_info_type     clat_pdn_cntxt_rereg_ev_info;
#endif /* FEATURE_DATA_PS_464XLAT */ 
  } dpm_cmd_info;

} ps_dpm_cmd_info_type;

/**
  @brief Data Type describing DL path data info
*/
typedef struct
{
  int32    bearer_handle;
  int32    um_handle;
} ps_dpm_dl_user_data_type;

typedef PACKED struct PACKED_POST
{
  uint8           version;
  uint8           flags;
  uint32          qos_handle;
} ps_dpm_qos_hdr_type;

typedef PACKED struct PACKED_POST
{
  ps_dpm_qos_hdr_type hdr;
  uint8               reserved[2];
} ps_dpm_qos2_hdr_type;

typedef enum
{
  PS_DPM_QOS_DISABLED = 0,
  PS_DPM_QOS_ENABLED  = 1
} ps_dpm_qos_data_format;


/**
  @brief
   Below macros sets,resets,gets hfn_resync_flag

  @param[in] bearer_ptr : pkt recived on bearer

  @return  NA

  @code
  @endcode
*/
#define PS_DPMI_SET_HFN_RESYNC_FLAG(bearer_ptr)    \
              ((ps_dpm_bearer_info_type*)bearer_ptr)->hfn_resync_flag = TRUE

#define PS_DPMI_RESET_HFN_RESYNC_FLAG(bearer_ptr)    \
              ((ps_dpm_bearer_info_type*)bearer_ptr)->hfn_resync_flag = FALSE

#define PS_DPMI_GET_HFN_RESYNC_FLAG(bearer_ptr)    \
              ((ps_dpm_bearer_info_type*)bearer_ptr)->hfn_resync_flag
  

/**
  @brief
   Increments continuous packet drop count on bearer and invokes
   mode handlers callback if count reaches threshold.

  @param[in] bearer_ptr : pkt recived on bearer
    param[in].err_info:        contains threshold and MH CB to be invoked

  @return  void

  @code
  @endcode
*/
#define PS_DPMI_INC_CONT_PKT_DROP_CNT ps_dpmi_inc_cont_pkt_drop_cnt
INLINE void ps_dpmi_inc_cont_pkt_drop_cnt
(
  ps_dpm_bearer_info_type * bearer_ptr,
  ps_dpm_cont_err_pkt_cb_info err_info 
)
{
  if( bearer_ptr != NULL && 
      err_info.err_ind_cb_fn_ptr != NULL && 
      bearer_ptr->is_bridged != TRUE )
  {
    bearer_ptr->cont_err_pkt_cnt++;
    bearer_ptr->cont_valid_pkt_cnt = 0;
    if( bearer_ptr->cont_err_pkt_cnt >= err_info.err_pkt_threshold )
    {
      err_info.err_ind_cb_fn_ptr(bearer_ptr->ps_phys_link_ptr,FALSE);
      PS_DPMI_SET_HFN_RESYNC_FLAG(bearer_ptr);
      bearer_ptr->cont_err_pkt_cnt = 0;
    }
  }
}

/**
  @brief
   Increments continuous valid packet  count on bearer and invokes
   mode handlers callback if count reaches threshold.

  @param[in] bearer_ptr : pkt recived on bearer
    param[in].err_info:        contains threshold and MH CB to be invoked

  @return  void

  @code
  @endcode
*/
#define PS_DPMI_INC_CONT_VALID_PKT_CNT ps_dpmi_inc_cont_valid_pkt_cnt
INLINE void ps_dpmi_inc_cont_valid_pkt_cnt
(
  ps_dpm_bearer_info_type       * bearer_ptr,
  ps_dpm_cont_err_pkt_cb_info     err_info 
  
)
{
  if( bearer_ptr != NULL && 
      err_info.err_ind_cb_fn_ptr != NULL &&
      !( bearer_ptr->cont_err_pkt_cnt = 0 ) && /* intentional assginment */
      PS_DPMI_GET_HFN_RESYNC_FLAG(bearer_ptr) &&
      bearer_ptr->is_bridged != TRUE ) 
  {
    bearer_ptr->cont_valid_pkt_cnt++;
    if( bearer_ptr->cont_valid_pkt_cnt >= err_info.err_pkt_threshold )
    {
      bearer_ptr->cont_valid_pkt_cnt = 0;
      err_info.err_ind_cb_fn_ptr(bearer_ptr->ps_phys_link_ptr,TRUE);
      PS_DPMI_RESET_HFN_RESYNC_FLAG(bearer_ptr);
    }
  }
}

/**
  @brief  This API sets up legacy wms.

  @code
  @endcode
*/
void ps_dpm_legacy_setup_ul_wm
(
  void
);

/**
  @brief  This API handles legacy packets and sends up the stack

  @param[in] sig         PS Signal
  @param[in] user_data   User data

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
boolean ps_dpm_legacy_sig_hdlr
(
  ps_sig_enum_type     sig,
  void               * user_data
);

/**
  @brief  This API processes qos hdr for given pkt

  @param[in] dpm_info_ptr            Ptr to DPM info entry
  @param[in] pkt_ptr                 Ptr to Packet
  @param[in] meta_info_ex_ptr_ptr    Ptr to meta info
  @param[in] qos_hdr                 Ptr to Qos header
  @param[in] ip_ver                  IP version of pkt 
  @param[out]flow_ptr                Effective flow_ptr from 
                                     qos_handle


  @return  TRUE      Atleast one call is flow enabled.
  @return  FALSE     None of call is flow enabled.

  @code
  @endcode
*/
void ps_dpm_process_qos_hdr
(
  dsm_item_type          * pkt_ptr,
  ps_iface_type          * um_iface_ptr,
  ip_version_enum_type     ip_ver,
  boolean                  is_opt,
  ps_tx_meta_info_type   * tx_meta_info_ptr,
  void                   * qos_hdr,
  ps_dpm_qos_format        qos_hdr_type,
  ps_flow_type           **flow_ptr
);

/**
  @brief  This API return dpm rm info given the handle

  @param[in] handle   handle of dpm rm entry

  @return  DPM rm entry corresponding to handle

  @code
  @endcode
*/
ps_dpm_rm_info_type * ps_dpm_get_dpm_rm_info_by_handle
(
  int32  handle
);

/**
  @brief  This API return dpm rm info given the bridge iface

  @param[in] bridge_iface_ptr   ptr to bridge iface

  @return  DPM rm entry corresponding to bridge iface.

  @code
  @endcode
*/
ps_dpm_rm_info_type * ps_dpm_get_dpm_rm_info_by_iface
(
  ps_iface_type  * rm_iface_ptr
);

/**
  @brief  This API return dpm um info given the bridge iface

  @param[in] handle   handle to dpm um info

  @return  DPM um entry corresponding to bridge iface.

  @code
  @endcode
*/
ps_dpm_um_info_type * ps_dpmi_get_dpm_um_info_by_handle
(
  int32  handle
);

/**
  @brief  This API enables data flow on all DPM RMs interface.

  @code
  @endcode
*/
void ps_dpm_rm_enable_all_flows
(
  void
);

/**
  @brief  Enables or disables logging on ipa

  @param[in] pkt_len   pkt length
  
  @return 

  @code
  @endcode
*/
void ps_dpm_control_dpl_hw_logging
(
  uint32         pkt_len
);

/**
  @brief  This API return dpm um info given the um iface

  @param[in] um_iface_ptr   ptr to um iface

  @return  DPM um entry corresponding to um iface.

  @code
  @endcode
*/
ps_dpm_um_info_type * ps_dpmi_get_dpm_um_info
(
  ps_iface_type  * um_iface_ptr
);

/**
  @brief  initializes global data stats struct and 
          registers for IOCTL with PS module.

  @code
  @endcode
*/
void ps_dpmi_global_stats_init
(
  void
);

/**
  @brief  fetches bearer info matching to phys link

  @param[in] ps_phys_link_ptr 

  @return 
    ps_dpm_bearer_info_type
    
  @code
  @endcode
*/
ps_dpm_bearer_info_type* ps_dpmi_get_bearer_from_phys_link
(
  const ps_phys_link_type	* ps_phys_link_ptr
);


#endif /* _PS_DPMI */
