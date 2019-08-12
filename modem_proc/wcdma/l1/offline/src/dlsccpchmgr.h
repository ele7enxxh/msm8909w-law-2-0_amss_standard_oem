#ifndef DLSCCPCHMGR_H
#define DLSCCPCHMGR_H

/*===========================================================================
                 D L S C C P C H M G R . H

GENERAL DESCRIPTION
  This file contains the declarations and definitions of dlsccpchmgr needed
  across modules.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlsccpchmgr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
05/05/14    rs      FR-3672 CTCH log packet changes. 
12/27/13    dm      FR 3672: CTCH enhancement support with queued schedule.
08/20/13    rs      Returns TTI duration for CTCH channel 
07/15/13    gsk     FR 3672: CTCH enhancement changes
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/04/12    dm      Back up CTCH schedule when suspending W and restore it when 
                    resuming.
06/10/11    scm     Keep CTCH up 100% for non-DRX with short allocation period.
09/21/10    scm     Support CTCH DRX lengths greater than 4096 frames.
03/06/09    vp      Provide APIs for dlPichMgr to set/reset assoc_pich_flag.
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/15/08    vp      Added code comments and Removed static db
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
08/20/07    vp      Added support for new sccpch0/pich reponse functions.
04/26/07    scm     Fix compiler warning.
02/21/07    vp      Initial version
                    
                    ===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dlchmgr.h"
#include "dlshmgr.h"
#include "l1const.h"
#include "l1task.h"
#include "wl1drxmanager.h"

/*The maximum number of sccpch that has to be managed by sccpch manager at a 
time is 2.*/
#define DL_SCCPCH_CFG_DB_SIZE 2

/* This is the value we use to return an invalid sfn */
#define DL_BMC_INVALID_SFN (-1)

/*Structure that holds all the information pertaining to sccpch*/
typedef struct {
  /*Whether the entry is in use*/
boolean in_use;

  /*Index for the SCCPCH channel in to the DlChMgr cfg db*/
dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx;

  /*Whether an associated PICH channel setup was requested by RRC*/
boolean assoc_pich_involved;

  /*Services that are enabled on this SCCPCH*/
dlshmgr_svc_mask_type cctrch_to_svc_map_mask;
} dl_sccpch_cfg_db_struct_type;

  /*Enum type that holds all the logical (drx) entity types that are managed 
  by SCCPCH manager*/
typedef enum {
DL_SCCPCH_R99_LOGICAL_ENTITY_PCH,
DL_SCCPCH_R99_LOGICAL_ENTITY_NON_CTCH_FACH,
#ifdef FEATURE_UMTS_BMC
DL_SCCPCH_R99_LOGICAL_ENTITY_CTCH_FACH,
#endif
DL_SCCPCH_R99_LOGICAL_ENTITY_MAX
} dl_sccpch_r99_logical_entity_enum_type;

  /*Enum type that holds the state of logical entities*/
typedef enum {
DL_SCCPCH_LOGICAL_ENT_STATE_WAIT_FOR_INACTIVE,
DL_SCCPCH_LOGICAL_ENT_STATE_INACTIVE,
DL_SCCPCH_LOGICAL_ENT_STATE_WAIT_FOR_ACTIVE,
DL_SCCPCH_LOGICAL_ENT_STATE_ACTIVE,
DL_SCCPCH_LOGICAL_ENT_STATE_MAX
} dl_sccpch_r99_logical_state_enum_type;

  /**/
typedef struct {
  /*entity_type can be used in future if we move from static to dynamic
  index allocation in drx_entities[]*/
  boolean logical_entity_on;

  drx_entity_type          entity_type;

  dl_sccpch_r99_logical_state_enum_type curr_state;

  channel_cb_struct_type *drx_cb_functions;

  dlshmgr_svr_type_enum_type trch_service_type;

  dl_ch_spec_cfg_db_idx_type ch_spec_idx;

  drx_entity_handle_type drx_entity_handle;

  dlshmgr_svc_to_trch_map_mask_type cfg_trch_mask;
} dl_sccpcch_r99_logical_entity_info_struct_type;

#ifdef FEATURE_UMTS_BMC
#ifdef FEATURE_UMTS_BMC_SLEEP_OPT
/* After checking PICH or receiving a CTCH block set, we must decide what to
** do next.  This is based on when the next PICH is and when the next CTCH
** block set is.  The 4 possibilities are enumerated below: */
typedef enum
{
  DL_BMC_SLEEP_WAIT_FOR_PICH = 0,
  DL_BMC_SLEEP_WAIT_FOR_CTCH,
  DL_BMC_DONT_SLEEP_WAIT_FOR_PICH,
  DL_BMC_DONT_SLEEP_WAIT_FOR_CTCH
} dl_bmc_sleep_enum_type;

/* Structure describing what to do after PICH or CTCH block set reception.
** If we're going back to sleep, "sleep_msec" says how long to sleep until
** the next PICH or CTCH block set, whichever comes first.  The last 2 items
** are for CTCH block set reception, when it starts and when it ends. */
typedef struct
{
  dl_bmc_sleep_enum_type  sleep_type;
  uint16                  sleep_msec;
  uint16                  sfn_of_ctch_block_set;
  uint16                  sfn_after_ctch_block_set;
} dl_bmc_sleep_info_struct_type;

#endif  /* #ifdef FEATURE_UMTS_BMC_SLEEP_OPT */

/* Run-time debug flag for extra F3 messages on CTCH block set reception. */
extern boolean  dl_bmc_extra_ctch_debug;

#endif  /* #ifdef FEATURE_UMTS_BMC */

/*callback functions to be registered with DRX for PCH*/
extern channel_cb_struct_type  dl_sccpch_pch_drx_cb_functions;

/*callback functions to be registered with DRX for non-CTCH FACH*/
extern channel_cb_struct_type  dl_sccpch_non_ctch_r99_fach_drx_cb_functions;

/*callback functions to be registered with DRX for CTCH FACH*/
extern channel_cb_struct_type  dl_sccpch_ctch_r99_fach_drx_cb_functions;

/* This structure holds the reponse functions for channel type: SCPPCH_0*/
extern dlchmgr_resp_func_cb_struct_type dl_sccpch0_resp_func_cb;

/* This structure holds the reponse functions for channel type: SCPPCH_1*/
extern dlchmgr_resp_func_cb_struct_type dl_sccpch1_resp_func_cb;

/* This structure holds the index management callback functions to sccpch manager*/
extern dlchmgr_idx_mgmt_func_struct_type dl_sccpch_idx_mgmt_func;

/* This structure holds the pre and post operation callbacks
   for sccpch*/
extern dlchmgr_op_cb_func_struct_type dl_sccpch_op_cb_func;

  /*The global that holds cfg information for all sccpch channels currently active*/
extern dl_sccpch_cfg_db_struct_type dl_sccpch_cfg_db[DL_SCCPCH_CFG_DB_SIZE];

/*===========================================================================
FUNCTION        dl_sccpch_alloc_dyn_db_idx

DESCRIPTION     This function finds a free entry in the sccpch manager dynamic 
                db and allocates it to the caller.

DEPENDENCIES    None

RETURN VALUE
                dyn-db-idx: The allotted index in sccpch manager dynamic db.

SIDE EFFECTS    None.
===========================================================================*/
extern dl_ch_spec_cfg_db_idx_type dl_sccpch_alloc_cfg_db_idx(
  dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_dealloc_dyn_db_idx

DESCRIPTION     This function frees entry in the sccpch manager dynamic db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_sccpch_dealloc_cfg_db_idx(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_init

DESCRIPTION     This function initializes the sccpch manager. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_sccpch_init( void );

/*===========================================================================
FUNCTION        dl_sccpch_pre_add_cb

DESCRIPTION     This function does any processing needed before the addition 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All pre-add operations were successful.
                FALSE: At least one pre-add operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_pre_add_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx,
  l1_dl_phychan_db_struct_type *phychan_db);

/*===========================================================================
FUNCTION        dl_sccpch_post_add_cb

DESCRIPTION     This function does any processing needed after the addition 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-add operations were successful.
                FALSE: At least one post-add operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_post_add_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_pre_drop_cb

DESCRIPTION     This function does any processing needed before the drop 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All pre-drop operations were successful.
                FALSE: At least one pre-drop operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_pre_drop_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_post_drop_cb

DESCRIPTION     This function does any processing needed after the drop 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-drop operations were successful.
                FALSE: At least one post-drop operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_post_drop_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_get_cctrch_for_pich

DESCRIPTION     This function finds the CCTrCh index of the SCCPCH that 
                carries an associated PICH.

DEPENDENCIES    None

RETURN VALUE
                CCTrCh Idx: CCTrCh index of the SCCPCH

SIDE EFFECTS    None.
===========================================================================*/
extern mcalwcdma_dec_cctrch_id dl_sccpch_get_cctrch_for_pich( void );

/*===========================================================================
FUNCTION        DL_PHYCHAN_ADD_CMD

DESCRIPTION     This function starts the procedure for adding a physical
                channel.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The downlink module will start the process of adding the
                specified physical channel.
===========================================================================*/
extern void dl_sccpch_drx_cmd_rcvd(void);

/*===========================================================================
FUNCTION        dl_sccpch_pre_add_cb

DESCRIPTION     This function does any processing needed before the resumption 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All pre-resumption operations were successful.
                FALSE: At least one pre-resumption operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_pre_resume_cb(drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        dl_sccpch_post_resume_cb

DESCRIPTION     This function does any processing needed after the resumption 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-resumption operations were successful.
                FALSE: At least one post-resumption operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_post_resume_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_pre_suspend_cb

DESCRIPTION     This function does any processing needed before the suspension 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All pre-suspension operations were successful.
                FALSE: At least one pre-suspension operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_pre_suspend_cb(drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        dl_sccpch_post_suspend_cb

DESCRIPTION     This function does any processing needed after the suspension 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-suspension operations were successful.
                FALSE: At least one post-suspension operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_post_suspend_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_is_ok_for_setup

DESCRIPTION     This function examines the possibilty of SCCPCH Setup.

DEPENDENCIES    None

RETURN VALUE
                TRUE: SCCPCH can be setup
                FALSE: SCCPCH cannot be setup

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_sccpch_is_ok_for_setup(l1_dl_phychan_enum_type phch_type,
  l1_dl_phychan_enum_type drop_dl_phychan);

/*===========================================================================
FUNCTION        dl_sccpch_get_pcch_drx_hdle

DESCRIPTION     This function returns the drx entity handle of PCCH.

DEPENDENCIES    None

RETURN VALUE
                drx_entity_handle: DRX Entity Handle of the PCCH Logical Entity

SIDE EFFECTS    None
===========================================================================*/
extern drx_entity_handle_type dl_sccpch_get_pcch_drx_hdle(void);

/*===========================================================================
FUNCTION        dl_sccpch_en_svc

DESCRIPTION     This function does any processing needed for enabling a
                particular service

DEPENDENCIES    None

RETURN VALUE
                TRUE: Enabling Service is successful.
                FALSE: Enabling Service fails.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_en_svc(
dl_ch_spec_cfg_db_idx_type cfg_db_idx,

/*cctrch index*/
uint8 cctrch_idx,

/*Service to be handled*/
dlshmgr_svr_type_enum_type shmgr_svc_type
);

/*===========================================================================
FUNCTION        dl_sccpch_dis_svc

DESCRIPTION     This function does any processing needed for disabling a
                particular service

DEPENDENCIES    None

RETURN VALUE
                TRUE: Disabling Service is successful.
                FALSE: Disabling Service fails.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_sccpch_dis_svc(
dl_ch_spec_cfg_db_idx_type cfg_db_idx,

/*cctrch index*/
uint8 cctrch_idx,

/*Service to be handled(currently, only FACH ??)*/
dlshmgr_svr_type_enum_type shmgr_svc_type
);

/*===========================================================================
FUNCTION        dl_sccpch_get_svr_type

DESCRIPTION     This function finds the R99 Service type associated 
                with ch spec idx

DEPENDENCIES    None

RETURN VALUE
                dlshmgr_svr_type_enum_type: Service type associated with ch spec idx.

SIDE EFFECTS    None.
===========================================================================*/
extern dlshmgr_svr_type_enum_type dl_sccpch_get_svr_type(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_sccpch_reset_assoc_pich_flag

DESCRIPTION     This function finds the PCCH channel specific cfg_idx from logical entity db and 
                resets the assoc_pich_involved flag.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_sccpch_reset_assoc_pich_flag(void);

/*===========================================================================
FUNCTION        dl_sccpch_set_assoc_pich_flag

DESCRIPTION     This function finds the PCCH channel specific cfg_idx from logical entity db and 
                sets the assoc_pich_involved flag.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_sccpch_set_assoc_pich_flag(void);

#ifdef FEATURE_UMTS_BMC
/*===========================================================================
FUNCTION        DL_BMC_CTCH_SCCPCH_OFFSET

DESCRIPTION
  This function returns the offset of the CTCH carrying SCCPCH in Chipx8.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Offset of the CTCH carrying SCCPCH in Chipx8.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 dl_bmc_ctch_sccpch_offset(void);

/*===========================================================================
FUNCTION        DL_BMC_IS_SCCPCH_FOR_CTCH_ACTIVE

DESCRIPTION
  This function returns whether or not we have the SCCPCH physical channel
  up receiving CTCH data.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if SCCPCH carrying CTCH is active.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bmc_is_sccpch_for_ctch_active(void);

/*===========================================================================
FUNCTION        DL_BMC_IS_CTCH_SCHEDULING_ACTIVE

DESCRIPTION
  This function is called by srchzz code when neighbor searching has finished
  during DRX operation, and search is planning to disable all active fingers.
  If this function returns TRUE, that means to keep fingers active so we can
  demodulate SCCPCH, carrying CTCH data on the FACH.  It is also called by
  DRX code, so we don't sleep the RF during a DRX cycle.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if WL1 has been configured to receive broadcast data on
            the CTCH (FACH on SCCPCH).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bmc_is_ctch_scheduling_active(void);

/*===========================================================================
FUNCTION        dl_bmc_is_current_ctch_mode_drx_scheduling

DESCRIPTION
  this API is called by drx.c to check if the current bmc_ctch_state is drx_scheduling

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if the current bmc_ctch_state is drx_Scheduling. 

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bmc_is_current_ctch_mode_drx_scheduling(void);

/*===========================================================================
FUNCTION        DL_BMC_TEARDOWN_SCCPCH

DESCRIPTION
  This function is called by DRX code when we wake up to check for a page.
  If SCCPCH is up for CTCH reception, tear it down for PICH demodulation.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dl_bmc_teardown_sccpch(void);

/*===========================================================================
FUNCTION        L1M_CPHY_BMC_SCHEDULING_REQ

DESCRIPTION
  We got the CPHY_BMC_SCHEDULING_REQ primitive from BMC (Broadcast Multicast
  Control).  This means WL1 is supposed to begin receiving broadcast data on
  the CTCH (mapped to FACH on SCCPCH).

  It may specify non-DRX mode, meaning WL1 just uses the CBS frame offset and
  CTCH allocation period it got earlier from RRC in the CPHY_SETUP_REQ for
  this SCCPCH.  Or it may specify DRX operation, meaning BMC passes the exact
  list of SFN's that have CTCH data on the SCCPCH.

PARAMETERS
  l1_ext_cmd_type *cmd_ptr - Pointer to BMC SCHEDULING_REQ command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void l1m_cphy_bmc_scheduling_req(l1_ext_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION        L1M_CPHY_BMC_SKIP_REQ

DESCRIPTION
  We got the CPHY_BMC_SKIP_REQ primitive from BMC (Broadcast Multicast
  Control).  This comes with a list of SFN's that WL1 should not monitor for
  CTCH data on SCCPCH.  This overrides anthing that came earlier in the
  L1M_CPHY_BMC_SCHEDULING_REQ command.

PARAMETERS
  l1_ext_cmd_type *cmd_ptr - Pointer to BMC CPHY_BMC_SKIP_REQ command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void l1m_cphy_bmc_skip_req(l1_ext_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION        L1M_CPHY_BMC_DEACTIVATE_REQ

DESCRIPTION
  We got the CPHY_BMC_DEACTIVATE_REQ primitive from BMC (Broadcast Multicast
  Control).  WL1 shall no longer monitor SCCPCH for CTCH data.  It shall
  maintain SCCPCH configuration information should BMC send the
  L1M_CPHY_BMC_SCHEDULING_REQ command at some point in the future.

PARAMETERS
  l1_ext_cmd_type *cmd_ptr - Pointer to BMC CPHY_BMC_DEACTIVATE_REQ command,
                             which is not used by this function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED (lint) */
extern void l1m_cphy_bmc_deactivate_req(l1_ext_cmd_type *cmd_ptr);
/*===========================================================================
FUNCTION      dl_bmc_get_extended_sfn_from_sfn

DESCRIPTION
  This function is used to derive the extended SFN, given the SFN (range 0-4095).
  If the current SFN is x, extended SFN y and the sfn passed as input is (x+a), this funtion
  returns (y+a) (normalized). 
  Limitation of this function is that 'a' cannot be greater than or equal to 4096.

PARAMETERS
  uint16 sfn - SFN (0-4095).

DEPENDENCIES
  

RETURN VALUE
  int32 - Returns the extended SFN corresponding to input SFN.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern int32 dl_bmc_get_extended_sfn_from_sfn(uint16 sfn);
/*===========================================================================
FUNCTION      dl_bmc_get_scheduled_sfn_index_between_pich

DESCRIPTION
  This function is called to get the index of CTCH SFNs between 2 PICH occasions. The SFN passed as 
  argument can be any extended SFN between 2 successive PICH occasions. This function will be called only in 
  DL_BMC_CTCH_DRX_SCHEDULING and DL_BMC_CTCH_DRX_PENDING modes.

PARAMETERS
  int32 sfn - extended SFN between 2 successive PICH occasions.

DEPENDENCIES
  Only called in DL_BMC_CTCH_DRX_SCHEDULING and DL_BMC_CTCH_DRX_PENDING modes. Will return Invalid
  index if called in any other mode.

RETURN VALUE
  uint16 - Returns the index of the next extended SFN that BMC wants following the input SFN.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 dl_bmc_get_scheduled_sfn_index_between_pich(int32 sfn);
/*===========================================================================
FUNCTION      dl_bmc_get_scheduled_sfn_between_pich

DESCRIPTION
  This function is called by DRX to get the CTCH SFNs between 2 PICH occasions. The SFN passed as 
  argument can be any SFN between 2 successive PICH occasions. This function will be called only in 
  DL_BMC_CTCH_DRX_SCHEDULING and DL_BMC_CTCH_DRX_PENDING modes.

PARAMETERS
  int32 sfn - Extended SFN between 2 successive PICH occasions.

DEPENDENCIES
  Only called in DL_BMC_CTCH_DRX_SCHEDULING and DL_BMC_CTCH_DRX_PENDING modes. 
  Will return Invalid index if called in any other mode.

RETURN VALUE
  uint32 - Returns the next extended SFN that BMC wants following the input SFN.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 dl_bmc_get_scheduled_sfn_between_pich(uint32 input_extended_sfn);
/*===========================================================================
FUNCTION      dl_bmc_is_sfn_priority_sfn

DESCRIPTION
  This function returns 1 if the input SFN is a priority SFN, else zero.

PARAMETERS

DEPENDENCIES

RETURN VALUE
  1 if the input SFN is a priority SFN, else zero.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bmc_is_sfn_priority_sfn(uint32 extended_sfn);
/*===========================================================================
FUNCTION        dl_sccpch_cphy_setup_has_ctch_drop

DESCRIPTION     This function finds out whether SCCPCH1 is being dropped

DEPENDENCIES    None

RETURN VALUE
  boolean - TRUE if SCCPCH1 is being dropped

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_sccpch_cphy_setup_has_ctch_drop(l1_setup_cmd_type* setup);

/*===========================================================================
FUNCTION        dl_sccpch_cphy_setup_has_ctch_add

DESCRIPTION     This function finds out whether CTCH needs to be added as part
                of either SCCPCH0/SCCPCH1

DEPENDENCIES    None

RETURN VALUE
boolean - TRUE if the channel being added is either SCCPCH0/1 with CTCH

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_sccpch_cphy_setup_has_ctch_add(l1_setup_cmd_type* setup);

/*===========================================================================
FUNCTION     DL_BMC_FRAME_KEEP_CTCH_AWAKE

DESCRIPTION
  This function helps us handle a special CTCH case having these conditions:

  - CTCH is in non-DRX mode, meaning no schedule, just periodic CTCH.
  - That period is relatively small, meaning CTCH is awake a lot.
  - Network has scheduled CTCH, but is not transmitting anything.

  When those things happen, we bring up CTCH hoping to receive something, but
  we end up timing out, receiving nothing.  We might time out right on top of
  the next CTCH occasion.  Checking SFN we might think we are past it and missed
  it, but really because of latencies, etc., we might be able to receive that
  CTCH occasion.  Normally we would drop CTCH because we think we missed it.
  But we don't really want to do that.  If network starts transmitting right
  then, we could miss some CTCH data.

  It's difficult to get this latency requirement correct, varies from target
  to target, etc.  Instead if the allocation period is small, keep CTCH up
  100% of the time.  A small allocation period essentially means that anyway.
  Don't continuously add/drop/add/etc CTCH because we risk missing data.  If
  allocation period is larger, we can allow the drop, because it won't
  accidentally happen during the next CTCH occasion.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if DRX Manager should keep CTCH awake.  FALSE otherwise.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bmc_keep_ctch_awake(void);

/*===========================================================================
FUNCTION     DL_BMC_GET_CTCH_TTI

DESCRIPTION
This API returns the CTCH TTI in frames.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  CTCH TTI in frames.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 dl_bmc_get_ctch_tti(void);
/*===========================================================================
FUNCTION     DL_BMC_FRAME_IS_WANTED

DESCRIPTION
  This function checks to see if a frame with the given extended SFN should
  be passed up to BMC.

PARAMETERS
  int32 sfn - Current extended SFN.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if the current frame should be passed to BMC.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bmc_frame_is_wanted(int32 sfn);

/*===========================================================================
FUNCTION        DL_BMC_IS_CTCH_SCHEDULE_SUSPENDED

DESCRIPTION
  This function checks if WCDMA is suspended and as a result, CTCH schedule is 
  backed up.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bmc_is_ctch_schedule_suspended(void);

/*===========================================================================
FUNCTION        dl_bmc_backup_ctch_sched

DESCRIPTION
Suspend WCDMA req is triggered. CTCH is dropped but the schedule is to backed
up and used when resuming WCDMA.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dl_bmc_backup_ctch_sched(void);

/*===========================================================================
FUNCTION        dl_bmc_restore_ctch_sched

DESCRIPTION
BMC schedule was backed up as part of suspend WCDMA. Now restore it as part of 
resume WCDMA.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dl_bmc_restore_ctch_sched(void);

/*===========================================================================
FUNCTION        DL_BMC_SKIP_CTCH_SFNS

DESCRIPTION
  This is called by other L1 modules (for example, srchzz) to skip CTCH SFNs.
  Input is the list of SFNs to skip. This function works on SFNs from both 
  active schedule and queued schedule.

PARAMETERS
  l1_ext_cmd_type *cmd_ptr - Pointer to BMC CPHY_BMC_SKIP_REQ command. Function
  extracts the SFN list from the command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dl_bmc_skip_ctch_sfns(l1_ext_cmd_type *cmd_ptr);
#endif  /* #ifdef FEATURE_UMTS_BMC */

/*===========================================================================
FUNCTION     DL_BMC_GET_CTCH_INFO_FOR_LOG_PKT

DESCRIPTION
  This API populates info required for CTCH log packtet

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dl_bmc_get_ctch_info_for_log_pkt(WCDMA_DRX_CTCH_ENH_LOG_PKT_type *dl_bmc_ctch_enh_log_pkt_ptr);

#endif

