#ifndef TDSEUL_H
#define TDSEUL_H

/*============================================================================
              WCDMA L1 HSUPA header file

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
physical channel configuration common for both downlink and uplink .

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2007 Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdseul.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/09/10    ksr     Changes to move logging variables to dynamic memory approach
07/31/09    gnk     Mainlining the feature FEATURE_TDSCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
08/13/08    gv      EUL fixes for stop and other clean up.
07/31/08    oh      EUL changes for Taxis.
10/26/07   gv/vp    Added support to define TDSEUL_MAX_RL using MCAL.
09/15/06    am      Moved HARQ RTT definitions from tdseulsg.h
08/14/06    yiz     Mainline HSUPA code
01/14/06    am      file created.
                    
===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES  

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_TDSCDMA_HSUPA



#include "tdsl1rrcif.h"

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

/*max num of configuration dbs maintained in EUL uplink / EUL downlink /
  serving grant module
*/ 
#define   TDSEUL_MAX_CFG_DB_COUNT              2

/*max num of radiolinks maintained in Layer1-. 25.331, Section 10.3.10. 
*/ 
  #define   TDSEUL_MAX_RL                        4 //MCALWCDMA_E_DL_MAX_RL //taxis workaround
/* max subframe number value*/
#define   TDSEUL_MAX_SUB_FN_COUNT              1280

/*invalid subframe number */
#define   TDSEUL_INVALID_SUB_FN                0xFFFF

/*number of slots in a subframe*/
#define   TDSEUL_SLOTS_PER_SUBFRAME            3

/* HARQ DATA*/
/* HARQ RTT for 10 ms TTI*/
#define      TDSEUL_HARQ_RTT_10MS              4

/* HARQ RTT for 2 ms TTI*/
#define      TDSEUL_HARQ_RTT_2MS               8
//#endif

//#ifdef FEATURE_WCDMA_HSUPA_2MS_TTI
#define      TDSEUL_MAX_HARQ_RTT               TDSEUL_HARQ_RTT_2MS
//#else
//#define      TDSEUL_MAX_HARQ_RTT               TDSEUL_HARQ_RTT_10MS
//#endif

/* This structure provides the E-DCH Radio Network Temporary Identifier 
   (E-RNTI) configured for UE. This structure is sent from downlink module
   to serving grant module during config/reconfig of E-RNTI.The identity
   (E-RNTI) of the UE (or group of UEs) is used to identify the grants sent
   from network to the UE (through an ID-specific CRC attachment)
*/
typedef enum
{
  /* No E-RNTI is configured for  UE*/
  TDSEUL_ERNTI_NOT_AVAILABLE,
  /* primary  E-RNTI is allocated to  UE */
  TDSEUL_PRIMARY_ERNTI,
  /* secondary E-RNTI is allocated to  UE */
  TDSEUL_SECONDARY_ERNTI,
  /* primary and secondary E-RNTI are allocated to  UE */
  TDSEUL_PRIMARY_AND_SECONDARY_ERNTI
  
}tdseul_ernti_cfg_info_enum_type;

/* This structure provides the subframe number within a 10ms frame*/
typedef enum
{
  /* sub frame 0*/
  TDSEUL_2MS_SUB_FR0,
  /* sub frame 1*/
  TDSEUL_2MS_SUB_FR1,
  /* sub frame 2*/
  TDSEUL_2MS_SUB_FR2,
  /* sub frame 3*/
  TDSEUL_2MS_SUB_FR3,
  /* sub frame 4*/
  TDSEUL_2MS_SUB_FR4,
  /*max number of sub frames in a 10ms frame*/
  TDSEUL_2MS_MAX_NUM_SUB_FR_IN_10MS
}tdseul_2ms_sub_fr_num_enum_type;
                                  
/* This structure provides the starting and ending slot numbers for subframes within a 10ms frame*/
typedef enum
{
  /* start slot for subfr0*/
  TDSEUL_2MS_SUB_FR0_START_SLOT=0,
  /* end slot for subfr0*/
  TDSEUL_2MS_SUB_FR0_END_SLOT=2,
  /* start slot for subfr1*/
  TDSEUL_2MS_SUB_FR1_START_SLOT,
  /* end slot for subfr1*/
  TDSEUL_2MS_SUB_FR1_END_SLOT=5,
  /* start slot for subfr2*/
  TDSEUL_2MS_SUB_FR2_START_SLOT,
  /* end slot for subfr2*/
  TDSEUL_2MS_SUB_FR2_END_SLOT=8,
  /* start slot for subfr3*/
  TDSEUL_2MS_SUB_FR3_START_SLOT,
  /* end slot for subfr3*/
  TDSEUL_2MS_SUB_FR3_END_SLOT=11,
  /* start slot for subfr4*/
  TDSEUL_2MS_SUB_FR4_START_SLOT,
  /* end slot for subfr4*/
  TDSEUL_2MS_SUB_FR4_END_SLOT=14
}tdseul_2ms_sub_fr_start_end_slot_num_enum_type;




#endif /*end of FEATURE_TDSCDMA_HSUPA */

#endif /*end of EUL_H*/
