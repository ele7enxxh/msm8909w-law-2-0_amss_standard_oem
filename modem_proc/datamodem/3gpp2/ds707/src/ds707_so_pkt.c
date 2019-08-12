/*===========================================================================

                          D S 7 0 7 _ P K T _ S O
GENERAL DESCRIPTION
  This file contains the service option configuration for packet data calls
  in a 1x CDMA system.  Each service option has a configuration associated
  with it, which tells MC/SNM things such as:
    
    - which RC's this SO can work on
    - what mux option this rateset is valid with
    - alternate service options 

  In addition, this file contains two tables.  The first table defines when
  a particular service option is enabled/disabled.  The deciding factors
  include things such as system p_rev, the QCSO setting, & desired rateset.
  As these factors change, each service option becomes either enabled or 
  disabled.  This state must be updated with SNM as it changes.
  
  The second table indicates which packet service option the mobile should
  originate with, based on factors similar to those in the first table.
  
EXTERNALIZED FUNCTIONS
  DS707_SO_PKT_RECAL
    Updates the enabled/disabled state with SNM.  Called when any of the
    deciding factors is changed.
    
  DS707_SO_PKT_INIT
    Called at startup.  Registers each packet SO with SNM.  Then tells SNM
    which SO's are (currently)_enabled and disabled.
    
  DS707_SO_PKT_GET_ORIG
    Returns the packet SO to originate with.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so_pkt_init() at startup.

 Copyright (c) 2002-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_pkt.c_v   1.10   21 Jan 2003 22:26:06   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_so_pkt.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/05/14    sd     Added changes to use new api to fetch p_rev value
06/03/13    sd     ATCoP decoupling changes
07/11/12    sb     Fixed KW Errors
07/22/11    dvk    Global Variable Cleanup
02/11/11    ack    Global Variable Cleanup
11/22/10    sn     Replaced REX premption macros with critical section for 
                   mutual exclusion.
11/09/10    ms     Added support for RCs 11 and 8 in packet SOs.
10/13/10    op     Migrated to MSG 2.0 macros
07/08/10    ash    Fixed compiler warnings.
03/11/10    sn     Added changes to disable MDR SOs 22, 25 under 
                   FEATURE_DATA_DISABLE_IS95B_MDR.
02/02/10    ms     Fixed Klocwork Errors.
11/02/09    as     Fixed ARM Compiler warning - included header
02/09/09    ss     Off Target Lint Fix
01/23/09    sn     Fixed incorrect setting of SO33 radio config for JCDMA.
12/12/08    ms     Lint fix
02/21/08    mga    Made changes to mainline FEATURE_SNM_ENHANCEMENTS
08/27/07    mga    Fixed High Lint Error
05/10/07    ac     Lint fix
09/08/04    fc     Service negotiation changes for DV.
10/28/03    ak     On pkt_recal, now also set page_rsp for JCDMA.
09/01/03    ak     Mainlined IS2000_RS2 and updates for dynamic feature.
01/21/02    ak     Moved sr_id allocation to pkt_mgr.
01/02/02    ak     For JCDMA, preferred FWD RC for SO 33 is RC 4.
12/30/02    sy     Added changes to support allocation of SR_ID by DS
                   during power-up time.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/13/02    ak     Updated for chips which don't support IS95B MDR.  Also
                   added CAI_RS1_ALL under JCDMA for SO 7.
10/10/02    ak     Updated to use M51 tables and apprate (JCDMA)
09/30/02    atp    Added support for Release-A.
09/24/02    ak     When MDR = 0 (MDR ONLY) and P_REV = 3 (no MDR), still
                   allow LSPD pkt data options.
07/24/02    ak     When MDR = 0 , and P_REV = 3, then have invalid SO
                   situation.  Mark it as such in orig_tbl.
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cai.h"
#include "data_msg.h"
#include "dec5000.h"
#include "ds707.h"
#include "ds707_p_rev.h"
#include "ds707_so_pkt.h"
#include "ds707_socm.h"
#include "dsat707extctab.h"
#include "dsat707vendctab.h"
#include "err.h"
#include "snm.h"
#include "mc.h"
#include "mccap.h"
#include "snm.h"

#include "ds707_so_jcdma.h"
#include "ds707_jcdma_apprate.h"
#include "ds707_jcdma_m51.h"

#include "ds_1x_profile.h"
#include "ds707_roaming.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "dsutil.h"
#include "dsat707mip.h"
#include "ds_1x_profile.h"

/*===========================================================================
                     DECLARATIONS & DEFINITIONS
===========================================================================*/
/*---------------------------------------------------------------------------
  Assuming 18 items in the sch_mo_list, this is what the array would have
  if no SCH's are supported.
---------------------------------------------------------------------------*/
#define DS707_NO_SCHS  (word)0, (word)0, (word)0, (word)0, \
                       (word)0, (word)0, (word)0, (word)0, \
                       (word)0, (word)0, (word)0, (word)0, \
                       (word)0, (word)0, (word)0, (word)0, \
                       (word)0, (word)0
                       
/*===========================================================================
                                TYPEDEFS
===========================================================================*/
typedef struct
{
  const snm_config_type   *default_cfg;        /* Default configuration    */
  const snm_neg_par_type  *neg_par;            /* Negotiation parameters   */
} ds707_so_pkt_param_type;

/*===========================================================================
                       PACKET SERVICE OPTIONS
===========================================================================*/
/*---------------------------------------------------------------------------
  List of all the defined Packet Data service options.
  Note that the order in which the elements are placed in this table should 
  be maintained for the SNM to bring up the packet calls with correct 
  Service Options.
---------------------------------------------------------------------------*/
static const ds707_so_type ds707_so_pkt_list[] =
{
  CAI_SO_PPP_PKT_DATA_3G,             /* SO 33     = 1x data               */
  CAI_SO_MDR_PKT_DATA_FRS2_RRS2,      /* SO 25     = MDR, RS2 fwd, RS2 rev */
  CAI_SO_MDR_PKT_DATA_FRS1_RRS1,      /* SO 22     = MDR, RS1 fwd, RS1 rev */
  CAI_SO_PPP_PKT_DATA_13K_PROPTRY,    /* SO 0x8020 = QC Proprietary, RS2   */
  CAI_SO_PPP_PKT_DATA_13K_PRE707,     /* SO 15     = Pre-707 & IS-707, RS2 */
  CAI_SO_PPP_PKT_DATA_IS707,          /* SO 4103   = IS-707, RS1           */
  CAI_SO_PPP_PKT_DATA_PRE707          /* SO 7      = Pre-707, RS1          */
};


/*---------------------------------------------------------------------------
  Table of enabled service options, dependent on various settings within
  the phone and also on the base station p_rev.
---------------------------------------------------------------------------*/
LOCAL const boolean ds707_so_pkt_tbl[DS707_NUM_P_REVS]
                                    [NUM_MDR_MODES]
                                    [DS_SO_SET_COUNT]
                                    [DS707_SO_PKT_NUM] =
{
#ifdef FEATURE_IS95B_MDR

#ifndef FEATURE_DATA_DISABLE_IS95B_MDR
/*===========================================================================
  P _ R E V = 3  (Lowspeed data, RLP 1)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */ { { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },

/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },      
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } } },
/*===========================================================================
  P _ R E V = 5 (MDR, RLP 2)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */ { { { FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 1 */     { FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 2 */     { FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE,  TRUE,  TRUE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE,  TRUE,  TRUE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE,  TRUE,  TRUE, FALSE, TRUE,  TRUE,  FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE,  TRUE,  TRUE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE,  TRUE,  TRUE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE,  TRUE,  TRUE, FALSE, TRUE,  TRUE,  FALSE } } },   
/*===========================================================================
  P _ R E V = 6 (3G/1x, RLP 3)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */ { { { FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 1 */     { FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 2 */     { FALSE,  TRUE,  TRUE, FALSE, FALSE, FALSE, FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE,  TRUE,  TRUE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE,  TRUE,  TRUE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE,  TRUE,  TRUE, FALSE, TRUE,  TRUE,  FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */   { { TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE } } } 

#else /* FEATURE_DATA_DISABLE_IS95B_MDR */
/*----------------------------------------------------------------------------
  MDR SOs 22, 25 are disabled as 1x firmware support for MDR has been removed.
----------------------------------------------------------------------------*/

/*===========================================================================
  P _ R E V = 3  (Lowspeed data, RLP 1)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */ { { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } } },
/*===========================================================================
  P _ R E V = 5 (MDR, RLP 2)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,    7         */
/* QCSO = 0 */ { { { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 1 */     { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } } },   
/*===========================================================================
  P _ R E V = 6 (3G/1x, RLP 3)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */ { { { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 1 */     { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE },   
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                     33,    25    22,   x8020,  15    4103,   7          */
/* QCSO = 0 */   { { TRUE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { TRUE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { TRUE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } } } 
#endif /* !FEATURE_DATA_DISABLE_IS95B_MDR */

#else  /* FEATURE_IS95B_MDR */
/*===========================================================================
  P _ R E V = 3  (Lowspeed data, RLP 1)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */ { { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } } },    
/*===========================================================================
  P _ R E V = 5 (MDR, RLP 2)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */ { { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE,  FALSE,  FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE,  FALSE,  FALSE, FALSE, TRUE,  TRUE,  FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                    33,    25     22,  x8020,  15     4103,   7          */
/* QCSO = 0 */   { { FALSE,  FALSE,  FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE,  FALSE,  FALSE, FALSE, TRUE,  TRUE,  FALSE } } },   
/*===========================================================================
  P _ R E V = 6 (3G/1x, RLP 3)
===========================================================================*/
/*---------------------------------------------------------------------------
                 M D R = 0 ( MDR ONLY )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */ { { { FALSE,  FALSE,  FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE,  FALSE,  FALSE, FALSE, TRUE,  TRUE,  FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 1 ( MDR IF AVAIL )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE,  FALSE,  FALSE, FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { FALSE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { FALSE,  FALSE,  FALSE, FALSE, TRUE,  TRUE,  FALSE } },   
/*---------------------------------------------------------------------------
                 M D R = 2 ( NO MDR )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE  },    
/* QCSO = 1 */     { FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE  },    
/* QCSO = 2 */     { FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE } },    
/*---------------------------------------------------------------------------
                 M D R = 3 ( SO 33 PREF )
---------------------------------------------------------------------------*/
/*                     33,    25     22,  x8020,  15     4103,   7         */
/* QCSO = 0 */   { { TRUE,  FALSE,  FALSE,  FALSE, TRUE,  FALSE, TRUE  },   
/* QCSO = 1 */     { TRUE,  FALSE,  FALSE,  TRUE,  FALSE, FALSE, TRUE  },   
/* QCSO = 2 */     { TRUE,  FALSE,  FALSE,  FALSE, TRUE,  TRUE,  FALSE } } }
#endif /* FEATURE_IS95B_MDR */
};


/*---------------------------------------------------------------------------
  Table of originating service options, dependent on various phone params
  as well as the BS p_rev.
---------------------------------------------------------------------------*/
LOCAL const ds707_so_type ds707_so_pkt_orig_tbl[DS707_NUM_P_REVS]
                                               [NUM_MDR_MODES]
                                               [DS_SO_SET_COUNT]
                                               [DS_RATESET_COUNT] = 
{
#ifdef FEATURE_IS95B_MDR 
  /* PREV = 3                                                                        */
  /*           MDR = 0                                                               */
  /*           RATE SET 1                         RATE SET 2                         */
  { { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },   /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },   /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } }, /*QCSO = 2*/
  
  /*           MDR = 1                                                               */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },   /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },   /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } }, /*QCSO = 2*/
  
  /*           MDR = 2                                                               */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },   /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },   /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } }, /*QCSO = 2*/

  /*           MDR = 3                                                                 */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },     /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },     /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } } }, /*QCSO = 2*/

  /* PREV = 5                                                                        */
  /*           MDR = 0                                                               */
  { { { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 0*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 1*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 } },  /*QCSO = 2*/

  /*           MDR = 1                                                               */
    { { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 0*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 1*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 } },  /*QCSO = 2*/

  /*           MDR = 2                                                               */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },   /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },   /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } }, /*QCSO = 2*/

  /*           MDR = 3                                                                 */
    { { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },      /*QCSO = 0*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },      /*QCSO = 1*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 } } },  /*QCSO = 2*/

  /* PREV = 6                                                                        */
  /*           MDR = 0                                                               */
  { { { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 0*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 1*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 } },  /*QCSO = 2*/

  /*           MDR = 1                                                               */
    { { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 0*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 },    /*QCSO = 1*/
      { CAI_SO_MDR_PKT_DATA_FRS1_RRS1, CAI_SO_MDR_PKT_DATA_FRS2_RRS2 } },  /*QCSO = 2*/

  /*           MDR = 2                                                                */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 3                                                             */
    { { CAI_SO_PPP_PKT_DATA_3G,       CAI_SO_PPP_PKT_DATA_3G },          /*QCSO = 0*/
      { CAI_SO_PPP_PKT_DATA_3G,       CAI_SO_PPP_PKT_DATA_3G },          /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_3G,       CAI_SO_PPP_PKT_DATA_3G } } }       /*QCSO = 2*/

#else  /* FEATURE_IS95B_MDR */
  /* PREV = 3                                                                         */
  /*           MDR = 0                                                                */
  /*           RATE SET 1                         RATE SET 2                          */
  { { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/
  
  /*           MDR = 1                                                                */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/
  
  /*           MDR = 2                                                                */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 3                                                                 */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },     /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },     /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } } }, /*QCSO = 2*/

  /* PREV = 5                                                                         */
  /*           MDR = 0                                                                */
  { { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 1                                                                */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 2                                                                */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 3                                                                  */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },      /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },      /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } } },  /*QCSO = 2*/

  /* PREV = 6                                                                         */
  /*           MDR = 0                                                                */
  { { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 1                                                                */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/ 
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 2                                                                */
    { { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PRE707  },    /*QCSO = 0*/
      { CAI_SO_PPP_PKT_DATA_PRE707,   CAI_SO_PPP_PKT_DATA_13K_PROPTRY },    /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_IS707,    CAI_SO_PPP_PKT_DATA_13K_PRE707  } },  /*QCSO = 2*/

  /*           MDR = 3                                                                */
    { { CAI_SO_PPP_PKT_DATA_3G,       CAI_SO_PPP_PKT_DATA_3G },             /*QCSO = 0*/
      { CAI_SO_PPP_PKT_DATA_3G,       CAI_SO_PPP_PKT_DATA_3G },             /*QCSO = 1*/
      { CAI_SO_PPP_PKT_DATA_3G,       CAI_SO_PPP_PKT_DATA_3G } } }          /*QCSO = 2*/
#endif /* FEATURE_IS95B_MDR */
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Packet Data Service Options                                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
  Definitions for Service Option 7, 9.6 kbps Packet Data (IS-657)
---------------------------------------------------------------------------*/
LOCAL snm_config_type  ds707_so_pkt7_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_pkt7_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2,      /* SO 25                           */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS1,      /* SO 22                           */
    CAI_SO_PPP_PKT_DATA_13K_PROPTRY,    /* SO 0x8020                       */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,     /* SO 15                           */
    CAI_SO_PPP_PKT_DATA_IS707,          /* SO 4103                         */
    CAI_SO_PPP_PKT_DATA_3G,             /* SO 33                           */
    CAI_SO_NULL
  },
  {                                     /* Compatible service options      */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS2,      /* SO 23                           */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS1       /* SO 24                           */
  },
  {CAI_MUX1, CAI_MUX1},                  /* Maximum fwd and rev mux options */
  {
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,
    #endif /* FEATURE_IS2000_1X_ADV */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, rate set 1              */
      #endif /* FEATURE_IS2000_1X_ADV */
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,
    #endif /* FEATURE_IS2000_1X_ADV */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, rate set 1              */
      #endif /* FEATURE_IS2000_1X_ADV */
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      DS707_NO_SCHS
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      DS707_NO_SCHS
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_NON_CDPD_PACKET_DATA
  , CAI_SO_BITMAP_SO_PPP_PKT_DATA_PRE707
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 4103, 9.6 kbps Packet Data (IS-707-A.5)
---------------------------------------------------------------------------*/
LOCAL snm_config_type  ds707_so_pkt4103_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_pkt4103_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2,      /* SO 25                           */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS1,      /* SO 22                           */
    CAI_SO_PPP_PKT_DATA_13K_PROPTRY,    /* SO 0x8020                       */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,     /* SO 15                           */
    CAI_SO_PPP_PKT_DATA_PRE707,         /* SO 7                            */
    CAI_SO_PPP_PKT_DATA_3G,             /* SO 33                           */
    CAI_SO_NULL
  },
  {                                     /* Compatible service options      */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS2,      /* SO 23                           */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS1       /* SO 24                           */
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,
    #endif /* FEATURE_IS2000_1X_ADV */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, rate set 1              */
      #endif /* FEATURE_IS2000_1X_ADV */
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,
    #endif /* FEATURE_IS2000_1X_ADV */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, rate set 1              */
      #endif /* FEATURE_IS2000_1X_ADV */
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      DS707_NO_SCHS
    }
  },
  {                                       
    (byte)0,                              /* # of mux options for rev sch  */
    {
      DS707_NO_SCHS
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_NON_CDPD_PACKET_DATA
  , CAI_SO_BITMAP_SO_PPP_PKT_DATA_IS707
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 15, 14.4 kbps Packet Data (IS-707-A.5).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_pkt15_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_pkt15_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2,      /* SO 25                           */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS1,      /* SO 22                           */
    CAI_SO_PPP_PKT_DATA_13K_PROPTRY,    /* SO 0x8020                       */
    CAI_SO_PPP_PKT_DATA_IS707,          /* SO 4103                         */
    CAI_SO_PPP_PKT_DATA_PRE707,         /* SO 7                            */
    CAI_SO_PPP_PKT_DATA_3G,             /* SO 33                           */
    CAI_SO_NULL
  },
  {                                     /* Compatible service options      */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS2,
    CAI_SO_MDR_PKT_DATA_FRS2_RRS1
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, 14,400 bps             */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* 1X, RS = 2                      */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      DS707_NO_SCHS
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      DS707_NO_SCHS
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_NON_CDPD_PACKET_DATA
  , CAI_SO_BITMAP_SO_PPP_PKT_DATA_13K_PRE707
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 0x8020, 14.4 kbps Packet Data (Proprietary).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_pkt8020h_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_pkt8020h_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2,      /* SO 25                           */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS1,      /* SO 22                           */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,     /* SO 15                           */
    CAI_SO_PPP_PKT_DATA_IS707,          /* SO 4103                         */
    CAI_SO_PPP_PKT_DATA_PRE707,         /* SO 7                            */
    CAI_SO_PPP_PKT_DATA_3G,             /* SO 33                           */
    CAI_SO_NULL
  },
  {                                     /* Compatible service options      */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS2,      /* SO 23                           */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS1       /* SO 24                           */
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, 14,400 bps             */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* 1X, RS = 2                      */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      DS707_NO_SCHS
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      DS707_NO_SCHS
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 22, High Speed Packet Data (IS-707-A.9).
  Provides speeds upto 8 x 9.6 = 76.8 kbps in each direction. Note: MSM3000
  supports upto 76.8 kbps on the forward link and 9.6 kbps on the reverse
  link.
---------------------------------------------------------------------------*/
LOCAL snm_config_type  ds707_so_pkt22_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX7,                      /* Default forward multiplex option = 7 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_pkt22_neg_par = 
{
  4,                                    /* Minimum PREV allowed = 4        */
  {                                     /* Alternate service options       */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2,      /* SO 25                           */
    CAI_SO_PPP_PKT_DATA_13K_PROPTRY,    /* SO 0x8020                       */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,     /* SO 15                           */
    CAI_SO_PPP_PKT_DATA_IS707,          /* SO 4103                         */
    CAI_SO_PPP_PKT_DATA_PRE707,         /* SO 7                            */
    CAI_SO_PPP_PKT_DATA_3G,             /* SO 33                           */
    CAI_SO_NULL
  },
  {                                     /* Compatible service options      */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS2,      /* SO 23                           */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS1       /* SO 24                           */
  },
  {CAI_MAX_USABLE_MUX_RS1, CAI_MUX1}    /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    2,                                  /* number of rc's in the list      */
    #else
    1,
    #endif /* FEATURE_IS2000_1X_ADV */
    {                                   /* forward radio channel           */
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, rate set 1              */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    2,                                  /* number of rc's in the list      */
    #else
    1,
    #endif /* FEATURE_IS2000_1X_ADV */
    {
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, rate set 1              */
      #endif /* FEATURE_IS2000_1X_ADV */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      DS707_NO_SCHS
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      DS707_NO_SCHS
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_NON_CDPD_PACKET_DATA
  , CAI_SO_BITMAP_SO_MDR_PKT_DATA_FRS1_RRS1
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 25, High Speed Packet Data (IS-707-A.9).
  Provides speeds upto 8 x 14.4 = 115.2 kbps in each direction. Note: MSM3000
  supports upto 86.4 kbps (= 6 x 14.4 kbps) on the forward link and 14.4 kbps
  on the reverse link.
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_pkt25_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX8,                      /* Default forward multiplex option = 8 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_pkt25_neg_par = 
{
  4,                                    /* Minimum PREV allowed = 4        */
  {                                     /* Alternate service options       */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS1,      /* SO 22                           */
    CAI_SO_PPP_PKT_DATA_13K_PROPTRY,    /* SO 0x8020                       */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,     /* SO 15                           */
    CAI_SO_PPP_PKT_DATA_IS707,          /* SO 4103                         */
    CAI_SO_PPP_PKT_DATA_PRE707,         /* SO 7                            */
    CAI_SO_PPP_PKT_DATA_3G,             /* SO 33                           */
    CAI_SO_NULL
  },
  {                                     /* Compatible service options      */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS2,      /* SO 23                           */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS1       /* SO 24                           */
  },
  {CAI_MAX_USABLE_MUX_RS2, CAI_MUX2}    /* Maximum fwd and rev mux options */
 ,{
    1,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_2,                         /* IS-95, RS = 2                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    1,                                  /* number of rc's in the list      */
    {
      CAI_RC_2,                         /* IS-95, RS = 2                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      DS707_NO_SCHS
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      DS707_NO_SCHS
    }
  }
  , {SNM_FCH, SNM_RESERVED, SNM_RESERVED}

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_NON_CDPD_PACKET_DATA
  , CAI_SO_BITMAP_SO_MDR_PKT_DATA_FRS2_RRS2
#endif /* FEATURE_IS2000_REL_A */
};

/*---------------------------------------------------------------------------
                                    SO 33
  Definitions for Service Option 33, High Speed Packet Data (IS-707-A.10) 
  via 1X. Provides speeds upto 153 kbps in each direction.
---------------------------------------------------------------------------*/

#define DS707_SO_PKT33_MAX_SCH_MO_LIST_LEN 16
/*---------------------------------------------------------------------------
  Array of SCH MO's supported by MSM's at a minimum.  RS2 16X and 32X may
  also be supported.  Those are put into the so33_neg_par table in the
  code, at initialization.
---------------------------------------------------------------------------*/
LOCAL const word ds707_so_pkt33_sch_mo_list[] = 
{
  (word)CAI_MUX_16X_RS1_DOUBLE,      /* RS1 MuxPDU 3, 16x, double sch */

  (word)CAI_MUX_8X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  8x, double sch */
  (word)CAI_MUX_8X_RS2_SINGLE,       /* RS2 MuxPDU 3,  8x, single sch */
  (word)CAI_MUX_8X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  8x, double sch */
  (word)CAI_MUX_8X_RS1_SINGLE,       /* RS1 MuxPDU 3,  8x, single sch */

  (word)CAI_MUX_4X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  4x, double sch */
  (word)CAI_MUX_4X_RS2_SINGLE,       /* RS2 MuxPDU 3,  4x, single sch */
  (word)CAI_MUX_4X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  4x, double sch */
  (word)CAI_MUX_4X_RS1_SINGLE,       /* RS1 MuxPDU 3,  4x, single sch */

  (word)CAI_MUX_2X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  2x, double sch */
  (word)CAI_MUX_2X_RS2_SINGLE,       /* RS2 MuxPDU 3,  2x, single sch */
  (word)CAI_MUX_2X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  2x, double sch */
  (word)CAI_MUX_2X_RS1_SINGLE,       /* RS1 MuxPDU 3,  2x, single sch */

  (word)CAI_MUX_1X_RS2,              /* RS2 MuxPDU 2,  1x, single sch */
  (word)CAI_MUX_1X_RS1,              /* RS1 MuxPDU 1,  1x, single sch */

  (word)0                            /* list terminator               */
};


/*---------------------------------------------------------------------------
  NOTE: This table is not used. Hence it was not updated for the RS2
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_pkt33_default_cfg =
{
  {
    /* Forward link defaults */
    1,                             /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All transmission rates used          */
    CAI_PRI_OR_SEC_TRAF            /* Primary or secondary traffic okay    */
  },
  {
    /* Reverse link defaults */
    1    ,                         /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All transmission rates used          */
    CAI_PRI_OR_SEC_TRAF            /* Primary or secondary traffic okay    */
  }
};

LOCAL snm_neg_par_type  ds707_so_pkt33_neg_par = 
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS2,      /* SO 25                           */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS1,      /* SO 22                           */
    CAI_SO_PPP_PKT_DATA_13K_PROPTRY,    /* SO 0x8020                       */
    CAI_SO_PPP_PKT_DATA_13K_PRE707,     /* SO 15                           */
    CAI_SO_PPP_PKT_DATA_IS707,          /* SO 4103                         */
    CAI_SO_PPP_PKT_DATA_PRE707,         /* SO 7                            */
    CAI_SO_NULL
  },
  {                                     /* Compatible service options      */
    CAI_SO_MDR_PKT_DATA_FRS1_RRS2,      /* SO 23                           */
    CAI_SO_MDR_PKT_DATA_FRS2_RRS1       /* SO 24                           */
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD}  /* Maximum fwd and rev mux options */

 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,
    #endif /* FEATURE_IS2000_1X_ADV */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* 1x phys channel (preferred)     */ 
      CAI_RC_4,                         /* 1x phys channel                 */ 
      CAI_RC_5,                          /* 1x Rate set 2                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, rate set 1              */
      #endif /* FEATURE_IS2000_1X_ADV */
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,
    #endif /* FEATURE_IS2000_1X_ADV */
    {
      CAI_RC_3,                         /* 1x phys channel                 */ 
      CAI_RC_4,                         /* 1x phys channel for RS2         */ 
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, rate set 1              */
      #endif /* FEATURE_IS2000_1X_ADV */
      CAI_INIT_RC,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      DS707_NO_SCHS                       /* struct will be filled out at */
    }                                     /* initiatlization              */
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      DS707_NO_SCHS                       /* struct will be filled out at */
    }                                     /* initiatlization              */
  }
#ifdef FEATURE_IS2000_REL_C_DV
  #error code not present
#else
  , {SNM_DCCH, SNM_FCH, SNM_RESERVED} 
#endif /* FEATURE_IS2000_REL_C_DV */

#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_NON_CDPD_PACKET_DATA
  , CAI_SO_BITMAP_SO_PPP_PKT_DATA_3G
#endif /* FEATURE_IS2000_REL_A */
};


/*---------------------------------------------------------------------------
  Array to store the default config and negotiation parameter data structures
  for each packet data service option (from the pkt_so_list[]).
  NOTE: The order and number of entries in this table must correspond to the
        order in which the service options are listed in the pkt_so_list[],
        and the number of entries in the pkt_so_list[], respectively.
---------------------------------------------------------------------------*/
LOCAL ds707_so_pkt_param_type ds707_so_pkt_param[DS707_SO_PKT_NUM] =
{
  { &ds707_so_pkt33_default_cfg,    &ds707_so_pkt33_neg_par },   /* 33     */
  { &ds707_so_pkt25_default_cfg,    &ds707_so_pkt25_neg_par },   /* 25     */
  { &ds707_so_pkt22_default_cfg,    &ds707_so_pkt22_neg_par },   /* 22     */
  { &ds707_so_pkt8020h_default_cfg, &ds707_so_pkt8020h_neg_par },/* 0x8020 */
  { &ds707_so_pkt15_default_cfg,    &ds707_so_pkt15_neg_par },   /* 15     */
  { &ds707_so_pkt4103_default_cfg,  &ds707_so_pkt4103_neg_par }, /* 4103   */
  { &ds707_so_pkt7_default_cfg,     &ds707_so_pkt7_neg_par }     /*  7     */
};


/*===========================================================================
                       LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_PKTI_SET_SO33_MO_LISTS

DESCRIPTION   Sets the SCH MO list (FWD and REV) for SO 33.  This is
              done at startup as well as when the P_REV changes.
              
              INTLOCK'd, as the list that is changed is registered with
              MC at the same time.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_pkti_set_so33_mo_lists(void)
{
  word i, j; 
  byte sys_p_rev;
  
  // Get MC p_rev
  sys_p_rev = mc_get_p_rev_in_use();  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enter critical section to protect SO33 negotiation parameters data.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_global_crit_sect);

  /*-------------------------------------------------------------------------
                            Zero out both Mux Option Lists
  -------------------------------------------------------------------------*/
  memset(ds707_so_pkt33_neg_par.fwd_sch_mo_list.list,
         0,
         CAI_MAX_MULT_OPT_SCH
        );

  memset(ds707_so_pkt33_neg_par.rev_sch_mo_list.list,
         0,
         CAI_MAX_MULT_OPT_SCH
        );

  /*-------------------------------------------------------------------------
                            Do Fwd SCH MO list.
  -------------------------------------------------------------------------*/
  i = 0;
  j = 0;

  /*-------------------------------------------------------------------------
    Is Mux PDU 5 enabled?
  -------------------------------------------------------------------------*/
  if (sys_p_rev >= 7)
  {
#ifdef FEATURE_IS2000_DYNAMIC_FEATURE
    if (mccap_is_f_sch_32x_supported() == TRUE)
    {
      ds707_so_pkt33_neg_par.fwd_sch_mo_list.list[i] = CAI_MUX_VARIABLE;
      i++;
    }
#elif defined(FEATURE_IS2000_REL_A)
    ds707_so_pkt33_neg_par.fwd_sch_mo_list.list[i] = CAI_MUX_VARIABLE;
    i++;
#endif /* FEATURE_IS2000_DYNAMIC_FEATURE */
  }

#ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
  /*-------------------------------------------------------------------------
    RS2 on 16X is enabled
  -------------------------------------------------------------------------*/
  ds707_so_pkt33_neg_par.fwd_sch_mo_list.list[i] = CAI_MUX_16X_RS2_DOUBLE;
  i++;
#endif /* FEATURE_DS_IS2000_RS2_16X_F_SCH */

  /*-------------------------------------------------------------------------
    Fill in the rest of the SCH MO list.
  -------------------------------------------------------------------------*/
  while ( j < DS707_SO_PKT33_MAX_SCH_MO_LIST_LEN && i < CAI_MAX_MULT_OPT_SCH )
  {
    ds707_so_pkt33_neg_par.fwd_sch_mo_list.list[i] = 
                                               ds707_so_pkt33_sch_mo_list[j];
    i++;
    j++;
  }

  /*-------------------------------------------------------------------------
    Error check and write in the number of valid mux opts into the struct
  -------------------------------------------------------------------------*/
  if (i > CAI_MAX_MULT_OPT_SCH)
  {
    ERR_FATAL("Bad init - mem overwritten",0,0,0);
  }
  else
  {
    ds707_so_pkt33_neg_par.fwd_sch_mo_list.num_mos = (byte)i;
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Num SO 33 Fwd SCH MOs = %d", i);
  }
  
  /*-------------------------------------------------------------------------
                              Do REV SCH MO list.
  -------------------------------------------------------------------------*/
  i = 0;
  j = 0;

  /*-------------------------------------------------------------------------
    Is Mux PDU 5 enabled?
  -------------------------------------------------------------------------*/
  if (sys_p_rev >= 7)
  {
#ifdef FEATURE_IS2000_DYNAMIC_FEATURE
    if (mccap_is_r_sch_32x_supported() == TRUE)
    {
      ds707_so_pkt33_neg_par.rev_sch_mo_list.list[i] = CAI_MUX_VARIABLE;
      i++;
    }
#elif defined(FEATURE_IS2000_REL_A)
    ds707_so_pkt33_neg_par.rev_sch_mo_list.list[i] = CAI_MUX_VARIABLE;
    i++;
#endif /* FEATURE_IS2000_DYNAMIC_FEATURE */
  }

#ifdef FEATURE_DS_IS2000_RS2_16X_R_SCH
  /*-------------------------------------------------------------------------
    RS2 on 16X is enabled
  -------------------------------------------------------------------------*/
  ds707_so_pkt33_neg_par.rev_sch_mo_list.list[i] = CAI_MUX_16X_RS2_DOUBLE;
  i++;
#endif /* FEATURE_DS_IS2000_RS2_16X_R_SCH */

  /*-------------------------------------------------------------------------
    Fill in the rest of the SCH MO list.
  -------------------------------------------------------------------------*/
  while ( j < DS707_SO_PKT33_MAX_SCH_MO_LIST_LEN && i < CAI_MAX_MULT_OPT_SCH )
  {
    ds707_so_pkt33_neg_par.rev_sch_mo_list.list[i] =
                                               ds707_so_pkt33_sch_mo_list[j];
    i++;
    j++;
  }

  /*-------------------------------------------------------------------------
    Error check and write in the number of valid mux opts into the struct
  -------------------------------------------------------------------------*/
  if (i > CAI_MAX_MULT_OPT_SCH)
  {
    ERR_FATAL("Bad init - mem overwritten",0,0,0);
  }
  else
  {
    ds707_so_pkt33_neg_par.rev_sch_mo_list.num_mos = (byte)i;
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Num SO 33 Rev SCH MOs = %d", i);
  }

  /*-------------------------------------------------------------------------
    Re-register new SCH MO lists with SNM.
  -------------------------------------------------------------------------*/
  (void) snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                       SNM_FORWARD,
                       &ds707_so_pkt33_neg_par.fwd_sch_mo_list);

  (void) snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                       SNM_REVERSE,
                       &ds707_so_pkt33_neg_par.rev_sch_mo_list);

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_global_crit_sect);
} /* ds707_so_pkti_set_so33_mo_lists() */

/*===========================================================================
FUNCTION      DS707_SO_PKTI_SET_SO33_RC

DESCRIPTION   Sets the Radio Config (RC) for SO 33.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_pkti_set_so33_rc(void)
{
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    #ifdef FEATURE_IS2000_1X_ADV
    ds707_so_pkt33_neg_par.fwd_rc.num_of_rc = 4;
    #else
    ds707_so_pkt33_neg_par.fwd_rc.num_of_rc = 3;
    #endif /* FEATURE_IS2000_1X_ADV */
    ds707_so_pkt33_neg_par.fwd_rc.list[0] = CAI_RC_4;
    ds707_so_pkt33_neg_par.fwd_rc.list[1] = CAI_RC_3;
    ds707_so_pkt33_neg_par.fwd_rc.list[2] = CAI_RC_5;
    #ifdef FEATURE_IS2000_1X_ADV
    ds707_so_pkt33_neg_par.fwd_rc.list[3] = CAI_RC_11;
    #endif /* FEATURE_IS2000_1X_ADV */
  }
  else
  {
    #ifdef FEATURE_IS2000_1X_ADV
    ds707_so_pkt33_neg_par.fwd_rc.num_of_rc = 4;
    #else
    ds707_so_pkt33_neg_par.fwd_rc.num_of_rc = 3;
    #endif /* FEATURE_IS2000_1X_ADV */
    ds707_so_pkt33_neg_par.fwd_rc.list[0] = CAI_RC_3;
    ds707_so_pkt33_neg_par.fwd_rc.list[1] = CAI_RC_4;
    ds707_so_pkt33_neg_par.fwd_rc.list[2] = CAI_RC_5;
    #ifdef FEATURE_IS2000_1X_ADV
    ds707_so_pkt33_neg_par.fwd_rc.list[3] = CAI_RC_11;
    #endif /* FEATURE_IS2000_1X_ADV */
  }
} /* ds707_so_pkti_set_so33_rc() */

/*===========================================================================
FUNCTION      DS707_SO_PKT7I_DEFAULT_CFG_INIT

DESCRIPTION   Sets the transmission rates (FWD and REV) for SO 7.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_pkt7i_default_cfg_init( void)
{

 if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
 {
   ds707_so_pkt7_default_cfg.forward.tx_rates = CAI_RS1_ALL;
   ds707_so_pkt7_default_cfg.reverse.tx_rates = CAI_RS1_ALL;
 }
 else
 {
  ds707_so_pkt7_default_cfg.forward.tx_rates =
     CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
  ds707_so_pkt7_default_cfg.reverse.tx_rates = 
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
 }
}

/*===========================================================================
FUNCTION      DS707_SO_PKT4103I_DEFAULT_CFG_INIT

DESCRIPTION   Sets the transmission rates (FWD and REV) for SO 4103.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_pkt4103i_default_cfg_init( void)
{

 if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
 {
   ds707_so_pkt4103_default_cfg.forward.tx_rates = CAI_RS1_ALL;
   ds707_so_pkt4103_default_cfg.reverse.tx_rates = CAI_RS1_ALL;
 }
 else
 {
  ds707_so_pkt4103_default_cfg.forward.tx_rates =
     CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
  ds707_so_pkt4103_default_cfg.reverse.tx_rates = 
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
 }
}

/*===========================================================================
FUNCTION      DS707_SO_PKT22I_DEFAULT_CFG_INIT

DESCRIPTION   Sets the transmission rates (FWD and REV) for SO 22.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_pkt22i_default_cfg_init( void)
{

 if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
 {
   ds707_so_pkt22_default_cfg.forward.tx_rates = CAI_RS1_ALL;
   ds707_so_pkt22_default_cfg.reverse.tx_rates = CAI_RS1_ALL;
 }
 else
 {
  ds707_so_pkt22_default_cfg.forward.tx_rates =
     CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
  ds707_so_pkt22_default_cfg.reverse.tx_rates = 
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200; /* Use Rate 1, 1/2 & 1/8   */
 }
}

/*===========================================================================
FUNCTION      DS707_SO_PKTI_REGISTER

DESCRIPTION   Register each defined packet data service option (from the
              pkt_so_list[]) with the Service Negotiation Manager, and check
              that the registration was successful.

              Note that the order in which the service options are added is
              important.  When searching for compatible service options, the
              SNM will use the first service option that it finds, therefore
              it is important to add the preferred service options first.

DEPENDENCIES  An instance of snm_config_type (default configuration) and
              snm_neg_par_type (negotiation parameters) should have been
               defined for each service option being registered, and should
              have been initialized with meaningful values prior to calling
              this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_pkti_register(void)
{
  byte  i;
#if (defined(FEATURE_IS95B_MDR) && defined(FEATURE_DS_IS707A_CMUX))
  word  fwd_mux;      /* Needed to set the max fwd supps for MDR SO's only */
  uint8 cmux_val[MAX_CMUX_VAL] = {0};
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_so_pkt7i_default_cfg_init();
  ds707_so_pkt4103i_default_cfg_init();
  ds707_so_pkt22i_default_cfg_init();
  /*-------------------------------------------------------------------------
    Add each service option in the pkt_so_list[] to the SNM.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_PKT_NUM; i++ )
  {
    if( !ds707_nv_util_is_refresh_in_progress(ds707_curr_subs_id()))
    {
    if( snm_add_so((word)ds707_so_pkt_list[i],
                   (snm_neg_par_type*)(ds707_so_pkt_param[i].neg_par),
                   ds707_so_pkt_param[i].default_cfg) == SNM_SUCCESS )
    {
      /*---------------------------------------------------------------------
        The service option was added successfully, register the service
        option control message handler (the same handler is used for all
        packet data service options).
      ---------------------------------------------------------------------*/
      (void) snm_set_cont_msg_handler((word)ds707_so_pkt_list[i],
                               ds707_socm_parse_socm);
    }
    else
    {
      /*---------------------------------------------------------------------
        The service option registration failed.
      ---------------------------------------------------------------------*/
      ERR_FATAL("Couldn't add SO %d to SNM", ds707_so_pkt_list[i], 0, 0);
    }
  }
  }

#if (defined(FEATURE_IS95B_MDR) && defined(FEATURE_DS_IS707A_CMUX))
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CMUX_VAL,
                                          (void *)cmux_val,MAX_CMUX_VAL);
  /*-------------------------------------------------------------------------
    Set the maximum forward supplementals for MDR service options, based on
    the AT+CMUX default value.
  -------------------------------------------------------------------------*/
  fwd_mux = dsat707_cmux_fwd_table[(cmux_val[0])];
  
  if( fwd_mux & 0x01 )
  {
    /*-----------------------------------------------------------------------
      Odd multiplex option. Set the maximum number of forward supplementals
      for the Rate Set 1 MDR Packet Data service option (i.e. SO 22).
    -----------------------------------------------------------------------*/
    (void) snm_set_max_for_sups(CAI_SO_MDR_PKT_DATA_FRS1_RRS1, (byte)((fwd_mux-1)/2));
  }
  else
  {
    /*-----------------------------------------------------------------------
      Even multiplex option. Set the maximum number of forward supplementals
      for the Rate Set 2 MDR Packet Data service option (i.e. SO 25).
    -----------------------------------------------------------------------*/
    (void) snm_set_max_for_sups(CAI_SO_MDR_PKT_DATA_FRS2_RRS2, (byte)((fwd_mux-2)/2));
  }
#endif  /* FEATURE_IS95B_MDR && FEATURE_DS_IS707A_CMUX */
} /* ds707_so_pkti_register() */

/*===========================================================================
FUNCTION      DS707_SO_PKTI_SET_PAGE_RSP

DESCRIPTION   Sets the SO to respond with if paged with a particular packet
              service option.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_pkti_set_page_rsp(void)
{
  ds707_so_type        pkt_so;                     /* pkt so from table    */ 
  ds707_so_type        page_rsp_so;                /* page response so     */
  byte                 i;                          /* for...loop invariant */
  word                 p_rev_index;                /* system p_rev         */
  ds707_mip_profile_type     *ds_static_profile_ptr = NULL;
  uint8                       qcmdr_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&qcmdr_val,
                                          DS707_NON_ARRAY);
  p_rev_index = ds707_p_rev_get_index();

  /*-------------------------------------------------------------------------
    Setup the page response service option for each packet data service
    option in the pkt_so_list[], based on system values.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_PKT_NUM; i++ )
  {
    pkt_so = ds707_so_pkt_list[i];

    if (ds707_so_pkt_tbl[p_rev_index]
                        [qcmdr_val]
                        [ds_static_profile_ptr->ds707.ds707_qcso_val]
                        [i] == TRUE)
    {
      page_rsp_so = pkt_so;
    }
    else
    {
       if (DS_ISDATA_RS1(pkt_so))
       {
         page_rsp_so = ds707_so_pkt_orig_tbl
                               [p_rev_index]
                               [qcmdr_val]
                               [ds_static_profile_ptr->ds707.ds707_qcso_val]
                               [DS_RATESET1];
       }
       else
       {
         page_rsp_so = ds707_so_pkt_orig_tbl[p_rev_index]
                               [qcmdr_val]
                               [ds_static_profile_ptr->ds707.ds707_qcso_val]
                               [DS_RATESET2];
       }

       /*--------------------------------------------------------------------
         For MDR, if in MDR-only, then response for non-MDR pages is the
         RS 2 MDR service option (25)
       --------------------------------------------------------------------*/
       if ( (qcmdr_val == DS_MDR_MODE_MDR_ONLY) &&
           (!DS_ISDATA_MDR(pkt_so))
          )
       {
         page_rsp_so = CAI_SO_MDR_PKT_DATA_FRS2_RRS2;
       }
    }

    /*-----------------------------------------------------------------------
      Invoke the SNM function to set the page response service option.
    -----------------------------------------------------------------------*/
    (void) snm_set_page_response_so((word)pkt_so, (word)page_rsp_so);

  }  /* for i = 0 to DS707_SO_PKT_NUM */
} /* ds707_so_pkti_set_page_rsp() */

/*===========================================================================
FUNCTION      DS707_SO_PKTI_SET_STATE

DESCRIPTION   Lookup the state of each packet data service option based on
              the current MDR mode and service option set in use. Enable or
              disable each packet data service option as per the table.

DEPENDENCIES  Uses the global variable ds707_qcmdr_val to determine the
              current setting of the MDR mode.  Uses the global variable 
              ds707_qcso_val to determine the current setting of the 
              service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_pkti_set_state(void)
{
  ds707_so_type        pkt_so;                /* service option from table */
  byte                 i;                     /* loop invariant            */
  word                 p_rev_index;           /* system p_rev      */
  ds707_mip_profile_type     *ds_static_profile_ptr = NULL;
  uint8                       qcmdr_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&qcmdr_val,
                                          DS707_NON_ARRAY);
  p_rev_index = ds707_p_rev_get_index();

  /*-------------------------------------------------------------------------
    Enable or disable each packet data service option, based on the
    appropriate settings.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_PKT_NUM; i++ )
  {
    pkt_so = ds707_so_pkt_list[i];

    if ( ds707_so_pkt_tbl[p_rev_index]
                         [qcmdr_val]
                         [ds_static_profile_ptr->ds707.ds707_qcso_val]
                         [i] == TRUE )
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to enable this service option.
      ---------------------------------------------------------------------*/
      (void) snm_enable_so((word)pkt_so);
    }
    else
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to disable this service option.
      ---------------------------------------------------------------------*/
      (void) snm_disable_so((word)pkt_so);
    }
  } /* for i = 0 to DS707_SO_PKT_NUM */
} /* ds707_so_pkti_set_state() */


/*===========================================================================/
                       EXTERNAL FUNCTION DEFINITIONS
============================================================================*/

/*===========================================================================
FUNCTION       DS707_SO_PKT_RECAL

DESCRIPTION    Re-does the service option page responses and the enable/
               disabling of the service options, based on the current 
               settings.

DEPENDENCIES   This function should be called only after ATCOP has been
               initialized, since it invokes functions that use some AT
               command settings.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_so_pkt_recal(void)
{
  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    ds707_so_jcdma_pkt_set_state();
    ds707_so_jcdma_pkt_set_page_rsp();
    ds707_so_pkti_set_so33_rc();
  }
  else
  {
    ds707_so_pkti_set_page_rsp();
    ds707_so_pkti_set_state();
    ds707_so_pkti_set_so33_rc();
    ds707_so_pkti_set_so33_mo_lists();
  }
}

/*===========================================================================
FUNCTION       DS707_SO_PKT_INIT

DESCRIPTION    Wrapper function for register_pkt_so() and set_pkt_so_state()
               functions, in order to simplify the external interface. Any
               power-up initialization that needs to be performed for packet
               data service options should be done here.

DEPENDENCIES   This function should be called only after ATCOP has been
               initialized, since it invokes functions that use some AT
               command settings.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_so_pkt_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Register all the defined packet data service options with the service
    negotiation manager.
  -------------------------------------------------------------------------*/
  ds707_so_pkti_register();

  /*-------------------------------------------------------------------------
    Set up the page response service option for each packet data service
    option.  Also enable/disable the service options according to the
    current mode.
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();
} /* ds707_so_pkt_init() */


/*===========================================================================
FUNCTION       DS707_SO_PKT_GET_ORIG

DESCRIPTION    Returns the SO to orig with.

DEPENDENCIES   NONE

RETURN VALUE   Service Option to originate with.

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_so_pkt_get_orig
(
  ds707_rateset_type         rateset
)
{
  word  p_rev_index;                                      /* system p_rev  */
  uint8 qcmdr_val = 0;
  ds707_mip_profile_type *ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  p_rev_index = ds707_orig_p_rev_get_index();
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&qcmdr_val,
  	                                       DS707_NON_ARRAY);

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    return(ds707_so_jcdma_pkt_get_orig(rateset));
  }
  else
  {
    return(ds707_so_pkt_orig_tbl[p_rev_index]
                              [qcmdr_val]
                              [ds_1x_runtime_profile_ptr->ds707.ds707_qcso_val]
                                [rateset]);
  }
} /* ds707_so_pkt_get_orig() */

/*===========================================================================
FUNCTION       DS707_GET_SO_PKT_TYPE

DESCRIPTION    Accessor function for Packet Data service option

DEPENDENCIES   NONE

RETURN VALUE   Returns the requested Packet Data service option

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_get_so_pkt_type
(
  byte index
)
{
  return (ds707_so_pkt_list[index]);
} /* ds707_get_so_pkt_type() */

#endif /* FEATURE_DATA_IS707 */

