#ifndef EUL_H
#define EUL_H

/*============================================================================
              WCDMA L1 HSUPA header file

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
physical channel configuration common for both downlink and uplink .

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2007 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/eul.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
07/23/14    ash     0x41B6 Enhancement to support Serv HICH, AGCH and UL DPCCH Per BPG Blanking Info
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/22/13    oh      Adding DC-HSUPA framework support
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
02/01/12    gnk     Nikel Feature cleanup.
11/29/11    gnk     Removed some unused features
04/09/10    ksr     Changes to move logging variables to dynamic memory approach
07/31/09    gnk     Mainlining the feature FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
08/13/08    gv      EUL fixes for stop and other clean up.
07/31/08    oh      EUL changes for Taxis.
10/26/07   gv/vp    Added support to define EUL_MAX_RL using MCAL.
09/15/06    am      Moved HARQ RTT definitions from eulsg.h
08/14/06    yiz     Mainline HSUPA code
01/14/06    am      file created.
                    
===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES  

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "l1rrcif.h"
#include "l1def.h"

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/
#ifdef  FEATURE_WCDMA_DC_HSUPA 
 /*max num UL CARRs with DC HSUPA*/
#define MAX_NUM_UL_CARR 2
#define EUL_PRIMARY_CARR_IDX 0
#define EUL_SECONDARY_CARR_IDX 1
#else
#define MAX_NUM_UL_CARR 1
#define EUL_PRIMARY_CARR_IDX 0
#endif /*FEATURE_WCDMA_DC_HSUPA */
/* max num of EDPDCH supported- 25.306-670  4.5.4*/

#define MAX_NUM_DCHSUPA_ACTIVE_CARR 2

#define   E_UL_MAX_NUM_EDPDCH                     4

/*max num of configuration dbs maintained in EUL uplink / EUL downlink /
  serving grant module
*/ 
#define   EUL_MAX_CFG_DB_COUNT              2

/*max num of radiolinks maintained in Layer1-. 25.331, Section 10.3.10. 
*/ 
  #define   EUL_MAX_RL                        4 //MCALWCDMA_E_DL_MAX_RL //taxis workaround
/* max subframe number value*/
#define   EUL_MAX_SUB_FN_COUNT              1280

/* number of BPG per sub frame */
#define   EUL_BPG_PER_SUB_FN                30

/* Invalid BPG number */
#define   EUL_INVALID_BPG_VAL               0xFF
/*invalid subframe number */
#define   EUL_INVALID_SUB_FN                0xFFFF

#ifdef FEATURE_WCDMA_HS_RACH
/*Invalid CFN VAL*/
#define EUL_INVALID_CFN_VAL                0xFFFF
#endif /*FEATURE_WCDMA_HS_RACH*/

/*number of slots in a subframe*/
#define   EUL_SLOTS_PER_SUBFRAME            3

/* HARQ DATA*/
/* HARQ RTT for 10 ms TTI*/
#define      EUL_HARQ_RTT_10MS              4

/* HARQ RTT for 2 ms TTI*/
#define      EUL_HARQ_RTT_2MS               8
//#endif

#define      EUL_MAX_HARQ_RTT               EUL_HARQ_RTT_2MS

/* This structure provides the E-DCH Radio Network Temporary Identifier 
   (E-RNTI) configured for UE. This structure is sent from downlink module
   to serving grant module during config/reconfig of E-RNTI.The identity
   (E-RNTI) of the UE (or group of UEs) is used to identify the grants sent
   from network to the UE (through an ID-specific CRC attachment)
*/
typedef enum
{
  /* No E-RNTI is configured for  UE*/
  EUL_ERNTI_NOT_AVAILABLE,
  /* primary  E-RNTI is allocated to  UE */
  EUL_PRIMARY_ERNTI,
  /* secondary E-RNTI is allocated to  UE */
  EUL_SECONDARY_ERNTI,
  /* primary and secondary E-RNTI are allocated to  UE */
  EUL_PRIMARY_AND_SECONDARY_ERNTI
  
}eul_ernti_cfg_info_enum_type;

/* This structure provides the subframe number within a 10ms frame*/
typedef enum
{
  /* sub frame 0*/
  EUL_2MS_SUB_FR0,
  /* sub frame 1*/
  EUL_2MS_SUB_FR1,
  /* sub frame 2*/
  EUL_2MS_SUB_FR2,
  /* sub frame 3*/
  EUL_2MS_SUB_FR3,
  /* sub frame 4*/
  EUL_2MS_SUB_FR4,
  /*max number of sub frames in a 10ms frame*/
  EUL_2MS_MAX_NUM_SUB_FR_IN_10MS
}eul_2ms_sub_fr_num_enum_type;

/* This structure provides the starting and ending slot numbers for subframes within a 10ms frame*/
typedef enum
{
  /* start slot for subfr0*/
  EUL_2MS_SUB_FR0_START_SLOT=0,
  /* end slot for subfr0*/
  EUL_2MS_SUB_FR0_END_SLOT=2,
  /* start slot for subfr1*/
  EUL_2MS_SUB_FR1_START_SLOT,
  /* end slot for subfr1*/
  EUL_2MS_SUB_FR1_END_SLOT=5,
  /* start slot for subfr2*/
  EUL_2MS_SUB_FR2_START_SLOT,
  /* end slot for subfr2*/
  EUL_2MS_SUB_FR2_END_SLOT=8,
  /* start slot for subfr3*/
  EUL_2MS_SUB_FR3_START_SLOT,
  /* end slot for subfr3*/
  EUL_2MS_SUB_FR3_END_SLOT=11,
  /* start slot for subfr4*/
  EUL_2MS_SUB_FR4_START_SLOT,
  /* end slot for subfr4*/
  EUL_2MS_SUB_FR4_END_SLOT=14
}eul_2ms_sub_fr_start_end_slot_num_enum_type;

#ifdef FEATURE_WCDMA_HS_RACH
/* This below ENUM is used to differentiate between the setup
** as a EDCH setup or HS RACH setup. As they both cannot
** co-exist .. we can used this approach.*/
typedef enum{
   /*This identifies the setup for EDCH*/
  WL1_EUL_EDCH_SETUP,

  /* This identifies the setup as for HS RACH*/
  WL1_EUL_HS_RACH_SETUP,

  /* INVALID ENUM value for the SETUP TYPE*/
  WL1_EUL_SETUP_INVALID
} wl1_eul_chan_setup_enum_type;

/* Defines the action and should be used with the above
** ENUM value to associate the action to the setup type*/
typedef enum{
  /* For Start*/
  WL1_EUL_ACTION_START,
  
    /* For RESUME ... for now this is only for HS RACH*/
  WL1_EUL_ACTION_RESUME,
  
  /* For SUSPEND ... for now this is only for HS RACH*/
  WL1_EUL_ACTION_SUSPEND,
  
  /* Reconfiguration*/
  WL1_EUL_ACTION_RECONFIG,
  
  /* For STOP*/
  WL1_EUL_ACTION_STOP,

  /* INVALID ACTION*/
  WL1_EUL_ACTION_INVALID
}wl1_eul_action_enum_type;

#endif /* FEATURE_WCDMA_HS_RACH*/



#endif /*end of EUL_H*/
