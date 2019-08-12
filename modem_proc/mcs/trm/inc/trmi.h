#ifndef TRMI_H
#define TRMI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Internal Header File

GENERAL DESCRIPTION

  This file provides some common definitions for trm.cpp & trmlog.cpp


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/inc/trmi.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
06/08/2015   sp      Changes for 7mode dynamic PBR bias (825586)
01/22/2015   tl      Move TRM profiling into its own header. Fix fatal error
                      on TRM API limit failure.
01/05/2015   mn      TRM QSH callback function should not wait on TRM critical 
                      section (CR: 776551).
12/28/2014   mn      TRM_SLEEP_DIVERSITY reason should be PAM compatible 
                      (CR: 775802).
12/25/2014   mn      TRM does not grant LTE diversity chain immediately after 
                      a tuneaway (CR: 774926).
12/19/2014   jm      Remove deprecated TRM APIs in TRM (CR: 767147)
12/15/2014   sk      Band compatibility check enhancement changes
12/11/2014   mn      LTE EMBMS functional changes (CR: 769538).
12/03/2014   sr      Use Primary Client Band for Secondary client while
                     updating RF(CR:758273)
12/03/2014   sk      Remove HP_Broadcast reason from tx capable list(762016)
12/03/2014   tl      Disable TRM profiling on off-target builds
12/03/2014   mn      Featurizing the TRM API delay ERR_FATAL under RUMI feature 
                      flag. (CR: 765820).
12/01/2014   tl      Add macros to collect profiling data
11/25/2014   sr      Add a SLEEP Diversity reason for LTE to use for diversity
                     to avoid sending wrong unlocks due to PAM (CR: 751597) 
11/18/2014   mn      A pending Request And Notify Enhanced followed by a 
                      Request And Notify results in Request And Notify Enhanced 
                      callback being called (CR: 752507).
11/04/2014   sk      Enable APS for SRLTE+G DSDS modes only(748680)
10/16/2014   mn      Support for LTE HO RxD (CR: 744539).
10/18/2014   sk      support for DRDSDS ASDiv(707105)
10/16/2014   sk      Added compile assert for offtarget memory issue(734841)
10/01/2014   ag      Ignore the NV bits for QTA,PAM, single chain, diversity
10/08/2014   sk      Added support for cross WTR PBR(708194)
09/29/2014   mn      Added a new TRM_DIVERSITY_IRAT_MEASUREMENT reason (CR: 712921).
09/05/2014   sk      Revert changes to make APS enable bit uniform (719590)
09/04/2014   sk      Changes to make APS enable bit uniform (719590)
08/14/2014   sk      wlan IRAT conflict support(708790)
08/08/2014   mn      Added okay_to_shorten flag for MODIFY_DURATION (CR: 703746).
08/04/2014   sk      Support to enable disable hopping for L+G coex issue(678827)
08/01/2014   sr      W is being allotted dev-3 for diversity when W primary is 
                     on dev-0 (703520)
07/31/2014   sr      Support for PBR Variance (681568)
07/30/2014   sk      Tx Sharing device mapping changes
07/18/2014   mn      1xSRLTE:- Requirement to default PBR bias values fro 1x 
                      and SLTE in SW code (CR: 681568).
07/07/2014   sr      1xSRLTE:-  Requirement for PBR to treat multiple 1x denials 
                      within 100 ms as redundant and get incremented by unity 
                      (CR: 685920).
07/16/2014   mn      TRM should map 1x and GSM2 to chain 2 when LTE is holding 
                      both chain 0 and chain 1 (CR: 695108).
07/07/2014   mn      Changes to support single SIM scenario for TRM
                      mode changes(CR: 679494)
07/03/2014   mn      TRM should not assign a RX only chain for TX reasons. 
                      (CR: 689103).
07/01/2014   sk      Added support for SIM swap without reset(665739)
07/01/2014   sk      NV control for disabling idle mode hopping re-added(684328) 
07/01/2014   mn      Added checks in VCO coupling logic to pair associated 
                      clients on the same WTR (CR: 687479).
06/26/2014   mn      NV control to enable/disable DR-DSDS (CR: 685806)
06/24/2014   sr      Support for 3-DL CA (CR:684208)
06/15/2014   sk      WLAN antenna sharing changes (615092)
06/18/2014   mn      Changes for DR-DSDS (CR: 651800).
04/03/2014   mn      Support for SLTE + CA (CR: 616403).
03/28/2014   sr      Changes for QTA gap issue in SGLTE+G (CR:640167)
02/27/2014   sr      SLTE Changes.
03/12/2014   mn      Adding support for connected mode WTR hopping.
02/08/2014   sk      Changes for reporting switch initiation
02/08/2014   sk      Changes enable\disable Idle hopping using NV
01/10/2014   sk      Changes for SGLTE+G DSDA wtr hopping requirement
11/06/2013   mn      Adding support for Opp SGLTE/SGTDS/SVLTE + G.
05/10/2013   mn      Adding support for TSTS.
03/19/2013   mn      Fix for trying to give 1x secondary chain when GPS is
                     holding a chain and resource is RX_ANY.
12/06/2012   sr      Changes for Band Tune logging issue.
11/22/2012   mn      Triton DSDA changes.
11/05/2012   sr      CD fix(CR: 390912) and Band Tune fix(418118).
10/21/2012   sr      Logging Fix (CR: 412848).
01/10/2012   ag      TRM Feature Cleanup
07/07/2011   ag      Support for wakeup manager.
10/18/2010   ag      Merged DSDS functionality.
11/13/2009   ag      Added modem_mcs_defs.h for GENESIS modem.
07/21/2009   ag      Merged INTLOCK and other Q6 changes from //depot
08/28/2008   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
11/02/2005   cab     Added BEST_POSSIBLE resource option - pre-empts lower
                     pri clients from chain 0, but takes chain 1 if higher
                     pri clients have chain 0
05/26/2005   ajn     Code review comment changes
04/08/2004   ajn     Initial AMSS version

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"
#include "rfm.h"
#include "modem_mcs_defs.h"
#include "trm.h"
#include "trm_wmgr.h"
#include "modem_mem.h"
#include "amssassert.h"
#include "mcsprofile.h"

#ifndef T_WINNT
#include "DDITimetick.h"
#endif /* T_WINNT */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/* MACROS for using rex based critical sections on Q6 platforms */

/* Atomic section macros for initialization of critical section */
#define TRM_ENTER_ATOMIC_SECT()     REX_DISABLE_PREMPTION()
#define TRM_LEAVE_ATOMIC_SECT()     REX_ENABLE_PREMPTION()

/* Atomic section macros for section of code under TASKLOCK */
#define TRM_ENTER_TASKLOCK_SECT()   REX_DISABLE_PREMPTION()
#define TRM_LEAVE_TASKLOCK_SECT()   REX_ENABLE_PREMPTION()

/* Critical section macros */
#define TRM_INIT_CRIT_SECT(lock)    rex_init_crit_sect(&(lock))
#define TRM_ENTER_CRIT_SECT(lock)   \
  do { \
    TRMMARKER(TRM_CRIT_SECT_LOCK_ENTER); \
    rex_enter_crit_sect(&(lock)); \
    TRMMARKER(TRM_CRIT_SECT_LOCK_ACQUIRED); \
  } while(0)
#define TRM_TRY_ENTER_CRIT_SECT(lock)   \
  rex_try_enter_crit_sect(&(lock))
#define TRM_LEAVE_CRIT_SECT(lock)   \
  do { \
    TRMMARKER(TRM_CRIT_SECT_LOCK_RELEASED); \
    rex_leave_crit_sect(&(lock)); \
  } while(0)

#define TRM_MEM_ALLOC( size ) modem_mem_alloc( size, MODEM_MEM_CLIENT_MCS )
#define TRM_MEM_FREE( ptr ) modem_mem_free( ptr, MODEM_MEM_CLIENT_MCS )

/* if primary chain */
#define TRM_IS_PRIMARY_CHAIN(chain)  ( (chain) == TRM_CHAIN_0 || \
                                       (chain) == TRM_CHAIN_2 || \
                                       (chain) == TRM_CHAIN_4 )

/* Macro to get the opposite RXTX Chain */
#define TRM_OPP_RXTX_CHAIN(chain)      ((chain == TRM_CHAIN_0) ? TRM_CHAIN_2 : TRM_CHAIN_0)
#define TRM_OPP_RX_CHAIN(chain)        ((chain == TRM_CHAIN_1) ? TRM_CHAIN_3 : TRM_CHAIN_1)
#define TRM_OPP_SIMULT_RX_CHAIN(chain) TRM_OPP_RX_CHAIN(chain)

#define TRM_ASSOCIATED_RX_CHAIN(chain) ((chain == TRM_CHAIN_0) ? TRM_CHAIN_1 : chain == TRM_CHAIN_1 ? TRM_CHAIN_0 : (chain == TRM_CHAIN_2) ? TRM_CHAIN_3 : (chain == TRM_CHAIN_3) ? TRM_CHAIN_2 : (chain == TRM_CHAIN_4) ? TRM_CHAIN_5 : TRM_CHAIN_4)
#define TRM_ASSOCIATED_RXTX_CHAIN(chain) ((chain == TRM_CHAIN_1) ? TRM_CHAIN_0 : ((chain == TRM_CHAIN_3) ? TRM_CHAIN_2 : TRM_CHAIN_4))
									   

#define TRM_ASSOCIATED_PRX_DRX_CHAIN(chain) (TRM_IS_PRIMARY_CHAIN(chain))? \
                                                TRM_ASSOCIATED_RX_CHAIN(chain):\
                                                   TRM_ASSOCIATED_RXTX_CHAIN(chain) 

#define TRM_OPP_PRX_DRX_CHAIN(chain) (TRM_IS_PRIMARY_CHAIN(chain))? \
                                                TRM_OPP_RXTX_CHAIN(chain):\
                                                   TRM_OPP_RX_CHAIN(chain) 

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define TRM_NOTUSED(i) if(i){}

#define TRM_ARR_SIZE(a) sizeof(a)/sizeof(a[0])

/*----------------------------------------------------------------------------
  Clients
----------------------------------------------------------------------------*/

/* Client is a valid client identifier */
#define VALID_CLIENT(c)           ( (unsigned)c < TRM::num_clients )

/* Returns the opposite priority */
#define OPPOSITE_SUB_CLIENT_PRIORITY(prio) \
	(((prio)==TRM_PRIO_HIGH) ? TRM_PRIO_LOW : TRM_PRIO_HIGH)

/*----------------------------------------------------------------------------
  RF Chains
    The clients request resources with certain capabilites, which need to be
    converted to more specific RF chain requests.  [The second RF chain may
    not exist.  If it does exist, it may not have the same sensitivity as the
    first, and probably doesn't have an associated transmit chain.]
----------------------------------------------------------------------------*/

/* Macro used to check if SV bit is on in the RF multi tech support bitmask */
#define IS_SV_BIT_ON(svcap) ((svcap) & 2)

#define IS_RF_DEVICE_TX_CAPABLE(cap)   (cap & RFM_DEVICE_TX_SUPPORTED)
#define IS_RF_DEVICE_RX_CAPABLE(cap)   (cap & RFM_DEVICE_RX_SUPPORTED)
#define IS_RF_DEVICE_DSDA_CAPABLE(cap) (cap & RFM_DEVICE_PREFERRED_SUBS_2)

#define IS_SGLTE_DSDA_MODE(mode)  (((mode) & TRM_SGLTE_SGTDS_IS_ENABLED) && ((mode) & TRM_DSDA_IS_ENABLED))
#define IS_SVLTE_DSDA_MODE(mode)  (((mode) & TRM_SVLTE_IS_ENABLED) && ((mode) & TRM_DSDA_IS_ENABLED))

#define IS_SRLTE_DSDS_MODE(mode)  (((mode) & TRM_SRLTE_MODE_BIT) && \
                                   ((mode) & TRM_DSDS_MODE_BIT))
#define IS_SRLTE_ONLY_MODE(mode)  ((mode) & TRM_SRLTE_MODE_BIT) 
#define IS_DUAL_MULTIMODE(mode)   ((mode) & TRM_DUAL_MULTI_MODE_BIT) 

#define TRM_COMPILE_ASSERT(a)  switch(0){case 0: case (a):;}

/*----------------------------------------------------------------------------
  RF Chain Groups

  TRM will assign clients to "RF Chain Groups", or simply "groups", in the
  following manner:

    -1: Client has not yet been assigned to an RF chain.
    -1: Client cannot be assigned to an RF chain.
     0: Client is assigned to the primary RF chain.
     1: Client is assigned to the secondary RF chain.
    2+: Client will be assigned to a chain, but which is not yet determined.

  For group "g" (g >= 0), "g xor 1" represents the group assigned to the other
  RF chain.  For example, if clients in group 2 end up being assigned to the
  secondary RF chain, group 3 (2 xor 1) will be assigned to the primary.

----------------------------------------------------------------------------*/

  typedef enum
  {
    NO_GROUP      = -1,

    CHAIN_0_GROUP = 0,

    CHAIN_1_GROUP,

    CHAIN_2_GROUP,

    CHAIN_3_GROUP,

    CHAIN_4_GROUP,

    CHAIN_5_GROUP,

    CHAIN_GPS_GROUP,

    CHAIN_WLAN_GROUP,

    CHAIN_SINGLE_RX_CA_GROUP,

    CHAIN_SINGLE_RX_CA_DIV_GROUP,

    CHAIN_MAX_GROUP

  } trm_chain_offset_type;

  typedef trm_chain_offset_type trm_group;

#define TRM_NUM_CHAIN_PAIRS         TRM_MAX_CHAINS/2+1
#define TRM_CHAIN_PAIR_PRIMARY      0
#define TRM_CHAIN_PAIR_DUAL_RECEIVE 1

/* Any reason which TRM considers urgent will cause TRM to ask retain lock 
   holders to unlock immediately. */
#define URGENT_REASON(reason)     ( ( (reason) == TRM_ACCESS_URGENT ) ||       \
                                    ( (reason) == TRM_SUBS_CAP_CHANGE ) )

/* Check if the lock is for future or for current use */
#define FUTURE_LOCK(s)            ((s) == TRM_RESERVED_AT) ? 1 : 0

/* Group is a valid group identifier (Eg, not NO_GROUP) */
#define IS_VALID_GROUP(g)            (((g) >= CHAIN_0_GROUP) && ((g) < CHAIN_MAX_GROUP))

#define IS_VALID_CLIENT(client)     (((client) >= TRM_1X) && ((client) <= TRM_LAST_CLIENT))
#define IS_RAT_GROUP_VALID(rat_group)  (((rat_group) >= TRM_RAT_GROUP_1X) && ((rat_group) < TRM_RAT_GROUP_NONE))

#define IS_TRM_CA_CLIENT(client)  ((client) == TRM_LTE_CA || (client) == TRM_LTE_CA1 || (client) == TRM_LTE_CA_SECONDARY || \
                               (client) == TRM_LTE_CA1_SECONDARY || (client) == TRM_UMTS_CA || (client) == TRM_UMTS_CA_SECONDARY)

#define IS_TRM_HO_RXD_CLIENT(client)  (((uint64)1 << (client)) & (((uint64)1 << TRM_LTE_HO_SECONDARY1) | ((uint64)1 << TRM_LTE_HO_SECONDARY2)))

#define IS_LOCK_OWNER(client)     ( ((client).lock_state == TRM_LOCK_RETAINED_ENH) || ((client).lock_state == TRM_LOCK_RETAINED) || ((client).lock_state == TRM_LOCK_RETAINED_ADVANCED) || ((client).lock_state == TRM_LOCKED_UNTIL) || ((client).lock_state == TRM_EXTENDING) || ((client).lock_state == TRM_GRANTING) || ((client).lock_state == TRM_GRANTING_ENH) )

/* Identifier for invalid band, also used as initializer */
#define TRM_BAND_INVALID          -1

#define TRM_PBR_DRX_BIAS_PROD(client)   (uint64)((client).pbr_info.pbr_bias * (uint64)(client).pbr_info.drx_cycle)
#define TRM_PBR_COUNT_DRX_PROD(client)  (uint64)((client).pbr_info.pbr_count * (uint64)(client).pbr_info.drx_cycle)

#define TRM_CALC_PBR(client)                                             \
          ((uint64)(TRM_PBR_DRX_BIAS_PROD(client)) *                                             \
           ((uint64)(client).pbr_info.pbr_count - (uint64)(client).pbr_info.pbr_accounted));   \

#define TRM_CHECK_PBR_PARTICIPATION(client, reason)                                    \
          ((uint64)((client).pbr_info.pbr_modes) & (uint64)((uint64)1 << (reason)))

/*----------------------------------------------------------------------------
  Definitions for Markov Algorithm
----------------------------------------------------------------------------*/

/* Max PBR Count any tech can reach after which all the tech PBR counts
   would be rebased. */
#define TRM_MAX_PBR_COUNT      4294967290U

/* Base count denotes the scaling value by which all PBR counts will be 
   scaled when they go above a certain threshold */
#define TRM_BASE_PBR_COUNT     2

/*----------------------------------------------------------------------------
  TRM Log Information
----------------------------------------------------------------------------*/

/* By default PAM, SVDO and SVLTE are disabled */
#define TRM_CONFIG_DISABLED       0

#define UINT64ONE                       ((uint64) 1)

/* macro to check if the resource asked involves choosing between primary and
   simultaneous rx chain. Currently, rx_any, rx_best_possible and rx_best_possible_2
   involves simultaneous rx chain */
#define TRM_SIMUL_RX_RESOURCE_MASK  (uint64) ( (UINT64ONE<<TRM_RX_ANY) | \
                                              (UINT64ONE<<TRM_REVERSE_RX_ANY) | \
                                              (UINT64ONE<<TRM_RX_BEST_POSSIBLE) | \
                                              (UINT64ONE<<TRM_RX_BEST_POSSIBLE_2) )


#define INVOLVES_SIMUL_RX_RESOURCE(res) ( TRM_SIMUL_RX_RESOURCE_MASK & UINT64ONE<<res )

#define TRM_VARIABLE_RX_RESOURCE_MASK  (uint64) ( (UINT64ONE<<TRM_RX_ANY) | \
                                                 (UINT64ONE<<TRM_RXTX_ANY) | \
                                                 (UINT64ONE<<TRM_RX_BEST_POSSIBLE) | \
                                                 (UINT64ONE<<TRM_RX_BEST_POSSIBLE_2) | \
                                                 (UINT64ONE<<TRM_RXTX_BEST_POSSIBLE) | \
                                                 (UINT64ONE<<TRM_REVERSE_RX_ANY) | \
                                                 (UINT64ONE<<TRM_REVERSE_RX_BEST_POSSIBLE) | \
                                                 (UINT64ONE<<TRM_RX_BEST_POSSIBLE_MOD) | \
                                                 (UINT64ONE<<TRM_RX_BEST_POSSIBLE_MOD_2))


/* Check to see if the resource is a variable resource. That is, it does not necessarily
   map to a single chain */
#define IS_VARIABLE_RESOURCE(res) ( TRM_VARIABLE_RX_RESOURCE_MASK & UINT64ONE<<res )

#define TRM_DIVERSITY_CLIENT_MASK (uint64) ( (UINT64ONE<<TRM_1X_SECONDARY) | \
                                             (UINT64ONE<<TRM_HDR_SECONDARY) | \
                                             (UINT64ONE<<TRM_UMTS_SECONDARY) | \
                                             (UINT64ONE<<TRM_LTE_SECONDARY) | \
                                             (UINT64ONE<<TRM_TDSCDMA_SECONDARY) | \
                                             (UINT64ONE<<TRM_GSM1_SECONDARY) | \
                                             (UINT64ONE<<TRM_GSM2_SECONDARY) | \
                                             (UINT64ONE<<TRM_GSM3_SECONDARY) | \
                                             (UINT64ONE<<TRM_LTE_CA_SECONDARY) | \
                                             (UINT64ONE<<TRM_LTE_CA1_SECONDARY) | \
                                             (UINT64ONE<<TRM_UMTS_CA_SECONDARY) )
									   
#define IS_DIVERSITY_CLIENT(id) ( TRM_DIVERSITY_CLIENT_MASK & UINT64ONE<<id )
										

#define TRM_VARIABLE_CHAIN_MASK (uint64) ( (UINT64ONE<<TRM_CHAIN_ANY) | \
                                          (UINT64ONE<<TRM_CHAIN_BEST_POSSIBLE) | \
                                          (UINT64ONE<<TRM_CHAIN_BEST_POSSIBLE_2) | \
                                          (UINT64ONE<<TRM_CHAIN_REVERSE_BEST_POSSIBLE) | \
                                          (UINT64ONE<<TRM_CHAIN_REVERSE_RX_ANY) | \
                                          (UINT64ONE<<TRM_CHAIN_BEST_POSSIBLE_MOD) | \
                                          (UINT64ONE<<TRM_CHAIN_BEST_POSSIBLE_MOD_2) )
                     			  							
#define IS_VARIABLE_CHAIN(chain) ( TRM_VARIABLE_CHAIN_MASK & UINT64ONE<<chain )
									
/* SRLTE and DSDS mode bits */
#define TRM_DSDS_MODE_BIT     (uint8)0x1
#define TRM_SRLTE_MODE_BIT    (uint8)0x2
#define TRM_DUAL_MULTI_MODE_BIT (uint8)0x4

/* Macros for handling of bitmasks */
#define TRM_SET_BIT(cfg,data)                 ((cfg) |= (data))
#define TRM_RESET_BIT(cfg,data)               ((cfg) &= ~(data))
#define TRM_IS_BIT_ENABLED(bitmask, bit_val)  ((bitmask) & (bit_val))
#define TRM_MSK_BIT_ON(mask, bit)          ( (mask) & ( 1 << bit ) )
#define TRM_SET_BIT_ON(mask, bit)          ( (mask) |= ( 1 << bit ) )
#define TRM_SET_BIT_OFF(mask, bit)         ( (mask) &= ~( 1 << bit ) )

/*----------------------------------------------------------------------------
  Special constants
----------------------------------------------------------------------------*/

/* If a client holds a lock on a resource for longer than promised,
   TRM needs to "guess" at when the lock will be released, in order for
   deterministic lock assignments to be made.  Assume the client holding
   the lock will release it in a few milliseconds. */
#define TRM_OVERRUN_ENDPOINT_MS   1
#define TRM_OVERRUN_ENDPOINT      (int64)(TIMETICK_SCLK_FROM_MS(TRM_OVERRUN_ENDPOINT_MS))
#define TRM_PBR_ENDPOINT_MS   100
#define TRM_PBR_ENDPOINT      (int64)(TIMETICK_SCLK_FROM_MS(TRM_PBR_ENDPOINT_MS))

#define TRM_RES_AT_REDUNDANCY_TIME_MS 5
#define TRM_RES_AT_REDUNDANCY_TIME      (int64)(TIMETICK_SCLK_FROM_MS(TRM_RES_AT_REDUNDANCY_TIME_MS))

/* If a client is holding a lock and higher priority client is reserving 
   the lock for/within next few milliseconds, TRM has to send a unlock required
   or unlock immediate callback to the lock/chain holder */
#define TRM_UNLOCK_ENDPOINT_MS   5
#define TRM_UNLOCK_ENDPOINT (int64)(TIMETICK_SCLK_FROM_MS(TRM_UNLOCK_ENDPOINT_MS))

#define TRM_MAX_DURATION          0x7fffffff

/* Minimum relative time in the future in which client can reserve resource
   Any time before this will be assumed as 'now'. This basically needs to be
   TRM_MIN_RES_TIME > sclk( trm_reserve_at is processed ) - 
   sclk(trm_reserve_at is called)                           ~1 sec */
#define TRM_MIN_RES_TIME          0x8000

/* 1x mode mode */
#define TRM_1X_MODE_MASK (0x8|0x4)
#define TRM_1X_MODE_SHIFT 0x2

/*----------------------------------------------------------------------
    TRM NV 3446 RF Config bit positions START
----------------------------------------------------------------------*/
/* define masks for extracting bit fields from nv item...
   0 - Single chain mode ...not supported
   1 - Diversity only on second chain ...not supported
   2 - Two chain unequal sensitivity  ...default mode
   3 - Two chain equal sensitivity    ...not supported   */
#define TRM_RF_ENUM_MASK  0x3

/* 0x4 and 0x8 are reserved */

/* TRM V2.1 Specific change... not used but the bit position should not be reused */
#define TRM_PAM_MODE_MASK 0X10

/* TRM V2.2 SVLTE Mask */
#define TRM_SVLTE_MASK 0x20

/* TRM V2.2 SVDO Mask... not used but the bit position should not be reused */
#define TRM_SVDO_MASK 0x40

/* TRM QTA logic kill bit... not used but the bit position should not be reused */
#define TRM_QTA_LOGIC_MASK 0x80

/* TRM APS logic enable bit*/
#define TRM_APS_LOGIC_MASK 0x100

/* TRM Idle hopping kill bit */
#define TRM_IDLE_HOPPING_MASK 0x200

/* tx sharing enable bit */
#define TRM_TX_SHARING_MASK 0x400
/*----------------------------------------------------------------------
    TRM NV 3446 RF Config bit positions END
----------------------------------------------------------------------*/

/* TRM TDD TECH info */
#define TRM_IS_TDD_TECH(client_id) ( trm.client[(client_id)].tech_info_bitmask & \
                                     1<<TRM_TDD_TECH_INFO )

#define TRM_DIVERSITY_REASON_MASK (uint64) ( (UINT64ONE<<TRM_DIVERSITY) | \
                                            (UINT64ONE<<TRM_DIVERSITY_INTERLOCK) | \
                                            (UINT64ONE<<TRM_DIVERSITY_LOWCMTHRESH) | \
                                            (UINT64ONE<<TRM_ACCESS_DIVERSITY) | \
                                            (UINT64ONE<<TRM_IDLE_DIVERSITY) | \
                                            (UINT64ONE<<TRM_ACQ_DIVERSITY) | \
                                            (UINT64ONE<<TRM_SLEEP_DIVERSITY) | \
                                            (UINT64ONE<<TRM_DIVERSITY_IRAT_MEASUREMENT) )

#define IS_REASON_DIV(client_id) ( TRM_DIVERSITY_REASON_MASK & UINT64ONE<<trm.client[(client_id)].reason )

#define TRM_TX_REASON_MASK (uint64) ( (UINT64ONE<<TRM_ACCESS) | \
                                     (UINT64ONE<<TRM_ACCESS_URGENT) | \
                                     (UINT64ONE<<TRM_BROADCAST_ACCESS) | \
                                     (UINT64ONE<<TRM_TRAFFIC) | \
                                     (UINT64ONE<<TRM_BROADCAST_TRAFFIC) | \
                                     (UINT64ONE<<TRM_LOW_LAT_TRAFFIC) | \
                                     (UINT64ONE<<TRM_BG_TRAFFIC) | \
                                     (UINT64ONE<<TRM_MC_TRAFFIC) )

#define TRM_IS_TX_REASON(reason)  ( TRM_TX_REASON_MASK & UINT64ONE<<(reason) )

#define TRM_NON_DR_REASON_MASK (uint64) ( (UINT64ONE<<TRM_ACQUISITION_HI) | \
                                         (UINT64ONE<<TRM_ACQUISITION_INV) | \
                                         (UINT64ONE<<TRM_ACQUISITION_INV_HI) | \
                                         (UINT64ONE<<TRM_ACQUISITION_LOW) | \
                                         (UINT64ONE<<TRM_ACQUISITION_MAX) | \
                                         (UINT64ONE<<TRM_ACQUISITION) )
                                         
#define TRM_IS_DIVERSITY_GROUP(group)   ( ( (group) < CHAIN_GPS_GROUP ) && \
                                          ( (group)%2 == 1 ) )

#define TRM_IS_NON_DR_REASON(reason) ( TRM_NON_DR_REASON_MASK & UINT64ONE<<(reason) )

#define IS_MULTISUB_CLIENT(client_id) (trm.client[(client_id)].is_multisub_client == TRUE)

#define IS_TRM_CLIENT_VALID(client_id) (((client_id) >= TRM_1X) && ((client_id) <= TRM_LAST_CLIENT))

#define DEVICE_TX_CAPABLE 2 

/* bitmask with all hopping behavior */
#define TRM_ALL_HOP_MASK  (uint8)( (1<<TRM_HOPPING_TYPE_IDLE_MODE) | \
                                   (1<<TRM_HOPPING_TYPE_CONNECTED_MODE) )

/* hop state is waiting or is in progress */
#define TRM_HOP_IN_PROGRESS(id) ( trm.client[(id)].hopping_info.hop_state == TRM_HOP_STATE_PENDING || \
                                  trm.client[(id)].hopping_info.hop_state == TRM_HOP_STATE_HOP )


/* TRM V4 Logging */
#define TRM_NV_INACTIVE_INDICATOR 0xDEADDEAD

/* Priorities are on the range 1..255 */
  typedef uint8                     trm_pri_t;

/* Invalid client/reason pairs are indicated by a priority of BAD */
#define BAD                       0


/* Client/reason compatibility mask */
  typedef uint64                    trm_compatible_mask_t;

/*-----------------------------------------------------------------------------
  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  Enumerations in this file are 8-bit values that are used directly by the TRM
  log.  Changing enumerator values and adding new enumerator values will
  necessitate a new TRM log parser.
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Client lock state

    A TRM client may be inactive, waiting for an RF resource, or holding one.
    When waiting for an RF resource, it may need it at a particular time, or
    merely ASAP.  When holding an RF resource, it may need it for a specific
    length of time, or as long as possible.

    When TRM is processing a client request, it may temporarily change a
    client's state during processing, and restore the state when done, or
    change the client to a new state.
-----------------------------------------------------------------------------*/

  typedef enum
  {
    /* Client does not have an RF chain, nor does it want one */
    TRM_INACTIVE,

    /* Client has a reservation for an RF chain at a particular time */
    TRM_RESERVED_AT,

    /* Client is requesting an RF chain immediately */
    TRM_REQUEST,

    /* Client requests to be notified when it can be granted an RF chain */
    TRM_REQUEST_AND_NOTIFY,

    /* Client has been given the RF chain, & will keep it until told not to */
    TRM_LOCK_RETAINED,

    /* Client has been given an RF chain until a specific time */
    TRM_LOCKED_UNTIL,


    /* The following states are TRANSIENT states.  TRM will change the client
       state to one of the above states before returning */


    /* Client is trying to extend the "locked until" duration */
    TRM_EXTENDING,

    /* Client has been assigned an RF chain, but client hasn't been told yet */
    TRM_GRANTING,

    /* Client has been assigned an RF chain and the maximum preemptible
       extension has been applied but the client hasn't been told yet */
    TRM_GRANTING_ENH,

    /* Client is requesting for a maximum possible extension of the lock after
       the minimum duration has been granted */
    TRM_REQUEST_AND_NOTIFY_ENH,

    /* Client has been given a RF chain for a minimum non premptible duration
       plus a preemptible maximum duration possible. */
    TRM_LOCK_RETAINED_ENH,

    /* Client has been given the RF chain, & will keep it until told not to */
    TRM_LOCK_RETAINED_ADVANCED
  } trm_client_lock_state_enum;

/*----------------------------------------------------------------------------
   Band Class State Enum definitions
 ----------------------------------------------------------------------------*/
  typedef enum
  {
    /* No band class activity going on or band tuning is completed */
    TRM_BC_STATE_INACTIVE,
    /* A new band class has been requested */
    TRM_BC_STATE_REQUEST,
    /* Waiting for a client to release its lock */
    TRM_BC_STATE_WAITING,
    /* Tuning to the requested band */
    TRM_BC_STATE_TUNING,
    /* Wait complete and the band can be granted */
    TRM_BC_STATE_GRANTED

  } trm_bc_enum_t;

/*-----------------------------------------------------------------------------
  RF Chains
    The clients request resources with certain capabilites, which need to be
    converted to more specific RF chain requests.  [The second RF chain may
    not exist.  If it does exist, it may not have the same sensitivity as the
    first, and probably doesn't have an associated transmit chain.]
-----------------------------------------------------------------------------*/

  typedef enum
  {
    /* The primary RF chain */
    TRM_CHAIN_0,

    /* The secondary RF chain */
    TRM_CHAIN_1,

    TRM_CHAIN_DIVERSITY = TRM_CHAIN_1,


    /* Any RF chain */
    TRM_CHAIN_ANY,

    /* The best possible RF chain */
    TRM_CHAIN_BEST_POSSIBLE,

    /* No RF chain */
    TRM_CHAIN_NONE,

    /* The third RF chain */
    TRM_CHAIN_2,

    /* The fourth RF chain */
    TRM_CHAIN_3,

    /* Best Possible 2 Chain */
    TRM_CHAIN_BEST_POSSIBLE_2,

    TRM_CHAIN_4,

    TRM_CHAIN_REVERSE_BEST_POSSIBLE,

    TRM_CHAIN_REVERSE_RX_ANY,

    TRM_CHAIN_BEST_POSSIBLE_MOD,

    TRM_CHAIN_BEST_POSSIBLE_MOD_2,

    TRM_CHAIN_SECOND_PREFERRED,

    TRM_CHAIN_5,

    TRM_CHAIN_GPS,

    TRM_CHAIN_WLAN,

    TRM_CHAIN_SINGLE_RX_CA,

    TRM_CHAIN_SINGLE_RX_CA_DIV,

    /* This needs to be the last value */
    TRM_CHAIN_LAST = TRM_CHAIN_SINGLE_RX_CA_DIV,

    TRM_CHAIN_MAX
  } trm_chain_enum;

/*-----------------------------------------------------------------------------
  RF Chain Groups

    TRM will assign clients to "RF Chain Groups", or simply "groups", in the
    following manner:

      -1: Client has not yet been assigned to an RF chain, or
          client cannot be assigned to an RF chain.
       0: Client is assigned to the primary RF chain.
       1: Client is assigned to the secondary RF chain.
      2+: Client will be assigned to a chain, but which is not yet determined.

    For group "g" (g >= 0), "g xor 1" represents the group assigned to the
    other RF chain.  For example, if clients in group 2 end up being assigned
    to the secondary RF chain, group 3 (2 ^ 1) will be assigned to the primary.

-----------------------------------------------------------------------------*/

//typedef int8                      trm_group;


/* Custom Data Type for Dual Receive Bitmask */
  typedef uint32 trm_slte_bitmask;

/*----------------------------------------------------------------------------
  SLTE Type information.
----------------------------------------------------------------------------*/
  typedef enum
  {
    TRM_DR_SLTE_DISABLED = 0,
    TRM_DR_SLTE_1X       = 1,
    TRM_DR_SLTE_GSM      = 2,
    TRM_DR_DSDS_ENABLED  = 4
  } trm_slte_info_type;

/*----------------------------------------------------------------------------
  TRM RAT coups
----------------------------------------------------------------------------*/
  typedef enum
  {
    TRM_RAT_GROUP_1X,

    TRM_RAT_GROUP_HDR,

    TRM_RAT_GROUP_GERAN,

    TRM_RAT_GROUP_WCDMA,

    TRM_RAT_GROUP_TDSCDMA,

    TRM_RAT_GROUP_LTE,

    TRM_RAT_GROUP_NONE,

    TRM_RAT_GROUP_ABSENT,

    TRM_RAT_GROUP_MAX = TRM_RAT_GROUP_NONE
  } trm_rat_group_type;

/*----------------------------------------------------------------------------
  VCO Coupling info type
----------------------------------------------------------------------------*/
  typedef struct
  {
    trm_rat_group_type rat_group;
    boolean            enabled;
  } trm_vco_coupling_info_type;

/*----------------------------------------------------------------------------
  TRM client and chain mapping information.
----------------------------------------------------------------------------*/
  typedef struct
  {
    trm_chain_enum first_config_chain[TRM_MAX_CLIENTS];
    trm_chain_enum second_config_chain[TRM_MAX_CLIENTS];
  } trm_client_chain_info_type;

/*----------------------------------------------------------------------------
  trm_get_device_mapping Input Type.
----------------------------------------------------------------------------*/
typedef struct
{
  /* client requesting band info */
  trm_client_enum_t     client;

  /* resource type being requested */
  trm_resource_enum_t   resource;

  /* Band information */
  trm_band_t            band;
} trm_get_device_mapping_input_type;

/*-----------------------------------------------------------------------------
  Client state information

    A client may be inactive, requesting a lock, or holding one.
    If it is requesting a lock, it requests a lock on a resource, which maps
    to an RF chain (primary, secondary, or any).
    It will request or hold a lock for a specific reason, which will map to
    a particular priority.
    TRM will assign the request into a group, based on priority and conflicts.
    A client may be granted an lock on an RF chain (primary, or secondary),
    which correspond to an RF device.
    If holding a lock, it may report to the client when it must release it.
-----------------------------------------------------------------------------*/

/*
  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  This structure is a hand-packed 8-byte long record, and is used for logging.
  Changes to the size, order, or content will require an updated log parser. */

  struct trm_client_state
  {
    /* Lock state (inactive, requesting a lock, holding a lock, ... ) */
    trm_client_lock_state_enum      lock_state;

    /* Resource the client wants (RX_ANY, RXTX, RX_GPS, ...) */
    trm_resource_enum_t             resource;

    /* Reason the client wants the resource (Acq, Traffic, QPCH, ...) */
    trm_reason_enum_t               reason;

    /* Relative priority (based on client and reason) */
    trm_pri_t                       priority;

    /* RF Chain client wants (based on resource & h/w capability) */
    trm_chain_enum                  chain;

    /* What the client currently holds (Nothing, Chain 0, or Chain 1) */
    trm_grant_event_enum_t          granted;

    /* Internal TRM group assignment, for conflict resolution */
    trm_group                       group;

    /* When the client must give up a lock (By a certain time, now, never) */
    trm_unlock_event_enum_t         unlock_state;


  };

/*----------------------------------------------------------------------------
  TRM Internal structure to hold all the wakeup clients information
----------------------------------------------------------------------------*/
  typedef struct
  {
    /* Is client registered */
    boolean                   registered;

    /* Client ID */
    trm_wakeup_client_enum_t  client;

    /* CLient Result */
    trm_wakeup_info_type      *result;

    /* Call back pointer */
    trm_wmgr_cb_type          cb;

  } trm_wakeup_client_type;

/*----------------------------------------------------------------------------
  TRM profiling

    Use MCS profiling to profile TRM

----------------------------------------------------------------------------*/

#define TRMMARKER MCSMARKER

/*----------------------------------------------------------------------------
  Time limits for TRM API calls

    The data structures and macros in this section implement configurable time
    limits for TRM API calls. If the time elapsed by certain TRM API functions
    or external API calls exceeds a configurable threshold, the TRM_API_EXIT()
    macro will log a warning message or cause a fatal error.

----------------------------------------------------------------------------*/

#ifdef T_WINNT
#error code not present
#else

/* EFS file used to configure the API time limits. */
#define TRM_PROFILE_EFS_PATH "/nv/item_files/mcs/trm/limits"

/* The time limits, in microseconds, at which to warn or error for a specific
 * API call. */
typedef struct
{
  /* Log a warning message if the API elapsed time exceeds this value, in
   * microseconds. If this value is zero, no warning will be logged regardless
   * of elapsed time. */
  timetick_type warn;

  /* Cause a fatal error if the API elapsed time exceeds this value, in
   * microseconds. If this value is zero, no fatal error will occur regardless
   * of elapsed time. */
  timetick_type fatal;
} trm_profile_limits_elem_type;

/* The time limits at which to warn or error for TRM API calls. Each API that
 * uses the TRM_API_ENTRY()/TRM_API_EXIT() macros must have an entry in this
 * data structure. */
typedef struct
{
  trm_profile_limits_elem_type  trm_request;
  trm_profile_limits_elem_type  trm_reserve_at;
  trm_profile_limits_elem_type  trm_request_and_notify;
  trm_profile_limits_elem_type  trm_modify_chain_state_reason;
  trm_profile_limits_elem_type  rfm_verify_rf_concurrency;
} trm_profile_limits_type;

/* The time limits for TRM API calls. This data structure is populated at
 * initialization from EFS, if specified in TRM_PROFILE_EFS_PATH, or from
 * default values. */
extern trm_profile_limits_type trm_profile_limits;

extern DalDeviceHandle *trm_timetick_handle;

/* Record the current timestamp, which will be checked in TRM_API_EXIT().
 * Intended to be used at the beginning of a TRM function, or prior to calling
 * an external function. */
#define TRM_API_ENTRY() \
  DalTimetickTime64Type api_entry_timestamp; \
  DalTimetick_GetTimetick64(trm_timetick_handle, &api_entry_timestamp)

/* Check the elapsed time since TRM_API_ENTRY(). If the elapsed time exceeds
 * the warn level, log a message. If the elapsed time exceeds the fatal level,
 * cause a fatal error. */
#ifndef T_RUMI_EMULATION
#define TRM_API_EXIT(func) \
  do { \
    DalTimetickTime64Type api_exit_timestamp; \
    DalTimetick_GetTimetick64(trm_timetick_handle, &api_exit_timestamp); \
    DalTimetickTime64Type elapsed; \
    DalTimetick_CvtFromTimetick64(trm_timetick_handle, \
        api_exit_timestamp - api_entry_timestamp, T_USEC, &elapsed); \
    if((trm_profile_limits.func.warn > 0) && \
        (elapsed >= trm_profile_limits.func.warn)) \
    { \
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, \
          "TRM API " #func " elapsed time: %d us", elapsed); \
    } \
    if((trm_profile_limits.func.fatal > 0) && \
        (elapsed > trm_profile_limits.func.fatal)) \
    { \
      ERR_FATAL("TRM API " #func " exceeded time limit. " \
          "Elapsed %d us (Start %x end %x)", \
          elapsed, api_entry_timestamp, api_exit_timestamp); \
    } \
  } while(0)
#else
  #define TRM_API_EXIT(func)
#endif /* T_RUMI_EMULATION */

#endif /* !T_WINNT */

/*============================================================================

FUNCTION TRM_ASDIV_INIT

DESCRIPTION
  Initializes the asdiv related variables, called from TRM during TRM
  initialization
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_init(boolean en, uint8 modes, trm_slte_bitmask dr_mask);

/*============================================================================

FUNCTION TRM_ASDIV_MUTEX_INIT

DESCRIPTION
  Initializes the asdiv mutex, TRM needs to call this api first to initialize
  the asdiv mutex
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_asdiv_mutex_init( void );

/*============================================================================

FUNCTION TRM_SWITCH_REASON_HI_LO

DESCRIPTION
  Changes the reason to the corresponding HI reason if the hi_lo is TRM_PRIO_HI
  else changes it to LO if hi_lo is TRM_PRIO_LO.
  This is an internal function, called when the TRM GSM clients change their 
  client priority and whenever the higher priority stack makes TRM requests.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_switch_reason_hi_lo
  (
  trm_reason_enum_t                  *reason,

  trm_client_priority_enum_t    hi_lo
  );

/*============================================================================

FUNCTION TRM_UPDATE_CHAIN_CLIENT_STATUS

DESCRIPTION
  API to update lmtsmgr with the chain and client status
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_update_chain_client_status
  (
  trm_client_enum_t client,    /* Client's whose state update is sent */
  trm_reason_enum_t reason,   /* Reason for which client is holding chain */
  trm_band_type band,          /* Band class used */
  uint8 priority
  );

/*============================================================================
FUNCTION TRM_UPDATE_START_END

DESCRIPTION
  This just updates start, end time for all the clients in the priority order
  list and the chain owners

DEPENDENCIES
  Current time

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_update_start_end 
  (
  trm_timestamp_t  now
  );

/*============================================================================
FUNCTION TRM_INVALID_REASON

DESCRIPTION
  Checks if the reason passed is invalid

DEPENDENCIES
  Current time

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  boolean trm_invalid_reason(trm_reason_enum_t       reason);

/*============================================================================

Function trm_update_bookkeeper_record

Description
  Update the trm_bookkeeper record with the proper second configuration.
  For Bolt SHDR, second config can differ according to the chain that 
  supports dual receive for a band

Dependencies
  Internal to assign group

Return Value
  None

Side Effects
  trm.any_bp_bookkeeper is updated by this funtion

============================================================================*/
  void trm_update_bookkeeper_record
  (
  trm_client_enum_t  client,
  trm_timestamp_t now
  );

/*============================================================================

FUNCTION TRM_UPDATE_EARLIEST

DESCRIPTION
  Determine the earliest request that clients in the same RF group require
  an RF chain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  trm.earliest[ ] updated.

============================================================================*/

  void trm_update_earliest( void );

/*============================================================================

Member Function TRM_GET_MAX_PBR_CLIENT

Description
  This functions returns the client id of the client with the maximum PBR

Dependencies
  None

Dependencies
  None

Side Effects
  None

============================================================================*/
  trm_client_enum_t trm_get_max_pbr_client(void);

/*============================================================================

FUNCTION TRM_ASSIGN_RESOURCES

DESCRIPTION
  Assigned request clients into RF groups (clients which can/will be assigned
  to the same RF resource).
  For any request_and_notify clients which can be granted, assign the clients
  to the appropriate RF chain. 
  
DEPENDENCIES
  Must be called from an INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

  void trm_assign_resources
  (
  /* The current time, for "now"-based time calculations */
  trm_timestamp_t           now
  );

/*============================================================================

FUNCTION TRM_GRANT_PENDING

DESCRIPTION
  Inform any clients which have been granted an RF chain of the event.
  
DEPENDENCIES
  Must be called from an INTLOCK'd/TASKLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  Clients' task signals may be set, causing a task switch on exit.

============================================================================*/
  void trm_grant_pending();

/*============================================================================
CALLBACK TRM_ASYNC_RESEPONSE_TYPE

DESCRIPTION
  Checks whether a client is a part of the PAM accept list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  boolean trm_check_pam_accept_list( trm_client_enum_t client );

/*============================================================================

FUNCTION TRM_TASK_HANDLER

DESCRIPTION
  Initialize the Transceiver Resource Manager
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_task_handler( void );

/*============================================================================

FUNCTION TRM_WRAPUP

DESCRIPTION
  Disable RF chains that are no longer in use.
  Inform any clients which have been granted an RF chain of the event.
  
DEPENDENCIES
  Must be called from an INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Clients' task signals may be set, causing a task switch on exit.

============================================================================*/
  void trm_wrapup(
  /* client id */
  trm_client_enum_t client_id,

  /* The current time, for "now"-based time calculations */
  trm_timestamp_t           now
  );

/*============================================================================

Member Function TRM_UPDATE_PBR

Description
  This function is called at the end of the scheduler. This has two major 
 functionality:

 a) Increment the PBR count for all the clients that lost with valid 
    PBR registered mode.

 b) Update the PBR priority for all the clients that won the chain with a 
    PBR registered mode.

 c) Update PBR priority for all clients that lost with PBR registered mode
    and against a client with PBR registered mode.

 d) Modify the PBR count if the PBR state changes: PBR lose to PBR win or 
    vice versa.

Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
  void trm_reset_redundant_pbr_increment(
  trm_client_enum_t id,  /* Client Id */
  trm_client_lock_state_enum desired_lock_state /* New desired lock state */
  );

/*============================================================================

FUNCTION TRM_ASSIGN_GROUPS

DESCRIPTION
  Assign all request clients into "RF chain" groups.

  Clients in the same group would be assigned to the same RF chain, though
  obviously at different times.
  
DEPENDENCIES
  Must be called from an INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_assign_groups
  (
  /* The current time, for "now"-based time calculations */
  trm_timestamp_t           now
  );

/*============================================================================

FUNCTION TRM_ASSIGN_CHAINS

DESCRIPTION
  After client request & lock structures have been updated, loop over any
  "request" and "request & notify" clients; assign those that can be granted.
  State will be changed to "TRM_GRANTING".
  
DEPENDENCIES
  Must be called from an INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_assign_chains
  (
  /* The current time, for "now"-based time calculations */
  trm_timestamp_t           now
  );

/*============================================================================

Member Function TRM_UPDATE_PBR

Description
  This function is called at the end of the scheduler. This has two major 
 functionality:

 a) Increment the PBR count for all the clients that lost with valid 
    PBR registered mode.

 b) Update the PBR priority for all the clients that won the chain with a 
    PBR registered mode.

 c) Update PBR priority for all clients that lost with PBR registered mode
    and against a client with PBR registered mode.

 d) Modify the PBR count if the PBR state changes: PBR lose to PBR win or 
    vice versa.

Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
  void trm_update_pbr(
  /* The current time, for "now"-based time calculations */
  trm_timestamp_t           now
  );

/*============================================================================

FUNCTION TRM_GET_PREFERRED_CHAIN_MAPPING

DESCRIPTION
  Specifies the preferred chain that supports the given band.
  
DEPENDENCIES
  None

RETURN VALUE
  chain that supports the given band
  TRM_CHAIN_NONE in case no valid device

SIDE EFFECTS
  None

============================================================================*/
  trm_chain_enum trm_get_preferred_chain_mapping(
  /* Input information for the API */
  trm_get_device_mapping_input_type input
  );

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*============================================================================

FUNCTION TRM_SWITCH_REASON_INV

DESCRIPTION
  Changes the reason to the corresponding INV reason.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_switch_reason_inv(trm_reason_enum_t  *reason);

/*============================================================================
FUNCTION TRM_GET_RF_DEVICE_FROM_GRANT_EVENT

DESCRIPTION
  Converts the trm_grant_event_enum_t to rfm_device_enum_type

DEPENDENCIES
  None

RETURN VALUE
  rfm_device_enum_type

SIDE EFFECTS
  None
============================================================================*/

  rfm_device_enum_type trm_get_rf_device_from_grant_event
  (
  trm_grant_event_enum_t chain
  );

/*============================================================================

FUNCTION TRM_HANDLE_EXTEND_DURATION

DESCRIPTION
  Attempts to extend the duration an RF resource lock is held for.
  If the entire extension can be granted, it will be granted.
  If the entire extension cannot be granted, the lock duration remains
  unchanged, and the client should release the lock at the original lock
  expiry point.

  If the client had originally locked the resource for longer than the
  required extension, the lock will remain the original length
  and the extension will be INDICATED AS BEING GRANTED.

  The extension is all or nothing.  If a partial extension is desired, the
  trm_change_duration( ) function should be used.
  
DEPENDENCIES
  The client must be holding an RF resource lock

RETURN VALUE
  TRUE if the lock duration extends from "now" to "now + maximum".
  FALSE if the lock duration is unchanged.

SIDE EFFECTS
  None

============================================================================*/

boolean trm_handle_extend_duration
(
  /* The client which is attempting to extend the lock duration */
  trm_client_enum_t               client_id,

  /* Modify duration info */
  trm_modify_duration_info*       modify_duration_info
);

/*============================================================================

FUNCTION TRM_HANDLE_CHANGE_REASON

DESCRIPTION
  When a client changes what it is doing, it should change the advertised
  reason for holding the RF resource, so its priority will change.

  Eg) A client request the RF resource for listening for a PAGE.  If it
  receives one, it would change its priority to PAGE_RESPONSE and attempt
  to respond to the page, and eventually to TRAFFIC.

  There is a possibility that the reason cannot be changed for cases such as
  the new reason is incompatible with the other chain owner. In this case,
  the change reason request is denied. 

  In case of a change reason request being denied, the client still has the 
  lock on the chain but with its original reason.
  
DEPENDENCIES
  The client must be holding an RF resource lock

RETURN VALUE
  TRM_CHANGE_ACCEPTED - the reason is changed. The client can still get
  an unlock callback if it has registered one with TRM.
  TRM_CHANGE_DENIED - the reason could not be changed.

SIDE EFFECTS
  None

============================================================================*/
  trm_change_result_enum_t trm_handle_change_reason
  (
  /* The client whose priority is to be changed */
  trm_client_enum_t               client_id,

  /* The new resource why the RF lock is held (used for priority decisions) */
  trm_reason_enum_t               reason
  );

/*============================================================================

FUNCTION TRM_COMMIT_ASYNC_INFORMATION

DESCRIPTION
  This API allows new dual receive information to be saved in the TRM structure.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_commit_async_information(void);



/*============================================================================

FUNCTION TRM_IS_QTA_ACTIVE

DESCRIPTION
  Checks if there is an active QTA session in place.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: QTA Session Active.
  FALSE: QTA Session Not Active.

SIDE EFFECTS
  None

============================================================================*/
boolean trm_is_qta_active(void);

/*============================================================================

Function TRM_IS_GROUP_ALLOWED

Description
  Sets the first and second preferred chains according to VCO coupling.

Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
  boolean trm_is_group_allowed
  (
  trm_group group,
  trm_reason_enum_t reason,
  trm_client_enum_t client
  );

/*============================================================================

FUNCTION TRM_HANDLE_DUAL_RECEIVE_STATE_CHANGE

DESCRIPTION
  Handles the dual receive state change and enforces the necessary restrictions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  trm_grant_return_enum_type trm_handle_dual_receive_state_change
  (
  trm_client_enum_t          client_id,
  trm_dual_receive_enum_type dr_vote
  );

/*============================================================================

FUNCTION TRM_UPDATE_FEATURE_INFO

DESCRIPTION
  Update feature api, called from each request/release call
  this will impact the reason based feature and update the
  active feature mask accordingly
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_update_feature_info
  (
  trm_client_enum_t client_id
  );

/*============================================================================

FUNCTION TRM_VALIDATE_EXTEND_DURATION 
 
DESCRIPTION
  Checks whether extend duration grant is valid. In case of QTA gaps, all tech
  not involved in QTA are denied groups/chains, so these techs even though may
  have higher priority wont be considered when exted duration logic is run. So
  techs involved in QTA can get extended even though there is high priority
  tech. To avoid this, we have to validate the extension grant in QTA.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  boolean trm_validate_extend_duration( trm_client_enum_t client_id );

/*============================================================================

FUNCTION TRM_IS_IDLE_HOPPING_ENABLED

DESCRIPTION
  This is an internal API that can be used to check if idle hopping is
  enabled

DEPENDENCIES
  None

RETURN VALUE
  TRUE if idle hopping is enabled
  FALSE if idle hopping is disabled

SIDE EFFECTS
  None

============================================================================*/
  boolean trm_is_idle_hopping_enabled();

/*============================================================================

FUNCTION TRM_IS_NV_REFRESH_NEEDED

DESCRIPTION
  Returns if NV refresh is needed 
  
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE: if NV refresh is needed after callback from MCFG
  FALSE: If NV refresh is not needed

SIDE EFFECTS
  None

============================================================================*/
  boolean trm_is_nv_refresh_needed( void );

/*============================================================================

FUNCTION TRM_DE_INITIALIZE

DESCRIPTION
  De-initializes the memory allocated.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
  void trm_de_initialize(void);

/*============================================================================

FUNCTION TRM_IS_PBR_VARIANCE_REASON

DESCRIPTION
  This function is used to determine if PBR Variance is to be applied on
returning back to Paging.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: PBR Variance Reason
  FALSE: Not a PBR Variance Reason

SIDE EFFECTS
  None

============================================================================*/
boolean trm_is_pbr_variance_reason(trm_reason_enum_t reason);

/*============================================================================

FUNCTION TRM_UPDATE_HOPPING_BEHAVIOR

DESCRIPTION
  This will enable/disable the hopping behavior
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
boolean trm_modify_hopping_behavior
( 
   const trm_modify_hop_behavior_data*  input 
);

/*============================================================================

FUNCTION TRM_CHECK_WLAN_IRAT_CONFLICT

DESCRIPTION
  This api checks if there is a conflict with scheduled IRAT activity. If 
  there is then, WLAN is asked to release the chain
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_check_wlan_irat_conflict( trm_client_enum_t );

/*============================================================================

Function TRM_CHECK_PBR_CONFLICT

Description
  This function checks for the PBR paticipation of the passed clients.
  if both clients passed are registered for PBR, the losing client's
  pbr_conflict field is set so that it's PBR info is updated later in
  pbr_update api
		
Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
void trm_check_pbr_conflict
(
   trm_client_enum_t  client_id
);

/*============================================================================

Function TRM_UPDATE_NEW_MODE

Description
  This functions updates new mode to ASDiv module
		
Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
void trm_asdiv_update_new_mode( uint8 mode,
                                trm_slte_bitmask dr_mask );

/*============================================================================

FUNCTION TRM_REQUEST_AND_NOTIFY_HANDLING

DESCRIPTION
  Specifies the given client needs the given transceiver resource, for the
  given duration, for the supplied reason.

  When the resource can be granted to the client, the event callback for the
  client will be called with the result of the lock request.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the client currently holds an RF chain, the chain is released.

============================================================================*/
void trm_request_and_notify_handling
(
  /* Input Information for Request and Notify */
  trm_request_notify_input_info *input,
  trm_grant_callback_type        grant_callback
);

/*============================================================================

FUNCTION TRM_CHANGE_PRIORITY

DESCRIPTION
  When a client changes what it is doing, it should change the advertised
  reason for holding the transceiver resource, so its priority will change.

  Eg) A client request the transceiver resource for listening for a PAGE.  If
  it receives one, it would change its priority to PAGE_RESPONSE and attempt
  to respond to the page, and eventually change its priority to TRAFFIC.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_change_priority
(
/* The client whose priority is to be changed */
trm_client_enum_t               client,

/* The new reason why the RF lock is held (used for priority decisions) */
trm_reason_enum_t               reason
);

/*============================================================================

FUNCTION TRM_IS_WINNING_CLIENT_EXCEPTION

DESCRIPTION
  Checks if the winning and losing clients are a part of the winning
  exceptions list (trm_winning_exceptions). 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE is found in the list.
  FALSE if not found.

SIDE EFFECTS
  None

============================================================================*/
boolean trm_is_winning_client_exception
(
  trm_client_enum_t winning_client,
  trm_client_enum_t losing_client
);


/*============================================================================

Member Function TRM_IS_PBR_INCREMENT_REASON

Description
  Checks whether the given reason is a PBR valid reason or not.

Dependencies
  None

Return Value
  TRUE: If the winning client's reason is a PBR Increment Reason
  FALSE: If the winning client's reason is not a PBR Increment Reason.

Side Effects
  None

============================================================================*/
boolean trm_is_pbr_increment_reason(trm_reason_enum_t reason);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRMI_H */

