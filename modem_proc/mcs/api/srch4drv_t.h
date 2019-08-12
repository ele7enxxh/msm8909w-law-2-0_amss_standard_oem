#ifndef SRCH4DRV_T_H
#define SRCH4DRV_T_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       S E A R C H E R   4   D R I V E R    T Y P E D E F    H E A D E R

GENERAL DESCRIPTION
   This file contains the exported structs and defines for searcher 4's
   driver layer.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/srch4drv_t.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/13   sr      Updating Log Packet Version (CR: 497075).
06/03/11   adw     Added carrier 3 for SV operation on NikeL.
04/25/11   vks     Removed featurization from previous check-in. 
04/12/11   smd     Moved search structure from HDR to srch4.
01/21/11   cjb     Moved unused declarations to internal header.
12/02/10   zm      Moved into mcs/api folder - SU API effort.
10/28/10   mca     Added support for diversity search combining methods.
11/11/10   cjb     De-coupled SRCH4_MAX_LPQ_TASKS from FW definition as the 
                   FEATURE_CDMA_AFLT in FW is not accessible to SW.
11/10/10   cjb     Corrected enum to avoid clash with the other #defines.
09/13/10   vks     Add support for AFLT and LPQ.
07/06/10   cjb     Removing redundant declarations of constants.
06/21/10   cjb     Featurization removal for CMMI-P4:
                   -> Removed featurizations from enums
05/28/10   vks     Merge from 1H09:
                   12/03/09   vks     Decouple 1x from SRCH4. Include only
                   srch_srch4.h from 1x and remove other includes. Make
                   appropriate changes.
11/09/09   ag      Added an include modem_mcs_defs.h for 9K builds.
10/26/09   vks     Include the missed header file srchfw_msg.h
09/17/09   vks     Fix lint errors
07/29/09   vks     First set of changes for MSGR implementation
27/03/09   ns      Clean up to support SCMM and 8650 builds only
09/08/08   adw     Added SRCH4_USE_LPQ and SRCH4_TECH_STRING macro.
06/17/08   gs      8650 turn-on changes
04/24/08   sst     Moved srch4 log definations here from srchlog.h
07/11/07   ljl     Supported multi-carriers.
10/23/06   jyw/awj Added structure members for uninterpolated search results.
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
01/16/06   awj     Lint cleanup
08/10/05   ddh     Added SRCH4_MIN_Q = 0 in srch4_queue_type
07/13/04   sst     Added SRCH4_NO_QUEUE = SRCH4_MAX_Q in srch4_queue_type
05/28/04   jcm     Sector module integration
05/04/04   sst     Merge from srch4 3Q dev branch
05/04/04   sst     Removed include search_int.h
05/03/04   sst     Moved COH_TRUNC_* defines to srch4trans_t.h to be exported
05/03/04   sst     Peer Review Comments
04/26/04   sst     Changed SRCH4DRV_RET_FREE to be queue specific
04/15/04   sst     More code cleanup
04/12/04   sst     Removed debug include file
04/08/04   sst     Code cleanup
                   Mainlined FEATURE_SRCH4_MDSP2
                   Created SRCH4_HAS_HPQ1, SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
                   Created SRCH4_DEBUG, SRCH4_DEBUG_MSG()
04/06/04   sst     Updated some featurization
03/24/04   sst     Added extern for srch4_prio_str
03/02/04   sst     Use of MDSP version of num of peaks
02/26/04   sst     File formating
01/06/03   sst     Added field age_search to srch4_task_struct_type
12/05/03   sst     initial implementation, broken out from srch4drv.h

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


/* Common */
#include "customer.h"
#include "comdef.h"

/* SRCH4 */
#include "srch4trans_t.h"

/* FW */
#include "srchfw_msg.h"

/* Other */
#include "log.h"
#include "amssassert.h"



/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Debugging defines and Macros
-------------------------------------------------------------------------*/
#undef SRCH4_DEBUG
#define SRCH4_DEBUG_MSG( str, a, b, c )  
/* MSG_FATAL( str, a, b, c ) */

/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/

/* Max number of tasks/sectors programmable for HPQ and LPQ searches */
/* Make sure SRCH4_MAX_NUM_TASKS in srch4trans.h is set correctly */
#define SRCH4_MAX_TASKS      SRCH_MAX_HPQ_TASKS
#define SRCH4_MAX_HPQ_TASKS  SRCH_MAX_HPQ_TASKS

/* Must be in sync with SRCH_MAX_LPQ_TASKS @ srchfw_msg.h.
   FEATURE_CDMA_AFLT in FW is not accessible to SW. */
#define SRCH4_MAX_LPQ_TASKS  16

/* number of peaks for searcher 4 */
#define SRCH4_NUM_PEAKS      NUM_SORTED_PEAKS

/* define for no dump occured queue */
#define SRCH4_NO_DUMP        -1

/* maximum sector gain index */
#define MAX_SECTOR_GAIN_VAL  4


/*===========================================================================

                  SRCH4 logging structs and definitions
  
  note: Reserved fields are to make packet to word aligned
===========================================================================*/

#define SRCH4_HDR_LOG_VERSION  2

typedef struct {
  uint8                 Reserved;
  uint8                 PeakPos_MSB;
  uint16                PeakPos_LSB;
  uint16                PeakEng;
} srch4_res_log_type;

typedef struct {
  uint16                Pilot_Antenna_BurstLen_QOF_WalshMSB;
  uint16                WalshLSB_NonCohLen;
  uint16                CohTrunc_CohLen;
  uint16                PNPos;
  uint16                WinSize;
  uint16                carrier;
  srch4_res_log_type    results[ SRCH4_NUM_PEAKS ];
} srch4_task_log_type;

typedef struct {
  log_hdr_type          hdr;
  uint8                 version;
  uint8                 reserved;
  uint8                 Tech_QPCH_NumPeaks;
  uint8                 Queue_Priority;
  uint8                 NumTasks;
  int8                  FreqOffset;
  int16                 rx0_agc;
  int16                 rx1_agc;
  srch4_task_log_type   tasks[ SRCH4_MAX_NUM_TASKS ];
} LOG_SRCH4_C_type;

/* size of the header block (minus task) */
#define SRCH4_LOG_HEADER_SIZ     ( FPOS( LOG_SRCH4_C_type, tasks ) )

/* size of the parameters (minus the results) */
#define SRCH4_LOG_PARAM_SIZ      ( FPOS( srch4_task_log_type, results ) )

/* size of the results */ 
#define SRCH4_LOG_RESULTS_SIZ    ( sizeof( srch4_res_log_type ) )

#define SRCH4_LOG_SIZ( tasks, peaks )                                    \
          ( SRCH4_LOG_HEADER_SIZ + (tasks) *                             \
              ( SRCH4_LOG_PARAM_SIZ + (peaks) * SRCH4_LOG_RESULTS_SIZ ) )

/*-------------------------------------------------------------------------
      Typedefs - ENUMs
-------------------------------------------------------------------------*/

/* searcher 4 error return values */
enum {
  SRCH4DRV_RET_FULL           =  0,       /* queue is full              */
  SRCH4DRV_RET_ACTIVE         = -1,       /* queue is active            */
  SRCH4DRV_RET_UNCLEAN        = -2,       /* last srch wasn't cleaned up*/
  SRCH4DRV_RET_INVALID_PARAM  = -3,       /* bad parameter was passed
                                             (i.e. null ptr)            */
  SRCH4DRV_RET_INVALID_QUEUE  = -4,       /* invalid/no queue specified */
  SRCH4DRV_RET_TECH_MISMATCH  = -5,       /* technology mismatch        */
  SRCH4DRV_RET_FATAL          = -6,       /* fatal errors               */
  SRCH4DRV_RET_FREE           = -7,
                                          /* queue is inactive and empty*/
  SRCH4DRV_RET_HPQ0_FREE      = -8,
                                          /* HPQ0 is inactive and empty */
  SRCH4DRV_RET_LPQ_FREE       = -9,
                                          /* LPQ is inactive and empty */
  SRCH4DRV_RET_HPQ1_FREE      = -10,
                                          /* HPQ1 is inactive and empty */
  SRCH4DRV_RET_OK             = -11,
                                          /* no errors to report        */
};

/* searcher 4 queue types */
/* make sure this matches srch4_mem_map defined in srch4drv.c */
typedef enum
{
  SRCH4_MIN_Q = 0,                       /* none    */
  SRCH4_HPQ0 = SRCH4_MIN_Q,              /* HPQ 0   */
  SRCH4_LPQ,                             /* LPQ     */
  SRCH4_HPQ1,                            /* HPQ 1   */
  SRCH4_MAX_Q,                           /* none    */
  SRCH4_NO_QUEUE = SRCH4_MAX_Q,
} srch4_queue_type;

/* searcher 4 antenna select */
typedef enum
{
  SRCH4_ANT_0         = 0,               /* antenna 0 mode    */
  SRCH4_ANT_1         = 1,               /* antenna 1 mode    */
  SRCH4_ANT_DUAL      = 2,               /* dual antenna mode */
  SRCH4_ANT_RD_0      = 3,               /* RxD antenna 0     */
  SRCH4_ANT_RD_1      = 4,               /* RxD antenna 1     */
  SRCH4_ANT_MAX,
} srch4_ant_select_type;

/* Carrier select */
typedef enum
{
   SRCH4_CARRIER_0 = 0,
   SRCH4_CARRIER_1 = 1,
   SRCH4_CARRIER_2 = 2,
   SRCH4_CARRIER_3 = 3
} srch4_carrier_type;

/*-------------------------------------------------------------------------
      Typedefs - Task structure types
-------------------------------------------------------------------------*/

/* searcher 4 task structure type
    (roughly ordered as it is to program the MDSP) */
struct  srch4_task_struct
{
  uint16    window_size;               /* srch sweep win size (chips)
                                          (MSM6100 max 8192
                                           MSM6500 max 32768)         */
  uint16    pn_pos;                    /* PN position (chips)
                                          (0-32767)                   */
  uint8     phase;                     /* phase (Cx8)
                                          (MSM6500 not used )         */
  uint16    coherent_len;              /* coherent length
                                          (typical 64-2048,
                                           MSM6500 min 48, max 2048)  */
  uint16    pilot_inx;                 /* pilot offset
                                          (0-511)                     */
  uint8     burst_len;                 /* HDR burst length
                                          (1-2)                       */
  uint8     non_coherent_len;          /* non-coherent length
                                          (0-128
                                           MSM6500-HDR 1-2)           */
  srch4_ant_select_type  diversity;    /* receive diversity select
                                          (see srch4_ant_select)      */

  srch4_ant_select_type  diversity_ant; /* antenna used for receive
                                           diversity                  */

  srch4_carrier_type     carrier;      /* carrier */
  uint8     coherent_trunc;            /* coherent truncation
                                          (see coh_tr[])              */
  uint8     qof;                       /* quasi-orthogonal function
                                          (0-3)                       */
  uint16    walsh;                     /* walsh index
                                          (0-511)                     */
  uint32    window_pos;                /* window position (Cx8) (end)
                                          (0-32767)                   */
  uint32    peak_pos[SRCH4_NUM_PEAKS]; /* peak position (Cx16)        */
  uint16    peak_eng[SRCH4_NUM_PEAKS]; /* peak energy                 */

  uint32    peak_posx2_raw[SRCH4_NUM_PEAKS]; /* raw peak position(Cx2)*/
  uint16    peak_eng_raw[SRCH4_NUM_PEAKS];   /* raw peak energy       */

  uint32    pilot_set_mask;            /* pilot set membership        */

  void    (*process_results)(
              struct srch4_task_struct *task);
                                       /* ptr to res processing func  */
  void     *parm_ptr;                  /* ptr to parm struct          */
  void     *result_ptr;                /* ptr to result struct        */
};
typedef struct srch4_task_struct srch4_task_struct_type;

typedef struct
{
  uint16 eng[SRCH4_NUM_PEAKS];
   /* Sorted peak energies of all the paths found by the searcher */

  int16 posx2[SRCH4_NUM_PEAKS];
   /* Peak positions in chipx2 units for the search peaks */

  uint16 eng_int[SRCH4_NUM_PEAKS];
   /* Interpolated peak energies */

  int32 posx8_int[SRCH4_NUM_PEAKS];
   /* Interpolated chipx8 peak positions */

  uint32 ecio_1x_linear; 
   /* linear ecio value for 1x search */

  uint32 ecio_1x_db; 
   /* ecio value in dB for 1x search */

}
srch4_srch_res_struct_type;

typedef struct
{
  /* NOTE: This structure is copied to the DSP search registers and hence
     the order of the elements of these struct is very important */
  uint8   carrier_idx;
    /* Carrier Selector Rotator/Sample RAM index */

  uint16  rtc_offsetx2;
   /* Offset of the center of the search window relative to the RTC */

  uint16  pn_offset;
   /* pilot pn offset of the sector */

  uint16  win_size;
   /* search window size to be used to search this sector.
      units : chips, Range : 1-215 for normal search
                     Range : 1-64  for temporal search type */

  byte    coh_trunc;
    /* Coherent truncation for scaling the search results */

  byte    burst_length;
    /* Number of pilot bursts that are to be combined for the search task */ 

  uint16  coh_int;
   /* coherent integration length */

  byte    noncoh_int;
   /* non-coherent integration length */

  srch4_carrier_type    carrier;
    /* carrier to search for */

  boolean dual_ant;
   /* If set to 1 then two antenna buffers will be non-coherently combined
      for reporting search results */

  boolean ant_sel;
   /* antenna select: RX0 or RX1. If dual_ant is set then ant_sel value
      does not matter since we are going to combine both antennae */

  srch4_srch_res_struct_type*  res_ptr;
   /* This result pointer will be used if sect_ptr=NULL. This is for the
      cases such as acquisition when the searches are initiated for
      parameters which are not sector specific. It is usually a good
      idea to fillup res_ptr = sect_ptr->res_ptr as it saves one extra
      indirection */

}
srch4_srch_struct_type;

#endif /* SRCH4DRV_T_H */
