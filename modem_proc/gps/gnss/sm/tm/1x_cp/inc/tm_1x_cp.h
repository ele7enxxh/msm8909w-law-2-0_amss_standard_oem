#ifndef TM_1X_CP_H
#define TM_1X_CP_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM 1X CP Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for TM 1x CP module

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_cp/inc/tm_1x_cp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/07   ank     Added handling for generic IS801 call flow.
01/29/07   ank     Added handling for CANCEL from PDE.
12/12/06   ank     Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "tm_common.h"
#include "tm_is801.h"
#include "tm_is801b.h"
#include "customer.h"

#define TM_1X_CP_MIN_PDDM_DEFAULT_VAL 58
#define TM_1X_CP_MAX_PDDM_DEFAULT_VAL 200

#define TM_1X_CP_PR_TIMEOUT_T1         2    /* PRQ T1 */
#define TM_1X_CP_PR_TIMEOUT_T2         4    /* PRQ T2 */
#define TM_1X_CP_PR_TIMEOUT_T3         8    /* PRQ T3 */
#define TM_1X_CP_PR_TIMEOUT_T4         16   /* PRQ T4 */
#define TM_1X_CP_PR_TIMEOUT_T5         TM_MGP_PR_TIMEOUT_MAX/1000  /* PRQ T5 and higher */

#define TM_1X_CP_SESSION_TIMEOUT       100*1000  /* 100 secs in ms units */

#define TM_1X_CP_SESS_TIMER_ID               (TM_1X_CP_TIMER_ID_RANGE+1)
#define TM_1X_CP_PPM_TIMER_ID                (TM_1X_CP_TIMER_ID_RANGE+2)
#define TM_1X_CP_IS801_TIMER_ID              (TM_1X_CP_TIMER_ID_RANGE+3)
#define TM_1X_CP_PPM_TRIGGER_TIMER_ID        (TM_1X_CP_TIMER_ID_RANGE+4)
#define TM_1X_CP_TIMER_DISPATCH_MASK         ((TM_1X_CP_SESS_TIMER_ID)|(TM_1X_CP_PPM_TIMER_ID)|(TM_1X_CP_IS801_TIMER_ID)|(TM_1X_CP_PPM_TRIGGER_TIMER_ID)) 

#define IS801_FWD_LINK_REQ_MASK_MT ( IS801_FWD_LINK_REQ_MASK_MSI | IS801_FWD_LINK_REQ_MASK_PPM | IS801_FWD_LINK_REQ_MASK_PRM )

typedef struct {
  boolean ppm_on;    /* Indicate if PPM is going on or not */
  boolean prm_on;     /* Indicate if PR is going on or not */
} tm_1x_cp_mgp_state_s_type;

/* Data structure for static 1x CP information */
typedef struct
{
    word                       dbm_pddm_size;
    boolean                    support_is801b;
    boolean                    support_glonass;
} tm_1x_cp_config_s_type;

typedef enum
{
  TM_1X_CP_SESS_START,
  TM_1X_CP_SESS_CONTINUE
} tm_1x_cp_sess_req_e_type;

typedef enum
{
  TM_1X_CP_TERM_SRC_NONE,  /* 1X CP calls terminate */
  TM_1X_CP_TERM_SRC_CM_IFACE, /* CM IFace calls terminate */
  TM_1X_CP_TERM_SRC_TM_CORE   /* TM-Core calls terminate */
} tm_1x_cp_term_src_e_type;

typedef enum 
{
  TM_1X_CP_SESS_STATE_NONE = 0,
  TM_1X_CP_SESS_STATE_WAIT,
  TM_1X_CP_SESS_STATE_CONNECTED
} tm_1x_cp_sess_state_e_type;

/*
 * The following bit masks represent the generic assistance data that 1x CP 
 * desires to get from the PDE. 
 *
 * Depending on NV configuration, and Base Station Capabilities, some of the
 * requests may not be sent at all.
 */
typedef uint32  tm_1x_cp_rlink_req_type;
#define TM_1X_CP_RLINK_REQ_NONE       0x0000
#define TM_1X_CP_RLINK_REQ_GPS_SA     0x0004
#define TM_1X_CP_RLINK_REQ_AA         0x0008
#define TM_1X_CP_RLINK_REQ_LR         0x0010

/*
 * Generic rlink responses that 1xCP sends to the PDE. Depending on PDE
 * type, these will correspond to IS801-1 or IS801-B rlink resp msgs.
 */
typedef uint32 tm_1x_cp_rl_resp_type;
#define TM_1X_CP_RL_RESP_PRM   IS801_REV_LINK_RESP_MASK_PRM
#define TM_1X_CP_RL_RESP_PPM   IS801_REV_LINK_RESP_MASK_PPM
#define TM_1X_CP_RL_RESP_AMWF  IS801_REV_LINK_RESP_MASK_AMWF
#define TM_1X_CP_RL_RESP_TOM   IS801_REV_LINK_RESP_MASK_TOM
#define TM_1X_CP_RL_RESP_LR    IS801_REV_LINK_RESP_MASK_LR

typedef uint32 tm_1x_cp_fl_req_type;
#define TM_1X_CP_FL_REQ_NONE      	     0x0000
#define TM_1X_CP_FL_REQ_MSI      	     0x0001
#define TM_1X_CP_FL_REQ_ADV_MSI              0x0002 
#define TM_1X_CP_FL_REQ_AMWF                 0x0004	
#define TM_1X_CP_FL_REQ_GPS_L1_PRM           0x0008 
#define TM_1X_CP_FL_REQ_GLO_G1_PRM           0x0010 
#define TM_1X_CP_FL_REQ_PPM                  0x0020	
#define TM_1X_CP_FL_REQ_LR                   0x0040	
#define TM_1X_CP_FL_REQ_TOM                  0x0080	
#define TM_1X_CP_FL_REQ_CAN                  0x0100	
#define TM_1X_CP_FL_REQ_ADV_CAN              0x0200	
#define TM_1X_CP_FL_REQ_UNKNOWN              0x0400	

#define TM_1X_CP_FL_REQ_MT    (TM_1X_CP_FL_REQ_MSI        | \
                               TM_1X_CP_FL_REQ_ADV_MSI    | \
                               TM_1X_CP_FL_REQ_PPM        | \
                               TM_1X_CP_FL_REQ_GLO_G1_PRM | \
                               TM_1X_CP_FL_REQ_GPS_L1_PRM  )

#define TM_1X_CP_FL_REQ_PRM   (TM_1X_CP_FL_REQ_GPS_L1_PRM |  \
                               TM_1X_CP_FL_REQ_GLO_G1_PRM)

typedef uint32 tm_1x_cp_fl_resp_type;
#define TM_1X_CP_FL_RESP_REJ                 0x0001
#define TM_1X_CP_FL_RESP_ADV_REJ             0x0002
#define TM_1X_CP_FL_RESP_BSC                 0x0004
#define TM_1X_CP_FL_RESP_ADV_BSC             0x0008
#define TM_1X_CP_FL_RESP_AA                  0x0010
#define TM_1X_CP_FL_RESP_LASC                0x0020
#define TM_1X_CP_FL_RESP_LACC                0x0040
#define TM_1X_CP_FL_RESP_SA                  0x0080
#define TM_1X_CP_FL_RESP_BSA                 0x0100
#define TM_1X_CP_FL_RESP_ALM                 0x0200
#define TM_1X_CP_FL_RESP_EPH                 0x0400
#define TM_1X_CP_FL_RESP_NMB                 0x0800
#define TM_1X_CP_FL_RESP_LR                  0x1000
#define TM_1X_CP_FL_RESP_AC                  0x2000
#define TM_1X_CP_FL_RESP_SHI                 0x4000
#define TM_1X_CP_FL_RESP_GGTO                0x8000

/* Data structure for dynamic 1x CP information */
typedef struct
{
    boolean                          sess_active;
    tm_1x_cp_sess_state_e_type       sess_state;      /* 1X CP session state */
    tm_1x_cp_mgp_state_s_type        mgp_state;
    boolean                          tm_core_session_started;
    byte                             gps_session_timeout;  /* Desired time for GPS engine to work on a fix. (1-255)secs */
    byte                             prm_prq;  /* PRQ received from PDE */
    dword                            ppm_timeout;
    boolean                          release_pd_call;
    /* TBD Change the types of 2 fields below to appropriate types */
    uint32                           is801_req_wait_bmap;
    uint32                           is801_resp_to_send_bmap;
    tm_1x_cp_fl_req_type             cancel_req_bmap;
    boolean                          bs_capabilities_valid;
    /* number of Is801-1 pddms decoded from a rcvd MT msg */
    uint8                            num_is801_1_pddm;
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
    #error code not present
#endif
    /* Is801-1 requests sent by the PDE */
    is801_fwd_link_req_mask_type     is_801_1_req;
    /* Is801-1 requests sent by the PDE that are to be rejected */
    is801_fwd_link_req_mask_type     is801_1_rej_req;
    os_TimerMsgType*                 tm_1x_cp_sess_timer;      /* Session recovery timer */
    os_TimerMsgType*                 tm_1x_cp_ppm_timer;       /* PPM tiemr for 500ms */
    os_TimerMsgType*                 tm_1x_cp_is801_timer;     /* IS-801 response timeout */
    os_TimerMsgType*                 tm_1x_cp_ppm_trigger_timer;
    byte                             pde_version;
    tm_1x_cp_fl_req_type             fl_req_mask;
    tm_1x_cp_fl_resp_type            fl_resp_mask;
    uint8                            num_is801b_pddm;          /* number of rcvd is801b pddms */
    void                            *is801b_pddm_ptr;
    /* copy of the latest pddm to be sent and one thats rcvd */
    is801_rev_link_pddm_info_s_type  rev_link_pddm; 
    is801_fwd_link_pddm_info_s_type  fwd_link_pddm;
    boolean                          doing_postfix_ppm;        /* TRUE if doing Post-Fix PPM */
} tm_1x_cp_sess_info_s_type;

extern tm_1x_cp_sess_info_s_type     tm_1x_cp_session_info;
extern tm_1x_cp_config_s_type        tm_1x_cp_config;
extern is801_pddm_sess_info_s_type   last_pddm_hdr; 

/*===========================================================================

FUNCTION tm_1x_cp_prtl_init

DESCRIPTION
  This function is called by TM-Core to initialize 1X CP module
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_1x_cp_prtl_init(void);

#ifdef __cplusplus
}
#endif

#endif /* TM_1X_CP_H */


