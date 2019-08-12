#ifndef HDRLMACI_H
#define HDRLMACI_H
/*===========================================================================

  L I N K   M A N A G E M E N T  A N D   C O N T R O L   P R O T O C O L
                 I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for the LMAC Protocol.

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrlmaci.h_v   1.12   16 Jan 2003 11:11:12   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrlmaci.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/16   sjj     FR26262 : LTE-U band support
06/09/14   kss     Added support for handling TX_RESET_REQ_IND processing. 
05/05/14   cnx     Update NSET and IDLE channel change in one command.
10/30/13  kss/rmv  FR3272 - Modem API for Cost of Power estimate changes 
02/26/13   mbs     Fixed featurization for IRAT code
08/05/11   eme     Changes to support free floating threads.
04/26/11   sju     Added field hdr_rev in tc_assign_params_type.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
08/15/07   sju     Added support for revB TCA message 
04/27/07   grl     Removed tc_mode from the traffic assignment cmd.
12/16/05   mpa     Added channel_change_in_sleep flag to idle chan change cmd.
09/10/05   grl     Added traffic mode to hdrlmac_tc_assign_params_type
05/16/05   dna     Add support for Rel. A TCA message fields
11/29/04   dna     Added HDRLMAC_FINISH_AHO_CMD to fix AEHO and aho_count
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
01/15/03   mpa     Removed hdrsrch.h dependency
05/06/02   dna     Whether or not to tune away to 1x during HDR connection
05/08/01   dna     Re-naming for consistency.
09/08/00   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrsrchtypes.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrmac.h"
#include "hdrind.h"

#include "hdrrmac.h"
#include "hdrfmac.h"

#include "hdrsrchcmd.h"
#include "hdrrup.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                               LMAC COMMANDS
--------------------------------------------------------------------------*/

/********** Command types **********/

/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRLMAC_ACTIVATE_CMD=0,          /* Activates the LMAC protocol */
  HDRLMAC_DEACTIVATE_CMD=1,        /* Deactivates the LMAC protocol */
  HDRLMAC_SET_PARAMETERS_CMD=2,    /* give operating parameters to HDRSRCH */
  HDRLMAC_SET_CC_HASH_CMD=3,       /* tell HDRSRCH what our slot is */
  HDRLMAC_ACQUIRE_CMD=4,           /* attempt to acquire a HDR channel and 
                                    * give an indication when done */ 
  HDRLMAC_SYSTEM_TIME_SYNC_CMD=5,  /* give SYNC message info to HDRSRCH */
  HDRLMAC_IDLE_MODE_CMD=6,         /* command HDRSRCH to idle mode */
  HDRLMAC_NEIGHBOR_LIST_UPDATE_CMD=7, /* give neighbor list to HDRSRCH  */
  HDRLMAC_IDLE_CHAN_CHANGE_CMD=8,  /* change HDR channels and give an 
                                    * indication when done */ 
  HDRLMAC_SLEEP_CMD=9,             /* OK to power off the RF hardware */
  HDRLMAC_ACCESS_MODE_CMD=10,      /* command HDRSRCH to access mode */
  HDRLMAC_PERFORM_AHO_CMD=11,      /* change to strongest pilot and give an
                                    * indication when done */ 
  HDRLMAC_PILOT_REPORT_RESET_CMD=12, /* reset HDRSRCH mechanism for flagging 
                                      * pilots that have been reported */
  HDRLMAC_TC_ASSIGN_CMD=13,          /* change to given active set and freq */
  HDRLMAC_CHANNEL_MEASUREMENT_CMD=14,/* measure the given set of frequencies
                                      * and report results when done */

  HDRLMAC_SET_SRCH_PARAMETERS_CMD=15,/* give operating params to HDRSRCH */
  HDRLMAC_SET_MGMT_SAME_CHAN_CMD=16, /* give operating parameters to HDRSRCH */
  HDRLMAC_SET_MGMT_DIFF_CHAN_CMD=17, /* give operating parameters to HDRSRCH */
  HDRLMAC_GET_CC_HASH_CMD=18,        /* ask HDRSRCH what our slot should be */
  HDRLMAC_FINISH_AHO_CMD=19,        /* change to strongest pilot and give an
                                    * indication when done, this command is 
                                    * queued instead of HDRLMAC_PERFORM_AHO_CMD
                                    * if ACMAC is active when trying to process
                                    * access handoff
                                    */ 
  HDRLMAC_FAST_ACQUISITION=22,      /*Route from tx queue to srch queue*/
  HDRLMAC_UPDATE_BC_INFO_CMD=23,    /*Route from tx queue to srch queue*/
  HDRLMAC_ACMAC_TA_TIME_OK_CMD=24,  /*Route from tx queue to srch queue*/
  HDRLMAC_ACMAC_DISABLE_TA_CMD=25,  /*Route from tx queue to srch queue*/
  HDRLMAC_ACMAC_READY_CMD=26,       /*Route from tx queue to srch queue*/
  HDRLMAC_TC_ACK_RECEIVED_CMD=27,   /*Route from tx queue to srch queue*/
  HDRLMAC_ADVISE_UNLOCK_RF_CMD=28,  /*Route from tx queue to srch queue*/
  HDRLMAC_TC_MODE_CMD=29,           /*Route from tx queue to srch queue*/
  HDRLMAC_TRAN_SYS_TIME_REQ=30,     /*Route from tx queue to srch queue*/
  HDRLMAC_PILOT_MEAS_CMD=31,        /*Route from tx queue to srch queue*/
  HDRLMAC_LTE_NSET_UPDATE_CMD=32,   /*Route from tx queue to srch queue*/
#ifdef FEATURE_HDR_MODEM_API
  HDRLMAC_MODEM_API_TIMEOUT_CMD=33, /* Timeout for Modem API updates */
#endif /* FEATURE_HDR_MODEM_API */
  HDRLMAC_TRIGGER_SYSTEM_LOST_CMD=34,  /* Timeout for Modem API updates */
  HDRLMAC_IDLE_CHAN_CHG_AND_NSET_UPDATE_CMD=35  
                                    /* Update nset and change HDR channels */
} hdrlmac_cmd_name_enum_type;

/* HDRLMAC_IDLE_CHAN_CHANGE_CMD */
typedef struct
{
  sys_link_type link;
  boolean channel_change_in_sleep;  
} hdrlmac_idle_chan_change_params_type;

/* HDRLMAC_SYSTEM_TIME_SYNC_CMD */
typedef struct
{
  qword                           sync_time;      
  sys_pilot_pn_type               pilot_pn;         /* Pilot SYNC msg came from */
  uint16                          half_slot;        /* HSTR value @ start of SCC */
  hdrsrch_link_id_type            link_id;          
} hdrlmac_synchronize_params_type;

/* HDRLMAC_NSET_UPDATE_CMD */
typedef struct
{
  hdrsrch_nset_pilot_type           nset_pilots[HDRSRCH_NSET_MAX_SIZE];
  uint8                             num_pilots;
  hdrsrch_nset_update_enum_type     update_type;
  sys_link_type                     cached_sects[HDRSRCH_MAX_CACHE_RECORDS];
  uint8                             num_sects;
  sys_channel_type                  channel_list[HDRSRCH_HASH_CHAN_MAX_SIZE];
    /* Hash channel list from Sector Parameters message*/
  uint8                             num_channels;
    /* Number of hash channels */
  uint8                             num_avoided_channels;
    /* Number of avoided channels */
  sys_avoided_channel_record_type   avoided_list[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];
#ifdef FEATURE_HDR_REVB
  hdrsrch_pilot_gid_type            serving_pilot_gid;
    /* The group id of the pilot from which the neighbor info is received */
  hdrsrch_rup_subtype_type          rup_subtype;
    /* RUP subtype */
#endif /* FEATURE_HDR_REVB */
} hdrlmac_nset_update_params_type;

/* HDRLMAC_IDLE_CHANNEL_CHANGE_AND_NSET_UPDATE_CMD */
typedef struct
{
  sys_link_type                     link;
  boolean                           channel_change_in_sleep; 
  hdrsrch_nset_pilot_type           nset_pilots[HDRSRCH_NSET_MAX_SIZE];
  uint8                             num_pilots;
  hdrsrch_nset_update_enum_type     update_type;
  sys_link_type                     cached_sects[HDRSRCH_MAX_CACHE_RECORDS];
  uint8                             num_sects;
  sys_channel_type                  channel_list[HDRSRCH_HASH_CHAN_MAX_SIZE];
    /* Hash channel list from Sector Parameters message*/
  uint8                             num_channels;
    /* Number of hash channels */
  uint8                             num_avoided_channels;
    /* Number of avoided channels */
  sys_avoided_channel_record_type   avoided_list[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];
#ifdef FEATURE_HDR_REVB
  hdrsrch_pilot_gid_type            serving_pilot_gid;
    /* The group id of the pilot from which the neighbor info is received */
  hdrsrch_rup_subtype_type          rup_subtype;
    /* RUP subtype */
#endif /* FEATURE_HDR_REVB */
} hdrlmac_idle_chan_chg_and_nset_update_params_type;

#ifdef FEATURE_HDR_TO_LTE
typedef struct
{
  hdrsrch_lte_nset_update_params_type lte_nset_update_params;
#if (LTE_BAND_NUM == 256)
  sys_lte_band_mask_e_type lte_band_pref; 
#else
  sys_band_mask_type lte_band_pref; 
#endif /* (LTE_BAND_NUM == 256) */
} hdrlmac_lte_nset_update_params_type;
#endif /* FEATURE_HDR_TO_LTE */
    
typedef enum
{
  HDRLMAC_QUEUED,                  /* Command is queued for processing */
  HDRLMAC_WAITING,                 /* Command is waiting for some link
                                    * change before it can be processed */
  HDRLMAC_READY                    /* Command is ready to be processed */
} hdrlmac_cmd_status_enum_type;

/* HDRLMAC_TC_ASSIGN_CMD */
typedef struct
{
  uint8 num_carriers;
  hdrrmac_rl_carrier_info_type rl_carrier[HDRSRCH_MAX_NUM_CARRIERS];
  uint8 frame_offset;
  uint8 dsc_gain_base;
  uint8 num_subasets;
  hdrrmac_subaset_info_type subaset_info[HDRSRCH_MAX_NUM_SUBASETS];
} hdrlmac_rmac_params_type;

typedef struct
{
  hdrlmac_cmd_status_enum_type status;
  hdrsrch_link_id_type link_id;
  hdrsrch_traffic_tune_away_enum_type  tc_tune_away;
  uint16 rup_subtype;
  uint8 num_sub_asets;   
  uint8 cc_subactive_set_idx;
  hdrsrch_subactive_set_type srch_subactive_set[HDRSRCH_MAX_NUM_SUBASETS]; 
  hdrlmac_rmac_params_type rmac_params;      
  hdrfmac_tcam_params_type fmac_tca_params;
  boolean initial_assignment;
  hdrrup_tca_msg_type tca_msg;  
  sys_active_prot_e_type hdr_rev;
} hdrlmac_tc_assign_params_type;

/********** Common command fields **********/

typedef union
{
  hdrlmac_idle_chan_change_params_type idle_chan_change;
  hdrlmac_tc_assign_params_type tc_assign;
  hdrsrch_fast_acq_params_type fast_acq_cmd;
  hdrsrch_acquire_params_type acquire;
  hdrlmac_synchronize_params_type synchronize;
  boolean update_bc_info_bcmcs_status; 
  boolean ta_time_ok; 
  uint16 no_ta_duration; 
  hdrsrch_sleep_params_type sleep; 
  hdrsrch_tc_mode_params_type tc_mode;
#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
  hdrsrch_tran_sys_time_params_type tran_sys_time_req;
  hdrsrch_pilot_meas_req_params_type pilot_meas_req; 
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */
#ifdef FEATURE_HDR_TO_LTE
  hdrlmac_lte_nset_update_params_type lte_nset_update;
#endif /*FEATURE_HDR_TO_LTE*/
  hdrlmac_nset_update_params_type nset_update;
  hdrlmac_idle_chan_chg_and_nset_update_params_type idle_chan_chg_and_nset_update;
} hdrlmac_cmd_params_union_type;

/********** Common command structure **********/

typedef struct
{
  hdrlmac_cmd_name_enum_type name;   /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving cmd, for debug */
  hdrlmac_cmd_params_union_type params; /* Common command fields */
} hdrlmac_cmd_type;


/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRLMAC_PROCESS_CMD

DESCRIPTION
  This function processes commands given to LMAC.

DEPENDENCIES
  None

PARAMETERS
  lmac_cmd_ptr - Pointer to the command for LMAC to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlmac_process_cmd (hdrlmac_cmd_type * lmac_cmd_ptr);

/*===========================================================================

FUNCTION HDRLMAC_PROCESS_IND

DESCRIPTION
  This function processes indications given to LMAC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for LMAC to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlmac_process_ind 
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

#endif /* HDRLMACI_H */
