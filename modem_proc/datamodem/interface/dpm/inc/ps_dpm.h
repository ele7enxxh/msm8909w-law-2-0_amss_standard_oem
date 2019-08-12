#ifndef PS_DPM_H
#define PS_DPM_H
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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/inc/ps_dpm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    bvd     Initial Version
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_iface_defs.h"
#include "ds_rmnet_meta_sm.h"
#include "dsm.h"
#include "ps_sys.h"
#include "ps_lan_llci.h"
#include "ps_ifacei.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/**
  @brief Enum describing DPM error codes
*/
typedef enum
{
  PS_DPM_SUCCESS  = 0x0,
  
  /* DPM general error codes */
  PS_DPM_ERROR_ARG_INVALID ,
  PS_DPM_ERROR_MEM_ALLOC_FAILED,
  PS_DPM_ERROR_IFACE_INVALID,
  PS_DPM_ERROR_DPM_UM_INFO_NULL,
  PS_DPM_ERROR_DPM_RM_INFO_NULL,
  PS_DPM_ERROR_CLAT_CNTXT_NULL,
  
  /* Hardware specific */
  PS_DPM_ERROR_HARDWARE_OP_FAILED,
  PS_DPM_ERROR_HARDWARE_OP_NOT_SUPPORTED,
  
  PS_DPM_ERROR_MAX
} ps_dpm_error_enum_type;

/**
  @brief Enum describing different global um dpm event
*/
typedef enum
{
  PS_DPM_UM_EV_BEARER_REG_COMPLETE,
  PS_DPM_UM_EV_IFACE_REG_COMPLETE,
  PS_DPM_UM_EV_BEARER_DEREG_COMPLETE,
  PS_DPM_UM_EV_IFACE_DEREG_COMPLETE,
  PS_DPM_UM_EV_BEARER_REREG_COMPLETE,
} ps_dpm_um_ev_enum_type;

/**
  @brief Enum describing different source of disable hw path 
*/
typedef enum
{
  PS_DPM_DISABLE_HW_PATH_NONE        = 0x0,
  PS_DPM_DISABLE_HW_PATH_NV          = 0x1,
  PS_DPM_DISABLE_HW_PATH_SYS_IOCTL   = 0x2,
  PS_DPM_DISABLE_HW_PATH_IFACE_IOCTL = 0x3,
  
} ps_dpm_disable_hw_path_enum_type;

/**
  @brief Data type associated with registering UM with DPM
  @see   ps_dpm_um_reg
*/
typedef struct
{
  ps_iface_type            * v4_iface_ptr;
  ps_iface_type            * v6_iface_ptr;
  ps_phys_link_type        * ps_phys_link_ptr;
  dsm_watermark_type       * l2_to_ps_wm_ptr;  //l2_to_ipa  DL wmk
  ps_sys_rat_ex_enum_type    rat;
} ps_dpm_um_reg_bearer_info_type;

/**
  @brief Data type associated with registering UM with DPM
  @see   ps_dpm_um_reg
*/
typedef struct
{
  ps_iface_type            * v4_iface_ptr;
  ps_iface_type            * v6_iface_ptr;
  ps_phys_link_type        * ps_phys_link_ptr;
  dsm_watermark_type       * l2_to_ps_wm_ptr;  //l2_to_ipa  DL wmk
  ps_sys_rat_ex_enum_type    rat;
} ps_dpm_um_rereg_bearer_info_type;

/**
  @brief Data type associated with registering UM with DPM
  @see   ps_dpm_um_reg
*/
typedef struct
{
  ps_iface_type            * v4_iface_ptr;
  ps_iface_type            * v6_iface_ptr;
  ps_phys_link_type        * ps_phys_link_ptr;
  dsm_watermark_type       * l2_to_ps_wm_ptr;  //l2_to_ps  DL wmk
  dsm_watermark_type       * hw_to_ps_wm_ptr;  //hw_to_ps  DL wmk
  ps_sys_rat_ex_enum_type    rat;
} ps_dpm_um_reg_tlb_bearer_info_type;


/**
  @brief Data type associated with registering UM with DPM
  @see   ps_dpm_um_reg
*/
typedef struct
{
  ps_iface_type  * registered_iface_ptr;
  ps_iface_type  * iface_ptr;
} ps_dpm_um_reg_iface_info_type;

/**
  @brief Data type associated with registering RM with DPM
  @see   ps_dpm_rm_reg
*/
typedef struct
{
  ps_iface_type                * rm_iface_ptr;
  dsm_watermark_type           * sio_rx_wm_ptr;
  dsm_watermark_type           * sio_tx_wm_ptr;
  rmnet_sm_link_prot_e_type      link_prot;
  rmnet_data_agg_enum_type       data_agg_protocol;
  lan_lle_enum_type              lan_llc_instance;
  sio_stream_id_type             sio_stream_id;
  uint8                          data_format;
  boolean                        te_flow_control;
  uint8                          qos_header_format;
} ps_dpm_rm_reg_iface_info_type;

/**
  @brief Data type associated with deregistering of bearer with DPM
  @see   ps_dpm_um_dereg
*/
typedef struct
{
  ps_iface_type      * v4_iface_ptr;
  ps_iface_type      * v6_iface_ptr;
  ps_phys_link_type  * ps_phys_link_ptr;
} ps_dpm_um_dereg_bearer_info_type;


/**
  @brief Data type associated with deregistering of iface with DPM
  @see   ps_dpm_um_dereg
*/
typedef struct
{
  ps_iface_type  * v4_iface_ptr;
  ps_iface_type  * v6_iface_ptr;
} ps_dpm_um_dereg_iface_info_type;

/**
  @brief Data type associated with deregistering of DPM
  @see   ps_dpm_deregister
*/
typedef struct
{
  ps_iface_type       * rm_iface_ptr;
  sio_stream_id_type    sio_stream_id;
} ps_dpm_rm_dereg_iface_info_type;

typedef struct
{
  sio_stream_id_type               sio_stream_id;
  ps_iface_type                  * rm_iface_ptr;
  llc_frm_packed_mode_zero_type    eth_hdr;
} ps_dpm_rm_eth_hdr_info_type;

/**
  @brief Data type indicating if dpm reg/dereg is
         successful/unsuccessful.
*/
typedef struct
{
  ps_iface_type                   * v4_iface_ptr;
  ps_iface_type                   * v6_iface_ptr;
  ps_phys_link_type               * phys_link_ptr;
  ps_sys_rat_ex_enum_type           rat;
  boolean                           result;
  boolean                           is_hw_accelerated;
} ps_dpm_um_ev_cback_info_type;

/**
  @brief This is callback provided by client which is to
         be called when particular event is triggered.
*/
typedef void (* ps_dpm_um_ev_cback_fptr_type)
(
  ps_dpm_um_ev_enum_type      dpm_event,
  void                      * event_info_ptr
);


/**
  @brief This is callback provided by Mode handlers which is to
     be called when error packet threshold count reached.
     this is called by DPM(dl_opt) in couple of cases.
     1)when continuous error packet count reached threshold
     2)first valid packet after reaching threshold.
*/
typedef void (* ps_dpm_err_ind_cback_fptr_type)
(
/* phys link of the bearer */
  ps_phys_link_type         * phys_link_ptr,
/* indicate if recovered from earlier failure */
  boolean                     resync_status
);


/**
  @brief Data type to maintain mode handler 
     specific info for HFN error indication
     registered mode handlers CB would be invoked in couple of cases:
     1)When continuous invalid packet count reaches threshold
     2)first valid packet after threshold.
*/
typedef struct
{
  uint8                           err_pkt_threshold;
  ps_dpm_err_ind_cback_fptr_type  err_ind_cb_fn_ptr;
} ps_dpm_cont_err_pkt_cb_info;

/*===========================================================================

                         MACROS

===========================================================================*/
#ifdef FEATURE_DATA_PS_464XLAT
/**
  @brief
  If iface is clat iface, then retrieve assoc iface and then call 
  PS_IFACE_GET_BASE_IFACE api. For clat iface, inherit ip flag info is 
  set to false as ip address of clat is different from base iface.

  @param[in] iface_ptr    Iface pointer

  @return  Base iface pointer.

  @code
  @endcode
*/
#define PS_DPM_GET_BASE_IFACE ps_dpm_get_base_iface
INLINE ps_iface_type * ps_dpm_get_base_iface
(
  ps_iface_type        * iface_ptr
)
{
  ps_iface_type * base_iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid iface 0x%p", iface_ptr);
    return iface_ptr;
  }
  
  if (CLAT_IFACE == PS_IFACE_GET_NAME(iface_ptr))
  {
    base_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(iface_ptr);
  }
  else
  {
    base_iface_ptr = iface_ptr;
  }
  
  if (NULL != base_iface_ptr)
  {
    return PS_IFACE_GET_BASE_IFACE(base_iface_ptr);  
  }

  return iface_ptr; 
} /* ps_dpm_get_base_iface */
#endif /* FEATURE_DATA_PS_464XLAT */

#define PS_DPM_GLOBAL_STATS_INC(iface_ptr) ps_dpm_global_stats_inc(iface_ptr)

/*===========================================================================

                         EXTERNAL FUNCTIONS

===========================================================================*/
/**
  @brief  Does DPM specific initialization

  @code
  @endcode
*/
void ps_dpm_init
(
  void
);

/**
  @brief  Modehandler will Register cb with dpm for
          retrieving status of registration deregistration
          completion with DPM.

  @return  N/A

  @code
  @endcode
*/
void ps_dpm_um_ev_cback_reg
(
  ps_sys_tech_enum_type                tech_type,
  ps_dpm_um_ev_cback_fptr_type         ev_cback_fptr
);


/**
  @brief  This API allows Modehandler to register bearer with
          DPM when call is brought up.

  @param[in] reg_info_ptr    Info required to register Um side

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_um_reg_bearer
(
  ps_dpm_um_reg_bearer_info_type   * reg_info_ptr
);

/**
  @brief  This API allows Modehandler to register iface. MH
          would call when PDN goes from single IP PDN to
          dual IP PDN

  @param[in] reg_info_ptr    Info required to register iface

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_um_reg_iface
(
  ps_dpm_um_reg_iface_info_type   * reg_info_ptr
);

/**
  @brief  This API allows RMnet to register with DPM.

  @param[in] reg_info_ptr    Info required to register Rm side
  @param[out] ps_errno       Failure reason.
                             DS_EFAULT : Invalid arguments
                             DS_NOMEMORY : Out of memory
                             DS_EINVAL : Invalid Operation

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_rm_reg_iface
(
  ps_dpm_rm_reg_iface_info_type   * reg_info_ptr,
  int16                           * ps_errno
);


/**
  @brief  This API allows modehandler to deregister bearer
          with DPM

  @param[in] dereg_info_ptr    Info required to deregister

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_um_dereg_bearer
(
  ps_dpm_um_dereg_bearer_info_type  * dereg_info_ptr
);

/**
  @brief  This API allows RM side to deregister with DPM

  @param[in] dereg_info_ptr     Info required to deregister

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_rm_dereg_iface
(
  ps_dpm_rm_dereg_iface_info_type  * dereg_info_ptr
);


/**
  @brief  Modehandler will use this API to reregister the new
          watermark in case of IRAT scenario( W to LTE or vice versa)
          Rest of the entries in DPM remains the same except
          rat and l2_to_ps_wmk.

  @param[in] rereg_info_ptr   Info required to re-register

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_um_rereg_bearer
(
  ps_dpm_um_rereg_bearer_info_type  * rereg_info_ptr
);


/**
  @brief  Modehandler will use this API to deregister the
          iface with DPM. MH would call this API when PDN
          goes from dual IP to single IP

  @param[in] dereg_info_ptr     Info required to Dereg

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_um_dereg_iface
(
  ps_dpm_um_dereg_iface_info_type  * dereg_info_ptr
);

/**
  @brief  Suspends hardware path

  @return  N/A

  @code
  @endcode
*/
void ps_dpm_suspend_hw_path
(
  void
);

/**
  @brief  Resumes hardware path

  @return  N/A

  @code
  @endcode
*/
void ps_dpm_resume_hw_path
(
  void
);

/**
  @brief  This API allows would be called by rmnet when it recieves
          ethernet address. For ethernet mode, we dont bridge till
          rmnet registers ethernet with Hardware.

  @param[in] rm_iface_ptr   Ptr to rm iface

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_rm_config_eth_hdr
(
  ps_dpm_rm_eth_hdr_info_type  * eth_hdr_info_ptr
);

/**
  @brief  This API allows Modehandler to register hw_to_ps wmk 
          in loopback mode

  @param[in] rereg_info_ptr   Ptr to loopback registration info

  @return  DSS_SUCCESS      On Success.
  @return  DSS_ERROR        On failure.

  @code
  @endcode
*/
int32 ps_dpm_um_reg_tlb_bearer
(
  ps_dpm_um_reg_tlb_bearer_info_type   * reg_info_ptr
);

/**
  @brief  Suspends hardware path for the PDN associated 
          with the passed iface.

  @param[in] iface_ptr   Iface ptr 
  
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_suspend_hw_path_per_pdn
(
  ps_iface_type * iface_ptr
);

/**
  @brief  Resumes hardware path for the PDN associated 
          wtih the passed iface

  @param[in] iface_ptr   Iface ptr 
  
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_resume_hw_path_per_pdn
(
  ps_iface_type * iface_ptr
);

/**
  @brief  Returns if data path is in software or hardware mode

  @param[in] iface_ptr   Iface ptr 
  
  @return 
  1 : Software mode
  0 : Hardware mode
  -1 : Other error 

  @code
  @endcode
*/
int ps_dpm_is_data_path_software
(
  ps_iface_type * iface_ptr
);

/**
   @brief  
    Function will return companion iface
 
   @param[in] iface_ptr   Iface ptr 
   
   @return 
     iface_ptr:  Companion iface pointer
    Null: Otherwise? 
 
   @code
   @endcode
 */
 ps_iface_type * ps_dpm_get_companion_iface
(
  ps_iface_type  * um_iface_ptr
);

/**
  @brief  This API is used to transmit IP packets on downlink towards sio path
    
  @param[in] iface_ptr        -  The interface on which to transmit the packet
  @param[in] dsm_item_ptr_ptr -  message (in dsm chain) to be transmitted
  @param[in] meta_info_ptr    -  meta info associated with the packet
  @param[in] client_info      -  user data passed that contains rmnet_meta_sm info ptr
    
  @return    0      On Success.
             -1     On Failure
    
  @code
  @endcode
*/

int ps_dpm_ip_mode_tx_cmd
(
  ps_iface_type      * iface_ptr,
  dsm_item_type      ** dsm_item_ptr_ptr,
  ps_meta_info_type  * meta_info_ptr,
  void               * client_info
);

/**
  @brief  This API would execute powersave filters on every incoming pkts on rm 
  iface and then send the data on downlink to sio path
        
  @param[in] iface_ptr     -  The interface on which to transmit the packet
  @param[in] pkt_ref_ptr   -  message (in dsm chain) to be transmitted
  @param[in] meta_info_ptr -  meta info associated with the packet
  @param[in] client_info   -  user data passed that contains rmnet_meta_sm info ptr
        
  @return    0      On Success.
             -1     On Failure
        
  @code
  @endcode
*/

int ps_dpm_powersave_mode_tx_cmd
(
  ps_iface_type      * iface_ptr,
  dsm_item_type      ** pkt_ref_ptr,
  ps_meta_info_type  * meta_info_ptr,
  void               * client_info
);

/**
  @brief  Modehandler will Register cb with dpm to know 
    1)Continuous error packet count reached threshold.
    2)first valid packet after previous failure.

  @param[in] err_pkt_thresh      error packet threshold count
  @param[in] err_ind_cb_fn       MH CB to be invoked
  
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_err_ind_cback_reg
(
  uint32                         err_pkt_threshold,
  ps_dpm_err_ind_cback_fptr_type err_ind_cb_fn_ptr
);


/**
  @brief  Modehandler will deregister cb with dpm when LTE detached.
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_err_ind_cback_dereg
(
  void
);

/**
  @brief  This would be invoked by HW when continuous error packet count
          reaches threshold (or) when recieved valid packet after recovery..
          DPM gets the phys_link_ptr with the uid and invokes MH callback to
          report HFN failure/success.

  @param[in] uid:                      UID of the bearer
  @param[in] resync_status:
             resync_status would be false when UE recieved threshold number
             of invalid packets continously.
             resync_status would be true when UE recieved first valid packet after
             threshold number of invlid packets.
  
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_cont_err_pkt_ind_handler
(
  uint8      uid,
  boolean    resync_status
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/**
  @brief  Returns if DPM UM info exists corresponding to the PS Iface

  @param[in] iface_ptr   Iface ptr 
  
  @return 
   TRUE:  If Um info exist for Iface
   FALSE: Otherwise? 

  @code
  @endcode
*/

boolean ps_dpm_check_dpm_um_info_exist
(
  ps_iface_type * iface_ptr
);

/**
  @brief  Returns TRUE if data path switching is allowed 
          between HW and SW via IFACE or SYS IOCTL.   

  @param[in] disable_hw_path_mode 
  
  @return 
  1 : If allowed to switch
  0 : if disallowed to switch
  
  @code
  @endcode
*/
boolean ps_dpm_data_path_switch_allowed
(
  ps_dpm_disable_hw_path_enum_type  requested_mode
);

/**
  @brief  sets client status information in DPM Global stats structure.
          based on client status DPM decides whether
          to invoke data activity resumed system event or not.

  @param[in] subscription_id 
  @param[in] client_status 
  
  @code
  @endcode
*/
void ps_dpm_data_activity_client_status_ind
(
  ps_sys_subscription_enum_type subscription_id,
  boolean                       client_status
);

/**
  @brief  increments global stats count in dpm global info

  @param[in] subscription_id 
  @param[in] client_status 
  
  @code
  @endcode
*/
void ps_dpm_global_stats_inc
(
  ps_iface_type       * iface_ptr
);
#endif /* PS_DPM_H */
