#ifndef MCCREG_I_H
#define MCCREG_I_H
/*===========================================================================

           C D M A    R E G I S T R A T I O N    S E R V I C E S
                       H E A D E R    F I L E

DESCRIPTION
  This file data and function declarations necessary for CDMA Registration
  Services.

  Copyright (c) 1993 - 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccreg.h_v   1.4   01 Aug 2002 19:56:00   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccreg_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/15   ppr     Reg Throttling chgs for Transceiver request denial cases
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/23/14   ppr     Graceful handling for NV Read/Write error in Card mode
01/24/13   ppr     New API - mccreg_powerup_mode_online() added for
                   PROT_OPRT_MODE_ONLINE case
10/16/12   ppr     FEATURE_USERZONE cleanup
04/04/12   trc     Convert to m1x_time module
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
04/25/11   ag      Replaced clk.h with header files from time services.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/11/11   jj      Added changes to have wildcard band_class/blk_sys.
12/17/10   ag      Replaced clk APIs with time APIs.
12/10/10   jj      Added support for CSFB operation.
11/15/10   jtm     Initial SU API split for CP.
09/15/10   azh     Fixed KlocWork warnings.
02/10/10   jj      Added support for ESS feature.
02/23/07   sb      Added support for operational mode feature.
01/15/07   sb      VOIP-1X handoff updates.
06/01/06   an      Added the bypass registration indicator changes
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone,
                    auth and rand during AHO/APHO.
08/17/04   az      Rel D Negative SCI changes
08/03/04   pg      Added support for Rel D timer based registration.
11/19/03   va      Added support for REL C register_in_idle
07/23/03   bkm     Mainlined FEATURE_ANTIREGISTRATION_BIAS.
08/01/02   fc      Added support for the count of the number of powerup
                   registration performed.
02/22/02   fc      Added support for Release A common channels.
02/14/02   fc      Merged from 1.3 in MSM archive.
 01/25/02  sj      Added prototype for funtion mccreg_uz_update().
 12/14/01  fc      Removed prototype for mccreg_blksys_cb_init.
06/26/01   fc      Merged the following from common archive:
           06/13/01   fc      Added prototype for mccreg_blksys_cb_init.
03/06/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ks      Removed ENHANCED_STANDBY_II feature.
10/05/99   lh      Serving System update for CM is added.
06/22/99   ks      new structure mccreg_bs_reg_info is created for current BS
                   information.
03/01/99   ejv     Merged functions for FEATURE_ANTIREGISTRATION_BIAS.
11/04/98   ych     Added prototype for mccreg_reset_reg().
05/13/96   jca     Updates for J-STD-008 registration procedures.
05/25/95   jca     Updates for IS-95A registration procedures.
03/18/93   jai     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "caii.h"
#include "caii_v.h"
#include "caii_i.h"
#include "nv.h"
#include "mccreg.h"
#include "mccreg_v.h"
#include "modem_1x_defs.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Special REG_TYPE value that indicates registration access not required */
#define MCCREG_NO_REG_REQUIRED 0xFF

/* This constant is only used by DS to indicate un-registered status */
#define MAXUINT16 65535U            /* Largest 16-bit unsigned integer */

/* This constant is used to check the boundary limits of  reg_count_table*/
#define MAX_REG_PERIOD 86
#define MIN_REG_PERIOD 29

/* Max number of consecutive reg attempts 1x can make before going to sleep 
 * and release the chain for transceiver request denial scenarios  */
#define MAX_REG_TRANS_REQ_DENIAL_CNT 3


#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* This provides wildcard value which will be used in LTE mode
 * to prevent powerup registrations because of blk_sys change
 * or wild card band class.
 * Since on LTE we dont have freq/channel we can not obtain
 * blk_sys and hence we dont want to perform power-up regs based on it
 * while being on LTE.
 */
#define MCC_BLK_SYS_WILDCARD 0xFFFF
#define MCC_BAND_CLS_WILDCARD 0x1F
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */


/* Type to hold an entry from the SID/NID list */
typedef struct
{
  q_link_type link;
  word sid;
  word nid;
  nv_block_or_sys_enum_type block_or_sys;
  byte band_class;
  int4 cnt;              /* Number of msecs before timer expires */
} mccreg_sid_nid_type;

/* This is used for the current BS info after a succesful
   CDMA registration */
typedef struct
{
  word sid;
  byte band_class;
  nv_block_or_sys_enum_type block_or_sys;
} mccreg_bs_reg_info;

typedef enum
{
  MCCREG_REG_NEEDED,
  MCCREG_REG_COMPLETED
} mccreg_registration_status_type;

typedef struct
{
  mccreg_registration_status_type reg_status;
    /* Indicator of whether the mobile has registered in CDMA mode */
  boolean powering_up;
    /* Indicator of whether the mobile is in powerup state */
  boolean powerup_timer_on;
    /* Indicator of whether the mccreg_powerup_timer is running */
  boolean counter_enabled;
    /* Indicator of whether the mobile is currently processing timer based
       registration */
  boolean reg_enabled;
    /* Registration status indicator */
  boolean mob_term;
    /* Call termination enabled indicator */
  int4 timer_cnt;
    /* Number of ms which have expired since last update of
       zone timer and sid nid list due to sleep */
  boolean dist_reg_reqd;
    /* Indicator of whether a distance based registration is required */
  word reg_dist;
    /* Registration distance received from base station where mobile last
       registered */
  dword reg_count_max;
    /* Number of 80 ms periods before a timer based registration is required */
  dword reg_count_ms;
    /* Number of ms since the last timer base registration */
  dword base_lat_reg;
    /* Lattitude of base station where the mobile last registered */
  dword base_long_reg;
    /* Longitude of base station where the mobile last registered */
  byte  band_class;
    /* Band class of base station where the mobile last registered */
  nv_block_or_sys_enum_type block_or_sys;
    /* Cellular serv-sys (A or B) or PCS frequency block (A-F) of base station
       where the mobile last registered */

  /* Stores Idle State BS info used in registration success/failure procdure */
  struct
  {
    word sid;                   /* System identification */
    word nid;                   /* Network identification */
    word reg_zone;              /* Registration zone */
    byte zone_timer;            /* Zone timer length */
  } idle_reg_info;

  /* Keeps track of last registered sci */
  int8 reg_sci;

  #ifdef FEATURE_IS2000_REL_C
  boolean register_in_idle;
   /* Indicates if MS needs to register after entering IDLE state */
  #endif /* FEATURE_IS2000_REL_C */

} mccreg_vars_type;

extern mccreg_vars_type mccreg_vars;


/* ------------------------------------------------------------------
** The mobile station shall store a list of zones in which the mobile
** station has registered (ZONE_LISTs).  Each entry in the zone list
** shall include the zone number (REG_ZONE) and the (SID,NID) pair
** for the zone.  The mobile station shall be capable of storing at
** least N9m entries in ZONE_LISTs (IS-95A section 6.6.5.1.5).
** ------------------------------------------------------------------ */
typedef struct
{
  q_link_type link;
  word reg_zone;
  word sid;
  word nid;
  nv_block_or_sys_enum_type block_or_sys;
  byte band_class;
  int4 cnt;              /* Number of ms before timer expires */
} mccreg_zone_type;

/* Pointer to item on ZONE_LISTs with a disabled timer */
extern mccreg_zone_type *mccreg_zone_ptr;

extern q_type mccreg_zone_free_q;
extern q_type mccreg_zone_q;



/* Pointer to item on SID_NID_LISTs with a disabled timer */
extern mccreg_sid_nid_type *mccreg_sid_nid_ptr;

extern q_type mccreg_sid_nid_free_q;
extern q_type mccreg_sid_nid_q;

/* ----------------------------------
** Registration Timers and Indicators
** ---------------------------------- */
extern timer_type mccreg_powerup_timer;
  /* Power-up/Initialization timer */
extern timer_type mccreg_timer_reg_timer;
  /* Timer-based registration timer */
extern timer_type mccreg_zone_timer;
  /* Timer for zone based registration */

/* Lookup table for timer based reg period formula */
extern dword const reg_count_table[];
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCCREG_INIT

DESCRIPTION
  Performs powerup initialization:
    - Initialize timers
    - Initialize queues

DEPENDENCIES
  This procedure should only be called once after powerup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_init( void );

/*===========================================================================

FUNCTION MCCREG_GET_BS_REG_INFO

DESCRIPTION
  This function is called by other subsystems to receive the current Base
  Station information (SID,BAND_CLASS and BLOCK_OR_SYS type) where the
  Mobile has registered.

DEPENDENCIES
  This should be called based on the current Mode of the MS. This function
  returns "FALSE" if the Mobile is not registered,

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS


===========================================================================*/

boolean mccreg_get_bs_reg_info( mccreg_bs_reg_info* info);

/*===========================================================================

FUNCTION MCCREG_POWERUP

DESCRIPTION
  This function performs power-up registration procedures as called out
  in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If power-up registration procedures were successful
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_powerup( void );

/*===========================================================================

FUNCTION MCCREG_POWERUP_MODE_ONLINE

DESCRIPTION
  This function performs power-up registration procedures as called out
  in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mccreg_powerup_mode_online( void );

/*===========================================================================

FUNCTION MCCREG_ACQ_INIT

DESCRIPTION
  This function performs registration variable initialization for a mode
  or serving system change as called out in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Flags are initialized.

===========================================================================*/

void mccreg_acq_init( void );

/*===========================================================================

FUNCTION MCCREG_IDLE_INIT

DESCRIPTION
  This function performs processing necessary when entering the Idle state
  from the Initialization state (see IS-95A section 6.6.5.5.1.3).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_idle_init( void );

/*===========================================================================

FUNCTION MCCREG_IDLE_CHECK

DESCRIPTION
  This function performs idle registration procedures called out in
  IS-95A section 6.6.5.5.2.1.

DEPENDENCIES
  None

RETURN VALUE
  If a registration is required, the type of registration required will be
  returned.  Otherwise the special indicator MCCREG_NO_REG_REQUIRED will
  be returned.

SIDE EFFECTS
  None

===========================================================================*/

byte mccreg_idle_check( void );

/*===========================================================================

FUNCTION MCCREG_SYSPARM

DESCRIPTION
  This function processes the registration variables of a received System
  Parameters Message as specified in IS-95A section 6.6.5.5.2.2.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_sysparm( caii_sysparm_type *msg_ptr );

/*===========================================================================

FUNCTION MCCREG_POWERDOWN

DESCRIPTION
  This function performs the powerdown updates to the zone list, the
  sid nid list and the distance registration variables in NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The NV SID/NID list, zone list and distance registration variables
  are updated.

===========================================================================*/

void mccreg_powerdown( void );

/*===========================================================================

FUNCTION MCCREG_IDLE_POWERDOWN

DESCRIPTION
  This function performs Idle Channel powerdown registration procedures
  as specified in IS-95A section 6.6.5.5.2.4.

DEPENDENCIES
  None

RETURN VALUE
  CAI_POWERDOWN_REG if the mobile is required to register.
  MCCREG_NO_REG_REQUIRED if the mobile is not required to register.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

byte mccreg_idle_powerdown( void );

/*===========================================================================

FUNCTION MCCREG_SET_REG_SCI

DESCRIPTION
  This sets the reg_sci in mccreg_vars structure.
  It is set during explicit and implicit registration.

  This set value is later used to appropriately update the operational SCI when
  L2 ack for the registration is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_set_reg_sci
(
  int8 sci
);

/*===========================================================================

FUNCTION MCCREG_SUCCESS

DESCRIPTION
  This function performs registration actions required after the mobile
  station receives an acknowledgement for a Registration Message,
  Origination Message, or Page Response Message sent on the Access
  Channel as described in IS-95A section 6.6.5.5.3.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_success( void );

/*===========================================================================

FUNCTION MCCREG_IDLE_ACK_OR_FAIL

DESCRIPTION
  This function performs registration actions required after an
  acknowledgement is received to any message other than a Registration,
  Origination or Page Response or after a failed access attempt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_idle_ack_or_fail( void );

/*===========================================================================

FUNCTION MCCREG_TC_INIT

DESCRIPTION
  This function performs registration procedures required for entering
  the Traffic Channel Initialization Substate of the Mobile Station Control
  on the Traffic Channel State as specified in IS-95A section 6.6.5.5.4.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Timer based registration is disabled.

===========================================================================*/

void mccreg_tc_init( void );

/*===========================================================================

FUNCTION MCCREG_MS_REG

DESCRIPTION
  This function performs registration procedures required after receiving
  a Mobile Station Registered Message as specified in IS-95A section
  6.6.5.5.4.3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Registration variables are updated.

===========================================================================*/

void mccreg_ms_reg( caii_registered_type *msg_ptr );

/*===========================================================================

FUNCTION MCCREG_GET_MOB_TERM

DESCRIPTION
  This function retrieves the call termination enabled indicator.

DEPENDENCIES
  None.

RETURN VALUE
  Call termination enabled indicator.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccreg_get_mob_term( void );

/*===========================================================================

FUNCTION MCCREG_RESET_REG_STATUS

DESCRIPTION
  This function resets the registration status to indicate that the mobile
  needs to register.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccreg_reset_reg_status( void );

/*===========================================================================

FUNCTION MCCREG_FIND_SID_NID

DESCRIPTION
  This function searches SID_NID_LISTs the input (SID,NID) pair.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the (SID,NID) pair on SID_NID_LISTs if it is found.
  NULL otherwise.

SIDE EFFECTS
  None

===========================================================================*/

mccreg_sid_nid_type *mccreg_find_sid_nid( word sid,  word nid );

/*===========================================================================

FUNCTION MCCREG_ZONE_REG_REQUIRED

DESCRIPTION
  This function checks if a zone-based registration is required
  for the input (ZONE, SID,NID).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a zone-based registration is required for the input (ZONE, SID,NID).
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_zone_reg_required
(
  word zone,  /* ZONE to find */
  word sid,   /* SID to find */
  word nid    /* NID to find */
);

/*===========================================================================

FUNCTION MCCREG_PARAMETER_REG_REQUIRED

DESCRIPTION
  This function checks if a zone-based registration is required
  for the input (SID,NID).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a parameter-based registration is required for the input (SID,NID).
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_parameter_reg_required
(
  word sid,   /* SID to find */
  word nid    /* NID to find */
);

/*===========================================================================

FUNCTION MCCREG_UPDATE_LISTS

DESCRIPTION
  This function updates and expires timers on the SID/NID and zone lists.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_update_lists( void );

/*===========================================================================

FUNCTION MCCREG_SET_IDLE_REG_INFO

DESCRIPTION
  This function stores Idle state BS info to be used in registration
  success/failure procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_set_idle_reg_info
(
  caii_sysparm_type *msg_ptr
    /* Pointer to received System Parameter message */
);


/*===========================================================================

FUNCTION MCCREG_GET_POWERUP_REG_COUNT

DESCRIPTION
  This function retrieves the count for the number of powerup registrations
  performed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_get_powerup_reg_count(qword count);

/*===========================================================================

FUNCTION MCCREG_RESET_POWERUP_REG_COUNT

DESCRIPTION
  This function resets the count for the number of powerup registrations
  performed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_reset_powerup_reg_count(void);

/*===========================================================================

FUNCTION MCCREG_INCREMENT_POWERUP_REG_COUNT

DESCRIPTION
  This function increments the count for the number of powerup registrations
  performed by 1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_increment_powerup_reg_count(void);


#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION MCCREG_SET_REGISTER_IN_IDLE

DESCRIPTION
  This function sets the register in idle flag which is used for
  enc/msg integritry resync required registration as commanded by BS on TC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Setting this may cause MS to perform "enc/msgint resync required" reg
  in the common channel.

===========================================================================*/

void mccreg_set_register_in_idle
(
  boolean register_in_idle
);
#endif

/*===========================================================================

FUNCTION MCCREG_SET_MOB_TERM

DESCRIPTION
  This function sets the call termination enabled indicator (MOB_TERMs)
  based on the input system parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mccreg_set_mob_term
(
  word sid,
    /* System ID */
  word nid
    /* Network ID */
);


/*===========================================================================

FUNCTION MCCREG_IS_REG

DESCRIPTION
  This function returns CDMA registration status.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - MS is currtenly registered.
  FALSE - MS is not currently registered.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mccreg_is_reg( void );

/*===========================================================================

FUNCTION MCCREG_SET_REG_ENABLED

DESCRIPTION
  This function sets the registration enabled indicator based on
  the input system parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_set_reg_enabled
(
  word sid,
    /* System ID */
  word nid,
    /* Network ID */
  boolean home_reg,
    /* Home registration enabled indicator */
  boolean for_nid_reg,
    /* Foreign NID roamer registration enabled indicator */
  boolean for_sid_reg
    /* Foreign SID roamer registration enabled indicator */
);

/*===========================================================================

FUNCTION MCCREG_UPDATE

DESCRIPTION
  This function performs the SID_NID_LIST and ZONE_LIST updates described
  in IS-95A section 6.6.4.4.3.1 (after the mobile station receives an
  acknowledgement for a Registration Message, Origination Message, or
  Page Response Message sent on the Access Channel) and section 6.6.5.5.4.3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  SID_NID_LISTs and ZONE_LISTs will change.

===========================================================================*/

void mccreg_update(
  word sid,        /* SID for new entry */
  word nid,        /* NID for new entry */
  word reg_zone,   /* REG_ZONEs for new entry */
  word zone_timer, /* Zone timer for new entry */
  word total_zones /* New number of total zones */
);

/*===========================================================================

FUNCTION MCCREG_CHECK_TOTAL_ZONES

DESCRIPTION
  This function removes entries from ZONE_LISTs if there are more entries
  than allowed by the setting of TOTAL_ZONES.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mccreg_check_total_zones(
  word total_zones
    /* Number of registration zones to be retained */
);

/*===========================================================================

FUNCTION MCCREG_DELETE_MULT_SIDS

DESCRIPTION
  If SID_NID_LISTs contains entries with different SIDs, this function
  deletes the excess entries according to the rules specified in
  IS-95A section 6.6.5.1.5.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mccreg_delete_mult_sids( void );

/*===========================================================================

FUNCTION MCCREG_DELETE_MULT_NIDS

DESCRIPTION
  If SID_NID_LISTs contains more than one entry for any SID, this function
  deletes the excess entries according to the rules specified in IS-95A
  section 6.6.5.1.5.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mccreg_delete_mult_nids( void );

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION MCCREG_READ_NV_POWERUP_REG_PERFORMED

DESCRIPTION
  Reads the NV item NV_CDMA_POWERUP_REG_PERFORMED_I. 
  This procedure should only be called during MCFG refresh.

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_read_nv_powerup_reg_performed( void );
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#endif /*MCCREG_V_H*/
