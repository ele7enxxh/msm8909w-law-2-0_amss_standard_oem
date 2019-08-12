#ifndef _PS_DPM_HW
#define _PS_DPM_HW
/*===========================================================================

                     PS_DPM_HW . H
DESCRIPTION
Header file describing DPM hardware definitions.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_hw.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
=========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_dpmi.h"


extern boolean ps_dpm_hw_dpl_enabled;

/**
  @brief  Hardware packet stats enum type

*/
typedef struct
{
  struct
  {
    uint64  num_ipv4_bytes;
    uint64  num_ipv6_bytes;
    uint32  num_ipv4_pkts;
    uint32  num_ipv6_pkts;    
  } dl_stats;
} ps_dpm_hw_stats_info_type;

/**
  @brief  DPL packet direction enum type

*/
typedef enum
{
  PS_DPM_HW_DPL_PKT_DIRECTION_DL,
  PS_DPM_HW_DPL_PKT_DIRECTION_UL
} ps_dpm_hw_dpl_pkt_direction;

/* @brief Struct: DSM IP packet app field info */
typedef struct
{
  /*! Filter result */
  uint32 filter_result           : 16;
  /*! Who did the filtering (Hardware of DPM) */
  uint32 filter_source           : 1; 
  /*! If filter result is valid or not */
  uint32 is_filter_result_valid  : 1;  
  /*! Version of IP packet */
  uint32 ip_version              : 2;  
  /*! Reserved for future use */
  uint32 reserved                : 3;  
  /*! Reserved for futre use for checksum */
  uint32 is_pseudo_header_cksum_computed  : 1;
  /*! If checksum is computed or not */
  uint32 is_cksum_computed       : 1;
  /*! Checksum start offset */
  uint32 cksum_start_offset      : 7;

} ps_dpm_dsm_app_field_info_type;
/*===========================================================================
MACRO PS_DPM_HW_IS_DPL_ENABLED()

DESCRIPTION
  This macro indicates if hardware dpl logging is enabled or not

PARAMETERS

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_DPM_HW_IS_DPL_ENABLED() (TRUE == ps_dpm_hw_dpl_enabled)

/**
  @brief
Sets the checksum offload flag value

  @param[in] dsm_item_ptr        DSM packet
  @param[in] cksum_start_offset  Checksum start offset

  @return  None

  @code
  @endcode
*/
#define PS_DPM_HW_SET_CKSUM_START_OFFSET_VALUE(dsm_item_ptr, cksum_start_offset_val) \
  ((ps_dpm_dsm_app_field_info_type *)(&((dsm_item_ptr)->app_field)))->cksum_start_offset = cksum_start_offset_val;
  
/**
  @brief
  Sets the checksum offload flag value

  @param[in] dsm_item_ptr        DSM packet
  @param[in] is_cksum_computed   checksum is computed or not

  @return  None

  @code
  @endcode
*/
#define PS_DPM_HW_SET_IS_CKSUM_COMPUTED_FLAG(dsm_item_ptr, is_cksum_computed_val) \
  ((ps_dpm_dsm_app_field_info_type *)(&((dsm_item_ptr)->app_field)))->is_cksum_computed = is_cksum_computed_val;
 
/**
  @brief  This API updates stats from hardware

  @param[in] uid            UID
  @param[in] iface_ptr      Iface pointer
  @param[in] stats_ptr      Stats adjustment pointer

  @return
  0 : Success
  -1 : Failure   

  @code
  @endcode
*/
int ps_dpm_hw_get_iface_stat_adjustment
(
  uint8                            uid,
  ps_iface_type                  * iface_ptr,   
  ps_dpm_hw_stats_info_type      * stats_ptr
);

/**
  @brief  This API registers bearer with hardware

  @param[in] uid              Unique ID for bearer
  @param[in] l2_to_ps_wm_ptr  Pointer to l2_to_ps_wm
  @param[in] hw_to_ps_wm_ptr  Pointer to hw_to_ps_wm 
  @param[in] sio_stream_id    sio  stream id
  @param[in] rat              RAT technology
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_reg_bearer
(
  uint8                      uid,
  dsm_watermark_type	   * l2_to_ps_wm_ptr,
  dsm_watermark_type	   * hw_to_ps_wm_ptr,
  sio_stream_id_type         sio_stream_id,
  ps_sys_rat_ex_enum_type    rat
);

/**
  @brief  This API deregisters bearer with hardware

  @param[in] uid                       Uid of the bearer
  @param[in] sio_srteam_id             SIO port id
  @param[in] is_data_path_bridged      is data path bridged with Hardware
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_dereg_bearer
(
  uint8                     uid,
  sio_port_id_type          sio_port_id,
  boolean                   is_data_path_bridged
);

/**
  @brief  This API bridges bearer with hardware

  @param[in] uid               Uid of the bearer
  @param[in] sio_port_id       SIO port id
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int16 ps_dpm_hw_bridge_bearer
(
  uint8                            uid, 
  sio_port_id_type                 sio_port_id  
);

/**
  @brief  This API unbridges bearer with hardware
  @param[in] uid               UID
  @param[in] dpm_um_info_ptr   Ptr to dpm um info
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int16 ps_dpm_hw_unbridge_bearer
(
  uint8                            uid,
  sio_port_id_type                 sio_port_id 
);

/**
  @brief  This API configures dpl with hardware

  @param[in] uid               Uid of the bearer
  @param[in] dpl_pkt_len_v4    DPL v4 pkt length
  @param[in] dpl_pkt_len_v6    DPL v6 pkt length
  @param[in] dpm_um_handle     DPM um handle
  @param[in] iface_ptr         Ptr to iface
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_config_dpl
(
  uint8                      uid,
  uint16                     dpl_pkt_len_v4,
  uint16                     dpl_pkt_len_v6,
  uint32                     dpm_um_handle,
  ps_iface_type            * iface_ptr
);


/**
  @brief  This API unregisters dpl with hardware

  @param[in] uid               Uid of the bearer

  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
void ps_dpm_hw_dereg_dpl
(
  uint8                      uid
);

/**
  @brief  This API suspends bearer with hardware

  @param[in] uid       Uid of the bearer
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_suspend_bearer
(
  uint8   uid
);

/**
  @brief  This API registers callback with hardware

  @param[in] None
  
  @return None

  @code
  @endcode
*/
void ps_dpm_hw_reg_event_cb
(
  void
);


/**
  @brief  This API activates bearer with hardware

  @param[in] uid       Uid of the bearer
  
  @return None

  @code
  @endcode
*/
void ps_dpm_hw_activate_bearer
(
  uint8   uid
);

/**
  @brief  This function waits for hardware for response
          and activates signal

  @param None
  
  @return None

  @code
  @endcode
*/
void ps_dpmi_hw_wait_for_ev_cback
(
  void
);

/**
  @brief  This function is called during powerup
  
  @return

  @code
  @endcode
*/
void ps_dpm_hw_dpl_powerup_init
(
  void
);

/**
  @brief  This function is called by DPM to configure sio stream id
          with hardware
  
  @param[in] sio_stream_id    Stream id
  @param[in] ifname           dpl iface name
  @param[in] num_bytes        Number of bytes to log

  @return

  @code
  @endcode
*/
void ps_dpm_hw_config_dpl_sio
(
  sio_stream_id_type  sio_stream_id,
  uint8               ifname,
  uint16              num_bytes
);

/**
  @brief  This function logs packet with hardware
  
  @param[in] iface_ptr     Pointer to iface
  @param[in] rx_pkt        DSM item to be logged
  @param[in] pkt_direction Direction of the packet

  @return

  @code
  @endcode
*/
void ps_dpm_hw_log_dpl_pkt
(
  ps_iface_type                * iface_ptr,
  dsm_item_type                * rx_pkt,
  ps_dpm_hw_dpl_pkt_direction    pkt_direction
);

/**
  @brief  This function enables or disables dpl logging with 
          hardware
  
  @param[in] pkt_len     pkt length
  
  @return

  @code
  @endcode
*/
void ps_dpm_hw_control_dpl_logging
(
  uint32         pkt_len
);

/**
  @brief  This function is used to register the DPM DPL WM with 
          hardware
          
  @param[in] dpl_wmk_ptr - Pointer to ps_dpm_dpl_wmk
  
  @return

  @code
  @endcode
*/
void ps_dpm_hw_reg_dpl_wmk
(
  dsm_watermark_type *dpl_wmk_ptr
);

/**
  @brief  Signal handler for PS_DPM_DPL_SIGNAL
   
  @param[in] sig  - Signal
  @param[in] user_data_ptr  - User data pointer (unused) 
   
  @return  boolean   TRUE: no more processing to be done
                     FALSE: Needs to be called again.

  @code
  @endcode
*/
boolean ps_dpm_hw_dpl_sig_handler
(
  ps_sig_enum_type    sig,
  void              * user_data_ptr
);

#ifdef FEATURE_DATA_PS_464XLAT
/**
  @brief  This function sets the clat context informtion
          per pdn on the hardware

  @param[in] clat_reg_info_ptr         clat pdn context
  @param[out] clat_hw_handle           clat hardware handle
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_clat_reg_pdn_context
(
  ps_dpm_clat_reg_pdn_cntxt_info_type       * clat_reg_info_ptr,
  void                                     ** clat_hw_handle
);

/**
  @brief  This function sets the clat global configuration
          on the hardware

  @param[in] clat_global_cntxt_info  clat global config
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_clat_reg_global_context
(
  ps_dpm_clat_global_cntxt_info_type   * clat_global_cntxt_info
);

/**
  @brief  This function asociates the sio stream id with the 
          clat pdn context

  @param[in] clat_hw_handle  clat hw handle
  @param[in] sio_stream_id   stream id

  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_clat_assoc_sio_stream
(
  void                                 * clat_hw_handle,
  sio_stream_id_type                     sio_stream_id
);

/**
  @brief  This function reconfigures the clat context informtion
          per pdn on the hardware

  @param[in] clat_reg_info_ptr         clat pdn context
  @param[in] clat_hw_handle            Hardware handle
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
int ps_dpm_hw_clat_rereg_pdn_context
(
  ps_dpm_clat_reg_pdn_cntxt_info_type       * clat_reg_info_ptr,
  void                                      * clat_hw_handle
);

/**
  @brief  This function deregisters the clat context information
          per pdn on the hardware

  @param[in] clat_hw_handle            Hardware handle
  
  @return 
  0 : Success
  -1 : Failure

  @code
  @endcode
*/
void ps_dpm_hw_clat_dereg_pdn_context
(
  void     * clat_hw_handle
);
#endif /* FEATURE_DATA_PS_464XLAT */

/**
  @brief  Registers CB with HW for HFN error indication
             call back would be invoked in two cases
             1)when continuous invalid packet count
             reaches threshold.
             2)first valid packet after invalid count threshold.
   
  @param[in] error_pkt_threshold:   threshold count
  @param[in] err_ind_cb_fn:            call back to be invoked by HW
   
  @return  TRUE : Pkt version is v6 
  
  @code
  @endcode
*/
void ps_dpm_hw_register_bearer_err_ind
(
  uint32                           err_pkt_threshold,
  void*                            err_ind_cb_fn_ptr
);
	
/**
  @brief  Deregisters CB with HW registered for HFN error indication.
   
  @return  NA
  
  @code
  @endcode
*/
void ps_dpm_hw_deregister_bearer_err_ind
(
  void
);

#endif /* _PS_DPM_HW */
