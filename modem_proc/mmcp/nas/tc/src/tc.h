#ifndef TCC_H
#define TCC_H

/*===========================================================================
                        Test Control (TC) COMMON Header File

DESCRIPTION

   Contains the definitions and enumerations used in the TC.  This includes
   the TC-TC peer messages, Various TC states, Timers and external functions.
   This file is used internally among the TC related source files.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tc.h_v   1.5   25 Mar 2002 15:26:58   madiraju  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tc.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/17/12   hc      Removed #include "rrclcm.h" as part of CMI 
08/31/04   mks     Added support for handling multiple radio bearers during
                   loopback call setup
03/25/02   mks     Defined TC internal message set
02/24/02   mks     Included customer.h 
02/06/02   mks     Updated rb_test_state
09/17/01   ks      Created file.
            
===========================================================================*/


/* <EJECT> */
/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "err.h"
#include "sys_stru_v.h"
#include "rrcdata.h"
#ifdef FEATURE_TDSCDMA
#include "tdsrrcdata.h"
#endif /* FEATURE_TDSCDMA */
#include "dsm.h"
#include "sys.h"
#include "nasutils.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#endif
/*=========================================================================
                 DEFINITIONS AND DECLARATIONS FOR THE MODULE
===========================================================================*/

extern word sm_pd;  // Defines the TC protocol discriminator

#define INVALID_ID    0xff
#define UNKNOWN_CMD   0
#define UNKNOWN_DOMAIN 0xff

#define MS_TC_TC 109

#define TC_SUB  0

/* -----------------------------------------------------------------------------
* Message Types for sm to sm peer messages, defined in Table 10.4a of TS 24.008.
* ----------------------------------------------------------------------------*/
typedef enum
{
  CLOSE_UE_TEST_LOOP               = 0x40,
  CLOSE_UE_TEST_LOOP_COMPLETE      = 0x41,
  OPEN_UE_TEST_LOOP                = 0x42,
  OPEN_UE_TEST_LOOP_COMPLETE       = 0x43,
  ACTIVATE_RB_TEST_MODE            = 0x44,   
  ACTIVATE_RB_TEST_MODE_COMPLETE   = 0x45,
  DEACTIVATE_RB_TEST_MODE          = 0x46,
  DEACTIVATE_RB_TEST_MODE_COMPLETE = 0x47
#ifdef FEATURE_MBMS
  #error code not present
#endif
} tc_layer_msg_T;

/*=========================================================================
                 DEFINITIONS OF STRUCTURES OF MESSAGES
===========================================================================*/

/* ---------------------------------------------------------------------------
* RB test mode state enumeration.
* --------------------------------------------------------------------------*/
typedef enum
{
  RB_UNKNOWN_MODE,
  RB_TEST_MODE_PENDING_CM_CONFIRM,
  RB_TEST_MODE
} rb_test_state_T;

/* ---------------------------------------------------------------------------
* Loopback state enumeration.
* --------------------------------------------------------------------------*/
typedef enum
{
  TC_UNKNOWN_MODE,
  TC_LOOPBACK_MODE1,
  TC_LOOPBACK_MODE2
#ifdef FEATURE_MBMS
  #error code not present
#endif
} loopback_state_T;

/* ---------------------------------------------------------------------------
* Loopback SDU type enumeration.
* --------------------------------------------------------------------------*/
typedef enum
{
  UNKNOWN_SDU_TYPE,
  RLC_SDU_TYPE,
  PDCP_SDU_TYPE 
} loopback_sdu_T;

              
/* ---------------------------------------------------------------------------
* This structure defines the characteristis of a Loopback RAB for mode 1.
* --------------------------------------------------------------------------*/
typedef struct 
{
  byte                   cn_domain_type;  
                                          /* Identifies if CS/PS domain */
  byte                   rab_id;          
                                          /* Actual 8 bit rab_id of the RAB */
  byte                   logical_chan_id; 
                                          /* Logical channel Id of the RAB */
  byte                   sdu_type;        
                                          /* SDU type */
  word                   ul_rlc_sdu_size[UE_MAX_UL_LOGICAL_CHANNEL]; 
                                          /* UL RLC SDU size */                   
  boolean                ul_rlc_sdu_size_valid[UE_MAX_UL_LOGICAL_CHANNEL]; 
                                          /* UL RLC SDU size is valid only if this flag is ON.
                                          This flag will be ON, if SDU size is greater than 0 or missing IE
                                          Will be OFF, if SDU size is 0*/                   
  boolean                rab_established; 
                                          /* Indicates if RAB is established */
  
  dsm_watermark_type     lb_rlc_dl_wm[UE_MAX_UL_LOGICAL_CHANNEL];
                                          /* Pool of UL RLC watermarks */
  dsm_watermark_type     lb_rlc_ul_wm[UE_MAX_DL_LOGICAL_CHANNEL];
                                          /* Pool of DL RLC watermarks */
  q_type                 lb_dl_rlc_q[UE_MAX_DL_LOGICAL_CHANNEL];
                                          /* Pool of Queues for DL SDUs */
  q_type                 lb_ul_rlc_q[UE_MAX_UL_LOGICAL_CHANNEL];
                                          /* Pool of Queues for UL SDUs */
  rrc_user_plane_lc_info_type lc_info_ptr;         
                                          /* LC info */
#ifdef FEATURE_TDSCDMA
  tdsrrc_user_plane_lc_info_type tdslc_info_ptr;         
                                          /* LC info */
#endif /* FEATURE_TDSCDMA */
} rab_context_info;

/* ---------------------------------------------------------------------------
* Data type for list of RAB contexts for Loopback Mode 1.
* --------------------------------------------------------------------------*/
extern rab_context_info  *lb_rab_list[];

#ifndef FEATURE_MODEM_HEAP
extern rab_context_info  lb_rab_list_static[];
#endif

extern boolean post_self_cmd_flag[MAX_RAB_TO_SETUP][UE_MAX_DL_LOGICAL_CHANNEL]; 

extern void tc_process_rab_setup(byte rab_id);

extern void tc_process_rab_release(byte rab_id);

/* -----------------------------------------------------------------------
** FUNCTION       LB_DEREGISTER_WM_RLC 

** DESCRIPTION      This function processes DEREGISTERS the WM with RLC for all the RABs that are established. 
** ----------------------------------------------------------------------- */
extern void lb_deregister_wm_rlc(void);

extern void lb_deregister_rab_wm_rlc(byte rab_id);

#ifdef FEATURE_TDSCDMA
/* -----------------------------------------------------------------------
** FUNCTION       LB_DEREGISTER_WM_TDSRLC 

** DESCRIPTION      This function processes DEREGISTERS the WM with
                    TD-SCDMA RLC for all the RABs that are established. 
** ----------------------------------------------------------------------- */
extern void lb_deregister_wm_tdsrlc(void);
extern void lb_deregister_rab_wm_tdsrlc(byte rab_id);
#endif /* FEATURE_TDSCDMA */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
extern loopback_state_T tc_current_state; 
extern rb_test_state_T rb_test_mode;  /* identifies the current state of the TC */

extern sys_radio_access_tech_e_type tc_rat_rab_established_on;
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#endif

#endif /* TCC_H */
