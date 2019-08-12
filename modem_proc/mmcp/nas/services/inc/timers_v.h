#ifndef TIMERS_V_H
#define TIMERS_V_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/timers.h_v   1.22   03 Jul 2002 12:50:56   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/timers_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/21/11   gps     FREE FLOAT FEATURE changes
08/01/11   rajesh  Added TIMER_T3442 
06/30/11   rajesh    Changes made for implementing PS detach timer in NAS.
08/30/10   MNK     Add T3417_EXT timer
05/17/10   zren    Updated EMM to only process EMM timers
04/13/10   MNK     Add ISR deactivate timer T3423 for LTE
02/03/10   zren    Added ESM new timer machanism
11/30/09   zren    Modified access barring timeout handling
05/29/09   zren    Added support for TAU feature
05/08/09   MNK     Added new ESM timers T3480,T3481,T3482 and T3492
04/21/09   hnam    Added T3440 timer support (required during attach and service reject)
10/29/02   jar     Increased GSDI_READ_TIMER_VALUE from 10 seconds to 180 
                   seconds for GCF Mandatory 11.10 SIM Test Case 27.11.1.3c
04/04/01   jca     Fixed LINT errors.
05/03/01   jca     Added GMM timers.
06/11/01   TS      Added GSM build support.
08/14/01   AB      Renamed connection manager reference to CNM from CM.
08/15/01   AB      Renamed all MMI references to CM.
10/24/01   VT      DTMF: Added TIMER_T336 & TIMER_T337 and removed TIMER_DTMF_ON & TIMER_DTMF_OFF
02/05/02   rmc     Renamed RR cell reselection timers. Added new RR_RESELECTION_SI_TIMER.
02/12/02   jca     Removed timer TIMER_RR_ABORT.
03/05/02   JC      Eliminated legacy L1 timer declarations.
04/19/02   CD      Added CNM_MM_REL_TIMER
04/22/02   CD      Changed value of TR2M expiration from 1000 to 19000
04/30/02   ks      Added TIMER_DETACH_REL definition
05/30/02   JAC     added L2_SEND_DCCH timer
06/02/02   Rao     Getting rid of Pathloss timer and adding 5 Sec Timers for
                   Serving & Surr Cells.
06/16/02   ks      Added timers for GPRS
07/03/02   jca     Deleted timers TIMER_PLMN_SELECT and TIMER_PLMN_LIST.
08/14/02   rmc     Removed RR timer definitions from this file
12/09/03   ks      Setting MIN_MM_TIMER to TIMER_T3210 and added comments.
04/06/04   cd      LCS support. Added TIMER_T3241
08/12/04   ab      Link control support. Added TLC1M.
12/17/04   vdr     Added definitions for TIMER_T3218 & TIMER_T3316
08/08/06   NR      Chaning the SMS_TR1M_TIMEOUT_VALUE & SMS_TR2M_TIMEOUT_VALUE values to 
                   spec compliant values
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "timers.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
 * TIME
 */

#define DECIHOUR                       360000  /* milliseconds */
#define SECOND                         1000

/*
 * Timer Ids.
 ************
 */


#define TIMER_UNDEFINED                ((timer_id_T)0xffffffff)
#define ALL_TIMERS                     0
#define TIMER_T303                     1
#define TIMER_T305                     2
#define TIMER_T308                     3
#define TIMER_T310                     4
#define TIMER_T313                     5
#define TIMER_T323                     6
#define TIMER_T324                     7
#define TIMER_T336                     8
#define TIMER_T337                     9
#define TIMER_TC1M_MO                  10
#define TIMER_TC1M_MT                  11
#define TIMER_RAB_RELEASED             12

/* MM Timers begin Here */

#define TIMER_T3210                    13
#define TIMER_T3211                    14
#define TIMER_T3212                    15
#define TIMER_T3213                    16
#define TIMER_T3220                    17
#define TIMER_T3230                    18
#define TIMER_T3240                    19
#define TIMER_T3246                    20
#define TIMER_MM_REL                   21
#define TIMER_MM_FORBIDDEN_LAI_CLEAR   22
#define TIMER_T3214                    23
#define TIMER_T3216                    24
#define TIMER_T3302                    25
#define TIMER_T3310                    26
#define TIMER_T3311                    27
#define TIMER_T3312                    28
#define TIMER_T3314                    29
#define TIMER_T3317                    30
#define TIMER_T3318                    31
#define TIMER_T3320                    32
#define TIMER_T3321                    33
#define TIMER_T3330                    34
#define TIMER_POWERDOWN                35
#define CNM_MM_REL_TIMER               36
#define TIMER_DETACH_REL               37
#define TIMER_T3241                    38

#define TIMER_T3218                    39
#define TIMER_T3316                    40

#define TIMER_T3319                    41

/*This is network timer which will be started 
 after it has sent RAU accept or Attach accept.
 Startng this timer in UE to take care of not to accept
 Attach accept or RAU accept when this timer is
 not running.*/

#define TIMER_T3350                    42
#define TIMER_T3242                    43
#define TIMER_T3243                    44 
#define TIMER_T3323                    45

#define TIMER_MM_RADIO_RETRY           46
#define TIMER_GMM_RADIO_RETRY          47

#define TIMER_SCRI_PAGING              48

#define TIMER_FOREGROUND_SEARCH        49

#define TIMER_MM_PS_DETACH          50

//network initiated detach timer
#define TIMER_NW_INIT_DETACH           51

#define TIMER_T3340                    52

/* common to MM and EMM */
#define TIMER_T3346                    53

#define TIMER_FORBIDDEN_OPERATOR_CSG_LIST 54
#define TIMER_FORCE_OOS                   55
#define TIMER_FORBID_TDS_RESELECTION      56
#define TIMER_CSMT_GUARD                  57
#define TIMER_PS_CONN_REL                 58


#define TIMER_POLICY_CHANGE_PENDING 61
/* MM Timers END Here */
/* EMM timers */
#define TIMER_T3410                     70
#define TIMER_T3417                     71
#define TIMER_T3421                     72
#define TIMER_T3418                     73
#define TIMER_T3420                     74
#define TIMER_T3416                     75
#define TIMER_T3411                     76
#define TIMER_T3402                     77
#define TIMER_T3412                     78
#define TIMER_T3430                     79
#define TIMER_T3440                     80
#define TIMER_T3423                     81
#define TIMER_T3417_EXT                 82

#define EMM_POWEROFF_DETACH_TIMER       84 
#define EMM_ACCESS_BARRING_TIMER        85
#define TIMER_EMM_PS_DETACH             86
#define EMM_FORBIDDEN_TAI_CLEAR_TIMER   87

#define TIMER_T3442                     88
#define TIMER_EMM_RADIO_RETRY           89
#define TIMER_SRLTE_ESR                 90
#define TIMER_DELAY_TAU                 91

#define MIN_EMM_TIMER                  TIMER_T3410
#define MAX_EMM_TIMER                  TIMER_DELAY_TAU 

/*EMM Timers End here*/

#define MIN_MM_TIMER                  TIMER_T3210

#define MAX_MM_TIMER                  TIMER_DELAY_TAU 

#define MAX_MM_ONLY_TIMER             TIMER_POLICY_CHANGE_PENDING


/* MN Timers */
#define MN_CC_TIMER                    0x40  /* CNM reserved 0-63, MN reserved 64+16+16 */ 
#define MN_SS_TIMER                    0x60
#define MN_SMS_TIMER                   0x70

#define MIN_CNM_TIMER                  1
#define MAX_CC_TIMER                   7     /* does not include DTMF timers */
#define MAX_CNM_TIMER                  63    /* no of actual timers usable   */
                                             /* by CC */
#define MAX_MNCNM_TIMER                127

/* SMS Timers */
#define TLC1M                         0x03
#define TRAM                          0x02
#define TR1M                          0x00
#define TR2M                          0x01

/* For SMS timers, timer_id = MN_SMS_TIMER + TIMER_OFFSET */

/* Offset from MN_SMS_TIMER */
#define TR1M_TIMER_OFFSET             (TR1M)
#define TR2M_TIMER_OFFSET             (TR2M)
#define TRAM_TIMER_OFFSET             (TRAM)
#define TLC1M_TIMER_OFFSET            (TLC1M)

#define SMS_STATUS_TIMER              0x60

/* MN & CNM Timers END here */


/* CM Timers */
#define CM_RSS_TIMER                  0x90
#define CM_DISP_TIMER                 0x91
#define CM_BACK_TIMER                 0x92
#define CM_ACM_TIMER                  0x93
#define CM_ABORT_TIMER                0x94

#define CM_CCM_TIMER_0                0x50
#define CM_CCM_TIMER_1                0x51
#define CM_CCM_TIMER_2                0x52
#define CM_CCM_TIMER_3                0x53
#define CM_CCM_TIMER_4                0x54
#define CM_CCM_TIMER_5                0x55
#define CM_CCM_TIMER_6                0x56
#define CM_CCM_TIMER_8                0x58
#define CM_CCM_TIMER_9                0x59
#define CM_CCM_TIMER_10               0x5a
#define CM_CCM_TIMER_11               0x5b
#define CM_CCM_TIMER_12               0x5c
#define CM_CCM_TIMER_13               0x5d
#define CM_CCM_TIMER_14               0x5e

#define ABORT_TIMER_TIMEOUT            2000



/*
** GHDI TIMERS
*/

#define GS_TIMER_SIM_WWT               0x01
#define GS_TIMER_SIM_POLL              0x02   /* polls for bytes from ISR */

#define SIM_RST_TIME_T1                13     /* ~12 milli-secs */

/*******************************/
/* Timers for the layer 2 task */
/*******************************/


#define SAPI0_TIMEOUT                   200
#define SAPI3_TIMEOUT                   250  /* bigger SAPI3 T200 better on Racal */ 

/*
 * Timeout values for MN SMS timers 
 */

#define SMS_TRAM_TIMEOUT_VALUE         30000 
#define SMS_TR1M_TIMEOUT_VALUE         45000
#define SMS_TR2M_TIMEOUT_VALUE         20000

#define SMS_STATUS_TIMER_TIMEOUT_VALUE 10000

/*
 * Timers for REG
 */

#define REG_HPLMN_TIMER               0x01
#define REG_NO_NETS_TIMER             0x02
#define REG_SEARCH_COUNTDOWN_TIMER    0x03
#ifdef FEATURE_LTE
#define REG_BACKOFF_FPLMN_TIMER     0x04
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define REG_HPLMN_TIMER_SIM_ID_2      0x05
#endif
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
#define REG_HPLMN_TIMER_SIM_ID_3      0x06
#endif

/*
** This timer is used for IRATs to LTE. Refer to FR2164.
*/

#define REG_HPLMN_IRAT_TIMER          0x07
#define REG_LTE_IRAT_TIMER          0x13
#define REG_BACK_OFF_LTE_SEARCH_TIMER 0x14


#define REG_T3245_TIMER               0x08

#ifdef FEATURE_DUAL_SIM
#define REG_T3245_TIMER_SIM_ID_2      0x09
#ifdef FEATURE_TRIPLE_SIM 
#define  REG_T3245_TIMER_SIM_ID_3     0x0A
#endif
#endif

#define REG_VALIDATE_SIM_TIMER        0x0B

#ifdef FEATURE_LTE
#define REG_UPDATE_LTE_CAP_TIMER        0x0C
#endif

#ifdef FEATURE_SGLTE

#define REG_SGLTE_PS_TIMER            0x0D

#endif
#if defined(FEATURE_DUAL_SIM) && defined (FEATURE_LTE)
#define REG_UPDATE_LTE_CAP_TIMER_SIM_ID_2        0x0E
#define REG_BACKOFF_FPLMN_TIMER_SIM_ID_2        0x0F
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define REG_VALIDATE_SIM_TIMER_SIM_ID_2         0x10
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
#define REG_VALIDATE_SIM_TIMER_SIM_ID_3         0x11
#endif
#endif


#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
#define REG_HPLMN_IRAT_TIMER_SIM_ID_2          0x12
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM     
#define REG_HPLMN_IRAT_TIMER_SIM_ID_3          0x17
#endif

#define REG_LTE_IRAT_TIMER_SIM_ID_2            0x15
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM      
#define REG_LTE_IRAT_TIMER_SIM_ID_3            0x16
#endif

#endif

/*
 * Timers for CB
 */
#define CB_TIMER_DUP_DET 0x01 /* timer_id assigned to duplication detection */
#define CB_ETWS_PRIM_DISABLE_CTCH_TIMER    0x02

/*CB_DUP_DET_TIMER
A REL-11 feature timer value =  1 hour for MCC = 440 or 441
timer value = 24 hours otherwise
*/
#define CB_DUP_TIMER_VALUE_1  0x36EE80    /* 1 hour */
#define CB_DUP_TIMER_VALUE_2  0x5265C00    /* 24 hours */
#define CB_DUP_TIMER_VALUE_INVALID  0xffffffffU    /* UINT_MAX */

/*
** After receiving the ETWS primary ind CB will stay in active state for 2 mins in the case
** of Non DRX mode and for 30 mins in DRX mode. If the CB search list consists of only 
** ETWS message IDs , CB has to move to Inactive state upon the expiry of this timer
*/

#define CB_ETWS_PRIM_DISABLE_CTCH_NDRX_TIMEOUT_VALUE  120000/* 2 min*/
#define CB_ETWS_PRIM_DISABLE_CTCH_DRX_TIMEOUT_VALUE  1800000/* 30 min*/

/*
* Timer IDs for data services
*/

#define DS_SYNCHRONISATION_TIMER      0x01
#define DS_CTS_INTEGRATION_ON_TIMER   0x02
#define DS_DCD_INTEGRATION_ON_TIMER   0x03
#define DS_CTS_INTEGRATION_OFF_TIMER  0x04
#define DS_DCD_INTEGRATION_OFF_TIMER  0x05
#define DS_AUTO_ANSWER_TIMER          0x06
#define DS_CISS_TIMER                 0x07
#define DS_RING_REPEAT_TIMER          0x08





/*
 * Value to use for timer value when default time value is required
 */

#define DEFAULT_TIMEOUT                0xffffffffU



typedef struct{

   rex_timer_type *timer;

} timer_info_T;




#endif /* TIMERS_H */
