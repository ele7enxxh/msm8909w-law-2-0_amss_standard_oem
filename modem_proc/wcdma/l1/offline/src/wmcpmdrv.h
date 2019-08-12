#ifndef WMCPMDRV_H
#define WMCPMDRV_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         WCDMA MCPM Driver interface

GENERAL DESCRIPTION

  This file contains the source code for the WCDMA Modem Power Abstraction
  layer implementation.


Copyright (c) 2000-2015 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wmcpmdrv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when           who      what, where, why
--------       ----     ----------------------------------------------------------
05/09/16       skk      F3 Reduction. 
09/11/15       stk      FR 30139: SVS+ and NOM+ frequency corner support in MCVS
02/04/15       sks      Add changes to bump up clk to 576 Mhz if EUL 2ms + CM is active. Change CM client to MCVS. 
02/02/15       sl       Ensure W Stack is up when RRC calls DL API "dl_get_cell_timing" 
06/20/14       skk      Added W2L client status api.
09/16/14       stk      JO SW-FW interface change to support Q6 clock speeds 
08/19/14       sks      Do not bump down VPE while searches inside CM are in progress.
08/08/14       sks      MCPM driver code cleanup and JO MCVS changes.
02/18/13       sr       Initial checkin for CPC Lite sleep
06/25/14       sks      Preliminary changes for MCPM driver for JO.
06/18/14       sks      Change num aset fingers to tot num of fingers for HS case.
05/06/14       sks/aj   MCVS changes for Searcher
05/12/14       sks      Correct an error in logic check while sending the MCPM config modem. 
05/11/14       pr       MCPM Opts for unification of HSDPA/HSUA Clk requests.
03/12/14       sks      Bump down VPE for single cell HS-TD on BOLT only if EQ active or EQ and QICE both inactive.  Also bump up Q6 to 518.4 MHz if VPE  goes to NOM/TURBO for DC.
03/12/14       sks      MCVS check-in on BOLT.
10/24/13       kr       Handle Triage and CME VPE request separately so as to update respective Params
11/22/13       kr       Need to bump up the clock to 384MHz with NBR cell enabled with QICE
11/04/13       stk      MCPM related changes for 3C
09/17/13       jkb      Porting Dual Sim from Triton to Dime
09/12/13       jd       Searcher redesign 
08/19/13       kr       Bump Q6 clk if DC is enable dand VPE is at NOM/Turbo
08/23/13       kr       MCVS Support for Pull in event
08/01/13       sks      Change finger counter for HS NOM. Change SVS PLUS to reduce fingers and counter total # fingers.
07/18/13       kr       MCVS: added support for SVS+ 
06/19/13       kr       Updated the Turbo switching condition and Number of fingers for NOM
06/10/13       kr       Added support to Bump up/down VPE witch PCCPCH add drop
04/15/13       kr       Protecting MCPM requests/release with Mutex lock.
04/12/13       stk      Changes to have NV support to bumpup Q6, VPE and Clkbus, and DIME MCVS changes.
04/04/13       stk      Preventing ISR/Task collision with offline clk change callback by having separate globals for request and relinquish
                        to indicate who called mcpm config modem.
04/02/13       stk      Reverting the change made to request_parms; making it global again since receiver cfg depends on previous params.
                        Example: EQ ON depends on previous QICE ON. 
03/28/13       stk      Preventing ISR/Task collision with WMCPMDRV drivers by making wmcpmdrv_request_parms a local variable.
03/22/13       kr       changed switching point logic for HS accrding to new uK.
03/14/13       stk      Changed the QICE query API for MCVS VPE update
03/09/13       kr       MCVS feature enhancement for DC
03/08/13       kr       MCVS feature enhancement
02/11/13       gsk/sr   EDRX Full Sleep initial check in.
01/18/13       kr       changing switching point criteria for HS and R99 calls
01/17/13       kr       Declaring a new function
01/16/13       kr       chnage srch period in frames instead of msec
01/10/13       kr       Featurizing FR2018,MCVS feature for Dime only.
12/11/12       kr       Added Support for MCVS 
07/24/12       geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/24/12       pr       Dime Bringup Changes
03/27/12       stk      New mechanism to track the offline clk speed and update FW.
02/21/12       pv       Support for MAX_OFFLINE_CLK_REQ client.
02/21/12       pv       Created API for registering with MCPM for any clock change.
11/11/11       pv       Support for Sleep timeline adjustment callback with MCPM.
10/25/11       pv       Added CM client.
08/09/11       stk      Added support for MCPM PARAMS UPDATE REQ.
08/02/11       pv       Added API to register MCPM callback for a HW block.
07/28/11       pv       APIs/macros to return W2G client status.
10/18/10       stk      First version of the WMCPM Driver interface for NIKEL.


===========================================================================*/

#include "wcdma_variation.h"
#ifdef FEATURE_WCDMA_MCPM_DRIVER
#include "mcpm_api.h"
#include "rex.h"
#include "wmcvsdrv.h"

#define WL1_MCPM_MSG(str, a, b, c) \
if (IS_HSPA_DATA_ACTIVE() || IS_R99_DATA_ACTIVE()) \
{ \
 WL1_MSG3(ERROR, str, a, b, c); \
}\
else \
{ \
  WL1_MSG3(LOW, str, a, b, c); \
}

/* MACROs for voice and data call bit setting in call setup bmsk*/
#define WMCPMDRV_R99_VOICE_CALL_ACTIVE   0x01
#define WMCPMDRV_R99_DATA_CALL_ACTIVE    0x02
#define WMCPMDRV_HS_DATA_CALL_ACTIVE     0x04

#define WMCPMDRV_DEFAULT_OFFLINE_CLK_MCPM_SPEED MCPM_CLK_72M

/* Enumeration detailing the different WCDMA PWR driver clients */
typedef enum
{
  WCDMA_MPWRDRV_CLIENT_STACK_ACTIVE,          /* 0 */
  WCDMA_MPWRDRV_CLIENT_DL_ACTIVE,             /* 1 */
  WCDMA_MPWRDRV_CLIENT_DRX,                   /* 2 */   
  WCDMA_MPWRDRV_CLIENT_W2G,                   /* 3 */
  WCDMA_MPWRDRV_CLIENT_W2L,                   /* 4 */
  WCDMA_MPWRDRV_CLIENT_R99_VOICE,             /* 5 */
  WCDMA_MPWRDRV_CLIENT_R99_DATA,              /* 6 */
  WCDMA_MPWRDRV_CLIENT_HSPA,                  /* 7 */
  WCDMA_MPWRDRV_CLIENT_RXD,                   /* 8 */
  WCDMA_MPWRDRV_CLIENT_EQ,                    /* 9 */
  WCDMA_MPWRDRV_CLIENT_QICE,                  /* 10 */
  WCDMA_MPWRDRV_CLIENT_MIMO,                  /* 11 */ 
  WCDMA_MPWRDRV_WAKEUP_UPDATE_REQ,            /* 12 */
  WCDMA_CDRX_GO_TO_LIGHT_SLEEP_REQ,           /* 13 - Data->LightSleep */
  WCDMA_EDRX_GO_TO_LIGHT_SLEEP_REQ,           /* 14 - Data->LightSleep */
  WCDMA_EDRX_GO_TO_SLEEP_REQ,                 /* 15 - LightSleep->Sleep */
  WCDMA_EDRX_WAKEUP_REQ,                      /* 16 - Sleep->LightSleep */
  WCDMA_MPWRDRV_MAX_NUM_CLIENTS
}wmcpmdrv_client_type;

/* These are bitmasks corresponding to wcdma_mpwrdrv_client_type just defined above. */
#define WMCPMDRV_CLIENT_MASK_STACK_ACTIVE               (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_STACK_ACTIVE)
#define WMCPMDRV_CLIENT_MASK_DL_ACTIVE                  (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_DL_ACTIVE)
#define WMCPMDRV_CLIENT_MASK_DRX                        (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_DRX)
#define WMCPMDRV_CLIENT_MASK_W2G                        (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_W2G)
#define WMCPMDRV_CLIENT_MASK_W2L                        (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_W2L)
#define WMCPMDRV_CLIENT_MASK_R99_VOICE                  (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_R99_VOICE)
#define WMCPMDRV_CLIENT_MASK_R99_DATA                   (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_R99_DATA)
#define WMCPMDRV_CLIENT_MASK_HSDPA                      (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_HSDPA)
#define WMCPMDRV_CLIENT_MASK_HSUPA                      (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_HSUPA)
#define WMCPMDRV_CLIENT_MASK_HSPA                       (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_HSPA)
#define WMCPMDRV_CLIENT_MASK_RXD                        (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_RXD)
#define WMCPMDRV_CLIENT_MASK_EQ                         (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_EQ)
#define WMCPMDRV_CLIENT_MASK_QICE                       (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_QICE)
#define WMCPMDRV_CLIENT_MASK_MIMO                       (0x1 << (int32)WCDMA_MPWRDRV_CLIENT_MIMO)
#define WMCPMDRV_CLIENT_MASK_PARAMS_UPDATE              (0x1 << (int32)WCDMA_MPWRDRV_PARMS_UPDATE_REQ)
#define WMCPMDRV_CLIENT_MASK_MAX_OFFLINE_CLK            (0x1 << (int32)WCDMA_MPWRDRV_MAX_OFFLINE_CLK_REQ)
#define WMCPMDRV_CLIENT_MASK_CDRX_LIGHT_SLEEP           (0x1 << (int32)WCDMA_CDRX_GO_TO_LIGHT_SLEEP_REQ)

/* bitmask shift for receiver config request parameters. */
#define WMCPM_RECEIVER_CONFIG_RXD_SHIFT    0 
#define WMCPM_RECEIVER_CONFIG_EQ_SHIFT     1   
#define WMCPM_RECEIVER_CONFIG_QICE_SHIFT   2   
#define WMCPM_RECEIVER_CONFIG_W_MIMO_SHIFT 3   

/* bitmask shift for neighbor meas request parameters. */
#define WMCPM_NEIGHBOR_MEAS_W2G_SHIFT      0 
#define WMCPM_NEIGHBOR_MEAS_W2L_SHIFT      1   

/* No Shift needed for RF BW. Since its just 1 bit. Bit 0: WCDMA and Bit 1: DC_WCDMA */
extern rex_crit_sect_type   wmcpmdrv_lock;

#define WMCPMDRV_MUTEXLOCK() REX_ISR_LOCK(&wmcpmdrv_lock)
#define WMCPMDRV_MUTEXFREE() REX_ISR_UNLOCK(&wmcpmdrv_lock)

/* Internal MACROS */
#define IS_UE_ASLEEP()                  ((!(wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_DRX)) && (wmcpmdrv_int_ue_wakeup_status == FALSE))      
#define IS_VOICE_ACTIVE()               (wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_R99_VOICE)
#define IS_R99_DATA_ACTIVE()            (wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_R99_DATA)
#define IS_DATA_ACTIVE()                (IS_HSPA_DATA_ACTIVE() | IS_R99_DATA_ACTIVE())
#define IS_W2G_CLIENT_ACTIVE()          ((wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_W2G) && \
                                         ((wmcpmdrv_int_neighbor_meas_bmask) & ((0x1)<<WMCPM_NEIGHBOR_MEAS_W2G_SHIFT)))
#define IS_RXD_ACTIVE()                 (wmcpmdrv_int_receiver_cfg_bmask & ((0x1)<<WMCPM_RECEIVER_CONFIG_RXD_SHIFT))
#define IS_EQ_ACTIVE()                  (wmcpmdrv_int_receiver_cfg_bmask & ((0x1)<<WMCPM_RECEIVER_CONFIG_EQ_SHIFT ))
#define IS_QICE_ACTIVE()                (wmcpmdrv_int_receiver_cfg_bmask & ((0x1)<<WMCPM_RECEIVER_CONFIG_QICE_SHIFT))
#define IS_MIMO_ACTIVE()                (wmcpmdrv_int_receiver_cfg_bmask & ((0x1)<<WMCPM_RECEIVER_CONFIG_W_MIMO_SHIFT))
#define IS_R99_ACTIVE()                 (IS_VOICE_ACTIVE () | IS_R99_DATA_ACTIVE())
//#define IS_DC_ACTIVE()                ((wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_DC_HSPA) || (wmcpmdrv_int_rf_bw_bmask))
#define IS_DC_ACTIVE()                  (wmcpmdrv_int_rf_bw_bmask) /* rf_bw_bmask set and reset only inside HSPA client anyway */
#define IS_HSPA_DATA_ACTIVE()           (wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_HSPA)
#define IS_CDRX_LIGHT_SLEEP_ACTIVE()    (wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_CDRX_LIGHT_SLEEP)
#define IS_W2L_ACTIVE()                 (wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_W2L)

#define IS_W_STACK_ACTIVE()             (wmcpmdrv_int_client_bmask & WMCPMDRV_CLIENT_MASK_STACK_ACTIVE)

/*=========================================================================== 
                                  EXTERN GLOBALS 
  =========================================================================== */
extern uint32 wmcpmdrv_int_client_bmask;
extern uint8 wmcpmdrv_int_receiver_cfg_bmask;
extern uint8 wmcpmdrv_int_neighbor_meas_bmask;
extern uint8 wmcpmdrv_int_dl_datarate;
extern uint8 wmcpmdrv_int_ul_datarate;
extern uint8 wmcpmdrv_int_rf_bw_bmask;
extern boolean wmcpmdrv_int_ue_wakeup_status;
extern boolean wmcpmdrv_mcpm_modem_cfg_called_by_W_req;
extern boolean wmcpmdrv_mcpm_modem_cfg_called_by_W_rel;
extern wmcpmdrv_mcpm_offline_clk_speed_type wmcpmdrv_offline_clk_speed_curr;

/*===========================================================================
FUNCTION     WCDMA_MPWRDRV_REQUEST_CLIENT_POWER_CONFIG

DESCRIPTION
  Wrapper function for W MCPM clients to request for power resourcess.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wcdma_mpwrdrv_request_client_power_config(wmcpmdrv_client_type client);

/*===========================================================================
FUNCTION     WCDMA_MPWRDRV_RELINQUISH_CLIENT_POWER_CONFIG

DESCRIPTION
  Wrapper function for W MCPM clients to relinquish power resources.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wcdma_mpwrdrv_relinquish_client_power_config(wmcpmdrv_client_type client);

/*===========================================================================
FUNCTION     wmcpmdrv_get_voice_call_status

DESCRIPTION
  Returns the status of Voice Call from the internal client bit mask

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if voice call is active / FALSE - otherwise)

SIDE EFFECTS
  None.
===========================================================================*/
boolean wmcpmdrv_get_voice_call_status(void);

/*===========================================================================
FUNCTION     wmcpmdrv_get_data_call_status

DESCRIPTION
  Returns the status of data Call from the internal client bit mask

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if data call is active / FALSE - otherwise)

SIDE EFFECTS
  None.
===========================================================================*/
boolean wmcpmdrv_get_data_call_status(void);

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*===========================================================================
FUNCTION     wmcpmdrv_get_r99_data_status

DESCRIPTION
  Returns the status of R99 data call

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if data call is active / FALSE - otherwise)

SIDE EFFECTS
  None.
===========================================================================*/
boolean wmcpmdrv_get_hs_data_status(void);

/*===========================================================================
FUNCTION     wmcpmdrv_get_hs_data_status

DESCRIPTION
  Returns the status of HS data call

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if data call is active / FALSE - otherwise)

SIDE EFFECTS
  None.
===========================================================================*/
boolean wmcpmdrv_get_r99_data_status(void);
#endif /* FEATURE_WCDMA_HS_FACH_DRX */

/*===========================================================================
FUNCTION     wmcpmdrv_get_w2g_client_status

DESCRIPTION
  Returns the status of W2G client from the internal client bit mask

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if W2G client is active / FALSE - otherwise)

SIDE EFFECTS
  None.
===========================================================================*/
boolean wmcpmdrv_get_w2g_client_status(void);

/*===========================================================================
FUNCTION     wmcpmdrv_get_w2l_client_status

DESCRIPTION
  Returns the status of W2L client from the internal client bit mask

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if W2G client is active / FALSE - otherwise)

SIDE EFFECTS
  None.
===========================================================================*/
boolean wmcpmdrv_get_w2l_client_status(void);

/*===========================================================================
FUNCTION     wmcpmdrv_get_ue_asleep_status

DESCRIPTION
  Returns the UE asleep status. Checks both DRX client status in the internal 
  bit mask and also the UE wakeup status variable.

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if UE is asleep and FALSE otherwise).

SIDE EFFECTS
  None.
===========================================================================*/
boolean wmcpmdrv_get_ue_asleep_status(void);

/*===========================================================================
FUNCTION     WMCPMDRV_REGISTER_BLK_RESTORE_CB

DESCRIPTION
  Wrapper function for registering a MCPM callback for restoring the block after power collapse.
PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wmcpmdrv_register_blk_restore_cb(mcpm_modem_block_type block,mcpm_block_restore_callback_type cb);

/*===========================================================================
FUNCTION     WMCPMDRV_REGISTER_SLP_TIMELINE_CB

DESCRIPTION
  Wrapper function for registering a MCPM callback that tells WL1 to use the optimized
  or extended timeline.
PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wmcpmdrv_register_slp_timeline_cb(mcpm_npa_slp_tmln_callback_type cb);

/*===========================================================================
FUNCTION     WMCPMDRV_REGISTER_CLK_CHANGE_CB

DESCRIPTION
  Wrapper function for registering a MCPM callback for change in any clock.
PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wmcpmdrv_register_clk_change_cb(mcpm_modem_clk_type clk,mcpm_clk_speed_change_callback_type cb);

/*===========================================================================
FUNCTION     WMCPMDRV_SEND_SRCH_CLK_SPEED_TO_FW

DESCRIPTION
  This function takes the MCPM speed and converts to corresponding WFW offline clock
  speed state and send it to FW.  

DEPENDENCIES
  None.

PARAMETERS
  One of enums corresponding to LOW,MED or HIGH state.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void wmcpmdrv_send_srch_clk_speed_to_fw();

/*===========================================================================
FUNCTION     WMCPMDRV_GET_MCPM_Q6_CLK_SPEED

DESCRIPTION
  Wrapper function for retrieving offline clock speed from MCPM.
PARAMETERS
  None.

RETURN VALUE
  Q6 clock speed of the type MCPM_ClkSpeedType.

SIDE EFFECTS
  None.
===========================================================================*/
extern MCPM_ClkSpeedType wmcpmdrv_get_mcpm_q6_clk_speed(void);

/*===========================================================================
FUNCTION     WMCPMDRV_RF_FTM_MODE_REQ

DESCRIPTION
Wrapper function for honoring voice requestfrom RF to mcpm when in FTM mode.

PARAMETERS
TRUE or FALSE

RETURN VALUE
None

SIDE EFFECTS
None.
===========================================================================*/
void wmcpmdrv_rf_ftm_mode_req(boolean request);

/*===========================================================================
FUNCTION     wmcpmdrv_q6clk_update_in_mcpm_param

DESCRIPTION
function to send MCVS request when not already running at desired Q6 frequency

PARAMETERS
Q6 frequency wanted

RETURN VALUE
None

SIDE EFFECTS
Blocking call for x usec
===========================================================================*/
void wmcpmdrv_q6clk_update_in_mcpm_param(wmcpmdrv_mcvs_q6clk_speedType q6_speed,mcvs_request_type req_type);

/*===========================================================================
FUNCTION     wmcpmdrv_get_ue_asleep_status

DESCRIPTION
  Returns the W Stack status. 

PARAMETERS
  None.

RETURN VALUE
  boolean (TRUE - if UE is stack is up and FALSE otherwise).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wmcpmdrv_get_w_stack_status(void);

#endif /* FEATURE_WCDMA_MCPM_DRIVER */
#endif /* ifndef WMCPMDRV_H */
