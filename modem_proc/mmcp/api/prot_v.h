#ifndef PROT_V_H
#define PROT_V_H
/*===========================================================================

                        C O M M O N    P R O T O C O L
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all common types required for the MMoC to
  communicate with the protocols and vice versa.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/prot.h_v   1.1   07 Jun 2002 13:23:22   sjaikuma  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/prot_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
12/23/10   sv      Integrate DSDS feature
01/05/10   ay      Fixes in subscription protocol
12/22/09   ay      Added protocol support for LTE
07/13/09   np      Added code for LTE support
02/06/09   aj      Update prot_subsc_chg_e_type
07/30/08   rm      CMI defeaturisation
09/04/08   sv      Added WLAN_WPS_START and WLAN_WPS_ABORT generic protocol
                   commands
03/13/08   sk      Added PROT_DEACT_FROM_DORMANT to support CM/CMREGPRX
                   changes for allowing CDMA scans during suspended PS data
                   call.
04/17/07   rp      Added RPC metacomments for WLAN.
01/04/07   pk      Added support for VoIP -> 1X Handoff
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
02/28/05   ka      Added new enums and macros for recording protocol that
                   had a subscription change.
09/28/04   sj      Added support for WLAN.
08/20/04   sj      Added support for prl_pref.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
06/07/02   SJ      Renamed PROT_TRANS_NONE_PENDING to PROT_TRANS_NONE.
03/28/02   SJ      Initial release.

===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "sd.h"        /* Interface to System Determination 2.0 */
#include "sys.h"       /* common header file */
#include "prot.h"


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== COMMON TYPES =================================
=============================================================================
=============================================================================
===========================================================================*/


/* Reserved values for prot_trans_type.
*/
/* To indicate that no transaction is pending.
*/
#define    PROT_TRANS_NONE            (prot_trans_type)(0)

/* To indicate that the report was generated autonomously.
*/
#define    PROT_TRANS_AUTONOMOUS      (prot_trans_type)(1)

/* Generic reserved transaction ID.
** Currently only being used by HDR to send response to MMOC when CM sends a
** tunnel deactivate request to it.
*/
#define    PROT_TRANS_ID_RESERVED     (prot_trans_type)(0xFF)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/






/* Enumeration of MMoC command types to the protocols.
*/
typedef enum {

  PROT_CMD_ACTIVATE,                   /* Command to activate the protocol.
                                       */

  PROT_CMD_DEACTIVATE,                 /* Command to deactivate the protocol.
                                       */

  PROT_CMD_GENERIC,                    /* Generic system selection command
                                       ** to the protocol.
                                       */

  PROT_CMD_PH_STAT_CHGD,               /* Command to inform the protocols of
                                       ** a change either due to
                                       ** subscription availability or
                                       ** operating mode change.
                                       */

  PROT_CMD_DS_STAT_CHGD,               /* Dual Standby status changed
                                       */

  PROT_CMD_PS_DETACH,                  /* Send PS detach req. to NAS for add/removal of
                                       ** in mode pref
                                       */

  PROT_CMD_SRV_REQ_PROCEED,            /* Aksing CMSOA if we could send service request */

  PROT_CMD_UE_MODE_SWITCH,

  PROT_CMD_MULTIMODE_SUBS_CHGD,               /* Multimode subscription changed 
                                       */                                       

  PROT_CMD_MAX                         /* For internal use. */

} prot_cmd_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of protocols that have a change in subscription.
** Used when subscription available for a protocol changes. Has been
** added to prevent protocols from reloading NV when ph_status_chgd_cmd
** is called.
*/
typedef enum {

  PROT_SUBSC_CHG_NONE       = 0,                       /* subscription chng
                                                       ** for no protocol
                                                       */

  PROT_SUBSC_CHG_CDMA       = BM(SYS_SYS_MODE_CDMA),   /* subscription chng
                                                       ** for cdma only
                                                       */

  PROT_SUBSC_CHG_GSM        = BM(SYS_SYS_MODE_GSM) ,   /* subscription chng
                                                       ** for GSM only
                                                       */

  PROT_SUBSC_CHG_WCDMA      = BM(SYS_SYS_MODE_WCDMA) , /* subscription chng
                                                       ** for WCDMA only
                                                       */
  //PROT_SUBSC_CHG_LTE        = BM(SYS_SYS_MODE_LTE) , /* subscription chng
  //                                                     ** for LTE only
  //                                                     */

  PROT_SUBSC_CHG_GW         = BM(SYS_SYS_MODE_GSM) | BM(SYS_SYS_MODE_WCDMA) ,
                                                       /* subscription chng
                                                       ** for GW
                                                       */

  //PROT_SUBSC_CHG_GWL        = BM(SYS_SYS_MODE_GSM) | BM(SYS_SYS_MODE_WCDMA) |
  //                            BM(SYS_SYS_MODE_LTE),
                                                       /* subscription chng
                                                       ** for GWL
                                                       */


  PROT_SUBSC_CHG_ALL        = BM(SYS_SYS_MODE_CDMA) | \
                              BM(SYS_SYS_MODE_GSM) | BM(SYS_SYS_MODE_WCDMA), \
  //                         |BM(SYS_SYS_MODE_LTE)
                                                   /* subscription change for
                                                   ** all protocols
                                                   */

  PROT_SUBSC_CHG_MAX                                   /* For internal use */

} prot_subsc_chg_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Adding new protocol
*/
#define    ADD_PROTO(X,Y) (prot_subsc_chg_e_type) ((int)X | (int)Y)

/* Checking for particular protocol
** Checks if protocol Y of type prot_subsc_chg_e_type
** is currently present in X.
*/
#define    CHECK_PROTO(X,Y)  ((int) X & (int) Y)



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/






/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with information on Activate
** protocol command.
*/
typedef struct {

  /* Protocol Activate reason (Online acquire, offline etc).
  */
  prot_act_e_type                      reason;

} prot_act_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with information on deactivate
** protocol command.
*/
typedef struct {

  /* Protocol deactivate reason (Powerdown, reset etc).
  */
  prot_deact_e_type                    reason;

  /* Transaction Id that should be used for acknowledging this command.
  */
  prot_trans_type                      trans_id;

} prot_deact_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with information on phone status
** changed protocol command.
*/
typedef struct {

  /* Phone status change type (NAM selection, Operating mode etc).
  */
  prot_ph_stat_chg_e_type              chg_type;

  /* Current NAM selection.
  */
  byte                                 nam;

  /* Current GSM/WCDMA/LTE subscription availability status.
  */
  boolean                              is_gwl_subs_avail;

  /* Current CDMA/AMPS/HDR subscription availability status.
  */
  boolean                              is_cdma_subs_avail;

  /* Current operating mode of the phone.
  */
  prot_oprt_mode_e_type                oprt_mode;

  /* Transaction Id that should be used for acknowledging this command.
  */
  prot_trans_type                      trans_id;

  /* Enumeration of protocols that have a change in
  ** subscription available status
  */
  prot_subsc_chg_e_type                prot_subsc_chg;

} prot_ph_stat_chgd_s_type;

/* A type for parameters that are associated with information on dual standby
** status changed protocol command.
*/
typedef struct
{
  sys_modem_dual_standby_pref_e_type  ds_pref;
  /* Dual Standby preference
  */

  prot_trans_type                     trans_id;
  /* Transaction Id that should be used for acknowledging this command.
  */

//arun khanna check will it uint8 or ss_something
  uint8                               active_ss;
  /* Bitmask containing status of which ss are active */

  sys_modem_device_mode_e_type        device_mode;


} prot_ds_stat_chgd_s_type;

typedef struct 
{
  uint32  subs_capability[MAX_AS_IDS];
  /* Dual Standby preference 
  */
  
  prot_trans_type                     trans_id;
  /* Transaction Id that should be used for acknowledging this command.
  */

} prot_subs_cap_chgd_s_type;

#endif /*PROT_V_H*/
