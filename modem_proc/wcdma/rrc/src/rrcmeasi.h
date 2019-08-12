#ifndef RRCMEASI_H
#define RRCMEASI_H
/*===========================================================================
        R R C   M E A S U R E M E N T S   H E A D E R   F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  Measurement Control and Report Procedure.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcmeasi.h_v   1.12   18 Jul 2002 19:40:48   vkhanna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcmeasi.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   ad      Made changes to log important parms from OTAs
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
08/24/15   sas   Made Changes to implement 3Gpp CR3382 related to multiple PLMNs list in SIB18.
11/11/14   vi    Made changes to not to change reporting mode when NW uses rel-9 and pre-rel9 MCM for DC-HSUPA
09/26/14   bc    Made changes to correct compilation errors.
07/14/14   ymu   Check in Compilation errors when disable DC-HSUPA
07/11/14   vi    Made changes to convert earfcn type from uint16 to unti32
05/28/14   vi    Made changes to support Rel-10 periodic measurements on secondary frequency
04/25/14   vi    Made changes to handle LTE deprioritization request in W-RRC
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/25/14   sr    Made changes to update meas action when MCM setup is received for an existing meas id
03/11/14   vi     Made changes to disable IRAT and IFREQ measurements in different states based on the NV rrc_disable_meas_nv
01/22/14   vi    Made changes to handle dch to pch state change indication if OC is not set.
01/18/14   vi    Correcting the protoype of rrcgps_fill_measured_result_pos_est_incl
01/07/13   vi    Modified code to configure MAC with TVM when 
                 new cell indication is posted to meas and rrccu_substate is wait for CU confirm
12/13/13   vi      Modified the code to ignore the invalid cell id configured in cellsforInterfreqmeaslist/cellsforinterRATmeaslist 
                    and sending MCF only when the whole list is invalid.
11/19/13   vi    Changing prototype of rrcgps_fill_measured_results_pos_err_incl
09/16/13   mp    Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
09/13/13   vi    Added code to support DC-HSUPA measurement(Event triggered)
05/28/13   sr    Added code for measurement identity extension
05/13/13   vi    Added code for GANSS under feature FEATURE_GANSS_SUPPORT
04/25/13   sg    Added code for REL9 under FEATURE_WCDMA_REL9
01/10/13   sa    Made changes to update the event criteria list based on the ul trch type
12/11/12   sr    Made changes to inherit all of the maximum possible ARFCNs during inter RAT reselections.
09/24/12   ad    Made changes to map Meas report for EDCH objects to DCH transport 
                 channel for HSRACH
08/31/12   rd    Transfer dedicated priority parameters to/from WCDMA and GERAN
                 during IRAT procedure
08/17/12   sa    Made the changes to delete SIB TVM context on reselection
12/13/11   mn    Made changes to ignore meas failure form L1
04/27/11   vg    added function prototype.
01/28/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad    Added changes for RRC SW decoupling
12/31/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   rm    Added code to support W2L measurements in DCH state under FEATURE_WCDMA_CM_LTE_SEARCH
10/11/10   ss    ASN.1 migration to ASN1 9.3.0
10/11/10   ss    Upgraded SIB11Bis code to ASN 8.a and added code to init sibs lookup table 
                 after initialization of rrc_nv_rel_indicator.
11/03/10   rl    Fixed compiler warnings
10/30/10   rl    Objective systems migration changes check in 
                 on code base VU_MODEM_WCDMA_RRC.01.80.00
09/09/10   rm    Made changes to hold dedicated priorities during OOS
08/02/10   sks   Increased max no. of internal measurements to 8.
06/29/10   rm    Added code to support LTE<->WCDMA Inter RAT
06/18/10   ss    Fixed compilation warnings
03/12/10   dm    Added code for supporting Deferred SIB reading feature.
10/07/09   gkg   Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss    Updated Copyright Information
01/19/08   ps    Made changes for ASn1 migration 25.331v7.9  
                 i.e.May 2008 version  (2008-05) 
12/29/08   rm    Added support for 3G<->GAN Handover
11/10/08   rm    Made changes to process Rel7 measurement control messages for 
                 InterRAT, Traffic, Quality and Internal measurements
09/12/08   ps    Added code for REl7 under FEATURE_WCDMA_REL7.ASn1 Migration
                   to march 08 ver
07/16/08   rm    Enhanced the handling of transaction id received in MCM message so that
                 UE accepts the MCM's with same transaction id and in case of failure UE
                 sends MCM failure for a transaction id only if it is latest among the same
                 transactions ids
06/13/08   gkg   For Cell_Id through OTAs in Cell_DCH state, we moved the definition
                 of INVALID_CELL_ID to rrcdata_v.h
03/10/08   rm    Added support for SIB11bis. Code changes are under feature flag 
                 FEATURE SIB11BIS SUPPORT  
02/04/07   rm    Changed the prototype for rrcgps_verify_positioning_capability and
                 added new prototype rrcgps_verify_positioning_capability_r5 for REL5
08/31/07   ps    Moved the RRC GPS procedure internal code from rrcgps.h to 
                 rrcmeasi.h.Changes are made under feature flag 
                 FEATURE_WCDMA_GPS_CELL_ID_READING.
07/17/07   rm    Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
06/28/07   rm    Fixed memory leak when the validation of a measurement report
                 for inter frequency fails
05/25/07   vg      added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
03/13/07   vg    added one more FAILURE enum in rrcmeas_status_e_type.
03/08/07   vg      fixed compilation error
10/11/06   sgk     Moved prototype of rrcgps_process_mcm_r6 under MMGPS feature.
09/25/06   kp      Added support for sib_change_ind interface change
09/11/06   sgk   Added rel6 measurement support and code optimization across
                 releases.
08/30/06   vg    added enum in rrcmeas_status_e_type.
08/29/06   sm/da Made changes required for R6 ASN.1 migration.  Also merged
                 FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
28/08/06   vg    changed function prototype and added new prototype.
08/14/06   vg    added enum "NOT_EXIST" in rrcmeas_layer_e_type
06/05/06   sgk   Made changes to include rel5 non HSDPA functionality under
                 FEATURE REL5 while retaining the HSDPA specifc processing
                 under FEATURE HSDPA.
05/11/06   sgk   Added changes for lsm PCH reporting.
04/18/06   da    Modified code to support migration to Mar-06 Rel-5 ASN1 +
                 R6 Hacks
02/28/06  da/sm  Modified code to support migration to Dec-05 Rel-6 ASN1
10/27/05   sgk   Modified prototype for function
                 rrcmeas_append_intra_freq_periodic_meas_results for rel 5.
10/21/05   sgk   Added prototype changes to support rel5 delta qrxlevmin, delta
                 qhcs and delta rscp processing.
                 Added prototype changes to support rel5 position measurements.
08/19/05   sgk   Added prototype changes to support event 1b and event 1d non
                 critical extensions in sib 11 and 12.
07/13/05   sgk   Added prototype changes to support rel 5 non critical
                 extensions for rel99 msg.
07/08/05   sgk   Added prototype for function
                 rrcmeas_check_meas_id_used_for_different_meas_type.
06/27/05   sgk   Added basic rel 5 measurement ctrl msg support.
06/23/05   sgk   Moved #define MAX_QHCS_RSCP 26 to rrcmeas.h
05/26/05   sgk   Fixed lint error int to enum mismatch in rrcueimeas.c
                 when macro RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT
                 is used.
05/26/05   sgk   Increased MAX_INTER_FREQ_MEAS to 7 to support L1 handling of
                 multiple event 2a ... 2f.
05/20/05   vk    Declared Maximum Rel 99 Transaction Identifer as 3 under
                 FEATURE HSDPA
05/03/05   vk    Increased the maximum no of transaction Id's from 4 to 16 and also
                 changed the value of invalid transaction id from 8 to 55 under
                 FEATURE HSDPA. Added R5 IE as additional parameter to fn
                 rrctvm_validate_additional_meas. Added Rel 5 transaction Identifier
                 to fn rrcmeas_process_mcm_r5()
04/19/05   vk    Corrected Time To Trigger Value for Event 1D
04/07/05   sgk   Added HCS support.
04/05/05   vk    Declared structure rrc_gps_l2_ack_struct_type to store L2 ACK
                 for AGPS under feature AAGPS_MR_RRC_NOTIFICATION_REQUIRED
04/01/05   sgk   Included rrccsp_v.h for true limited service function.
03/28/05   sgk   Added prototype for transaction id initialization function.
03/18/05   vk    Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA.
                 Contains partial support for Intra Freq Rel 5 MCM
03/15/05   sgk   Added measurement optimization to restrict max pending ack
                 mode measurement rpts to 12.
03/07/05   sgk   Fixed compiler warnings
03/04/05   sgk   added Quality measurement function prototypes.
03/02/05   ttl   Support RRC ASN1 Release 5.
02/17/05   vk    Removed the header file l1m.h and include it directly in rrcmeas.c
02/14/05   vk    Included header file l1m.h so that fn l1m_get_state could be
                 invoked while processing additional measured results
12/21/04   sgk   Lint fix removed  function parameter tmp_inter_freq_cell_list
                 from rrcifreq_build_composite_cell_list as it is now global.
12/02/04   sgk   Removed prototype for rrcmeas_copy_meas_id_list.
11/23/04   sgk   Added function prototypes for SIB12 support.
11/18/04   vk    Removed cells_for_meas from meas id variable as it is not used
10/11/04   sgk   Replaced the last return value of macro RET_POS_METHOD_TYPE from.
                 RRC_UE_BASED_PREFERED to RRC_UE_ASSISTED_PREFERED.
09/28/04   sgk   Added #defines MEASUREMENT_IDENTITY_MIN and MAX for measurement
                 id range check.
09/02/04   vk    Declared prototype of 2 fns rrcgps_process_dch_to_fach() &
                 rrcgps_process_fach_to_dch() to support DCH<->FACH transitions
                 for AGPS
08/20/04   sgk   Removed the parameter meas_type_setup_incl from
                 rrcmeas_process_pos_mcm as it was not being used.
08/16/04   vk    Added code to supprt path loss measurements
08/13/04   vk    Changed the name of ASN1 variable rrc_methodNotSupported to
                 rrc_notAccomplishedGPS_TimingOfCellFrames as per Mar 04 ASN1
06/07/04   vk    Added rrcmeas_set_cell_plmn_info under FEATURE_MMGPS
06/04/04   vk    Declared rrcmeas_set_cell_plmn_info fn that retrieves sets cell
                 id, plmn id, psc and freq in the global structure plmn_cell_info
06/02/04   vk    Declared a bool mcm_owned in intra freq and inter freq meas id list.
                 Added a new parameter to rrcmeas_config_intra_freq_db fn and to fn
                 rrcifreq_config_inter_freq_db to pass validity. Declared new fn
                 rrcmeas_check_if_scr_present_in_aset() and
                 rrcmeas_update_meas_after_state_trans() to process validity related
                 handling
05/19/04   vk    Declared fn rrctvm_process_sib_change_notification() to use new SIB11
                 or SIB12 in CELL_FACH state
05/14/04   vk    Declared fn rrcirat_delete_dup_non_verified_cells to purge
                 duplicate non verfied ARFCN cells
04/21/04   vk    Declared fn rrcmeas_get_rach_rpt_quantity to get RACH rpt quan
04/06/04   vk    Changed an value in MEAS_RPT_CRIT MACRO to fix lint warning
03/29/04   vk    Declared rrcmeas_get_fach_meas_info() info to get FACH Meas
                 Info from SIB11
02/02/04   vk    Added support for A-GPS fns and variables under FEATURE_MMGPS
11/07/03   vk    Declared function rrcmeas_check_mcm_for_setup_modify_additional(..)
11/06/03   vk    Added support for UE Internal Measurements & Inter Freq Measurements
                 under FEATURE_CM_FOR_INTER_FREQUENCY_MEASUREMENTS
07/22/03   vk    Added HCS support
05/29/03   vk    Added code for implementing additional measurements of type
                 Intra Freq, Traffic Volume & Inter RAT Measurements at UE.
                 Removed FEATURE_2D_2F_MEAS.
02/24/02   vk    Merged traffic volume fix from branch MSM5200_RRC.02.02.07_2
                 in the mainline
02/14/02   vk    Fixed most of lint errors
02/05/03   sk    March2002Rel specific changes
                    Changed    rrc_UL_TrCH_Identity_rach_chosen
          to  rrc_rachorcpch_chosen
01/21/03   vk    Changed the name of feature FEATURE_INTRA_INTER_FREQ_MEAS to
                 FEATURE_2D_2F_MEAS
12/03/02   vk    Declared functions to handle inter-freq events 2d and 2f under
                 feature FEATURE_INTRA_INTER_FREQ_MEAS
11/07/02   vk    Declared function rrcmeas_append_inter_rat_results(..) to append
                 an Inter-RAT Measurement as an additional measurement to an
                 Intra-Freq Measurement Report
10/29/02   vk    Merged Inter-RAT Measurement Code from branch irat_gap to mainline.
                 Removed feature define FEATURE_DCH_FACH_MEAS. Declared function
                 rrcmeas_process_delete_meas_id(..) to delete meas to be deleted
                 due to compressed mode run time error.
10/15/02   vk    Fixed merge problems. Fn rrcmeas_validate_intra_freq_report(..)
                 was deleted earlier while merging.
10/14/02   vk    Defined FEATURE_DCH_FACH_MEAS to make sure that the existing
                 functionality of the code is not disturbed
10/09/02   vk    Merged code from branch in main line to handle intra-freq
                 measuremenmts and traffic volume measurements on DCH<-->FACH
                 transitions under feature FEATURE_DCH_FACH_MEAS
10/08/02   vk    Added declarations to handle Measurement Identity List
09/23/02   vk    Declared functions rrctvm_process_sibs_on_dch_to_fach(..)
                 and rrctvm_process_sibs_on_dch_to_fach(...) to process
                 traffic volume measurements on state transition.
09/19/02   vk    Added basic support for (DCH<-->FACH) measurements under
                 FEATURE_DCH_FACH_MEAS
07/18/02   vk    Removed fns rrcmeas_append_additional_meas_to_meas_rpt(..)
                 and rrcmeas_process_intra_freq_meas_additional_results(..)
06/09/02   vk    Declared function rrcmeas_process_state_change_ind_in_fach_state
                 to process state change indication in CELL_FACH state of RRC
05/17/02   vk    Added Traffic Volume Measurement Code under the feature
                 FEATURE TRAFFIC VOLUME MEAS
02/14/02   vk    Added code review Comments.
01/24/02   vk    Declared functions rrcmeas_update_intra_freq_cell_list_mcm,
                 rrcmeas_check_cell_order_mcm, rrcmeas_check_cell_order_rscp,
                 rrcmeas_check_cell_order_ecn0, rrcmeas_remove_cellid,
                 rrcmeas_update_intra_freq_cell_list, rrcmeas_find_psc. These
                 functions support the cell info list obtained from SIB11 and
                 also from Measurement Control Message. Intra-freq cell id's are
                 now stored in intra_freq_cell_list which provide the mapping
                 of intra freq cell id and the corresponding scrambling code
01/14/02   vk    Added a function convert_meas_value to convert meas values
01/11/02   vk    Removed mcm_received_in_dch_state_e_type, MAX_TRANSACTION_ID,
                 as they are not required
12/06/01   xfg   Added the changes for June specs conversion. The changes are
                 related to the change of the ASN1.
11/08/01   vk    Added function prototypes to support measurement control
                 message and measurement failure message in DCH state and also
                 support additional measurements.
09/14/01   vk    For periodical reporting and reporting amount, added macro
                 RRCMEAS_RET_REPORTING_INTERVAL and RRCMEAS_RET_REPORTING_AMT.
                 Removed function rrcmeas_fill_intra_freq_event_criteria.
                 Added function rrcmeas_set_intra_freq_event_id. The above
                 was done to make sure that only one event is sent per meas
                 ind from L1. Also added defines W and NO_OF_CELLS_FORBIDDEN
                 so that these params are set to 0XFFFF when not present in SIB
08/30/01   rj    Moved rrcmeas_rpt_stat_e_type and function prototype
                 rrcmeas_append_meas_rpt_on_rach to rrcmeas.h
08/29/01   vk    Created file which contains all the variable/function declarations
                 required for implementing RRC Measurement procedure

===========================================================================*/



/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "l1rrcif.h"
#include "rrcasn1.h"
#include "rrcmmif.h"
#include "rrcdata_v.h"
#include "err.h"
#include "rrccmd_v.h"
#include "rrcccm.h"
#include "rrccsp.h"

#if defined (FEATURE_CGPS)
  #include "rrclsmif.h"
#endif 

#include "mm_umts.h"
#include "sys_eplmn_list.h"
#include "rrcdata_v.h"
#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
#include "geran_grr_types.h"
#endif
/*===========================================================================
**                  STRUCTURE TYPES
**=========================================================================*/

/* Stores RRC transaction Identifier and Error Cause to be used by Meas
 * Control Failure Message */
typedef struct
{
  uint8 rrc_transaction_id;
  uint32 error_choice;
  rrc_ProtocolErrorCause error_cause;
}rrcmeas_mcf_struct_type;


/* Enumerated type of the different substates for RRC GPS  procedure           */
typedef enum
{
  RRCGPS_INITIAL,                   /* Initial Substate                    */
  RRCGPS_WAIT_FOR_SIB_CNF    /* Waiting for confirmation of Cell id from SIB                     */                       
}rrcgps_substates_e_type;

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


/* Indicates if RACH report should be appended */
typedef enum
{
  NO_RRC_MEAS_RACH_REPORT,
  APPEND_RRC_MEAS_RACH_REPORT
} rach_report_crit_e_type;




/* Indicates the RRC Measurement Procedure Current sub-state */
typedef enum
{
  RRCMEAS_INITIAL,
  RRCMEAS_IDLE,
  RRCMEAS_CELL_DCH,
  RRCMEAS_CELL_FACH,
  RRCMEAS_PCH
}rrcmeas_substate_e_type;
/* Below #defines are added for logging purpose only */
#define rrcmeas_substate_e_type_value0 RRCMEAS_INITIAL,
#define rrcmeas_substate_e_type_value1 RRCMEAS_IDLE,
#define rrcmeas_substate_e_type_value2 RRCMEAS_CELL_DCH,
#define rrcmeas_substate_e_type_value3 RRCMEAS_CELL_FACH,
#define rrcmeas_substate_e_type_value4 RRCMEAS_PCH

/* Indicates that if SIB12 is present in the cell and has been stored
 * as specified by SIB11. This helps in reading meas directly from SIB12 ON
 * TRANSITION from IDLE-->DCH
 */
typedef enum
{
  SIB12_ABSENT,
  SIB12_PRESENT
} sib12_present_e_type;

/* Indicates if a cell is occupied or vacant in CELL_INFO_LIST array */
typedef enum
{
  VACANT,
  OCCUPIED
} cell_position_e_type;

/* Indicates if psc is present or absent */
typedef enum
{
  PSC_ABSENT,
  PSC_PRESENT
} psc_e_type;

typedef enum
{
  RRCMEAS_FAIL,           /* Successful Status */
  RRCMEAS_SUCCESS,         /* Failure status */
  SFN_FAIL,
  RRCMEAS_UNSUPPORTED      /* if band unsupported */
}rrcmeas_status_e_type;

typedef enum
{
  L1_MEAS,
  MAC_MEAS,
  INTRA_FREQ,
  INTER_FREQ,
  INTER_SYS,
  INTERNAL_MEAS,
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
  POSITION_MEAS,
#endif
  QUALITY_MEAS,
  UNKNOWN_MEAS,
  NOT_EXIST
}rrcmeas_layer_e_type;

typedef enum
{
  TVM_FAIL,           /* Successful Status */
  TVM_SUCCESS         /* Failure status */
}rrctvm_status_e_type;

typedef enum
{
  SIB11,
  SIB12
}sib_choice_e_type;

/*
 * 10.3.7.51  - Measurement Validity
 */
typedef enum
{
  ALL_STATES,
  ALL_STATES_BUT_DCH,
  STATE_DCH,
  NOT_PRESENT
} meas_validity_enum_type;



/* Accepted transaction Table */
typedef struct
{
  uint8 transaction_id;
  /*
   boolean to track if the received trans id is latest among the same
   transaction ids 
  */
  boolean is_latest_trans;
} rrcmeas_accepted_transaction_list;

#ifdef FEATURE_AAGPS_MR_RRC_NOTIFICATION_REQUIRED
typedef struct
{
  boolean waiting_for_l2_ack;
  uint8 mui;
  uint8 meas_id;
} rrc_gps_l2_ack_struct_type;
#endif
/* Store Reporting Criteria in Internal Format */
typedef enum
{
  PERIODIC,
  EVENT,
  ADDTL,
  NOT_AVAILABLE
} rpt_crit_enum_type;


/*===================================================================
                        Defines
====================================================================*/

/* Max no of meas supported by L1. At present it is 1 */
// #define MAX_NUMBER_OF_MEAS_SUPPORTED_BY_L1  1

/* Max no of monitored cells sent by L1 in RACH report to UTRAN
 * Actually the total no is 7 with first being the active cell.
 * RRCMEAS procedure build the list for 7 cells. L1 sends list of 6
 * monitored cells. To read 6 monitored cells from L1, it is defined
 */
#define MAX_NO_MONITORED_CELLS  6

#define MEASUREMENT_IDENTITY_MIN 1
#if defined (FEATURE_WCDMA_REL9) || defined(FEATURE_WCDMA_REL10)
  /* For now setting it for 16. Set to 32 if meas_id_extn_support is set to TRUE */
  #define MEASUREMENT_IDENTITY_MAX 32
#else
  #define MEASUREMENT_IDENTITY_MAX 16
#endif
/* Maximum allowed UE transmit power */
#define UE_TXPWR_AMX_RACH 33

/* Quality params */
#define QQUALMIN  -20

#define QRXLEVMIN -115

#define QOFFSET1 0

#define QOFFSET2 0

/* L1 wants these 2 params initialized when they are not present */
#define NO_FORBIDDEN_CELLS  0xFFFF

#define NO_W  0xFFFF

/* Initializes transaction id to default ERROR value */
#define INVALID_TRANSACTION_ID  55
#define MAX_REL99_TRANSACTION_ID 3



/* To convert UTRAN values of reference time difference to L1 format */
#define REF_TIME_DIFF_ACCURACY_40   40
#define REF_TIME_DIFF_ACCURACY_256    256
#define REF_TIME_DIFF_ACCURACY_2560   2560

#define MAX_NO_OF_CELLS 32

#define AVAILABLE_CELL_POSITION 100

#define INVALID_PSC 1000

#define INVALID_MEAS_ID 200

/* Following are used to check params in Meas Report */
#define TM_MIN 0

#define TM_MAX 38399

#define OFF_MIN 0

#define OFF_MAX 255

#define MAXIMUM_TRANSACTIONS 16


/* MAX No of Inter-Freq Measurements */
#define MAX_INTER_FREQ_MEAS 7


/* MAX No of Inter-RAT Measurements */
#define MAX_INTER_RAT_MEAS 4

/* Invalid Cell Id for Inter-RAT Measurements */
/*Moved the definiton of INVALID_CELL_ID to rrcdata_v.h*/


#define MAX_PEND_MEAS_RPT 12

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
#define MAX_POS_MEAS 2
#endif
/* As per specs, Max Intra Freq Meas at a time is 8 */
#define MAX_INTRA_FREQ_MEAS 8

/* No scenario seen as yet of more than 1. */
#define MAX_ADDITIONAL_MEAS 4

#define ECN0_MIN_VALUE  0
#define ECN0_MAX_VALUE  49
#define RSCP_MIN_VALUE  0
#define RSCP_MAX_VALUE  91

#define DELTA_RSCP_MAX -1
#define DELTA_RSCP_MIN -5

#define PATH_LOSS_MIN_VALUE    46
#define PATH_LOSS_MAX_VALUE    158
#define SFN_SFN_TYPE1_MIN_VALUE 0
#define SFN_SFN_TYPE1_MAX_VALUE 9830399
#define SFN_SFN_TYPE2_MIN_VALUE 0
#define SFN_SFN_TYPE2_MAX_VALUE 40961
#define MAX_PSC 511
#define REMOVE_ALL_INTRA_FREQ_CELLS 0xff

#define INTER_FREQ_RSSI 76

#define MIN_CPICH_TX_POWER_RANGE -10
#define MAX_CPICH_TX_POWER_RANGE 50

#define PRI_FREQ 0
#define SEC_FREQ 1

#define RRC_DISABLE_WTOW_IDLE_MEAS 1
#define RRC_DISABLE_WTOL_IDLE_MEAS 2
#define RRC_DISABLE_WTOG_IDLE_MEAS 4
#define RRC_DISABLE_WTOW_FACH_MEAS 8
#define RRC_DISABLE_WTOL_FACH_MEAS 16
#define RRC_DISABLE_WTOG_FACH_MEAS 32
#define RRC_DISABLE_WTOW_CM_MEAS   64
#define RRC_DISABLE_WTOL_CM_MEAS   128
#define RRC_DISABLE_WTOG_CM_MEAS   256

/* RLC reporting criteria */
#define RRCMEAS_REPORT_TRANSFER_MODE(rlc_report_transfer_mode) \
(     \
 (rlc_report_transfer_mode == ACKNOWLEDGED_MODE_RLC) ? UE_MODE_ACKNOWLEDGED :     \
  ((rlc_report_transfer_mode == UNACKNOWLEDGED_MODE_RLC) ? UE_MODE_UNACKNOWLEDGED : NONE  \
  )   \
)


/* Converts meas quantity from ASN format to L1 format */
#define  RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN(intra_freq_meas_quan)   \
(  \
 (intra_freq_meas_quan == rrc_IntraFreqMeasQuantity_FDD_cpich_Ec_N0) ? L1_EC_NO : \
  ((intra_freq_meas_quan == rrc_IntraFreqMeasQuantity_FDD_cpich_RSCP) ? L1_RSCP :L1_PATHLOSS)\
)


/* Converts sfn sfn observed difference from ASN format to L1 format */
#define  RRCMEAS_RET_INTRA_FREQ_SFN_DELTA(sfn_sfn_otd_type) \
(     \
 (sfn_sfn_otd_type == rrc_SFN_SFN_OTD_Type_noReport) ? L1_NO_REPORT :     \
  ((sfn_sfn_otd_type == rrc_SFN_SFN_OTD_Type_type1) ? L1_REPORT_TYPE_1 : L1_REPORT_TYPE_2  \
  )   \
)

/* Converts sfn sfn observed difference from l1 FORMAT TO ASN format */
#define  RRCMEAS_RET_INTRA_FREQ_SFN_SFN(l1_sfn_report_enum_type) \
(     \
  (l1_sfn_report_enum_type == L1_REPORT_TYPE_1) ? T_rrc_SFN_SFN_ObsTimeDifference_type1: \
   T_rrc_SFN_SFN_ObsTimeDifference_type2 \
)




/* Converts the Intra Freq RACH Reporting Quantity for FDD
 * from ASN1 format to L1 format
 */
#define  RRCMEAS_RET_INTRA_FREQ_RACH_REP_QUAN(intra_freq_rach_rep_quan)  \
(  \
 (intra_freq_rach_rep_quan == rrc_IntraFreqRepQuantityRACH_FDD_cpich_EcN0) ? L1_EC_NO : \
  ((intra_freq_rach_rep_quan == rrc_IntraFreqRepQuantityRACH_FDD_cpich_RSCP) ? L1_RSCP : \
    ((intra_freq_rach_rep_quan == rrc_IntraFreqRepQuantityRACH_FDD_pathloss) ? \
     L1_PATHLOSS : L1_MEAS_NONE  \
    )  \
  )  \
)

#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif

/* Converts triggering condition for FDD from ASN1 format to L1
 * format used by event 1a
 */
#define  RRCMEAS_RET_TRIGGERING_CONDITION_2(triggering_condition)  \
(  \
 (triggering_condition == rrc_TriggeringCondition2_activeSetCellsOnly) ? 0 : \
  ((triggering_condition == rrc_TriggeringCondition2_monitoredSetCellsOnly) ? 1 : \
  ((triggering_condition == rrc_TriggeringCondition2_activeSetAndMonitoredSetCells) ? 2 :\
  (triggering_condition == rrc_TriggeringCondition2_detectedSetCellsOnly) ? 3 : 4 \
   ) \
  ) \
)


/* Converts triggering condition for FDD from ASN1 format to L1
 * format Used by event 1b
 */
#define  RRCMEAS_RET_TRIGGERING_CONDITION_1(triggering_condition)  \
(  \
 (triggering_condition == rrc_TriggeringCondition1_activeSetCellsOnly) ? 0 : \
  ((triggering_condition == rrc_TriggeringCondition1_monitoredSetCellsOnly) ? 1 : 2 \
  ) \
)


/* Converts report deactivation threshold for FDD from ASN1 format
 * to L1 format
 */
#define  RRCMEAS_RET_DEACTIVATION_THRESHOLD(deactivation_threshold)  \
(  \
 (deactivation_threshold == rrc_ReportDeactivationThreshold_notApplicable) ? 0 : \
  ((deactivation_threshold == rrc_ReportDeactivationThreshold_t1) ? 1 : \
  ((deactivation_threshold == rrc_ReportDeactivationThreshold_t2) ? 2 :\
  ((deactivation_threshold == rrc_ReportDeactivationThreshold_t3) ? 3 :\
  ((deactivation_threshold == rrc_ReportDeactivationThreshold_t4) ? 4 :\
  ((deactivation_threshold == rrc_ReportDeactivationThreshold_t5) ? 5 :\
  (deactivation_threshold == rrc_ReportDeactivationThreshold_t6) ? 6 : 7 \
   ) \
  ) \
  ) \
  ) \
  ) \
)


/* Converts report deactivation threshold for FDD from ASN1 format to
 * L1 format
 */
#define  RRCMEAS_RET_REPORTING_AMOUNT(reporting_amount)  \
(  \
 ((reporting_amount == rrc_ReportingAmount_ra1) ? 1 : \
  ((reporting_amount == rrc_ReportingAmount_ra2) ? 2 : \
  ((reporting_amount == rrc_ReportingAmount_ra4) ? 4 :\
  ((reporting_amount == rrc_ReportingAmount_ra8) ? 8 :\
  ((reporting_amount == rrc_ReportingAmount_ra16) ? 16 :\
  ((reporting_amount == rrc_ReportingAmount_ra32) ? 32 :\
  (reporting_amount == rrc_ReportingAmount_ra64) ? 64 : 0 \
  )))))) \
)


/* Converts replacement activation threshold for FDD from ASN1 format
 * to L1 format
 */
#define  RRCMEAS_RET_REPLACEMENT_ACTIVATION_THRESHOLD(replacement_activation_threshold)  \
(  \
 (replacement_activation_threshold == rrc_ReplacementActivationThreshold_notApplicable) ? 0 : \
  ((replacement_activation_threshold == rrc_ReplacementActivationThreshold_t1) ? 1 : \
  ((replacement_activation_threshold == rrc_ReplacementActivationThreshold_t2) ? 2 :\
  ((replacement_activation_threshold == rrc_ReplacementActivationThreshold_t3) ? 3 :\
  ((replacement_activation_threshold == rrc_ReplacementActivationThreshold_t4) ? 4 :\
  ((replacement_activation_threshold == rrc_ReplacementActivationThreshold_t5) ? 5 :\
  (replacement_activation_threshold == rrc_ReplacementActivationThreshold_t6) ? 6 : 7 \
  ))))) \
)


/* Converts replacement activation threshold for FDD from ASN1
 * format to L1 format
 */
#define  RRCMEAS_RET_REPORTING_INTERVAL(reporting_interval)  \
(  \
 (reporting_interval == rrc_ReportingInterval_noPeriodicalreporting) ? 0 : \
  ((reporting_interval == rrc_ReportingInterval_ri0_25) ? 250 : \
  ((reporting_interval == rrc_ReportingInterval_ri0_5) ? 500 :\
  ((reporting_interval == rrc_ReportingInterval_ri1) ? 1000 :\
  ((reporting_interval == rrc_ReportingInterval_ri2) ? 2000 :\
  ((reporting_interval == rrc_ReportingInterval_ri4) ? 4000 :\
  (reporting_interval == rrc_ReportingInterval_ri8) ? 8000 : 16000 \
  ))))) \
)


/* Converts time to trigger for FDD from ASN1 format to L1 format */
#define  RRCMEAS_RET_TIME_TO_TRIGGER(time_to_trigger)  \
(  \
 (time_to_trigger == rrc_TimeToTrigger_ttt0) ? 0 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt10) ? 10 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt20) ? 20 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt40) ? 40 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt60) ? 60 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt80) ? 80 : \
  ((time_to_trigger ==rrc_TimeToTrigger_ttt100) ? 100 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt120) ? 120 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt160) ? 160 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt200) ? 200 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt240) ? 240 : \
  ((time_to_trigger == rrc_TimeToTrigger_tt320) ? 320 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt640) ? 640 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt1280) ? 1280 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt2560) ? 2560 : 5000 \
  )))))))))))))) \
)

/* Converts meas quantity from L1 format to ASN format */

#define  RRCMEAS_RET_ASN_MEAS_QUANTITY(meas_quantity_type)  \
(  \
 (meas_quantity_type == L1_EC_NO) ? rrc_cpich_Ec_N0_present : \
 ((meas_quantity_type == L1_RSCP) ? rrc_cpich_RSCP_present : \
  rrc_fdd_pathloss_present  \
 ) \
)


/* Converts meas reporting mode for FDD from ASN1 format to L1 format */
#define  RRCMEAS_RET_MEAS_REPORTING_MODE(periodic_or_event_trigger)  \
(  \
 (periodic_or_event_trigger == rrc_PeriodicalOrEventTrigger_periodical) ? L1_PERIODIC_RPT : \
 ((periodic_or_event_trigger == rrc_PeriodicalOrEventTrigger_eventTrigger) ? L1_EVENT_TRIGGER_RPT : L1_NO_CHANGE \
 ) \
)


/* Converts meas report transfer mode for RLC for a meas report from ASN1 format to Local format */
#define  RRCMEAS_RET_MEAS_RLC_TRANSFER_MODE(rrc_TransferMode)  \
(  \
 (rrc_TransferMode == rrc_TransferMode_acknowledgedModeRLC) ? ACKNOWLEDGED_MODE : UNACKNOWLEDGED_MODE \
)

/* Converts intra-freq meas quantity filter coefficient for FDD from ASN1
 * format to L1 format
 */

#define  RRCMEAS_RET_INTRA_FREQ_MEAS_QUAN_FILTER_COEFFICIENT(fc)  \
(  \
   (fc == rrc_FilterCoefficient_fc0) ? L1_WCDMA_FILTER_COEF_0 : \
  ((fc == rrc_FilterCoefficient_fc1) ? L1_WCDMA_FILTER_COEF_1 :\
    ((fc == rrc_FilterCoefficient_fc2) ? L1_WCDMA_FILTER_COEF_2 :\
  ((fc == rrc_FilterCoefficient_fc3) ? L1_WCDMA_FILTER_COEF_3 :\
    ((fc == rrc_FilterCoefficient_fc4) ? L1_WCDMA_FILTER_COEF_4 :\
  ((fc == rrc_FilterCoefficient_fc5) ? L1_WCDMA_FILTER_COEF_5 :\
  ((fc == rrc_FilterCoefficient_fc6) ? L1_WCDMA_FILTER_COEF_6 :\
  ((fc == rrc_FilterCoefficient_fc7) ? L1_WCDMA_FILTER_COEF_7 :\
    ((fc == rrc_FilterCoefficient_fc8) ? L1_WCDMA_FILTER_COEF_8 :\
  ((fc == rrc_FilterCoefficient_fc9) ? L1_WCDMA_FILTER_COEF_9 :\
  ((fc == rrc_FilterCoefficient_fc11) ? L1_WCDMA_FILTER_COEF_11 :\
  ((fc == rrc_FilterCoefficient_fc13) ? L1_WCDMA_FILTER_COEF_13 :\
  ((fc == rrc_FilterCoefficient_fc15) ? L1_WCDMA_FILTER_COEF_15 :\
  ((fc == rrc_FilterCoefficient_fc17) ? L1_WCDMA_FILTER_COEF_17 :\
  (fc == rrc_FilterCoefficient_fc19) ? L1_WCDMA_FILTER_COEF_19 : L1_WCDMA_FILTER_COEF_0 \
  ))))))))))))) \
)

/* Converts max no of reported cells on RACH from ASN1
 * format to L1 format
 */
#define  RRCMEAS_RET_RACH_REPORTED_CELLS(max_no_of_reported_cells)  \
(  \
 (max_no_of_reported_cells == rrc_MaxReportedCellsOnRACH_noReport) ? L1_RACH_RPT_NONE : \
  ((max_no_of_reported_cells == rrc_MaxReportedCellsOnRACH_currentCell) ? L1_RACH_RPT_0 : \
  ((max_no_of_reported_cells == rrc_MaxReportedCellsOnRACH_currentAnd_1_BestNeighbour) ? L1_RACH_RPT_1 :\
  ((max_no_of_reported_cells == rrc_MaxReportedCellsOnRACH_currentAnd_2_BestNeighbour) ? L1_RACH_RPT_2 :\
  ((max_no_of_reported_cells == rrc_MaxReportedCellsOnRACH_currentAnd_3_BestNeighbour) ? L1_RACH_RPT_3 :\
  ((max_no_of_reported_cells == rrc_MaxReportedCellsOnRACH_currentAnd_4_BestNeighbour) ? L1_RACH_RPT_4 :\
  (max_no_of_reported_cells == rrc_MaxReportedCellsOnRACH_currentAnd_5_BestNeighbour) ? L1_RACH_RPT_5 : L1_RACH_RPT_6 \
  ))))) \
)

/* Converts periodic reporting interval for FDD from ASN1
 * format to L1 format
 */
#define  RRCMEAS_RET_PERIODIC_REPORTING_INTERVAL(ri)  \
(  \
 (ri == rrc_ReportingIntervalLong_ril0) ? 0 : \
  ((ri == rrc_ReportingIntervalLong_ril0_25) ? 250 : \
  ((ri == rrc_ReportingIntervalLong_ril0_5) ? 500 :\
  ((ri == rrc_ReportingIntervalLong_ril1) ? 1000 :\
  ((ri == rrc_ReportingIntervalLong_ril2) ? 2000 :\
  ((ri == rrc_ReportingIntervalLong_ril3) ? 3000 :\
  ((ri == rrc_ReportingIntervalLong_ril4) ? 4000 :\
  ((ri == rrc_ReportingIntervalLong_ril6) ? 6000 :\
  ((ri == rrc_ReportingIntervalLong_ril8) ? 8000 :\
  ((ri == rrc_ReportingIntervalLong_ril12) ? 12000 :\
  ((ri == rrc_ReportingIntervalLong_ril16) ? 16000 :\
  ((ri == rrc_ReportingIntervalLong_ril20) ? 20000 :\
  ((ri == rrc_ReportingIntervalLong_ril24) ? 24000 :\
  ((ri == rrc_ReportingIntervalLong_ril28) ? 28000 :\
  (ri == rrc_ReportingIntervalLong_ril32) ? 32000 : 64000 \
  ))))))))))))) \
)

/* Converts meas quantity from L1 format to RACH ASN format */
#define  RRCMEAS_RET_RACH_MEAS_QUANTITY(meas_quantity_type)  \
(  \
 (meas_quantity_type == L1_EC_NO) ? T_rrc_measurementQuantity_cpich_Ec_N0 : \
 ((meas_quantity_type == L1_RSCP) ? T_rrc_measurementQuantity_cpich_RSCP : \
  rrc_pathloss  \
 ) \
)


#define MEAS_QUANTITY_INTERVAL_OFFSET 20

#define  TVM_RET_REPORTING_AMOUNT(reporting_amount)  \
(  \
 ((reporting_amount == rrc_ReportingAmount_ra1) ? 1 : \
  ((reporting_amount == rrc_ReportingAmount_ra2) ? 2 : \
  ((reporting_amount == rrc_ReportingAmount_ra4) ? 4 :\
  ((reporting_amount == rrc_ReportingAmount_ra8) ? 8 :\
  ((reporting_amount == rrc_ReportingAmount_ra16) ? 16 :\
  ((reporting_amount == rrc_ReportingAmount_ra32) ? 32 :\
  (reporting_amount == rrc_ReportingAmount_ra64) ? 64 : 0xff \
  )))))) \
)

#define  TVM_RET_PERIODIC_REPORTING_INTERVAL(ri)  \
(  \
 (ri == rrc_ReportingIntervalLong_ril0) ? 10 : \
  ((ri == rrc_ReportingIntervalLong_ril0_25) ? 250 : \
  ((ri == rrc_ReportingIntervalLong_ril0_5) ? 500 :\
  ((ri == rrc_ReportingIntervalLong_ril1) ? 1000 :\
  ((ri == rrc_ReportingIntervalLong_ril2) ? 2000 :\
  ((ri == rrc_ReportingIntervalLong_ril3) ? 3000 :\
  ((ri == rrc_ReportingIntervalLong_ril4) ? 4000 :\
  ((ri == rrc_ReportingIntervalLong_ril6) ? 6000 :\
  ((ri == rrc_ReportingIntervalLong_ril8) ? 8000 :\
  ((ri == rrc_ReportingIntervalLong_ril12) ? 12000 :\
  ((ri == rrc_ReportingIntervalLong_ril16) ? 16000 :\
  ((ri == rrc_ReportingIntervalLong_ril20) ? 20000 :\
  ((ri == rrc_ReportingIntervalLong_ril24) ? 24000 :\
  ((ri == rrc_ReportingIntervalLong_ril28) ? 28000 :\
  (ri == rrc_ReportingIntervalLong_ril32) ? 32000 : 64000 \
  ))))))))))))) \
)


#define TVM_RET_EVENT_ID(event_id) \
( \
(event_id == rrc_TrafficVolumeEventType_e4a) ? EVENT_4A :EVENT_4B \
)


#define  TVM_RET_THRESHOLD_BYTES(threshold)  \
(  \
 (threshold == rrc_TrafficVolumeThreshold_th8) ? 8 : \
  ((threshold == rrc_TrafficVolumeThreshold_th16) ? 16 : \
  ((threshold == rrc_TrafficVolumeThreshold_th32) ? 32 :\
  ((threshold == rrc_TrafficVolumeThreshold_th64) ? 64 :\
  ((threshold == rrc_TrafficVolumeThreshold_th128) ? 128 :\
  ((threshold == rrc_TrafficVolumeThreshold_th256) ? 256 :\
  ((threshold == rrc_TrafficVolumeThreshold_th512) ? 512 :\
  ((threshold == rrc_TrafficVolumeThreshold_th1024) ? 1024 :\
  ((threshold == rrc_TrafficVolumeThreshold_th2k) ? 2048 :\
  ((threshold == rrc_TrafficVolumeThreshold_th3k) ? 3072 :\
  ((threshold == rrc_TrafficVolumeThreshold_th4k) ? 4096 :\
  ((threshold == rrc_TrafficVolumeThreshold_th6k) ? 6144 :\
  ((threshold == rrc_TrafficVolumeThreshold_th8k) ? 8192 :\
  ((threshold == rrc_TrafficVolumeThreshold_th12k) ? 12288 :\
  ((threshold == rrc_TrafficVolumeThreshold_th16k) ? 16384 :\
  ((threshold == rrc_TrafficVolumeThreshold_th24k) ? 24576 :\
  ((threshold == rrc_TrafficVolumeThreshold_th32k) ? 32678 :\
  ((threshold == rrc_TrafficVolumeThreshold_th48k) ? 49152 :\
  ((threshold == rrc_TrafficVolumeThreshold_th64k) ? 65536 :\
  ((threshold == rrc_TrafficVolumeThreshold_th96k) ? 98304 :\
  ((threshold == rrc_TrafficVolumeThreshold_th128k) ? 131072 :\
  ((threshold == rrc_TrafficVolumeThreshold_th192k) ? 196608 :\
  ((threshold == rrc_TrafficVolumeThreshold_th256k) ? 262144 :\
  ((threshold == rrc_TrafficVolumeThreshold_th384k) ? 393216 :\
  (threshold == rrc_TrafficVolumeThreshold_th512k) ? 524288 : 786432 \
  ))))))))))))))))))))))) \
)


#define  TVM_RET_RLC_BUFFER_PAYLOAD(payload)  \
(  \
 (payload == 0) ? rrc_RLC_BuffersPayload_pl0 : \
  (((payload > 0) && (payload <= 4)) ? rrc_RLC_BuffersPayload_pl4 : \
  (((payload > 4) && (payload <= 8)) ? rrc_RLC_BuffersPayload_pl8 :\
  (((payload > 8) && (payload <= 16)) ? rrc_RLC_BuffersPayload_pl16 :\
  (((payload > 16) && (payload <= 32)) ? rrc_RLC_BuffersPayload_pl32 :\
  (((payload > 32) && (payload <= 64)) ? rrc_RLC_BuffersPayload_pl64 :\
  (((payload > 64) && (payload <= 128)) ? rrc_RLC_BuffersPayload_pl128 :\
  (((payload > 128) && (payload <= 256)) ? rrc_RLC_BuffersPayload_pl256 :\
  (((payload > 256) && (payload <= 512)) ? rrc_RLC_BuffersPayload_pl512 :\
  (((payload > 512) && (payload <= 1024)) ? rrc_RLC_BuffersPayload_pl1024 :\
  (((payload > 1024) && (payload <= 2048)) ? rrc_RLC_BuffersPayload_pl2k :\
  (((payload > 2048) && (payload <= 4096)) ? rrc_RLC_BuffersPayload_pl4k :\
  (((payload > 4096) && (payload <= 8192)) ? rrc_RLC_BuffersPayload_pl8k :\
  (((payload > 8192) && (payload <= 16384)) ? rrc_RLC_BuffersPayload_pl16k :\
  (((payload > 16384) && (payload <= 32768)) ? rrc_RLC_BuffersPayload_pl32k :\
  (((payload > 32768) && (payload <= 65536)) ? rrc_RLC_BuffersPayload_pl64k :\
  (((payload > 65536) && (payload <= 131702)) ? rrc_RLC_BuffersPayload_pl128k :\
  (((payload > 131702) && (payload <= 262144)) ? rrc_RLC_BuffersPayload_pl256k :\
  ((payload > 262144) && (payload <= 524288)) ? rrc_RLC_BuffersPayload_pl512 : rrc_RLC_BuffersPayload_pl1024 \
  ))))))))))))))))) \
)


#define  TVM_RET_AVG_BUFFER_PAYLOAD(payload)  \
(  \
 (payload == 0) ? rrc_AverageRLC_BufferPayload_pla0 : \
  (((payload > 0) && (payload <= 4)) ? rrc_AverageRLC_BufferPayload_pla4 : \
  (((payload > 4) && (payload <= 8)) ? rrc_AverageRLC_BufferPayload_pla8 :\
  (((payload > 8) && (payload <= 16)) ? rrc_AverageRLC_BufferPayload_pla16 :\
  (((payload > 16) && (payload <= 32)) ? rrc_AverageRLC_BufferPayload_pla32 :\
  (((payload > 32) && (payload <= 64)) ? rrc_AverageRLC_BufferPayload_pla64 :\
  (((payload > 64) && (payload <= 128)) ? rrc_AverageRLC_BufferPayload_pla128 :\
  (((payload > 128) && (payload <= 256)) ? rrc_AverageRLC_BufferPayload_pla256 :\
  (((payload > 256) && (payload <= 512)) ? rrc_AverageRLC_BufferPayload_pla512 :\
  (((payload > 512) && (payload <= 1024)) ? rrc_AverageRLC_BufferPayload_pla1024 :\
  (((payload > 1024) && (payload <= 2048)) ? rrc_AverageRLC_BufferPayload_pla2k :\
  (((payload > 2048) && (payload <= 4096)) ? rrc_AverageRLC_BufferPayload_pla4k :\
  (((payload > 4096) && (payload <= 8192)) ? rrc_AverageRLC_BufferPayload_pla8k :\
  (((payload > 8192) && (payload <= 16384)) ? rrc_AverageRLC_BufferPayload_pla16k :\
  (((payload > 16384) && (payload <= 32768)) ? rrc_AverageRLC_BufferPayload_pla32k :\
  (((payload > 32768) && (payload <= 65536)) ? rrc_AverageRLC_BufferPayload_pla64k :\
  (((payload > 65536) && (payload <= 13102)) ? rrc_AverageRLC_BufferPayload_pla128k :\
  (((payload > 13102) && (payload <= 262144)) ? rrc_AverageRLC_BufferPayload_pla256k :\
  ((payload > 262144) && (payload <= 524288)) ? rrc_AverageRLC_BufferPayload_pla512 : rrc_AverageRLC_BufferPayload_pla1024 \
  ))))))))))))))))) \
)



#define  TVM_RET_VAR_BUFFER_PAYLOAD(payload)  \
(  \
 (payload == 0) ? rrc_VarianceOfRLC_BufferPayload_plv0 : \
  (((payload > 0) && (payload <= 4)) ? rrc_VarianceOfRLC_BufferPayload_plv4 : \
  (((payload > 4) && (payload <= 8)) ? rrc_VarianceOfRLC_BufferPayload_plv8 :\
  (((payload > 8) && (payload <= 16)) ? rrc_VarianceOfRLC_BufferPayload_plv16 :\
  (((payload > 16) && (payload <= 32)) ? rrc_VarianceOfRLC_BufferPayload_plv32 :\
  (((payload > 32) && (payload <= 64)) ? rrc_VarianceOfRLC_BufferPayload_plv64 :\
  (((payload > 64) && (payload <= 128)) ? rrc_VarianceOfRLC_BufferPayload_plv128 :\
  (((payload > 128) && (payload <= 256)) ? rrc_VarianceOfRLC_BufferPayload_plv256 :\
  (((payload > 256) && (payload <= 512)) ? rrc_VarianceOfRLC_BufferPayload_plv512 :\
  (((payload > 512) && (payload <= 1024)) ? rrc_VarianceOfRLC_BufferPayload_plv1024 :\
  (((payload > 1024) && (payload <= 2048)) ? rrc_VarianceOfRLC_BufferPayload_plv2k :\
  (((payload > 2048) && (payload <= 4096)) ? rrc_VarianceOfRLC_BufferPayload_plv4k :\
  ((payload > 262144) && (payload <= 524288)) ? rrc_VarianceOfRLC_BufferPayload_plv8k : rrc_VarianceOfRLC_BufferPayload_plv16k \
  ))))))))))) \
)

#define  TVM_RET_TIME_TO_TRIGGER(time_to_trigger)  \
(  \
 (time_to_trigger == rrc_TimeToTrigger_ttt0) ? 0 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt10) ? 10 : \
  ((time_to_trigger == rrc_TimeToTrigger_ttt20) ? 20 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt40) ? 40 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt60) ? 60 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt80) ? 80 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt100) ? 100 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt120) ? 120 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt160) ? 160 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt200) ? 200 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt240) ? 240 :\
  ((time_to_trigger == rrc_TimeToTrigger_tt320) ? 320 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt640) ? 640 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt1280) ? 1280 :\
  ((time_to_trigger == rrc_TimeToTrigger_ttt2560) ? 2560 : 5000\
  )))))))))))))) \
)

#define  TVM_RET_PENDING_TIME_AFTER_TRIGGER(pending_time_after_trigger)  \
(  \
 (pending_time_after_trigger == rrc_PendingTimeAfterTrigger_ptat0_25) ? 250 : \
  ((pending_time_after_trigger == rrc_PendingTimeAfterTrigger_ptat0_5) ? 500 : \
  ((pending_time_after_trigger == rrc_PendingTimeAfterTrigger_ptat1) ? 1000 :\
  ((pending_time_after_trigger == rrc_PendingTimeAfterTrigger_ptat2) ? 2000 :\
  ((pending_time_after_trigger == rrc_PendingTimeAfterTrigger_ptat4) ? 4000 :\
  ((pending_time_after_trigger == rrc_PendingTimeAfterTrigger_ptat8) ? 8000 : 16000 \
  ))))) \
)


#define  TVM_RET_INTPT_TIME_AFTER_TRIGGER(intpt_time_after_trigger)  \
(  \
 (intpt_time_after_trigger == rrc_TX_InterruptionAfterTrigger_txiat0_25) ? 250 : \
  ((intpt_time_after_trigger == rrc_TX_InterruptionAfterTrigger_txiat0_5) ? 500 : \
  ((intpt_time_after_trigger == rrc_TX_InterruptionAfterTrigger_txiat1) ? 1000 :\
  ((intpt_time_after_trigger == rrc_TX_InterruptionAfterTrigger_txiat2) ? 2000 :\
  ((intpt_time_after_trigger == rrc_TX_InterruptionAfterTrigger_txiat4) ? 4000 :\
  ((intpt_time_after_trigger == rrc_TX_InterruptionAfterTrigger_txiat8) ? 8000 : 16000 \
  ))))) \
)




#define TVM_RET_REPORTING_MODE(mode) \
(\
(mode == rrc_TransferMode_acknowledgedModeRLC) ? ACK_MODE : UNACK_MODE \
)



#define TVM_RET_TRCH_TYPE(channel) \
(\
((channel == DCH) || (channel == EDCH))  ? T_rrc_UL_TrCH_Identity_dch :  T_rrc_UL_TrCH_Identity_rachorcpch \
)


/* Used for storing Reporting Criteria in RRC */
#define MEAS_RPT_CRIT(mode) \
(  \
 (mode == L1_PERIODIC_RPT) ? PERIODIC : \
  ((mode == L1_EVENT_TRIGGER_RPT) ? EVENT : \
  ((mode == L1_NO_RPT) ? ADDTL : NOT_AVAILABLE \
  ) \
 ) \
)

/* Default Traffic Vol Meas Identity  */
#define DEFAULT_TRAFFIC_VOL_MEAS_IDENITY  4
#define QUANTITY_NOT_PRESENT 0

#define MAX_TRAFFIC_VOLUME_MEASUREMENTS 4

#define  TVM_RET_MEAS_VALIDITY(validity)  \
(  \
 (validity == rrc_MeasurementValidity_ue_State_all_States) ? ALL_STATES  : \
  ((validity == rrc_MeasurementValidity_ue_State_all_But_Cell_DCH) ? ALL_STATES_BUT_DCH : STATE_DCH \
  ) \
)


/* Used for storing TVM Reporting Criteria in RRC */
#define TVM_RPT_CRIT(mode) \
(  \
 (mode == NO_REPORTING) ? ADDTL : \
  ((mode == PERIODIC_MODE) ? PERIODIC : \
  ((mode == EVENT_TRIGGERED_MODE) ? EVENT : NOT_AVAILABLE \
  ) \
 ) \
)

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)


/* Used for storing Reporting Criteria in RRC */
#define POS_RPT_CRIT(mode) \
(  \
 (mode == RRC_PERIODIC_RPT) ? PERIODIC : \
  ((mode == RRC_EVENT_TRIGGER_RPT) ? EVENT : ADDTL ) \
)

#endif

/* Converts hcs tmp offset 1 for FDD from ASN1 format to L1 format */
#define  RRCMEAS_RET_HCS_TMP_OFFSET1(t_off1)  \
(  \
 (t_off1 == rrc_TemporaryOffset1_to3) ? 3 : \
  ((t_off1 == rrc_TemporaryOffset1_to6) ? 6 : \
  ((t_off1 == rrc_TemporaryOffset1_to9) ? 9 :\
  ((t_off1 == rrc_TemporaryOffset1_to12) ? 12 :\
  ((t_off1 == rrc_TemporaryOffset1_to15) ? 15 :\
  ((t_off1 == rrc_TemporaryOffset1_to18) ? 18 :\
  (t_off1 == rrc_TemporaryOffset1_to21) ? 21 : 50 \
  ))))) \
)

/* Converts hcs tmp offset 2 for FDD from ASN1 format to L1 format */
#define  RRCMEAS_RET_HCS_TMP_OFFSET2(t_off2)  \
(  \
 (t_off2 ==  rrc_TemporaryOffset2_to2) ? 2 : \
  ((t_off2 == rrc_TemporaryOffset2_to3) ? 3 : \
  ((t_off2 == rrc_TemporaryOffset2_to4) ? 4 :\
  ((t_off2 == rrc_TemporaryOffset2_to6) ? 6 :\
  ((t_off2 == rrc_TemporaryOffset2_to8) ? 8 :\
  ((t_off2 == rrc_TemporaryOffset2_to10) ? 10 :\
  (t_off2 == rrc_TemporaryOffset2_to12) ? 12 : 50 \
  ))))) \
)

/* INTRA_FREQ_CELL_INFO_LIST variable that stores the cell info */
typedef struct
{
  uint16 psc;
  cell_position_e_type cell_position;  /* Vacant or Occupied */
} intra_freq_cell_list_struct_type;


typedef struct
{
  uint16 cell_position;
  uint16 psc;
} cells_for_meas_struct_type;



/* MEASUREMENT_IDENTITY variable. Required to trash measurements*/
typedef struct
{
  uint16 meas_id;
  uint16 meas_id_legacy;
  l1_meas_type_enum_type meas_type;
  meas_validity_enum_type meas_validity;
  rpt_crit_enum_type rpt_crit[MAX_NUM_CARR];
  /* If MCM owns this measurement */
  boolean mcm_owned;
  boolean meas_can_be_reported;

  uint8 additional_meas[MAX_ADDITIONAL_MEAS];

} intra_freq_meas_id_list_struct_type;



/* INTER_RAT_CELL_INFO_LIST variable that stores the cell info */
typedef struct
{
  uint16 cell_id;
  cell_position_e_type cell_position;  /* Vacant or Occupied */
  uint16 arfcn;
} inter_rat_cell_list_struct_type;

typedef enum
{
  RRCMEAS_IRAT_GSM
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  ,RRCMEAS_IRAT_EUTRA
#endif
} rrcmeas_interrat_type;

/* INTER_RAT_MEAS_ID_LIST VARIABLE */
typedef struct
{
  uint16 meas_id;
  uint16 meas_id_legacy;
  meas_validity_enum_type meas_validity;
  rpt_crit_enum_type rpt_crit;
  uint8 additional_meas[MAX_ADDITIONAL_MEAS];
  
#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  /*InterRAT Type, If it is GSM/EUTRA etc*/
  rrcmeas_interrat_type interrat_type;
} inter_rat_meas_id_list_struct_type;



#define MAX_QUALITY_MEAS 2

/* QUALITY_MEAS_ID_LIST VARIABLE */
typedef struct
{
  uint16 meas_id;
  uint16 meas_id_legacy;
  meas_validity_enum_type meas_validity;
  rpt_crit_enum_type rpt_crit;
  uint8 additional_meas[MAX_ADDITIONAL_MEAS];
} quality_meas_id_list_struct_type;


/*-------------------------------------------------------------------
STRUCTURE: tvm_meas_id_list_struct_type
Stores the Traffic Volume Meas Id's
--------------------------------------------------------------------*/
/* TRAFFIC VOLUME MEASUREMENT_IDENTITY variable. Required to trash measurements*/
typedef struct
{
  uint8 meas_id;
  uint8 meas_id_legacy;
  meas_validity_enum_type meas_validity;
  boolean meas_ctrl_msg;
  boolean block_rach_incl;
  rpt_crit_enum_type rpt_crit;
  uint8 additional_meas[MAX_ADDITIONAL_MEAS];
} tvm_meas_id_list_struct_type;



/* INTER_FREQ_MEAS_ID_LIST VARIABLE */
typedef struct
{
  uint16 meas_id;
  uint16 meas_id_legacy;
  meas_validity_enum_type meas_validity;
  rpt_crit_enum_type rpt_crit;
  /* If MCM owns this measurement */
  boolean mcm_owned;
  uint8 additional_meas[MAX_ADDITIONAL_MEAS];
} inter_freq_meas_id_list_struct_type;


#define FDD_ARFCN 16383
#define INVALID_INTER_FREQ 25000

/*
Dl Freq 2110 - 2170 MHz, then the default duplex distance is 190 MHz;
DL frequency 1930 - 1990 MHz,then the default duplex distance is 80 MHz;
*/

#define DL_BAND1_MIN 1930
#define DL_BAND1_MAX 1990
#define DL_BAND2_MIN 2110
#define DL_BAND2_MAX 2170


/* Used for storing Reporting Criteria in RRC */
#define RET_INTER_FREQ_RPT_CRIT(mode) \
(  \
 (mode == L1_INTER_FREQ_PERIODIC) ? PERIODIC : \
  ((mode == L1_INTER_FREQ_EVT_TRIG) ? EVENT : \
  ((mode == L1_INTRA_FREQ_EVT_TRIG) ? EVENT : ADDTL \
  ) \
 ) \
)

/* Inter Freq CELL_INFO_LIST */
typedef struct
{
  cell_position_e_type cell_position;
  uint16 psc;
  uint16 dl_freq;
} inter_freq_cell_list_struct_type;


typedef struct /* for each removed cell   */
{
  /* Freq and Scr code to remove */
  uint16    freq;
  uint16    psc;
} inter_freq_removed_cell_struct_type;


/* Max No of Internal Measurements */
#define MAX_INTERNAL_MEAS 8

/* UEIM_MEAS_ID_LIST VARIABLE */
typedef struct
{
  uint16 meas_id;
  uint16 meas_id_legacy;
  meas_validity_enum_type meas_validity;
  rpt_crit_enum_type rpt_crit;
  uint8 additional_meas[MAX_ADDITIONAL_MEAS];
} ueim_meas_id_list_struct_type;

typedef struct
{
  boolean interRAT_cell_info_present;
  uint8 interRAT_cell_info_ind;
} interRAT_cell_info_status_type;


#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)

/* Positioning MEAS_ID_LIST VARIABLE */
typedef struct
{
  uint8 meas_id;
  uint8  meas_id_legacy;
  meas_validity_enum_type meas_validity;
  rpt_crit_enum_type rpt_crit;
  rrc_positioning_method_enum_type pos_method_type;
  uint8 additional_meas[MAX_ADDITIONAL_MEAS];
} pos_meas_id_list_struct_type;

/* Declared to find out the owner of positioning module */
typedef struct
{
  uint8 meas_id;
  rrc_positioning_method_enum_type pos_method_type;
  rrc_meas_cmd_enum_type pos_meas_cmd;
} pos_info_struct_type;

#define  RET_POS_METHOD(pos_method)  \
(  \
 (pos_method == rrc_PositioningMethod_otdoa) ? RRC_OTDOA : \
 ((pos_method == rrc_PositioningMethod_gps) ? RRC_GPS : \
 ((pos_method == rrc_PositioningMethod_otdoaOrGPS) ? RRC_OTDOAorGPS : \
  RRC_CELLID  \
 )) \
)


#define  RET_POS_METHOD_TYPE(pos_method_type)  \
(  \
 (pos_method_type == rrc_UE_Positioning_MethodType_ue_Assisted) ? RRC_UE_ASSISTED : \
 ((pos_method_type == rrc_UE_Positioning_MethodType_ue_Based) ? RRC_UE_BASED : \
 ((pos_method_type == rrc_UE_Positioning_MethodType_ue_BasedPreferred) ? RRC_UE_BASED_PREFERED : \
  RRC_UE_ASSISTED_PREFERED  \
 )) \
)

#define  RET_POS_RPT_TYPE(pos_rpt_type)  \
( \
  (pos_rpt_type == rrc_TransferMode_acknowledgedModeRLC) ? MODE_ACKNOWLEDGED : MODE_UNACKNOWLEDGED \
)

#define RET_POS_ERR(pos_error)  \
(  \
 (pos_error == RRC_NOT_ENOUGH_OTDOA_CELLS) ? rrc_notEnoughOTDOA_Cells : \
 ((pos_error == RRC_NOT_ENOUGH_GPS_SATELLITES) ? rrc_notEnoughGPS_Satellites : \
 ((pos_error == RRC_ASSISTANCE_DATA_MISSING) ? rrc_assistanceDataMissing : \
 ((pos_error == RRC_METHOD_NOT_SUPPORTED) ? rrc_notAccomplishedGPS_TimingOfCellFrames : \
 ((pos_error == RRC_UNDEFINED_ERROR) ? rrc_undefinedError : \
 ((pos_error == RRC_REQUEST_DENIED_BY_USER) ? rrc_requestDeniedByUser : \
 ((pos_error == RRC_NOT_PROCESSED_AND_TIMEOUT) ? rrc_notProcessedAndTimeout : \
  rrc_referenceCellNotServingCell \
 )))))) \
)

/*
#define  RET_GET_POS_METHOD(meas_id)  \
((pos_meas_id_list[0] == meas_id) ? pos_meas_id_list[0].pos_method_type: \
pos_meas_id_list[1].pos_method_type)
*/


#endif
#ifdef  FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/

#ifdef FEATURE_GAN_3G
#error code not present
#endif


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY

#define RRC_MAX_UTRA_DED_PRI_LAYERS 64
#define RRC_MAX_EUTRA_DED_PRI_LAYERS 64
#define RRC_MAX_GSM_DED_PRI_LAYERS 128

typedef struct
{
  uint32 frequency;/*ARFCN/URAFCN/EARFCN*/
  uint8 priority;
#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
  geran_grr_band_ind_e band_ind;
#endif
}rrc_frequency_layer_priority_info_type;

typedef struct
{
  /*If the dedicated priority information is valid or not*/
  boolean is_valid;

  /*
    TRUE if the variables are updated and DED_PRI_CHANGE_IND is not yet processed 
    FALSE otherwise
  */
  boolean dedicated_priority_info_crit_sec;

  /*PLMN in which the dedicated priorties are received*/
  rrc_plmn_identity_type plmn_id;
/*validity Timer value in minutes, 0 if timer is not present*/
  uint64 validity_timer; 

  uint8 num_utra_layers;
  rrc_frequency_layer_priority_info_type utra_priority_layers[RRC_MAX_UTRA_DED_PRI_LAYERS];

  uint8 num_gsm_layers;
  rrc_frequency_layer_priority_info_type gsm_priority_layers[RRC_MAX_GSM_DED_PRI_LAYERS];

  uint8 num_eutra_layers;
  rrc_frequency_layer_priority_info_type eutra_priority_layers[RRC_MAX_EUTRA_DED_PRI_LAYERS];

  uint32 num_utra_tdd_freq;         /*!< Number of UTRA-TDD frequencies */
  lte_irat_reselection_priority_s utra_tdd_freq_list[LTE_IRAT_MAX_UTRAN_FREQ];
  uint32 num_hrdp_band_class;
  lte_irat_cdma_reselection_priority_s cdma_hrpd_list[LTE_IRAT_MAX_CDMA_BAND_CLASS];
  uint32 num_1x_band_class;
  lte_irat_cdma_reselection_priority_s cdma_1x_list[LTE_IRAT_MAX_CDMA_BAND_CLASS];
}rrc_dedicated_priority_info_type;
#endif

#ifdef FEATURE_LTE_TO_WCDMA

typedef struct
{
  boolean is_deprio_db_valid;
  /*! Indicates if all EUTRA neighbor freqs should be deprioritized */
  boolean  depri_all_freq;
 
  /*! Number of freqs being deprioritized; Will be set to a valid value
    only if the depri_all_freq boolean is set to FALSE
    If num_depri_freq = 0 and depri_all_freq = FALSE, this implies
    LTE deprioritization is cleared*/
  uint8   num_depri_freq;
  
  /*! Set of deprioritized EUTRA frequencies. This will be valid only
    if the depri_all_freq boolean is set to FALSE */
  uint32 depri_freqs[WCDMA_RRC_MAX_LTE_DEPRI_FREQ];
}rrc_deprio_db_type;

#endif

#ifdef FEATURE_WCDMA_TO_LTE
typedef struct
{
  uint32 earfcn;
}rrcirat_eutra_freq_type;

#define RRCMEAS_GET_EUTRA_MEAS_BANDWIDTH(bandwidth)\
    ((bandwidth == rrc_EUTRA_MeasurementBandwidth_mbw6) ? l1_mbw6 :\
      ((bandwidth == rrc_EUTRA_MeasurementBandwidth_mbw15) ? l1_mbw15 :\
        ((bandwidth == rrc_EUTRA_MeasurementBandwidth_mbw25) ? l1_mbw25: \
          ((bandwidth == rrc_EUTRA_MeasurementBandwidth_mbw50) ? l1_mbw50: \
            (bandwidth == rrc_EUTRA_MeasurementBandwidth_mbw75) ? l1_mbw75 : l1_mbw100\
            )\
          )\
        )\
      )\


#endif
/*===================================================================
                      DATA STRUCTURES Type
====================================================================*/


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/


/*===========================================================================

FUNCTION     RRCMEAS_PROCESS_INITIAL_STATE

DESCRIPTION

  Processes RRC_CELL_SELECTED_MEAS_REQ received in
  RRCMEAS_INITIAL state of RRC Measurements procedure.
  RRC_CELL_SELECTED_MEAS_REQ from cell selection procedure
  would come in this state which would cause RRC Measurement
  to transition from RRCMEAS INITIAL state to RRCMEAS IDLE
  state and give indication to L1 to start meas based on SIB11.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

    None

===========================================================================*/
void rrcmeas_process_initial_state
(
rrc_cmd_type*  cmd_ptr
);


/*===========================================================================

FUNCTION     RRCMEAS_PROCESS_IDLE_STATE

DESCRIPTION

  Processes all the messages received in RRCMEAS_IDLE state
  of RRC Measurements procedure.

DEPENDENCIES

    None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcmeas_process_idle_state
(
rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_CELL_DCH_STATE

DESCRIPTION

  Processes all events that are dispatched to RRC Measurement
  Establishment procedure in RRCMEAS DCH state.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_cell_dch_state
(
rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION     RRCMEAS_PROCESS_CELL_FACH_STATE

DESCRIPTION

  Processes all the messages received in RRCMEAS FACH state
  of RRC Measurements procedure.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_cell_fach_state
(
rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION    RRCMEAS_PROCESS_PCH_STATE

DESCRIPTION

  Processes all the messages received in RRCMEAS PCH state
  of RRC Measurements procedure.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_pch_state
(
rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION rrcmeas_read_sib11

DESCRIPTION

  Reads SIB11 from the current active serving cell for all the measurements.
  If SIB11 information is incorrect, meas setup command is not send to L1/MAC
  and UE does not do any measurements..

DEPENDENCIES

  SIB must have been stored by the SIB procedure

RETURN VALUE

  Success if SIB11 has been correctly read
  Failure if SIB11 has not been correctly read

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_read_sib11
(
void
);


/*===========================================================================

FUNCTION rrcmeas_read_sib12

DESCRIPTION

  Caller must check the sib12_present before calling this fn.
  At present, this fn is only called on state transition
  from Idle to connected mode (CELL_DCH). It build up the
  new intra freq cell list from SIB11 cell list stored
  in rrcmeas_sib11 struct and from the cell info
  given. L1 would perform meas based on the new constructed cell
  list

DEPENDENCIES

  SIB12 must have been received by the camping cell.

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void  rrcmeas_read_sib12
(
  rrc_SysInfoType12 *sib12_ptr,
  l1_meas_sib_parm_struct_type* rrcmeas_sib12_ptr
);

/*===========================================================================

FUNCTION          RRCMEAS_SEND_L1_CMD

DESCRIPTION

  This function sends measurement setup requests to L1.
  Since all L1 cmds go via LLC, the LLC command
  RRC_LLC_CMD_REQ actaully encapsules the real L1
  command. This is the LLC command funneling mechanism.
  It frees allocated command and embedded buffers when failure
  is detected before sending the command.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcmeas_send_l1_cmd
(
l1_meas_sib_parm_struct_type* rrcmeas_sib_ptr,
l1_meas_ctrl_parm_struct_type* rrcmeas_mcm_ptr,
l1_meas_trans_type* rrcmeas_trans_ptr,
l1_meas_ctrl_enum_type  meas_choice
);

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*===========================================================================

FUNCTION          RRCMEAS_SEND_DEFERRED_MEAS_REQ

DESCRIPTION

 This function sends CPHY_MEASUREMENT_REQ to L1 with parm type as
 L1_DEFERRED_MEASUREMENT_FROM_SIB. This parm type is chosen when 
 SIB11 hasn't been read. Depending on the state transition, meas identity 
 and cell info list are to be cleared. Also, meas quan should be chosen appropriately
 depending on whether SIB3 has been read or not.
 
 Since all L1 cmds go via LLC, the LLC command
 RRC_LLC_CMD_REQ actaully encapsules the real L1
 command. This is the LLC command funneling mechanism.
 It frees allocated command and embedded buffers when failure
 is detected before sending the command.

DEPENDENCIES

 None.

RETURN VALUE

 None.

SIDE EFFECTS

 None.

===========================================================================*/

void rrcmeas_send_deferred_meas_req
(
  uint8  l1_deferred_meas_from_sib_action
);
#endif

/*===========================================================================

FUNCTION     RRCMEAS_SEND_INTRA_FREQ_MEAS_REPORT_TO_UTRAN

DESCRIPTION

  Converts the intra-freq measurement report received
  From L1 into the measurement report structure format
  expected by UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  void

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_send_intra_freq_meas_report_to_utran
(
  l1_meas_rpt_struct_type* l1_meas_report_ptr
);

/*===========================================================================

FUNCTION     RRCMEAS_PROCESS_QMI_MTC_CFG_REQ

DESCRIPTION

This function take care of processing MTC_CFG request from QMI and sending M2M_CMD_IND to WL1.


DEPENDENCIES

 None.

RETURN VALUE

 None.

SIDE EFFECTS

 None

===========================================================================*/

void rrcmeas_process_qmi_mtc_cfg_req(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION          RRCMEAS_FILL_MEAS_ID

DESCRIPTION

 This function fills the measurement identity depending on whether the extended
 measurements are supported or not

DEPENDENCIES

 None.

RETURN VALUE
 None


SIDE EFFECTS

 None.

===========================================================================*/
void rrcmeas_fill_meas_id
(
  rrc_MeasurementReport *msg_ptr,
  uint16 meas_id,
  uint8 meas_type
);


/*===========================================================================

FUNCTION  RRCMEAS_PROCESS_CELL_SELECTED_MEAS_REQ_FOR_IDLE_STATE

DESCRIPTION

  Processes new cell req from CSP in RRCMEAS Idle state.
  Reads new SIB paramteres and sends new measurement
  request to L1 based on the new SIB's. L1 on
  receiving new SIB's deletes meas based on old SIB's

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_cell_selected_meas_req_for_idle_state
(
void
);

/*===========================================================================

FUNCTION  rrcmeas_handle_dch_to_pch_state_change

DESCRIPTION

  Process DCH to PCH state transition.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrcmeas_handle_dch_to_pch_state_change(void);



/*===========================================================================

FUNCTION    RRCMEAS_PROCESS_STATE_CHANGE_IND_IN_IDLE_STATE

DESCRIPTION

  Processes state change indication received in idle mode.

DEPENDENCIES

  None.

RETURN VALUE

  void

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_state_change_ind_in_idle_state
(
rrc_cmd_type* cmd_ptr
);


/*===========================================================================

FUNCTION     RRCMEAS_PROCESS_STATE_CHANGE_IND_IN_DCH_STATE

DESCRIPTION

  Processes state change indication message received in CELL_DCH
  Also changes the RRCMEAS procedure state from DCH to the
  desired state.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_state_change_ind_in_dch_state
(
rrc_cmd_type* cmd_ptr
);


/*===========================================================================

FUNCTION    RRCMEAS_PROCESS_CPHY_MEAS_IND_IN_DCH_STATE

DESCRIPTION

  Processes measurement indication received from L1
  in CELL_DCH state.

DEPENDENCIES

  None.

RETURN VALUE

  void

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_cphy_meas_ind_in_dch_state
(
rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION    RRCMEAS_PROCESS_DOWNLINK_L2_ACK

DESCRIPTION

  Processes ack received from layer2 of the meas report transmitted to UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  void

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_downlink_l2_ack
(
rrc_cmd_type *cmd_ptr
);



/*===========================================================================

FUNCTION  RRCMEAS_FILL_RSCP_CELL_INFO

DESCRIPTION

  This function fills in Cell Info for a Non-HCS cell when
  Cell Selection Reselection Quality Measure is RSCP.
  The function processes Cell Info in ASN1 format and
  translates into the L1 RRC Interface format.

DEPENDENCIES

  Pointer to rrc_CellInfoSI_RSCP must be valid.

RETURN VALUE

  FAILURE: If passed pointer is NULL.
  SUCCESS: If able to analyze and fill Cell Info.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_rscp_cell_info
(
rrc_CellInfoSI_RSCP *sib_cell_info,
l1_cell_info_struct_type *l1_cell_info
,rrc_CellSelectReselectInfo_v590ext * rel5_ext_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_FILL_ECN0_CELL_INFO

DESCRIPTION

  This function fills in Cell Info for a Non-HCS cell when
  Cell Selection Reselection Quality Measure is ECN0.
  The function processes Cell Info in ASN1 format and
  translates into the L1 RRC Interface format.

DEPENDENCIES

  Pointer to rrc_CellInfoSI_ECN0 must be valid.

RETURN VALUE

  FAILURE: If passed pointer is NULL.
  SUCCESS: If able to analyze and fill Cell Info.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_ecn0_cell_info
(
rrc_CellInfoSI_ECN0 *sib_cell_info,
l1_cell_info_struct_type *l1_cell_info
,rrc_CellSelectReselectInfo_v590ext * rel5_ext_ptr
);


/*===========================================================================

FUNCTION          RRCMEAS_FILL_INTRA_FREQ_EVENT_CRITERIA

DESCRIPTION

  This function fills in intra-freq event criteria for a Non-HCS cell when
  Cell Selection Reselection Quality Measure is RCSP.
  The function processes intra-ftreq event criteria Info in ASN1 format
  and translates into the L1 RRC Interface format.

DEPENDENCIES

  The passed pointers must be valid.

RETURN VALUE

  No of events.

SIDE EFFECTS

  None

===========================================================================*/

uint16 rrcmeas_fill_intra_freq_event_criteria
(
rrc_IntraFreqReportingCriteria  *rrc_intra_freq_reporting_criteria_ptr,
l1_intra_freq_event_crit_struct_type *l1_intra_freq_event_crit_ptr
, rrc_MeasurementControl_v590ext_IEs *rel5_ext_ptr
, rrc_SysInfoType11_v590ext_IEs *sib11_rel5_ext_ptr
, rrc_SysInfoType12_v590ext_IEs *sib12_rel5_ext_ptr
);


/*===========================================================================

FUNCTION      RRCMEAS_FILL_NO_OF_REPORTING_CELLS

DESCRIPTION
  This function fills no of reporting cells for a Non-HCS cell
  for an event triggered criteria for RCSP or ECNO.
  The function processes this Info in ASN1 format and
  translates into the L1 RRC Interface format.

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If unable to fill no of reporting cells correctly
  SUCCESS: If no of reporting cells filled correctly

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_no_of_reporting_cells
(
rrc_ReportingCellStatus  *rrc_rpt_cell_status_ptr,
l1_meas_rpt_cell_status_struct_type  *l1_rpt_cell_status_ptr
);


/*===========================================================================

FUNCTION          RRCMEAS_FILL_DCH_STATE_INFO

DESCRIPTION

  This function fills DCH state info for a Non-HCS cell
  for RCSP/ECNO. The function processes this Info in ASN1 format
  and translates into the L1 RRC Interface format.

DEPENDENCIES

  None.

RETURN VALUE

  Success if DCH state info filled correctly
  Failure if unable to fill DCH state info correctly

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_dch_state_info
(
rrc_ReportingInfoForCellDCH* rrc_reporting_info_for_cell_dch_ptr,
l1_intra_freq_dch_rpt_info_struct_type* l1_dch_rpt_info_ptr
, rrc_SysInfoType11_v590ext_IEs *sib11_rel5_ext_ptr
, rrc_SysInfoType12_v590ext_IEs *sib12_rel5_ext_ptr
);


/*===========================================================================

FUNCTION     RRCMEAS_PROCESS_REMOVED_INTRA_FREQ_CELLS

DESCRIPTION

 This function fills the removed intra-freq cells for HCS Non-HCS cells
 for RSCP and ECN0. The function processes this Info in ASN1 format and
  translates into the L1 RRC Interface format.

DEPENDENCIES

  None

RETURN VALUE

 FAILURE: If unable to fill removed intra freq cell info correctly
  SUCCESS: If removed intra freq cell info filled correctly

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_removed_intra_freq_cells
(
rrc_RemovedIntraFreqCellList *removed_intra_freq_cell_list_ptr,
l1_intra_freq_cell_list_struct_type *l1_intra_freq_cell_ptr
);


/*===========================================================================

FUNCTION    RRCMEAS_SET_INTRA_FREQ_EVENT_ID

DESCRIPTION

  Sets the event id in measurement report from L1 format to ASN1 format

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE if event no is incorrect
  SUCCESS if event no is correct

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_set_intra_freq_event_id
(
uint16 event_id,
rrc_EventIDIntraFreq* rrcmeas_intra_freq_id_results_ptr
);

/*===========================================================================

FUNCTION    CHECK_FOR_RACH_REPORTING

DESCRIPTION

  Determines if RACH report can be appended to RACH messages.
  The calling function should call rrc_meas_append_meas_rpt_on_rach
  only it has received APPEND_RRC_MEAS_RACH_REPORT value.

DEPENDENCIES

  None.

RETURN VALUE

  APPEND_RRC_MEAS_RACH_REPORT if RACH report can be appended
  NO_RRC_MEAS_RACH_REPORT if RACH report cannot be appended

SIDE EFFECTS

  None

===========================================================================*/

rach_report_crit_e_type rrcmeas_check_for_rach_reporting
(
void
);


/*===========================================================================

FUNCTION    RRCMEAS_CLEAR_SIB11

DESCRIPTION

  Initialises SIB11 stored variable.

DEPENDENCIES

  None.

RETURN VALUE

  void

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_clear_sib11
(
void
);


/*===========================================================================

FUNCTION    RRCMEAS_BUILD_SIB12_INTRA_FREQ_LIST_FROM_SIB11

DESCRIPTION

  Builds neighbouring cell list from SIB11 for SIB12.

DEPENDENCIES

  None.

RETURN VALUE

  void

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_build_sib12_intra_freq_list_from_sib11
(
rrc_RemovedIntraFreqCellList *removedIntraFreqCellList,
l1_intra_freq_cell_list_struct_type* sib11_cell_list_ptr,
l1_intra_freq_cell_list_struct_type* sib12_cell_list_ptr,
intra_freq_cell_list_struct_type  *tmp_intra_freq_cell_list
);

/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_INTRA_FREQ_PERIODIC_MEAS_RESULTS

DESCRIPTION

  Processes intra-freq periodic measurements and appends
  to Measured Results in Measurement Report

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrcmeas_append_intra_freq_periodic_meas_results
(
l1_intra_freq_meas_rpt_struct_type* l1_intra_freq_periodic_meas_ptr,
rrc_MeasurementReport *meas_rpt_ptr
,rrc_IntraFreqMeasuredResultsList** rrc_intra_freq_meas_results_ptr
, boolean addtl_meas

);


/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_MEAS_CTRL_MSG

DESCRIPTION

  Processes meas control message in DCH state and directs L1 to setup/modify/
  release measurements accordingly. If the message determined is incorrect,
  send a measurement control failure message to UTRAN

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrcmeas_process_meas_ctrl_msg
(
rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_READ_MCM_MEASUREMENTS

DESCRIPTION

  Reads different measurements, validate the paramters of each meas and sends
  a measurement control message to L1 to do meas based on new parameters.
  Send a measurement control failure message to UTRAN on finding an incorrect/
  unsupported measurement

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrcmeas_read_mcm_measurements
(
rrc_MeasurementControl_r3_IEs* mcm_ptr
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
, void* decoded_msg_ptr
#endif
,rrc_DL_DCCH_Message* dcch_msg_ptr
,uint8 meas_ctrl_tr_id
);


/*===========================================================================

FUNCTION  RRCMEAS_FILL_MCM_CELL_INFO

DESCRIPTION

  This function fills in Cell Info received from a
  Measurement Control Message. The function processes
  Cell Info in ASN1 format and translates into the L1
  RRC Interface format.

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If unable to fill cell info correctly
  SUCCESS: If cell info is filled correctly

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_mcm_cell_info
(
rrc_CellInfo* mcm_cell_info_ptr,
l1_cell_info_struct_type* l1_cell_info_ptr
);

/*===========================================================================

FUNCTION  RRCMEAS_FILL_REFERENCE_TIME_DIFFERENCE

DESCRIPTION

  This function fills in Cell Info received from a
  Measurement Control Message. The function processes
  Cell Info in ASN1 format and translates into the L1
  RRC Interface format.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrcmeas_fill_reference_time_difference
(
rrc_ReferenceTimeDifferenceToCell* time_diff_ptr,
l1_cell_info_struct_type* l1_cell_ptr
);


/*===========================================================================

FUNCTION  RRCMEAS_FILL_COMMON_MCM_INTRA_F_INFO

DESCRIPTION

  This function fills in common Cell Info for meas setup and
  meas modify command type in a Measurement Control Message.
  The function processes Cell Info in ASN1 format and translates
  into the L1RRC Interface format.

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If unable to fill common mcm info correctly
  SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_status_e_type rrcmeas_fill_common_mcm_intra_f_info
(
rrc_IntraFrequencyMeasurement* intra_f_meas_ptr,
l1_meas_ctrl_parm_struct_type *l1_mcm 
, rrc_MeasurementControl_v590ext_IEs *rel5_ext_ptr
);


/*===========================================================================

FUNCTION          RRCMEAS_FILL_INTRA_FREQ_REP_QUAN

DESCRIPTION

  Fills intra freq reporting info obtained from MCM.
  The function processes this Info in ASN1 format and translates
  into the L1 RRC Interface format.

DEPENDENCIES

  None.

RETURN VALUE

  Success if intra freq reporting quantity filled correctly
  Failure if unable to fill intra freq reporting quantity

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_intra_freq_rep_quan
(
rrc_IntraFreqReportingQuantity* rrc_intra_freq_reporting_quan_ptr,
l1_intra_freq_meas_ctrl_struct_type* l1_intra_freq_mcm_ptr
);


/*===========================================================================

FUNCTION     RRCMEAS_SEND_MEAS_CTRL_FAILURE_MSG

DESCRIPTION

  Sends meas control failure message to UTRAN

DEPENDENCIES

  None.

RETURN VALUE

  void


SIDE EFFECTS

  None

===========================================================================*/
void rrcmeas_send_meas_ctrl_failure_msg
(
rrcmeas_mcf_struct_type* mcf_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_L1_ACK

DESCRIPTION

  Processes meas control message in DCH state and directs L1 to setup/modify/
  release measurements accordingly. If the message determined is incorrect,
  send a measurement control failure message to UTRAN

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_l1_ack
(
rrc_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION   RRCMEAS_FIND_PSC

DESCRIPTION

  Finds the primary scrambling code of the intra freq cell id

DEPENDENCIES

  None

RETURN VALUE

  PSC_ABSENT if psc absent
  PSC_PRESENT if psc present

SIDE EFFECTS

  None
===========================================================================*/

psc_e_type rrcmeas_find_psc
(
uint8   freq_type,
uint32* intra_freq_cell_id_ptr,
uint16* psc_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_UPDATE_INTRA_FREQ_CELL_LIST

DESCRIPTION

  Updates the intra freq cell list with new values of intra freq
  cell id. This is done on receving SIB system info or on
  receiving Meas Ctrl Message. The calling function gives
  position_no_ptr indicating where to store the psc. If it
  points to AVAILABLE_CELL_POSITION, then store psc in the first
  available position in intra_freq_cell_list and and mark that
  position as occupied. If position_no_ptr points to some other
  value then store psc at the position given by that value


DEPENDENCIES

  None

RETURN VALUE

  RRCMEAS_FAILURE if intra freq cell list cannot be updated due to incorrect
  value of the intra freq cell id
  RRCMEAS_SUCCESS if intra freq cell list can be successfully updated

SIDE EFFECTS

  None
===========================================================================*/

rrcmeas_status_e_type rrcmeas_update_intra_freq_cell_list
(
uint32* position_no_ptr,
uint16* psc_ptr
);

/*===========================================================================

FUNCTION   RRCMEAS_REMOVE_CELLID

DESCRIPTION

  Delete intra-freq cell id from the intra freq cell list

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrcmeas_remove_cellid
(
uint8    freq_type,
uint32* intra_freq_cell_id_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_CHECK_CELL_ORDER_ECN0

DESCRIPTION

  Checks if the cells received in SIB11/12 are in the correct order. By correct
  order, one means that the cells should be received in increasing order of
  their cell id's. If not then, ignore these cells and return RRCMEAS_FAIL.
  Consequently the cell list would not be sent to L1 as CELL_LIST_INCL
  would be set to FALSE in meas command sent to L1

DEPENDENCIES

  None

RETURN VALUE

  RRCMEAS_FAIL if cells are not in ascending order
  RRCMEAS_SUCCESS if cells are in ascending order

SIDE EFFECTS

  None
===========================================================================*/

rrcmeas_status_e_type rrcmeas_check_cell_order_ecn0
(
  rrc_NewIntraFreqCellSI_List_ECN0   *ecn0_intra_freq_cell_list_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_CHECK_CELL_ORDER_RSCP

DESCRIPTION

  Checks if the cells received in SIB11/12 are in the correct order. By correct
  order, one means that the cells should be received in increasing order of
  their cell id's. If not then, ignore these cells and return RRCMEAS_FAIL.
  Consequently the cell list would not be sent to L1 as CELL_LIST_INCL
  would be set to FALSE in meas command sent to L1

DEPENDENCIES

  None

RETURN VALUE

  RRCMEAS_FAIL if cells are not in ascending order
  RRCMEAS_SUCCESS if cells are in ascending order

SIDE EFFECTS

  None
===========================================================================*/

rrcmeas_status_e_type rrcmeas_check_cell_order_rscp
(
 rrc_NewIntraFreqCellSI_List_RSCP   *rscp_intra_freq_cell_list_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_CHECK_CELL_ORDER_MCM

DESCRIPTION

  Checks if the cells received in Measurement Ctrl Msg are in correct order.
  By correct order, one means that the cells should be received in
  increasing order of their cell id's. If not then, ignore these cells and
  return RRCMEAS_FAIL. Consequently the cell list would not be sent to L1
  as CELL_LIST_INCL would be set to FALSE in meas command sent to L1

DEPENDENCIES

  None

RETURN VALUE

  RRCMEAS_FAIL if cells are not in ascending order
  RRCMEAS_SUCCESS if cells are in ascending order

SIDE EFFECTS

  None
===========================================================================*/

rrcmeas_status_e_type rrcmeas_check_cell_order_mcm
(
 rrc_NewIntraFreqCellList   *mcm_intra_freq_cell_list_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_UPDATE_INTRA_FREQ_CELL_LIST_MCM

DESCRIPTION

  Updates the intra freq cell list with new values of intra freq
  cell id. This is done on receving Meas Ctrl Message. The calling
  function givesposition_no_ptr indicating where to store the psc. If it
  points to AVAILABLE_CELL_POSITION, then store psc in the first
  available position in intra_freq_cell_list and and mark that
  position as occupied. If position_no_ptr points to some other
  value then store psc at the position given by that value

DEPENDENCIES

  None

RETURN VALUE

  RRCMEAS_FAILURE if intra freq cell list cannot be updated due to incorrect
  value of the intra freq cell id
  RRCMEAS_SUCCESS if intra freq cell list can be successfully updated

SIDE EFFECTS

  None
===========================================================================*/

rrcmeas_status_e_type rrcmeas_update_intra_freq_cell_list_mcm
(
uint8   freq_type,
uint32* position_no_ptr,
uint16* psc_ptr,
uint16* psc_to_remove_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_SET_INTRA_FREQ_MEAS_IDENTITY

DESCRIPTION

  Set intra freq meas identity variable from SIB information.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrcmeas_set_intra_freq_meas_identity
(
l1_intra_freq_sib_struct_type* l1_intra_freq_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_RETRIEVE_MEAS_OWNER

DESCRIPTION

  Finds the owner of measurement identity given in a MCM.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

rrcmeas_layer_e_type rrcmeas_retrieve_meas_owner
(
 rrc_MeasurementCommand* mcm_ptr,
 uint32* meas_id_ptr
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
 ,pos_info_struct_type* pos_info_ptr
#endif
);


/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO

DESCRIPTION

    Process traffic volume information in a Meas
    Ctrl Message. Also sends a Meas Ctrl Failure
    Message in case of an error


DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/

rrctvm_status_e_type rrctvm_process_traffic_vol_info
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
, uint32 meas_ctrl_tr_id
);


/*===========================================================================

FUNCTION  RRCMEAS_PROCESS_MAC_ACK

DESCRIPTION

  Processes ack from MAC

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrcmeas_process_mac_ack
(
 rrc_cmd_type* cmd_ptr
);



/*===========================================================================

FUNCTION     RRCTVM_PROCESS_SIB_INFO

DESCRIPTION

  Process traffic volume information from SIB11/12.
  Send a Measurement command to MAC to start
  doing traffic volume Measurements


DEPENDENCIES

  None.

RETURN VALUE

  rrctvm_status_e_type

SIDE EFFECTS

  None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_sib_info
(
  rrc_MeasurementControlSysInfo* sib_meas_ptr
);


/*===========================================================================

FUNCTION     RRCTVM_READ_TRAFFIC_VOL_PARAMS

DESCRIPTION

  Read traffic volume parameters in a Meas
  Ctrl Message. Also sends a Meas Ctrl Failure
  Message in case of an error


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/
rrctvm_status_e_type rrctvm_read_traffic_vol_params
(
  rrc_TrafficVolumeMeasurement* rrc_mcm_ptr,
  mac_ul_traffic_meas_config_type*  mac_config_ptr
);


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_EVENT_CRITERIA

DESCRIPTION

  Sets the traffic volume event criteria for MAC.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

rrctvm_status_e_type rrctvm_process_event_criteria
(
  rrc_TransChCriteriaList * event_crit_ptr,
  mac_ul_traffic_event_trigger_cfg_type* mac_event_crit_ptr
#ifdef FEATURE_WCDMA_HS_RACH
,mac_ul_traffic_meas_config_type * mac_config_ptr
#endif
);


/*===========================================================================

FUNCTION   RRCTVM_SEND_TRAFFIC_VOL_MEAS_REPORT

DESCRIPTION

  Sends the traffic volume Measurement Report

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrctvm_send_traffic_vol_meas_report
(
  rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION          RRCTVM_SEND_MAC_CMD

DESCRIPTION

  This function sends measurement requests to MAC.
  Since all MAC cmds go via LLC, the LLC command
  RRC_LLC_CMD_REQ actaully encapsules the real L1
  command. This is the LLC command funneling mechanism.
  It frees allocated command and embedded buffers when failure
  is detected before sending the command.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrctvm_send_mac_cmd
(
mac_ul_traffic_meas_config_type* traffic_vol_params_ptr
);



/*===========================================================================

FUNCTION   RRCTVM_SET_MEAS_IDENTITY

DESCRIPTION

  Set traffic volume meas identity variable from SIB information.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrctvm_set_meas_identity
(
  uint16* meas_id_ptr,
  rrc_TrafficVolumeMeasSysInfo* sib_meas_ptr
);


/*===========================================================================

FUNCTION   RRCTVM_INITIALIZE

DESCRIPTION

  Initializes traffic volume configuration parameters.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrctvm_initialize
(
  void
);


/*===========================================================================

FUNCTION     RRCMEAS_PROCESS_STATE_CHANGE_IND_IN_FACH_STATE

DESCRIPTION

  Processes state change indication message received in CELL_FACH
  Also changes the RRCMEAS procedure state from FACH to the
  desired state.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_state_change_ind_in_fach_state
(
rrc_cmd_type* cmd_ptr
);



/*===========================================================================

FUNCTION   RRCMEAS_CONFIG_L1_MEAS_ON_TRANSITION_BY_OTHER_MSG

DESCRIPTION

  Processes the secnario when the transition from DCH-->FACH is
  as a result of radio link failure and the default intra-freq
  measurement needs to be deleted. CELL_INFO_LIST variable
  is cleared. The function tries to read
  SIB11 of the new cell and if SIB11 is valid, it updates the
  MEASUREMENT_IDENTIY variable and also re-configures L1 with
  with Stop & Overwrite command. CELL_INFO_LIST is also updated
  with the new SIB11.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  If SIB11 of new cell is not valid, then no intra-freq measurements would be done.

===========================================================================*/

void rrcmeas_config_l1_meas_on_transition_by_other_msg
(
 void
);


/*===========================================================================

FUNCTION   RRCMEAS_SAVE_L1_MEAS

DESCRIPTION

  Processes the secnario when the transition from DCH-->FACH
  transition determines that intra-freq measurement has to be
  saved and no measurement reporting for this measurement has to be done.
  Since the move may result in selection of a new cell, CELL_INFO_LIST
  has to be configured again. This requires reading cells from SIB11
  again and updating the CELL_INFO_LIST

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  If SIB11 is not valid, then no intra-freq measurements would be done.

===========================================================================*/

void rrcmeas_save_l1_meas
(
 void
);


/*===========================================================================

FUNCTION   RRCTVM_GET_SIBS_IN_CONNECTED_STATE

DESCRIPTION

  Processes the scenario when the transition from DCH-->FACH
  transition determines that traffic volume measurement for the
  default measurement has to be overwritten by new SIB's from
  the new cell. So read the new SIB's for traffic volume measurements


DEPENDENCIES

  None

RETURN VALUE

  TVM_SUCCESS if SIB 11/12 is successfully read for Traffic Volume
  TVM_FAIL if SIB 11/12 is successfully read for Traffic Volume

SIDE EFFECTS

  If SIB11 is not valid, then  traffic volume measurements may not be done.

===========================================================================*/

rrctvm_status_e_type rrctvm_get_sibs_in_connected_state
(
  void
);


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_DCH_TO_FACH

DESCRIPTION

  The main handler function that handles the processing of
  traffic volume measurements when RRC transitions from
  DCH-->FACH. Check the MEASUREMENT_IDENTITY variable and
  determine for each measurement what has to be done as per
  its validity. The function accordingly updates the
  MEASUREMENT_IDENTITY variable and sends a command to
  MAC to reconfigure the traffic volume measurements.
  It also processes the scenario when no traffic volume
  measurement has been assigned to the UE with a MEASUREMENT
  CONTROL message that is valid in CELL_FACH state (stored
  in the variable tvm_meas_id_list), that has the same identity
  as the one indicated in the "Traffic volume measurement
  system information


DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  If SIB11 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_process_dch_to_fach
(
 void
);


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_FACH_TO_DCH

DESCRIPTION

  The main handler function that handles the processing of
  traffic volume measurements when RRC transitions from
  FACH-->DCH. Check the MEASUREMENT_IDENTITY variable and
  determine for each measurement what has to be done as per
  its validity. The function accordingly updates the
  MEASUREMENT_IDENTITY variable and sends a command to
  MAC to reconfigure the traffic volume measurements.


DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  If SIB11 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_process_fach_to_dch
(
  void
);


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_TVM_SIBS_IN_CONNECTED_STATE

DESCRIPTION

  Invoked when cell reselection is done in CELL_FACH state
  and traffic volume measurements need to be updated as
  per the new SIB's. It reads the new traffic volume
  measurememt parameters from SIB11/12 and stores it in an
  internal data structure

DEPENDENCIES

  None

RETURN VALUE

  Returns TVM_SUCCESS if SIB11/12 successfully read.
  Returns TVM_FAIL if SIB11/12 successfully read.

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

rrctvm_status_e_type rrctvm_process_tvm_sibs_in_connected_state
(
  void
);


/*===========================================================================

FUNCTION   RRCTVM_GET_MEAS_ID_FROM_SIBS

DESCRIPTION

  Gets the default traffic volume measurement identity by peeking
  into SIB11/12 of SIB database.

DEPENDENCIES

  None

RETURN VALUE

  Returns DEFAULT_TRAFFIC_VOL_MEAS_IDENITY if SIB11/12 does not specify
  a measurement identity or return the measurememt identity specified
  in SIB11/12

  Returns INVALID_MEAS_ID if traffic volume system information is not present

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

uint16 rrctvm_get_meas_id_from_sibs
(
  void
);


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_TVM_ON_CELL_RESELECTION

DESCRIPTION

  Main handler that processes traffic volume measurements on cell
  reselection in connected mode. Checks the MEASUREMENT_IDENTITY
  variable for any measurement that is owned by UTRAN.
  It saves that measurement and tells MAC to store it. If it does
  not find any measurement in the database that corresponds to the
  default measurement, it reads SIB11/12 again to get the default
  measurement.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_process_tvm_on_cell_reselection
(
  void
);


/*===========================================================================

FUNCTION   RRCTVM_INITIALIZE_MEASUREMENT_IDENTITY

DESCRIPTION

  Handles initialization of MEASUREMNT_IDENTIY variable
  for Traffic Volume Measurements.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_initialize_measurement_identity
(
 void
);


/*===========================================================================

FUNCTION   RRCMEAS_INITIALIZE_CELL_LIST

DESCRIPTION

  Handles initialization of CELL_INFO_LIST variable
  for Intra Freq Measurements.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcmeas_initialize_cell_list
(
 void
);


/*===========================================================================

FUNCTION   RRCMEAS_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

  Handles initialization of MEASUREMNT_IDENTIY variable
  for Intra Freq Measurements.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcmeas_initialize_meas_id_list
(
 void
);

/*===========================================================================

FUNCTION   RRCTVM_CHECK_MEAS_ID_ABSENT

DESCRIPTION

  Checks if the traffic volume measurement identity is present
  in MEASUREMENT_IDENTITY variable.


DEPENDENCIES

  None

RETURN VALUE

  Success if Measurement identity is not present.
  Failure if Measurement identity is present.

SIDE EFFECTS

  None.

===========================================================================*/

rrctvm_status_e_type rrctvm_check_meas_id_absent
(
  void
);



/*===========================================================================

  FUNCTION     RRCMEAS_VALIDATE_INTRA_FREQ_REPORT

DESCRIPTION

  Processes the Intra-freq Measurement Report from L1 and checks if
  the parameters sent in the report are valid. If the parameter/s are
  found to be invalid, the measurement report is trashed.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  Trashing Measurement Report may result in Soft Hand Off not working properly

===========================================================================*/
rrcmeas_status_e_type rrcmeas_validate_intra_freq_report
(
 l1_meas_rpt_struct_type* l1_rpt_ptr
);



/*===========================================================================

FUNCTION   IRAT_READ_SIB11

DESCRIPTION

  This function reads Inter RAT Cell List from SIB 11 and
  stores it in inter_rat_meas_id_list. This Inter RAT
  Cell List is used for cell reselection. When UTRAN sends a meas
  in connected DCH mode, this cell list is overwritten.

DEPENDENCIES

  Pointer to rrc_InterRATCellInfoList_B must be valid.

RETURN VALUE

  FAILURE: If cell list is not in order
  SUCCESS: If able to analyze and fill Cell Info.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_read_sib11
(
  l1_meas_sib_parm_struct_type* rrc_inter_rat_sib_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_CHECK_INTER_RAT_CELL_ORDER

DESCRIPTION

  This function checks cell order received

DEPENDENCIES

  Pointer to rrc_InterRATCellInfoList_B must be valid.

RETURN VALUE

  FAILURE: If cell list is not in order
  SUCCESS: If cell list is in order.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_check_inter_rat_cell_order
(
  rrc_InterRATCellInfoList_B* rrc_cell_info_list_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_UPDATE_INTER_RAT_CELL_LIST

DESCRIPTION

  This function updates the inter rat cell list received from SIB11

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If cell list is not in order
  SUCCESS: If cell list is in order.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_update_inter_rat_cell_list
(
 uint16* position_no_ptr,
 uint16 arfcn
);

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_REPORT_CRITERIA

DESCRIPTION

  This function processes the report criteria in the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_report_criteria
(
  rrc_InterRATReportCriteria* reportCriteria_ptr,
  l1_inter_sys_meas_ctrl_struct_type*   inter_s_ptr,
  rpt_crit_enum_type* mcm_rpt_crit_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
,uint8 meas_ctrl_tr_id
,   rrc_DL_DCCH_Message* dcch_msg_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_UPDATE_INTER_RAT_CELL_LIST

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If Cell list is invalid
  SUCCESS: If Cell list is valid.

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_status_e_type rrcirat_process_cell_list_mcm
(
  rrc_InterRATCellInfoList* mcm_cell_list_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_EVENT_CRITERIA

DESCRIPTION

  This function processes the event criteria received in a Measurement Control Message
    and stores in the format that is sent to L1

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If event criteria is invalid
  SUCCESS: If event criteria is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_event_criteria
(
  rrc_InterRATReportingCriteria* rrc_event_criteria_ptr,
  l1_inter_sys_report_crit_struct_type* l1_event_criteria_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_CHECK_INTER_RAT_CELL_ORDER_MCM

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If cell list is in order
  SUCCESS: If cell list is not in order

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_check_inter_rat_cell_order_mcm
(
  rrc_NewInterRATCellList   *local_inter_rat_cell_list_ptr
);

/*===========================================================================

FUNCTION   RRCIRAT_FILL_REMOVED_INTER_RAT_CELLS

DESCRIPTION

  This function processes the removed cell list received in a Measurement Control Message


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If removed cell list is valid
  SUCCESS: If removed cell list is invalid

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_fill_removed_inter_rat_cells
(
  rrc_RemovedInterRATCellList* rrc_remove_cells_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_FIND_CELL_ID

DESCRIPTION

  This function processes the inter rat cell id


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If cell id is valid
  SUCCESS: If cell id is invalid

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_find_cell_id
(
 uint16* intra_freq_cell_id_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_REMOVE_CELL_ID

DESCRIPTION

  This function removes the inter rat cell id


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_remove_cell_id
(
 uint16* inter_rat_cell_id_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_UPDATE_INTER_RAT_CELL_LIST_MCM

DESCRIPTION

  This function updates the inter rat cell list received in a Meas Ctrl Msg


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If cell list is valid
  SUCCESS: If cell list is invalid

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_update_inter_rat_cell_list_mcm
(
 uint32* position_no_ptr,
 uint16* cell_id_ptr,  /* L1 value */
 uint16* cell_id_to_remove_ptr  /* Determines if a cell has to be removed */
);


/*===========================================================================

FUNCTION   RRCIRAT_INITIALIZE_CELL_LIST

DESCRIPTION

  This function initializes the inter rat cell list


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_initialize_cell_list(void);


/*===========================================================================

FUNCTION   rrcirat_initialize_local_neighbor_list

DESCRIPTION

  This function initializes the local neighbor list


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_initialize_local_neighbor_list(void);


/*===========================================================================

FUNCTION   RRCIRAT_SEND_MEAS_REPORT

DESCRIPTION

  This function sends the Measurement Report to UTRAN in CELL_DCH only

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_SET_INTER_SYS_EVENT_ID

DESCRIPTION

  This function sets event id in L1 structure

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_set_inter_sys_event_id
(
  l1_inter_sys_event_enum_type* l1_event_type_ptr,
  rrc_EventIDInterRAT* rrc_event_id_ptr
);

/*===========================================================================

FUNCTION   RRCIRAT_VALIDATE_EVENT_RESULTS

DESCRIPTION

  This function validates the event results sent by L1

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If event resuulst are valid
  SUCCESS: If event results are invalid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_validate_event_results
(
  l1_inter_sys_event_results* l1_event_results_ptr
);

/*===========================================================================

FUNCTION   RRCIRAT_CHECK_CELL_ABSENCE_IN_CELL_LIST

DESCRIPTION

  This function checks if the cells are absent in CELL_INFO_LIST

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If CELL_INFO_LIST is empty
  SUCCESS: If CELL_INFO_LIST is not empty

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_check_cell_absence_in_cell_list
(
  void
);


/*===========================================================================

FUNCTION   RRCIRAT_VALIDATE_MEASURED_RESULTS

DESCRIPTION

  This function validates the measured results sent by L1

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If measured results are valid
  SUCCESS: If measured results are invalid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_validate_measured_results
(
  l1_inter_sys_meas_result_struct_type* meas_results_ptr
);

/*===========================================================================

FUNCTION   RRCIRAT_INIT_SIB11_PARAMS

DESCRIPTION

  This function initializes SIB11 params for Inter-RAT Meas

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If measured results are valid
  SUCCESS: If measured results are invalid.

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_init_sib11_params
(
  l1_meas_sib_parm_struct_type* l1_sib11_inter_rat_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_CONVERT_RSSI_VALUE

DESCRIPTION

  This function converst the RSSI value sent by L1 into a format expected by UTRAN.
    Maximum converted value is 63 and minimum converted value is 0

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcirat_convert_rssi_value
(
  int16* gsm_carrier_rssi_ptr,
  uint8* rssi_converted_value_ptr
);


/*===========================================================================

FUNCTION   RRCIRAT_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

  Handles initialization of MEASUREMNT_IDENTIY variable
  for Iner RAT Measurements.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcirat_initialize_meas_id_list
(
 void
);


/*===========================================================================

FUNCTION  RRCIRAT_FILL_HCS_INFO

DESCRIPTION

 This function fills the overall HCS info for an Inter RAT Cell.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill overall HCS info correctly
 SUCCESS: If overall HCS info is filled correctly

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcirat_fill_hcs_info
(
  rrc_InterRATMeasurementSysInfo* sib_inter_rat_ptr,
  l1_meas_sib_parm_struct_type* l1_sib_ptr
  ,rrc_SysInfoType11_v590ext_IEs_newInterRATCellInfoList_v590ext * rel5_ext_ptr
);



/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_COMPRESSED_MODE_STATUS

DESCRIPTION

  This function processes Compressed Mode Status information received
 in a Meas Ctrl Message


DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If compressed mode status info is valid
  SUCCESS: If compressed mode status info is invalid

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_compressed_mode_status
(
  rrc_DPCH_CompressedModeStatusInfo* rrc_cm_ptr,
  l1_cm_status_info_struct_type* l1_cm_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_DELETE_MEAS_ID

DESCRIPTION

  This function processes the deletion of Measurement Identity from
    MEASUREMENT_IDENTITY list. The Measurement Identity belongs either to
    Inter-RAT Measurements or to Inter-Freq Measurements


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_delete_meas_id
(
  uint16* meas_id_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

  Handles initialization of MEASUREMNT_IDENTIY variable
  for Inter Freq Measurements.

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcifreq_initialize_meas_id_list
(
 void
);


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_EVENT_CRITERIA

DESCRIPTION

  This function processes the event criteria received in a Measurement Control Message
    and stores in the format that is sent to L1

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If event criteria is invalid
  SUCCESS: If event criteria is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_event_criteria
(
  rrc_InterFreqReportingCriteria* rrc_event_criteria_ptr,
  l1_inter_freq_report_crit_struct_type* l1_event_criteria_ptr
 ,rrc_MeasurementControl_v590ext_IEs* rel5_ext_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM

DESCRIPTION

  This function processes the Measurement Control Message for
    Inter-Freq Measurements

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
,rrc_DL_DCCH_Message* dcch_msg_ptr
,uint8 meas_ctrl_tr_id
);


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA

DESCRIPTION

  This function process the reporting criteria specified
    in Measurement Control Message

DEPENDENCIES

  None

RETURN VALUE

  Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria
(
 rrc_InterFreqReportCriteria* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
 ,rrc_MeasurementControl_v590ext_IEs* rel5_ext_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_SEND_MEAS_REPORT

DESCRIPTION

    This function sends the measurement report. First it validates the event
    and measured results depending upon the report type and it ignores the L1
    report if reported values are incorrect. The function also appends
    additional measurements if any and also updates the Inter freq meas
    id list if last periodic report is being sent

DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcifreq_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_ADDITIONAL_MEAS

DESCRIPTION

  This function contains the logic of checking if additional measurements
  need to be appended and also appends additional measurements to measured results.

DEPENDENCIES

  The calling function should provide rpt_ptr as NULL if additional results
  do not exist at all.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_additional_measured_results
(
 uint16* primary_meas_id_ptr,
 rrcmeas_layer_e_type primary_meas_type,
 rrc_MeasurementReport* rpt_ptr
);



/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_ADDITIONAL_MEAS

DESCRIPTION

  This function contains the logic of checking if additional measurements
  need to be appended and also appends additional measurements to measured results.

DEPENDENCIES

  The calling function should provide rpt_ptr as NULL if additional results
  do not exist at all.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_process_additional_measured_results_sec_freq
(
 uint16* primary_meas_id_ptr,
 rrcmeas_layer_e_type primary_meas_type,
 rrc_MeasurementReport* rpt_ptr
);


/*===========================================================================

FUNCTION   RRCTVM_APPEND_TRAFFIC_VOL_MEAS

DESCRIPTION

  This function appends Traffic Volume Measurements as additional measurements
  to any other Measurement Report

DEPENDENCIES

  The called function appends the element at the end of
  the list by allocating memory

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

boolean rrctvm_append_traffic_vol_meas
(
 uint16* additional_meas_id_ptr,
 rrc_MeasurementReport* rpt_ptr
);

/*===========================================================================

FUNCTION   RRCMEAS_FIND_ADDITIONAL_MEAS_TYPE

DESCRIPTION

  This function checks if additional measurements exists in RRC database. If it
  exists, it returns its type else returns unknown Measurement which essentially
  implies that the additional measurement does not exist


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_layer_e_type rrcmeas_find_additional_meas_type
(
 uint16* additional_meas_id_ptr,
 rpt_crit_enum_type* rpt_crit_ptr,
 meas_validity_enum_type* addtl_meas_validity_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_FIND_ADDITIONAL_MEAS_TYPE

DESCRIPTION

    This function checks if additional measurements exists in RRC database. If it
    exists, it returns its type else returns unknown Measurement which essentially
    implies that the additional measurement does not exist. The function also returns
    the reporting criteria and meas validity as per the measurement type. The reporting criteria is used by
    the calling function to determine if UTRAN is trying to append an event triggered meas


DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/

rrcmeas_layer_e_type rrcmeas_find_additional_meas_type_sec_freq
(
 uint16* additional_meas_id_ptr,
 rpt_crit_enum_type* rpt_crit_ptr,
 meas_validity_enum_type* addtl_meas_validity_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_APPEND_INTER_RAT_RESULTS

DESCRIPTION

  This function appends Inter-RAT Measured Results to any report.
  It first checks if additional measured results have already been appended
  the measurement report. If yes, then it increments the pointer to point
  to the next available space in additional measured result list


DEPENDENCIES

  The calling function must have initialized the additional measured
  results pointer

RETURN VALUE

  True
  False

SIDE EFFECTS

  None

===========================================================================*/
boolean rrcmeas_append_inter_rat_results
(
 uint8* meas_id_ptr,
 l1_inter_sys_meas_result_struct_type * l1_meas_results
);

void rrcmeas_append_inter_rat_results_ptr
(
 rrc_MeasuredResults* measured_results_ptr,
 l1_inter_sys_meas_result_struct_type * l1_meas_results
);

/*===========================================================================

FUNCTION   RRCMEAS_APPEND_INTRA_FREQ_RESULTS

DESCRIPTION

  This function appends Intra-Freq Measured Results to any report.
  It first checks if additional measured results have already been appended
  the measurement report. If yes, then it increments the pointer to point
  to the next available space in additional measured result list


DEPENDENCIES

  The calling function must have initialized the additional measured
  results pointer

RETURN VALUE

  True
  False

SIDE EFFECTS

  None

===========================================================================*/
boolean rrcmeas_append_intra_freq_results
(
#ifdef FEATURE_WCDMA_DC_HSUPA
 uint8 freq_type,
#endif 
 uint8* meas_id_ptr,
 l1_meas_rpt_struct_type * l1_rpt
);


/*===========================================================================

FUNCTION   RRCTVM_APPEND_TRAFFIC_VOL_MEAS

DESCRIPTION

  This function appends Traffic Volume Measurements as additional measurements
  to any other Measurement Report

DEPENDENCIES

  The called function appends the element at the end of
  the list by allocating memory. The calling function must set
  the first element of the list to NULL if the list is empty

RETURN VALUE

  True
  False

SIDE EFFECTS

  None

===========================================================================*/

boolean rrctvm_append_traffic_vol_results
(
 uint8* meas_id_ptr,
 mac_ul_traffic_meas_ind_type * additional_tvm
);


void  rrctvm_append_traffic_vol_results_ptr
(
   rrc_MeasuredResults * measured_results_ptr,
   mac_ul_traffic_meas_ind_type * additional_tvm
);

/*===========================================================================

FUNCTION   RRCMEAS_CONFIG_INTRA_FREQ_DB

DESCRIPTION

  Sets Intra Freq Reporting Criteria in RRC Measurement Database
  from Measurement Control Message after the validation of
  the message has been completed

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

boolean rrcmeas_config_intra_freq_db
(
 l1_meas_ctrl_parm_struct_type* l1_mcm_ptr,
 meas_validity_enum_type m_validity
);


/*===========================================================================

FUNCTION   RRCMEAS_DELETE_MEAS

DESCRIPTION

  Removes Intra Freq Meas Information in RRC Measurement Database.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrcmeas_delete_meas(uint16* meas_id_ptr);

/*===========================================================================

FUNCTION   RRCMEAS_SET_ADDITIONAL_MEAS_IN_DB

DESCRIPTION

  This function sets Additional Measurements in RRC database for the measurement
  type. This function is only called if it has been determined previously that
  the primary measurement exists and the new additional measurement does not conflict
  with the primary measurement validity and reporting criteria


DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_set_additional_meas_in_db
(
 rrcmeas_layer_e_type* meas_type_ptr,
 rrc_AdditionalMeasurementID_List * additional_list_ptr,
 uint32 meas_id
);


/*===========================================================================

FUNCTION   RRCMEAS_VALIDATE_ADDITIONAL_MEAS

DESCRIPTION

  This function checks if additional measurements exists in RRC database and have the
  same validity as the primary measurement. If it exists, it returns its type else
  returns unknown Measurement which essentially implies that the additional
  measurement does not exist. The function also returns the reporting criteria as per
  the measurement type. The reporting criteria is used by the calling function to
  determine if UTRAN is trying to append an event triggered meas. The function also
  sends Measurement Control Failure Message in case of error condition


DEPENDENCIES

  None.

RETURN VALUE

  False if invalid. Meas Ctrl Failure already sent.

SIDE EFFECTS

  None

===========================================================================*/

boolean rrcmeas_validate_additional_meas
(
  rrc_AdditionalMeasurementID_List * additional_list_ptr,
  uint32 tr_id,
  rrcmeas_layer_e_type* primary_meas_type_ptr,
  uint32 mid
);


/*===========================================================================

FUNCTION   RRCMEAS_CHECK_ADDITIONAL_MEAS

DESCRIPTION

    This function checks if additional measurements exists in RRC database and have the
    same validity as the primary measurement. If it
    exists, it returns its type else returns unknown Measurement which essentially
    implies that the additional measurement does not exist. The function also returns
    the reporting criteria as per the measurement type. The reporting criteria is used by
    the calling function to determine if UTRAN is trying to append an event triggered meas
    The function also sends Measurement Control Failure Message in case of error cond.
    This function is only used if primary measurement is other than Traffic Volume
    Measurements


DEPENDENCIES

    None.

RETURN VALUE

    True if additional measurement has been validated successfully
    False Otherwise

SIDE EFFECTS

    None

===========================================================================*/

boolean  rrcmeas_check_additional_meas
(
  rrc_AdditionalMeasurementID_List *additional_list_ptr,
  uint32 mid,
  uint32 tr_id
);


/*===========================================================================

FUNCTION   RRCTVM_PROCESS_MCM_IDENTITY

DESCRIPTION

  Set traffic volume meas identity variable from SIB information.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrctvm_process_mcm_identity
(
uint16* meas_id_ptr
);


/*===========================================================================

FUNCTION   RRCTVM_VALIDATE_ADDITIONAL_MEAS

DESCRIPTION

  This function checks if additional measurements exists in RRC database and have the
  same validity as the primary measurement. If it
  exists, it returns its type else returns unknown Measurement which essentially
  implies that the additional measurement does not exist. The function also returns
  the reporting criteria as per the measurement type. The reporting criteria is used by
  the calling function to determine if UTRAN is trying to append an event triggered meas


DEPENDENCIES

  None.

RETURN VALUE

  False if invalid. Meas Ctrl Failure already sent.

SIDE EFFECTS

  None

===========================================================================*/

boolean rrctvm_validate_additional_meas
(
 meas_validity_enum_type* p_meas_validity,
 rrc_AdditionalMeasurementID_List * additional_list_ptr,
 uint32 mcm_tr_id,
 uint32 mid
);


/*===========================================================================

FUNCTION   RRCTVM_CONFIG_TVM_DB

DESCRIPTION

  Sets Traffic Volume Meas Information in RRC Measurement Database.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

boolean rrctvm_config_tvm_db
(
 mac_ul_traffic_meas_config_type* mac_mcm_ptr,
 meas_validity_enum_type* p_meas_validity
);


/*===========================================================================

FUNCTION   RRCTVM_SET_REPORTING_CRITERIA

DESCRIPTION

    This function sets reporting criteria in tvm_meas_id_list from SIB's


DEPENDENCIES

    None

RETURN VALUE

    None

SIDE EFFECTS

    None

===========================================================================*/

void rrctvm_set_reporting_criteria
(
  uint16* meas_id_ptr,
  rrc_TrafficVolumeReportCriteriaSysInfo* sib_rpt_crit_ptr
);

/*===========================================================================

FUNCTION   RRCMEAS_DELETE_ADDTL_MEAS

DESCRIPTION

    This function deletes any additional measurement that may be present
  in any of the MEASUREMENT_IDENTITY list at RRC level. The primary
  measurement identity has already been deleted


DEPENDENCIES

    None

RETURN VALUE

    None

SIDE EFFECTS

    None

===========================================================================*/

void rrcmeas_delete_addtl_meas
(
  uint16* prim_meas_id_ptr
);


/*===========================================================================

FUNCTION  RRCMEAS_FILL_HCS_INFO

DESCRIPTION

 This function fills the overall HCS info for an Intra Freq Cell.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill overall HCS info correctly
 SUCCESS: If overall HCS info is filled correctly

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_hcs_info
(
  rrc_MeasurementControlSysInfo* sib_meas_ctrl_ptr
  ,rrc_SysInfoType11_v590ext_IEs *sib11_rel5_ext_ptr
);


/*===========================================================================

FUNCTION  RRCMEAS_FILL_HCS_RSCP_CELL_INFO

DESCRIPTION

 This function fills in Cell Info for a HCS cell when
 Cell Selection Reselection Quality Measure is RSCP.
 The function processes Cell Info in ASN1 format and
 translates into the L1 RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill cell info correctly
 SUCCESS: If Cell Info is filled correctly

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_hcs_rscp_cell_info
(
rrc_CellInfoSI_HCS_RSCP *sib_cell_info_ptr,
l1_cell_info_struct_type* l1_cell_info_ptr
,rrc_CellSelectReselectInfo_v590ext * rel5_ext_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_FILL_HCS_ECN0_CELL_INFO

DESCRIPTION

 This function fills in Cell Info for a HCS cell when
 Cell Selection Reselection Quality Measure is ECN0.
 The function processes Cell Info in ASN1 format and
 translates into the L1 RRC Interface format.

DEPENDENCIES

 Pointer to rrc_CellInfoSI_ECN0 must be valid.

RETURN VALUE

 FAILURE: If passed pointer is NULL.
 SUCCESS: If able to analyze and fill Cell Info.

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_hcs_ecn0_cell_info
(
  rrc_CellInfoSI_HCS_ECN0 *sib_cell_info_ptr,
  l1_cell_info_struct_type *l1_cell_info_ptr
,rrc_CellSelectReselectInfo_v590ext * rel5_ext_ptr
);


/*===========================================================================

FUNCTION  RRCIFREQ_VALIDATE_FREQ

DESCRIPTION

 This function validates the uplink and downlink carrier frequency.


DEPENDENCIES

 None

RETURN VALUE

 True if validation successful
 False if validation fails

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcifreq_validate_freq
(
 l1_freq_info_struct_type* freq_info_ptr
);


/*===========================================================================

FUNCTION  RRCIFREQ_VALIDATE_MEAS_REPORT

DESCRIPTION

 This function validates the measurement report.

DEPENDENCIES

 None

RETURN VALUE

 True if validation successful
 False if validation fails

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcifreq_validate_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
);


/*===========================================================================

FUNCTION  RRCIFREQ_SET_INTER_FREQ_EVENT_ID

DESCRIPTION

 This function sets the event id as per ASN1 format from what L1 specified
 in the measurement report

DEPENDENCIES

 None

RETURN VALUE

 True if validation successful
 False if validation fails

SIDE EFFECTS

 None

===========================================================================*/

void rrcifreq_set_inter_freq_event_id
(
 l1_inter_freq_event_enum_type* event_id_ptr,
 rrc_EventIDInterFreq* rrc_inter_freq_event_id_ptr
);

/*===========================================================================

FUNCTION   IFREQ_READ_SIB11

DESCRIPTION

  This function reads Inter Freq Cell List from SIB 11 and
  stores it in inter_freq_cell_id_list. This Inter Freq
  Cell List is used for cell reselection. When UTRAN sends a meas
  in connected DCH mode, this cell list is overwritten.

DEPENDENCIES

  -

RETURN VALUE
    Success if parameters successfully read
  Failure otherwise


SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcifreq_read_sib11
(
 l1_inter_freq_cell_list_struct_type* rrc_inter_freq_sib_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_UPDATE_INTER_FREQ_CELL_LIST

DESCRIPTION

  Updates the inter freq cell list with new values of inter freq
  cell id. This is done on receving SIB 11 system info. The calling fn gives
  position_no_ptr indicating where to store the psc. If it
  points to AVAILABLE_CELL_POSITION, then store psc in the first
  available position in inter_freq_cell_list and and mark that
  position as occupied. If position_no_ptr points to some other
  value then store psc at the position given by that value

DEPENDENCIES

  None

RETURN VALUE

  FAILURE if inter freq cell list cannot be updated due to incorrect
  value of the inter freq cell id
  SUCCESS if inter freq cell list can be successfully updated

SIDE EFFECTS

  None
===========================================================================*/

uecomdef_status_e_type rrcifreq_update_inter_freq_cell_list
(
 uint32* position_no_ptr,
 uint16* psc_ptr,
 uint16* dl_freq_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_INITIALIZE_CELL_LIST

DESCRIPTION

  This function initializes the inter freq cell list

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/

void rrcifreq_initialize_cell_list
(
 void
);


/*===========================================================================

FUNCTION   RRCIFREQ_FILL_INTER_FREQ_INFO

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List

DEPENDENCIES

 Pointer to rrc_CellInfoSI_ECN0 must be valid.

RETURN VALUE

 FAILURE: If passed pointer is NULL.
 SUCCESS: If able to analyze and fill Cell Info.

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcifreq_fill_inter_freq_info
(
  rrc_InterFrequencyMeasurement* inter_f_meas_ptr,
  l1_meas_ctrl_parm_struct_type* l1_meas_pr
);


/*===========================================================================

FUNCTION   RRCIFREQ_UPDATE_INTER_FREQ_CELL_LIST_MCM

DESCRIPTION

 Invoked from the cell list processing fn specified by MCM
 The calling fn specifies the position, scrambling code
 and the downlink freq. The fn checks if there is
 already a valid scr code and dl freq at that position.
 If yes, then it stores them in psc_to_remove_ptr and
 dl_freq_to_remove_ptr and updates the inter freq cell list
 with the new scr code and dl freq and does not change
 the position. If there was no cell at that position, then
 it stores the dl freq and scr code and markls that position
 in the inter freq cell list as occupied.

DEPENDENCIES

 None.

RETURN VALUE

 FAILURE: If unable to update the inter freq cell list.
 SUCCESS: If able to update the inter freq cell list.

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcifreq_update_inter_freq_cell_list_mcm
(
  uint32* position_no_ptr,
  uint16* psc_ptr,
  uint16* dl_freq_ptr,
  uint16* psc_to_remove_ptr,
  uint16* dl_freq_to_remove_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_FIND_PSC_FREQ

DESCRIPTION

 Finds the primary scrambling code of the inter freq cell id. The primary
 scrambling code is returned back in psc_ptr provided. If not found,
 then psc_ptr contains INVALID_PSC

DEPENDENCIES

 None

RETURN VALUE

 PSC_ABSENT if psc absent
 PSC_PRESENT if psc present

SIDE EFFECTS

 None
===========================================================================*/

psc_e_type rrcifreq_find_psc_freq
(
  uint32 inter_freq_cell_id,
  uint16* psc_ptr,
  uint16* dl_freq_ptr
);

/*===========================================================================

FUNCTION   RRCIFREQ_REMOVE_CELLID

DESCRIPTION

 This function removes the cell id in inter freq cell list and marks
 the posiiton as available. It also invalidates the scr code
 and the dl freq to DEFAULT values.

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcifreq_remove_cellid
(
 uint32 inter_freq_cell_id
);


/*===========================================================================

FUNCTION   RRCIFREQ_FILL_RPT_QUAN

DESCRIPTION

 This function fills the reporting quantity in L1 structure from the ASN1 format

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcifreq_fill_rpt_quan
(
 rrc_InterFreqReportingQuantity* mcm_rpt_quan_ptr,
 l1_inter_freq_rpt_quan_struct_type* l1_rpt_quan_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_SEND_L1_CMD

DESCRIPTION

 This function sends the L1 command from SIB's only

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcifreq_send_l1_cmd
(
 l1_meas_ctrl_enum_type*  meas_choice_ptr,
 l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MEAS_QUANTITY

DESCRIPTION

 This function processes the measurement quantity and converst it into L1 format.

DEPENDENCIES

 None

RETURN VALUE

 Success if meas quantity is valid
 Failure if meas quantity is invalid

SIDE EFFECTS

 None
===========================================================================*/

rrcmeas_status_e_type rrcifreq_process_meas_quantity
(
  rrc_InterFreqMeasQuantity* meas_quan_ptr,
  l1_inter_freq_meas_quan_struct_type* l1_meas_quan_ptr
);


/*===========================================================================

FUNCTION   RRCIFREQ_CONFIG_INTER_FREQ_DB

DESCRIPTION

 This function is called after the the validation of Measurement Control
 Message is successful and then Inter Freq Meas Id list needs to be updated.
 It stores the meas id, reporting criteria and validity

DEPENDENCIES

 None

RETURN VALUE

 Success if inter freq meas id list is successfully updated
 Failure otherwise

SIDE EFFECTS

 None
===========================================================================*/

boolean rrcifreq_config_inter_freq_db
(
 l1_meas_ctrl_parm_struct_type* l1_mcm_ptr,
 rpt_crit_enum_type* rpt_crit_ptr,
 meas_validity_enum_type m_validity
);


uecomdef_status_e_type rrcifreq_fill_hcs_info
(
  l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr
  ,rrc_SysInfoType12_v590ext_IEs_newInterFrequencyCellInfoList_v590ext * rel5_ext_ptr
);


/*===========================================================================

FUNCTION   RRCUEIM_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

 This function initializes the measurement identity list of
 UE Internal Measurements

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcueim_initialize_meas_id_list
(
 void
);


/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM

DESCRIPTION

 This function processes the Measurement Control Message

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
 ,uint8 meas_ctrl_tr_id
);


/*===========================================================================

FUNCTION   RRCUEIM_SEND_MEAS_REPORT

DESCRIPTION

 This function sends the Measurement Report after validation

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcueim_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
);


/*===========================================================================

FUNCTION   RRCUEIM_SET_EVENT_ID

DESCRIPTION

 This function sets the event ID in ASN1 format

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcueim_set_event_id
(
 l1_internal_meas_event_enum_type* l1_event_type_ptr,
 rrc_UE_InternalEventResults* rrc_event_id_ptr
);


/*===========================================================================

FUNCTION   RRCUEIM_VALIDATE_MEASURED_RESULTS

DESCRIPTION

 This function validates the measured results

DEPENDENCIES

 None

RETURN VALUE

 Success if validation successful
 Failure Otherwise

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_validate_measured_results
(
 l1_ue_internal_meas_rpt_struct_type* meas_results_ptr
);


/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_EVENT_CRITERIA

DESCRIPTION

 This function processes the event criteria specified in Measurement
 Control Message

DEPENDENCIES

 None

RETURN VALUE

 Success if event parameters are valid
 Failure Otherwise

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_event_criteria
(
 rrc_UE_InternalReportingCriteria* rrc_event_criteria_ptr,
 l1_internal_meas_rpt_crit_struct_type* l1_event_criteria_ptr
);

/*===========================================================================

FUNCTION   RRCMEAS_APPEND_INTERNAL_MEAS_RESULTS

DESCRIPTION

    This function appends Internal Measured Results to any report.
    It first checks if additional measured results have already been appended
    the measurement report. If yes, then it increments the pointer to point
    to the next available space in additional measured result list


DEPENDENCIES

    The calling function must have initialized the additional measured
    results pointer

RETURN VALUE

    True if UE Internal Measured Results successfully appended
    False Otherwise

SIDE EFFECTS

    None

===========================================================================*/

boolean rrcmeas_append_internal_meas_results
(
 uint8* meas_id_ptr,
 l1_ue_internal_meas_rpt_struct_type * ue_internal_meas
);

void rrcmeas_append_internal_meas_results_ptr
(
 rrc_MeasuredResults * measured_results_ptr,
 l1_ue_internal_meas_rpt_struct_type * ue_internal_meas
);

/*===========================================================================

FUNCTION   RRCMEAS_APPEND_INTER_FREQ_MEAS_RESULTS

DESCRIPTION

    This function appends Inter-Freq Measured Results to any report.
    It first checks if additional measured results have already been appended
    the measurement report. If yes, then it increments the pointer to point
    to the next space in additional measured result list and sets the result
    in that location. Additional Measurement Identity passed to
  this function has already been validated

DEPENDENCIES

    The calling function must have initialized the additional measured
    results pointer

RETURN VALUE

    True if results successfully appended
    False otherwise

SIDE EFFECTS

    None

===========================================================================*/
boolean rrcmeas_append_inter_freq_meas_results
(
 uint8* meas_id_ptr,
 l1_meas_rpt_struct_type * l1_rpt
);

void rrcmeas_append_inter_freq_meas_results_ptr
(
 rrc_MeasuredResults* measured_results_ptr,
 l1_meas_rpt_struct_type * l1_rpt
);

/*===========================================================================

FUNCTION   RRCMEAS_CHECK_MCM_FOR_SETUP_MODIFY_ADDITIONAL

DESCRIPTION

  This function checks if the existing measurement already exists
  in the measurement database. If yes, then it deletes the
  additional measurement associated with it. Also for a modify
  it checks if UTRAN tries to add more than the maximum no of
  allowed measurements, then it rejects the configuration

DEPENDENCIES

  None

RETURN VALUE

  True/False

SIDE EFFECTS

  None
===========================================================================*/

boolean rrcmeas_check_mcm_for_setup_modify_additional
(
 boolean addtl_meas_incl,
 rrcmeas_layer_e_type primary_meas_type,
 uint32 mid,
 rrc_AdditionalMeasurementID_List * local_addtl_meas_list_ptr
);


#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)

#ifdef FEATURE_AAGPS_MR_RRC_NOTIFICATION_REQUIRED
/*===========================================================================

FUNCTION   RRCGPS_INITIALIZE_L2_ACK_PARAMS

DESCRIPTION

    This function initializes the L2 ack param structure.

DEPENDENCIES

    None.

RETURN VALUE

    None

SIDE EFFECTS

    None

===========================================================================*/

void rrcgps_initialize_l2_ack_params
(
  void
);
#endif

/*===========================================================================

FUNCTION    RRCGPS_VERIFY_POSITIONING_CAPABILITY

DESCRIPTION

 Checks positioning capability to determine if UE could process
 A-GPS Measurement Control Message

DEPENDENCIES

 None.

RETURN VALUE

 Success/Failure

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcgps_verify_positioning_capability
(
  const rrc_UE_Positioning_ReportingQuantity *rpt_quant_ptr
);


/*===========================================================================

FUNCTION    RRCGPS_VERIFY_POSITIONING_CAPABILITY_R5

DESCRIPTION

 Checks positioning capability to determine if UE could process
 A-GPS Measurement Control Message for RRC R5.

DEPENDENCIES

 None.

RETURN VALUE

 Success/Failure

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcgps_verify_positioning_capability_r5
(
  const rrc_UE_Positioning_ReportingQuantity_r4 *rpt_quan_ptr
);

/*===========================================================================

FUNCTION    RRCMEAS_PROCESS_POS_MCM

DESCRIPTION

 Processes Measurement Control Message for any Positioning Measurement

DEPENDENCIES

 None.

RETURN VALUE

 Success/Failure

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_pos_mcm
(
  void* msg_ptr,
  pos_info_struct_type* pos_info_ptr
  ,uint8 meas_ctrl_tr_id
);


/*===========================================================================

FUNCTION    RRCGPS_PROCESS_MCM

DESCRIPTION

 Processes GPS Measurement Control Message

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcgps_process_mcm
(
  void* pos_msg_ptr,
  pos_info_struct_type* pos_info_ptr
  ,uint8 meas_ctrl_tr_id
);


/*===========================================================================

FUNCTION    RRCGPS_SEND_LSM_CMD

DESCRIPTION

 Sends command to LSM to start Measurements

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_send_lsm_cmd
(
  rrc_meas_params_struct_type* gps_meas_params
);


/*===========================================================================

FUNCTION    RRCGPS_SEND_MEAS_REPORT

DESCRIPTION

 Sends GPS Measurement Report to UTRAN

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_send_meas_report
(
  rrc_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION    RRCGPS_SET_MEAS_RESULTS

DESCRIPTION

 Sets Measured Results received from LSM in ASN1 format

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_set_meas_results
(
  rrc_gps_measured_results_struct_type* lsm_gps_meas_results_ptr,
  rrc_UE_Positioning_GPS_MeasurementResults* rrc_gps_meas_results_ptr
);


/*===========================================================================

FUNCTION    RRCGPS_SET_POS_ERROR

DESCRIPTION

 Sets Positioning Error received from LSM in ASN1 format

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_set_pos_error
(
  rrc_positioning_error_struct_type* lsm_gps_meas_ptr,
  rrc_UE_Positioning_Error* rrc_gps_pos_error_ptr
);


/*===========================================================================

FUNCTION    RRCGPS_SET_POS_ESTIMATE

DESCRIPTION

 Sets Positioning Estimate received from LSM in ASN1 format

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_set_pos_estimate
(
  rrc_positioning_estimate_struct_type* lsm_pos_est_ptr,
  rrc_UE_Positioning_PositionEstimateInfo *rrc_gps_pos_est_ptr
);



/*===========================================================================

FUNCTION    RRCGPS_INITIALIZE_POS_MEAS_ID_LIST

DESCRIPTION

 Initializes Positioning Measurement Identity List

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcmeas_initialize_pos_meas_id_list
(
  void
);

/*===========================================================================

FUNCTION    RRCGPS_PROCESS_ASD_MSG

DESCRIPTION

 Processes Assistance Data Delievery Message received from UTRAN

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_process_asd_msg
(
  rrc_cmd_type* cmd_ptr
);


/*===========================================================================

FUNCTION    RRCGPS_DISPATCH_LSM_COMMANDS

DESCRIPTION

 Dispatch LSM Command

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrc_dispatch_lsm_commands
(
  void
);


/*===========================================================================

FUNCTION    RRCGPS_VALIDATE_MEAS_RESULTS

DESCRIPTION

 Validates Measured Results received from LSM

DEPENDENCIES

 None.

RETURN VALUE

 True/False

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcgps_validate_meas_results
(
  rrc_gps_measured_results_struct_type* lsm_gps_meas_results_ptr
);


/*===========================================================================

FUNCTION    RRCGPS_VALIDATE_POS_ESTIMATE

DESCRIPTION

 Validates Positioning Estimate received from LSM

DEPENDENCIES

 None.

RETURN VALUE

 True/False

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcgps_validate_pos_estimate
(
  rrc_positioning_estimate_struct_type* lsm_pos_est_ptr
);


/*===========================================================================

FUNCTION    RRCGPS_VALIDATE_POS_ERROR

DESCRIPTION

 Validates Positioning Error received from LSM

DEPENDENCIES

 None.

RETURN VALUE

 True/False

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcgps_validate_pos_error
(
  rrc_positioning_error_struct_type* lsm_gps_meas_ptr
);


/*===========================================================================

FUNCTION   RRCMEAS_CONFIG_POS_DB

DESCRIPTION

 Sets Positioning Meas Information in RRC Measurement Database.

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

boolean rrcmeas_config_pos_db
(
 pos_info_struct_type* pos_info_ptr,
 meas_validity_enum_type* p_meas_validity,
 rpt_crit_enum_type* pos_rpt_crit,
 boolean modify_meas_params_incl /* Only applicable for MODIFY */
);

#endif


/*===========================================================================

FUNCTION    RRCMEAS_SEND_RRC_STATUS_MSG

DESCRIPTION

 Sends RRC Status Message to UTRAN

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcmeas_send_rrc_status_msg
(
 uint8* transaction_id_ptr,
 uint8* error_choice_ptr
);


/*===========================================================================

  FUNCTION     RRCMEAS_GET_FACH_MEAS_INFO

DESCRIPTION

  Gets FACH Meas Occasion Info from SIB11

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  Trashing Measurement Report may result in Soft Hand Off not working properly

===========================================================================*/

rrcmeas_status_e_type rrcmeas_get_fach_meas_info
(
 void
);

/*===========================================================================

FUNCTION rrcmeas_get_rach_rpt_quantity

DESCRIPTION

 Gets RACH reporting measurement quantity

DEPENDENCIES

 This fn is only called when RACH criteria for the new cell is supported

RETURN VALUE

 Success if RACH quantity read is supported
 Failure if RACH quantity read is not supported

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_get_rach_rpt_quantity
(
  l1_meas_quan_enum_type* meas_quantity
);


/*===========================================================================

FUNCTION  RRCIRAT_DELETE_DUP_NON_VERIFIED_CELLS

DESCRIPTION

 This function deletes duplicate non verified cells from Meas Report.

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcirat_delete_dup_non_verified_cells
(
 l1_inter_sys_meas_result_struct_type* meas_results_ptr
);


/*===========================================================================

FUNCTION  RRCTVM_PROCESS_SIB_CHANGE_NOTIFICATION

DESCRIPTION

 This function processes SIB change notification to modify TVM

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrctvm_process_sib_change_notification
(
  rrc_sib_change_mask_type sib_change_mask
);



/*===========================================================================

FUNCTION rrcmeas_check_if_scr_present_in_aset

DESCRIPTION

 Checks if SCR Code is present in ASET when UE transitions from Idle-->DCH
 or from FACH-->DCH

DEPENDENCIES



RETURN VALUE

 Success if SCR present in ASET
 Failure if SCR not present in ASET

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcmeas_check_if_scr_present_in_aset
(
  void
);


/*===========================================================================

FUNCTION rrcmeas_update_meas_after_state_trans

DESCRIPTION

 Updates in all meas id variable that the specific meas cannot be reported

DEPENDENCIES


RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcmeas_update_meas_after_state_trans
(
  void
);


/*===========================================================================

FUNCTION rrcmeas_check_if_rpt_can_be_txted

DESCRIPTION

 Updates in all meas id variable that the specific meas cannot be reported

DEPENDENCIES


RETURN VALUE

 True/False

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcmeas_check_if_rpt_can_be_txted
(
  uint16 m_id
);


/*===========================================================================

FUNCTION rrcmeas_check_if_meas_reconfig_reqd

DESCRIPTION

 Get the meas id from SIB11 of the new cell. Then compare
 if this meas id matches with any of the meas id which was
 owned by a MCM. If TRUE, then return FALSE indicating that
 complete measurement reconfig not required. Else return TRUE
 indicating that complete meas reconfig required. The fn also checks
 if at meas db is empty. In that case it returns true in meas_db_empty_ptr

DEPENDENCIES


RETURN VALUE

 True/False

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcmeas_check_if_meas_reconfig_reqd
(
  uint16* m_id_ptr,
  boolean* meas_db_empty_ptr
);

#if defined(FEATURE_CGPS)
/*===========================================================================

FUNCTION rrcmeas_set_cell_plmn_info

DESCRIPTION

 Sets the PLMN id, Cell id ,Psc, Freq of the current serving cell in the
 global structure plmn_cell_info

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcmeas_set_cell_plmn_info
(
  void
);
#endif /*FEATURE_CGPS */

/*===========================================================================

FUNCTION RRCMEAS_PATH_LOSS_CHECK

DESCRIPTION
 This function checks if the intra fq reporting quantity is path loss for SETUP & if
 so verifies if the cpich tx power is present in the cell info in valid range.
 If not it returns an error

DEPENDENCIES

 None

RETURN VALUE

FAILURE: Path loss error
SUCCESS: Path loss check validated

SIDE EFFECTS

 None
============================================================================*/
rrcmeas_status_e_type rrcmeas_path_loss_check
(
rrc_IntraFrequencyMeasurement* intra_f_meas_ptr
);

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/*===========================================================================

FUNCTION   RRCGPS_PROCESS_DCH_TO_FACH_PCH

DESCRIPTION

  DCH->FACH and DCH->PCH transition


DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/

void rrcgps_process_dch_to_fach_pch
(
 void
);


/*===========================================================================

FUNCTION   RRCGPS_PROCESS_FACH_TO_DCH

DESCRIPTION

  DCH->FACH transition


DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS



===========================================================================*/

void rrcgps_process_fach_to_dch
(
 void
);

/*===========================================================================

FUNCTION   RRCGPS_PROCESS_FACH_TO_PCH

DESCRIPTION

  FACH->PCH transition


DEPENDENCIES

  None

RETURN VALUE

  None.

SIDE EFFECTS



===========================================================================*/

void rrcgps_process_fach_to_pch
(
 void
);
#endif
/*===========================================================================

FUNCTION rrcirat_read_sib12

DESCRIPTION

 Caller must check the sib12_present before calling this fn.
 This function processes SIB12 IEs and build the information
 to be sent down to L1 and updates the cell info list for
 inter rat measurements.

DEPENDENCIES

 SIB12 must have been received by the camping cell.

RETURN VALUE

 SUCCESS/FAIILURE

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcirat_read_sib12
(
rrc_SysInfoType12 *sib12_ptr,
l1_meas_sib_parm_struct_type* l1_inter_rat_sib_ptr
);

/*===========================================================================

FUNCTION rrcirat_process_remove_cell_list

DESCRIPTION

 This function processes the remove cell list of SIB12 and updates the inter
 rat cell list

DEPENDENCIES

 SIB12 must have been received by the camping cell.

RETURN VALUE
 None

SIDE EFFECTS

 None

===========================================================================*/
void rrcirat_process_remove_cell_list
(
rrc_RemovedInterRATCellList *removedInterRATCellList,
uint16 *num_removed_cells
);

/*===========================================================================

FUNCTION rrcirat_build_composite_cell_list_from_SIB11_SIB12

DESCRIPTION

 This function builds a composite cell info list from SIB11 and SIB12 that is
 then sent down to L1

DEPENDENCIES

 SIB12 must have been received by the camping cell.

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcirat_build_composite_cell_list_from_SIB11_SIB12
(
rrc_RemovedInterRATCellList *removedInterRATCellList,
l1_inter_sys_cell_list_struct_type* sib11_cell_list_ptr,
l1_inter_sys_cell_list_struct_type* sib12_cell_list_ptr,
uint16 num_removed_cells
);

/*===========================================================================

FUNCTION   copy_sib11_inter_freq_cell_list_to_sib12

DESCRIPTION

    This function copies the inter freq cell list from SIB 11 nto the SIB12
    inter freq cell list ptr

DEPENDENCIES

    None

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/

void rrcifreq_copy_sib11_inter_freq_cell_list_to_sib12
(
l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib11_ptr,
l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib12_ptr
);

/*===========================================================================

FUNCTION   rrcifreq_build_composite_cell_list

DESCRIPTION

    This function builds the composite inter freq cell info list from SIB 11
    and SIB12.

DEPENDENCIES

    None

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/
void rrcifreq_build_composite_cell_list
(
rrc_RemovedInterFreqCellList *removedInterFreqCellList,
l1_inter_freq_cell_list_struct_type* sib11_cell_list_ptr,
l1_inter_freq_cell_list_struct_type* sib12_cell_list_ptr,
uint32 *num_rmv_cells
);

/*===========================================================================

FUNCTION    RRCMEAS_CLEAR_SIB12

DESCRIPTION

 Initialises SIB12 stored variable, sib12_rach_report_crit
DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcmeas_clear_sib12
(
l1_meas_sib_parm_struct_type *rrcmeas_sib12_ptr
);

/*===========================================================================

FUNCTION   IFREQ_READ_SIB12

DESCRIPTION

    This function reads Inter Freq Cell List from SIB 12 and
    stores it in inter_freq_meas_id_list. This Inter Freq
    Cell List is used for cell reselection. When UTRAN sends a meas
    in connected DCH mode, this cell list is overwritten.

DEPENDENCIES

    This function assumes that SIB12 is present and that the sib12_ptr
    != NULL

RETURN VALUE
   None


SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_read_sib12
(
 rrc_SysInfoType12 *sib12_ptr,
 l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib11_ptr,
 l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib12_ptr
);

/*===========================================================================

FUNCTION    RRCMEAS_INITIALIZE_TRANSACTION_ID_LIST

DESCRIPTION

 Initialises transaction id list

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcmeas_initialize_transaction_id_list
(
 void
);



/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO

DESCRIPTION

    Process traffic volume information in a Meas
    Ctrl Message. Also sends a Meas Ctrl Failure
    Message in case of an error


DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r5
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
);

/*===========================================================================

FUNCTION   RRCMEAS_RETRIEVE_MEAS_OWNER_R5

DESCRIPTION

 Finds the owner of measurement identity given in a MCM Rel 5 Msg

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

rrcmeas_layer_e_type rrcmeas_retrieve_meas_owner_r5
(
rrc_MeasurementCommand_r4* mcm_ptr,
uint32 meas_id
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
,pos_info_struct_type* pos_info_ptr
#endif
);

/*===========================================================================

FUNCTION   rrcmeas_process_mcm_r5

DESCRIPTION

    This function processed the Release 5 Meas Ctrl Message

DEPENDENCIES

    None

RETURN VALUE
   Void


SIDE EFFECTS

    None

===========================================================================*/

void rrcmeas_process_mcm_r5
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION  RRCMEAS_PROCESS_MCM_INTRA_F_CELL_INFO_R5

DESCRIPTION

 This function processes the Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 The function processes Cell Info in ASN1 format and translates
 into the L1RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill mcm cell info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcmeas_process_mcm_intra_f_cell_info_r5
(
rrc_IntraFreqCellInfoList_r4* intra_freq_cell_info_list_ptr,
l1_intra_freq_meas_ctrl_struct_type* l1_intra_f_mcm_ptr
);

/*===========================================================================

FUNCTION  RRCMEAS_FILL_COMMON_MCM_INTRA_F_INFO_R5

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 The function processes Cell Info in ASN1 format and translates
 into the L1RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcmeas_fill_common_mcm_intra_f_info_r5
(
rrc_IntraFrequencyMeasurement_r4* intra_f_meas_ptr,
l1_meas_ctrl_parm_struct_type *l1_mcm,
rrc_MeasurementControl_v590ext_IEs* mcm_ext_ptr
);

/*===========================================================================

FUNCTION  RRCMEAS_FILL_MCM_CELL_INFO_R5

DESCRIPTION

 This function fills in Cell Info received from a R5
 Measurement Control Message. The function processes
 Cell Info in ASN1 format and translates into the L1
 RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill cell info correctly.
 SUCCESS: If cell info is filled correctly

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_mcm_cell_info_r5
(
rrc_CellInfo_r4* mcm_cell_info_ptr,
l1_cell_info_struct_type* l1_cell_info_ptr
);

/*===========================================================================

FUNCTION     RRCMEAS_FILL_INTRA_FREQ_EVENT_CRITERIA_R5

DESCRIPTION

 This function fills in intra-freq event criteria for a Non-HCS cell when
 Cell Selection Reselection Quality Measure is RCSP or ECNO for R5 Message
 The function processes intra-ftreq event criteria Info in ASN1 format
 and translates into the L1 RRC Interface format.

DEPENDENCIES

 The passed pointers must be valid.

RETURN VALUE

 No of events.

SIDE EFFECTS

 None

===========================================================================*/

uint16 rrcmeas_fill_intra_freq_event_criteria_r5
(
rrc_IntraFreqReportingCriteria_r4  *rrc_intra_freq_reporting_criteria_ptr,
l1_intra_freq_event_crit_struct_type *l1_intra_freq_event_crit_ptr,
rrc_MeasurementControl_v590ext_IEs* mcm_ext_ptr
);

/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_INTRAFREQ_R5

DESCRIPTION

 Processed Intra Freq Meas in Rel5 Meas Ctrl Message

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcmeas_process_intrafreq_r5
(
  rrc_cmd_type* cmd_ptr,
  uint8 tid
);

/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM_R5

DESCRIPTION

 This function processes the Measurement Control Message rel 5

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm_r5
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
);

/*===========================================================================

FUNCTION   RRCUEIM_FILL_MCM_INFO_R5

DESCRIPTION

 This function fills the Measurement Control information into the L1 structure

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_fill_mcm_info_r5
(
  rrc_UE_InternalMeasurement_r4 *ue_InternalMeasurement_ptr,
  l1_ue_internal_meas_ctrl_struct_type *ue_internal_ptr,
  uint32 transaction_id,
  rpt_crit_enum_type *mcm_rpt_crit_ptr
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM_R5

DESCRIPTION

    This function processes the release 5 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm_r5
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION  RRCIFREQ_FILL_INTER_FREQ_INFO_R5

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List for rel 5

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcifreq_fill_inter_freq_info_r5
(
 rrc_InterFreqCellInfoList_r4* interFreqCellInfoList_ptr,
 l1_meas_ctrl_parm_struct_type* l1_meas_ptr
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R5

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r5
(
 rrc_InterFreqReportCriteria_r4* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr,
  rrc_MeasurementControl_v590ext_IEs* mcm_590_ext_ptr
);

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R5

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_r5
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_CELL_LIST_MCM_R5

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If Cell list is invalid
  SUCCESS: If Cell list is valid.

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_status_e_type rrcirat_process_cell_list_mcm_r5
(
  rrc_InterRATCellInfoList_r4* mcm_cell_list_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
);


/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM_R5

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements for rel 5

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm_r5
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_POS_MCM_R5

DESCRIPTION

 This function processes the Positioning Measurement Control Message and
 determines if it is a A-GPS or Cell Id or OTDOA Message. Depending upon that
 it configures either LSM or L1 with measurement parameters

DEPENDENCIES

 None

RETURN VALUE

 FAILURE:
 SUCCESS:

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_pos_mcm_r5
(
  void* msg_ptr,
  pos_info_struct_type* pos_info_ptr,
  uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCGPS_FILL_MCM_R5_INFO

DESCRIPTION

    This function fill the GPS structure for
    A-GPS Measurements.

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_fill_mcm_info_r5
(
  pos_info_struct_type* pos_info_ptr,
  rrc_UE_Positioning_Measurement_r4 *up_Measurement_ptr,
  rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
  uint8 transaction_id,
  boolean modify_meas_params_incl
);

/*===========================================================================

FUNCTION   RRCGPS_PROCESS_MCM_R5

DESCRIPTION

    This function processes the rel 5 Measurement Control Message for
    A-GPS Measurements.

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_process_mcm_r5
(
  void* pos_mcm_ptr,
  pos_info_struct_type* p_info_ptr
  ,uint8 meas_ctrl_tr_id
);
#endif





/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm
(
  rrc_MeasurementControl_r3_IEs* rrc_mcm_ptr
  ,uint8 meas_ctrl_tr_id
);

/*==========================================================================
 FUNCTION   RRCQM_FILL_MCM_INFO

DESCRIPTION

  This function fills the L1 structure for Quality Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_fill_mcm_info
(
  rrc_QualityMeasurement *qualityMeasurement_ptr,
  l1_qual_meas_ctrl_struct_type  *qual_ptr,
  uint8 meas_ctrl_tr_id,
  rpt_crit_enum_type *mcm_rpt_crit_ptr
);

/*==========================================================================
 FUNCTION   RRCQM_INITIALIZE_MEAS_ID_LIST

DESCRIPTION

  This function initializes the measurement identity list for
  Quality Measurements

DEPENDENCIES

    None.

RETURN VALUE

    None

SIDE EFFECTS

    None

===========================================================================*/

void rrcqm_initialize_meas_id_list
(
 void
);

/*===========================================================================

FUNCTION   RRCQM_SEND_MEAS_REPORT

DESCRIPTION

 This function sends the Quality Measurement Report after validation

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcqm_send_meas_report
(
 l1_meas_rpt_struct_type* l1_meas_report_ptr
);

/*===========================================================================

FUNCTION rrcmeas_check_meas_id_used_for_different_meas_type

DESCRIPTION

 The function checks if the new meas id is already used for a different meas type
 and if so releases the measurement from the primary measurement database, from
 the additional measurement dtabase and send a release command to L1 to release
 the measurement.

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/
void rrcmeas_check_meas_id_used_for_different_meas_type
(
  rrcmeas_layer_e_type meas_owner,
  uint16 meas_id
);

/*===========================================================================

FUNCTION   rrcirat_add_irat_cell_info_ind

DESCRIPTION

  This function set the irat cell info ind in meas report.


DEPENDENCIES

  None

RETURN VALUE

  none

SIDE EFFECTS

  None

===========================================================================*/
void rrcirat_add_irat_cell_info_ind
(
  rrc_MeasurementReport* rpt_ptr
);

/*===========================================================================

FUNCTION   rrcmeas_process_mcm_r6

DESCRIPTION

    This function processed the Release 5 Meas Ctrl Message

DEPENDENCIES

    None

RETURN VALUE
   Void


SIDE EFFECTS

    None

===========================================================================*/

void rrcmeas_process_mcm_r6
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCMEAS_RETRIEVE_MEAS_OWNER_R6

DESCRIPTION

 Finds the owner of measurement identity given in a MCM Rel 6 Msg

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

rrcmeas_layer_e_type rrcmeas_retrieve_meas_owner_r6
(
rrc_MeasurementCommand_r6* mcm_ptr,
uint32 meas_id
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
,pos_info_struct_type* pos_info_ptr
#endif
);

/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_INTRAFREQ_R6

DESCRIPTION

 Processed Intra Freq Meas in Rel6 Meas Ctrl Message

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcmeas_process_intrafreq_r6
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION  RRCMEAS_FILL_COMMON_MCM_INTRA_F_INFO_R6

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 The function processes Cell Info in ASN1 format and translates
 into the L1RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcmeas_fill_common_mcm_intra_f_info_r6
(
rrc_IntraFrequencyMeasurement_r6* intra_f_meas_ptr,
l1_meas_ctrl_parm_struct_type *l1_mcm ,
rrc_IntraFreqReportingCriteria_1b_r5* intraFreqReportingCriteria_1b_r5_ptr
);

/*===========================================================================

FUNCTION     RRCMEAS_FILL_INTRA_FREQ_EVENT_CRITERIA_R6

DESCRIPTION

 This function fills in intra-freq event criteria for a Non-HCS cell when
 Cell Selection Reselection Quality Measure is RCSP or ECNO for R5 Message
 The function processes intra-ftreq event criteria Info in ASN1 format
 and translates into the L1 RRC Interface format.

DEPENDENCIES

 The passed pointers must be valid.

RETURN VALUE

 No of events.

SIDE EFFECTS

 None

===========================================================================*/

uint16 rrcmeas_fill_intra_freq_event_criteria_r6
(
rrc_IntraFreqReportingCriteria_r6  *rrc_intra_freq_reporting_criteria_ptr,
l1_intra_freq_event_crit_struct_type *l1_intra_freq_event_crit_ptr,
rrc_IntraFreqReportingCriteria_1b_r5* intraFreqReportingCriteria_1b_r5_ptr
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM_R6

DESCRIPTION

    This function processes the release 6 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm_r6
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R6

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r6
(
 rrc_InterFreqReportCriteria_r6* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_EVENT_CRITERIA_R6

DESCRIPTION

    This function processes the event criteria received in a Measurement Control Message
  and stores in the format that is sent to L1

DEPENDENCIES

    None

RETURN VALUE

    FAILURE: If event criteria is invalid
    SUCCESS: If event criteria is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_event_criteria_r6
(
  rrc_InterFreqReportingCriteria_r6* rrc_event_criteria_ptr,
  l1_inter_freq_report_crit_struct_type* l1_event_criteria_ptr
);

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R6

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_r6
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_CELL_LIST_MCM_R6

DESCRIPTION

  This function processes the cell list received in a Measurement Control Message

DEPENDENCIES

  None

RETURN VALUE

  FAILURE: If Cell list is invalid
  SUCCESS: If Cell list is valid.

SIDE EFFECTS

  None

===========================================================================*/

rrcmeas_status_e_type rrcirat_process_cell_list_mcm_r6
(
  rrc_InterRATCellInfoList_r6* mcm_cell_list_ptr,
  l1_inter_sys_cell_list_struct_type* l1_cell_list_ptr
);

/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R6

DESCRIPTION

    Process traffic volume information in a Meas
    Ctrl Message R6. Also sends a Meas Ctrl Failure
    Message in case of an error


DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r6
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
);

/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM_R6

DESCRIPTION

 This function processes the Measurement Control Message rel 6

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm_r6
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
);



#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/*===========================================================================

FUNCTION   RRCGPS_PROCESS_MCM_R6

DESCRIPTION

    This function processes the rel 6 Measurement Control Message for
    A-GPS Measurements.

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_process_mcm_r6
(
  void* pos_mcm_ptr,
  pos_info_struct_type* p_info_ptr
  ,uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_POS_MCM_R6

DESCRIPTION

 This function processes the Positioning Measurement Control Message and
 determines if it is a A-GPS or Cell Id or OTDOA Message. Depending upon that
 it configures either LSM or L1 with measurement parameters

DEPENDENCIES

 None

RETURN VALUE

 FAILURE:
 SUCCESS:

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_pos_mcm_r6
(
  void* msg_ptr,
  pos_info_struct_type* pos_info_ptr,
  uint8 meas_ctrl_tr_id
);
#endif /*FEATURE_CGPS_UMTS_CP_WCDMA */

/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM_R6

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements for rel 6

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm_r6
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*==========================================================================
 FUNCTION   rrcifreq_free_memory_allocated_for_reporting

DESCRIPTION

  This function takes care of deallocating cell list which was allocated by L1 when it is 
  sending a inter frequency measurement indication to RRC
DEPENDENCIES

    None
RETURN VALUE

    None
SIDE EFFECTS

    None

===========================================================================*/
void rrcifreq_free_memory_allocated_for_reporting
(
  l1_meas_rpt_struct_type* l1_meas_report_ptr
);

#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif


/*===========================================================================

FUNCTION RRCLBT_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  RRC GPS  procedure. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrcgps_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================

FUNCTION  

DESCRIPTION

  This function initializes the RRC GPS procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcgps_init_procedure( void );
/*===========================================================================

FUNCTION  rrcgps_set_psc_for_cell_id_req

DESCRIPTION

  This function updates the psc for which cell id is requested
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcgps_set_psc_for_cell_id_req
 (
   uint32 psc
 );
/*===========================================================================

FUNCTION  rrcgps_get_psc_for_cell_id_req

DESCRIPTION

  This function returns the psc for which cell id is requested
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
uint32 rrcgps_get_psc_for_cell_id_req
(
 void
);

/*===========================================================================

FUNCTION  rrcgps_is_gps_event_active

DESCRIPTION

  This function will return status of rrcgps event if active.
    
DEPENDENCIES

  None.
 
RETURN VALUE

TRUE : if GPS event is active,
FALSE : Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcgps_is_gps_event_active
(
  void
);


/*===========================================================================

FUNCTION rrcmeas_read_sib11bis

DESCRIPTION

  Reads SIB11bis from the current active serving cell for all the measurements.
  If SIB11bis information is incorrect, meas setup command is not send to L1
  and UE does not do any measurements..

DEPENDENCIES

  SIB must have been stored by the SIB procedure

RETURN VALUE

  Success if SIB11bis has been correctly read
  Failure if SIB11bis has not been correctly read

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcmeas_read_sib11bis
(
  boolean sib18_present,
  rrc_PLMNsOfIntraFreqCellsList * tmp_rrc_PLMNsOfIntraFreqCellsList_ptr,
  rrc_MultiplePLMNsOfIntraFreqCellsList *tmp_rrc_MultiplePLMNsOfIntraFreqCellsList_ptr
);

/*===========================================================================

FUNCTION rrcifreq_read_sib11bis

DESCRIPTION

  Reads SIB11bis from the current active serving cell for all the measurements.
  If SIB11bis information is incorrect, meas setup command is not send to L1
  and UE does not do any measurements..

DEPENDENCIES

  SIB must have been stored by the SIB procedure

RETURN VALUE

  Success if SIB11bis has been correctly read
  Failure if SIB11bis has not been correctly read

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcifreq_read_sib11bis
(
  l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr,
  boolean sib18_present,
  rrc_PLMNsOfInterFreqCellsList *tmp_rrc_PLMNsOfInterFreqCellsList_ptr,
  rrc_MultiplePLMNsOfInterFreqCellsList* tmp_rrc_MultiplePLMNsOfInterFreqCellsList_ptr
);

/*===========================================================================

FUNCTION rrcirat_read_sib11bis

DESCRIPTION

  Reads SIB11bis from the current active serving cell for all the measurements.
  If SIB11bis information is incorrect, meas setup command is not send to L1
  and UE does not do any measurements..

DEPENDENCIES

  SIB must have been stored by the SIB procedure

RETURN VALUE

  Success if SIB11bis has been correctly read
  Failure if SIB11bis has not been correctly read

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_read_sib11bis
(
  l1_meas_sib_parm_struct_type* l1_inter_rat_sib_ptr,
  boolean sib18_present,
  rrc_PLMNsOfInterRATCellsList *tmp_rrc_PLMNsOfInterRATCellsList_ptr
);

/*===========================================================================

FUNCTION  rrcmeas_get_int_trans_index_for_trans_id

DESCRIPTION

  This function will return internal transaction index given the trans_id of the MCM and updates the current idx in accepted
  transactions as latest
    
DEPENDENCIES

  None.
 
RETURN VALUE

Internal transaction index

SIDE EFFECTS

  None.

===========================================================================*/
uint8 rrcmeas_get_int_trans_index_for_trans_id
(
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   rrcmeas_process_mcm_r7

DESCRIPTION

    This function processed the Release 7 Meas Ctrl Message

DEPENDENCIES

    None

RETURN VALUE
   Void


SIDE EFFECTS

    None

===========================================================================*/

void rrcmeas_process_mcm_r7
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCMEAS_RETRIEVE_MEAS_OWNER_R7

DESCRIPTION

 Finds the owner of measurement identity given in a MCM Rel 7 Msg

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

rrcmeas_layer_e_type rrcmeas_retrieve_meas_owner_r7
(
rrc_MeasurementCommand_r7* mcm_ptr,
uint32 meas_id
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
,pos_info_struct_type* pos_info_ptr
#endif
);

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/*===========================================================================

FUNCTION   RRCGPS_PROCESS_MCM_R7

DESCRIPTION

    This function processes the rel 7 Measurement Control Message for
    A-GPS Measurements.

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_process_mcm_r7
(
  void* pos_mcm_ptr,
  pos_info_struct_type* p_info_ptr
  ,uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_POS_MCM_R7

DESCRIPTION

 This function processes the Positioning Measurement Control Message and
 determines if it is a A-GPS or Cell Id or OTDOA Message. Depending upon that
 it configures either LSM or L1 with measurement parameters

DEPENDENCIES

 None

RETURN VALUE

 FAILURE:
 SUCCESS:

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_pos_mcm_r7
(
  void* msg_ptr,
  pos_info_struct_type* pos_info_ptr,
  uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCGPS_FILL_MCM_R7_INFO

DESCRIPTION

    This function fill the GPS structure for 
    A-GPS Measurements. 

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_fill_mcm_info_r7
(
  pos_info_struct_type* pos_info_ptr,                       
  rrc_UE_Positioning_Measurement_r7 *up_Measurement_ptr,
  rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
  uint8 transaction_id,
  boolean modify_meas_params_incl
);

/*===========================================================================

FUNCTION    RRCGPS_VERIFY_POSITIONING_CAPABILITY

DESCRIPTION

 Checks positioning capability to determine if UE could process
 A-GPS Measurement Control Message

DEPENDENCIES

 None.

RETURN VALUE

 Success/Failure

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcgps_verify_positioning_capability_r7
(
  const rrc_UE_Positioning_ReportingQuantity_r7 *rpt_quan_ptr
);

#ifdef FEATURE_CGPS_UMTS_CP_WCDMA

/*===========================================================================

FUNCTION
  RRCGPS_CONVERT_ASSISTANCE_DATA

DESCRIPTION
  This is a function takes a rrc_UE_Positioning_GPS_AssistanceData structure 
  filled in by the oss_decode() function and returns a rrc_assistance_data
  structure. 

  The ASN.1 structure makes frequent use of linked lists. When these are encountered
  the data is copied into the appropriate array in the RRC structure.

PARAMETERS
  asn1_ptr - Pointer to ASN.1 data structure output from the OSS decoder
  rrc_ptr - Pointer to the RRC structure to be filled in

RETURN VALUE
  TRUE if conversion succeeded, FALSE if an error occurred.

===========================================================================*/
boolean rrcgps_convert_assistance_data_r7( 
  rrc_UE_Positioning_GPS_AssistanceData_r7* asn1_ptr,
  rrc_assistance_data_struct_type* rrc_ptr 
);
  #endif
  
#endif /*FEATURE_CGPS_UMTS_CP_WCDMA */

/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_INTRAFREQ_R7

DESCRIPTION

 Processed Intra Freq Meas in Rel7 Meas Ctrl Message

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcmeas_process_intrafreq_r7
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM_R7

DESCRIPTION

    This function processes the release 7 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm_r7
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R7

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message R7. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r7
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
);

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R7

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_r7
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM_R7

DESCRIPTION

 This function processes the Measurement Control Message rel 7

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm_r7
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
);

/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM_R7

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements for rel 7

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm_r7
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION  RRCMEAS_FILL_COMMON_MCM_INTRA_F_INFO_R7

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 The function processes Cell Info in ASN1 format and translates
 into the L1RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcmeas_fill_common_mcm_intra_f_info_r7
(
rrc_IntraFrequencyMeasurement_r7* intra_f_meas_ptr,
l1_meas_ctrl_parm_struct_type *l1_mcm 
);

/*===========================================================================

FUNCTION     RRCMEAS_FILL_INTRA_FREQ_EVENT_CRITERIA_R7

DESCRIPTION

 This function fills in intra-freq event criteria for a Non-HCS cell when
 Cell Selection Reselection Quality Measure is RCSP or ECNO for R7 Message
 The function processes intra-ftreq event criteria Info in ASN1 format
 and translates into the L1 RRC Interface format.

DEPENDENCIES

 The passed pointers must be valid.

RETURN VALUE

 No of events.

SIDE EFFECTS

 None

===========================================================================*/

uint16 rrcmeas_fill_intra_freq_event_criteria_r7
(
rrc_IntraFreqReportingCriteria_r7  *rrc_intra_freq_reporting_criteria_ptr,
l1_intra_freq_event_crit_struct_type *l1_intra_freq_event_crit_ptr
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R7

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r7
(
 rrc_InterFreqReportCriteria_r7* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
);

#ifdef FEATURE_GAN_3G
#error code not present
#endif

/*===========================================================================

FUNCTION   rrcmeas_process_mcm_r8

DESCRIPTION

    This function processed the Release 8 Meas Ctrl Message

DEPENDENCIES

    None

RETURN VALUE
   Void


SIDE EFFECTS

    None

===========================================================================*/

void rrcmeas_process_mcm_r8
(
  rrc_cmd_type *cmd_ptr,
  uint8 meas_ctrl_tr_id
);

/*===========================================================================
FUNCTION   RRCMEAS_RETRIEVE_MEAS_OWNER_R8

DESCRIPTION

 Finds the owner of measurement identity given in a MCM Rel 7 Msg

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
rrcmeas_layer_e_type rrcmeas_retrieve_meas_owner_r8
(
rrc_MeasurementCommand_r8* mcm_ptr,
uint32 meas_id
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
,pos_info_struct_type* pos_info_ptr
#endif
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_MCM_R8

DESCRIPTION

    This function processes the release 7 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_mcm_r8
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================
FUNCTION   RRCMEAS_PROCESS_INTRAFREQ_R8

DESCRIPTION

 Processed Intra Freq Meas in Rel8 Meas Ctrl Message

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcmeas_process_intrafreq_r8
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);


/*===========================================================================
FUNCTION   RRCMEAS_PROCESS_INTRAFREQ_R8

DESCRIPTION

 Processed Intra Freq Meas in Rel8 Meas Ctrl Message

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

void rrcmeas_meas_ctrl_fail_action
(
  uint8 transaction_id,
  uint32 meas_err_cause
);



/*===========================================================================

FUNCTION     RRCTVM_PROCESS_TRAFFIC_VOL_INFO_R8

DESCRIPTION  

    Process traffic volume information in a Meas
    Ctrl Message R8. Also sends a Meas Ctrl Failure
    Message in case of an error
    
                    
DEPENDENCIES

    None.

RETURN VALUE

    None.

SIDE EFFECTS

    None

===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r8
(
  rrc_cmd_type* cmd_ptr,
  uint32 tid
);

/*===========================================================================

FUNCTION   RRCUEIM_PROCESS_MCM_R8

DESCRIPTION 

 This function processes the Measurement Control Message rel 8

DEPENDENCIES

 None

RETURN VALUE

 Success if message successfully processed
 Failure otherwise 

SIDE EFFECTS

 None
===========================================================================*/

uecomdef_status_e_type rrcueim_process_mcm_r8
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
);

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_POS_MCM_R8

DESCRIPTION

 This function processes the Positioning Measurement Control Message and
 determines if it is a A-GPS or Cell Id or OTDOA Message. Depending upon that
 it configures either LSM or L1 with measurement parameters

DEPENDENCIES

 None

RETURN VALUE

 FAILURE:
 SUCCESS:

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_pos_mcm_r8
(
  void* msg_ptr,
  pos_info_struct_type* pos_info_ptr,
  uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCGPS_PROCESS_MCM_R7

DESCRIPTION

    This function processes the rel 7 Measurement Control Message for
    A-GPS Measurements.

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_process_mcm_r8
(
  void* pos_mcm_ptr,
  pos_info_struct_type* p_info_ptr
  ,uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCGPS_FILL_MCM_R8_INFO

DESCRIPTION

    This function fill the GPS structure for 
    A-GPS Measurements. 

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_fill_mcm_info_r8
(
  pos_info_struct_type* pos_info_ptr,                       
  rrc_UE_Positioning_Measurement_r8 *up_Measurement_ptr,
  rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
  uint8 transaction_id,
  boolean modify_meas_params_incl
);

/*===========================================================================

FUNCTION    RRCGPS_VERIFY_POSITIONING_CAPABILITY

DESCRIPTION

 Checks positioning capability to determine if UE could process
 A-GPS Measurement Control Message

DEPENDENCIES

 None.

RETURN VALUE

 Success/Failure

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcgps_verify_positioning_capability_r8
(
  const rrc_UE_Positioning_ReportingQuantity_r8 *rpt_quan_ptr
);
#endif /*FEATURE_CGPS_UMTS_CP_WCDMA*/

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R8

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrcirat_process_mcm_r8
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*==========================================================================
 FUNCTION   RRCQM_PROCESS_MCM_R8

DESCRIPTION

  This function processes the Measurement Control Message for
  Quality Measurements for rel 8

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcqm_process_mcm_r8
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================
FUNCTION  rrcifreq_fill_inter_freq_info_r8

DESCRIPTION
 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List for rel 8

DEPENDENCIES
 None

RETURN VALUE
 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS
 None
===========================================================================*/
rrcmeas_status_e_type rrcifreq_fill_inter_freq_info_r8
(
 rrc_InterFreqCellInfoList_r8* interFreqCellInfoList_ptr,
 l1_meas_ctrl_parm_struct_type* l1_meas_ptr
);


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/*===========================================================================

FUNCTION   rrcmeas_configure_l1_with_priority_info_eutra_cell_list

DESCRIPTION 
  This function uses stored dedicated priority info and SIB19 to configure WL1 with priority info and E-UTRA cell list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_configure_l1_with_priority_info_eutra_cell_list
(
  void
);

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*===========================================================================

FUNCTION   rrcmeas_handle_get_dedicated_pri_req

DESCRIPTION 
  This function handles the get dedicated priorities req from inter RAT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcmeas_handle_get_dedicated_pri_req
(
  rrc_cmd_type* cmd_ptr
);
#endif

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION   rrcirat_initialize_eutra_freq_list

DESCRIPTION 
  This function initializes EUTRA frequency list stored

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcirat_initialize_eutra_freq_list
(
  void
);
/*===========================================================================

FUNCTION   rrcirat_update_eutra_freq_list

DESCRIPTION 
  This function updates stored EUTRA frequency with EUTRA freq received in the argument

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA frequency is updated in the DB
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcirat_update_eutra_freq_list
(
  uint32 earfcn
);
#endif
#endif

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION   rrc_is_earfcn_in_deprio_list

DESCRIPTION 
  This function checks if earfcn is present in deprio ;ist or not.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA frequency is present in deprio list
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean rrc_is_earfcn_in_deprio_list
(
  uint32 earfcn
);


#endif

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION   rrcirat_is_meas_id_eutra_meas

DESCRIPTION
  This function returns TRUE if the meas id is EUTRA meas id

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the meas id is found in DB and is EUTRA measurement
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/

boolean rrcirat_is_meas_id_eutra_meas
(
  uint16 meas_id
);

#endif

/*===========================================================================

FUNCTION   rrcmeas_trash_and_send_meas_failure

DESCRIPTION
  This function send MCF for all active meas_ctrl msgs

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/

void rrcmeas_trash_and_send_meas_failure(void);
/*===========================================================================

FUNCTION   rrctvm_delete_sib_tvm_config

DESCRIPTION 

  Main handler that deletes the sib based tvm configuration
 
DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

  If SIB11/12 is not valid, then traffic volume measurements may not be done.

===========================================================================*/

void rrctvm_delete_sib_tvm_config(void);

/*===========================================================================

FUNCTION   rrcmeas_handle_cell_sel_meas_req_in_fach

DESCRIPTION 

  Handles cell sel meas req / new cell ind in cell FACH state
 
DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

  None

===========================================================================*/

void rrcmeas_handle_cell_sel_meas_req_in_fach(void);

#ifdef FEATURE_GANSS_SUPPORT
/*==============================================
FUNCTION   rrcgps_fill_measured_results_ganss_incl

DESCRIPTION
  This function fills the measurement report extn recieved from LSM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================
*/

void rrcgps_fill_measured_results_ganss_incl(
  rrc_UL_DCCH_Message *msg_ptr,
  rrc_meas_rpt_struct_type *lsm_meas_ptr
  );


/*====================================================
FUNCTION   rrcgps_fill_meas_ctrl_ganss_assistance_data

DESCRIPTION
  This function fills the measurement control paramters as recieved 
  from UE positioning measurements

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===================================================
*/
void rrcgps_fill_meas_ctrl_ganss_assistance_data(
  rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
  rrc_UE_Positioning_Measurement_r8 *up_Measurement_ptr
);


/*=====================================================
FUNCTION   rrcgps_fill_lsm_assistance_data

DESCRIPTION
  This function fills the lsm assistance data  
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=====================================================
*/
void rrcgps_fill_lsm_assistance_data(
  rrc_DL_DCCH_Message *dcch_msg_ptr,
  rrc_meas_params_struct_type *lsm_assistance_data_ptr
);

/*=====================================================
FUNCTION   rrcgps_fill_measured_results_pos_err_incl

DESCRIPTION
  This function fills the measured results when positioning error is included.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=====================================================
*/

void rrcgps_fill_measured_results_pos_err_incl(
rrc_UL_DCCH_Message* msg_ptr,
rrc_meas_rpt_struct_type *lsm_meas_ptr
);
/* ====================================================
FUNCTION   rrcgps_fill_measured_result_pos_est_incl

DESCRIPTION
  This function fills the measured results if positioning estimate included
  

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None


======================================================== */

boolean rrcgps_fill_measured_result_pos_est_incl(
rrc_UL_DCCH_Message* msg_ptr,
rrc_meas_rpt_struct_type *lsm_meas_ptr
);

/* ====================================================
FUNCTION   rrcgps_set_bitmask_IE_ext770

DESCRIPTION
  This function sets the bitmask till 770 ext IE
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


======================================================== */
void rrcgps_set_bitmask_IE_ext770(
rrc_UL_DCCH_Message* msg_ptr
);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_ganss_id

DESCRIPTION

 Fills ganss Id IE for generic data

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_generic_data_ganss_id(
rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
uint8 ganss_id,uint8 index);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_time_model_list

DESCRIPTION

 Fills time model list IE for generic data

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_generic_data_time_model_list(
rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
rrc_GANSSTimeModelsList ganssTimeModelsList,uint8 index);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_add_navigation_models

DESCRIPTION

 Fills additional navigation models IE for generic data

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_generic_data_add_navigation_models(
rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
rrc_UE_Positioning_GANSS_AddNavigationModels uePositioningGANSSAddNavigationModels,uint8 index);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_real_time_integrity

DESCRIPTION

 Fills real time integrity IE for generic data

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_generic_data_real_time_integrity(
rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
rrc_UE_Positioning_GANSS_RealTimeIntegrity uePositioningGANSSRealTimeIntegrity,uint8 index);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_ref_meas_info

DESCRIPTION

 Fills reference measurement info IE for generic data

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_generic_data_ref_meas_info(
rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,
rrc_UE_Positioning_GANSS_ReferenceMeasurementInfo uePositioningGANSSReferenceMeasurementInfo,uint8 index);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_almanac_r8

DESCRIPTION

 Fills almanac_r8 IE for generic data

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_generic_data_almanac_r8(
rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,
rrc_UE_Positioning_GANSS_Almanac_r8 uePositioningGANSSAlmanac,uint8 index);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_add_utc_models

DESCRIPTION

 Fills additional UTC models IE for generic data.

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_fill_generic_data_add_utc_models(
rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,
rrc_UE_Positioning_GANSS_AddUTCModels uePositioningGANSSAddUTCModels,uint8 index);
/*===========================================================================

FUNCTION    rrcgps_fill_generic_data_aux_info

DESCRIPTION

 Fills  auxiliary info IE for generic data.

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_generic_data_aux_info(
rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,
rrc_UE_Positioning_GANSS_AuxiliaryInfo uePositioningGANSSAuxiliaryInfo,uint8 index);


#ifdef FEATURE_WCDMA_REL9
/*===========================================================================

FUNCTION    rrcgps_fill_meas_ctrl_ganss_assistance_data_r9

DESCRIPTION

 Fills  ganss assistance data for rel9

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/
void rrcgps_fill_meas_ctrl_ganss_assistance_data_r9(
rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
rrc_UE_Positioning_Measurement_r9 *up_Measurement_ptr);

#endif
#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

#endif

#ifdef FEATURE_WCDMA_REL9


/*===========================================================================
FUNCTION   RRCMEAS_PROCESS_INTRAFREQ_R9

DESCRIPTION

 Processed Intra Freq Meas in Rel9 Meas Ctrl Message

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcmeas_process_intrafreq_r9
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);



/*===========================================================================
FUNCTION   RRCMEAS_RETRIEVE_MEAS_OWNER_R9

DESCRIPTION

 Finds the owner of measurement identity given in a MCM Rel 9 Msg

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/

rrcmeas_layer_e_type rrcmeas_retrieve_meas_owner_r9
(
rrc_MeasurementCommand_r9* mcm_ptr,
uint32 meas_id
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
,pos_info_struct_type* pos_info_ptr
#endif
);


/*===========================================================================

FUNCTION   rrcmeas_process_mcm_r9

DESCRIPTION

    This function processed the Release 9 Meas Ctrl Message

DEPENDENCIES

    None

RETURN VALUE
   Void


SIDE EFFECTS

    None

===========================================================================*/

void rrcmeas_process_mcm_r9
(
  rrc_cmd_type *cmd_ptr,
  uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCMEAS_CHECK_IF_MEAS_ID_EXTN_IS_GIVEN

DESCRIPTION

    This function is used to check if measurement id extension is given

DEPENDENCIES

    None

RETURN VALUE
   Void


SIDE EFFECTS

    None

===========================================================================*/
boolean rrcmeas_check_if_meas_id_extn_is_given
(
rrc_MeasurementControl_r9 *mcm_r9, 
uint8 *meas_id
);


/*===========================================================================

FUNCTION   RRCMEAS_CHECK_IF_ADDNL_MEAS_ID_EXTN_IS_GIVEN

DESCRIPTION

  This function is used to check if additional measurement id extension is given

DEPENDENCIES

    None

RETURN VALUE
   Void


SIDE EFFECTS

    None

===========================================================================*/
boolean rrcmeas_check_if_addnl_meas_id_extn_is_given
(
  rrc_MeasurementControl_r9 *mcm_r9
);

#ifdef FEATURE_CGPS_UMTS_CP_WCDMA
/*===========================================================================

FUNCTION   RRCMEAS_PROCESS_POS_MCM_R9

DESCRIPTION

 This function processes the Positioning Measurement Control Message and
 determines if it is a A-GPS or Cell Id or OTDOA Message. Depending upon that
 it configures either LSM or L1 with measurement parameters

DEPENDENCIES

 None

RETURN VALUE

 FAILURE:
 SUCCESS:

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_process_pos_mcm_r9
(
  void* msg_ptr,
  pos_info_struct_type* pos_info_ptr,
  uint8 meas_ctrl_tr_id
);

/*===========================================================================

FUNCTION   RRCGPS_FILL_MCM_R9_INFO

DESCRIPTION

    This function fill the GPS structure for 
    A-GPS Measurements. 

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_fill_mcm_info_r9
(
  pos_info_struct_type* pos_info_ptr,                       
  rrc_UE_Positioning_Measurement_r9 *up_Measurement_ptr,
  rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,
  uint8 transaction_id,
  boolean modify_meas_params_incl,
  rrc_MeasurementControl_r9 *mcm_r9
);

#endif
/*===========================================================================

FUNCTION
  RRCGPS_CONVERT_ASSISTANCE_DATA_R9

DESCRIPTION
  This is a function takes a rrc_UE_Positioning_GPS_AssistanceData structure 
  filled in by the oss_decode() function and returns a rrc_assistance_data
  structure. 

  The ASN.1 structure makes frequent use of linked lists. When these are encountered
  the data is copied into the appropriate array in the RRC structure.

PARAMETERS
  asn1_ptr - Pointer to ASN.1 data structure output from the OSS decoder
  rrc_ptr - Pointer to the RRC structure to be filled in

RETURN VALUE
  TRUE if conversion succeeded, FALSE if an error occurred.

===========================================================================*/
boolean rrcgps_convert_assistance_data_r9( 
  rrc_UE_Positioning_GPS_AssistanceData_r9* asn1_ptr,
  rrc_assistance_data_struct_type* rrc_ptr );

/*===========================================================================

FUNCTION   RRCIRAT_PROCESS_MCM_R9

DESCRIPTION

  This function processes the Measurement Control Message

DEPENDENCIES

  None.

RETURN VALUE

  FAILURE: If Meas Ctrl is invalid
  SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type rrcirat_process_mcm_r9
(
  rrc_cmd_type *cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   rrcirat_process_mcm_setup_modify_eutra_R9

DESCRIPTION 
  This function processes setup or modify of EUTRA IRAT measurement IE from MCM

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA measurement IE from MCM is successfully processed
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcirat_process_mcm_setup_modify_eutra_r9
(
  rrc_InterRATMeasurement_r9 *mcm_irat_ptr,
  l1_meas_ctrl_parm_struct_type *l1_inter_rat_meas_params,
  uint32 transaction_id,
  rpt_crit_enum_type *mcm_rpt_crit_ptr
);

/*===========================================================================

FUNCTION   rrcirat_process_eutra_freq_list_mcm_R9

DESCRIPTION 
  This function processes EUTRA freq list from MCM

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS : If EUTRA frequency list is successfully processed
  FAILURE   : Otherwise

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcirat_process_eutra_freq_list_mcm_r9
(
  rrc_EUTRA_FrequencyList_r9 *eutra_freq_list,
  l1_inter_sys_eutra_freq_list_struct_type *l1_freq_list
);

/*===========================================================================
FUNCTION   RRCIFREQ_PROCESS_MCM_R9

DESCRIPTION
    This function processes the release 9 Measurement Control Message for
  Inter-Freq Measurements

DEPENDENCIES
    None.

RETURN VALUE
    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS
    None
==========================================================================*/
uecomdef_status_e_type rrcifreq_process_mcm_r9
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION   RRCIFREQ_PROCESS_REPORTING_CRITERIA_R9

DESCRIPTION

    This function process the reporting criteria specified
  in Measurement Control Message

DEPENDENCIES

    None

RETURN VALUE

    Success or Failure depending upon processing of reporting criteria

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcifreq_process_reporting_criteria_r9
(
 rrc_InterFreqReportCriteria_r9* ifreq_rpt_criteria_ptr,
 l1_inter_freq_meas_ctrl_struct_type* l1_rpt_params_ptr
);

/*===========================================================================
FUNCTION  rrcifreq_fill_inter_freq_info_r9

DESCRIPTION
 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 Also updates the Inter Freq Cell Info List for rel 8

DEPENDENCIES
 None

RETURN VALUE
 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS
 None
===========================================================================*/
rrcmeas_status_e_type rrcifreq_fill_inter_freq_info_r9
(
 rrc_InterFreqCellInfoList_r9* interFreqCellInfoList_ptr,
 l1_meas_ctrl_parm_struct_type* l1_meas_ptr
);

/*===========================================================================

FUNCTION     RRCMEAS_FILL_INTRA_FREQ_EVENT_CRITERIA_R9

DESCRIPTION

 This function fills in intra-freq event criteria for a Non-HCS cell when
 Cell Selection Reselection Quality Measure is RCSP or ECNO for R7 Message
 The function processes intra-ftreq event criteria Info in ASN1 format
 and translates into the L1 RRC Interface format.

DEPENDENCIES

 The passed pointers must be valid.

RETURN VALUE

 No of events.

SIDE EFFECTS

 None

===========================================================================*/

uint16 rrcmeas_fill_intra_freq_event_criteria_r9
(
  rrc_IntraFreqReportingCriteria_r9  *rrc_intra_freq_reporting_criteria_ptr,
  l1_intra_freq_event_crit_struct_type *l1_intra_freq_event_crit_ptr
);

/*===========================================================================

FUNCTION  RRCMEAS_FILL_MCM_CELL_INFO_R9

DESCRIPTION

 This function fills in Cell Info received from a R9
 Measurement Control Message. The function processes
 Cell Info in ASN1 format and translates into the L1
 RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill cell info correctly.
 SUCCESS: If cell info is filled correctly

SIDE EFFECTS

 None

===========================================================================*/

uecomdef_status_e_type rrcmeas_fill_mcm_cell_info_r9
(
rrc_CellInfo_r9* mcm_cell_info_ptr,
l1_cell_info_struct_type* l1_cell_info_ptr
);

/*===========================================================================

FUNCTION  RRCMEAS_PROCESS_MCM_INTRA_F_CELL_INFO_R9

DESCRIPTION

 This function processes the Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 The function processes Cell Info in ASN1 format and translates
 into the L1RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill mcm cell info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcmeas_process_mcm_intra_f_cell_info_r9
(
rrc_IntraFreqCellInfoList_r9* intra_freq_cell_info_list_ptr,
l1_intra_freq_meas_ctrl_struct_type* l1_intra_f_mcm_ptr
);

/*===========================================================================

FUNCTION  RRCMEAS_FILL_COMMON_MCM_INTRA_F_INFO_R9

DESCRIPTION

 This function fills in common Cell Info for meas setup and
 meas modify command type in a Measurement Control Message.
 The function processes Cell Info in ASN1 format and translates
 into the L1RRC Interface format.

DEPENDENCIES

 None

RETURN VALUE

 FAILURE: If unable to fill common mcm info correctly
 SUCCESS: If common mcm info filled correctly

SIDE EFFECTS

 None

===========================================================================*/

rrcmeas_status_e_type rrcmeas_fill_common_mcm_intra_f_info_r9
(
rrc_IntraFrequencyMeasurement_r9* intra_f_meas_ptr,
l1_intra_freq_meas_ctrl_struct_type* l1_intra_f_mcm_ptr,
l1_meas_ctrl_parm_struct_type *l1_mcm 
);

/*===========================================================================
FUNCTION   RRCMEAS_PROCESS_INTRAFREQ_R9

DESCRIPTION

 Processed Intra Freq Meas in Rel9 Meas Ctrl Message

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcmeas_process_intrafreq_r9
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

/*===========================================================================
FUNCTION   RRCMEAS_PROCESS_MCM_ADDNL_MEAS

DESCRIPTION

 Processed the additional measurements IE received in Rel 9 Measurement control message

DEPENDENCIES

 None

RETURN VALUE
 boolean

SIDE EFFECTS

 None
===========================================================================*/
boolean rrcmeas_process_mcm_addnl_meas
(
  rrc_MeasurementControl_r9 *mcm_r9,
  uint8 transaction_id,
  boolean *addtl_meas_validation,
  rrcmeas_layer_e_type primary_meas_type,
  uint8 meas_id,
  meas_validity_enum_type* p_meas_validity
);


/*===========================================================================
FUNCTION   RRCMEAS_CHECK_ADDNL_MEAS_IN_DB

DESCRIPTION

 Set the additional measurement information in DB based on measurement id
 extension is present or not

DEPENDENCIES

 None

RETURN VALUE
 None

SIDE EFFECTS

 None
===========================================================================*/
void rrcmeas_check_addtl_meas_in_db
(
  rrcmeas_layer_e_type *primary_meas_type,
  rrc_MeasurementControl_r9 *mcm_r9, 
  uint8 meas_id);

/*===========================================================================
FUNCTION   rrctvm_process_traffic_vol_info_r9

DESCRIPTION

Process traffic volume info rel 9

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
rrctvm_status_e_type rrctvm_process_traffic_vol_info_r9
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
);

/*===========================================================================
FUNCTION   rrcueim_process_mcm_r9

DESCRIPTION

 Processed UE internal measurements rel 9

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcueim_process_mcm_r9
(
  rrc_cmd_type* cmd_ptr,
  uint32 transaction_id
);

/*===========================================================================
FUNCTION   rrcqm_process_mcm_r9

DESCRIPTION

Process quality measurements rel 9

DEPENDENCIES

 None

RETURN VALUE

 None

SIDE EFFECTS

 None
===========================================================================*/
uecomdef_status_e_type rrcqm_process_mcm_r9
(
  rrc_cmd_type* cmd_ptr,
  uint8 transaction_id
);

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */

/*===========================================================================
FUNCTION  rrcmeas_process_sib18_filtering

DESCRIPTION
This function does a one to one comparison on cell list 
passed with the SIB18 elements and determines if the cell 
needs to be measured in non-DCH states..

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS
 None
===========================================================================*/
void rrcmeas_process_sib18_filtering(
  rrc_MultiplePLMNsOfIntraFreqCellsList *tmp_rrc_MultiplePLMNsOfIntraFreqCellsList_ptr,
  rrc_PLMNsOfIntraFreqCellsList *tmp_rrc_PLMNsOfIntraFreqCellsList_ptr,
  rrc_plmn_identity_type *tmp_plmn_identity_ptr,
  uint16   cell_count,
  boolean *measure_in_idle_pch_fach_ptr,
  boolean previous_measure_in_idle_pch_fach /* Previous cell value if exists*/
);
/*===========================================================================

FUNCTION   rrc_qsh_log_meas_rpt_params

DESCRIPTION

  This function processes process periodic reporting critera for secondary freq
present in REl-10 MCM


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrc_qsh_log_meas_rpt_params(rrc_log_rrc_mrm_ota *mrm_params);

/*===========================================================================

FUNCTION   rrc_qsh_log_meas_ctrl_params

DESCRIPTION

  This function processes process periodic reporting critera for secondary freq
present in REl-10 MCM


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrc_qsh_log_meas_ctrl_params(rrc_log_rrc_mcm_ota *mcm_params);

/*===========================================================================

FUNCTION   rrc_qsh_log_mcm_fail_params

DESCRIPTION

  This function processes process periodic reporting critera for secondary freq
present in REl-10 MCM


DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrc_qsh_log_mcm_fail_params(uint32 fail_cause);
#endif /* RRCMEASI_H */
