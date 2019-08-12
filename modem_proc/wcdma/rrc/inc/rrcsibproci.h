#ifndef RRCSIBPROCI_H
#define RRCSIBPROCI_H
/*===========================================================================
                        R R C   S I B   P R O C E S S I N G  

DESCRIPTION 

  This is the internal header file for the SIB processing module.
  This is shared only between the SIB processing modules and is
  not visible to other RRC modules.
  
  This file contains the definition of the SIB events database that
  is used to hold information on SIB events that are currently active.

Copyright (c) 2000-2005, 2007-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsibproci.h_v   1.18   31 May 2002 18:18:18   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcsibproci.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/15   ad      Made changes to take UE to idle if SIB5 capability changes from non-hsfach
                   to hsfach (vice versa) and there is a cell update being initiated
11/07/13   bc      Made changes to abort sib reading if we find that the cell is barred 
                   or la is forbidden.
06/12/13   sn      Changes to not invalidate SIBDB when MIB value tag matches
05/23/13   sr      Added support for SIB19/SIB19 deferring
02/08/13   sn      Changes to handle SB1/SB2 removal from scheduling
12/07/12   sn      CSG feature changes
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/02/12   sn      Changes to build and send SIB schedule change indication after 
                   receiving SIB7 in FACH state
02/15/12   gv      Added support to log new event when complete SIB is received
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/01/10   rm      Fixed compilation issues
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
05/11/09   dm      Feature to dynamically allocate sib_event ptr
05/08/09   ss      Updated Copyright Information
02/02/09   kp      Added support for optimized SIB heap under feature flag 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP
10/07/08   ns      Added code to store SIB segments received before the scheduling block. 
                   This appears as part of the feature FEATURE_RRC_TSIB_OPTIMIZATION.
05/29/08   ns      Added code to take care of the SIB segments being dropped when 
                   the offset is greater than 255 
04/24/08   ns      Fixed klockwork errors
07/09/07   kp      Added support for Tsib optimization db.
03/28/07   kp      Code clean up for bcch_modification_time  in structure 
                   sib_active_event_database_type, as BCCH Mod Timer is implemented 
                   in PG1 module.
11/09/05   kc      Changes to setup PCCPCH based on L1 request
05/27/05   kc      Added var's to keep track of max_rept_rate received for this cell
11/22/04   kc      Support for BPLMN
13/09/04   kc      Added var's to keep track of sib's scheduled per event and also
                   to keep track if SB's are decoded before SIB-DB is found.
08/10/04   kc      Constant definitions to support SIB-Scheduling
04/20/04   kc      Changed MAX_SEG_INDEX_FOR_A_SIB from 14 to 15.
07/15/04   kc      Defined invalid-mib-sb-value-tag and invalid-mod-time const
06/05/03   ram     Moved some data types to a different area for cleanup 
                   purposes.
04/29/03   ram     Changed some data types to fix lint errors.
05/09/02   ram     Added the name of the requesting procedure to the
                   structure sib_active_event_type.
02/08/02   ram     Fixed incorrect comment in previous checkin.
02/06/02   ram     Added bcch_modification_time to sib_active_event_type.
                   Although it's not used currently it needs to be saved
                   when we get a RRC_UPDATE_SIBS_IND command.
02/02/02   ram     Added an indication of whether the MIB's position has
                   been validated for an event. This is in the structure 
                   sib_active_events_type.
09/05/01   ram     For a fixed complete SIB, the length can be upto 226 bits
                   as per the December spec. Added separate definitions for
                   complete sib length in bits and bytes.
06/26/01   ram     Updated sib_events structure to indicate if an event is for
                   the serving cell or a neighbor cell. This is used only for the
                   "GET_SPECIFIC_SIBS" command. This is used to send the correct
                   frequency and scrambling code in the confirmation command.
06/11/01   ram     Updated sib_events structure to store only one BCCH information
                   for serving cell and one for neighbor cell. Earier there was one
                   per sib event. Added support for storing the new MIB value tag
                   when the MIBs value tag changes.
05/03/01   ram     Removed the structure that holds optional sib info. This is not
                   needed since the sib database is directly checked to see if
                   optional sibs are present in a cell.
04/09/01   ram     Added structure that's used to keep track of whether optional
                   SIBs (SIB4, SIB6 and SIB12) are present in a cell or not.
                   Added a definition for maximum segment index for a segmented
                   SIB.
03/08/01   ram     Added more elements to the sib_bcch_action_e_type enum in order
                   to enhance BCCH setup and teardown mechanism. Removed element
                   bcch_active from struct sib_active_bcch_setup_type since it's
                   no longer needed.
01/26/01   ram     Added definition MAX_SIB_LENGTH_BITS. Changed the type
                   of seg_count and seg_index to match the types in
                   rrcasn1. Updated comments.
01/25/01   ram     Various updates to support changes in the SIB processing
                   functionality. Added enum types sib_bcch_action_e_type and
                   sib_event_name_e_type to indicate the action wrt the BCCH
                   for an event and a name to represent the event. Added macro
                   GET_PDU_LENGTH_IN_BYTES to convert the given (non-octet
                   alligned) length in bits to bytes. Updated copyright.
12/20/00   ram     Moved the definition of RRC_TOT_SIBS to rrcdata.h.
12/15/00   ram     Since SIBs for a cell cannot be put in the database until 
                   the MIB and SIB3 are decoded, added a structure to temporarily
                   hold the decoded MIB and SIB3 within the event database.
                   Added an indication to the temp_sib_holder structure to indicate
                   which SIBs have been decoded already.
12/08/00   ram     Included rrcasn1.h which contains SIB related type definitions
                   used in this file.
12/01/00   ram     Changed MAX_SIB_EVENTS to MAX_SIB_EVENTS_INDEX
12/01/00   ram     Updated SIB events datastructures to support serving
                   cell and neighbor cell events. Added a 
                   sib_events_index_e_type to point to the indices of active
                   sib events.
10/23/00   ram     Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rrcasn1.h"
#include "rrcdata_v.h"
#include "rrcintif.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Maximum allowed length for an encoded SIB.
This is calculated like this - Total of 16 segments
are allowed for a SIB. Maximum data bits per segment
is 222. So the total - 16*222/8 = 444 bytes */
#define MAX_SIB_LENGTH_BYTES 444
#define INVALID_PDU_LENGTH 0

/* The same length in bits */
#define MAX_SIB_LENGTH_BITS (MAX_SIB_LENGTH_BYTES*8)

/* Maximum number of BCCHs allowed to be setup for
SIB events */
#define MAX_SIB_BCCH 2

/* Length in bits of a "SIB-Data-Fixed" as defined
by the standard. */
#define FIXED_SIB_DATA_LENGTH_BITS 222

/* Length in bytes of a "SIB-Data-Fixed" (222/8) rounded up. */
#define FIXED_SIB_DATA_LENGTH_BYTES 28

/* Length in bits of a "SIB-Data-Fixed" for a complete
SIB as defined by the standard. */
#define FIXED_COMPLETE_SIB_DATA_LENGTH_BITS 226

/* Length in bytes of a "SIB-Data-Fixed" for a complete
sib (226/8) rounded up. */
#define FIXED_COMPLETE_SIB_DATA_LENGTH_BYTES 29

/* Maximum number of segments allowed for a segmented SIB */
#define MAX_SEGMENTS_FOR_A_SIB 16 

/* Maximum Segment count allowed for a segmented SIB. The
segment count starts from 0 for the first segment. */
#define MAX_SEG_COUNT_FOR_A_SIB (MAX_SEGMENTS_FOR_A_SIB - 1)

/* Maximum Segment Index allowed for a segmented SIB. This
is defined by the spec as 15 since the segment index for
the first segment is 0 and the first subsequent segment is
1. If a segmented SIB has only 2 segments, the last segment
has segment index 1. */
#define MAX_SEG_INDEX_FOR_A_SIB (MAX_SEGMENTS_FOR_A_SIB - 1)

/*invalid Value_tag to invalidate value tags for SB's and MIB's*/
#define INVALID_MIB_SB_VALUE_TAG 4096

/*invalid SFN for schedule and maintaining sfn time*/
#define INVALID_SIB_SCHEDULE_REP 4097

#define EVENT_INDEX_VALID(event_index) \
  (  \
    (event_index<MAX_SIB_EVENTS_INDEX)?(TRUE):(FALSE)\
  )

/* Maximum number of sib segments that can be holed in a temp 
 *buffer which is used for TSIB optimization. Currently its set to 55 
 *based on max number of active sibs segments which are in use, 
 *Below is the data 
 *SIB-11/12      - 14 segments at max  = 14*2  =  28
 *SIB-1/2/MIB    - 2 segments at max   = 2* 3  =  06
 *SIB3/4/5/6/18  - 4 segments at max   = 4*5   =  20  
 *SIB-7          - 1 segments at max   = 1*1   =  01  
 *----------------------------------------------------
 *Total Number of SIB segments                 = 55 
 *Note: looking into test results, we are fine tuning this value to 25 for now
 *If needed in future where N/W start using larger SIB segments for all SIB's 
 *then we need to change this value appropriately
 */
#define MAX_TEMP_SIB_SEGMENTS_TO_HOLD 25

/*Value for 1st segment index*/
#define FIRST_SIB_SEGMENT_INDEX_VAL 0

/*Value for complete sib segment index*/
#define COMPLETE_SIB_SEGMENT_INDEX_VAL 16

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
#ifdef FEATURE_WCDMA_TO_LTE
  #define RRC_DEFERRED_SIBS 5
#else
  #define RRC_DEFERRED_SIBS 4
#endif

#define MAX_DEFERRED_SIB_SEGMENTS_TO_HOLD 30
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Enumerated type of status of processing a SIB segment. */
typedef enum
{
  SEG_PROC_NONE,        /* No status to report yet       */
  SEG_PROC_SUCCEEDED,   /* Segment succesfully processed */
  SEG_PROC_FAILED,      /* Segment processing failed     */
  SEG_PROC_DUPLICATE,    /* This segment has been already received
                        and processed. */
  SEG_PROC_STORED   /* Segment processing Stored in temp sib optimization db  */
  ,SEG_PROC_COMPLETE   /* Segment processing from stored temp sib optimization db 
                         resulted in a complete SIB building */
  //More may be needed later
}seg_proc_stat_e_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_WCDMA_HS_FACH_DRX
  extern uint32 curr_sfn;
#endif
/* Order is changed to make sure that we first check whether SB1/SB2 are
   received or not before checking for other SIBs in check_even_completion_status*/
   typedef enum 
  {
    rrc_MIB = 0,
    rrc_SB1 = 1,
    rrc_SB2 = 2,
    rrc_SIB1 = 3,
    rrc_SIB2 = 4,
    rrc_SIB3 = 5,
    rrc_SIB4 = 6,
    rrc_SIB5 = 7,
    rrc_SIB6 = 8,
    rrc_SIB7 = 9,
    rrc_SIB11 = 10,
    rrc_SIB12 = 11,
    rrc_SIB18 = 12,
    rrc_SIB5bis = 13,
    rrc_SIB11bis = 14,
    rrc_SIB19 = 15,
    rrc_ExtensionType = 16,
    rrc_MAX_SIB = 17
  }rrc_SIB_type;


typedef struct
{
   uint8  num_segments;
   uint32  sfn_offset[MAX_SEGMENTS_FOR_A_SIB];   
   uint16 repitition_rate;                      
}sib_scheduling_info;


/* Data structure to hold an encoded SIB */

typedef struct
{
  boolean sib_needed;     /* Indicates whether this SIB is needed or not.
                          If this is set to FALSE, the data below is 
                          irrelavant. */
  boolean sib_received;   /* Indicate whether a SIB has
                          been completely received */
  boolean sib_decoded;    /* Indicate whether a SIB has been ASN.1 decoded
                          already */
  uint32 seg_count;       /* Total segments in this SIB. Maximum is 16 */
  uint32 seg_index;       /* Index of the latest segment received 
                          Index is 0 for the subsequent segment received
                          first. Maximum is 14 */
  uint32 sib_tot_length;  /* Total length of the encoded SIB in bits.
                          Cannot exeed (MAX_SIB_LENGTH*8) */ 
  byte sib_data_ptr[MAX_SIB_LENGTH_BYTES];   /* To hold the encoded SIB.
                                        Maximum possible length of a encoded
                                        SIB is MAX_SIB_LENGTH_BYTES bytes. */
}temp_sib_holder_type;


/* Enumerated type of the number of SIB related events supported
at a time. Right now, only two events are supported at a time -
one serving cell and one neighbor cell. */
typedef enum
{
  SIB_SRV_INDEX = 0, /* SIB event for the serving cell */
  SIB_NGH_INDEX,     /* SIB event for a neighbor cell  */
  //May need to support more events here - TBD
  MAX_SIB_EVENTS_INDEX
}sib_events_index_e_type;


/* These are data structures to store information about a
BCCH setup */

typedef enum
{
  SIB_BCCH_NO_ACTION,       /* No action related to BCCH setup/teardown */
  SIB_BCCH_SETUP_PENDING,   /* A BCCH setup has been requested and an Ack
                            has not been received */
  SIB_BCCH_RELEASE_PENDING, /* A BCCH Teardown has been requested and
                            an ack has not been received */
  SIB_BCCH_SETUP_COMPLETE,  /* The BCCH setup ack has been received */
  SIB_BCCH_RELEASE_COMPLETE /* The BCCH release ack has been received */
}sib_bcch_action_e_type;

/*These are the valid sib segment types*/
typedef enum
{
  noSegment_chosen,
  firstSegment_chosen,
  subsequentSegment_chosen,
  lastSegmentShort_chosen,
  lastAndFirst_chosen,
  lastAndComplete_chosen,
  lastAndCompleteAndFirst_chosen,
  completeSIB_List_chosen,
  completeAndFirst_chosen,
  completeSIB_chosen,
  lastSegment_chosen,
  payload_spare5_chosen,
  payload_spare4_chosen,
  payload_spare3_chosen,
  payload_spare2_chosen,
  payload_spare1_chosen
}sib_segment_e_type;

typedef struct
{
  sib_bcch_action_e_type bcch_action; /* Indicates the current action
                       related to BCCH setup/teardown for this event */
  uint32 scr_code; /* Scrambling code of the cell for this BCCH  */
  uint32 freq;     /* Frequency of the cell for this BCCH        */
}sib_active_bcch_setup_type;
  


/* Structure to temporarily hold the MIB and SIB3 till we
find the database index for saving SIBs from a cell. Only after
we decode MIB and SIB3 we can find the index since it's based
on the combination of the PLMN Id and Cell Id. */ 
typedef struct
{
  boolean mib_decoded;
  rrc_MasterInformationBlock *mib;
  boolean sib3_decoded;
  rrc_SysInfoType3 *sib3;
  boolean sb1_decoded;
  boolean sb2_decoded;
  boolean sib1_decoded;
}rrc_mib_sib3_holder_type;

/* Name (Description) of the SIB event - Used to remember the type
of event requested so that the appropriate response can be sent. */
typedef enum
{
  RRCSIB_GET_SPECIFIC_SIBS, /* Get a few specific SIBs for a cell */
  RRCSIB_UPDATE_SIBS        /* Update SIBs that have changed (I.e either the value
                            tag has changed or the sib timer has expired) */
  /* More events may be needed later */
}sib_event_name_e_type;
                    
/* Data structure to store information about SIBs that have
expiration timers */
typedef struct
{
  boolean sib_timer_update_req; /* Indicates if a SIB update based
                                on an expiration timer is required.*/
  boolean sib_ext_proc_update_req; /* Indicates if a SIB update has
                                   been requested by an external
                                   RRC procedure */
  rrc_proc_e_type req_proc;     /* The procedure that requested it */
}rrc_sib_exp_timer_sib_info_type;

/* Enumerated type of internal events. Currently internal
events are used for SIBs with expiration timers */
typedef enum
{
  RRCSIB_INT_SIB7_EVENT,         /* SIB7 expiration           */
  RRCSIB_INT_SIB9_EVENT,         /* SIB9 expiration           */
  RRCSIB_INT_SIB10_EVENT,        /* SIB10 expiration          */
  RRCSIB_INT_MAX_EVENT
}rrc_sib_int_event_e_type;

/* Data structure to hold info for internal SIB events -
i.e. SIB events used by the SIB procedure itself */
typedef struct
{
  rrc_sib_int_event_e_type int_event_type; /* Store type
                                           of int event*/
}rrc_sib_int_event_info_type;

/* This is a data structure that stores all necessary information for
a particular event. Note that this is more than 10Kbytes in size. Each SIB
needs 444 bytes and we need to store RRC_TOT_SIBS number of those. */

typedef struct
{ 

  uint8  transaction_id;       /* Used in associating the channel config confirm with
                                a specific channel config request */
  boolean event_active;         /* Indicates whether this event is active
                                or not. If set to FALSE, information below
                                is irrelevant and invalid. */
  sib_event_name_e_type event_name;  /* Name of this event - GET_ALL_SIBS,
                                GET_SPECIFIC_SIBS e.t.c. */
  rrc_proc_e_type req_proc;     /* Name of the procedure that requested
                                this event - To be used while sending
                                the CNF command. */
  boolean for_serving_cell;     /* Set to true if a GET_SPECIFIC_SIBS command
                                is for the serving cell. This var is not
                                used when the command is not GET_SPECIFIC_SIBS. */
  boolean sib_db_index_found;   /* Indicates whether an index in the SIB
                                database has been found for this event 
                                or not. */
  boolean mib_pos_validated;    /* Indicates whether the MIBs position has
                                been validated or not for a cell. The MIB
                                must be in a position where (SFN mod 32 == 0)
                                otherwise the cell is barred. */
#ifdef FEATURE_FEMTO_CSG
  boolean for_csg;               /* set to true for CSG specific search*/

  boolean is_csg;               /* Is current cell CSG?*/

  rrc_int_csg_id_type csg_id;       /* CSG ID of the cell */
#endif
  uint32 sib_db_index;          /* The index in the SIB database for this
                                event. */
  boolean is_mib_vtag_match;    /* Set to 1 if mib vtag matches */

  rrc_mib_sib3_holder_type tempholder; /* Hold the MIB and SIB3 temporarily
                                       till a SIB database index is found
                                       for this cell */
  temp_sib_holder_type tsib[ RRC_TOT_SIBS ];   /* To hold all encoded 
                                               SIBs */
  rrc_sib_int_event_info_type int_event; /* Hold the type of int
                                 event incase this is an int event */

  uint8 sched_present_for_sibs[RRC_TOT_SIBS];  //contains info if SIB/SB scheduled or not

  rrc_sib_scan_e_type sib_scan_type;           //contains info about scan type

}sib_active_event_type;


/* This is a central database for all active events. */

typedef struct
{
  /* An array of active SIB events */
  sib_active_event_type event[ MAX_SIB_EVENTS_INDEX ];
  /* Serving Cell BCCH info        */
  sib_active_bcch_setup_type srv_bcch;
  /* Neighbor Cell BCCH info        */
  sib_active_bcch_setup_type ngh_bcch;
  uint32 mib_value_tag_from_command; /* Only used for RRC_UPDATE_SIBS_IND
  command. Holds the value tag for the new MIB as given in the
  BCCH modification info. */
  rrc_proc_e_type update_sibs_req_proc; /* Only used for RRC_UPDATE_SIBS_IND
  command. Holds the requesting procedure name. */
  rrcsib_update_sibs_cause_e_type update_sibs_cause;/* Only used for RRC_UPDATE_SIBS_IND
    command. Holds the cause for RRC_UPDATE_SIBS_IND. */

  sib_scheduling_info sib_sched[RRC_TOT_SIBS]; //contains scheduling information

  uint16 scheduling_max_rept_rate;           //max rept rate for any sib 
                                             //during a particular sib event
  boolean scheduling_updated_by_sb1;         //bool to figure out if SB1 has already update sib-schedule
  boolean scheduling_updated_by_sb2;         //bool to figure out if SB2 has already updated sib-schedule
  boolean update_sibs_real_sib_mod;

}sib_active_event_database_type;

/* Enumerated type indicating the status of the SIB collection function */
typedef enum
{
  NO_SIBS_COMPLETED, /* None of the SIBs were completed this time */
  SIBS_COMPLETED     /* One or more SIBs were completed this time */
}rrc_sib_collect_e_type;              

typedef struct
{
  /* This indicate whether the sib segment index is being used or free,
  TRUE - Indicate its being currently used, 
  FALSE - Indicate its free to be used */
  boolean index_in_use;
  
  /* This indicate SIB type */
  rrc_SIB_type sib_type;
  
  /* Used to hold SFN prime at which this SIB segment was received*/
  uint32 sfn_prime;
  
  /* Used to hold SIB segment index*/
  uint8 seg_index;
  
  /* Used to hold SIB segment data length*/
  uint32 seg_data_length;
  
  /* Used to hold sib segment type*/
  sib_segment_e_type sib_seg_type;
  
  /* Used to hold SIB segment data*/
  byte seg_data_ptr[FIXED_SIB_DATA_LENGTH_BYTES];
}rrc_temp_sib_optimization_holder_type;

  /*tsib_optimization_db used to hold sib segments received in out of order*/
extern rrc_temp_sib_optimization_holder_type tsib_optimization_db[MAX_TEMP_SIB_SEGMENTS_TO_HOLD];

  /*Look up table used to indicate whether a sib segment 
    of a sib tpye is presnt in temp sib optimization db or not*/
extern boolean tsib_opt_db_look_up_table[rrc_MAX_SIB];

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING

/* Deferred SIB DB used to hold value tag info, segments of the deferred SIBs */
typedef struct
{
   uint32  sib11_val_tag;
   uint32  sib11bis_val_tag;
   uint32  sib12_val_tag;
   uint32  sib18_val_tag;
#ifdef FEATURE_WCDMA_TO_LTE
   uint32  sib19_val_tag;
#endif
   
}deferred_sib_value_tag_info;

typedef struct
{
  uint32 seg_index;       /* Index of the latest segment received 
                          Index is 0 for the subsequent segment received
                          first. Maximum is 14 */
  uint32 sib_tot_length;  /* Total length of the encoded SIB in bits.
                          Cannot exeed (MAX_SIB_LENGTH*8) */ 
  byte sib_data_ptr[MAX_SIB_LENGTH_BYTES];   /* To hold the encoded SIB.
                                        Maximum possible length of a encoded
                                        SIB is MAX_SIB_LENGTH_BYTES bytes. */
}deferred_sib_holder_type;

typedef struct
{
  deferred_sib_holder_type sib11;
  deferred_sib_holder_type sib11bis;
  deferred_sib_holder_type sib12;
  deferred_sib_holder_type sib18;
#ifdef FEATURE_WCDMA_TO_LTE
  deferred_sib_holder_type sib19;
#endif
}deferred_sib_db_type;

typedef struct
{
  rrc_temp_sib_optimization_holder_type deferred_sib_seg_db[MAX_DEFERRED_SIB_SEGMENTS_TO_HOLD];
  deferred_sib_db_type dsib;
  deferred_sib_value_tag_info val_tag;
  rrc_plmn_identity_type plmn_id;  /* PLMN Id of this cell - internal format */
  rrc_int_cell_id_type cell_id;    /* Cell Id of this cell - internal format */
  uint32 scr_code;                 /* Scrambling code of this cell */
  uint32 freq;                     /* Frequency of this cell */

}rrc_deferred_sib_info_type;

extern rrc_deferred_sib_info_type deferred_sib_info;
#endif

/* The database is externalized so that the rrcsibcollect
module can use the database. */
extern sib_active_event_database_type *sib_events_ptr;

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/*========================================================================

MACRO  GET_PDU_LENGTH_IN_BYTES

INPUT: PDU length in bits.

  The bits may not be octet alligned, so first check if they
  are octet alligned. If they are, divide the pdu length in bits
  by 8. If they are not octet alligned, divide by 8 and add 1.

==========================================================================*/

#define GET_PDU_LENGTH_IN_BYTES( length ) \
        ( (((length)%8) == 0) ? ( (length)/8 ) : ((length)/8 + 1) )




/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION PROCESS_SYS_INFO_MSG

DESCRIPTION
 
  This function processes a received System Information Message. The
  System Information Message must have already been ASN.1 decoded before
  passing it to this function. Both FACH System Information Messages
  and BCH System Information Messages are processed here.
  
  The function first determines what combination of SIBs or SIB segments
  are included in the message. Then based on the combination, each SIB
  or SIB segment in the message is processed by calling the appropriate
  function.
  
  This function returns a status value which indicates whether any SIB
  has been completely received or not. If a SIB has been completely 
  received, the calling function is responsible to ASN.1 decode the SIB
  and save the decoded SIB in the SIB database.

DEPENDENCIES

  None.

RETURN VALUE

  NO_SIBS_COMPLETED if none of the SIBs were completely received in this
  system information message.
  SIBS_COMPLETED if any SIB was completely received in this message. It's
  not necessary that the entire SIB was received in this message. If the
  last semgment of a SIB was received in this message, SIBS_COMPLETED will
  be returned by this function. 

SIDE EFFECTS

===========================================================================*/
rrc_sib_collect_e_type process_sys_info_msg
(
  //NEED TO MODIFY LATER (FACH msg also needed)
  rrc_SystemInformation_BCH *msg,  /* Pointer to Sys Info Message BCH */

  sib_events_index_e_type index    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
);
/*===========================================================================

FUNCTION rrcsib_clear_tsib_optimization_db

DESCRIPTION
  This function will clear tsib optimization data base.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcsib_clear_tsib_optimization_db
(
  void
);
/*===========================================================================

FUNCTION rrcsib_clear_a_sib_from_tsib_optim_db

DESCRIPTION
  This function will clear any segment(if its stored) of 
  a sib type from tsib optimization data base.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcsib_clear_a_sib_from_tsib_optim_db
(
  rrc_SIB_type sib_type
);
/*===========================================================================

FUNCTION rrcsib_find_free_index_in_tsib_optim_db

DESCRIPTION
  This function will return first free index in tsib optimization db.
  or the index which already has this sib segment

DEPENDENCIES
  None.

RETURN VALUE
  Free index into tsib optimization db.
  else MAX_TEMP_SIB_SEGMENTS_TO_HOLD i.e.
  non of the index are free.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 rrcsib_find_free_index_in_tsib_optim_db
(
  rrc_SIB_type sib_type,  /*This will indicate type of SIB,e.g MIB,SIB1...etc*/
  uint8 seg_index,  /*SIB segment index*/
  sib_segment_e_type sib_seg_type/*Used to indicate type of sib segment*/
);
/*===========================================================================

FUNCTION rrcsib_store_sib_seg_in_tsib_optim_db

DESCRIPTION
  This function will store received sib segment in tsib
  optimization db only if a index in tsib optimization db
  is free.
  
DEPENDENCIES
  None.

RETURN VALUE
  SEG_PROC_STORED = if sib segment is stored in tsib opt db
  SEG_PROC_FAILED  =  if sib segment could not be stored in tsib opt db
  
SIDE EFFECTS
  None.
===========================================================================*/
seg_proc_stat_e_type rrcsib_store_sib_seg_in_tsib_optim_db
(
  rrc_SIB_type sib_type,  /*This will indicate type of SIB,e.g MIB,SIB1...etc*/
  uint32 sfn_prime,  /*SFN prime at which this sib segment received*/
  uint8 seg_index,  /*SIB segment index*/
  sib_segment_e_type sib_seg_type,/*Used to indicate type of sib segment*/
  uint32 seg_data_length,  /*SIB segment data lenght*/
  void *seg_data_ptr,  /*pointer to sib segment data*/
  sib_events_index_e_type event_index    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
);
/*===========================================================================

FUNCTION rrcsib_is_sib_seg_in_tsib_optim_db

DESCRIPTION
  This function will say whether a sib segment 
  of a sib type is stored in tsib optimization db
  by looking at look up table..
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE : if a sib segment is stored in tsib optimization db
  FASLE: otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcsib_is_sib_seg_in_tsib_optim_db
(
  rrc_SIB_type sib_type
);
/*===========================================================================

FUNCTION rrcsib_process_sib_seg_in_tsib_optim_db

DESCRIPTION
  This function will process stored sib sements 
  in tsib optimization db if any by looking at
  look up table..
  
DEPENDENCIES
  None.

RETURN VALUE
  If a complete sib is constructed at the end of this function then returns = SEG_PROC_COMPLETE.
  else
  Return SEG_PROC_NONE 

SIDE EFFECTS
  None.
===========================================================================*/
seg_proc_stat_e_type rrcsib_process_sib_seg_in_tsib_optim_db
(
  rrc_SIB_type sib_type,   /*Sib type*/
  sib_events_index_e_type index    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
);

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*===========================================================================
FUNCTION rrcsib_can_event_support_deferred_sib

DESCRIPTION

  This function checks whether the current event can support Deferred SIB reading. s
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: True, if event can support Deferred SIB reading

SIDE EFFECTS
  None.

===========================================================================*/

boolean rrcsib_can_event_support_deferred_sib
(
  sib_events_index_e_type index
);
/*===========================================================================

FUNCTION  rrcsib_update_sib_for_deferred_sib

DESCRIPTION

  This function will post Update SIBS comand for deferred SIB reading.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_update_sib_for_deferred_sib
(
  void
);

/*===========================================================================

FUNCTION  rrcsib_post_process_deferred_sib

DESCRIPTION

  This function will perform all the required processing needed once SIBs have been read and the SIB event is to be 
  completed. 

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.

     
===========================================================================*/

void rrcsib_post_process_deferred_sib(sib_events_index_e_type index,   uint32 dbindex);


/*===========================================================================

FUNCTION rrcsib_is_event_deferred_meas_read

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  due to deferred SIB reading.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If this event is UPDATE_SIBs due to deferred SIB reading.
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/

boolean rrcsib_is_event_update_sibs_for_deferred_sib
(  
  sib_events_index_e_type  event_index
);
/*===========================================================================

FUNCTION  rrcsib_is_sib12_present_in_serving_cell

DESCRIPTION

  This function will check if SIB12 is present for the serving cell.

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/

boolean rrcsib_check_if_sib12_present_for_dbindex
(
  int32 dbindex
);

/*===========================================================================

FUNCTION rrcsib_store_sib_seg_in_deferred_sib_db

DESCRIPTION
  This function will clear all SIBs in tsib optimization data base except SIB11/11bis/12.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcsib_store_sib_seg_in_deferred_sib_db
(
  void
);

#endif
/*===========================================================================
FUNCTION rrcsib_map_asn1_sibtype_to_int_sibtype

DESCRIPTION

  This function maps the value used for an ASN1 sib type to the corresponding internal sib type.
  
DEPENDENCIES
  None

RETURN VALUE
  Value used for internal SIB type

SIDE EFFECTS
  None.

===========================================================================*/

rrc_SIB_type rrcsib_map_asn1_sibtype_to_int_sibtype
(
  rrc_SIB_Type asn1_sib_type
);
/*===========================================================================
FUNCTION rrcsib_map_asn1_sibtype_to_int_sibtype

DESCRIPTION

  This function maps the value used for an ASN1 sib type to the corresponding internal sib type.
  
DEPENDENCIES
  None

RETURN VALUE
  Value used for internal SIB type

SIDE EFFECTS
  None.

===========================================================================*/

rrc_SIB_Type rrcsib_map_int_sibtype_to_asn1_sibtype
(
  rrc_SIB_type int_sib_type
);

/*===========================================================================

FUNCTION rrcsib_map_extension_sib_to_internal_sibtype

DESCRIPTION
  This function will return the SIB type in internal format for extension SIBs. It will find out the SIB type based on
  the scheduling info and SFN at which the SIB arrives.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the SIB type based on the SFN at which it arrives. 
  
SIDE EFFECTS
  None.
===========================================================================*/
rrc_SIB_type rrcsib_map_extension_sib_to_internal_sibtype
(
  rrc_SegmentIndex seg_index,
  uint32 sfn_prime
);

/*===========================================================================
FUNCTION    rrc_log_complete_sib 

DESCRIPTION
  This function will log rrc_System_Information_Block_PDU message into QXDM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrc_log_complete_sib
(
  rrc_SIB_Type  sib_Type,
  rrc_SegmentIndex seg_index,
  uint32 sfn_prime
);
/*===========================================================================

FUNCTION rrcsib_update_sibs_active_for_bcch_mod

DESCRIPTION

  This function checks whether update sibs event if active for bcch mod
  
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE if active, otherwise FALSE.

SIDE EFFECTS

  None.
     
===========================================================================*/


boolean rrcsib_update_sibs_active_for_bcch_mod(void);
#endif /* RRCSIBPROCI_H */

