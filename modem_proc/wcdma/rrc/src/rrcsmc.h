#ifndef RRCSMC_H
#define RRCSMC_H
/*===========================================================================
                  R R C  SECURITY MODE COMMAND

DESCRIPTION
  This header file contains function prototypes for Security Mode control
  procedure

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsmc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   ad      Made changes to log important parms from OTAs
05/09/16   sp      Made changes for Mav debug snapshot
11/04/15   ad      Made changes not to apply the pending cs keys on wtog handover
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
09/03/14   sp      Made changes to save procedure substates in crash debug info
06/19/14   sg      Made changes to revert the cipher config db if cipher config type is
                   updated to step1 but later message validation failed.
11/07/13   as      Made changes to indicate security status ind as part of CU for Rel10 spe CR
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/30/13   as      Made chagnes to pass rb-id to get smc complete status function
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
10/22/12   pm      Made changes to initialize the LSB 12 bits of COUNTI when SMC is received 
                   without new keys being received or new start value which is not yet applied
12/13/11   sks     Made changes to support LTOW SRVCC.
09/28/11   ad      Added code changes for SMC to wait for till RRC comes back 
                   to active state if L2 ack for SMC is received when RRC is in 
                   STANDBY mode.
08/26/11   ad      Made changes to initialise the nas_sync_ind flag
                   while closing the signalling connection
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
06/27/11   kp      Added code to track nas_sync_ind so that DDT messages acn be allowed 
                   if we have sent atleast one nas_sync_ind to that CN doamin.
04/18/11   kp      Added code to save mapped ck/ik for L2W PSHO no cipher case.
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/08/11   rl      Fixed the compilation warnings
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/02/10   su      Provided fix to process TFCC message while another 
                   Reconfiguration is on going.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/07/10   ss      Added code to get and apply latest start_value_to_transmit that still 
                   has not been udpated to security database.06/07/10   ss      Added code to reset SVTHHO after using in
06/07/10   ss      Added code to reset SVTHHO after using in
                   rrcsmc_extract_latest_start_list_per_cn_domain
05/07/10   ss      Fixed compiler warnings.
04/26/10   ss      Added rrcsmc_is_rlc_re_est_for_srns_active for Fast Dormancy feature 
01/27/10   ss      Fixed compilation errors.
01/22/10   ss      Made changes in Start-Value calculation for CU in DCH to FACH with RAB est
01/22/10   ss      Added code to take backup of MAX HFN for CS RB's before HHO Step-1 
                   and later use that backup to calculate START_VALUE_TO_TRANSMIT for CS domain.
01/11/10   ss      Bug fix to set START = 0 in IDT if new keys are received for that domain.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
05/29/09   kp      Added support for new SNOW3g security Algo & single cipher algo usage across 
                   access stratum layers.
05/08/09   ss      Updated Copyright Information
03/16/09   ss      Added support to use START value of 0x00 in IDT message if we have new key
                   generated for a cn doamin.
03/05/09   kp      Added security related SRNS relocation support for R7 DCCH messages 
                   URA update cnf/UMI/RB reconfig based on ASN.1-7.9 version. 
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
09/15/08   ns      Made changes to calculate the activation time using the CFN of the
                   new cell, and thereby ensure that the activation time sent is at least 200
                   frames ahead of CFN. 
06/05/08   ns      Added code to store the maximum of Count C values of RBs to be released.
                   This max value will be used later to calculate START
05/16/08   ns      Added code to stop DL AM/UM channels during SRNS relocation
04/21/08   ns      Fixed klockwork errors
09/11/07   kp      Renamed function & local variable names to give appropriate 
                   meaning, needed for supporting RLC STOP_REQ at SRNS.
07/23/07   kp      Added support for spec cr 2567-Handling of security key set 
                   generated in other RAT at G to W handover.Added new structure
                   rrcsmc_security_keyset_generated_in_grat_type & some function prototype
                   Code changes are under feature flag 
                   FEATURE.SPEC.CR.2567.GTOW.SECURITY.KEY.HANDLING.
03/16/07   kp      Added proto type for function rrcsmc_get_security_start_status()
                   Which will return status of whether LATEST_CONFIGURED_CN_DOMAIN
                   is initialized or not.   
03/12/07   kp      Added New NV variable nv_ciphering_enabled_status which will keep 
                   track of exact status of Ciphering NV item.
02/09/07   kp      Removed extra hop of command interface between SMC & LLC, 
                   for resuming uplink RB’s & updating uplink ciphering keys. 
                   Now SMC directly places these request to L2 task & L1 respectively 
                   bypassing LLC.
01/12/07   sm/vr   Implementation of Spec CR 2284R1 and added support for HHO with
                   AMR rb-setup message 
01/04/07   kp      Added support for differing update of UL Ciphering keys to L1
                   till L2 Ack for SMC complete is received.
11/30/06   kp      Changed RLC Uplink Activation Sequence Number offset for
                   SRB-2 to 4.
11/23/06   kp      SMC suspend request & confirmation will be through call back 
                   function with RLC Adding the required support & also 
                   cleaning up code for command interface. 
09/11/06   vr      Added support for RLC re-establishment due to RLC Size change
09/11/06   kp      Added support to fill start list per active cn-domain which
                   will be used only by cell update procedure.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/05/06   vr      Added macros RLC_CIPH_SRB_2_ACT_SN_OFFSET and 
                   RLC_NUM_PDUS_FOR_SMCOMPLETE to support SRB2 suspension
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
03/10/06   sm      Added function rrcsmc_revert_back_to_old_fresh_for_srns_relocation to
                   revert to old FRESH(in case of SRNS relocation scenario) when UE is
                   supposed to revert back to old configuration due to physical-Ch-Est-Failure.
02/23/06   sm      Added support for spec compliant processing of non-integrity
                   protected DDT message.
02/07/06   vr      Support for FEATURE_LARGE_CIPH_ACT_SN_FOR_SRB2. 
10/07/05   sm      Added variables to support third integrity configuration on DL
09/26/05   ss      added the macro MAC_D_HFN_LENGTH and
                   also for invalid value RRCSMC_INVALID_HFN_VALUE
09/12/05   tkk     Added support to new macros for IP check for Rel-5.
07/02/05   sm      Added function declaration for rrcsmc_is_integrity_prot_mode_info_present()
                   and added MACROs used in this function.
06/13/05   vr      Added a new enum type rrcsmc_suspend_cmd_type to be passed to
                   the function send_suspend_req()
05/03/05   sm      Removed 'RRCSMC_WAIT_FOR_CU_CNF' from rrcsmc_substate_e_type,
                   added function declaration to revert back to old configuration
04/20/05   sm      Added structure 'rrcsmc_security_variables_backup' to take
                   back up of global security variables and added prototype of 
                   functions 'rrcsmc_revert_to_old_cipher_config_db()' &
                   'rrcsmc_backup_cipher_config_db()'
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
02/17/05   sm      Added macros DL_DCCH_R3_INDICATOR_POS, and NUM_OF_BITS_FOR_DL_DCCH_R3_INDICATOR
                   to be used to validate SRNS relocation triggered by config PDU.
12/24/04   sm      Added prototype of funtion rrcsmc_get_latest_configured_cn_domain()
                   which returns the latest_configured_cn_domain received in SMC
12/10/04   sm      Added macro to determine activation time expiry. This macro 
                   covers all corner cases related to activation time expiry.
10/19/04   kc      Removed unused flags in count_i_info_type structure.
08/19/04   sgk     Removed RRC_MAX_HFN_LENGTH as it is already defined in 
                   rrcmmif.h
08/09/04   kc      Provided API for other procs to to check if TM ciphering is active.
06/28/04   kc      Constant and Macro defn. to support if RRC-SN received is > by half
                   the modulus value of SN to determine if act time is reached.
06/01/04   kc      Fn. declaration for API to indicate whether SMC is active and
                   in the middle of a security reconfiguration.
02/18/04   kc      Added vars to save last received FRESH value for SRNS relocation.
                   Added bool to check and see if IK/CK was used during an RRC-conn.
01/26/04   kc      Fix to send MAC Ciphering configuration only if a TM bearer
                   config has changed.
12/16/03   kc      Fixed merging issue for ciphering act sn constant from +4 to +2
11/11/03   kc      Added Security support for SRNS relocation.
10/21/03   kc      Changed the ciphering act sn constant from +4 to +2
10/14/03   kc      Added append_start_type enum in append_start_list and 
                   append_start_per_domain
08/20/03   kc      Added const defn. for vars to peek into DL msgs to determine if
                   IPModeInfo is present in non-SMC messages.
08/11/03   kc      Added interface to send latest_transmitted_start_val to RLC
                   to support RLC-Re-establishment.
07/21/03   kc      Fix for CR30817, added interfaces to support commiting of
                   ciphering info to RLC/MAC/L1 before receiving L2_ACK for 
                   security mode command
07/16/03   kc      Fix for CR32146.  Integrity fix for Multi-Rab issue to commit
                   START values only when SMC command is received.
07/11/03   kc      Provided interface to DT procedure to inform SMC when Sig
                   Conn. is released for a particular domain.
06/19/03   kc      Also changed RLC Ciphering activation time to add +0
                   RLC provides vt(s) which is already + 1.
06/13/03   kc      Changed RLC Ciph Activation times for RB_2 to +4.
06/04/03   kc      append_start_list interface changed to add procedure_id. Also
                   changed RLC ciphering activation time to +1 from +4
05/30/03   rj      Fixed lint errors and updated copy right.
05/01/03   kc      Provide interface rrcsmc_step_2_ho_ciph_req to support 
                   Step_2 ciphering configuration of GtoW Interrat Handover
04/04/03   kc      Provide interface to add START value for a single domain
                   to fix CR1282
02/03/03   kc      Removed lint errors
10/23/02   kc      Update Copy-right info and did beautification of code.
10/28/02   rj      Added prototypes for Integrity Protection and Ciphering
                   NV items.
10/23/02   kc      Corrected UEA_0 and UEA_1 Encryption algo capability 
                   definitions
09/30/02   kc      Added more interfaces to support PS data and generic 
                   interfaces between SMC and Reconfig procedures for 
                   ciphering when SRNS relocation is supported.
09/27/02   kc      Added interfaces to query if ciphering is on and to update
                   ciphering activation time for TM radio bearers.
09/04/02   kc      Added fn. interfaces to support ciphering
08/08/02   kc      Added fn. to support SIM_INSERT_REQ and SIM_UPDATE_REQ
08/02/02   kc      Added fn. to read NV items INTEGRITY_ENABLED and 
                   CIPHERING_ENABLED
06/17/02   kc      Added code to support Integrity Modify in Security Mode 
                   Command.
04/22/02   kc      Added interface to reset Integrity STARTED status.
04/10/02   kc      More changes to Integrity Protection Interface
04/10/02   kc      Added constant definitions for START, FRESH, MAC bit and
                   byte length to this file.
04/09/02   kc      More interface changes for Integrity Protection.
03/29/02   rj      Added enum type rrcsmc_ip_status_e_type  and modified
                   return value for function rrcsmc_check_integrity_status
                   to rrcsmc_ip_status_e_type.
03/29/02   kc      Added interface to return Integrity Protection 
                   Started/NotStarted status
03/28/02   kc      Added interface functions append START_LIST, add UL act.
                   info, check DL Integrity, append UL Integrity Check Info
                   and to update Integrity Keys during Connection Setup/
                   Connection Release and when RRC_CIPHER_REQ is rcvd.
02/28/02   kc      Updated code to send a security mode command failure if an
                   invalid message is received. Added wait for l2_ack state.
10/17/01   kc      Initial check in for Security Mode Command procedure.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "rrcasn1.h"
#include "rrcllc.h"
#include "rrcllci.h"
#include "rrcllcoc.h"
#include "rrcintif.h"
#include "rlcrrcif_v.h"
#include "rrcdt.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAC_D_HFN_LENGTH 24
#define RRCSMC_INVALID_HFN_VALUE  0xffffffff /* invalid hfn value */

#define RRC_MAX_SRB                  5
#define RRC_START_VALUE_LENGTH       20    /* Length of the START Value in 
                                              bits */
#define RRC_MAX_START_LENGTH         3     /* Length of the START Value in 
                                              bytes */
#define RRC_MAX_MAC_LENGTH           4     /* Length in bytes of MAC */

#define RRC_FRESH_LENGTH_IN_BITS     32    /* Len. of FRESH in bits*/
#define RRC_MAC_LENGTH_IN_BITS       32    /* Len. of MAC in bits*/
#define RRC_SEQ_NUM_LENGTH_IN_BITS   4     /* Len. of SEQ_NUM in bits*/


#define KASUMI_SUPPORTED         0x0002    /* Const. Define for Kasumi algo.*/
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
#define SNOW_3G_SUPPORTED          0x0004    /* Const. dfn. for UEA 2*/
#endif


#define UEA_0_SUPPORTED          0x0001    /* Const. dfn. for UEA 0*/
#define UEA_1_SUPPORTED          0x0002    /* Const. dfn. for UEA 1*/
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
#define UEA_2_SUPPORTED          0x0004    /* Const. dfn. for UEA 2*/
#endif

#define RRC_HFN_BIT_MASK         0xFFFFFFF0
#define RRC_MSB_20BIT_HFN_BIT_MASK         0xFFFFF000
#define RRC_SEQ_NUM_BIT_MASK     0xF
#define RRC_UL_ACT_TIME_OFFSET   0x4
#define RRC_MAX_UM_AM_SRB        0x4
#define RLC_CIPH_ACT_SN          0x0
#define RRCSMC_SUSPEND_UM_AM_0   0x0
#define RRCSMC_MAX_KEYS_TO_L1    0x4
#define MAX_CIPH_KEYS_PER_DOMAIN 0x2

#define RRC_RLC_AM_HFN_OFFSET    12
#define RRC_RLC_UM_HFN_OFFSET    7
#define RRC_MAC_HFN_OFFSET       8

#define RRC_RLC_UM_START_OFFSET    5
#define RRC_RLC_TM_START_OFFSET    4

#ifdef FEATURE_LARGE_CIPH_ACT_SN_FOR_SRB2
  /* This feature was added as a workaround for a network issue.Since we always move to the 
     new cipher config immediately after SecurityModeComplete PDUs(spec recommended),any UL 
	 messages(Meas reports) can go out immediately after SecurityModeComplete on SRB2, since it 
	 is not suspended and we dont need to wait for the L2 Ack for SecurityModeComplete.
	 However in one of the networks it was noticed that the network was not able to handle 
	 these PDUs as it was in the middle of reconfiguring the lower layers with new ciphering 
	 config.This resulted in call drops due to network side RRC-RLC ciphering issues.
	 If this feature is turned ON,instead of starting the ciphering immediately after 
	 SecurityModeComplete,we would give an additional 2 PDU buffer which is 2 tti delay  
	 This would give network the time to finish configuring the lower layers.
	 It is NOT recommended to turn ON this feature.
  */
  #define RLC_CIPH_ACT_SN_RB_2 0x4
#else
  #define RLC_CIPH_ACT_SN_RB_2 0x2
#endif

/* 0x2C = 44. [1(integrity check info bit indicator)+36 (integrity check info)+
              5(msg type)+1(r3 indicator)+1(non-critical ext in r3 IE)] */
#define DCCH_R3_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS 44
                                          /* Bit position for integrity 
                                             protection mode info present in msgs*/
#define NUM_OF_BITS_FOR_DCCH_R3_INTEGRITY_PROTECTION_MODE_INFO_PRESENT 1
                                          /* Number of bits for integrity
                                             prot bit-mask                */

#define DL_DCCH_R3_INDICATOR_POS 42 /* 0x2A = 42. */
                              /* Bit position to indicate whether 
                              message contains 'r3' IE or 'later-than-r3' IE */
#define NUM_OF_BITS_FOR_DL_DCCH_R3_INDICATOR 1

#define DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_R5_POS 45

#define DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_R7_POS 46

#define NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_R5 1

/* 45. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)
       ] */
#define DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_POS 45

#define NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN 1

/* 46. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)
       ] */
#define DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_R5_POS 46

#define NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_R5 1

/* 49. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '0' indicates Critical extensions for r5)+
         2(mask bits for r5_add_ext_presen, and r5_nonCriticalExtensions_present)
       ] */
#define DL_DCCH_R5_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS 49

#define NUM_OF_BITS_FOR_DL_DCCH_R5_INTEGRITY_PROTECTION_MODE_INFO_PRESENT 1

/* 47. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)
       ] */
#define DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R6_POS 47

#define NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R6 1
/* 50. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '0' indicates Critical extensions for r6)+
         2(mask bits for r6_add_ext_presen, and r6_nonCriticalExtensions_present)
       ] */
#define DL_DCCH_R6_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS 50

#define NUM_OF_BITS_FOR_DL_DCCH_R6_INTEGRITY_PROTECTION_MODE_INFO_PRESENT 1

/* 48. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)
         1(value '1' indicates Critical extensions)
       ] */
#define DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R7_POS 48

#define NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R7 1

/* 49. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)
         1(value '1' indicates Critical extensions)
         1(value '1' indicates Critical extensions)
       ] */
#define DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R8_POS 49

#define NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R8 1


#ifdef FEATURE_WCDMA_REL9


/* 50. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)
         1(value '1' indicates Critical extensions)
         1(value '1' indicates Critical extensions)
         1(value '1' indicates later-than-r8 indicator)
       ] */
#define DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_LATER_THAN_R8_CRIT_EXTN_R9_POS 50

#define NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_LATER_THAN_R8_CRIT_EXTN_R9 1

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */



/* 50. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '0' indicates Critical extensions for r7)+
         1(mask bits for r7_nonCriticalExtensions_present)
       ] */
#define DL_DCCH_R7_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS 50

#define NUM_OF_BITS_FOR_DL_DCCH_R7_INTEGRITY_PROTECTION_MODE_INFO_PRESENT 1


/* 52. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions for r8)+
         1(mask bits for r8_nonCriticalExtensions_present)
         1(mask bits for r8_nonCriticalExtensions_present)
       ] */
#define DL_DCCH_R8_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS 52

#define NUM_OF_BITS_FOR_DL_DCCH_R8_INTEGRITY_PROTECTION_MODE_INFO_PRESENT 1

#ifdef FEATURE_WCDMA_REL9

/* 53. [ 1(integrity check info bit indicator)+
        36(integrity check info)+
         5(msg type)+
         1(value '1' indicates later-than-r3 indicator)+
         2(transaction id)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates Critical extensions)+
         1(value '1' indicates later r8 Critical extensions )+
         1(value '1' indicates Critical extensions for r9)+
         1(mask bits for r9_add Extensions_present)
         1(mask bits for r9_nonCriticalExtensions_present)
       ] */
#define DL_DCCH_R9_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS 53

#define NUM_OF_BITS_FOR_DL_DCCH_R9_INTEGRITY_PROTECTION_MODE_INFO_PRESENT 1

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */



/* R3 indicator bit position without integrity check info:
   [ 1: integrity check info bit indicator
     5: msg type
   ] */
#define DL_DCCH_R3_INDICATOR_WITHOUT_INT_CHECK_INFO_POS  6

/* IE 'CN Domain' position in R3 DDT message:
   [ 1: integrity check info bit indicator
     5: msg type
     1: r3 indicator bit
     1: non-critical extenstion in r3 branch
     2: transaction identifier
   ] */
#define DL_DCCH_R3_DDT_CN_DOMAIN_IE_POS 10

#define NUM_OF_BITS_FOR_DL_DCCH_R3_DDT_CN_DOMAIN_IE 1

#define RRC_SN_MOD_VAL           16

/* S: Stored RRC-SN, A: Activation SN, R: received RRC-SN
   If this macro returns true then it means that activation time has expired.
 */
#define RRC_CHECK_FOR_IP_ACT_TIME_EXPIRY(S,A,R)  (((16+(A)-(S)) & 0xF) <= ((16+(R)-(S)) & 0xF))

/* Number of RLC PDUs for SecurityModeComplete message */
#define RLC_NUM_PDUS_FOR_SMCOMPLETE 2

/* Ciph Act SN for SRB2 : 2 PDUs for SMComplete + 2 PDUs for Measurement report */
#define RLC_CIPH_SRB_2_ACT_SN_OFFSET 4


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/*==========================================================================
  Type for defining the substate of RRC SMC Procedure.
===========================================================================*/

/*Enum for different states of SMC procedure*/
typedef enum
{
  RRCSMC_INITIAL,                 /* Initial Substate */
  RRCSMC_WAIT_FOR_L2_ACK,         /* Wait for L2_ack state */
  RRCSMC_WAIT_FOR_RECONFIG_CNF,   /* Wait for any Reconfig to be completed */
  RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF,/* Wait for LLC to configure lower layers
                                     with new ciphering information */
  RRCSMC_WAIT_FOR_HO_STEP_1_CIPH_CONFIG_CNF,
  RRCSMC_WAIT_RE_EST_SRNS_CNF,
  RRCSMC_WAIT_SRNS_RELOC_L2_ACK_IND
  ,RRCSMC_WAIT_TILL_STANDBY_ENDS
}rrcsmc_substate_e_type;

/*SMC Success or failure type*/
typedef enum
{
  RRCSMC_SUCCESS,               /* Successful Status */
  RRCSMC_FAILURE                /* Failure status */
}rrcsmc_status_e_type;

/*ENUM to indicate to external procedures about the type of failure
  for Config received in Integrity_Protection_Info or 
  Cipher_Mode_Info*/
typedef enum
{
  RRCSMC_VALID_CONFIG,
  RRCSMC_UNSUPPORTED_CONFIG,
  RRCSMC_PHYSICAL_CHANNEL_FAILURE,
  RRCSMC_INCOMPATIBLE_SIMULTANEOUS_RECONFIG,
  RRCSMC_PROTOCOL_ERROR,
  RRCSMC_CELL_UPDATE_OCCURED,
  RRCSMC_INVALID_CONFIGURATION,
  RRCSMC_INCOMPLETE_CONFIGURATION
}rrcsmc_config_status_e_type;

/*enum to indicate if SECURITY for Integrity/Ciphering has started or not*/
typedef enum
{
  RRCSMC_SECURITY_STARTED,            /* Indicates IP/CIPH started */
  RRCSMC_SECURITY_NOT_STARTED         /* Indicates IP/CIPH not started */
}rrcsmc_security_status_e_type;

/*returns status to indicate if integrity protection is started or not*/
typedef enum
{
  RRCSMC_IP_STARTED,            /* Indicates IP started */
  RRCSMC_IP_NOT_STARTED         /* Indicates IP not started */
}rrcsmc_ip_status_e_type;

/* Ciphering affected Type*/
typedef enum
{
  CIPH_FIRST_TIME,
  CIPH_NOT_STARTED,
  CIPH_STARTED,
  CIPH_RESTART
}rrcsmc_affected_rb_e_type;

/* Appending Start Type*/
typedef enum
{
  RRCSMC_EXTRACT_AND_APPEND_START,
  RRCSMC_APPEND_START
}rrcsmc_append_start_e_type;

/* Ciphering affected Type*/
typedef enum
{
  RRCSMC_UPDATE_START_LIST,
  RRCSMC_UPDATE_START_LIST_LATEST
}rrcsmc_start_list_update_e_type;

/* SRNS Relocation IP-Check Status type */
typedef enum
{
  RRCSMC_SRNS_SECURITY_INFO_INVALID,
  RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION,
  RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS,
  RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE
}rrcsmc_srns_ip_check_status_e_type;

/* Type to indicate the command that triggers the RLC suspension*/
typedef enum
{
  RRCSMC_SMC_CMD,
  RRCSMC_SRNS_CMD
}rrcsmc_suspend_cmd_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

extern boolean    nv_integrity_enabled;     
extern boolean    nv_ciphering_enabled;
extern boolean    nv_ciphering_enabled_status;
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/*global data struct which maintains ciphering information needed by
  rlc, mac and l1 layers
 */
typedef struct
{
  rlc_ul_cipher_config_type rlc_ul_cipher_info;
  rlc_dl_cipher_config_type rlc_dl_cipher_info;
  mac_cipher_config_type    mac_cipher_info[RRC_MAX_CN_DOMAINS];
  l1_ciphering_key_cmd_type l1_cipher_info;
  boolean                   mac_ciph_config_affected;
}rrcsmc_ciphering_config_info_type;

/*type to hold rb_id and sequence number*/
typedef struct
{
  uint8 rb_id;        /*rb id*/
  uint16 seq_num;      /*seq num to activate ciphering*/
} rrcsmc_rb_id_type;


/*type to save contents of security mode command message.
  Security Mode Command may not be acted upon by immediately if
  Ordered_Config is set or if Cell_Update is in progress*/
typedef struct 
{
  uint32 transaction_id;                     /*transaction id of message*/
  rrc_cn_domain_identity_e_type domain_id;   /*domain id                */


  /*ciphering related information*/
  uecomdef_wcdma_cipher_algo_e_type ciph_algo;          /*ciphering algo being used*/
  boolean dpch_ciph_act_time_present;        /*flag to indicate if dpch 
                                               act time present         */
  boolean rb_ciph_dl_act_time_present;       /*flag to indicate if dl rb
                                               act time present         */
  uint32 dpch_ciph_act_time;                 /*dl dpch act time         */
  uint8 num_rb_ciph_dl_act_time;             /*num dl rb activation time*/
  rrcsmc_rb_id_type rb_ciph_dl_act_time[UE_MAX_DL_UM_CHANNEL + 
                                        UE_MAX_AM_ENTITY];
  /*array to hold dl rb act 
    time                     */

  /*integrity related information*/
  uecomdef_wcdma_integrity_protection_algorithm_e_type integrity_algo; 
  /*integrity algo being used*/
  boolean start_chosen;                      /*start chosen             */
  boolean modify_chosen;                     /*modify chosen            */
  uint32 start;                              /*start value if start     */
  rrcsmc_rb_id_type srb_int_dl_act_time[RRC_MAX_SRB];
  /*activation time for dl 
integrity                */
} rrcsmc_security_mode_command_type;

typedef struct 
{
  boolean                  ciph_algo_present;
  uecomdef_wcdma_cipher_algo_e_type  ciph_algo;
  boolean                  dpch_act_time_present;  
  uint32                   dpch_activation_time;
  boolean                  fresh_present;
  uint32                   fresh;
  uint32                   fresh_old;
  uint32                   fresh_old1;/* stores the old FRESH value, needed for 
                                         reverting back to old configuration after
                                         PhyChEstFailure(SRNS Relocation) */
  boolean                  reconfig_cnf_req;
  rrc_rlc_re_est_for_srns_rb_e_type rb_type;  
  rrc_proc_e_type          proc_type;
  boolean                  integrity_algo_present;
  uecomdef_wcdma_integrity_protection_algorithm_e_type  integrity_algo;
} rrcsmc_srns_info;

/*struct contains info about SRNS relocation integrity and ciphering info
  passed from reconfig-procedures to SMC*/
typedef struct
{
  boolean                          integrity_info_present;
  boolean                          ciphering_mode_info_present;
  rrc_IntegrityCheckInfo           *integrity_check_info;
  rrc_IntegrityProtectionModeInfo  *integrity_protection_mode_info;
  rrc_CipheringModeInfo            *ciphering_mode_info;
} rrcsmc_srns_relocation_security_info_type;


/*Temporary structure to hold the max count_c values for released rbs */
typedef struct
{
  uint32 count_c_cs;
  uint32 count_c_ps;
}rrcsmc_rel_rbs_max_count_c;

/*struct contains info about SRNS relocation integrity and ciphering info
  passed from reconfig-procedures to SMC*/
typedef struct
{
  boolean                          integrity_info_present;
  boolean                          ciphering_mode_info_present;
  rrc_IntegrityCheckInfo           *integrity_check_info;
  rrc_IntegrityProtectionModeInfo_r7 *integrity_protection_mode_info;
  rrc_CipheringModeInfo_r7 *ciphering_mode_info;
} rrcsmc_srns_relocation_security_info_r7_type;

/*struct contains info if rlc rb's are suspended by SMC procedure, also
  contains info about which rb's are suspended*/
typedef struct
{
  boolean rlc_rb_suspended_or_stopped_flag;               /* whether suspended/stopped or not */
  rlc_ul_stop_suspend_request_type  suspend_or_stopped_info;      /* which rb's suspended */
  rlc_dl_stop_request_type dl_stop_info;
} rrcsmc_suspend_stop_rlc_type;

/*type to hold rb_id and sequence number*/
typedef struct
{
  uint8                          rb_id;
  rrcsmc_affected_rb_e_type      rb_status;
  boolean                        is_rb_srb;
  rrc_cn_domain_identity_e_type  domain_id;
} rrcsmc_um_am_config_info;

/*type to hold UL AM ciphering config info*/
typedef struct
{
  uint8                      num_rb;
  rrcsmc_um_am_config_info   config_info[UE_MAX_AM_ENTITY];
}rrcsmc_ul_am_ciph_config;

/*type to hold UL UM ciphering config info*/
typedef struct
{
  uint8                      num_rb;
  rrcsmc_um_am_config_info   config_info[UE_MAX_UL_UM_CHANNEL];
}rrcsmc_ul_um_ciph_config;

/*type to hold TM ciphering config info*/
typedef struct
{
  rrcsmc_affected_rb_e_type      rb_status;
  boolean                        affected;
  uint8                          num_rb;
}rrcsmc_tm_ciph_config;

/*type to hold GLBOBAL CIPHERING Database used by MAC/RLC/L1*/
typedef struct
{
  rrcsmc_ul_um_ciph_config    um_rb_ciph_config;
  rrcsmc_ul_am_ciph_config    am_rb_ciph_config;
  rrcsmc_tm_ciph_config       tm_rb_ciph_config[RRC_MAX_CN_DOMAINS];
}rrcsmc_rb_ciphering_config;

/*type which holds list of ciphering keys*/
typedef struct
{
  /*0 indicates cs_1 used, 1 indicates cs_2 used*/
  boolean key_used;

  /*indication that a new key is received*/  
  boolean new_key;  

  /* Indication that new key is received from previous uncompleted SMC
     * This new key will be operation after IDT carries START = 0x0,
     * (Instead of sending generated START as per spec 25.331 section 8.5.9 
     */
  boolean new_key_for_start_value; 

  /*cs and ps keys that will be used.  Index given to MAC and RLC will
    be between 0-3*/
  byte  key[MAX_CIPH_KEYS_PER_DOMAIN][RRC_MAX_CIPHERING_KEY_LENGTH];
}rrcsmc_ciph_keys_type;

typedef struct
{
  /* CN Domain Identity */
  rrc_cn_domain_identity_e_type       domain_id;

  rrcsmc_ciph_keys_type               ciph_keys_info;

  byte  integrity_key[RRC_MAX_INTEGRITY_KEY_LENGTH];
  byte  integrity_key_old[RRC_MAX_INTEGRITY_KEY_LENGTH];

  rrc_cipher_algo_e_type  cipher_algo; 
  /* Hyper Frame Number */
  byte                  hfn[RRC_MAX_HFN_LENGTH];
  byte                  hfn_latest[RRC_MAX_HFN_LENGTH];
}rrcsmc_security_config_type;

  /*This structure is used to store the security key set generated 
    for CS domain in other RAT(i.e-G-RAT).*/
typedef struct
{
  /*This field is used to indicate whether the security key set 
    in this structure are valid*/
  boolean grat_key_set_valid;
  /*used to hold Integrity key*/
  byte  integrity_key[RRC_MAX_INTEGRITY_KEY_LENGTH];
  /*used to hold Ciphering key*/
  byte  ciphering_key[RRC_MAX_CIPHERING_KEY_LENGTH];
  /*flag indicated if there is a pending cs key not activated in W*/
 boolean  pending_cs_key_to_be_activated;
}rrcsmc_security_keyset_generated_in_grat_type;

/*local SMC representation of cn_ciphering_info_type*/
typedef struct
{
  uint32                         num_cn_domains;

  /* latest and little less latest CN Domain Id */
  rrc_cn_domain_identity_e_type  latest_config_cn_id;     
  rrc_cn_domain_identity_e_type  latest_config_cn_id_old;     

  /* latest_config_cn_id_old_1 will hold the value of latest_config_cn_id_old whenever
      security mode command with "modify" is received */
  rrc_cn_domain_identity_e_type  latest_config_cn_id_old_1;     

  boolean ciphering_info_received;
  boolean integrity_info_received;

  boolean start_value_update_flag[RRC_MAX_CN_DOMAINS];

  /*whether ciphering is started or not per cn domain*/
  rrcsmc_security_status_e_type  ciph_status[RRC_MAX_CN_DOMAINS];   

  /*whether sync_ind is sent to NAS or not per cn domain*/
  boolean  is_sync_ind_sent_to_nas[RRC_MAX_CN_DOMAINS];   

  rrcsmc_security_config_type    int_ciph_info[RRC_MAX_CN_DOMAINS];

  /*save latest ciphering algorithm sent for both domains*/
  uecomdef_wcdma_cipher_algo_e_type saved_ciph_algo[RRC_MAX_CN_DOMAINS];

  /*save latest Integrity algorithm sent for both domains*/
  uecomdef_wcdma_integrity_protection_algorithm_e_type  saved_integrity_algo[RRC_MAX_CN_DOMAINS];

  boolean                        int_start_status_for_conn[RRC_MAX_CN_DOMAINS];

  /*max count c for released RBs per domain*/
  uint32 max_count_c_rel_rbs[RRC_MAX_CN_DOMAINS];

}rrcsmc_security_config_info_type;

/*struct to hold activation times for UL and DL ciphering*/
typedef struct
{
  uint8 nchan;
  uint16 sq_num[MAX_RB];
  uint8 act_time_present[MAX_RB];
  uint16 sq_num_for_complete[MAX_RB];
  uint8 lc_id_for_complete[MAX_RB];
} rrcsmc_ciph_act_type;

/*All information with regards to the current Ciphering status
  is saved in this struct*/
typedef struct
{
  boolean                            reconfig; /*reconfig true or false*/

  /*local value to store suspend_cnf information*/
  rrcsmc_ciph_act_type ul_ciph_act_info;

  /*local value to store dl act time info*/
  rrcsmc_ciph_act_type dl_ciph_act_info;

} rrcsmc_ciph_info_type;

/*struct to hold Uplink and Downlink count_i information. will add cound_c
  struct later when we support ciphering*/
typedef struct
{
  uint32  dl_count_i[RRC_MAX_SRB];
  boolean dl_count_i_init_flag[RRC_MAX_SRB];
  boolean dl_count_i_act_time_pending[RRC_MAX_SRB];
  boolean dl_new_key_flag_per_rb[RRC_MAX_SRB];

  uint32  ul_count_i[RRC_MAX_SRB];
  boolean ul_count_i_act_time_pending[RRC_MAX_SRB];
  boolean ul_new_key_flag_per_rb[RRC_MAX_SRB];

} rrcsmc_count_i_info_type;


/*All information with regards to the current Integrity Protection status
  is saved in this struct*/
typedef struct
{
  rrcsmc_security_status_e_type            ip_status;/*started or not*/

  boolean                                  reconfig; /*reconfig true/false*/

  boolean                                  modify_received;   /*true or false*/

  /*ul or dl act time if modify received*/
  uint32                                   dl_act_seq_num[RRC_MAX_SRB];
  uint32                                   ul_act_seq_num[RRC_MAX_SRB];

  /* to indicate whether latest_config_cn_id_old_1 is valid for a given SRB or not */
  boolean                                dl_latest_config_cn_id_old_1_valid[RRC_MAX_SRB];

  /* keys below will hold the integrity key for a SRB when cn_id_old_1 is valid  for that SRB */
  byte  integrity_key_for_cn_id_old_1[RRC_MAX_SRB][RRC_MAX_INTEGRITY_KEY_LENGTH];

  rrcsmc_count_i_info_type                 count_i_info[RRC_MAX_CN_DOMAINS];
  /*holds ul and dl count_i*/
} rrcsmc_integrity_protection_info_type;

/*Local representation of Integrity Check Info IE.*/
typedef struct
{
  uint32 msg_auth_code;
  uint32 msg_seq_num;
} rrcsmc_integrity_check_info_type;

/*start_list for hfn's*/
typedef struct
{
  uint32 hfn_srb;
  uint32 hfn_rab;
  uecomdef_wcdma_cipher_algo_e_type ciph_algo_srb;
  uecomdef_wcdma_cipher_algo_e_type ciph_algo_urb;
}rrcsmc_re_est_start_list;

typedef struct
{
  /*static var to hold all kinds of security config which is not calculated
    by SMC procedure,which is essentially received from other procedures or
    OTA message */
  rrcsmc_security_config_info_type  rrcsmc_int_ciph_info;
  
  /*declare the struct integrity_protection_info*/
  rrcsmc_integrity_protection_info_type integrity_protection_info;
  
  /*declare the struct integrity_protection_info*/
  rrcsmc_ciph_info_type ciphering_info;

  rrcsmc_ciphering_config_info_type cipher_config_db;

  rrcsmc_rb_ciphering_config ciph_rb_info;

  /*boolean to check is START_VALUE_TO_TRANSMIT is set or not*/
  boolean start_value_to_transmit_flag;

  /*new tm radio bearer indication flag*/
  boolean new_tm_rb_ind_flag;

} rrcsmc_security_variables_backup;
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================
FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind_r9

DESCRIPTION
  This function extracts ip check info/integrity protection mode info/ciphering mode info from SRNS 
  relocation triggering messages for Release 9 version of OTAs. In summary, input srns_info has to be filled
  from OTA in cmd_ptr.

  NOTE:  There is no R9 UTRANMobilityInformation and URAUpdateConfirm messages in ASN 9.5.
  Right now, only 5 Reconfig OTAs + CU CNF can do SRNS relocation for R9 OTA.

DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None
===========================================================================*/
uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind_r9
(
  rrcsmc_srns_relocation_security_info_r7_type * srns_info,
  rrc_cmd_type *cmd_ptr
);

#endif

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif


/*===========================================================================

FUNCTION rrcsmc_is_integrity_prot_mode_info_present

DESCRIPTION
  This function returns TRUE if IE "integrityProtectionModeInfo" is present in
  configuration message, otherwise it returns FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_is_integrity_prot_mode_info_present(uint8 *dl_sdu_ptr, uint8 msg_type);

/*===========================================================================

FUNCTION rrcsmc_check_and_abort_ongoing_security_configuration

DESCRIPTION
   This function checks whether security configuration is in progress or not. 
   If yes, then it will abort the ongoing security configuration by indicating
   LLC to revert to old configuration. This will also indicate LLC as what action
   needs to be taken to abort the security configuration.   

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if security procedure is aborted
  FLASE if secuirty procedure is not active

===========================================================================*/
boolean rrcsmc_check_and_abort_ongoing_security_configuration
(
  void
);
/*===========================================================================

FUNCTION rrcsmc_fill_crlc_dl_abort_cipher_req

DESCRIPTION
   This function fills the 'crlc_dl_abort_cipher_req' with the DL LC ids for which
   new ciphering configuraiton was given to RLC on receipt of SMC. 

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_fill_crlc_dl_abort_cipher_req
(
  rlc_abort_cipher_config_type *abort_cipher_info_ptr
);

/*===========================================================================

FUNCTION rrcsmc_fill_crlc_ul_abort_cipher_req

DESCRIPTION
   This function fills the 'crlc_ul_abort_cipher_req' with the UL LC ids for which
   new ciphering configuraiton was given to RLC on receipt of SMC. 

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_fill_crlc_ul_abort_cipher_req
(
  rlc_abort_cipher_config_type *abort_cipher_info_ptr
);

/*===========================================================================

FUNCTION rrcsmc_send_resume_req

DESCRIPTION
   This function sends resume req to LLC which resumes transmission of  all 
   RLC UM and AM radio bearers(except for RB2)

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
boolean rrcsmc_fill_ul_resume_req
(
  rlc_ul_resume_type *rrc_ul_resume_req_ptr
);

/*===========================================================================

FUNCTION  rrcsmc_revert_to_old_cipher_config_db

DESCRIPTION
  This function reverts the value of global variable 'cipher_config_db' from its back-up value.
  This function is supposed to be called whenever RRC has to revert back to old configuration
  during HHO(timing initialized) or SRNS relocation. 

  NOTE: Make sure that "cipher_config_db" value is backed-up before this function is 
        called. By default backup variable will have junk value (its value is never cleared) 

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_revert_to_old_cipher_config_db
(
  void
);

/*===========================================================================

FUNCTION  rrcsmc_backup_cipher_config_db

DESCRIPTION
  This function back-up the global variable 'cipher_config_db'. This function
  is supposed to be called immediately before updating this variable for
  STEP1 cipher config for HHO.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_backup_cipher_config_db
(
  void
);

/*===========================================================================

FUNCTION  rrcsmc_backup_global_security_variables

DESCRIPTION
  This function back-up all security related global variables. This function
  is supposed to be called before processing any config/re-config request.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_backup_global_security_variables
(
  void
);
/*===========================================================================

FUNCTION  rrcsmc_revert_to_old_security_variables

DESCRIPTION
  This function reverts the security variables to their back-up values. This
  function will also take care of 

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_revert_to_old_security_variables
(
  void
);

/*===========================================================================

FUNCTION rrcsmc_get_latest_configured_cn_domain

DESCRIPTION
  Returns the latest Configured Cn Domain value which is set based on the cnDomainIdentity IE that comes
  in last received Security Mode Command.

DEPENDENCIES
  None.

RETURN VALUE
  Cn Domain

SIDE EFFECTS
  None

===========================================================================*/
rrc_cn_domain_identity_e_type rrcsmc_get_latest_configured_cn_domain(void);

/*===========================================================================

FUNCTION rrcsmc_init_procedure

DESCRIPTION
  This function initializes required parameters for Security Mode Command
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void rrcsmc_init_procedure(void);

/*===========================================================================

FUNCTION rrcsmc_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for Security Mode Command procedure
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing SECURITY
      Mode Command message.
    RRC_CIPHER_REQ Primitive with new C-key and I-key
    RRC_STATE_CHANGE_IND Primitive informs transition from Connected mode to
      Idle Disconnected state.
    RRC_DOWNLINK_L2ACK_IND Primitive indicates success or failure of 
      transmission of Security Mode Complete Msg.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void rrcsmc_procedure_event_handler(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrcsmc_init_integrity_ciphering_config

DESCRIPTION
  This function initializes the rrcsmc_integrity_and_ciphering_info with 
  the values received from MM in the RRC_EST_REQ. Also a copy is made in 
  the rrcsmc_integrity_and_ciphering_info_old.
  RCE procedure will call this function in SMC.


DEPENDENCIES
  None.

RETURN VALUE
  Returns SUCCESS if Initialization of Integrity and Ciphering 
  Configuration is successful.
  Returns FAILURE if Initialization of Integrity and Ciphering 
  Configuration is not successful.


SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_init_integrity_ciphering_config
(
  rrc_sim_inserted_req_type  *sim_insert_req_ptr
);
/*===========================================================================

FUNCTION rrcsmc_update_integrity_ciphering_config

DESCRIPTION
  This function updates the Integrity and Ciphering Keys received from
  MM in RRC_SIM_UPDATE_REQ command

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_update_integrity_ciphering_config
(    
 rrc_sim_update_req_type *sim_update_req_ptr 
);
/*===========================================================================

FUNCTION rrcsmc_check_downlink_integrity

DESCRIPTION
  This function checks the Integrity of the DL message received and returns 
  the status.  It receives the pointer to the Command and runs Integrity Check 
  on the message.
  The dispatcher procedure will call this function.


DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if DL Integrity check was successful
  The function returns FAILURE if DL Integrity check was unsuccessful

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_check_downlink_integrity 
(
  rrc_cmd_type *cmd_ptr 
);
/*===========================================================================

FUNCTION rrcsmc_append_uplink_integrity_check_info

DESCRIPTION
  RRCSMC receives the encoded uplink SDU and adds Integrity Check Info to
  the Uplink SDU.
  The send chain will call this function in SMC before sending a 
  message on a UL Signaling Radio Bearer to append the Integrity Check
  Info in the message.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns SUCCESS if IntegrityCheckInfo is appended successfully
  This function returns FAILURE if if it is failed to append IntegrityCheckInfo.


SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_uplink_integrity_check_info 
(
  byte*  ul_sdu,
  uint32 ul_sdu_length,
  uint32 rb_id
);

/*===========================================================================

FUNCTION rrcsmc_append_start_list

DESCRIPTION
  This function initializes the START_LIST with the latest values to 
  be sent to the UTRAN by the RCE, CU  and ASU procedures.
  
  RCE, CU and ASU procedures will call this function in SMC.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns SUCCESS if STARTlist is appended.
  This function returns FAILURE if STARTlist is not appended.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_start_list 
(
  rrc_STARTList *start_list_ext,
  rrcsmc_append_start_e_type start_type
);

/*===========================================================================

FUNCTION rrcsmc_process_integrity_protection_mode_info

DESCRIPTION
  This procedure process Integrity Protection Mode Info.  It is called by 
  the foll. procedures.  
  
  UMI, Cell Update, URA Update, RBE, ASU
  
  RRCSMC will copy the IntegrityProtectionModeInfo IE.
  If IntegrityProtectionModeInfo indicates to start Integrity
  Protection, it will update the necessary paramters and start Integrity.
  Otherwise it returns invalid configuration.
    
  The above procedures will call this function in SMC to notify the SMC
  procedure of the Integrity Protection Info and to start Integrity Protection.


DEPENDENCIES
  None.

RETURN VALUE
  Returns RRCSMC_VALID_CONFIG if IntegrityProtectionModeInfo is 
  valid(Start only).
  Returns RRCSMC_INVALID_CONFIG if IntegrityProtectionModeInfo is invalid(Modify)

SIDE EFFECTS
  None

===========================================================================*/
rrcsmc_config_status_e_type rrcsmc_process_integrity_protection_mode_info
(
  struct rrc_IntegrityProtectionModeInfo* integrityProtModeInfo
);

/*===========================================================================

FUNCTION rrcsmc_append_ul_integrity_prot_act_info

DESCRIPTION
  This procedure populates the UL Activation Info times into the ptr passed
  to it by the foll. procedures.
  
  UMI, Cell Update, URA Update, RBE, ASU
  
  The above procedures will call this function in SMC to get the UL Activation 
  Info while sending a Complete message.  The UL Activation Info will be used 
  by the UTRAN to start integrity protection on the Uplink.  


DEPENDENCIES
  None.

RETURN VALUE
  Returns SUCCESS if UL Activation Info is successfully populated.
  Returns FAILURE if UL Activation Info is not populated.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_ul_integrity_prot_act_info 
(
  struct rrc_IntegrityProtActivationInfo * ulActivationInfo
);

/*===========================================================================

FUNCTION rrcsmc_append_hfns_to_rel_ind

DESCRIPTION
  This procedure appends the CS and PS domain HFNs to RRC_REL_IND message
  ptr which will be sent to MM.

DEPENDENCIES
  None.

RETURN VALUE
  Returns SUCCESS if PS and CS HFNs are successfully appended to rrc_rel_ind_type.
  Returns FAILURE if PS and CS HFNs are not appended to rrc_rel_ind_type.


SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_hfns_to_rel_ind 
(
  byte *hfncs, 
  byte *hfnps
);
/*===========================================================================

FUNCTION rrcsmc_check_integrity_status

DESCRIPTION
  This function checks if Integrity Protection is already started or not.
  Other procedures may call this function to check the Start/NotStart
  Status of Integrity Protection.

DEPENDENCIES
  None.

RETURN VALUE
  Returns RRCSMC_IP_STARTED if Integrity Protection is started.
  Returns RRCSMC_IP_NOT_STARTED if Integrity Protection is not started.


SIDE EFFECTS
  None

===========================================================================*/
rrcsmc_ip_status_e_type rrcsmc_check_integrity_status(void);

/*===========================================================================

FUNCTION rrcsmc_reset_smc_ip_started_flag

DESCRIPTION
  Resets the STATUS flag to  RRCSMC_IP_NOT_STARTED.  This function will be
  called by rrcrce procedure on entering connected mode.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_reset_smc_ip_started_flag(void);

/*===========================================================================

FUNCTION rrcsmc_reset_smc_configure_flag

DESCRIPTION
  This function is called by UMI, Cell Update, URA Update, RBE, ASU 
  procedures when they receive an L2Ack for the Complete message they
  sent out on the uplink.  This is to indicate to the SMC that
  changing the configuration of Security is complete and it should
  reset its internal flag

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_reset_smc_config_flag(void);
/*===========================================================================

FUNCTION rrcsmc_update_ciphering_config_info

DESCRIPTION
  This function is called by LLC procedure before it sends ciphering
  information to RLC, MAC and L1 layers.  When this function is called,
  SMC procedure updates the global DB for ciphering configuration with
  the latest ciphering information. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_update_ciphering_config_info
(
  rrc_ciph_config_e_type config_cause
);
/*===========================================================================

FUNCTION rrcsmc_update_ciphering_config_info

DESCRIPTION
  This function is called by LLC procedure before it sends ciphering
  information to RLC, MAC and L1 layers.  When this function is called,
  SMC procedure updates the global DB for ciphering configuration with
  the latest ciphering information. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_update_ciphering_config_info_for_reconfig
(
  rrcllc_rlc_config_req_list_type *rlc_req_type_ptr,
  rrc_proc_e_type proc_id
);
/*===========================================================================

FUNCTION rrcsmc_get_nv_items

DESCRIPTION
  This  function gets NV  item NV_RC_INTEGRITY_ENABLED  and
  NV_RRC_CIPHERING_ENABLED and stores it in local variables
  used to determine if Integrity/Ciphering are supported or
  not

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcsmc_get_nv_items
(
  sys_modem_as_id_e_type wrrc_as_id
);

/*===========================================================================
FUNCTION rrcsmc_is_ciphering_enabled

DESCRIPTION
  This function indicates if ciphering is started or not.
  
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS or FAILURE whether ciphering is started or not respectively.

SIDE EFFECTS
  None
===========================================================================*/
rrcsmc_security_status_e_type rrcsmc_is_ciphering_enabled(void);
/*===========================================================================

FUNCTION rrcsmc_copy_pending_keys_to_saved_security_key_set

DESCRIPTION
  If there is a pending key, it is updated to saved security key set
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void rrcsmc_copy_pending_keys_to_saved_security_key_set(void);
/*===========================================================================

FUNCTION rrcsmc_reset_pending_cs_key_to_be_activated

DESCRIPTION
  Resets the pending_cs_key_to_be_activated flag
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_reset_pending_cs_key_to_be_activated(void);
/*===========================================================================

FUNCTION rrcsmc_update_ciphering_activation_cfn

DESCRIPTION
  Update Ciphering Activation CFN global var.  This value is updated by
  other procedures if they receive a Ciphering Activation CFN from the
  N/W or with the current CFN which they query from L1

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_ciphering_activation_cfn( uint8 ciphering_activation_cfn );
/*===========================================================================

FUNCTION rrcsmc_get_and_reset_start_value_to_transmit

DESCRIPTION
  If a new UM or AM Radio Bearer is being added, we need to send start
  values to the n/w side.  The reconfig procedure queries this start
  value to transmit and also this flag is reset here bcos once this
  start value is sent, the flag has to be reset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_and_reset_start_value_to_transmit
(  
byte * start_value
);

/*===========================================================================

FUNCTION rrcsmc_get_calc_start_value

DESCRIPTION
  Returns the calculated start_value_to_transmit

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_calc_start_value
(
byte * start_value
);

/*===========================================================================

FUNCTION rrcsmc_get_start_value_to_transmit

DESCRIPTION
  Returns the calculated start_value_to_transmit

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rrcsmc_get_start_value_to_transmit
(
  void
);

/*===========================================================================

FUNCTION rrcsmc_get_latest_transmitted_start_values

DESCRIPTION
  This interface returns the START values that are updated latest to the
  N/W in any Uplink message.
  This function populates the start value for Signalling RB and for RAB's.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_start_list_with_latest_transmitted_start_value
(
  rrcsmc_re_est_start_list * start_list
);


/*===========================================================================

FUNCTION rrcsmc_get_start_value_to_transmit_flag_status

DESCRIPTION
  Returns the status of START_VALUE_TO_TRANSMIT_FLAG

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_start_value_to_transmit_flag_status
(
  void
);

/*===========================================================================

FUNCTION rrcsmc_get_and_reset_new_tm_rb_ind

DESCRIPTION
  If a new TM Radio Bearer is being added, then send TM RB ciphering act
  CFN to the procedure which configured the TM radio bearers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_is_new_tm_rb_setup(void);
/*===========================================================================

FUNCTION rrcsmc_append_start_per_domain

DESCRIPTION
  This function initializes the a byte ptr with START values for the domain
  requested.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_append_start_per_domain 
(
byte  *value,
rrc_cn_domain_identity_e_type domain_id,
rrcsmc_append_start_e_type start_type
);

/*===========================================================================

FUNCTION rrcsmc_step_2_ho_ciph_req

DESCRIPTION
  Process Ciph Config Cnf in Step_2_Ciph_config_cnf substate

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE

===========================================================================*/
uecomdef_status_e_type rrcsmc_step_2_ho_ciph_req
( 
  uint16 step_2_ciph_act_time
);
/*===========================================================================

FUNCTION rrcsmc_update_sync_ind_info_for_signalling_release

DESCRIPTION
  Function to update ciphering database if signalling connection release is
  received.  This cleans up the sync indication variable .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_sync_ind_info_for_signalling_release
( 
  rrc_cn_domain_identity_e_type domain_id 
);
/*===========================================================================

FUNCTION rrcsmc_update_ciphering_config_info_for_signalling_release

DESCRIPTION
  Function to update ciphering database if signalling connection release is
  received.  This cleans up the ciphering database, as RB-Release will/may
  not be released in this case.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_ciphering_config_info_for_signalling_release
( 
  rrc_cn_domain_identity_e_type domain_id 
);
/*===========================================================================

FUNCTION rrcsmc_get_smc_procedure_complete_status

DESCRIPTION
  This status is queried by UDT procedure.  If procedure is waiting for L2_ACK
  for SMC complete or for lower layer ciphering_configuration, then return
  FALSE.  UDT procedure will wait for indication from SMC when L2_ACK is rcvd.
DEPENDENCIES
  TRUE/FALSE

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_smc_procedure_complete_status
(
  rrc_cn_domain_identity_e_type cn_id, rrc_RB_Identity rb_id
);
/*===========================================================================

FUNCTION rrcsmc_get_latest_transmitted_start_values

DESCRIPTION
  This interface returns the START values that are updated latest to the
  N/W in any Uplink message.
  This function populates the start value for Signalling RB and for RAB's.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_get_latest_transmitted_start_values
(
rrcsmc_re_est_start_list * start_list
);
/*===========================================================================

FUNCTION rrcsmc_check_integrity_for_srns_relocation

DESCRIPTION
  This function checks integrity for an SRNS triggering relocation
  message.
 
DEPENDENCIES
  None.

RETURN VALUE
  rrcsmc_srns_ip_check_status_e_type

SIDE EFFECTS
  None

===========================================================================*/
rrcsmc_srns_ip_check_status_e_type rrcsmc_check_integrity_for_srns_relocation
(
  rrc_cmd_type *cmd_ptr
  ,rrc_msg_ver_enum_type msg_version
);
/*===========================================================================

FUNCTION rrcsmc_commit_fresh_for_srns_relocation

DESCRIPTION
  This function commits the FRESH value received in SRNS Relocation
  triggering message.
 
DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_commit_fresh_for_srns_relocation
(
  void
);

/*===========================================================================

FUNCTION rrcsmc_revert_back_to_old_fresh_for_srns_relocation

DESCRIPTION
  This function reverts back to old stored FRESH value which UE was using before
  receipt of SRNS relocation confiruation message(for which UE has to revert back
  to old configuration).
 
DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_revert_back_to_old_fresh_for_srns_relocation
(
  void
);


/*===========================================================================

FUNCTION rrcsmc_step_2_cipher_config_for_first_tm_rb_setup

DESCRIPTION
  Process Ciph Config Cnf for first TM RBs being setup

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
uint32 rrcsmc_step_2_cipher_config_for_first_tm_rb_setup
( 
void
);

/*===========================================================================

FUNCTION rrcsmc_process_cipher_config_for_first_tm_setup

DESCRIPTION
  Process Step 1 ciphering for First TM RB Setup

DEPENDENCIES
  None.

RETURN VALUE
  uecomdef_status_e_type SUCCESS if Ciphering is started on TM RB's
                         FAILURE if Ciphering is not started on TM RB's  

===========================================================================*/
uecomdef_status_e_type rrcsmc_process_cipher_config_for_first_tm_setup
(
   uint32 mac_d_hfn
);
/*===========================================================================

FUNCTION rrcsmc_process_step_1_ho_ciph_req

DESCRIPTION
  Process Step 1 ciphering for Handover

DEPENDENCIES
  None.

RETURN VALUE
  uecomdef_status_e_type SUCCESS if Ciphering is started on TM RB's
                         FAILURE if Ciphering is not started on TM RB's  
===========================================================================*/
uecomdef_status_e_type rrcsmc_process_cipher_config_for_hho( uint32 mac_d_hfn);
/*===========================================================================

FUNCTION rrcsmc_step_2_cipher_config_for_hho

DESCRIPTION
  Process Ciph Config Cnf for Hard Handover

DEPENDENCIES
  None.

RETURN VALUE
  Returns Count_C Activation Time

===========================================================================*/
uint32 rrcsmc_step_2_cipher_config_for_hho ( void );

/*===========================================================================

FUNCTION rrcsmc_update_start_value_to_transmit_cs_hho

DESCRIPTION
  Updates start_value_to_transmit_cs_hho

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_update_start_value_to_transmit_cs_hho
(
void
);


/*===========================================================================

FUNCTION rrcsmc_is_security_reconfig_active

DESCRIPTION
  checks to see if SMC status is in INITIAL.  

DEPENDENCIES
  None.

RETURN VALUE
  Return's TRUE is SMC status is not initial
  Return's FALSE is SMC status is initial

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_is_security_reconfig_active 
(
  void
);
/*===========================================================================

FUNCTION rrcsmc_tm_ciph_active

DESCRIPTION
  Checks to see if TM-Ciphering is active or not.

DEPENDENCIES
  None.

RETURN VALUE
  True:If TM ciphering is active
  False: If TM ciphering is inactive

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_tm_ciph_active
(
 void
);
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
/*===========================================================================

FUNCTION rrcsmc_append_start_list_per_active_domain

DESCRIPTION
  Checks to see if TM-Ciphering is active or not.

DEPENDENCIES
  None.

RETURN VALUE
  True:If TM ciphering is active
  False: If TM ciphering is inactive

SIDE EFFECTS
  None

===========================================================================*/

uecomdef_status_e_type rrcsmc_append_start_list_per_active_domain 
(
rrc_STARTList *start_list_ext,
rrcsmc_append_start_e_type start_type
);
#endif
/*===========================================================================

FUNCTION rrcsmc_append_start_value_to_start_list

DESCRIPTION
  This function initializes the START_LIST with the latest values to 
  be sent to the UTRAN by the RCE, CU  and ASU procedures. The START
  values for all CN domains for which we have received Integrity
  Protection Info will be sent.
  
  RCE, CU and ASU procedures will call this function in SMC.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns SUCCESS if STARTlist is appended.
  This function returns FAILURE if STARTlist is not appended.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_start_value_to_start_list 
(
rrc_STARTList *start_list_ext,
rrcsmc_append_start_e_type start_type
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
,boolean both_cn_domain_open
,rrc_cn_domain_identity_e_type active_cn_domain
#endif
);

/*===========================================================================

FUNCTION RRCSMC_EXTRACT_RETURN_HFN_PER_DOMAIN_AND_UPDATE_START_FLAG

DESCRIPTION
  This procedure does start value calculation for a given domain and 
  sets the start value update flag.
  
  DEPENDENCIES
  None.

RETURN VALUE
  Updated Start value. Calling func should apply offset to this value to use it for AM/UM/TM

SIDE EFFECTS
  None

===========================================================================*/
uint32 rrcsmc_extract_return_hfn_per_domain_and_update_start_flag
(
  rrc_cn_domain_identity_e_type domain_id
);

/*===========================================================================

FUNCTION RRCSMC_UPDATE_HFN_FOR_RLC_RB

DESCRIPTION
  This procedure updates HFN for RLC size change feature
  
  DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_hfn_for_rlc_rb
(
  rrc_RB_Identity rb_id, 
  rlc_reestablish_e_type direction
);

/*===========================================================================

FUNCTION rrcsmc_process_suspend_cnf

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
/*lint -e715*/
void rrcsmc_process_suspend_cnf
(
rlc_ul_stop_suspend_cnf_type *rrcsmc_suspend_cnf
);
/*===========================================================================

FUNCTION rrcsmc_process_suspend_srns_cnf

DESCRIPTION
  This function performs all the functionality needed for SRNS relocation case 
  after suspending all AM & UM Uplink RLC RB's except-SRB-2/SRB-0.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_process_suspend_srns_cnf
(
void
);

/*===========================================================================

FUNCTION rrcsmc_is_rlc_re_est_for_srns_active

DESCRIPTION
  This function returns the status of SMC doing RLC re-establishment.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
boolean rrcsmc_is_rlc_re_est_for_srns_active
(
  void
);

/*===========================================================================

FUNCTION rrcsmc_send_ul_ciphering_key_req

DESCRIPTION
   This function sends cmd to L1 for configuring UL cipher 
   keys.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_send_ul_ciphering_key_req
(
void
);

/*===========================================================================

FUNCTION rrcsmc_get_start_value_to_transmit_domain

DESCRIPTION
  This function returns the value of global 'start_value_to_transmit_domain'
  
DEPENDENCIES
  None.

RETURN VALUE
  CN Domain Identity

SIDE EFFECTS
  None

===========================================================================*/
rrc_cn_domain_identity_e_type rrcsmc_get_start_value_to_transmit_domain(void);
/*===========================================================================

FUNCTION rrcsmc_get_security_start_status

DESCRIPTION
  This function will look to see if security is started.
  This includes check for ciphering or integrity active 
  for any CN domain.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If security is already stated for any CN domain
  FALSE: other wise.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_security_start_status
(
void
);
/*===========================================================================

FUNCTION rrcsmc_return_saved_security_key_set_status

DESCRIPTION
  This function will return status of security key set 
  generated in other RAT & saved in a buffer for CS domain.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
boolean rrcsmc_return_saved_security_key_set_status
(
  rrc_cn_domain_identity_e_type domain_id
);
/*===========================================================================

FUNCTION rrcsmc_reset_saved_security_key_set_status

DESCRIPTION
  This function will reset status of security key set 
  generated in other RAT & saved in a buffer for CS domain.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_reset_saved_security_key_set_status
(
  void
);
/*===========================================================================

FUNCTION rrcsmc_check_and_update_security_key_set_gen_in_other_rat

DESCRIPTION
  This function will move security key sets(generated in other RAT for CS domain ) from
  buffer to ciphering & integrity db if any.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_check_and_update_security_key_set_gen_in_other_rat
(
  rrc_cn_domain_identity_e_type domain_id
);
/*===========================================================================

FUNCTION rrcsmc_send_continue_req

DESCRIPTION
  This function constructs the CRLC_UL_CONTINUE_REQ primitive and
  sends it to L2 UL Task.
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_send_continue_req
(
  void
);

/*===========================================================================

FUNCTION rrcsmc_calc_max_count_c_released_rb

DESCRIPTION
  This function calculates and stores the maximum count_c values for all 
  released rbs for both domains in a temporary variable.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

void rrcsmc_calc_max_count_c_released_rb
(
  rrcllc_rlc_config_req_list_type *rlc_req_type_ptr,
  rrc_cn_domain_identity_e_type  local_domain_id_list[MAX_RB],
  rrc_proc_e_type proc_id
);

/*===========================================================================

FUNCTION rrcsmc_update_count_c_released_rbs

DESCRIPTION
  This function updates the maximum count c stored in the temporary variables 
  to the permanent one.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

void rrcsmc_update_count_c_released_rbs
(
  void
);

/*===========================================================================

FUNCTION rrcsmc_revert_max_count_c

DESCRIPTION
  This function reverts the maximum count c stored in the temporary variables 
  to the older value, i.e, the value in permanent variable
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_revert_max_count_c
(
  void
);
/*===========================================================================

FUNCTION rrcsmc_get_int_cipher_algo_from_dl_sdu_ind

DESCRIPTION
  This function extracts Ciphering Algo from the OTA message to
  internal format of RRC.

DEPENDENCIES
  None.

RETURN VALUE
  Cipher Algo in RRC internal format
  
SIDE EFFECTS
  None

===========================================================================*/
uecomdef_wcdma_cipher_algo_e_type rrcsmc_get_int_cipher_algo_from_dl_sdu_ind
(
  void *cipher_algo,
  rrc_msg_ver_enum_type msg_version
);

/*===========================================================================

FUNCTION   rrcsmc_append_start_list_with_already_computed_start

DESCRIPTION

  This function appends start list and takes care of the already calculated start value of 
  a given domain which will be included in msg and hence it doesnt re-compute the 
  start value for that domain.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_start_list_with_already_computed_start
(
  rrc_STARTList *start_list_ext,
  byte start_value[RRC_MAX_START_LENGTH], /* start_value should always have a valid value */
  rrcsmc_append_start_e_type append_start_type
);

/*===========================================================================

FUNCTION   rrcsmc_append_already_computed_start_value_to_start_list

DESCRIPTION

  This function appends start list and takes care of the already calculated start value of 
  a given domain which will be included in msg and hence it doesnt re-compute the 
  start value for that domain.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_already_computed_start_value_to_start_list
(
  rrc_STARTList *start_list_ext,
  byte start_value[RRC_MAX_START_LENGTH], /* start_value should always have a valid value */
  rrcsmc_append_start_e_type append_start_type
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
  ,boolean both_cn_domain_open
  ,rrc_cn_domain_identity_e_type active_cn_domain
#endif
);


#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
#error code not present
#endif

/*===========================================================================
FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind_r8

DESCRIPTION
  This function extracts ip check info/integrity protection mode info/ciphering mode info from SRNS 
  relocation triggering messages for Release 8 version of OTAs. In summary, input srns_info has to be filled
  from OTA in cmd_ptr.

  NOTE:  There is no R8 UTRANMobilityInformation and URAUpdateConfirm messages in ASN 8.6.
  Right now, only 5 Reconfig OTAs + CU CNF can do SRNS relocation for R8 OTA.

DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None
===========================================================================*/
uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind_r8
(
  rrcsmc_srns_relocation_security_info_r7_type * srns_info,
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION rrcsmc_check_new_keys_and_reset_append_start_per_domain

DESCRIPTION
  This function checks whether RRC received new keys from NAS and accordingly
  initializes the a byte *value with START values for the domain requested.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_check_new_keys_and_reset_append_start_per_domain 
(
  byte  *value,
  rrc_cn_domain_identity_e_type domain_id,
  rrcsmc_append_start_e_type start_type
);

/*===========================================================================

FUNCTION  rrcsmc_is_reconfig_in_progress

DESCRIPTION

  This function will check if SMC Procedure substate is reconfig in progress
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE: if SMC procedure is wating for reconfig in progress
  FALSE: othewise.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcsmc_is_reconfig_in_progress
(
  void
);

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION rrcsmc_save_security_key_set_from_lte

DESCRIPTION
  This function will move security key sets (generated in LTE for PS domain ) from
  buffer to ciphering & integrity db if any.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_save_security_key_set_from_lte
(
  rrc_ho_ciph_req_type  ho_ciph_req,
  rrc_cn_domain_identity_e_type domain_id
);

#endif

/*===========================================================================

FUNCTION rrcsmc_revert_to_old_config_for_failure_message

DESCRIPTION
This function is used  to revert the cipher config db.
Whenever we update cipher config type to step-1 HO and later if message validation fails, 
we need to revert to the backed up config.

We also reset start value to transmit to 0.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_revert_to_old_config_for_failure_message(boolean need_log);
/*===========================================================================
FUNCTION rrcsmc_get_smc_sub_state()

DESCRIPTION
  This function will return current 
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current smc sub state.
SIDE EFFECTS
none
===========================================================================*/
rrcsmc_substate_e_type  rrcsmc_get_smc_sub_state(void);

/*===========================================================================

FUNCTION rrcsmc_return_substate

DESCRIPTION
returns SMC substate

We also reset start value to transmit to 0.

DEPENDENCIES
  None.

RETURN VALUE
  rrcsmc_substate_e_type
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrcsmc_substate_e_type rrcsmc_return_substate(void);
#ifdef FEATURE_QSH_EVENT_METRIC

/*===========================================================================

FUNCTION rrcsmc_return_substate

DESCRIPTION
returns SMC substate

We also reset start value to transmit to 0.

DEPENDENCIES
  None.

RETURN VALUE
  rrcsmc_substate_e_type
===========================================================================*/

void rrcqsh_log_smc_ota_params(void);
#endif

#endif /* RRCSMC_H */
