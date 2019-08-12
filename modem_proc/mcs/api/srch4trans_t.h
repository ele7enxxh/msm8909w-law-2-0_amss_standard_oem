#ifndef SRCH4TRANS_T_H
#define SRCH4TRANS_T_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   T R A N S L A T I O N S
                      T Y P E D E F   H E A D E R F I L E

GENERAL DESCRIPTION
   This file contains the exported structs and defines for searcher 4's
   translation layer.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/srch4trans_t.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/12   sr      Srch4 Cleanup and merging avg and sum search combining.
12/02/10   zm      Moved into mcs/api folder - SU API effort.
12/02/10   bb      Changes to support not active or empty HPQ/LPQ/HPQ1 scenarios
10/28/10   mca     Added SRCH4_RD_PROXIMITY
07/12/10   cjb     Added SRCH4_CHIPX16_PER_HALF_CHIP (1x dependency removal).
06/21/10   cjb     Featurization removal for CMI-P4:
                   -> MACROS moved to srch4drv.h
                   -> Removed featurizations from enums
05/28/10   vks     Merge from 1H09:
                   12/03/09   vks     Decouple 1x from SRCH4. Include only
                   srch_srch4.h from 1x and remove other includes. Make
                   appropriate changes.
04/16/10   bb      Code changes to increase SRCH4_MAX_NUM_TASKS value to 32
                   for GENESIS modem. 
12/30/09   vks     Add SRCH4's own tech module for MSGR.
12/29/09   vks     Replace INTLOCKs.
11/09/09   ag      Added an include modem_mcs_defs.h for 9K builds.
06/30/09   sst     Re-introduce support for 7627
03/27/09   ns      Clean up to support SCMM and 8650 builds only
09/08/08   adw     Added SRCH4_AFLT_ENABLED & SRCH4_HDR_ENABLED.
06/20/08   mca     Merge from //depot
05/27/08   mca     Modified srch4 differentiate between normal 1x and offtl
                   searching
04/24/08   ljl     Changed the task number to 28 for MSM7800
01/19/07   sst     Create srch4_tech_type to replace defines
01/16/06   awj     Lint cleanup
10/06/05   bt      Added SRCH4_MIN_WINDOW_SIZE
07/05/05   sst     Update comment for SRCH4_MAX_WINDOW_SIZE
05/17/05   sst     Defined SRCH4_MAX_WINDOW_SIZE
07/13/04   sst     Changed SRCH4_MAX_TYPE to SRCH4_MAX_TECH in srch4_srch_type
                   Added SRCH4_NO_TECH = SRCH4_MAX_TECH in srch4_srch_type
                   Changed SRCH4_NO_TECH to SRCH4_NO_TECH_MASK
06/25/04   jcm     Mainlined FEATURE_HDR, SRCH4_HDR_ENABLED, and
                   SRCH4_AFLT_ENABLED
05/04/04   sst     Merge from srch4 3Q dev branch
05/03/04   sst     Moved COH_TRUNC_* defines from srch4drv_t.h to be exported
04/15/04   sst     More code cleanup
04/12/04   sst     Moved srch4_tech_params_struct_type to srch4trans.c
                   Removed debug include file
04/08/04   sst     Code cleanup
03/25/04   sst     Added SRCH4_RET_APP_NOT_VALID and SRCH4_PRIO_MIN
03/11/04   sst     Added search priority enum
03/04/04   sst     First cut of MDSP mechanism rework
02/20/04   sst     Intermediate Checkin
02/12/04   sst     Intermediate Checkin
12/05/03   sst     initial implementation, broken out from srch4drv.h

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/* Common */
#include "customer.h"
#include "comdef.h"

/* FW */
#include "srchfw_msg.h"

/* Other */
#include "rex.h"


/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/

/* External "re-definitions" for internal defines */

#define  SRCH4_ABS( x ) ( ((x) < 0) ? -(x) : (x) )

/* Mask for normalizing chipx8 values */
#define  SRCH4_PN_CIRCLE_MASK    0x3FFFF

/* The number of 1/8 chip "chipx8" units per half-chip. Used in slew math. */
#define  SRCH4_CHIPX8_PER_HALF_CHIP    4

/* The number of 1/16 chip "chipx16" units per half-chip. Used in slew math */
#define  SRCH4_CHIPX16_PER_HALF_CHIP     8

/* The number of 1/16 chip "chipx16" units per Cx8. Used in slew math */
#define  SRCH4_CHIPX16_PER_EIGHTH_CHIP   2

/* The number of 1/8 chip "chipx8" units per chip. Used in slew math. */
#define  SRCH4_CHIPX8_PER_CHIP    8

/* Special window index which implies the full PN space
   is desired in the search */
#define SRCH4_FULL_CIRCLE          -1

/* The walsh code used for the TD path of a common pilot */
#define SRCH4_SRCH_COM_TD_WALSH  0x10

/* define the maximum allowable 1x srch4 window size in chips */
#define SRCH4_MAX_WINDOW_SIZE  32767

/* dfine the minimum 1X srch4 window size in chips */
#define SRCH4_MIN_WINDOW_SIZE  4

/* Max number of tasks/sectors programmable for HPQ and LPQ searches */
/* Make sure SRCH4_MAX_NUM_TASKS is set correctly */
#define SRCH4_MAX_NUM_TASKS      SRCH_MAX_HPQ_TASKS

/* for AFLT support */
#define SRCH4_MAX_AFLT_TASKS     40

/* coherent truncation value */
/*  (use determined by MSM6500 MDSP 1x/Searcher Interface, dated 9/2002)    */
#define COH_TRUNC_1        0x0 /* truncate by 1 bit                         */
#define COH_TRUNC_2        0x1 /* truncate by 2 bit(eqv to gain=16 in srch2)*/
#define COH_TRUNC_3        0x2 /* truncate by 3 bit(eqv to gain=8 in srch2) */
#define COH_TRUNC_4        0x3 /* truncate by 4 bit(eqv to gain=4 in srch2) */
#define COH_TRUNC_5        0x4 /* truncate by 5 bit(eqv to gain=2 in srch2) */
#define COH_TRUNC_6        0x5 /* truncate by 6 bit                         */
#define COH_TRUNC_7        0x6 /* truncate by 7 bit                         */
#define COH_TRUNC_8        0x7 /* truncate by 8 bit                         */

/* estimated search time constants (experimentally found) in us */
#define SRCH4_SW_QUEUE_OVHD_US  187     /* SW overhead for a queue (any siz) */
#define SRCH4_SW_TASK_OVHD_US    30     /* SW overhead for a task            */


#define SRCH4_TD_PROXIMITY        6     /* maximum proximity between common
                                           and diversity search Indexes    */

#define SRCH4_RD_PROXIMITY        6     /* maximum proximity between common
                                           and diversity search Indexes    */

/* searcher 4 error return values */
enum {
  SRCH4_RET_FULL             =  -1,     /* queue is full                 */
  SRCH4_RET_ACTIVE           =  -2,     /* queue is active               */
  SRCH4_RET_UNCLEAN          =  -3,     /* last search wasn't cleaned up */
  SRCH4_RET_INVALID_QUEUE    =  -4,     /* no queue has been assigned    */
  SRCH4_RET_INVALID_PARAM    =  -5,     /* bad parameter was passed
                                           (null ptr)                    */
  SRCH4_RET_TECH_MISMATCH    =  -6,     /* technology mismatch           */
  SRCH4_RET_NO_QUEUES        =  -7,     /* no queues were avail for srch */
  SRCH4_RET_APP_NOT_VALID    =  -8,     /* MDSP app was not valid        */
  SRCH4_RET_FATAL            =  -9,     /* fatal errors                  */
  SRCH4_RET_FREE             =  -10,    /* HPQ0/LPQ/HPQ1 is inactive or empty    */
  SRCH4_RET_OK               =  -11,     /* no errors to report           */
};

/* retain old type for gps/aflt */
typedef int32 srch4_err_ret_values_type;

/*===========================================================================

         S R C H 4    M S G R    M A C R O S

===========================================================================*/
#define MSGR_MODULE_SRCH_SW       0x02


#define MSGR_C2K_SRCH_SW          MSGR_TECH_MODULE( MSGR_TECH_C2K,       \
                                                    MSGR_MODULE_SRCH_SW)


/*===========================================================================

           M D S P    A P P    R E L A T E D    S T R U C T S

===========================================================================*/
/* bit masks for technologies */
typedef enum
{
  SRCH4_NO_TECH_MASK    = 0x0,
  SRCH4_1X_MASK         = 0x1,
  SRCH4_HDR_MASK        = 0x2,
  SRCH4_AFLT_MASK       = 0x4,
  SRCH4_GPS_MASK        = 0x0,   /* this makes this non GPS dependent */
} srch4_tech_type;

#define SRCH4_MDSP_APP_CB_POOL_SIZE    4

/*===========================================================================

           T E C H N O L O G Y   M A P P I N G    S T R U C T S

===========================================================================*/
/* searcher 4 tech type */
/* the following enum can also be used as a mask, by the way it's designed
   if everything,   then one gets 1x=0, HDRs=1, HDRa=3, AFLT=4, NUM=5 
   if !HDR,         then one gets 1x=0, AFLT=1                , NUM=2
   if !AFLT,        then one gets 1x=0, HDRs=1, HDRa=3        , NUM=4
   if !HDR & !AFLT, then one gets 1x=0,                       , NUM=1


   */
typedef enum
{
  SRCH4_1X              = 0x00,          /* CDMA 1X search             */
  SRCH4_HDR_SHOULDERS,                   /* HDR search interpolation
                                            using shoulders            */
  SRCH4_RESERVED,                        /* not used, but keeps the mask
                                            correct */
  SRCH4_HDR_ACCUM,                       /* HDR search with actual
                                            accumulation energies      */
  SRCH4_AFLT,                            /* AFLT search (used to diff
                                            between 1x and AFLT srch,
                                            MDSP want's 0, but all but
                                            2 LSB bits are masked off) */
  SRCH4_NUM_TECH,                        /* number of technologies     */
  SRCH4_NO_TECH         = 0x08,          /* mask for no tech           */
   /* Masks to differentiate between normal 1x and offtl searching */
  SRCH4_1X_NORM_MASK  = 0x10,           
  SRCH4_1X_OFFTL_MASK = 0x20,
} srch4_srch_type;

/*===========================================================================

           S E A R C H    P R I O R I T Y    E N U M

===========================================================================*/
/* searcher 4 priority */
/* this list was derived from a MSM6500 memo on search scenarios */
/* the "real" priority of each scenario has been shifted left and or'd
   with it's technology type to differentiate priorities between
   technologies */
typedef enum
{
  SRCH4_PRIO_MIN                  = 0,
  SRCH4_PRIO_AFLT                 = 1,
  SRCH4_PRIO_HDR_ACQ              = 3,
  SRCH4_PRIO_1X_ACQ               = 3,
  SRCH4_PRIO_HDR_TRAFFIC          = 4,
  SRCH4_PRIO_HDR_NON_SLOTTED      = 4,
  SRCH4_PRIO_1X_TRAFFIC           = 4,
  SRCH4_PRIO_1X_NON_SLOTTED       = 4,
  SRCH4_PRIO_HDR_TRAFFIC_CFS      = 5,
  SRCH4_PRIO_HDR_OFREQ            = 5,
  SRCH4_PRIO_1X_TRAFFIC_CFS       = 5,
  SRCH4_PRIO_1X_OFREQ             = 5,
  SRCH4_PRIO_HDR_REACQ_BROADCAST  = 6,
  SRCH4_PRIO_1X_REACQ_BROADCAST   = 7,
  SRCH4_PRIO_HDR_REACQ_PAGING     = 8,
  SRCH4_PRIO_1X_REACQ_PAGING      = 9,
  SRCH4_PRIO_MAX                  = 10       /* note: this is the max value */
} srch4_priority_type;

/*===========================================================================

           S E A R C H    M O D E    E N U M

===========================================================================*/
/* type of search */
typedef enum 
{
  SRCH4_MODE_DEFAULT,
  SRCH4_MODE_HDR_TRK_PILOTS,
  SRCH4_MODE_IRAT,
  SRCH4_DSDA_OFFLINE
} srch4_mode_type;

#endif /* SRCH4TRANS_T_H */
