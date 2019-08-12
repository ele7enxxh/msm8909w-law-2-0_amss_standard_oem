#ifndef DSUMTS_RMSMI_H
#define DSUMTS_RMSMI_H
/*===========================================================================

                          D S U M T S _ R M S M I . H

DESCRIPTION
  This file contains the private definitions for RM state machine applicable
  for UMTS. It contains the common definitions and structures used for both
  pdp-ip and pdp-ppp RM state machines
  
  NOTE:
  This definitions are private to DSUMTS_RMSM and should not be used by 
  the clients of DSUMTS_RMSM directly.

  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2016 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/dsumts_rmsmi.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/07/12    nd     SIO port not freed after DUN is ended in entitlement check.
06/19/12    nd     MT PDP pkt call.
02/08/12    vs     Added support for 3GPP IPv6 over PPP
06/09/11    asn    Support for new event for active Ifaces 
06/20/11    ttv    Merged silent redial support changes for UMTS tethered calls.
04/01/11    ttv    Support for Q6 free floating.
07/29/10    sa     Fixed DUN data rate reporting based on device capability.
07/09/10    asn    Support for Arbitration
03/04/09    sa     AU level CMI modifications.
09/27/08    pp     Metainfo Optimizations.
11/15/07    asn    Added support for new feature MT-PDP terminating in TE
02/02/07    rr     Fixed Lint Warnings.
06/09/06    vk     A round of Lint fixes.
08/11/05    rr     Changed signature of dsumts_rmsmi_ppprm_event_handler_cb 
                   as per PS TIP.
04/13/05    rr     Changed profile_id member of dsumts_rmsmi_info_type from
                   uint8 to uint32
11/21/04    rc     Added fields for storing phys link enable and disable 
                   buffers. Added prototype for 
                   dsumts_rmsmi_phys_link_um_event_handler_cb()  
10/14/04    ar     Add SIO port allocation semaphore.
09/10/04    sb     September PS changes
01/27/04    vsk    M-PDP support 
08/15/03    mvl    Updates for separation of phys_link.
04/22/03    vsk    code review cleanup
04/18/03    vsk    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "sys.h"
#include "dstask.h"
#include "ps_iface.h"
#include "dsm.h"
#include "ps_svc.h"
#include "dsat_v.h"

#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif /* FEATURE_QXDM_WM_TRACING */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)
#include "dsumts_rmsm.h"
#include "dsumts_rmsm_pppi.h"
#include "dsumts_rmsm_ipi.h"
#include "dsumtspdpreg.h"
#include "ds3gsiolib.h"
#include "ds_3gpp_qos.h"
#include "ps_ppp.h"
#include "modem_mem.h"
#include "ds_3gpp_rmsm_ip_common.h"
#include "ps_acl.h"
#include "dsumtsps.h"
#include "ds_sys.h"

/*---------------------------------------------------------------------------
  Critical Section macros
---------------------------------------------------------------------------*/
#define DSUMTS_INIT_CRIT_SECTION( rex_crit_section )                        \
    { rex_init_crit_sect( rex_crit_section ); }

#define DSUMTS_ENTER_CRIT_SECTION( rex_crit_section )                       \
    { rex_enter_crit_sect( rex_crit_section ); }

#define DSUMTS_BRANCH_LEAVE_CRIT_SECTION( rex_crit_section )                \
    { rex_leave_crit_sect( rex_crit_section ); }

#define DSUMTS_LEAVE_CRIT_SECTION( rex_crit_section )                       \
    { rex_leave_crit_sect( rex_crit_section ); }

#define DSUMTS_ENTER_CRIT_SECTION_NO_TASKLOCK( rex_crit_section )           \
    { rex_enter_crit_sect( rex_crit_section ); }

#define DSUMTS_LEAVE_CRIT_SECTION_NO_TASKLOCK( rex_crit_section )           \
    { rex_leave_crit_sect( rex_crit_section ); }

/*---------------------------------------------------------------------------
  Utility macros
---------------------------------------------------------------------------*/

#define DSUMTS_CONDITIONAL_BLOCK_START( cond, msg_low )                     \
        if ( cond ) {                                                       \
          MSG_LOW(msg_low, 0, 0, 0);

#define DSUMTS_CONDITIONAL_BLOCK_ELSE( cond, msg_low )                      \
        } else if ( cond ) {                                                \
          MSG_LOW(msg_low, 0, 0, 0);

#define DSUMTS_CONDITIONAL_BLOCK_END( )                                     \
        } /* end of conditional block */

/*------------------------------------------------------------------------- 
  TODO: Check if setting this proc_id is correct. Also define a new
  enum for this to allow sharing between RmNet and RmSM
-------------------------------------------------------------------------*/
#define PROC_ID_TE_DUN   0x1
#define APP_ID_TE_DUN    0x0000000100000000ULL

#define PROC_ID_APPS     0x2
#define APP_ID_APPS      0x0000000200000000ULL

#define PROC_ID_TE_RMNET 0x3
#define APP_ID_TE_RMNET  0x0000000300000000ULL

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   

/*---------------------------------------------------------------------------
  Macros to allocate and free memory from heap.
---------------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK
  #error code not present
#else
    #define DSUMTS_RMSMI_MEM_ALLOC( size, client ) modem_mem_alloc( size, client)
    #define DSUMTS_RMSMI_MEM_FREE( ptr, client ) modem_mem_free( ptr, client )
#endif /* TEST_FRAMEWORK */

#define DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr)  \
  rmsmi_info_ptr->profile_context_info.pdp_type

/*---------------------------------------------------------------------------
  The state machine type. Either PDP-IP or PDP-PPP
---------------------------------------------------------------------------*/              
typedef enum
{
  DSUMTS_RMSM_MIN_SM_TYPE = -1,                
  DSUMTS_RMSM_INVALID_SM_TYPE = -1,               
  DSUMTS_RMSM_SM_TYPE_PDP_IP  = 0,                /* PDP-IP State machine    */
  DSUMTS_RMSM_SM_TYPE_PDP_PPP = 1,                /* PDP-PPP state machine   */
  DSUMTS_RMSM_MAX_SM_TYPE    
} dsumts_rmsmi_sm_type;

/*-------------------------------------------------------------------------
    PDP-ip or PDP-ppp State machine specific info 
-------------------------------------------------------------------------*/
typedef union
{
  dsumts_rmsm_ipi_specific_info  pdp_ip;
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  dsumts_rmsm_pppi_specific_info pdp_ppp;
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
} dsumts_rmsmi_pdp_specific_info_type;

/*---------------------------------------------------------------------------
  State-Machine Specific control block. Each State Machine will have a
  unique instance of this control block.
  When we support dual-IP, there will be 2 instances of this block -
  one for V4 and one for V6.
---------------------------------------------------------------------------*/
typedef struct dsumts_rmsmi_sm_info_s
{

  dsumts_rmsm_sm_instance_type        instance;  
                                      /* instance # of the rmsm */
  boolean                             inited;
                                      /* State machine been initialized? */
  ps_iface_type*                      um_iface_ptr; 
                                      /* Pointer to associated UM iface */
  ps_phys_link_type*                  um_phys_lnk_ptr;
                                      /* Pointer to Um Phys link */

  void *um_iface_active_out_of_use_buf_ptr;
                                  /* Event buf for active UM Iface OO USE  */
  void*                               um_iface_up_buf_ptr;
                                      /* um up event registration buffer */
  void*                               um_iface_down_buf_ptr;
                                      /* um down event registration buffer */
  void*                               um_flow_enable_buf_ptr; 
                                      /* um flow on event registration buffer */
  void*                               um_flow_disable_buf_ptr;
                                      /* um flow off event registration buffer */
  void* um_phys_link_flow_enable_buf_ptr;   
									  /* um phys link flow on event registration
										 buffer  */
  void* um_phys_link_flow_disable_buf_ptr;  
									  /* um phys link flow off event registration
										 buffer */
  boolean                             arb_close_in_progress; 
									  /* to flag Iface close in progress due to
										 arb */
  ps_iface_type                       sio_iface;
                                      /* Rm Iface*/
  ps_phys_link_type                   sio_phys_link; 
                                      /* physlink data structure */
  dsumts_rmsmi_pdp_specific_info_type pdp_specific_info;
									  /*PDP-ip or PDP-ppp State machine specific 
										info */

  acl_type                            sio_acl;      /* sio interface acl           */

  void*                               vobj_data_ptr;
                                      /*Pointer to the Generic Control Block*/

  byte                                flow_enabled;
}dsumts_rmsmi_sm_info_type;

/*---------------------------------------------------------------------------
  The Generic RM control block. Each Rm Device will have a
  unique instance of this control block.
---------------------------------------------------------------------------*/
typedef struct dsumts_rmsmi_info_s
{
  rex_crit_sect_type        rx_data_crit_sec;

  boolean                   rx_data_can_be_pulled;

  dsm_watermark_type        sio_tx_wmk;     
                            /* watermark for SIO tx operation */
  q_type                    sio_tx_wmk_q;   
                            /* queue for SIO tx watermark */
  dsm_watermark_type        sio_rx_wmk; 
                            /* watermark for SIO rx operation */
  q_type                    sio_rx_wmk_q;  
                            /* queue for SIO rx watermark */
  /*-------------------------------------------------------------------------
    Signal to indicate to PS that there is data on the Rm rx watermark
  -------------------------------------------------------------------------*/
  ps_sig_enum_type ps_ppp_rm_rx_signal; 

  uint32                    profile_id;     
                            /* Context/profile ID */
  sys_modem_as_id_e_type    subs_id; 
                            /* Subscription_id */
  ds_umts_pdp_context_type  profile_context_info; 
                            /* profile: context def */
  boolean                   user_cancel;
                            /* Tracks whether user pressed cancel
                                          before call came up              */
  void*                     ppp_handle;  
                            /* Handle for registering for PPP events */
#ifdef FEATURE_DATA_TE_MT_PDP
  boolean                   is_locked;
								   /* lock used to resolve contention between
									  MO and MT call*/
  boolean                   is_mt; 
								   /* Marks the RMSM instance (either PPP/IP)
									  as MT-PDP */
  uint8                     mt_index; 
                            /* if is_mt is set, this field is valid */
  boolean                   mt_te_ring;   
								   /* flag to indicate whether MT failed at
								      ring due to either ATH or other reasons.
									  It is set on entering RINGING_TE and
                                reset on RM_WANTS_PKT_CALL_EV */
#endif /*FEATURE_DATA_TE_MT_PDP*/
  boolean                   entitlement_check_req; 
								   /* flag to indicate entitlement check
									  is requested */

  boolean                    ent_profile_info_chg; 
                            /* Set if any profile parameters
                               have been changed during 
                               entitlement or redial*/

  dsumts_rmsmi_sm_type       sm_type;       
                            /* state machine type:pdp-ip or ppp */
  ppp_dev_opts_type                 *rm_ppp_config_ptr;
								   /* Dev options to be passed while
									  starting PPP */ 
  dsumts_rmsm_rm_dev_instance_type  instance;
                                   /* state machine instance */

  ds_3gpp_rmsm_ip_common_info_type *ds_3gpp_rmsm_ip_common_info_ptr;
								   /* Pointer to RmSM IP Common*/

  dsumts_rmsmi_sm_info_type *sm_info_ptr [DSUMTS_RMSM_MAX_SM_INSTANCES];
								   /*Pointers to State machine(SM) specific info. 
								     Currently we have only one instance of the
								     SM. But when we support dual-ip, we shall
								     have 2 instances: one for v4 and one for
									 v6*/
  ds3g_siolib_port_e_type             port_id;
                                     /* Port Id on which the DUN call is being 
                                       attempted */
} dsumts_rmsmi_info_type;

void dsumts_rmsmi_set_rx_data_can_be_pulled_flag 
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
);

#define SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) \
  dsumts_rmsmi_set_rx_data_can_be_pulled_flag(rmsmi_info_ptr)

void dsumts_rmsmi_reset_rx_data_can_be_pulled_flag 
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
);
#define RESET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) \
  dsumts_rmsmi_reset_rx_data_can_be_pulled_flag(rmsmi_info_ptr)

void dsumts_rmsmi_reset_rx_data_can_be_pulled_flag 
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
);
#define RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) \
  dsumts_rmsmi_get_rx_data_can_be_pulled_flag(rmsmi_info_ptr)

/*---------------------------------------------------------------------------
  max_bit_rate values for data rates included in extended result code for 
  CONNECT for PDP type IP call.
---------------------------------------------------------------------------*/

#define MAX_BIT_RATE_8KBPS            8000
#define MAX_BIT_RATE_16KBPS           16000
#define MAX_BIT_RATE_32KBPS           32000
#define MAX_BIT_RATE_64KBPS           64000
#define MAX_BIT_RATE_128KBPS          128000
#define MAX_BIT_RATE_256KBPS          256000
#define MAX_BIT_RATE_512KBPS          512000
#define MAX_BIT_RATE_1024KBPS         1024000
#define MAX_BIT_RATE_2048KBPS         2048000
#define MAX_BIT_RATE_7200KBPS         7200000
#define MAX_BIT_RATE_14000KBPS        14000000
#define MAX_BIT_RATE_21000KBPS        21000000
#define MAX_BIT_RATE_42000KBPS        42000000
#define MAX_BIT_RATE_63000KBPS        63000000

#ifdef FEATURE_DATA_LTE
  #define MAX_BIT_RATE_150000KBPS      (QOS_LTE_MAX_DL_KBIT_RATE*1000)
  #define MAX_BIT_RATE_50000KBPS       (QOS_LTE_MAX_UL_KBIT_RATE*1000)
#endif /* FEATURE_DATA_LTE */

/* Default rate based off HSDPA device capability */
#ifdef FEATURE_WCDMA_3C_HSDPA
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_63000KBPS
#elif defined (FEATURE_WCDMA_REL8)
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_42000KBPS
#elif defined(FEATURE_HSPA_PLUS_CAT14)
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_21000KBPS
#elif defined(FEATURE_HSPA_PLUS_CAT10)
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_14000KBPS
#else
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_7200KBPS
#endif /* FEATURE_WCDMA_3C_HSDPA */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS
  These are to be used by DSUMTS_RMSM internally and should not be used by 
  the clients of DSUMTS_RMSM

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_INIT_ALL_INSTANCES()

  DESCRIPTION
    This function initializes all the instances of UMTS RM state machines. 
    There is one state machine per Rm interface that is enabled for Data.
    
    As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   
   IT also calls the pdp-ip and pdp-ppp specific sub state machine initializations
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_init_all_instances( void );

/*====================================================================
  FUNCTION dsumts_rmsmi_sio_cleanup ()

  DESCRIPTION
    This is used to perform SIO port related cleanup before call tear down

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
void dsumts_rmsmi_sio_cleanup ( dsumts_rmsm_rm_dev_instance_type rm_dev );

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_HANDLE_ATD_ON_RM

  DESCRIPTION
    This function handles the dial strings for pkt data calls on a 
    specific RM instance and initiates the data call over the appropriate 
    pkt iface.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type dsumts_rmsmi_handle_atd_on_rm
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance,  /* Rm inst. on which atd */
  const byte *dial_string,                        /* dial string           */
  boolean    digit_mode,                          /* non-digit or digits   */
  const dsat_dial_modifier_info_type * modifiers,  /* Dial sting modifiers  */
  sys_modem_as_id_e_type        subs_id
);

#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_FIND_PROFILE_MATCH()

  DESCRIPTION
    This routine is used to find matching profile to MT call info

  PARAMETERS
    instance
   *mt_info_ptr
    num_of_profiles
   *profile_number

  RETURN VALUE
    dsat_call_abort_cb_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsmi_mt_find_profile_match
(
  dsumts_rmsm_rm_dev_instance_type instance,
  const dsumts_rmsm_mt_info_type *mt_info_ptr,
  uint8 num_of_profiles,
  const uint16 *profile_number
);


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_RING_TE

  DESCRIPTION
    This function handles MT PDP calls on a specific RM instance and 
    initiates the data call over the appropriate pkt iface.
    
  PARAMETERS
   *mt_info_ptr : MT info from Um

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_mt_ring_te
(
  const dsumts_rmsm_mt_info_type *mt_info_ptr
);


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_HANDLE_ANS_ON_RM

  DESCRIPTION
    This function handles MT PDP calls on a specific RM instance and 
    initiates the data call over the appropriate pkt iface.
    
  PARAMETERS
    instance: RMSM instance
    dsat_pdp_answer_data_type

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type dsumts_rmsmi_mt_handle_ans_on_rm
(
  dsumts_rmsm_rm_dev_instance_type instance,
  dsat_pdp_answer_data_type * ans_cb_data
);


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_GET_INDEX_FROM_INSTANCE

  DESCRIPTION
    Given a RMSM instance, get mt_index
    
  PARAMETERS
    instance: RMSM instance
    mt_index: MT index

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_get_mt_index_from_instance
(
  dsumts_rmsm_rm_dev_instance_type instance,
  uint8 *mt_index
);
#endif /*FEATURE_DATA_TE_MT_PDP*/


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_CALL_ABORT_CB()

  DESCRIPTION
    This function is registered as abort handler with ATCOP. If ATCOP
    receives an AT cmd while processing an abortable cmd, this function gets
    called.  It is registered along with the dial string handler. It posts
    appropriate event to DSUMTS_RMSM which initiates the call termination.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_call_abort_cb
(
  void
);



/*===========================================================================
  FUNCTION DSUMTS_RMSMI_PROCESS_DTR_SIG_CB()

  DESCRIPTION
    This function processes the change in dtr state.  If DTR is deasserted
    appropriate event is posted to gen_rmsm which initiates the call 
    termination. DTR assert is ignored for now.

  PARAMETERS
    is_asserted: flag which indicates current DTR state -
                 TRUE: DTR asserted, FALSE: DTR deasserted
    cb_data:     Callback Data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_process_dtr_sig_cb
(
  boolean is_asserted,
  void* cb_data
);


/*===========================================================================
FUNCTION   DSUMTS_RMSMI_UM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on the UM_PKT_IFACE.  Registraion is done for various events based
  upon the call type during call setup. As part of event handling
  appropriate event is posted to DSUMTS_RMSM.

DEPENDENCIES
  dsumts_rmsmi_init() should have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsumts_rmsmi_um_event_cmd_hdlr
(
  dsumts_rmsm_instance_type* inst_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info
);

void dsumts_rmsmi_um_phys_link_event_cmd_hdlr
(
  dsumts_rmsm_instance_type* inst_ptr,
  ps_iface_event_enum_type   event
);

void dsumts_rmsmi_ppprm_event_cmd_hdlr
(
  dsumts_rmsm_instance_type* inst_ptr,
  void *data_ptr
);

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_PHYS_LINK_UM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_phys_link 
  events occur on the UM_PKT_IFACE.  Registraion is done for various events based
  upon the call type during call setup. As part of event handling
  appropriate event is posted to DSUMTS_RMSM.

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsumts_rmsmi_phys_link_um_event_handler_cb
(
  ps_phys_link_type          *this_phys_link_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
  /*lint -esym(715,user_data_ptr) */
);



/*===========================================================================
  FUNCTION DSUMTS_RMSMI_SETUP_SIO_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the laptop pkt data calls handled by DSUMTS_RMSM.

  PARAMETERS
    rm_dev_instance   - RM Device Instance 
    *tx_wm_ptr - ptr to sio transmit (to laptop) watermark
    *rx_wm_ptr - ptr to sio receive (from laptop) watermark

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause ERR_FATALs due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_setup_sio_watermarks
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance,
  dsm_watermark_type *tx_wm_ptr,
  dsm_watermark_type *rx_wm_ptr,
  q_type             *tx_wmk_q_ptr,
  q_type             *rx_wmk_q_ptr 
);



/*===========================================================================
FUNCTION DSUMTS_RMSMI_SIO_TX_PPP_DATA

DESCRIPTION
  This function is called to transmit a packet coming on Um PPP
  interface onto Rm interface.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_sio_tx_ppp_data
(
  dsm_item_type **item_head_ptr,
  void           *meta_info_ptr,
  void           *user_data
);

/*===========================================================================
FUNCTION DSUMTS_RMSMI_SIO_RX_PPP_DATA

DESCRIPTION
  This function is called to receive the next PPP packet, coming on the Rm
  interface, for rx processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* dsumts_rmsmi_sio_rx_ppp_data( void *user_data );


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_PROCESS_ESC_SEQ_CB

  DESCRIPTION
    This function is called when DTR goes low and AT&D = 1.  This should do
    the same action as the dtr_sig_cb.
  PARAMETERS
    Callback Data
  RETURN VALUE
    Always return DSAT_ONLINE_DATA.
  DEPENDENCIES
    None
  SIDE EFFECTS
    None
 ===========================================================================*/
dsat_mode_enum_type dsumts_rmsmi_process_esc_seq_cb
(
  void* cb_data
);


/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_INSTANCE_NUM_FROM_UM_IFACE()

DESCRIPTION
  Returns the instance of the UMTS_RMSM corresponding to the supplied 
  um_iface_ptr.

NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  -1: In case of error
  instance: The RMSM instance corr. to the supplied um_iface_ptr, 
    otherwise

RETURN VALUE
  Pointer to the DSUMTS_RMSM info structure corr. to the supplied um_iface_ptr, 
  otherwise NULL

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsm_sm_instance_type dsumts_rmsmi_get_sm_instance_num_from_um_iface
(
  const ps_iface_type* um_iface_ptr
);

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_INSTANCE_NUM_FROM_SIO_IFACE()

DESCRIPTION
  Returns the instance of the UMTS_RMSM corresponding to the supplied 
  sio_iface_ptr.

NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  sio_iface_ptr: The sio_iface_ptr which the caller wants to map to RMSM
  instance

RETURN VALUE
  -1: In case of error
  instance: The RMSM instance corr. to the supplied sio_iface_ptr, otherwise


DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsm_sm_instance_type  dsumts_rmsmi_get_instance_num_from_sio_iface
(
  const ps_iface_type* sio_iface_ptr
);


/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_RMSMI_INFO_PTR

DESCRIPTION
  Returns pointer to the Rm Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  rm_dev_instance - Rm Device Instance

RETURN VALUE
  Pointer to the RMSMI info structure.

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsmi_info_type*  dsumts_rmsmi_get_rmsmi_info_ptr
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance
);

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_SM_INFO_PTR

DESCRIPTION
  Returns pointer to the SM specific Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS 
  inst_ptr - Instance of the Rm Device and the State Machine Instance

RETURN VALUE
  Pointer to the RMSMI info structure.

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsmi_sm_info_type*  dsumts_rmsmi_get_sm_info_ptr
(
  dsumts_rmsm_instance_type* inst_ptr
);

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_SIOLIB_DEREGISTER_CALLBACKS()

DESCRIPTION
  De reregisters the callbacks registered with SIO lib
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  Port Id for which the callbacks need to be deregistered

RETURN VALUE
  none
  
DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void  dsumts_rmsmi_siolib_deregister_callbacks
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_GET_UM_IFACE_PTR

  DESCRIPTION
    This function retrieves the pointer to UM iface in the system which can
    be used to originate the packet data call. The iface pointer is obtained
    from the routing lookup. If no policy info is provided, the router 
    returns the default UM interface.
  
  PARAMETERS
    None
  
  RETURN VALUE
    Pointer to ps_iface
  
  DEPENDENCIES
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type* dsumts_rmsmi_get_um_iface_ptr
( 
  const dsumts_rmsmi_sm_info_type     *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_BRING_UP_UM_IFACE_BY_POLICY

  DESCRIPTION
    This function retrieves the pointer to UM iface in the system which can
    be used to originate the packet data call. The iface pointer is obtained
    from the routing lookup and arbitration is performed if needed.
  
  PARAMETERS
    None
  
  RETURN VALUE
    0: SUCCESS
   -1: FAILURE
  
  DEPENDENCIES
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
int dsumts_rmsmi_bring_up_um_iface_by_policy
( 
  const dsumts_rmsmi_sm_info_type     *sm_ptr,
  ps_iface_type                   **if_ptr,
  int16                            *ps_errno
);

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION  DSUMTS_RMSMI_REGISTER_WITH_FC

DESCRIPTION
  Used to register clients that need to be reported with FC

PARAMETERS
  Rm Device instance

DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_register_with_fc
(
  dsumts_rmsm_instance_type rm_dev_instance
);

/*===========================================================================
FUNCTION  DSUMTS_RMSMI_DEREGISTER_WITH_FC

DESCRIPTION
  Used to deregister clients that need to be reported with FC

PARAMETERS
  Rm Device instance

DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_deregister_with_fc
(
  dsumts_rmsm_instance_type rm_dev_instance
);

/*===========================================================================
FUNCTION  DSUMTS_RMSMI_REPORT_WM_STATS_CB

DESCRIPTION
  Called by FC to gather watermark statistics

PARAMETERS
  dsumts_rmsm_instance_type rm_dev_instance
  wm_stat_ptr - Pointer to structure where fc stores watermark info
DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_report_wm_stats_cb
(
  uint32 instance,
  fc_wm_stat_type *wm_stat_ptr
);
#endif /* FEATURE_QXDM_WM_TRACING */
/*--------------------------------------------------------------------------
  IOCTL Handler
--------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION DSUMTS_RMSM_IFACE_IOCTL_HDLR

DESCRIPTION
  This function implements dsumts rmsm iface ioctls needed for Autonomous mode
  PPP header addition and autonomous HDLC framing/deframing using A2.

PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on
  ioctl_name        - Type of request
  argval_ptr        - QoS specified by the application
  *ps_errno         - Ptr to error code (if any)

DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure

SIDE EFFECTS

===========================================================================*/
int dsumts_rmsm_iface_ioctl_hdlr
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);

/*===========================================================================
FUNCTION DSUMTS_RMSMI_SET_SIO_PORT_ID

DESCRIPTION
  This function is used to set the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev instance
  Port Id
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void 
dsumts_rmsmi_set_sio_port_id
(
  dsumts_rmsm_rm_dev_instance_type rm_dev,
  ds3g_siolib_port_e_type          port_id
);

/*===========================================================================
FUNCTION DSUMTS_RMSMI_GET_SIO_PORT_ID

DESCRIPTION
  This function is used to get the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   Port Id
    
SIDE EFFECTS
   None

===========================================================================*/
ds3g_siolib_port_e_type dsumts_rmsmi_get_sio_port_id
(
  dsumts_rmsm_rm_dev_instance_type rm_dev
);


/*====================================================================
  FUNCTION dsumts_rmsmi_entitlement_check_complete ()

  DESCRIPTION
    This API is called by client once the entitlement check is done. 
    The result of this check would determine whether to bring up the call 
    or not.

  PARAMETERS
    profile_num: (IN) Profile number to be used for the call
    result:      (IN) TRUE if entitlement check was successful
                      FALSE otherwise
    cb_data:          Pointer to Callback Data
    profile_info_changed (IN) Set to TRUE if profile contents changed 
                           during entitlement check
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
void dsumts_rmsmi_entitlement_check_complete
( 
  boolean                  result,
  dsumts_rmsm_rm_dev_instance_type  rm_dev_inst
);

/*====================================================================
 FUNCTION dsumts_rmsmi_redial_check_complete ()
 
 DESCRIPTION
 This API is called by DS task after a client has indicated the redial 
 check is done. The result of this check would determine whether to 
 bring up a new call or terminate the RMSM.
 
 PARAMETERS
 profile_num: (IN) Profile number to be used for the call
 result:      (IN) TRUE if entitlement check was successful
 FALSE otherwise
 cb_data:          Pointer to Callback Data
 profile_info_changed (IN) Set to TRUE if profile contents changed 
                           during entitlement redial
 RETURN VALUE
 None
 
 DEPENDENCIES
 None
 
 SIDE EFFECTS
 None
 ====================================================================*/
void dsumts_rmsmi_redial_check_complete
( 
  boolean                  result,
  dsumts_rmsm_rm_dev_instance_type  rm_dev_inst
);
/*===========================================================================

FUNCTION dsumts_rmsmi_process_fc_cmd_q

DESCRIPTION
  Handles the FC commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_process_fc_cmd_q ( void );

/*===========================================================================

FUNCTION dsumts_rmsmi_dl_fc_deregister

DESCRIPTION
  RMSM deregisters with Flow Control module for DL traffic.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_dl_fc_deregister
(
  void
);

/*===========================================================================

FUNCTION dsumts_rmsmi_dl_fc_register

DESCRIPTION
  RMSM registers with Flow Control module for DL traffic.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_dl_fc_register
(
  void
);

/*===========================================================================
FUNCTION DSUMTS_RMSMI_GET_PPP_DEV

DESCRIPTION
  This function is used to map Rm Dev to the PPP Device

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   PPP Device
    
SIDE EFFECTS
   None

===========================================================================*/
ppp_dev_enum_type dsumts_rmsmi_get_ppp_dev
(
  dsumts_rmsm_rm_dev_instance_type rm_dev
);

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_GET_RM_DEV_FROM_PPP_DEV

  DESCRIPTION
    This function returns the Rm Device instance corresponding to a
    PPP device.
    
  PARAMETERS
    PPP Device

  RETURN VALUE
    Rm Device Instance    

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
dsumts_rmsm_rm_dev_instance_type dsumts_rmsmi_get_rm_dev_from_ppp_dev
(
  ppp_dev_enum_type ppp_dev
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_REG_UM_EVENTS

  DESCRIPTION
    This function is used to register for Um events.
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_reg_um_events
(
  dsumts_rmsmi_sm_info_type       *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_DEREG_UM_EVENTS

  DESCRIPTION
    This function is used to de-register for Um events.
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_dereg_um_events
(
  dsumts_rmsmi_sm_info_type       *sm_ptr
);

/*===========================================================================
FUNCTION DSUMTS_RMSMI_VALIDATE_RM_DEV_INSTANCE

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether Rm Device instance is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean dsumts_rmsmi_validate_rm_dev_instance
(
  dsumts_rmsm_rm_dev_instance_type rm_dev
);

/*===========================================================================
FUNCTION DSUMTS_RMSMI_VALIDATE_PPP_DEV

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether PPP Device is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean dsumts_rmsmi_validate_ppp_dev
(
  ppp_dev_enum_type ppp_dev
);

/*============================================================================
  FUNCTION DSUMTS_RMSMI_GET_NEXT_RM_DEV_INSTANCE

  DESCRIPTION
    This function fetches the next Rm Dev Instance to be use for DUN calls.
    
  PARAMETERS
    void

  RETURN VALUE
    Next Rm Dev Instance

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
dsumts_rmsm_rm_dev_instance_type dsumts_rmsmi_get_next_rm_dev_instance
(
  void
);


dsumts_rmsmi_sm_info_type* dsumts_rmsmi_get_companion_sm_ptr
(
  dsumts_rmsm_instance_type* inst_ptr
);

boolean dsumts_rmsmi_allow_companion_iface_bringup
(
  ps_iface_type *iface_ptr
);


#endif /*defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE) */
#endif /* DSUMTS_RMSMI_H */
