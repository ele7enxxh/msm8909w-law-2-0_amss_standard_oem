
/*===========================================================================
            R R C   M E A S U R E M E N T S

DESCRIPTION

   This module supports Inter Freq Measurement Control and Measurement Reporting
   for the RRC layer. At present, it supports the following features
   1. Sends one shot A-GPS Meas Report to UTRAN in connected mode of UE
   2. Supports Measurement Control Message received from UTRAN



EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

   RRC Task must have been started. All RRC command queues must
   have been initialized. All RRC signals must have been cleared
   at task startup.

Copyright (c) 2003-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

             EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcgpsmeas.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/22/15   sp    Made changes to report cell-id to QSH whenever GPS cell ID request is honored.
03/19/15   vi      Made changes to correctly check only GPS capability when UE support GANSS and MCM is GPS+GANSS
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/04/14   vi      Made changes to avoid compiler warnings
05/16/14   vi    Made changes to support MOB framework
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/24/14   vi     Modified rrc_mulitpath_enum_type enum variables
01/07/14   rmn     Made changes to provide current camped band info in 
                   CM_RRC_DCH_CELL_INFO_CNF & CM_RRC_CELL_INFO_IND
12/26/13   vi    Moved GPS_CELL_ID changes out of feature FEATURE_CGPS_UMTS_WCDMA
12/09/13   sp    Made changes for OSYS 6.6.5 migration
12/03/13   mn      Moved all declarations to top of the code.
12/03/13  vi    Made changes to check for velocity_estimate present before validating it.
11/19/13   vi    Made changes to correctly set measurement id for gps in case of rel-9 MCM
09/16/13   mp    Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
09/11/13   vi    Added handling of GPS_REFERENCE_TIME_ONLY in ganss measured results
08/28/13   vi    Added NV check for GANSS feature
08/21/13   gv    Made changes to return back existing cell id for a GPS request if the cell id 
                 is valid. Also to fail the GPS request when waiting for channel config confirm
07/16/13   vi    Made changes to read GPS-cell id on HS serving cell if HS is active
07/12/13   vi    Fixed compilation error.
06/23/13   sr    Changes to fix stack overflow with GANSS strcutures
06/22/13   sr    Changes to fix compilation errors
06/20/13   sr    Adding AGNSS RRC R8 changes without feature flag in API
06/20/13   vi    Corrected check for num_of_satellites. 
06/07/13   vi    Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
06/05/13   vi    Initialized the GPS_MEAS structure before sending to GPS
05/28/13   sr    Added code for measurement identity extension
05/13/13   vi    Added code for GANSS under feature FEATURE_GANSS_SUPPORT
04/25/13   sg    Added code for REL9 under FEATURE_WCDMA_REL9
04/18/13   sr    Made changes to update CM with PLMN ID
01/03/13   ad    Made changes to reduce the redundant F3s
10/14/12   gv    Added code to support exchange of AS ID between WRRC and GRR commands
07/24/12   geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/25/12   sr    Made changes to abort CELL_ID_IN_DCH when RRC Connection Release procedure is active
06/07/12   sn    Change to send correct LAC info in idle mode 
03/21/12   sn    Changes to include LAC info when providing cell info to GPS in DCH state
03/02/12   mp    Added code to send cell-info in idle mode if requested by CM
12/05/11   pm    Added code to support cell id request by CM in DCH state"
07/18/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/05/11   vg    added code to fix r8 gps MCM handling issue. 
06/30/11   rl    Added protection for gps function callback pointer check.
05/03/11   rl    Fixed the lint error
04/26/11   rl    Support for transition from TMC heap to Modem Heap in WCDMA.
04/25/11   rl    Fixed the KW error
04/13/11   vg    fixed null pointer access issue which was causing a crash
01/28/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad    Added changes for RRC SW decoupling
01/04/11   rl    Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/20/10   sks   Made changes to send MCM failure to n/w if MCM message has TDD info in "rrc_UE_
                 Positioning_GPS_AcquisitionAssistance" and "rrc_UE_Positioning_GPS_ReferenceTime".
12/31/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl    Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/12/10   sks   For assistance data delievery_msg, added code to process GPS assistance data
                 only if present.
09/28/10   rm    Added code to process GPS assistance data only if present in MCM
11/03/10   rl    Fixed compiler warnings
10/30/10   rl    Objective systems migration changes check in 
                 on code base VU_MODEM_WCDMA_RRC.01.80.00
09/02/10   rm    Fixed lint warnings
08/24/10   rm    Added code to report GPS reference time uncertainity in GPS
                 measured results
05/06/10   rm    Fixed compiler warnings
10/07/09   gkg   Changes for REL8 under FEATURE_WCDMA_REL8
08/27/09   sks   Fixed lint warnings.
08/21/09   rm    Fixed Lint Warnings
08/13/09   rm    Fixed compilation warings
06/04/09   kp    Fixed compiler warnings.
05/08/09   ss    Updated Copyright Information
01/28/09   dm    Fixed lint errors
12/08/08   kp    Fixed Lint error in modem off target build. 
09/12/08   rm    Added code for REl7 under FEATURE_WCDMA_REL7.Changes for ASN1 
                 migration to march 08 version
09/05/08   rm    Moved code to check if UE supports timing of cell, under feature flag
09/05/08   rm    Made changes to correctly update the horizontal accuracy from OTA
06/09/08   rm    Added a Validation check for Maximum Fractional GPS Chips in GPS measurement
                 Report
03/06/08   rm    Made changes to check the call back function for NULL and send MCM failure
                 with cause unsupported measurement if the call back function is NULL
02/06/08   rm    Made changes to rename the varibles of rrc_rpt_quan_struct_type
02/04/08   rm    Made Changes to send MCF with failure cause Incomplete Configuration
                 when "GPS-Timing Of Cell Wanted" is requested in the MCM
02/01/08   ps    Made changes to store rrc_gps_psc_stored as the primary scrambling
                 code value
01/31/08   ps    Made changes to convert the psc to the primary scrambling code
                 before passing it to GPS for the feature
                 FEATURE_WCDMA_GPS_CELL_ID_READING and also to pass the converted 
                 psc to ASU
09/07/07   vm    Added support for feature FEATURE_RRC_CALLOC
08/31/07   ps    Moved the RRC GPS procedure code from rrcgps.c to rrcgpsmes.c 
                 Changes are made under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
05/25/07   vg    added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
09/11/06   sgk   Added rel6 measurement support and code optimization across
                 releases. 
08/29/06   sm/da Made changes required for R6 ASN.1 migration.  Also merged
                 FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/05/06   sgk   Made changes to include rel5 non HSDPA functionality under 
                 FEATURE REL5 while retaining the HSDPA specifc processing 
                 under FEATURE HSDPA. 
05/18/06   sgk   Fixed lint errors
05/16/06   sgk   Fixed lint errors.
05/11/06   sgk   Added code for lsm PCH reporting.
05/08/06   sgk   Removed error check iode > 255.  
04/18/06   da    Modified code to support migration to Mar-06 Rel-5 ASN1 +
                 R6 Hacks
03/10/06   sgk   Added code to allow reporting amount > 1 in the position MCM.
02/28/06  da/sm  Modified code to support migration to Dec-05 Rel-6 ASN1
10/21/05   sgk   Added rel5 position measurement support. 
10/04/05   vk    If network tries to change the positioning method to Cell Id
                 or OTDOA through Modify, then reject the Measurement Control Message
09/14/05   vk    Changed the value of MAX_DOPPLER and MIN_DOPPLER
07/13/05   sgk   Added rel5 non critical extensions for rel 99 MCM ie the 
                 transaction identifier extension. 
06/06/05   vk    Use RRC_RELEASE_chosen so that both Rel 99 and Rel 5 ASN1 can
                 be used
06/01/05   vk    If network changes the measurement validity of a measurement
                 by sending a modify, then indicate to LSM to do measurements
                 only if the measurement validity is in agreement with the
                 current RRC state.
04/05/05   vk    Declared global structure gps_l2_ack_params to store L2 ack
                 for A-GPS params. Defined fn rrcgps_initialize_l2_ack_params
                 to initialize L2 ack params under FEATURE_AAGPS_MR_RRC_NOTIFICATION_REQUIRED
03/15/05   sgk   Added measurement optimization to restrict max pending ack
                 mode measurement rpts to 12.
03/02/05   ttl   Support RRC ASN1 Release 5.
01/09/05   sbs   Modified rrcgps_verify_positioning_capabilities() to reflect
                 current accept/reject criteria.
12/22/04   vn    Removed un-necessary comparisons for uint32 limits.
12/08/04   sgk   Fixed compiler warnings.
11/16/04   vk    Use fn lsm_wcdma_ue_pos_capability_get() instead of lsm_ue_pos_capability_get()
                 since the original function has now been split into 2 fns[One for GSM
                 and one for WCDMA]
10/19/04   vk    Initialized contents of Report pointer to 0 so that in case LSM
                 does not specify that Almanac is not required, then network
                 is indicated to not send ALMANAC info in next MCM 
10/15/04   vk    Set the new primitive that indicates if Measurements need to be 
                 setup based on the measurement validity received and the current
                 RRC state
10/11/04   sgk   Typecast NETWORK_BASED and RRC_UE_ASSISTED to uint8 and removed
                 the else if check *pos_method_type_ptr == NONE in function
                 rrcgps_verify_positioning_capability.
                 Typeast RRC_STATE_CHANGE_PARMS, RRC_MEAS_DELETE, RRC_MEAS_RESUME,
                 RRC_MEAS_save, to uint8 in function rrcgps_process_dch_to_fach and
                 rrcgps_process_fach_to_dch.
09/28/04   sgk   Added check for measurement identity sent by lsm if < 1 or >16
                 print error msg and return in function rrcgps_send_meas_report.
09/02/04   vk    Added code to support DCH<->FACH transitions for AGPS measurements
08/19/04   sgk   Changed numerical constant '2322431999999' to 4294967295 to fix
                 lint error 'larger than unsigned long' in function 
                 rrcgps_validate_pos_estimate 
                 Checked return value of 'rrcmeas_config_pos_db' and if FALSE,
                 returned FAILURE to fix lint error Ignoring return value of 
                 function 'rrcmeas_config_pos_db 
                 Replaced the MACROs POS_RPT_CRIT and RET_POS_ERR by switch
                 statements, to solve lint warning enum to int conversion 
08/02/04   vk    Initialized gps_assistance_data_incl to FALSE while processing MCM
06/16/04   vk    Indicate periodic reporting interval to LSM as they now support it
04/06/04   vk    Cleared LINT warnings
02/02/04   vk    Initial release that contains A-GPS support under FEATURE_MMGPS

===========================================================================*/



/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "customer.h"
#include "uecomdef.h"
#include "rrcsend.h"
#include "rrcscmgr.h"
#include "comdef.h"
#include "rrcsibdb.h"
#include "msg.h"
#include "rrccmd_v.h"
#include "rrclcm.h"
#include "rrcasn1Def.h"

#include "rrcmeas.h"
#include "rrcmeasi.h"
#include "rrclsmif.h"
#include "rrcmcm.h"
#include "l1rrcif.h"
#include "rrcmeasi.h"
#include "rrccspi.h"
#include "rrcsibproc.h"
#include "rrcllc.h"
#include "rrcdata_v.h"
#include "rrcllcoc.h"
#include "rrcscmgr.h"
#include "rrccu.h"
#include "rrccui.h"
#include "bit.h"
#include "rrcasn1util.h"
#include "rrcmmif.h"
#include "rrcrcr.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif


#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)

/* UMTS_CP_WCDMA call back function pointers */

/*lint -e551 */
static rrcgps_cgps_event_cb_type         rrcgps_cgps_event_cb = NULL;
/*lint +e551 */
rrcgps_cgps_ue_pos_capability_cb_type    rrcgps_cgps_ue_pos_capability_cb = NULL;
 
/* Boolean that indicates measurement id extension is supported or not */
extern boolean rrc_meas_id_extn_support;



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define MAX_SFN 4095

#define GPS_MSB_TIMING 16383

#define PSC_MAX 511

#define MAX_SAT_ID 63

#define MAX_DOPPLER 32768

#define MIN_DOPPLER -32768

#define MULTIPATH_VALUE 3

#define MAX_GPS_CHIPS 1022

#define MAX_FRAC_GPS_CHIPS 1023

#define MAX_RMS_ERROR 63

#define MAX_C_NO 63

#define MAX_LATITUDE 8388607

#define MIN_LONGITUDE -8388608

#define MAX_LONGITUDE 8388608

#define MAX_LATITUDE_SIGN 1

#define MAX_ALTITUDE 32767

#define MAX_ALTITUDE_DIRECTION 1

#define MAX_CONFIDENCE 100

#define UNCERTAINITY_SEMI_MINOR 127

#define UNCERTAINITY_SEMI_MAJOR 127

#define ORIENTATION_MAJOR_AXIS 89

#define MAX_GPS_WEEK 1023

#define GPS_AGE_OF_TOLERANCE 10

#define MAX_IODE 255

#define MAX_GPS_TOW 167

#define MAX_UNCERTAINITY_ALTITUDE 127

#ifdef FEATURE_GANSS_SUPPORT
#define MAX_BEARING 359

#define MAX_HORIZONTALSPEED 2047

#define MAX_SPEED 255

#define MAX_GANSS_TIME_ID 7

#define MAX_GANSS_TOD 3599999


#define GANSS_POS_METHOD_GPS 0x8000

#define GANSS_POS_METHOD_GLONASS 0x400
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/*-------------------------------------------------------------------
ENUM: rrcmeas_current_substate
Stores substate of RRC Measurement procedure
--------------------------------------------------------------------*/
extern rrcmeas_substate_e_type rrcmeas_current_substate; 

#ifdef FEATURE_GANSS_SUPPORT
boolean rrc_ganss_supported = FALSE;
#endif
#endif /*FEATURE_CGPS_UMTS_CP_WCDMA*/


#define RRC_GPS_INVALID_PSC 0xFFFF
#define RRC_GPS_INVALID_FREQ  0xFFFF

uint32 rrc_gps_psc_stored=RRC_GPS_INVALID_PSC;

cell_info_type cell_id_dch_info;

/* pointer to callback func for cell id change notification */
RRC_GPS_CELL_ID_CNF_CB_FUNC_TYPE *rrc_cell_id_cnf_cb_func;

/* RRC GPS Module substate*/
rrcgps_substates_e_type rrcgps_substate;


uint32 rrc_prev_cell_id_reported;

/*--------------------------------------------------------------------------
       Macro to convert scrambling code to primary scrambling code
--------------------------------------------------------------------------*/
#define SCR_CODE_TO_PRIMARY_SCR_CODE(scr) ((scr) >> 4)

/*===========================================================================

FUNCTION  rrc_gps_send_sib_cmd

DESCRIPTION

  This function fills the GET_SPECIFIC_SIB_CMD to be sent to SIB
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type  
rrc_gps_send_sib_cmd(void)
{
  uint32 i;
  rrc_csp_sib_mask_type  gen_mask = MIB_BIT;
  rrc_cmd_type   *rrcgps_out_cmd_ptr;
  rrc_csp_sib_mask_type    sib_mask = 0;
  uint32 pnpos;
  

  /* Send SIB procedure an event to get SIBs */
  rrcgps_out_cmd_ptr = rrc_get_int_cmd_buf();
  /* Populate the cmd data */
  rrcgps_out_cmd_ptr->cmd_hdr.cmd_id = RRC_GET_SPECIFIC_SIBS_REQ;
  sib_mask = MIB_BIT|SIB_3_BIT|SIB_1_BIT;

  /* Get Frequency and Scrambling Code from CSP Internal Data */
  rrcgps_out_cmd_ptr->cmd.get_specific_sibs_req.freq = rrc_ccm_get_curr_camped_freq();

  (void)l1_get_hs_psc_sib_read((uint16 *)&rrcgps_out_cmd_ptr->cmd.get_specific_sibs_req.scrambling_code, &pnpos);

  rrcgps_out_cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell = TRUE;
  rrcgps_out_cmd_ptr->cmd.get_specific_sibs_req.procedure = RRC_PROCEDURE_GPS;
 
  rrcgps_out_cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type =
      RRC_SIB_CELL_ID_READ_IN_DCH_SCAN; 

  /* Indicate which SIBs are needed */
  for(i = 0;  i < RRC_TOT_SIBS;  i++, gen_mask <<= 1)
  {
    if(sib_mask & gen_mask)
    {
      rrcgps_out_cmd_ptr->cmd.get_specific_sibs_req.sib_req[i] = TRUE;
    }
    else
    {
      rrcgps_out_cmd_ptr->cmd.get_specific_sibs_req.sib_req[i] = FALSE;
    }
  }

  WRRC_MSG0_HIGH("RRCGPS:Send RRC_GET_SPECIFIC_SIBS_REQ to SIB ");
  /* Send the Internal cmd to RRC */
  rrc_put_int_cmd( rrcgps_out_cmd_ptr );
  return(SUCCESS);
}

/*===========================================================================

FUNCTION  rrc_gps_process_cell_id_req

DESCRIPTION

  This function handles the RRC_GPS_CELL_ID_REQUEST from GPS.
   It Validates the RRC state, 
     If RRC is in a State Transition or  OOS or Not Camped , RRC will send a Failure to GPS.

    Else,
    In FACH, PCH, URA PCH and DISCONNECTED state It will send the GPS the Cell ID info  with
     the data present in RRC database.

     In DCH State,  it will request the SIB module to read SIBs to get the Cell ID for present Cell.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrc_gps_process_cell_id_req( void )
{
  rrc_mode_e_type   rrc_mode;
  boolean handle_cell_id_req= TRUE;
  rrcllc_oc_set_status_e_type oc_status;
  rrc_state_e_type rrc_curr_state;
  rrcmeas_plmn_cell_info_struct_type  cell_id_info;
  rrc_CellUpdateCause       cu_cause;
  rrc_csp_substate_e_type rrc_csp_sstate = rrc_get_csp_substate();

  rrc_mode = rrcmcm_get_rrc_mode();
  oc_status = rrcllc_get_ordered_config_status();
  rrc_curr_state = rrc_get_state();

  memset(&cell_id_info,0x00, sizeof(rrcmeas_plmn_cell_info_struct_type));
  
  cell_id_info.cell_id = INVALID_CELL_ID;
  cell_id_info.freq = RRC_GPS_INVALID_FREQ;
  cell_id_info.psc = RRC_GPS_INVALID_PSC;

   /* If this is a dummy request due to race condition or GPS bug and we already have 
    * a valid cell-id, just return this back.
    * Else Check if any State Transition or Reconfig or W->G Handover 
    * is ongoing or UE is not camped on or if UE is just resuming from being suspended during tune-away.
    * If so, we are not in a good state to handle this. GPS will retry.
    */

  if(cell_id_dch_info.cell_id != WCDMA_INVALID_CELL_ID)
  {
    rrc_send_cell_id_cnf_to_cm_in_idle();
    handle_cell_id_req = FALSE;
  }
  else 
  if ( ( rrc_mode != RRC_MODE_ACTIVE ) ||
       ( RRC_NOT_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_GPS)) ||
       (RRCCSP_WT_FOR_CHAN_CFG_CNF == rrc_csp_sstate) ||
       (RRCCSP_WT_FOR_CHAN_CFG_CNF_PENDING == rrc_csp_sstate))
  {
    MSG_ERROR(" RRCGPS: Cannot handle RRC_GPS_CELL_ID_REQUEST: mode %d, camped %d (0: camped), csp substate %d",
                rrc_mode,rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_GPS),rrc_csp_sstate);
    rrc_send_cell_id_cnf_to_cm(NULL);
    handle_cell_id_req = FALSE;
  }

  if(handle_cell_id_req == TRUE )
  {
    switch(rrc_curr_state)
    {
      case RRC_STATE_CELL_DCH:
        /* send a Failure to GPS if : 
          *  1)If the ordered config is in use that means some state transitions are ongoing,
          *   2) Or if UE is in OOS 
          */
        if( (oc_status != OC_NOT_SET) ||(rrcllc_get_toc_usage() == TOC_FOR_OOS) || 
            (rrccu_get_cell_update_started_status_with_cause(&cu_cause)== RRCCU_STARTED)) 
        {
          MSG_ERROR(" RRCGPS: RRC cannot handle RRC_GPS_CELL_ID_REQUEST oc_status %d  TOC  OOS status  %d cu state %d ",
                                                                  oc_status,rrcllc_get_toc_usage(),rrccu_return_cu_substate() );
          rrc_send_cell_id_cnf_to_cm(NULL);
        }
        else if(rrcrcr_is_proc_active())
        {
          WRRC_MSG0_ERROR(" RRCGPS: RRC cannot handle  RRC_GPS_CELL_ID_REQUEST due to RCR active");
          rrc_send_cell_id_cnf_to_cm(NULL);
        }
        else
        {
          if(rrc_gps_send_sib_cmd() == FAILURE)
          {
            MSG_LOW("RRCGPS: Failure to send RRC_GET_SPECIFIC_SIBS_REQ into SIB ",0,0,0);
          }
          rrcgps_substate = RRCGPS_WAIT_FOR_SIB_CNF;
        }
      break;
  
      /* get from SIB database */
      case RRC_STATE_CELL_FACH:
      case RRC_STATE_URA_PCH:
      case RRC_STATE_CELL_PCH:
      case RRC_STATE_CONNECTING:
      case RRC_STATE_DISCONNECTED:
        /* We should send a response soon */
//Temporary hack till GPS is able to not send cell-id request        rrc_send_cell_id_cnf_to_cm(NULL);
        rrc_send_cell_id_cnf_to_cm_in_idle();
      break;
  
      default:
       WRRC_MSG1_ERROR("RRCGPS: Received RRC_GPS_CELL_ID_REQUEST in bad RRC state %d",rrc_curr_state);
      break;
    }
  }
}

/*===========================================================================

FUNCTION   rrcgps_initial_substate_event_handler

DESCRIPTION
    
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void  rrcgps_initial_substate_event_handler
(
  rrc_cmd_type *cmd_ptr /* Received command                   */
)
{

  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_GPS_CELL_ID_REQUEST:
      /*CM will request RRC to read CID in DCH state*/
    case RRC_CM_DCH_CELL_ID_REQUEST:
      rrc_gps_process_cell_id_req();
      WRRC_MSG1_HIGH("RRCGPS: Received RRC_GPS_CELL_ID_REQUEST in rrc state %d",rrc_get_state());
      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("RRCGPS: Ignoring RRC GPS event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rrcgps_substate);
      break;
  }
}



/*===========================================================================

FUNCTION   rrcgps_wait_sib_cnf_substate_event_handler

DESCRIPTION

 This function handles the events received by this module when the module is in 
  RRCGPS_WAIT_FOR_SIB_CNF sub-state. 
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcgps_wait_sib_cnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr /* Received command                   */
)
{
  rrcmeas_plmn_cell_info_struct_type  cell_id_info;
  
  cell_id_info.cell_id = INVALID_CELL_ID;
  cell_id_info.freq = RRC_GPS_INVALID_FREQ;
  cell_id_info.psc = RRC_GPS_INVALID_PSC;
  
  memset(&cell_id_info,0x00, sizeof(rrc_plmn_identity_type));
   
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_GPS_CELL_ID_REQUEST:
    /*CM will request RRC to read CID in DCH state*/
    case RRC_CM_DCH_CELL_ID_REQUEST:
      WRRC_MSG0_ERROR(" RRCGPS: Did not expect a RRC_GPS_CELL_ID_REQUEST while already processing one");
     
    break;

    case RRC_GET_SPECIFIC_SIBS_CNF:
      if(cmd_ptr->cmd.get_sibs_cnf.status == SIB_EVENT_SUCCEEDED)
      {
        rrc_send_cell_id_cnf_to_cm(cmd_ptr );
#ifdef FEATURE_QSH_EVENT_METRIC
        rrc_update_cell_id_in_serving_cell_metrics(cmd_ptr->cmd.get_sibs_cnf.cell_id);
#endif
      }
      else
      {
        rrc_send_cell_id_cnf_to_cm(NULL);
        WRRC_MSG0_HIGH(" RRCGPS: Failed to  read of cell id,Failure GPS");
      }
      rrcgps_substate = RRCGPS_INITIAL;
      break;

    case RRC_CPHY_DL_WEAK_IND:
       /* On receiving RRC_CPHY_DL_WEAK_IND from L1, call the SIB function 
         * to abort the event */
       WRRC_MSG0_HIGH("RRCGPS: Recieved  RRC_CPHY_DL_WEAK_IND, abort sib event");
       rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
      break;

    case RRC_STATE_CHANGE_IND :
      rrc_send_cell_id_cnf_to_cm(NULL);
      WRRC_MSG0_HIGH(" RRCGPS: Abort Due to State Change Ind, Send failure GPS");
      rrcgps_substate = RRCGPS_INITIAL;
      break;
    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("RRCGPS: Ignoring RRC GPS event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rrcgps_substate);
      break;
  }
}

/*===========================================================================

FUNCTION  rrcgps_init_data

DESCRIPTION

  This function clears all the procedure variables and puts the initial
  values wherever needed.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcgps_init_data( void )
{                   

  /* Reset variables for each transaction */
  rrcgps_substate = RRCGPS_INITIAL;
  rrc_cell_id_cnf_cb_func = NULL;
}

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
)
{

  WRRC_MSG1_HIGH_OPT("function rrcgps_procedure_event_handler is called in rrcgps_substate = %d",rrcgps_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_GPS,rrcgps_substate,cmd_ptr->cmd_hdr.cmd_id);

  /* Check the procedure substate and procedure the event accordingly */  
  switch( rrcgps_substate )
  {
    case RRCGPS_INITIAL:
      /* Call the event handler for Initial substate */
      rrcgps_initial_substate_event_handler( cmd_ptr );
      break;

    case RRCGPS_WAIT_FOR_SIB_CNF:
      /* Call the event handler for waiting for chan cfg cnf substate */
      rrcgps_wait_sib_cnf_substate_event_handler( cmd_ptr );
      break;
  
     default:
      /* Invalid substate - should never happen. Clear
         all procedure vars */
     WRRC_MSG1_ERROR("RRCGPS: Invalid RRC GPS Substate %d", rrcgps_substate);
      rrcgps_init_data();
      break;
  }/* end switch */
}
/*===========================================================================
FUNCTION rrc_send_cell_id_ind_to_cm

DESCRIPTION
  This function is called to notify the registered callback of the serving
  cell_id in Cell_DCH state.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void rrc_send_cell_id_ind_to_cm(void )
{
  /* This stores cell location info to be given to Callbacks*/
  cm_rpt_type *cmd_ptr;
  
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
  "Cell info: Cell-id %d, LAC %d %d, PSC %d, Freq %d, Sib_idx PLMN: MCC %d-MNC %d ", 
  cell_id_dch_info.cell_id,
  cell_id_dch_info.lac[0],
  cell_id_dch_info.lac[1],
  cell_id_dch_info.psc,
  cell_id_dch_info.uarfcn_dl,
  RRC_CSP_GET_INT_MCC(cell_id_dch_info.plmn_id),
  RRC_CSP_GET_INT_MNC(cell_id_dch_info.plmn_id),0,0);


//HACK for unit testing
  cmd_ptr = cm_get_cmd_buf();
  cmd_ptr->hdr.cmd = CM_RRC_CELL_INFO_IND;
#ifdef FEATURE_DUAL_SIM
  cmd_ptr->asubs_id = rrc_get_as_id();
#endif

  cmd_ptr->cmd.cell_info.cell_id = cell_id_dch_info.cell_id;
  cmd_ptr->cmd.cell_info.plmn_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(cell_id_dch_info.plmn_id);
  cmd_ptr->cmd.cell_info.lac_id = b_unpackw(cell_id_dch_info.lac, 0, 16); 
  cmd_ptr->cmd.cell_info.psc = cell_id_dch_info.psc;
  cmd_ptr->cmd.cell_info.uarfcn_dl = cell_id_dch_info.uarfcn_dl;
  cmd_ptr->cmd.cell_info.uarfcn_ul = cell_id_dch_info.uarfcn_ul;
  cmd_ptr->cmd.cell_info.current_band = cell_id_dch_info.current_band;
  cm_put_cmd(cmd_ptr);
#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_update_cell_id_in_serving_cell_metrics(cell_id_dch_info.cell_id);
#endif
}

/*===========================================================================
FUNCTION rrc_send_cell_id_cnf_to_cm_in_idle

DESCRIPTION
  This function is called to notify the registered callback of the serving
  cell_id in Cell_idle state.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_send_cell_id_cnf_to_cm_in_idle
(
void
)
{
  cm_rpt_type *cm_cmd_ptr;
  
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
  "Cell info: Cell-id %d, LAC %d %d, PSC %d, Freq %d, Sib_idx PLMN: MCC %d-MNC %d ", 
  cell_id_dch_info.cell_id,
  cell_id_dch_info.lac[0],
  cell_id_dch_info.lac[1],
  cell_id_dch_info.psc,
  cell_id_dch_info.uarfcn_dl,
  RRC_CSP_GET_INT_MCC(cell_id_dch_info.plmn_id),
  RRC_CSP_GET_INT_MNC(cell_id_dch_info.plmn_id),0,0);


/* For unit-testing*/
  cm_cmd_ptr = cm_get_cmd_buf();
  cm_cmd_ptr->hdr.cmd = CM_RRC_DCH_CELL_INFO_CNF;
#ifdef FEATURE_DUAL_SIM
  cm_cmd_ptr->asubs_id = rrc_get_as_id();
#endif

  cm_cmd_ptr->cmd.cell_info.cell_id = cell_id_dch_info.cell_id;
  cm_cmd_ptr->cmd.cell_info.plmn_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(cell_id_dch_info.plmn_id);
  cm_cmd_ptr->cmd.cell_info.lac_id = b_unpackw(cell_id_dch_info.lac, 0, 16);	
  cm_cmd_ptr->cmd.cell_info.psc = cell_id_dch_info.psc;
  cm_cmd_ptr->cmd.cell_info.uarfcn_dl = cell_id_dch_info.uarfcn_dl;
  cm_cmd_ptr->cmd.cell_info.uarfcn_ul = cell_id_dch_info.uarfcn_ul;
  cm_cmd_ptr->cmd.cell_info.current_band = cell_id_dch_info.current_band;

  cm_put_cmd(cm_cmd_ptr);

  return;
}
/*===========================================================================
FUNCTION rrc_send_cell_id_cnf_to_cm

DESCRIPTION
  This function is called to notify the registered callback of the serving
  cell_id in Cell_DCH state.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_send_cell_id_cnf_to_cm
(
   rrc_cmd_type *cmd_ptr /* Received command                   */
)
{
  cm_rpt_type *cm_cmd_ptr;
  if (cmd_ptr == NULL)
  {
    cell_id_dch_info.cell_id = WCDMA_INVALID_CELL_ID;
    /* rest of the values are same as previous indication*/
  }
  else
  {
    cell_id_dch_info.cell_id = cmd_ptr->cmd.get_sibs_cnf.cell_id;
    cell_id_dch_info.psc  = rrc_gps_psc_stored;
    cell_id_dch_info.lac[0] = cmd_ptr->cmd.get_sibs_cnf.lac[0];
    cell_id_dch_info.lac[1] = cmd_ptr->cmd.get_sibs_cnf.lac[1];
    cell_id_dch_info.plmn_id = cmd_ptr->cmd.get_sibs_cnf.plmn_id;
    /* Rest of the values are same as prev indication*/
  }
  
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
  "Cell info: Cell-id %d, LAC %d %d, PSC %d, Freq %d, Sib_idx PLMN: MCC %d-MNC %d ", 
  cell_id_dch_info.cell_id,
  cell_id_dch_info.lac[0],
  cell_id_dch_info.lac[1],
  cell_id_dch_info.psc,
  cell_id_dch_info.uarfcn_dl,
  RRC_CSP_GET_INT_MCC(cell_id_dch_info.plmn_id),
  RRC_CSP_GET_INT_MNC(cell_id_dch_info.plmn_id),0,0);


/* For unit-testing*/
  cm_cmd_ptr = cm_get_cmd_buf();
  cm_cmd_ptr->hdr.cmd = CM_RRC_DCH_CELL_INFO_CNF;
#ifdef FEATURE_DUAL_SIM
  cm_cmd_ptr->asubs_id = rrc_get_as_id();
#endif

  cm_cmd_ptr->cmd.cell_info.cell_id = cell_id_dch_info.cell_id;
  cm_cmd_ptr->cmd.cell_info.plmn_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(cell_id_dch_info.plmn_id);
  cm_cmd_ptr->cmd.cell_info.lac_id = b_unpackw(cell_id_dch_info.lac, 0, 16);  
  cm_cmd_ptr->cmd.cell_info.psc = cell_id_dch_info.psc;
  cm_cmd_ptr->cmd.cell_info.uarfcn_dl = cell_id_dch_info.uarfcn_dl;
  cm_cmd_ptr->cmd.cell_info.uarfcn_ul = cell_id_dch_info.uarfcn_ul;
  cm_cmd_ptr->cmd.cell_info.current_band = cell_id_dch_info.current_band;

  cm_put_cmd(cm_cmd_ptr);

  return;
}


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
void rrcgps_set_psc_for_cell_id_req(uint32 psc)
{
  rrc_gps_psc_stored = SCR_CODE_TO_PRIMARY_SCR_CODE(psc);
}


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
uint32 rrcgps_get_psc_for_cell_id_req()
{
  return rrc_gps_psc_stored;
}


/*===========================================================================

FUNCTION  rrc_register_cell_id_cnf_cb

DESCRIPTION

  This function registers the callback function to be called for the returning the cell id values to GPS.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE if Registration is success
  else
  FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_register_cell_id_cnf_cb
(
  RRC_GPS_CELL_ID_CNF_CB_FUNC_TYPE  *cb_ptr
)
{


  if (cb_ptr)
  {
    /* First go thru the list to make sure func does 
     * not get registered twice.
     */
      if (rrc_cell_id_cnf_cb_func == NULL)
      {
        WRRC_MSG1_HIGH("RRCGPS: Registering rrc_cell_id_cnf_cb_func ind cb: %d",cb_ptr);
        rrc_cell_id_cnf_cb_func = cb_ptr;
        return TRUE;
      }
      
      if (rrc_cell_id_cnf_cb_func == cb_ptr)
      {
        WRRC_MSG1_HIGH("RRCGPS:Function already registered cb:%d",cb_ptr);
        return TRUE;
      }

     
  }
  else
  {
    WRRC_MSG0_ERROR("RRCGPS: Cannot Register NULL CC Callback");
  }
  return FALSE;
}






/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Local Object Definition
** ----------------------------------------------------------------------- */
#ifdef FEATURE_CGPS_UMTS_CP_WCDMA

/* Note: with FEATURE_MMGPS which is mutually exclusive from FEATURE_CGPS_xxx,
   an equivelant function (lsm_convert_assistance_data), and the function it 
   calls (oss_bitstring_to_int), are defined in LSM and were once shared by 
   RRC and LSM. For FEATURE_CGPS, there will be no LSM, so these functions 
   will be defined only here. */
boolean rrcgps_convert_assistance_data( 
  rrc_UE_Positioning_GPS_AssistanceData* asn1_ptr,
  rrc_assistance_data_struct_type* rrc_ptr );


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Temp global variable for fixing stack overflow.
   Should be removed after memory allocation using pointer is introduced */
rrc_meas_params_struct_type temp_gps;
pos_meas_id_list_struct_type pos_meas_id_list[MAX_POS_MEAS];

extern uint8 rrcmeas_pending_rpt;

#ifdef FEATURE_AAGPS_MR_RRC_NOTIFICATION_REQUIRED
rrc_gps_l2_ack_struct_type gps_l2_ack_params;
#endif


/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */


#ifdef FEATURE_CGPS_UMTS_CP_WCDMA

/*===========================================================================

FUNCTION  rrcgps_register_cgps_event_cb

DESCRIPTION
  This function is called by CGPS to install a callback which will be used
  by RRC to provide events and/or data back to CGPS

DEPENDENCIES None

RETURN VALUE : TRUE/FALSE

SIDE EFFECTS None

===========================================================================*/

boolean rrcgps_register_cgps_event_cb( rrcgps_cgps_event_cb_type event_cb_fp )
{
  if ( event_cb_fp == NULL )
    return FALSE;

  rrcgps_cgps_event_cb = event_cb_fp;
  return TRUE;
}


/*===========================================================================

FUNCTION  rrcgps_register_cgps_ue_capability_cb

DESCRIPTION
  This function is called by CGPS to install a callback which will be used
  by RRC to get UE capability information from TM.

DEPENDENCIES None

RETURN VALUE : TRUE/FALSE

SIDE EFFECTS None

===========================================================================*/


boolean rrcgps_register_cgps_ue_pos_capability_cb ( rrcgps_cgps_ue_pos_capability_cb_type ue_cap_cb_fp)
{
  if ( ue_cap_cb_fp == NULL )
    return FALSE;

  rrcgps_cgps_ue_pos_capability_cb = ue_cap_cb_fp;
  return TRUE;
}  

#endif /* FEATURE_CGPS_UMTS_CP_WCDMA */



/*===========================================================================

FUNCTION   RRCGPS_PROCESS_MCM

DESCRIPTION

    This function processes the Measurement Control Message for
    A-GPS Measurements. 

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_process_mcm
(
  void* pos_mcm_ptr,
  pos_info_struct_type* p_info_ptr
  ,uint8 meas_ctrl_tr_id
)
{
  /* Process the A-GPS Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  uint16 deleted_meas_id = 0;
  
  /* Declare MCM config structure that goes to L1 */
  rrc_meas_params_struct_type  *gps_meas_params;
  
  /* Declare ptr to store message received from UTRAN */
  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;
  
  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  rpt_crit_enum_type rpt_mode;
  
  boolean modify_meas_params_incl = FALSE;
  
  meas_validity_enum_type pos_meas_validity = NOT_PRESENT;
  
  rrc_UE_Positioning_ReportingQuantity* rpt_quan_ptr = NULL;
  
  uint8 transaction_id = INVALID_TRANSACTION_ID;

  gps_meas_params = &temp_gps;
  memset(gps_meas_params,0,sizeof(rrc_meas_params_struct_type));
  gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = FALSE;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.environ_ch_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = FALSE;
  gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.meas_start_incl = TRUE;
  
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)pos_mcm_ptr;
  transaction_id = meas_ctrl_tr_id;

  
  /* By default, it is set to 0 => 1 */
  gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = 0;
  
  /* Set Transaction Id and Meas Id */
  p_info_ptr->meas_id = gps_meas_params->u.meas_ctrl_params.meas_id =
    (uint8) dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementIdentity;
  
  gps_meas_params->u.meas_ctrl_params.config_conf_ind = TRUE;
  
  gps_meas_params->u.meas_ctrl_params.config_ack_num = (uint8) transaction_id;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan.method = (uint8)RRC_GPS;
  gps_meas_params->params_type = (uint8)RRC_MEAS_CTRL_PARMS;
  
  /* Check if Meas Validity present */
  if (dcch_msg_ptr->message.u.measurementControl.u.r3.m.v390nonCriticalExtensionsPresent)
  {
    
    
    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(dcch_msg_ptr->message.u.measurementControl.u.r3.
      v390nonCriticalExtensions.measurementControl_v390ext,
       ue_Positioning_Measurement_v390ext))
    {
      /* Check if Meas Validity present */
      if (RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.measurementControl.u.r3.
        v390nonCriticalExtensions.measurementControl_v390ext.ue_Positioning_Measurement_v390ext,
        rrc_UE_Positioning_Measurement_v390ext,measurementValidity))
      {
        WRRC_MSG1_ERROR("POS:Meas validity %d (0/d:1/f:2/a)", dcch_msg_ptr->message.u.measurementControl.u.r3.
          v390nonCriticalExtensions.measurementControl_v390ext.ue_Positioning_Measurement_v390ext.measurementValidity.ue_State);
        
        /* Store Measurement Validity */
        pos_meas_validity = TVM_RET_MEAS_VALIDITY(dcch_msg_ptr->message.u.measurementControl.u.r3.
          v390nonCriticalExtensions.measurementControl_v390ext.ue_Positioning_Measurement_v390ext.measurementValidity.ue_State);
        /* Check if Meas needs to be restarted */
        if (!(RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand,
            rrc_MeasurementCommand_release))
           && (((pos_meas_validity == STATE_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_FACH)) ||
           ((pos_meas_validity == ALL_STATES_BUT_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_DCH))))
        {
          WRRC_MSG0_HIGH("POS: RRC val, MCM val differ");
          gps_meas_params->u.meas_ctrl_params.meas_start_incl = FALSE;
        }
      }
    }
  }
  
  if (RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand,
       rrc_MeasurementCommand_setup))
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_SETUP;
    
    if (!(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3,
        rrc_MeasurementControl_r3_IEs,measurementReportingMode)))
    {
      /* Send Meas Ctrl Failure */
      WRRC_MSG0_ERROR("Meas Reporting Mode missing for SETUP");
      
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
    }
    else
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode =
       (uint8) RET_POS_RPT_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementReportingMode.measurementReportTransferMode);
    }
    
    
    rpt_quan_ptr = &dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.
      measurementCommand.u.setup->u.ue_positioning_Measurement->ue_positioning_ReportingQuantity;
    
    
    gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = TRUE;
    
    /* Get Pos Method type */
    gps_meas_params->u.meas_ctrl_params.rpt_quan.method_type = (uint8)RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
    
    if (rrcgps_verify_positioning_capability(rpt_quan_ptr)
      == FAILURE)
    {
      /* Send Meas Ctrl Failure */
      
      
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
#ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      if(rrcgps_cgps_ue_pos_capability_cb == NULL)
      {
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      }
      else
#endif
      {
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      }
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
      
    }

    
    /* Get Reporting Quantity */
    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rpt_quan_ptr,horizontal_Accuracy))
    {
      gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = TRUE;
      
      /* Set the 7 bits */
      gps_meas_params->u.meas_ctrl_params.rpt_quan.horizontal_accuracy = (uint8)
        (*(rpt_quan_ptr->horizontal_Accuracy.data) >> 1);
    }
    
    
    /* AdditionalAssistanceDataRequest */
    if (rpt_quan_ptr->additionalAssistanceDataRequest == TRUE)
    {
      gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = TRUE;
    }
    
    if (rpt_quan_ptr->gps_TimingOfCellWanted == TRUE)
    {
      gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = TRUE;
    }
    
    /* Get Reporting Criteria */
    if (RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.
        measurementCommand.u.setup->u.ue_positioning_Measurement->reportCriteria,
      rrc_UE_Positioning_ReportCriteria_ue_positioning_ReportingCriteria))
    {
      WRRC_MSG0_ERROR("POS: Event trigg crit unsupported");
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
    }
    else if (RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.setup->u.
      ue_positioning_Measurement->reportCriteria,
      rrc_UE_Positioning_ReportCriteria_noReporting))
    {
      gps_meas_params->u.meas_ctrl_params.rpt_crit.rpt_mode = (uint8)RRC_NO_RPT;
    }
    else
    {
      
      gps_meas_params->u.meas_ctrl_params.rpt_crit.rpt_mode = (uint8) RRC_PERIODIC_RPT;
      
      /* Get reporting criteria and interval */
      gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = (uint8)rrc_ReportingAmount_ra_Infinity;

      gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = (uint8) 
        dcch_msg_ptr->message.u.measurementControl.u.r3.
           measurementControl_r3.measurementCommand.u.setup->u.ue_positioning_Measurement->
           reportCriteria.u.periodicalReportingCriteria->reportingAmount;

      gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_interval =(uint8) 
        dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.
        u.setup->u.ue_positioning_Measurement->reportCriteria.u.periodicalReportingCriteria->reportingInterval;
    }
    if(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.setup->u.
        ue_positioning_Measurement->m.ue_positioning_GPS_AssistanceDataPresent
      )
    {
    /* Now call LSM fn to store Meas Object if FEATURE_MMGPS; call a local function to do the same if FEATURE_CGPS_xxx */
    #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      if (rrcgps_convert_assistance_data(&dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.setup->u.
        ue_positioning_Measurement->ue_positioning_GPS_AssistanceData,
        &(gps_meas_params->u.meas_ctrl_params.gps_assistance_data)) == FALSE)
    #else  /* this will only be FEATURE_MMGPS */
      if (lsm_convert_assistance_data(&dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.setup->u.
        ue_positioning_Measurement->ue_positioning_GPS_AssistanceData,
        &(gps_meas_params->u.meas_ctrl_params.gps_assistance_data)) == FALSE)
    #endif
      {
        WRRC_MSG0_ERROR("POS: Assistance Data conversion failed");
      
        /* Set transaction ID */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
        /* Set faliure t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
      
        rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      
        return FAILURE;
      }
      gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = TRUE;
    }
   
    switch(gps_meas_params->u.meas_ctrl_params.rpt_crit.rpt_mode)
    {
      case 0: rpt_mode = PERIODIC;
          break;
      case 1: rpt_mode = EVENT;
          break;
      case 2: rpt_mode = ADDTL;
          break;
      case 3: rpt_mode = NOT_AVAILABLE;
          break;
      default:break;
    } 
    
    /* Additional Meas. LSM does not support it. Ignore */
    if(rrcmeas_config_pos_db(p_info_ptr, &pos_meas_validity, &rpt_mode, modify_meas_params_incl) == FALSE)
    {
       return FAILURE;   
    }
    
  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand,
    rrc_MeasurementCommand_modify))
  {
    /* Addtl Meas processing later. Reporting crit stored in db later */
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_MODIFY;
    
    if (RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3,
        rrc_MeasurementControl_r3_IEs,measurementReportingMode))
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode = (uint8)
        RET_POS_RPT_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(dcch_msg_ptr->message.u.measurementControl.u.r3.
        measurementControl_r3.measurementCommand.u.modify,rrc_MeasurementCommand_modify,
        measurementType))
    {
      modify_meas_params_incl = TRUE;
      
      rpt_quan_ptr = &dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.modify->measurementType.u.
        ue_positioning_Measurement->ue_positioning_ReportingQuantity;
     
      gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = TRUE;
      
      
      /* Get Pos Method */
      gps_meas_params->u.meas_ctrl_params.rpt_quan.method_type = (uint8) RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
      
      if (rrcgps_verify_positioning_capability(rpt_quan_ptr)
        == FAILURE)
      {
        /* Send Meas Ctrl Failure */
        
        
        /* Set transaction ID */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
#ifdef FEATURE_CGPS_UMTS_CP_WCDMA
        if(rrcgps_cgps_ue_pos_capability_cb == NULL)
        {
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        }
        else
#endif
        {
          /* Set error t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
        }
        
        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return FAILURE;
        
      }

      
      /* Get Reporting Quantity */
      if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rpt_quan_ptr,horizontal_Accuracy))
      {
        gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = TRUE;
        
        /* Set the 7 bits */
        gps_meas_params->u.meas_ctrl_params.rpt_quan.horizontal_accuracy = (uint8)
          (*(rpt_quan_ptr->horizontal_Accuracy.data) >> 1);
      }
      
      /* AdditionalAssistanceDataRequest */
      if (rpt_quan_ptr->additionalAssistanceDataRequest == TRUE)
      {
        gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = TRUE;
      }
      
      if (rpt_quan_ptr->gps_TimingOfCellWanted == TRUE)
      {
        gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = TRUE;
      }
      
      
      /* Get Reporting Criteria */
      if (RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.
        modify->measurementType.u.ue_positioning_Measurement->reportCriteria,
        rrc_UE_Positioning_ReportCriteria_ue_positioning_ReportingCriteria))
      {
        WRRC_MSG0_ERROR("POS: Event trigg crit unsupported");
        /* Set transaction ID */
        rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
        /* Set error t */
        rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
        
        /* Send MCF msg to UTRAN */
        rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
        
        return FAILURE;
      }
      else if (RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.modify->measurementType.u.
        ue_positioning_Measurement->reportCriteria,
        rrc_UE_Positioning_ReportCriteria_noReporting))
      {
        gps_meas_params->u.meas_ctrl_params.rpt_crit.rpt_mode = (uint8)RRC_NO_RPT;
      }
      else
      {
        gps_meas_params->u.meas_ctrl_params.rpt_crit.rpt_mode = (uint8)RRC_PERIODIC_RPT;
        
        /* Get reporting criteria and interval */
        gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = (uint8)rrc_ReportingAmount_ra_Infinity;

          gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = 
            (uint8) dcch_msg_ptr->message.u.measurementControl.u.r3.
            measurementControl_r3.measurementCommand.u.modify->measurementType.u.ue_positioning_Measurement->
            reportCriteria.u.periodicalReportingCriteria->reportingAmount;     

        gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_interval = (uint8)
          dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.
          u.modify->measurementType.u.ue_positioning_Measurement->reportCriteria.u.periodicalReportingCriteria->reportingInterval;

      }
      if(dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.modify->measurementType.u.ue_positioning_Measurement->m.ue_positioning_GPS_AssistanceDataPresent  )
      {
      /* Now call LSM fn to store Meas Object if FEATURE_MMGPS; call a local function to do the same if FEATURE_CGPS_xxx */
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
        if (rrcgps_convert_assistance_data(&dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.modify->measurementType.u.
          ue_positioning_Measurement->ue_positioning_GPS_AssistanceData,
          &(gps_meas_params->u.meas_ctrl_params.gps_assistance_data)) == FALSE)
      #else /* this will only be FEATURE_MMGPS */
        if (lsm_convert_assistance_data(&dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementCommand.u.modify->measurementType.u.
          ue_positioning_Measurement->ue_positioning_GPS_AssistanceData,
          &(gps_meas_params->u.meas_ctrl_params.gps_assistance_data)) == FALSE)
      #endif
        {
          WRRC_MSG0_ERROR("POS: Assistance Data conversion failed");
        
          /* Set transaction ID */
          rrcmeas_mcf.rrc_transaction_id = transaction_id;
        
          /* Set faliure t */
          rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
        
          rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
        
          return FAILURE;
        }
        gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = TRUE;
      }
    } /* End of if of Meas Type present */
    
    switch(gps_meas_params->u.meas_ctrl_params.rpt_crit.rpt_mode)
    {
      case 0: rpt_mode = PERIODIC;
          break;
      case 1: rpt_mode = EVENT;
          break;
      case 2: rpt_mode = ADDTL;
          break;
      case 3: rpt_mode = NOT_AVAILABLE;
          break;
      default:break;
    } 
    
    /* Store Pos Parameters in RRC Database */
    if(rrcmeas_config_pos_db(p_info_ptr, &pos_meas_validity, &rpt_mode, modify_meas_params_incl) == FALSE)
    {
       return FAILURE;
    }
    
  } /* End of modify */
  else
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8) RRC_MEAS_DELETE;
    /* At present just store meas id in meas db */
    for ( count = 0; count < MAX_POS_MEAS; count++ )
    {
      if (pos_meas_id_list[count].meas_id == gps_meas_params->u.meas_ctrl_params.meas_id)
      {
        WRRC_MSG1_HIGH("POS: Meas Id %d deleted from DB", gps_meas_params->u.meas_ctrl_params.meas_id);
        pos_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        pos_meas_id_list[count].meas_validity = NOT_PRESENT;
        pos_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          pos_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        break;
      }
    } /* End of for */
    
    deleted_meas_id = (uint16) dcch_msg_ptr->message.u.measurementControl.u.r3.measurementControl_r3.measurementIdentity;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&deleted_meas_id);
    
    rrcgps_send_lsm_cmd(gps_meas_params);
    return SUCCESS;
  }

  rrcgps_send_lsm_cmd(gps_meas_params);
  return SUCCESS;

}


/*===========================================================================

FUNCTION    RRCGPS_SEND_MEAS_REPORT

DESCRIPTION

 Processes measurement report received from LSM
 in CELL_DCH state.

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
)
{
  /* Stores meas report */
  rrc_UL_DCCH_Message* msg_ptr = NULL;
  
  uint8 cnt = 0;
  
  boolean invalid_t = TRUE;
  /* Stores status of send chain */
  rrcsend_status_e_type status;
  
  /* RLC ack or unack mode */
  uecomdef_logch_mode_e_type logical_channel_mode = UE_MODE_TRANSPARENT;
  
  rrc_meas_rpt_struct_type* lsm_meas_ptr = NULL;
  
  /* Logical Channel Id */
  rlc_lc_id_type ul_lc_id = 0;
  
  /* Radio bearer mode */
  rrc_RB_Identity rb_mode = 0;
  
  /* L2 ack status */
  rrcsend_l2ack_status_e_type l2_ack_status;
  
  lsm_meas_ptr = cmd_ptr->cmd.meas_rpt_ind.meas_rpt;
  
  if((lsm_meas_ptr->meas_id < MEASUREMENT_IDENTITY_MIN) || (lsm_meas_ptr->meas_id > MEASUREMENT_IDENTITY_MAX))
  {
    WRRC_MSG1_ERROR("Invalid Measurement identity %d set by LSM", lsm_meas_ptr->meas_id);
    return;
  } 
  if ( lsm_meas_ptr->tranfer_mode == (uint8)MODE_ACKNOWLEDGED)
  {
    logical_channel_mode = UE_MODE_ACKNOWLEDGED_DATA;
    rb_mode = DCCH_AM_RADIO_BEARER_ID;
    l2_ack_status = RRCSEND_L2ACK_REQUIRED;
  }
  else
  {
    logical_channel_mode = UE_MODE_UNACKNOWLEDGED;
    rb_mode = DCCH_UM_RADIO_BEARER_ID;
    l2_ack_status = RRCSEND_L2ACK_NOT_REQUIRED;
  }
  
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH, rb_mode,
    logical_channel_mode);
  
  if ( ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND )
  {
    WRRC_MSG0_ERROR("RLC Logical channel not found");
    return;
  }
  
  
  /* Allocate a buffer for an uplink DCCH Message. This buffer
  * should be released by SEND_CHAIN using OSS compiler functions */
  
  msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc (sizeof(struct rrc_UL_DCCH_Message));
  
  /* Store meas report chosen */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_measurementReport;
  
  /* First store the meas id */
  rrcmeas_fill_meas_id(&msg_ptr->message.u.measurementReport,lsm_meas_ptr->meas_id, POSITION_MEAS);
  
  RRC_SET_COMMON_MSG_TYPE(msg_ptr->message.u.measurementReport.measuredResults,
    rrc_MeasuredResults_ue_positioning_MeasuredResults);

  msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults =
    rtxMemAllocTypeZ(&enc_ctxt,rrc_UE_Positioning_MeasuredResults);
  
  RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.measurementReport,
    rrc_MeasurementReport,measuredResults);
  
  /* TBD Add a function to validate the report */
  
  
  /* Check what kind of report it is */
  if (lsm_meas_ptr->gps_measured_results_incl == TRUE)
  {
    invalid_t=FALSE;
    RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_positioning_MeasuredResults,
      rrc_UE_Positioning_MeasuredResults,ue_positioning_GPS_Measurement);
    
    if (rrcgps_validate_meas_results(&lsm_meas_ptr->gps_measured_results) == FALSE )
    {
      
      rrc_free(msg_ptr);
      return;
    }
    
    rrcgps_set_meas_results(&lsm_meas_ptr->gps_measured_results,
      &msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_positioning_MeasuredResults->ue_positioning_GPS_Measurement);
    if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
    {
      if(lsm_meas_ptr->gps_measured_results.gps_reference_time_uncertainity_incl)
      {
        WRRC_MSG1_HIGH("REL7: GPS reference time uncertainity included : %d ",
            lsm_meas_ptr->gps_measured_results.gps_reference_time_uncertainity);
        if(!RRC_CHECK_MSG_TYPE_IE(msg_ptr->message.u.measurementReport,v390nonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport,v390nonCriticalExtensions);

        }
        if(!RRC_CHECK_MSG_TYPE_IE(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions, laterNonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions, laterNonCriticalExtensions);
     
        }
        if(!RRC_CHECK_MSG_TYPE_IE(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions, v4b0NonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions, v4b0NonCriticalExtensions);
    
        }
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions, v590NonCriticalExtensions);
 

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions, v5b0NonCriticalExtensions);


        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions, v690NonCriticalExtensions);


        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions, v770NonCriticalExtensions);


        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          measurementReport_v770ext, measuredResults);

        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          measurementReport_v770ext.measuredResults.t = T_rrc_MeasuredResults_v770ext_ue_positioning_MeasuredResults;

        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults =
           rtxMemAllocTypeZ(&enc_ctxt,rrc_UE_Positioning_MeasuredResults_v770ext);

        RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults, rrc_UE_Positioning_MeasuredResults_v770ext, ue_positioning_GPS_Measurement);

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_GPS_Measurement, ue_Positioning_GPS_ReferenceTimeUncertainty);

        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.measurementReport_v770ext.
          measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_GPS_Measurement.ue_Positioning_GPS_ReferenceTimeUncertainty = 
          lsm_meas_ptr->gps_measured_results.gps_reference_time_uncertainity;

      }
    }
  }
   #ifdef FEATURE_GANSS_SUPPORT
  if((rrc_ganss_supported == TRUE) && (lsm_meas_ptr->ganss_measured_results_incl == TRUE))
  {
      invalid_t = FALSE;
      rrcgps_fill_measured_results_ganss_incl(msg_ptr,lsm_meas_ptr);
  }
#endif
  if (lsm_meas_ptr->positioning_error_incl == TRUE)
  {
  invalid_t=FALSE;
#ifdef FEATURE_GANSS_SUPPORT
    if(rrc_ganss_supported == TRUE)
    {
      rrcgps_fill_measured_results_pos_err_incl(msg_ptr,lsm_meas_ptr);
    }
    else
  {
#endif
    RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_positioning_MeasuredResults,
      rrc_UE_Positioning_MeasuredResults,ue_positioning_Error);
    
      if(rrcgps_validate_pos_error(&lsm_meas_ptr->pos_error) == FALSE)
      {
        
        rrc_free(msg_ptr);
        return;
      }
    rrcgps_set_pos_error(&lsm_meas_ptr->pos_error,
      &msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_positioning_MeasuredResults->ue_positioning_Error);
#ifdef FEATURE_GANSS_SUPPORT
  }
#endif
  }
  if (lsm_meas_ptr->positioning_estimate_incl == TRUE)
  {
  invalid_t= FALSE;
#ifdef  FEATURE_GANSS_SUPPORT
if(rrc_ganss_supported)
 {
   if(FAILURE == rrcgps_fill_measured_result_pos_est_incl(msg_ptr,lsm_meas_ptr))
   {
     /*MRM validation fails*/
      WRRC_MSG0_HIGH("Meas Report Ignored");
      return;
   }
 }
 else
 {
 #endif
    if (rrcgps_validate_pos_estimate(&lsm_meas_ptr->positioning_estimate) == FALSE)
    {
      WRRC_MSG0_HIGH("POS: Ignore Meas Report");
      rrc_free(msg_ptr);
      return;
    }
    RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_positioning_MeasuredResults,
      rrc_UE_Positioning_MeasuredResults,ue_positioning_PositionEstimateInfo);
    
    rrcgps_set_pos_estimate(&lsm_meas_ptr->positioning_estimate,
      &msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo);
#ifdef FEATURE_GANSS_SUPPORT
  }
#endif
  }
 if(invalid_t== TRUE)
  {
    WRRC_MSG0_ERROR("POS: Invalid t");
    rrc_free(msg_ptr);
    return;
  }
  
  /* Additional Meas to be added later to this report */
  
  /* Check if this is the last report. Addtl Meas to be taken care of later */
  if (lsm_meas_ptr->num_reports_ind == TRUE)
  {
    for (cnt = 0; cnt < MAX_POS_MEAS; cnt++)
    {
      if (pos_meas_id_list[cnt].meas_id == lsm_meas_ptr->meas_id)
      {
        WRRC_MSG1_HIGH("POS: Meas %d deleted from DB",lsm_meas_ptr->meas_id);
        pos_meas_id_list[cnt].meas_id = INVALID_MEAS_ID;
        break;
      }
    }
  }
  
#ifdef FEATURE_AAGPS_MR_RRC_NOTIFICATION_REQUIRED
  
  if (l2_ack_status == RRCSEND_L2ACK_REQUIRED)
  {
    /* This places SDU into RLC watermark queue */
    status =  rrcsend_send_ul_sdu_with_mui(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
                                           ul_lc_id, l2_ack_status, &gps_l2_ack_params.mui);

    WRRC_MSG2_HIGH("GPS MUI for Meas Rpt %d is %d", 
             lsm_meas_ptr->meas_id, gps_l2_ack_params.mui);

    if (status == RRCSEND_SUCCESS)
    {
      WRRC_MSG0_HIGH("AGPS:L2 ack requested");
      gps_l2_ack_params.waiting_for_l2_ack = TRUE;
      gps_l2_ack_params.meas_id = lsm_meas_ptr->meas_id;
    }
  }
  else
  {
    /* This places SDU into RLC watermark queue */
    status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
      ul_lc_id, l2_ack_status);
  }
#else

  /* This places SDU into RLC watermark queue */
  status =  rrcsend_send_ul_sdu(RRC_PROCEDURE_MCMR, (void *) msg_ptr,
    ul_lc_id, l2_ack_status);

#endif
  
  
  if ( status != RRCSEND_SUCCESS )
  {
    WRRC_MSG0_ERROR("POS:Send chain failed Meas Rpt");
  }
  else
  {
    WRRC_MSG0_HIGH("POS:Rpt sent to RLC");
    if(lsm_meas_ptr->tranfer_mode == (uint8) MODE_ACKNOWLEDGED)
    {
      rrcmeas_pending_rpt++; 
      WRRC_MSG1_HIGH("pending meas rpt = %d",rrcmeas_pending_rpt);
    }
  }
  
  return;
}


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

void rrcgps_send_lsm_cmd ( rrc_meas_params_struct_type* gps_meas_params_ptr )
{

 WRRC_MSG0_HIGH("POS: Sending GPS Meas Request");
#ifdef FEATURE_CGPS_UMTS_CP_WCDMA
  if(rrcgps_cgps_event_cb != NULL)
  {
    rrc_cgps_event_data_s_type    event_data;
    rrc_meas_params_struct_type *gps_meas_params_ptr2 = (rrc_meas_params_struct_type *)rrc_malloc(sizeof(rrc_meas_params_struct_type));
    WRRC_MSG2_HIGH("Gps_meas_params_ptr2=%p \tSize %d",gps_meas_params_ptr2,sizeof(*gps_meas_params_ptr2));
    WCDMA_MEMCPY((void *)gps_meas_params_ptr2,
                 sizeof(rrc_meas_params_struct_type),
                 (void *)gps_meas_params_ptr,
                 sizeof(rrc_meas_params_struct_type));
     /*lint -e522 */
     
    event_data.e_event_type = RRC_CGPS_RRC_MSG_EVENT;
    event_data.u.z_rrc_cgps_msg_data.length = sizeof(rrc_meas_params_struct_type);
    event_data.u.z_rrc_cgps_msg_data.p_data = (uint8 *)gps_meas_params_ptr2;
#ifdef FEATURE_GANSS_SUPPORT
    MSG_7(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GPS : Meas id %3d, GPS AD incl %d, RefTime %d, RefLoc %d, UTC %d, Alm %d, Nav %d", 
        gps_meas_params_ptr2->u.meas_ctrl_params.meas_id, 
        gps_meas_params_ptr2->u.meas_ctrl_params.gps_assistance_data_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.gps_assistance_data.gps_ref_time_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.gps_assistance_data.gps_ref_loc_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.gps_assistance_data.gps_utc_model_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.gps_assistance_data.gps_almanac_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.gps_assistance_data.gps_navigation_model_incl);
    MSG_7(MSG_SSID_GPSSM, MSG_LEGACY_MED, "params_type %3d, ganss_assistance_data_incl %d, ganss_ref_time_incl %d, ganss_ref_pos_incl %d, ganss_addlono_model_incl %d, ganss_data_list_incl %d, list_cnt  %d",
        gps_meas_params_ptr2->params_type, gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_ref_time_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_ref_pos_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_addlono_model_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list_incl,
        gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.list_cnt); 
    MSG_9(MSG_SSID_GPSSM, MSG_LEGACY_MED, "ganss_id_incl %d, ganssId %d, time_mode_list_incl %d,add_navigation_models_incl %d, RealTime_Integrity_incl %d, ref_meas_info_incl %d, GANSS_Almanac_incl %d, add_utc_models_incl %d, Auxiliary_Info_incl %d",
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].ganss_id_incl,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].ganssId,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].time_mode_list_incl,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].add_navigation_models_incl,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].RealTime_Integrity_incl,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].ref_meas_info_incl,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].GANSS_Almanac_incl,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].add_utc_models_incl,
          gps_meas_params_ptr2->u.meas_ctrl_params.ganss_assistance_data.ganss_data_list[0].Auxiliary_Info_incl);
#endif
    (void)rrcgps_cgps_event_cb(&event_data);
    /*lint +e522 */
  }
  else
  {
    ERR_FATAL("Callback func pointer is NULL", 0, 0, 0);
  }

#endif

}


/*===========================================================================

FUNCTION    RRCGPS_SEND_MEAS_REPORT

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
)
{
  rrc_GPS_MeasurementParamList * gps_meas_list_ptr = NULL;
  
  uint8 cnt = 0;
  
  if (lsm_gps_meas_results_ptr->positioning_estimate == (uint8)GPS_REFERENCE_TIME_ONLY )
  {
    RRC_SET_COMMON_MSG_TYPE(rrc_gps_meas_results_ptr->referenceTime,
      rrc_UE_Positioning_GPS_MeasurementResults_referenceTime_gps_ReferenceTimeOnly);
    
    rrc_gps_meas_results_ptr->referenceTime.u.gps_ReferenceTimeOnly =
      lsm_gps_meas_results_ptr->u.gps_reference_time;
    
  }
  else
  {
    RRC_SET_COMMON_MSG_TYPE(rrc_gps_meas_results_ptr->referenceTime,
      rrc_UE_Positioning_GPS_MeasurementResults_referenceTime_utran_GPSReferenceTimeResult);
    
    if (lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.gps_ref_psc_incl == TRUE)
    {
      RRC_SET_COMMON_MSG_TYPE(rrc_gps_meas_results_ptr->referenceTime.u.utran_GPSReferenceTimeResult->modeSpecificInfo,
        rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd);
      
      rrc_gps_meas_results_ptr->referenceTime.u.utran_GPSReferenceTimeResult->modeSpecificInfo.
        u.fdd->referenceIdentity.primaryScramblingCode =
        lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.psc;
    }
    
    /* SFN */
    rrc_gps_meas_results_ptr->referenceTime.u.utran_GPSReferenceTimeResult->sfn =
      lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.sfn;
    
    rrc_gps_meas_results_ptr->referenceTime.u.utran_GPSReferenceTimeResult->ue_GPSTimingOfCell.ls_part =
      lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.lsb_timing;
    
    rrc_gps_meas_results_ptr->referenceTime.u.utran_GPSReferenceTimeResult->ue_GPSTimingOfCell.ms_part =
      lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.msb_timing;
    
  } /* End of else of utran_GPSReferenceTimeResults */
  
  /* Now set measured params */
  if (lsm_gps_meas_results_ptr->no_of_satellites == 0)
  {
    return;
  }

  /* Assign head of list */
  gps_meas_list_ptr = &rrc_gps_meas_results_ptr->gps_MeasurementParamList;
    
  gps_meas_list_ptr->n = lsm_gps_meas_results_ptr->no_of_satellites;
  ALLOC_ASN1ARRAY2(&enc_ctxt,gps_meas_list_ptr,rrc_GPS_MeasurementParam);
    
  for (cnt =0; cnt < lsm_gps_meas_results_ptr->no_of_satellites; cnt++)
  {
    gps_meas_list_ptr->elem[cnt].c_N0 = lsm_gps_meas_results_ptr->gps_measured_params[cnt].c_no;
    
    gps_meas_list_ptr->elem[cnt].doppler = lsm_gps_meas_results_ptr->gps_measured_params[cnt].doppler;
    
    gps_meas_list_ptr->elem[cnt].fractionalGPS_Chips =
      lsm_gps_meas_results_ptr->gps_measured_params[cnt].fractional_gps_chips;
      
    gps_meas_list_ptr->elem[cnt].multipathIndicator =
      (rrc_MultipathIndicator)lsm_gps_meas_results_ptr->gps_measured_params[cnt].multipath_ind;
      
    gps_meas_list_ptr->elem[cnt].pseudorangeRMS_Error =
      lsm_gps_meas_results_ptr->gps_measured_params[cnt].rms_error;
      
    gps_meas_list_ptr->elem[cnt].satelliteID =
      lsm_gps_meas_results_ptr->gps_measured_params[cnt].sat_id;
      
    gps_meas_list_ptr->elem[cnt].wholeGPS_Chips =
      lsm_gps_meas_results_ptr->gps_measured_params[cnt].whole_gps_chips;
    
  }
  
  return;

}

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
)
{
  uint8 cnt = 0;
  
  rrc_SatDataList * sat_list_ptr = NULL;
  
  
  
  /* Set error reason */
  
  switch(lsm_gps_meas_ptr->error_cause)
  {
    case 0: rrc_gps_pos_error_ptr->errorReason = rrc_UE_Positioning_ErrorCause_notEnoughOTDOA_Cells;
        break;
    case 1: rrc_gps_pos_error_ptr->errorReason = rrc_UE_Positioning_ErrorCause_notEnoughGPS_Satellites;
        break;
    case 2: rrc_gps_pos_error_ptr->errorReason = rrc_UE_Positioning_ErrorCause_assistanceDataMissing;
        break;
    case 3: rrc_gps_pos_error_ptr->errorReason =rrc_UE_Positioning_ErrorCause_notAccomplishedGPS_TimingOfCellFrames;
        break;
    case 4: rrc_gps_pos_error_ptr->errorReason = rrc_UE_Positioning_ErrorCause_undefinedError;
        break;
    case 5: rrc_gps_pos_error_ptr->errorReason = rrc_UE_Positioning_ErrorCause_requestDeniedByUser;
        break;
    case 6: rrc_gps_pos_error_ptr->errorReason = rrc_UE_Positioning_ErrorCause_notProcessedAndTimeout;
        break;
    case 7: rrc_gps_pos_error_ptr->errorReason = rrc_UE_Positioning_ErrorCause_referenceCellNotServingCell;
        break;
    default: break;/* no action */
  }
    
  if (lsm_gps_meas_ptr->addtl_assistance_data_incl)
  {
    RRC_MSG_COMMON_SET_BITMASK_IE_PTR(rrc_gps_pos_error_ptr,
        rrc_UE_Positioning_Error,ue_positioning_GPS_additionalAssistanceDataRequest);
    
    if (lsm_gps_meas_ptr->addtl_assistance_data.almanac_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.almanacRequest = TRUE;
    }
    
    if (lsm_gps_meas_ptr->addtl_assistance_data.utc_model_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.utcModelRequest = TRUE;
    }
    
    if (lsm_gps_meas_ptr->addtl_assistance_data.ionospheric_model_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.ionosphericModelRequest = TRUE;
    }
    
    if (lsm_gps_meas_ptr->addtl_assistance_data.nac_model_data_req_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navigationModelRequest = TRUE;
    }
    
    if (lsm_gps_meas_ptr->addtl_assistance_data.dgps_corrections_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.dgpsCorrectionsRequest = TRUE;
    }
    if (lsm_gps_meas_ptr->addtl_assistance_data.reference_location_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.referenceLocationRequest = TRUE;
    }
    if (lsm_gps_meas_ptr->addtl_assistance_data.reference_time_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.referenceTimeRequest = TRUE;
    }
    if (lsm_gps_meas_ptr->addtl_assistance_data.aquisition_assistance_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.aquisitionAssistanceRequest = TRUE;
    }
    if (lsm_gps_meas_ptr->addtl_assistance_data.real_time_integrity_request_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.realTimeIntegrityRequest = TRUE;
    }
    if (lsm_gps_meas_ptr->addtl_assistance_data.nac_model_data_req_incl == TRUE)
    {
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.m.navModelAddDataRequestPresent =1;
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navigationModelRequest = TRUE;
      
      /* GPS Week */
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navModelAddDataRequest.gps_Week =
        lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.gps_week;
      
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navModelAddDataRequest.gps_Toe =
        lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.gps_tow;
      
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navModelAddDataRequest.tToeLimit =
        lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.age_tolerance;
      
      rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.
        navModelAddDataRequest.satDataList.n = 0;
      
      
      if (lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites == 0)
      {
        return;
      }
      
      /* Assign head of list */
      sat_list_ptr = &rrc_gps_pos_error_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.
        navModelAddDataRequest.satDataList  ;
      sat_list_ptr->n =lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites;

      ALLOC_ASN1ARRAY2(&enc_ctxt,sat_list_ptr,rrc_SatData);
      
      for (cnt =0; cnt < lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites; cnt++)
      {
        sat_list_ptr->elem[cnt].satID = lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.sat_data[cnt].sat_id;
        
        sat_list_ptr->elem[cnt].iode = lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.sat_data[cnt].iode;
      }
    }
  }
}

#ifdef FEATURE_GANSS_SUPPORT
/*===========================================================================

FUNCTION    RRCGPS_SET_POS_ESTIMATE_R7

DESCRIPTION

 Sets Positioning Estimate received for GANSS feature from LSM in ASN1 format

DEPENDENCIES

 None.

RETURN VALUE

 void

SIDE EFFECTS

 None

===========================================================================*/

void rrcgps_set_pos_estimate_r7
(
  rrc_positioning_estimate_struct_type* lsm_pos_est_ptr,
  rrc_UE_Positioning_PositionEstimateInfo_v770ext_r7* rrc_gps_pos_est_ptr
)
{
  WRRC_MSG1_HIGH("GANSS: Positioning Estimate is %d",lsm_pos_est_ptr->positioning_estimate);
  switch (lsm_pos_est_ptr->positioning_estimate)
  {
    case GANSS_REFERENCE_TIME_ONLY:

     rrc_gps_pos_est_ptr->referenceTime.t = T_rrc_UE_Positioning_PositionEstimateInfo_v770ext_referenceTime_ganssReferenceTimeOnly;

     rrc_gps_pos_est_ptr->referenceTime.u.ganssReferenceTimeOnly = rtxMemAllocTypeZ(&enc_ctxt,rrc_GANSSReferenceTimeOnly);

     rrc_gps_pos_est_ptr->referenceTime.u.ganssReferenceTimeOnly->gANSS_tod = lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_tod;

      if(lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_timeId_incl == TRUE)
      {
        rrc_gps_pos_est_ptr->referenceTime.u.ganssReferenceTimeOnly->m.gANSS_timeIdPresent = 1;

        rrc_gps_pos_est_ptr->referenceTime.u.ganssReferenceTimeOnly->gANSS_timeId = 
              lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_timeId;
      }
      break;

    default: 

       break;
  } /* End of switch */
  
  /* POS Estimate structure. Vivek Added type rrc_gps_reference_loc_struct_type */
  
  WRRC_MSG1_HIGH("GANSS: Positioning Estimate Choice is %d",lsm_pos_est_ptr->pos_estimate_choice);
  switch (lsm_pos_est_ptr->pos_estimate_choice)
  {
  case ELLIPSOID_POINT:
    RRC_SET_COMMON_MSG_TYPE(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate_ellipsoidPoint);
     rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint =
       rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPoint);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
   
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitudeSign = rrc_EllipsoidPoint_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitudeSign = rrc_EllipsoidPoint_latitudeSign_south;
    }
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    break;
    
    
  case ELLIPSOID_POINT_UNCERTAIN_CIRCLE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointUncertCircle);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointUncertCircle);
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->latitudeSign = rrc_EllipsoidPointUncertCircle_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->latitudeSign = rrc_EllipsoidPointUncertCircle_latitudeSign_south;
    }
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->uncertaintyCode =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_altitude;
    
    
    break;
    
  case ELLIPSOID_POINT_UNCERTAIN_ELLIPSE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointUncertEllipse);

    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointUncertEllipse);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->confidence =
      lsm_pos_est_ptr->gps_ref_location.confidence;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->latitudeSign = rrc_EllipsoidPointUncertEllipse_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitudeSign = rrc_EllipsoidPointUncertEllipse_latitudeSign_south;
    }
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->orientationMajorAxis =
      lsm_pos_est_ptr->gps_ref_location.orientation_major_axis;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->uncertaintySemiMajor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->uncertaintySemiMinor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor;
    
    
    break;
    
  case ELLIPSOID_POINT_ALTITUDE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointAltitude);

    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointAltitude);
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->altitude =
      lsm_pos_est_ptr->gps_ref_location.altitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.altitude_direction == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->altitudeDirection = rrc_EllipsoidPointAltitude_altitudeDirection_height;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->altitudeDirection = rrc_EllipsoidPointAltitude_altitudeDirection_depth;
    }
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->latitudeSign = rrc_EllipsoidPointAltitude_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->latitudeSign = rrc_EllipsoidPointAltitude_latitudeSign_south;
    }
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    break;
    
  case ELLIPSOID_POINT_ALTITUDE_ELLIPSE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointAltitudeEllipse);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointAltitudeEllipsoide);
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->confidence =
      lsm_pos_est_ptr->gps_ref_location.confidence;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->latitudeSign = rrc_EllipsoidPointAltitudeEllipsoide_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->latitudeSign = rrc_EllipsoidPointAltitudeEllipsoide_latitudeSign_south;
    }
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->orientationMajorAxis =
      lsm_pos_est_ptr->gps_ref_location.orientation_major_axis;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->uncertaintySemiMajor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->uncertaintySemiMinor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->altitude =
      lsm_pos_est_ptr->gps_ref_location.altitude;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->uncertaintyAltitude =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_altitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.altitude_direction == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->altitudeDirection = rrc_EllipsoidPointAltitudeEllipsoide_altitudeDirection_height;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->altitudeDirection = rrc_EllipsoidPointAltitudeEllipsoide_altitudeDirection_depth;
    }
    
    break;
  default: break; 
  }

}

#endif

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
  rrc_UE_Positioning_PositionEstimateInfo* rrc_gps_pos_est_ptr
)
{
  switch (lsm_pos_est_ptr->positioning_estimate)
  {
  case CELL_TIMING_ONLY:
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->referenceTime,
        rrc_UE_Positioning_PositionEstimateInfo_referenceTime,cell_Timing);

    rrc_gps_pos_est_ptr->referenceTime.u.cell_Timing
        = rtxMemAllocTypeZ(&enc_ctxt,rrc_UE_Positioning_PositionEstimateInfo_cell_Timing);
    rrc_gps_pos_est_ptr->referenceTime.u.cell_Timing->modeSpecificInfo.t =
      T_rrc_UE_Positioning_PositionEstimateInfo_modeSpecificInfo_fdd;

    rrc_gps_pos_est_ptr->referenceTime.u.cell_Timing->modeSpecificInfo.u.fdd=
        rtxMemAllocTypeZ(&enc_ctxt, rrc_UE_Positioning_PositionEstimateInfo_fdd);
    rrc_gps_pos_est_ptr->referenceTime.u.cell_Timing->modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode =
      lsm_pos_est_ptr->u.cell_timing.psc;
    
    rrc_gps_pos_est_ptr->referenceTime.u.cell_Timing->sfn = lsm_pos_est_ptr->u.cell_timing.sfn;
    
    break;
    
  case GPS_REFERENCE_TIME_ONLY:
    RRC_SET_COMMON_MSG_TYPE(rrc_gps_pos_est_ptr->referenceTime,
      rrc_UE_Positioning_PositionEstimateInfo_referenceTime_gps_ReferenceTimeOnly);
    
    rrc_gps_pos_est_ptr->referenceTime.u.gps_ReferenceTimeOnly = lsm_pos_est_ptr->u.gps_reference_time;
    
    break;
    
  case UTRAN_GPS_REFERENCE_TIME:
    RRC_SET_COMMON_MSG_TYPE(rrc_gps_pos_est_ptr->referenceTime,
      rrc_UE_Positioning_PositionEstimateInfo_referenceTime_utran_GPSReferenceTimeResult);

    rrc_gps_pos_est_ptr->referenceTime.u.utran_GPSReferenceTimeResult
        = rtxMemAllocTypeZ(&enc_ctxt,rrc_UTRAN_GPSReferenceTimeResult);
    // TBD. Vivek gps_ref_psc_incl should be removed from interface
    if (lsm_pos_est_ptr->u.utran_gps_ref_time.gps_ref_psc_incl == TRUE)
    {
      
      RRC_SET_COMMON_MSG_TYPE(rrc_gps_pos_est_ptr->referenceTime.u.utran_GPSReferenceTimeResult->modeSpecificInfo,
        rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd);

      rrc_gps_pos_est_ptr->referenceTime.u.utran_GPSReferenceTimeResult->modeSpecificInfo.u.fdd
        = rtxMemAllocTypeZ(&enc_ctxt,rrc_UTRAN_GPSReferenceTimeResult_fdd);
      rrc_gps_pos_est_ptr->referenceTime.u.utran_GPSReferenceTimeResult->modeSpecificInfo.u.fdd->
        referenceIdentity.primaryScramblingCode = lsm_pos_est_ptr->u.utran_gps_ref_time.psc;
    }
    
    rrc_gps_pos_est_ptr->referenceTime.u.utran_GPSReferenceTimeResult->sfn = lsm_pos_est_ptr->u.utran_gps_ref_time.sfn;
    
    rrc_gps_pos_est_ptr->referenceTime.u.utran_GPSReferenceTimeResult->ue_GPSTimingOfCell.ls_part =
      lsm_pos_est_ptr->u.utran_gps_ref_time.lsb_timing;
    
    rrc_gps_pos_est_ptr->referenceTime.u.utran_GPSReferenceTimeResult->ue_GPSTimingOfCell.ms_part =
      lsm_pos_est_ptr->u.utran_gps_ref_time.msb_timing;
    
    break;
   default: break;
    
  } /* End of switch */
  
  /* POS Estimate structure. Vivek Added type rrc_gps_reference_loc_struct_type */
  
  
  switch (lsm_pos_est_ptr->pos_estimate_choice)
  {
  case ELLIPSOID_POINT:
    RRC_SET_COMMON_MSG_TYPE(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate_ellipsoidPoint);
     rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint =
       rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPoint);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
   
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitudeSign = rrc_EllipsoidPoint_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitudeSign = rrc_EllipsoidPoint_latitudeSign_south;
    }
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    break;
    
    
  case ELLIPSOID_POINT_UNCERTAIN_CIRCLE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointUncertCircle);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointUncertCircle);
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->latitudeSign = rrc_EllipsoidPointUncertCircle_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->latitudeSign = rrc_EllipsoidPointUncertCircle_latitudeSign_south;
    }
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertCircle->uncertaintyCode =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_altitude;
    
    
    break;
    
  case ELLIPSOID_POINT_UNCERTAIN_ELLIPSE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointUncertEllipse);

    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointUncertEllipse);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->confidence =
      lsm_pos_est_ptr->gps_ref_location.confidence;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->latitudeSign = rrc_EllipsoidPointUncertEllipse_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPoint->latitudeSign = rrc_EllipsoidPointUncertEllipse_latitudeSign_south;
    }
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->orientationMajorAxis =
      lsm_pos_est_ptr->gps_ref_location.orientation_major_axis;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->uncertaintySemiMajor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointUncertEllipse->uncertaintySemiMinor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor;
    
    
    break;
    
  case ELLIPSOID_POINT_ALTITUDE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointAltitude);

    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointAltitude);
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->altitude =
      lsm_pos_est_ptr->gps_ref_location.altitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.altitude_direction == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->altitudeDirection = rrc_EllipsoidPointAltitude_altitudeDirection_height;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->altitudeDirection = rrc_EllipsoidPointAltitude_altitudeDirection_depth;
    }
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->latitudeSign = rrc_EllipsoidPointAltitude_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->latitudeSign = rrc_EllipsoidPointAltitude_latitudeSign_south;
    }
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitude->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    break;
    
  case ELLIPSOID_POINT_ALTITUDE_ELLIPSE:
    
    RRC_SET_COMMON_MSG_TYPE2(rrc_gps_pos_est_ptr->positionEstimate,rrc_PositionEstimate,ellipsoidPointAltitudeEllipse);
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse
        =rtxMemAllocTypeZ(&enc_ctxt,rrc_EllipsoidPointAltitudeEllipsoide);
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->confidence =
      lsm_pos_est_ptr->gps_ref_location.confidence;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->latitude =
      lsm_pos_est_ptr->gps_ref_location.latitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.latitude_sign == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->latitudeSign = rrc_EllipsoidPointAltitudeEllipsoide_latitudeSign_north;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->latitudeSign = rrc_EllipsoidPointAltitudeEllipsoide_latitudeSign_south;
    }
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->longitude =
      lsm_pos_est_ptr->gps_ref_location.longitude;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->orientationMajorAxis =
      lsm_pos_est_ptr->gps_ref_location.orientation_major_axis;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->uncertaintySemiMajor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major;
    
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->uncertaintySemiMinor =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->altitude =
      lsm_pos_est_ptr->gps_ref_location.altitude;
    
    rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->uncertaintyAltitude =
      lsm_pos_est_ptr->gps_ref_location.uncertainity_altitude;
    
    if(lsm_pos_est_ptr->gps_ref_location.altitude_direction == 0)
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->altitudeDirection = rrc_EllipsoidPointAltitudeEllipsoide_altitudeDirection_height;
    }
    else
    {
      rrc_gps_pos_est_ptr->positionEstimate.u.ellipsoidPointAltitudeEllipse->altitudeDirection = rrc_EllipsoidPointAltitudeEllipsoide_altitudeDirection_depth;
    }
    
    break;
  default: break; 
  }

}

#ifdef FEATURE_CGPS_UMTS_CP_WCDMA



/*===========================================================================

FUNCTION
  OSS_BITSTRING_TO_INT 

DESCRIPTION
  This utility function converts the ossBitString type to an integer. 

  The ossBitString is a structure containing a length field in bits and a pointer to an 
  array of one or more bytes. The bytes are ordered from most significant to least significant.
  It is possible that the number of bits is not divisible by 8. If this is the case, the remaining
  bits ( in the last byte ) will reside in the upper bits of the byte.

PARAMETERS
  max_bytes - Maximum number of bytes that are available to hold result
  bitstring_failed - Pointer to a flag that indicates whether the bitstring conversion failed
  bitstring_ptr - Pointer to the OSS bitstring to be converted

RETURN VALUE
  Integer containing contents of OSS bitstring.	

===========================================================================*/
static uint32 rrcgps_oss_bitstring_to_int( uint8 max_bytes, boolean *bitstring_failed, ASN1BitStr32 *bitstring_ptr )
{
    uint32 result = 0;
    uint32 num_bits = (uint32) bitstring_ptr->numbits;
    uint8  *data_ptr = (uint8 *) bitstring_ptr->data;

    if ( num_bits > ((uint32)max_bytes * 8) )
    {
      /* More bits than can be held in the result, return an error */
      #ifndef WIN32
      WRRC_MSG0_ERROR("ERROR: OSS Bitstring exceeds maximum INT size!");
      #endif
      *bitstring_failed = TRUE;
      return (0);
    }
    
    /* While there are bits remaining, build the int */
    while( num_bits )
    {
      if ( num_bits >= 8 )
      {
        /* More than 8 bits remaining so shift the next byte in */
        result <<= 8;
            result |= *data_ptr++;
        num_bits -= 8;
      }
      else
      {
        /* Less than 8 bits remaining. Shift the remaining bits in */ 
        result <<= num_bits;
        result |= ( ( *data_ptr++ ) >> ( 8 - num_bits ) );
        num_bits = 0;
      }
    }

    return ( result );
}

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
boolean rrcgps_convert_assistance_data( 
  rrc_UE_Positioning_GPS_AssistanceData* asn1_ptr,
  rrc_assistance_data_struct_type* rrc_ptr )
{
  uint32 q_Cnt;
  uint8  sat_info_cnt = 0;
  boolean bs_failed = FALSE;

  /* Zero out RRC structure */
  memset( (void *) rrc_ptr, 0, sizeof( rrc_assistance_data_struct_type ) );

  /* Acq Assistance */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_AcquisitionAssistance))
  {
    rrc_UE_Positioning_GPS_AcquisitionAssistance *asn1_acq_assist = 
      &asn1_ptr->ue_positioning_GPS_AcquisitionAssistance;
    rrc_gps_acq_assistance_struct_type *rrc_acq_assist = 
      &rrc_ptr->gps_acq_assistance;

    struct rrc_AcquisitionSatInfoList *asn1_info_ptr;
    rrc_gps_sat_info_struct_type *rrc_info_ptr;

    rrc_ptr->gps_acq_assistance_incl = TRUE;
    rrc_acq_assist->gps_reference_time = asn1_acq_assist->gps_ReferenceTime;

    /* GPS - UTRAN Reference Time */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_acq_assist,rrc_UE_Positioning_GPS_AcquisitionAssistance,utran_GPSReferenceTime))
    {
      rrc_acq_assist->gps_umts_reference_time_incl = TRUE;
      rrc_acq_assist->gps_umts_reference_time.msb_timing =
        asn1_acq_assist->utran_GPSReferenceTime.utran_GPSTimingOfCell.ms_part;
      rrc_acq_assist->gps_umts_reference_time.lsb_timing =
        asn1_acq_assist->utran_GPSReferenceTime.utran_GPSTimingOfCell.ls_part;
      rrc_acq_assist->gps_umts_reference_time.sfn =
        asn1_acq_assist->utran_GPSReferenceTime.sfn;

      /* Primary Scrambling Code */
      if ( RRC_MSG_COMMON_BITMASK_IE(asn1_acq_assist->utran_GPSReferenceTime,rrc_UTRAN_GPSReferenceTime,modeSpecificInfo) )
      {
        rrc_acq_assist->gps_umts_reference_time.gps_ref_psc_incl = TRUE;

        if(asn1_acq_assist->utran_GPSReferenceTime.modeSpecificInfo.t == T_rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd)
        {
          /* Only FDD mode supported in UE, grab primary scrambling code */
          rrc_acq_assist->gps_umts_reference_time.psc =
            asn1_acq_assist->utran_GPSReferenceTime.modeSpecificInfo.u.fdd->referenceIdentity.primaryScramblingCode;
        }
        else
        {
          WRRC_MSG0_ERROR("TDD info given in MCM. Send failure.");
          return FALSE;
        }
      }
    }

    /* Traverse Satellite Info List */
    sat_info_cnt = 0;
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_AcquisitionAssistance.satelliteInformationList;
    rrc_info_ptr = &rrc_ptr->gps_acq_assistance.gps_sat_info[ 0 ];

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& 
        (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if( asn1_info_ptr == NULL )
        break;

      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->doppler = asn1_info_ptr->elem[q_Cnt].doppler0thOrder;

          /* Extra Doppler Info */
      if ( asn1_info_ptr->elem[q_Cnt].m.extraDopplerInfoPresent )
      {
        rrc_info_ptr->extra_doppler_info_incl = TRUE;
        rrc_info_ptr->extra_doppler_info.doppler_first_order =
          asn1_info_ptr->elem[q_Cnt].extraDopplerInfo.doppler1stOrder;
        rrc_info_ptr->extra_doppler_info.Doppler_uncertainity =
          asn1_info_ptr->elem[q_Cnt].extraDopplerInfo.dopplerUncertainty;
      }

      rrc_info_ptr->code_phase = asn1_info_ptr->elem[q_Cnt].codePhase;
      rrc_info_ptr->integer_code_phase = asn1_info_ptr->elem[q_Cnt].integerCodePhase;
      rrc_info_ptr->gps_bit_number = asn1_info_ptr->elem[q_Cnt].gps_BitNumber;
      rrc_info_ptr->search_window = asn1_info_ptr->elem[q_Cnt].codePhaseSearchWindow;

          /* Azimuth and Elevation */
      if ( asn1_info_ptr->elem[q_Cnt].m.azimuthAndElevationPresent )
      {
        rrc_info_ptr->azimuth_elevation_incl = TRUE;
        rrc_info_ptr->azimuth_elevation_info.azimuth =
          asn1_info_ptr->elem[q_Cnt].azimuthAndElevation.azimuth;
        rrc_info_ptr->azimuth_elevation_info.elevation =
          asn1_info_ptr->elem[q_Cnt].azimuthAndElevation.elevation;
      }

          /* Update sat info ptr with next element in the list */
      
      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_acq_assist->no_of_gps_sat = sat_info_cnt;

  }
  
  /* Almanac */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_Almanac))
  {
    rrc_UE_Positioning_GPS_Almanac *asn1_almanac =
      &asn1_ptr->ue_positioning_GPS_Almanac;
    rrc_gps_almanac_struct_type *rrc_almanac =
      &rrc_ptr->gps_almanc;

    rrc_AlmanacSatInfoList *asn1_info_ptr;
    rrc_almanac_sat_info_struct_type *rrc_info_ptr; 

    rrc_ptr->gps_almanac_incl = TRUE;
    rrc_almanac->wn_a = 
      ( uint8 )rrcgps_oss_bitstring_to_int( sizeof( uint8 ), &bs_failed, &asn1_almanac->wn_a );

    /* Global Health */
    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(asn1_almanac,sv_GlobalHealth))
    {
      uint16 num_bits = asn1_almanac->sv_GlobalHealth.numbits;
      uint8 num_bytes; 
      uint8 *dest_ptr = (uint8 *)&rrc_almanac->global_health[0];
      uint8 *src_ptr = (uint8 *)asn1_almanac->sv_GlobalHealth.data;

      /* This is a special case where we can not use rrcgps_oss_bitstring_to_int(). The Global
         Health could be hundreds of bytes long, therefore we need to handle this data 
         in this function */

      num_bytes = num_bits / 8;

      /* Determine if we need an extra byte for any straggler bits */
      if ( num_bits % 8 )
        num_bytes++;

      /* Copy global health data */
      if ( num_bytes <= GLOBAL_HEALTH_ARRAY_SIZE )
      {
        rrc_almanac->global_health_incl = TRUE;
        rrc_almanac->global_health_num_bits = num_bits;
      
        /* Copy global health bits */
        for ( q_Cnt = num_bytes; q_Cnt; q_Cnt-- )
        {
              *dest_ptr++ = *src_ptr++;
        }
      }
    }

    /* Traverse Almanac List */
     
    sat_info_cnt = 0;
    asn1_info_ptr = &asn1_almanac->almanacSatInfoList;
    rrc_info_ptr = &rrc_almanac->almanac_sat_info_list[0];

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& 
        (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if ( asn1_info_ptr == NULL )
        break;

      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->data_id = (uint8) asn1_info_ptr->elem[q_Cnt].dataID;

      rrc_info_ptr->a_Sqrt = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].a_Sqrt );
      rrc_info_ptr->deltaI = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].deltaI );
      rrc_info_ptr->e = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].e );
      rrc_info_ptr->m0 = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].m0 );
      rrc_info_ptr->omega0 = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omega0 );
      rrc_info_ptr->omega_dot = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omegaDot );
      rrc_info_ptr->omega =
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omega );
      rrc_info_ptr->sat_health = 
        (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].satHealth );
      rrc_info_ptr->t_oa =
        (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].t_oa );
      rrc_info_ptr->af0 =
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].af0 );
      rrc_info_ptr->af1 =
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].af1 );

      
      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_almanc.no_of_almanac_sat = sat_info_cnt;
  }

  /* Nav Model */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_NavigationModel))
  {
    rrc_navigation_model_sat_info_list_struct_type *rrc_info_ptr;
    rrc_NavigationModelSatInfoList *asn1_info_ptr;

    rrc_ptr->gps_navigation_model_incl = TRUE;

    /* Traverse NavModel List */
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_NavigationModel.navigationModelSatInfoList;
    rrc_info_ptr = &rrc_ptr->gps_navigation_model.satellite_info[0];
    sat_info_cnt = 0;

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& 
        (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
        /* Check to see if we have reached the end of the linked list */
        if ( asn1_info_ptr == NULL )
          break;

      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->satellite_status = asn1_info_ptr->elem[q_Cnt].satelliteStatus;

      /* Ephemeris Parameters */
      if ( asn1_info_ptr->elem[q_Cnt].m.ephemerisParameterPresent )
      {
        rrc_info_ptr->ephermeral_clock_params_incl = TRUE;
        rrc_info_ptr->ephermeral_clock_params.a_Sqrt =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.a_Sqrt );
        rrc_info_ptr->ephermeral_clock_params.af0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af0 );
        rrc_info_ptr->ephermeral_clock_params.af1 = 
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af1 );
        rrc_info_ptr->ephermeral_clock_params.af2 =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af2 );
        rrc_info_ptr->ephermeral_clock_params.aodo =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.aodo );
        rrc_info_ptr->ephermeral_clock_params.c_ic =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_ic );
        rrc_info_ptr->ephermeral_clock_params.c_is =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_is );
        rrc_info_ptr->ephermeral_clock_params.c_rc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_rc );
        rrc_info_ptr->ephermeral_clock_params.c_rs =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_rs );
        rrc_info_ptr->ephermeral_clock_params.c_uc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_uc );
        rrc_info_ptr->ephermeral_clock_params.c_us =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_us );
        rrc_info_ptr->ephermeral_clock_params.code_on_l2 =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.codeOnL2 );
        rrc_info_ptr->ephermeral_clock_params.delta_n =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.delta_n );
        rrc_info_ptr->ephermeral_clock_params.e =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.e );
        rrc_info_ptr->ephermeral_clock_params.fit_interval =
            (boolean) rrcgps_oss_bitstring_to_int( sizeof(boolean), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.fitInterval );
        rrc_info_ptr->ephermeral_clock_params.i0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.i0 );
        rrc_info_ptr->ephermeral_clock_params.iDot =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.iDot );
            rrc_info_ptr->ephermeral_clock_params.iodc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.iodc );
        rrc_info_ptr->ephermeral_clock_params.l2_pflag =
            (boolean) rrcgps_oss_bitstring_to_int( sizeof(boolean), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.l2Pflag );
        rrc_info_ptr->ephermeral_clock_params.m0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.m0 );
        rrc_info_ptr->ephermeral_clock_params.omega =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omega );
        rrc_info_ptr->ephermeral_clock_params.omega0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omega0 );
        rrc_info_ptr->ephermeral_clock_params.omegaDot =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omegaDot );
        rrc_info_ptr->ephermeral_clock_params.sat_health =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.satHealth );
        rrc_info_ptr->ephermeral_clock_params.t_gd =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_GD );
        rrc_info_ptr->ephermeral_clock_params.t_oc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_oc );
        rrc_info_ptr->ephermeral_clock_params.t_oe =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_oe );
        rrc_info_ptr->ephermeral_clock_params.ura_index =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.uraIndex );
      }

      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_navigation_model.no_of_satellites = sat_info_cnt;

  }

  /* Real Time Integrity */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_Real_timeIntegrity) )
  {
    rrc_BadSatList *asn1_info_ptr;
    uint8 *bad_sat_ptr;

    rrc_ptr->gps_real_time_integrity_incl = TRUE;

    /* Fill in RTI values */
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_Real_timeIntegrity;
    bad_sat_ptr = (uint8 *)&rrc_ptr->gps_rti.satellite_id[0];

    sat_info_cnt = 0;

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& 
        (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if ( asn1_info_ptr == NULL )
        break;

      *bad_sat_ptr = (uint8) asn1_info_ptr->elem[q_Cnt];

      bad_sat_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_rti.no_of_satellites = sat_info_cnt;
  }
    
  /* Reference Location */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_ReferenceLocation) )
  {
    rrc_gps_reference_loc_struct_type *rrc_ref_loc =
      &rrc_ptr->gps_ref_loc;
    rrc_EllipsoidPointAltitudeEllipsoide *asn1_ref_loc =
      &asn1_ptr->ue_positioning_GPS_ReferenceLocation.ellipsoidPointAltitudeEllipsoide;

    rrc_ptr->gps_ref_loc_incl = TRUE;
    rrc_ref_loc->altitude = asn1_ref_loc->altitude;
    rrc_ref_loc->altitude_direction = asn1_ref_loc->altitudeDirection;
    rrc_ref_loc->confidence = asn1_ref_loc->confidence;
    rrc_ref_loc->latitude = asn1_ref_loc->latitude;
    rrc_ref_loc->latitude_sign = asn1_ref_loc->latitudeSign;
    rrc_ref_loc->longitude = asn1_ref_loc->longitude;
    rrc_ref_loc->orientation_major_axis = (uint8) asn1_ref_loc->orientationMajorAxis;
    rrc_ref_loc->uncertainity_altitude = asn1_ref_loc->uncertaintyAltitude;
    rrc_ref_loc->uncertainity_semi_major = (uint8) asn1_ref_loc->uncertaintySemiMajor;
    rrc_ref_loc->uncertainity_semi_minor = (uint8) asn1_ref_loc->uncertaintySemiMinor;
  }

  /* Reference Time */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_ReferenceTime))
  {
    rrc_gps_reference_time_struct_type *rrc_ref_time =
      &rrc_ptr->gps_ref_time;
    rrc_UE_Positioning_GPS_ReferenceTime *asn1_ref_time =
     &asn1_ptr->ue_positioning_GPS_ReferenceTime;

    rrc_ptr->gps_ref_time_incl = TRUE;
    rrc_ref_time->time = asn1_ref_time->gps_tow_1msec;
    rrc_ref_time->week = asn1_ref_time->gps_Week;

    /* GPS Drift Rate */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, 
        rrc_UE_Positioning_GPS_ReferenceTime,utran_GPS_DriftRate) )
    {
      rrc_ref_time->gps_drift_rate_incl = TRUE;
      rrc_ref_time->gps_drift_rate =
      asn1_ref_time->utran_GPS_DriftRate;
    }

    /* GPS - UTRAN reference time */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, 
        rrc_UE_Positioning_GPS_ReferenceTime,utran_GPSReferenceTime))
    {
      rrc_ref_time->gps_utran_reference_time_incl = TRUE;
      rrc_ref_time->gps_utran_ref_time.lsb_timing =
        asn1_ref_time->utran_GPSReferenceTime.utran_GPSTimingOfCell.ls_part;
      rrc_ref_time->gps_utran_ref_time.msb_timing =
        asn1_ref_time->utran_GPSReferenceTime.utran_GPSTimingOfCell.ms_part;
      rrc_ref_time->gps_utran_ref_time.sfn =
        asn1_ref_time->utran_GPSReferenceTime.sfn;

      /* Primary Scrambling Code */ 
      if (RRC_MSG_COMMON_BITMASK_IE( asn1_ref_time->utran_GPSReferenceTime,
        rrc_UTRAN_GPSReferenceTime,modeSpecificInfo))
      {
        rrc_ref_time->gps_utran_ref_time.gps_ref_psc_incl = TRUE;

        if(asn1_ref_time->utran_GPSReferenceTime.modeSpecificInfo.t == T_rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd)
        {
          /* UE Only supports FDD, grab primary scrambling code */
          rrc_ref_time->gps_utran_ref_time.psc =
          asn1_ref_time->utran_GPSReferenceTime.modeSpecificInfo.u.fdd->referenceIdentity.primaryScramblingCode;
        }
        else
        {
          WRRC_MSG0_ERROR("TDD info given in MCM. Send failure.");
          return FALSE;
        }
      }
    }

    /* SFN TOW Uncertainty */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime,
        sfn_tow_Uncertainty) )
    {
      rrc_ref_time->sfn_tow_uncertainity_incl = TRUE;
      rrc_ref_time->sfn_tow_uncertainity = asn1_ref_time->sfn_tow_Uncertainty;
    }

    /* GPS TOW Assistance List */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime,gps_TOW_AssistList))
    {
      rrc_GPS_TOW_AssistList *asn1_info_ptr;
      rrc_gps_tow_assist_struct_type *rrc_info_ptr;

      asn1_info_ptr = &asn1_ref_time->gps_TOW_AssistList;
      rrc_info_ptr = &rrc_ref_time->gps_tow_assist[0];

      /* Traverse TOW Assist List */
      sat_info_cnt = 0;
      for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
      {
        /* Check to see if we have reached the end of the linked list */
        if ( asn1_info_ptr == NULL )
          break;

        rrc_info_ptr->satellite_identity = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
        rrc_info_ptr->tlm_msg = 
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].tlm_Message );
        rrc_info_ptr->tlm_rsvd = 
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].tlm_Reserved );
        rrc_info_ptr->alert = asn1_info_ptr->elem[q_Cnt].alert;
        rrc_info_ptr->anti_spoof = asn1_info_ptr->elem[q_Cnt].antiSpoof;

        rrc_info_ptr++;
        sat_info_cnt++;                                                       
      }

      rrc_ref_time->no_of_gps_tow_assist = sat_info_cnt;
    }
  }

  /* Reference Cell Info */

  /* DGPS Corrections - Not currently supported by RRC layer */

  /* UTC Model - Not currently supported by RRC layer */

  /* IONO Model */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_IonosphericModel))
  {
    rrc_gps_iono_struct_type *rrc_iono =
      &rrc_ptr->gps_iono;

    rrc_UE_Positioning_GPS_IonosphericModel *asn1_iono = 
      &asn1_ptr->ue_positioning_GPS_IonosphericModel;

    rrc_ptr->gps_iono_incl = TRUE;

    rrc_iono->alfa0 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha0 );
    rrc_iono->alfa1 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha1 );
    rrc_iono->alfa2 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha2 );
    rrc_iono->alfa3 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha3 );
    rrc_iono->beta0 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta0 );
    rrc_iono->beta1 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta1 );
    rrc_iono->beta2 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta2 );
    rrc_iono->beta3 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta3 );
  }

  /** UTC model ***/
  if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData,ue_positioning_GPS_UTC_Model))
  {
    rrc_gps_utc_struct_type *rrc_utc =
      &rrc_ptr->gps_utc_model;

    rrc_UE_Positioning_GPS_UTC_Model *asn1_utc = 
      &asn1_ptr->ue_positioning_GPS_UTC_Model;

    rrc_ptr->gps_utc_model_incl = TRUE;

    rrc_utc->q_A0   = (uint32)rrcgps_oss_bitstring_to_int(sizeof(uint32), &bs_failed, &asn1_utc->a0);
    rrc_utc->q_A1 =   (uint32)rrcgps_oss_bitstring_to_int(sizeof(uint32), &bs_failed, &asn1_utc->a1);
    rrc_utc->u_DeltaTls = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->delta_t_LS );
    rrc_utc->u_DeltaTlsf = (uint8)rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->delta_t_LSF);
    rrc_utc->u_DN = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->dn );
    rrc_utc->u_Tot = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->t_ot );
    rrc_utc->u_WNlsf = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->wn_lsf );
    rrc_utc->u_WNt = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->wn_t );

  } 

  /* If any of the bitstring conversions failed, indicate this to caller by returning
     false. */
  if ( bs_failed )
  {
    #ifndef WIN32
    WRRC_MSG0_ERROR("ASN.1 to RRC structure conversion encountered errors");
    #endif
    return( FALSE );
  }
  else
    return( TRUE );
}




#endif /* FEATURE_CGPS_UMTS_CP_WCDMA */

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
)
{
  uint8 trans_id = 0;
  
  uint8 error_choice = 0;
  
  rrc_meas_params_struct_type *lsm_assistance_data;

  /* Declare ptr to store message received from UTRAN */
  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;
  
  /* Declare ptr to point to actual IE's in Meas Ctrl Msg */
  rrc_AssistanceDataDelivery *assistance_data_delievery_ptr = NULL;
  
  lsm_assistance_data = &temp_gps;
  memset(lsm_assistance_data,0,sizeof(rrc_meas_params_struct_type));
  lsm_assistance_data->params_type = (uint8)RRC_ASSISTANCE_DATA_PARMS;
  
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
    downlink_sdu_ind.decoded_msg;
  
  if ( dcch_msg_ptr == NULL )
  {
    WRRC_MSG0_ERROR("POS:ASN1 decoding failed");
    
    /* Transaction ID already set to 0 */
    
    error_choice = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    
    rrcmeas_send_rrc_status_msg(&trans_id, &error_choice);
    
    return;
  }
  
  if (!(RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.assistanceDataDelivery,
    rrc_AssistanceDataDelivery_r3 )))
  {
    WRRC_MSG0_ERROR("POS:Critical extensions not supported");
    
    error_choice = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    
    /* Transaction ID already set to 0. Send Failure Msg */
    rrcmeas_send_rrc_status_msg(&trans_id, &error_choice);
    return ;
  }
  
  /* Get the pointer to the actual MC message */
  assistance_data_delievery_ptr = &(dcch_msg_ptr->message.u.assistanceDataDelivery);
  
  trans_id = assistance_data_delievery_ptr->u.r3.assistanceDataDelivery_r3.rrc_TransactionIdentifier;
  if(assistance_data_delievery_ptr->u.r3.assistanceDataDelivery_r3.m.ue_positioning_GPS_AssistanceDataPresent  )
  {
  /* No need to store it in transaction table */
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
  if (rrcgps_convert_assistance_data(&assistance_data_delievery_ptr->u.r3.assistanceDataDelivery_r3.
    ue_positioning_GPS_AssistanceData,
    &(lsm_assistance_data->u.assistance_data_params)) == FALSE)
  #else /* this will only be FEATURE_MMGPS */ 
  if (lsm_convert_assistance_data(&assistance_data_delievery_ptr->u.r3.assistanceDataDelivery_r3.
    ue_positioning_GPS_AssistanceData,
    &(lsm_assistance_data->u.assistance_data_params)) == FALSE)
  #endif
  {
    WRRC_MSG0_ERROR("POS: Assistance Data conversion failed");
    
    error_choice = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
    
    /* Send Failure Message */
    rrcmeas_send_rrc_status_msg(&trans_id, &error_choice);
    
    return;
  }
  }
#ifdef FEATURE_GANSS_SUPPORT
  else if(rrc_ganss_supported)
  {
    rrcgps_fill_lsm_assistance_data(dcch_msg_ptr,lsm_assistance_data);
  }
#endif

  else
  {
    WRRC_MSG0_ERROR("POS: Assistance Data not present. Ignoring ADD msg.");
    return;
  }
  
  WRRC_MSG0_HIGH("POS: Sending Assistance Meas Request");
  rrcgps_send_lsm_cmd(lsm_assistance_data);
  
  return;
}


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
)
{
  uint8 cnt = 0;
  
  if (lsm_gps_meas_results_ptr->positioning_estimate == (uint8)GPS_REFERENCE_TIME_ONLY )
  {
    /* TBD Check to be added */
    // lsm_gps_meas_results_ptr->u.gps_reference_time;
  }
  else
  {
    if (lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.gps_ref_psc_incl == TRUE)
    {
      if (lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.psc > PSC_MAX)
      {
        WRRC_MSG1_ERROR("POS: PSC %d invalid 0..511",lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.psc);
        return FALSE;
      }
    }
    
    /* SFN */
    if (lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.sfn > MAX_SFN)
    {
      WRRC_MSG1_ERROR("POS: SFN %d invalid 0..4095",
        lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.sfn);
      return FALSE;
    }
    
    if (lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.msb_timing > GPS_MSB_TIMING)
    {
      WRRC_MSG1_ERROR("POS:GPS MSB timing %d invalid 0..16383",
        lsm_gps_meas_results_ptr->u.gps_umts_meas_results.gps_umts_reference_time.msb_timing);
      return FALSE;
    }
    
  } /* End of else of utran_GPSReferenceTimeResults */
  
  /* Now set measured params */
  if ((lsm_gps_meas_results_ptr->no_of_satellites < 1) || (lsm_gps_meas_results_ptr->no_of_satellites > MAX_NO_OF_SATELLITES))
  {
    WRRC_MSG1_ERROR("POS: No of satellites %d invalid 1..16",
      lsm_gps_meas_results_ptr->no_of_satellites);
    return FALSE;
  }
  
  for (cnt =0; cnt < lsm_gps_meas_results_ptr->no_of_satellites; cnt++)
  {
    if (lsm_gps_meas_results_ptr->gps_measured_params[cnt].c_no > MAX_C_NO)
    {
      WRRC_MSG1_ERROR("POS:C_NO %d invalid 0..63",
        lsm_gps_meas_results_ptr->gps_measured_params[cnt].c_no);
      return FALSE;
      
    }
    
    if ((lsm_gps_meas_results_ptr->gps_measured_params[cnt].doppler > MAX_DOPPLER) ||
      (lsm_gps_meas_results_ptr->gps_measured_params[cnt].doppler < MIN_DOPPLER))
    {
      WRRC_MSG1_ERROR("POS: Doppler %d invalid -32768...32768",
        lsm_gps_meas_results_ptr->gps_measured_params[cnt].doppler);
      return FALSE;
    }
    

    if (lsm_gps_meas_results_ptr->gps_measured_params[cnt].fractional_gps_chips > MAX_FRAC_GPS_CHIPS)
    {
      WRRC_MSG1_ERROR("POS: Fractional GPS Chips %d invalid ",
      lsm_gps_meas_results_ptr->gps_measured_params[cnt].fractional_gps_chips);
      return FALSE;
    }
    
    if (lsm_gps_meas_results_ptr->gps_measured_params[cnt].multipath_ind > MULTIPATH_VALUE)
    {
      WRRC_MSG1_ERROR("POS:Multipath %d invalid 0..3",
        lsm_gps_meas_results_ptr->gps_measured_params[cnt].multipath_ind);
      return FALSE;
    }
    
    if (lsm_gps_meas_results_ptr->gps_measured_params[cnt].rms_error > MAX_RMS_ERROR)
    {
      WRRC_MSG1_ERROR("POS:RMS Error Value %d invalid 0..63",
        lsm_gps_meas_results_ptr->gps_measured_params[cnt].rms_error);
      return FALSE;
    }
    
    if (lsm_gps_meas_results_ptr->gps_measured_params[cnt].sat_id > MAX_SAT_ID)
    {
      WRRC_MSG1_ERROR("POS:Satellite Id %d invalid 0..63",
        lsm_gps_meas_results_ptr->gps_measured_params[cnt].sat_id);
      return FALSE;
    }
    
    if (lsm_gps_meas_results_ptr->gps_measured_params[cnt].whole_gps_chips > MAX_GPS_CHIPS)
    {
      WRRC_MSG1_ERROR("POS:Max GPS Chips %d invalid 0..1022",
        lsm_gps_meas_results_ptr->gps_measured_params[cnt].whole_gps_chips);
      return FALSE;
    }
  }
  return TRUE;
}


/*===========================================================================

FUNCTION    RRCGPS_VALIDATE_POS_ESTIMATE_CHOICE

DESCRIPTION

 Validates Positioning Estimate received from LSM

DEPENDENCIES

 None.

RETURN VALUE

 True/False

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcgps_validate_pos_estimate_choice
(
  rrc_positioning_estimate_struct_type* lsm_pos_est_ptr
)
{
  switch (lsm_pos_est_ptr->pos_estimate_choice)
  {
  case ELLIPSOID_POINT:
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude > MAX_LATITUDE)
    {
      WRRC_MSG1_ERROR("POS:Latitude %d invalid 0..8388607",lsm_pos_est_ptr->gps_ref_location.latitude);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude_sign > MAX_LATITUDE_SIGN)
    {
      WRRC_MSG1_ERROR("POS:Latitude Sign %d invalid 0..1",lsm_pos_est_ptr->gps_ref_location.latitude_sign);
      return FALSE;
    }
    
    
    if ((lsm_pos_est_ptr->gps_ref_location.longitude < MIN_LONGITUDE) ||
      (lsm_pos_est_ptr->gps_ref_location.longitude > MAX_LONGITUDE))
    {
      WRRC_MSG1_ERROR("POS:Longitude %d invalid -8388608 .. 83886080",lsm_pos_est_ptr->gps_ref_location.longitude);
      return FALSE;
    }
    
    break;
    
    
  case ELLIPSOID_POINT_UNCERTAIN_CIRCLE:
  case ELLIPSOID_POINT_ALTITUDE:
    
    if (lsm_pos_est_ptr->gps_ref_location.altitude > MAX_ALTITUDE)
    {
      WRRC_MSG1_ERROR("POS:Altitude %d invalid 0..32767",lsm_pos_est_ptr->gps_ref_location.altitude);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.altitude_direction > MAX_ALTITUDE_DIRECTION)
    {
      WRRC_MSG1_ERROR("POS:Altitude direction %d invalid 0..1",lsm_pos_est_ptr->gps_ref_location.altitude_direction);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude > MAX_LATITUDE)
    {
      WRRC_MSG1_ERROR("POS:Latitude %d invalid 0..8388607",lsm_pos_est_ptr->gps_ref_location.latitude);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude_sign > MAX_LATITUDE_SIGN)
    {
      WRRC_MSG1_ERROR("POS:Latitude Sign %d invalid 0..1",lsm_pos_est_ptr->gps_ref_location.latitude_sign);
      return FALSE;
    }
    
    
    if ((lsm_pos_est_ptr->gps_ref_location.longitude < MIN_LONGITUDE) ||
      (lsm_pos_est_ptr->gps_ref_location.longitude > MAX_LONGITUDE))
    {
      WRRC_MSG1_ERROR("POS:Longitude %d invalid -8388608 .. 83886080",lsm_pos_est_ptr->gps_ref_location.longitude);
      return FALSE;
    }
    
    break;
    
  case ELLIPSOID_POINT_ALTITUDE_ELLIPSE:
    if (lsm_pos_est_ptr->gps_ref_location.confidence > MAX_CONFIDENCE)
    {
      WRRC_MSG1_ERROR("POS:Confidence %d invalid 0..100",lsm_pos_est_ptr->gps_ref_location.confidence);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude > MAX_LATITUDE)
    {
      WRRC_MSG1_ERROR("POS:Latitude %d invalid 0..8388607",lsm_pos_est_ptr->gps_ref_location.latitude);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude_sign > MAX_LATITUDE_SIGN)
    {
      WRRC_MSG1_ERROR("POS:Latitude %d invalid 0..8388607",lsm_pos_est_ptr->gps_ref_location.latitude_sign);
      return FALSE;
    }
    
    if ((lsm_pos_est_ptr->gps_ref_location.longitude < MIN_LONGITUDE) ||
      (lsm_pos_est_ptr->gps_ref_location.longitude > MAX_LONGITUDE))
    {
      WRRC_MSG1_ERROR("POS:Longitude %d invalid -8388608 .. 83886080",lsm_pos_est_ptr->gps_ref_location.longitude);
      return FALSE;
    }
    
    
    if (lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major > UNCERTAINITY_SEMI_MAJOR)
    {
      WRRC_MSG1_ERROR("POS:UncertSemiMajor %d invalid 0..127",lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major);
      return FALSE;
    }
    
    
    if (lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor > UNCERTAINITY_SEMI_MINOR)
    {
      WRRC_MSG1_ERROR("POS:UncertSemiMinor %d invalid 0..127",lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.orientation_major_axis > ORIENTATION_MAJOR_AXIS)
    {
      WRRC_MSG1_ERROR("POS:OrientationMajorAxis %d invalid 0..89",lsm_pos_est_ptr->gps_ref_location.orientation_major_axis);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.altitude > MAX_ALTITUDE)
    {
      WRRC_MSG1_ERROR("POS:Altitude %d invalid 0..32767",lsm_pos_est_ptr->gps_ref_location.altitude);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.uncertainity_altitude > MAX_UNCERTAINITY_ALTITUDE)
    {
      WRRC_MSG1_ERROR("POS:Uncert.Altitude %d invalid 0..127",lsm_pos_est_ptr->gps_ref_location.uncertainity_altitude);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.altitude_direction > MAX_ALTITUDE_DIRECTION)
    {
      WRRC_MSG1_ERROR("POS:Max Altitude dir %d invalid 0..1",lsm_pos_est_ptr->gps_ref_location.altitude_direction);
      return FALSE;
    }
    
    break;
    
  case ELLIPSOID_POINT_UNCERTAIN_ELLIPSE:
    
    if (lsm_pos_est_ptr->gps_ref_location.confidence > MAX_CONFIDENCE)
    {
      WRRC_MSG1_ERROR("POS:Confidence %d invalid 0..100",lsm_pos_est_ptr->gps_ref_location.confidence);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude > MAX_LATITUDE)
    {
      WRRC_MSG1_ERROR("POS:Latitude %d invalid 0..8388607",lsm_pos_est_ptr->gps_ref_location.latitude);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.latitude_sign > MAX_LATITUDE_SIGN)
    {
      WRRC_MSG1_ERROR("POS:Latitude %d invalid 0..8388607",lsm_pos_est_ptr->gps_ref_location.latitude_sign);
      return FALSE;
    }
    
    if ((lsm_pos_est_ptr->gps_ref_location.longitude < MIN_LONGITUDE) ||
      (lsm_pos_est_ptr->gps_ref_location.longitude > MAX_LONGITUDE))
    {
      WRRC_MSG1_ERROR("POS:Longitude %d invalid -8388608 .. 83886080",lsm_pos_est_ptr->gps_ref_location.longitude);
      return FALSE;
    }
    
    
    if (lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major > UNCERTAINITY_SEMI_MAJOR)
    {
      WRRC_MSG1_ERROR("POS:UncertSemiMajor %d invalid 0..127",lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_major);
      return FALSE;
    }
    
    
    if (lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor > UNCERTAINITY_SEMI_MINOR)
    {
      WRRC_MSG1_ERROR("POS:UncertSemiMinor %d invalid 0..127",lsm_pos_est_ptr->gps_ref_location.uncertainity_semi_minor);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->gps_ref_location.orientation_major_axis > ORIENTATION_MAJOR_AXIS)
    {
      WRRC_MSG1_ERROR("POS:OrientationMajorAxis %d invalid 0..89",lsm_pos_est_ptr->gps_ref_location.orientation_major_axis);
      return FALSE;
    }
    break;
    
  default:
    WRRC_MSG1_ERROR("POS:Estimate Choice %d invalid", lsm_pos_est_ptr->pos_estimate_choice);
    return FALSE;
  }
  return TRUE;
}

#ifdef FEATURE_GANSS_SUPPORT
/*===========================================================================

FUNCTION    RRCGPS_VALIDATE_POS_ESTIMATE_R7

DESCRIPTION

 Validates Positioning Estimate for GANSS feature received from LSM

DEPENDENCIES

 None.

RETURN VALUE

 True/False

SIDE EFFECTS

 None

===========================================================================*/

boolean rrcgps_validate_pos_estimate_r7
(
  rrc_positioning_estimate_struct_type* lsm_pos_est_ptr
)

{
  switch (lsm_pos_est_ptr->positioning_estimate)
  {
   
    case GANSS_REFERENCE_TIME_ONLY:

      if( lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_tod > MAX_GANSS_TOD)
      {
        WRRC_MSG1_ERROR("POS: ganss tod %d invalid 0..3599999", lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_tod);
        return FALSE;  
      }

      if(lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_timeId_incl == TRUE)
      {
        if(lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_timeId > MAX_GANSS_TIME_ID)
        {
          WRRC_MSG1_ERROR("POS: ganss time id %d invalid 0..7", lsm_pos_est_ptr->u.ganss_ReferenceTimeOnly.gANSS_timeId);
           return FALSE;
        }
      }

      break;

      default:

        WRRC_MSG1_ERROR("POS:  Pos Estimate Choice", lsm_pos_est_ptr->positioning_estimate);
        //return FALSE;
    }

    if( rrcgps_validate_pos_estimate_choice(lsm_pos_est_ptr) == FALSE)
    {
      return FALSE;
    }
  if(lsm_pos_est_ptr->velocityEstimate_incl == TRUE)
  {
    switch(lsm_pos_est_ptr->velocityEstimate.velocity_estimate_choice)
    {
      case HORIZONTAL_VELOCITY:
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing > MAX_BEARING)
        {
          WRRC_MSG1_ERROR("VEL:bearing %d invalid 0..359",lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing);
          return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed > MAX_HORIZONTALSPEED)
        {
           WRRC_MSG1_ERROR("VEL:horizontalSpeed %d invalid 0..2047",lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed);
           return FALSE;        
        }
        break;
      case HORIZONTAL_WITH_VERTICAL_VELOCITY:
        if((lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection != UPWARD) && 
           (lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection != DOWNWARD))
        {
            WRRC_MSG1_ERROR("VEL:verticalSpeedDirection %d invalid UPWARD/DOWNWARD",lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing > MAX_BEARING)
        {
            WRRC_MSG1_ERROR("VEL:bearing %d invalid 0..359",lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed > MAX_HORIZONTALSPEED)
        {
            WRRC_MSG1_ERROR("VEL:horizontalSpeed %d invalid 0..2047",lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeed > MAX_SPEED)
        {
            WRRC_MSG1_ERROR("VEL:verticalSpeed %d invalid 0..255",lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeed);
            return FALSE;        
        }      
        break;
      case HORIZONTAL_VELOCITY_WITH_UNCERTAINTY:
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing > MAX_BEARING)
        {
            WRRC_MSG1_ERROR("VEL:bearing %d invalid 0..359",lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed > MAX_HORIZONTALSPEED)
        {
            WRRC_MSG1_ERROR("VEL:horizontalSpeed %d invalid 0..2047",lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeedUncertainty > MAX_SPEED)
        {
            WRRC_MSG1_ERROR("VEL:horizontalSpeedUncertainty %d invalid 0..255",lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeedUncertainty);
            return FALSE;        
        }
        break;
      case HORIZONTAL_WITH_VERTICAL_VELOCITY_AND_UNCERTAINTY:
        if((lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection != UPWARD) && 
           (lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection != DOWNWARD))
        {
            WRRC_MSG1_ERROR("VEL:verticalSpeedDirection %d invalid UPWARD/DOWNWARD",lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing > MAX_BEARING)
        {
            WRRC_MSG1_ERROR("VEL:bearing %d invalid 0..359",lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed > MAX_HORIZONTALSPEED)
        {
            WRRC_MSG1_ERROR("VEL:horizontalSpeed %d invalid 0..2047",lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeed > MAX_SPEED)
        {
            WRRC_MSG1_ERROR("VEL:verticalSpeed %d invalid 0..255",lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeed);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalUncertaintySpeed > MAX_SPEED)
        {
            WRRC_MSG1_ERROR("VEL:horizontalUncertaintySpeed %d invalid 0..255",lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalUncertaintySpeed);
            return FALSE;        
        }
        if(lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalUncertaintySpeed > MAX_SPEED)
        {
            WRRC_MSG1_ERROR("VEL:verticalUncertaintySpeed %d invalid 0..255",lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalUncertaintySpeed);
            return FALSE;        
        } 
        break;
     default:
        WRRC_MSG1_ERROR("VEL:Estimate Choice %d invalid", lsm_pos_est_ptr->velocityEstimate.velocity_estimate_choice);
        return FALSE;      
    }
  }
  return TRUE;
}
#endif


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
)
{
  switch (lsm_pos_est_ptr->positioning_estimate)
  {
  case CELL_TIMING_ONLY:
    
    if (lsm_pos_est_ptr->u.cell_timing.psc > PSC_MAX)
    {
      WRRC_MSG1_ERROR("POS: psc %d invalid 0..511", lsm_pos_est_ptr->u.cell_timing.psc);
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->u.cell_timing.sfn > MAX_SFN)
    {
      WRRC_MSG1_ERROR("POS: SFN %d invalid 0..4095", lsm_pos_est_ptr->u.cell_timing.sfn);
      return FALSE;
    }
    break;
    
  case GPS_REFERENCE_TIME_ONLY:    
    break;
    
  case UTRAN_GPS_REFERENCE_TIME:
    
    // TBD. Vivek gps_ref_psc_incl should be removed from interface
    if (lsm_pos_est_ptr->u.utran_gps_ref_time.gps_ref_psc_incl == TRUE)
    {
      
      if (lsm_pos_est_ptr->u.utran_gps_ref_time.psc > PSC_MAX)
      {
        WRRC_MSG1_ERROR("POS:psc %d invalid 0..511",lsm_pos_est_ptr->u.utran_gps_ref_time.psc);
        return FALSE;
      }
    }
    else
    {
      WRRC_MSG0_ERROR("POS: PSC missing");
      return FALSE;
    }
    
    if (lsm_pos_est_ptr->u.utran_gps_ref_time.sfn > MAX_SFN)
    {
      WRRC_MSG1_ERROR("POS:SFN %d invalid 0..4095",lsm_pos_est_ptr->u.utran_gps_ref_time.sfn);
      return FALSE;
    }
    
    
    if (lsm_pos_est_ptr->u.utran_gps_ref_time.msb_timing > GPS_MSB_TIMING)
    {
      WRRC_MSG1_ERROR("POS:MSB Timing %d invalid 0..16383",lsm_pos_est_ptr->u.utran_gps_ref_time.msb_timing);
      return FALSE;
    }
    
    break;
#ifdef FEATURE_GANSS_SUPPORT    
  case GANSS_REFERENCE_TIME_ONLY:
    break;
#endif
  default:
    WRRC_MSG1_ERROR("POS: Invalid Pos Estimate Choice %d", lsm_pos_est_ptr->positioning_estimate);
    return FALSE;
    
  }
  
 if( rrcgps_validate_pos_estimate_choice(lsm_pos_est_ptr) == FALSE)
    {
      return FALSE;
    }
  
  return TRUE;
}


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
)
{
  uint8 cnt = 0;
  
  if (lsm_gps_meas_ptr->addtl_assistance_data_incl == TRUE)
  {
    if (lsm_gps_meas_ptr->addtl_assistance_data.nac_model_data_req_incl == TRUE)
    {
      
      if (lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.gps_week > MAX_GPS_WEEK)
      {
        WRRC_MSG1_ERROR("POS: GPS Week %d invalid 0..1023",
          lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.gps_week);
        return FALSE;
      }
      
      if (lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.gps_tow> MAX_GPS_TOW)
      {
        WRRC_MSG1_ERROR("POS: GPS TOW %d invalid 0..167",
          lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.gps_tow);
        return FALSE;
      }
      
      if (lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.age_tolerance> GPS_AGE_OF_TOLERANCE)
      {
        WRRC_MSG1_ERROR("POS: GPS Age of Tolerance %d invalid 0..10",
          lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.age_tolerance);
        return FALSE;
      }
      
      if (lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites > MAX_NO_OF_SATELLITES)
      {
        WRRC_MSG1_ERROR("POS: No of sat %d invalid 0..16",
          lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites);
        return FALSE;
      }
      
      for (cnt =0; cnt < lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites; cnt++)
      {
        if (lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.sat_data[cnt].sat_id > MAX_SAT_ID)
        {
          WRRC_MSG1_ERROR("POS:Sat Id %d invalid 0..63",
            lsm_gps_meas_ptr->addtl_assistance_data.navModelAddDataRequest.sat_data[cnt].sat_id);
          return FALSE;
        }
        
      }
    }
  }
  return TRUE;
}


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
  const rrc_UE_Positioning_ReportingQuantity *rpt_quan_ptr
)
{

  rrc_positioning_method_type_enum_type pos_method_type;
  tm_umts_cp_wcdma_ue_pos_capability_struct_type pos_capability;  
  
  uecomdef_status_e_type verify_status = SUCCESS;

  pos_method_type = RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  /* First get positioning capability from LSM */
  
  if(rrcgps_cgps_ue_pos_capability_cb == NULL)
  {
    WRRC_MSG0_ERROR("GPS Call back function is NULL");
    return FAILURE;
  }
  else
  {
    rrcgps_cgps_ue_pos_capability_cb(&pos_capability);
  }
  
  WRRC_MSG1_HIGH("POS:LSM returned capability %d",
    pos_capability.network_assisted_gps_support);
#ifdef FEATURE_CGPS_TIMING_OF_CELL_RULE_ENFORCED
  if ( ( rpt_quan_ptr->gps_TimingOfCellWanted == TRUE  ) &&
       ( pos_capability.gps_timing_of_cell_frames_supported == FALSE )
     )
  {
    verify_status = FAILURE;
  }
#endif
  if ((rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_otdoa) &&
      (pos_capability.ue_based_otdoa_supported == FALSE))
  {
    WRRC_MSG0_ERROR("Pos Method OTDOA unsupported");
    verify_status = FAILURE;
  }

  if ( rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_cellID )
  {
    WRRC_MSG0_ERROR("Pos Method CELL ID unsupported");
    verify_status = FAILURE;
  }

  if ( verify_status == SUCCESS ) /* continue examining if still good */
  {

    switch ( pos_capability.network_assisted_gps_support )
    {
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED:
      #endif
      
      {
        /* Reject any UE Assisted Requests */
        if (pos_method_type == RRC_UE_ASSISTED )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=UE BASED, Req=UE_ASSISTED");
        }

        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED:
      #endif
      
      {
        /* Reject UE Based if we don't support standalone */
        if ( ( pos_method_type == RRC_UE_BASED ) && 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NW BASED, Req=UE_BASED, No Standalone");
        }
        break;
      }

      
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_BOTH:
      #endif
      {
        /* Never Reject */
        break;
      }

     
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NONE:
      #endif
      {
        /* Reject everything unless UE Based is requested and standalone is supported */
        if ( ( pos_method_type == RRC_UE_ASSISTED ) || 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NONE, Req=UE_ASSISTED or No Standalone");
        }
        break;
      }

      default:
        break;
    }

  } /* if verify_status is still SUCCESS */

  WRRC_MSG1_HIGH("POS:UE cap check status %d",verify_status);
  
  return verify_status;
}

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

uecomdef_status_e_type rrcgps_verify_positioning_capability_r5
(
  const rrc_UE_Positioning_ReportingQuantity_r4 *rpt_quan_ptr
)
{

  rrc_positioning_method_type_enum_type pos_method_type;
  tm_umts_cp_wcdma_ue_pos_capability_struct_type pos_capability;  

  
  uecomdef_status_e_type verify_status = SUCCESS;

  
  pos_method_type = RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  /* First get positioning capability from LSM */
  
  if(rrcgps_cgps_ue_pos_capability_cb == NULL)
  {
    WRRC_MSG0_ERROR("GPS Call back function is NULL");
    return FAILURE;
  }
  else
  {
    rrcgps_cgps_ue_pos_capability_cb(&pos_capability);
  }
  
  WRRC_MSG1_HIGH("POS:LSM/TM returned capability %d",
    pos_capability.network_assisted_gps_support);

  #ifdef FEATURE_CGPS_TIMING_OF_CELL_RULE_ENFORCED
  if ( ( rpt_quan_ptr->gps_TimingOfCellWanted == TRUE  ) &&
       ( pos_capability.gps_timing_of_cell_frames_supported == FALSE )
     )
  {
    verify_status = FAILURE;
  }
  #endif  
  if ((rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_otdoa) &&
      (pos_capability.ue_based_otdoa_supported == FALSE))
  {
    WRRC_MSG0_ERROR("Pos Method OTDOA unsupported");
    verify_status = FAILURE;
  }

  if ( rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_cellID )
  {
    WRRC_MSG0_ERROR("Pos Method CELL ID unsupported");
    verify_status = FAILURE;
  }

  if ( verify_status == SUCCESS ) /* continue examining if still good */
  {

    switch ( pos_capability.network_assisted_gps_support )
    {
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED:
      #endif
      {
        /* Reject any UE Assisted Requests */
        if (pos_method_type == RRC_UE_ASSISTED )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=UE BASED, Req=UE_ASSISTED");
        }

        break;
      }

      
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED:
      #endif
      {
        /* Reject UE Based if we don't support standalone */
        if ( ( pos_method_type == RRC_UE_BASED ) && 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NW BASED, Req=UE_BASED, No Standalone");
        }
        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_BOTH:
      #endif
      {
        /* Never Reject */
        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NONE:
      #endif

      {
        /* Reject everything unless UE Based is requested and standalone is supported */
        if ( ( pos_method_type == RRC_UE_ASSISTED ) || 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NONE, Req=UE_ASSISTED or No Standalone");
        }
        break;
      }

      default:
        break;
    }

  } /* if verify_status is still SUCCESS */

  WRRC_MSG1_HIGH("POS:UE cap check status %d",verify_status);
  
  return verify_status;
}

/*===========================================================================

FUNCTION   RRCGPS_PROCESS_DCH_TO_FACH_PCH

DESCRIPTION 

  DCH->FACH transition and DCH->PCH
  

DEPENDENCIES

  None

RETURN VALUE

  None. 

SIDE EFFECTS

===========================================================================*/

void rrcgps_process_dch_to_fach_pch
(
 void
)
{
  uint8 count = 0;
  
  rrc_meas_params_struct_type *pos_cmd;
  pos_cmd= &temp_gps;
  memset(pos_cmd,0,sizeof(rrc_meas_params_struct_type));

  pos_cmd->params_type = (uint8)RRC_STATE_CHANGE_PARMS;
  
  /* Check the Validity in Measurement Identity variable for  position Meas */  
  for (count = 0; count < MAX_POS_MEAS; count++)
  {
    if (pos_meas_id_list[count].meas_id != INVALID_MEAS_ID)
    {
      switch(pos_meas_id_list[count].meas_validity)
      {
      case NOT_PRESENT:
        
        WRRC_MSG1_HIGH("POS:Validity absent. Delete Meas = %d",
          pos_meas_id_list[count].meas_id);
        
        /* Cmd type */
        pos_cmd->u.trans_params.trans_cmd_type = (uint8)RRC_MEAS_DELETE;
        
        /* Meas Id */
        pos_cmd->u.trans_params.meas_id = pos_meas_id_list[count].meas_id;
        
        
        WRRC_MSG1_HIGH("Delete POS meas = %d, Indicate LSM ", pos_cmd->u.trans_params.meas_id);
        
        /* Set Meas Id in Meas Identity var to Invalid */
        pos_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        
        pos_meas_id_list[count].meas_validity = NOT_PRESENT;
        
        pos_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        rrcgps_send_lsm_cmd(pos_cmd);
        
        break;  /* Come out of switch and find if any other meas needs to be deleted */
        
      case ALL_STATES:
        
        WRRC_MSG1_HIGH("POS:Validity ALL. Continue meas = %d",
          pos_meas_id_list[count].meas_id);
#ifdef FEATURE_LSM_PCH_REPORTING
        #error code not present
#endif        
        /* No Need to send cmd to LSM */
        break;
        
      case STATE_DCH:
        
        WRRC_MSG1_HIGH("POS:Validity DCH only. Stop,Save meas= %d",
          pos_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        pos_cmd->u.trans_params.meas_id = pos_meas_id_list[count].meas_id;
        
        /* Stop the Measurement */
        pos_cmd->u.trans_params.trans_cmd_type = (uint8)RRC_MEAS_SAVE;
        
        rrcgps_send_lsm_cmd(pos_cmd);
        
        break;
        
      case ALL_STATES_BUT_DCH:
#ifdef FEATURE_LSM_PCH_REPORTING
        #error code not present
#endif
        WRRC_MSG1_HIGH("POS:Validity All except DCH. Resume meas= %d",
          pos_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        pos_cmd->u.trans_params.meas_id = pos_meas_id_list[count].meas_id;
        
        /* Resume the Measurement */
        pos_cmd->u.trans_params.trans_cmd_type = (uint8)RRC_MEAS_RESUME;
        
        rrcgps_send_lsm_cmd(pos_cmd);
        
        break; 
        
      default:
        /* Not possible */
        WRRC_MSG0_ERROR("Invalid meas validity in pos_meas_id_list");
        return;
        
      } /* End of switch */
    } /* end of if */
    
  } /* End of for */
}


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
)
{
  uint8 count = 0;
  rrc_meas_params_struct_type *pos_cmd;
  pos_cmd = &temp_gps;
  memset(pos_cmd,0,sizeof(rrc_meas_params_struct_type));

  pos_cmd->params_type = (uint8)RRC_STATE_CHANGE_PARMS;

  /* Check the Validity in Measurement Identity variable for Pos Meas */  
  for (count = 0; count < MAX_POS_MEAS; count++)
  {
    if (pos_meas_id_list[count].meas_id != INVALID_MEAS_ID)
    {
      switch(pos_meas_id_list[count].meas_validity)
      {
      case NOT_PRESENT:
        
        break;  /* Come out of switch and find if any other meas needs to be deleted */
        
      case ALL_STATES:
        
        WRRC_MSG1_HIGH("POS:Validity ALL. Continue meas = %d",
          pos_meas_id_list[count].meas_id);
        
        /* No Need to send cmd to LSM */
        break;
        
      case STATE_DCH:
        
        WRRC_MSG1_HIGH("POS:Validity DCH only. Resume meas= %d",
          pos_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        pos_cmd->u.trans_params.meas_id = pos_meas_id_list[count].meas_id;
        
        /* Stop the Measurement */
        pos_cmd->u.trans_params.trans_cmd_type = (uint8)RRC_MEAS_RESUME;
        
        rrcgps_send_lsm_cmd(pos_cmd);
        
        break;
        
      case ALL_STATES_BUT_DCH:
        WRRC_MSG1_HIGH("POS:Validity All except DCH. Stop,Save meas= %d",
          pos_meas_id_list[count].meas_id);
        
        /* Set Meas Id */
        pos_cmd->u.trans_params.meas_id = pos_meas_id_list[count].meas_id;
        
        /* Resume the Measurement */
        pos_cmd->u.trans_params.trans_cmd_type = (uint8)RRC_MEAS_SAVE;
        
        rrcgps_send_lsm_cmd(pos_cmd);
        
        break; 
        
      default:
        /* Not possible */
        WRRC_MSG0_ERROR("Invalid meas validity in pos_meas_id_list");
        return;
        
      } /* End of switch */
    } /* end of if */
    
  } /* End of for */
}

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
)
{
  uint8 count = 0;
  
  rrc_meas_params_struct_type *pos_cmd;
  pos_cmd = &temp_gps;
  memset(pos_cmd,0,sizeof(rrc_meas_params_struct_type));
  
  pos_cmd->params_type = (uint8)RRC_STATE_CHANGE_PARMS;
  
  /* Check the Validity in Measurement Identity variable for Traffic Vol Meas */  
  for (count = 0; count < MAX_POS_MEAS; count++)
  {
    if (pos_meas_id_list[count].meas_id != INVALID_MEAS_ID)
    {
      switch(pos_meas_id_list[count].meas_validity)
      {
      case NOT_PRESENT:
        
        break;  /* Come out of switch and find if any other meas needs to be deleted */
        
      case ALL_STATES:
        
        WRRC_MSG1_HIGH("POS:Validity ALL. Continue meas = %d",
          pos_meas_id_list[count].meas_id);
        
        if (rrcmeas_current_substate == RRCMEAS_PCH)
        {
          /* send cmd to LSM to change the reporting interval to 64s */
          WRRC_MSG0_HIGH("POS: change rpt interval to 64s for PCH");
          pos_cmd->u.trans_params.trans_cmd_type = (uint8)RRC_CHANGE_PERIODIC_INTERVAL_TO_PCH;
          
          /* Meas Id */
          pos_cmd->u.trans_params.meas_id = pos_meas_id_list[count].meas_id;
          rrcgps_send_lsm_cmd(pos_cmd);
        }
        
        break;
        
      case STATE_DCH:
        /* no cmd is required as the measurement has already been saved */
        break;
        
      case ALL_STATES_BUT_DCH:
        WRRC_MSG1_HIGH("POS:Validity All except DCH. Continue meas= %d",
          pos_meas_id_list[count].meas_id);
        
        if (rrcmeas_current_substate == RRCMEAS_PCH)
        {
          /* send cmd to LSM to change the reporting interval to 64s */
          WRRC_MSG0_HIGH("POS: change rpt interval to 64s for PCH");
          pos_cmd->u.trans_params.trans_cmd_type = (uint8)RRC_CHANGE_PERIODIC_INTERVAL_TO_PCH;
          
          /* Meas Id */
          pos_cmd->u.trans_params.meas_id = pos_meas_id_list[count].meas_id;
          rrcgps_send_lsm_cmd(pos_cmd);
        }
        
        break; 
        
      default:
        /* Not possible */
        WRRC_MSG0_ERROR("Invalid meas validity in pos_meas_id_list");
        return;
        
      } /* End of switch */
    } /* end of if */
    
  } /* End of for */
}

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
)
{
  WRRC_MSG0_HIGH("Initialize AGPS L2 ack values");
  gps_l2_ack_params.waiting_for_l2_ack = FALSE;
  gps_l2_ack_params.meas_id = INVALID_MEAS_ID;
}

#endif


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
)
{
  meas_validity_enum_type pos_meas_validity = NOT_PRESENT;
  
  rrc_UE_Positioning_ReportingQuantity_r4* rpt_quan_ptr = NULL;

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  rpt_crit_enum_type rpt_mode;

  rpt_quan_ptr = &up_Measurement_ptr->ue_positioning_ReportingQuantity;

  meas_ctrl_params_ptr->rpt_quan_incl = TRUE;
  
  
  /* Get Pos Method */
  meas_ctrl_params_ptr->rpt_quan.method_type =
    (uint8) RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  if (rrcgps_verify_positioning_capability_r5(rpt_quan_ptr)
    == FAILURE)
  {
    /* Send Meas Ctrl Failure */
    WRRC_MSG0_ERROR("POS: Pos Capability error");
    
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
#ifdef FEATURE_CGPS_UMTS_CP_WCDMA
    if(rrcgps_cgps_ue_pos_capability_cb == NULL)
    {
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
    }
    else
#endif
    {
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
    }
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
    
  }
  
  /* Get Reporting Quantity */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r4,
    horizontalAccuracy))
  {
    meas_ctrl_params_ptr->rpt_quan.hor_accuracy_incl = TRUE;
    
    /* Set the 7 bits */
    meas_ctrl_params_ptr->rpt_quan.horizontal_accuracy = (uint8)
      (*(rpt_quan_ptr->horizontalAccuracy.data) >> 1);
  }
  
  /* AdditionalAssistanceDataRequest */
  if (rpt_quan_ptr->additionalAssistanceDataReq == TRUE)
  {
    meas_ctrl_params_ptr->rpt_quan.additional_assistance_data_request = TRUE;
  }
  
  if (rpt_quan_ptr->gps_TimingOfCellWanted == TRUE)
  {
    meas_ctrl_params_ptr->rpt_quan.sfn_gps_timing_required = TRUE;
  }
  
  
  /* Get Reporting Criteria */
  if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_ue_positioning_ReportingCriteria))
  {
    WRRC_MSG0_ERROR("POS: Event trigg crit unsupported");
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
    /* Set error t */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_noReporting))
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_NO_RPT;
  }
  else
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_PERIODIC_RPT;
    
    /* Get reporting criteria and interval */
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)rrc_ReportingAmount_ra_Infinity;  

    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)
      up_Measurement_ptr->
      reportCriteria.u.periodicalReportingCriteria->reportingAmount;
  
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_interval = (uint8)
      up_Measurement_ptr->reportCriteria.u.
      periodicalReportingCriteria->reportingInterval;
  }
  if(up_Measurement_ptr->m.ue_positioning_GPS_AssistanceDataPresent )
  {
  /* Now call LSM fn to store Meas Object if FEATURE_MMGPS; call a local function to do the same if FEATURE_CGPS_xxx */
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
    if (rrcgps_convert_assistance_data(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
      &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)
  #else /* this will only be FEATURE_MMGPS */
    if (lsm_convert_assistance_data(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
      &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)
  #endif
    {
      WRRC_MSG0_ERROR("POS: Assistance Data conversion failed");
    
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
    
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
    
      return FAILURE;
    }
    meas_ctrl_params_ptr->gps_assistance_data_incl = TRUE;
  }
  
  /* Check if Meas Validity present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(up_Measurement_ptr,
    & rrc_UE_Positioning_Measurement_r4,measurementValidity))
  {
    WRRC_MSG1_HIGH("POS:Meas validity %d (0/d:1/f:2/a)", up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Store Measurement Validity */
    pos_meas_validity = TVM_RET_MEAS_VALIDITY(up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Check if Meas needs to be restarted */
    if (((pos_meas_validity == STATE_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_FACH)) ||
       ((pos_meas_validity == ALL_STATES_BUT_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_DCH)))
    {
      WRRC_MSG0_HIGH("POS: RRC val, MCM val differ");
      meas_ctrl_params_ptr->meas_start_incl = FALSE;
    }
  }    
  /* Store Pos Parameters in RRC Database */
  if(rrcmeas_config_pos_db(pos_info_ptr, &pos_meas_validity, &rpt_mode, modify_meas_params_incl) == FALSE)
  {
    return FAILURE;
  }
  return SUCCESS;
}

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
)
{
  /* Process the A-GPS Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  uint16 deleted_meas_id = 0;
  
  /* Declare MCM config structure that goes to L1 */
  rrc_meas_params_struct_type  *gps_meas_params;
  
  /* Declare ptr to store message received from UTRAN */
  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrc_MeasurementControl_r4_IEs* rrc_mcm_ptr = NULL; 

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  uint8 transaction_id = INVALID_TRANSACTION_ID;
  gps_meas_params = &temp_gps;
  memset(gps_meas_params,0,sizeof(rrc_meas_params_struct_type));  
  gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = FALSE;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.environ_ch_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = FALSE;
  gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.meas_start_incl = TRUE;
  
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)pos_mcm_ptr;
  transaction_id = meas_ctrl_tr_id;

  /* By default, it is set to 0 => 1 */
  gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = 0;

  if(!(dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.t
    == T_rrc_MeasurementControl_criticalExtensions_4_r4))
  {
    return FAILURE;
  }

  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.r4.measurementControl_r4;

  /* Set Transaction Id and Meas Id */
  p_info_ptr->meas_id = gps_meas_params->u.meas_ctrl_params.meas_id =
    (uint8) rrc_mcm_ptr->measurementIdentity;
  
  gps_meas_params->u.meas_ctrl_params.config_conf_ind = TRUE;
  
  gps_meas_params->u.meas_ctrl_params.config_ack_num = (uint8) transaction_id;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan.method = (uint8)RRC_GPS;
  gps_meas_params->params_type = (uint8)RRC_MEAS_CTRL_PARMS;
  

  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r4_setup))
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_SETUP;
    
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,measurementReportingMode)))
    {
      /* Send Meas Ctrl Failure */
      WRRC_MSG0_ERROR("Meas Reporting Mode missing for SETUP");
      
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
    }
    else
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode =
       (uint8) RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    if(FAILURE == rrcgps_fill_mcm_info_r5(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.setup->u.
      up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id, FALSE))
    {
      return FAILURE;
    }
  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r4_modify))
  {
    /* Addtl Meas processing later. Reporting crit stored in db later */
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_MODIFY;
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r4_IEs,measurementReportingMode))
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode = (uint8)
        RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,rrc_MeasurementCommand_r4_modify,measurementType))
    {
      if(FAILURE == rrcgps_fill_mcm_info_r5(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.modify->
         measurementType.u.up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id,
         TRUE))
      {
        return FAILURE;
      }
    } /* End of if of Meas Type present */
  } /* End of modify */
  else
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8) RRC_MEAS_DELETE;
    WRRC_MSG0_HIGH("POS: Sending GPS Meas Request");
    /* At present just store meas id in meas db */
    for ( count = 0; count < MAX_POS_MEAS; count++ )
    {
      if (pos_meas_id_list[count].meas_id == gps_meas_params->u.meas_ctrl_params.meas_id)
      {
        WRRC_MSG1_HIGH("POS: Meas Id %d deleted from DB", gps_meas_params->u.meas_ctrl_params.meas_id);
        pos_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        pos_meas_id_list[count].meas_validity = NOT_PRESENT;
        pos_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          pos_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        break;
      }
    } /* End of for */
    
    deleted_meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&deleted_meas_id);
    
    rrcgps_send_lsm_cmd(gps_meas_params);
    return SUCCESS;
  }
  
  
  WRRC_MSG0_HIGH("POS: Sending GPS Meas Request");
  rrcgps_send_lsm_cmd(gps_meas_params);
  return SUCCESS;

}

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
)
{
  /* Process the A-GPS Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  uint16 deleted_meas_id = 0;
  
  /* Declare MCM config structure that goes to L1 */
  rrc_meas_params_struct_type  *gps_meas_params;
  
  /* Declare ptr to store message received from UTRAN */
  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrc_MeasurementControl_r6_IEs* rrc_mcm_ptr = NULL; 

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  uint8 transaction_id = INVALID_TRANSACTION_ID;
  gps_meas_params = &temp_gps;
  memset(gps_meas_params,0,sizeof(rrc_meas_params_struct_type));  
  gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = FALSE;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.environ_ch_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = FALSE;
  gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.meas_start_incl = TRUE;
  
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)pos_mcm_ptr;
  transaction_id = meas_ctrl_tr_id;

  /* By default, it is set to 0 => 1 */
  gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = 0;

  if(!(dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.t
    == T_rrc_MeasurementControl_criticalExtensions_4_later_than_r4))
  {
    return FAILURE;
  }

  rrc_mcm_ptr = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.r6.measurementControl_r6;

  /* Set Transaction Id and Meas Id */
  p_info_ptr->meas_id = gps_meas_params->u.meas_ctrl_params.meas_id =
    (uint8) rrc_mcm_ptr->measurementIdentity;
  
  gps_meas_params->u.meas_ctrl_params.config_conf_ind = TRUE;
  
  gps_meas_params->u.meas_ctrl_params.config_ack_num = (uint8) transaction_id;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan.method = (uint8)RRC_GPS;
  gps_meas_params->params_type = (uint8)RRC_MEAS_CTRL_PARMS;
  

  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r6_setup))
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_SETUP;
    
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,measurementReportingMode)))
    {
      /* Send Meas Ctrl Failure */
      WRRC_MSG0_ERROR("Meas Reporting Mode missing for SETUP");
      
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
    }
    else
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode =
       (uint8) RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    if(FAILURE == rrcgps_fill_mcm_info_r5(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.setup->u.
      up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id, FALSE))
    {
      return FAILURE;
    }
  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r6_modify))
  {
    /* Addtl Meas processing later. Reporting crit stored in db later */
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_MODIFY;
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r6_IEs,
        measurementReportingMode))
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode = (uint8)
        RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      & rrc_MeasurementCommand_r6_modify,measurementType))
    {
      if(FAILURE == rrcgps_fill_mcm_info_r5(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.modify->
         measurementType.u.up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id,
         TRUE))
      {
        return FAILURE;
      }
    } /* End of if of Meas Type present */
  } /* End of modify */
  else
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8) RRC_MEAS_DELETE;
    /* At present just store meas id in meas db */
    for ( count = 0; count < MAX_POS_MEAS; count++ )
    {
      if (pos_meas_id_list[count].meas_id == gps_meas_params->u.meas_ctrl_params.meas_id)
      {
        WRRC_MSG1_HIGH("POS: Meas Id %d deleted from DB", gps_meas_params->u.meas_ctrl_params.meas_id);
        pos_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        pos_meas_id_list[count].meas_validity = NOT_PRESENT;
        pos_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          pos_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        break;
      }
    } /* End of for */
    
    deleted_meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&deleted_meas_id);
    
    rrcgps_send_lsm_cmd(gps_meas_params);
    return SUCCESS;
  }
  
  rrcgps_send_lsm_cmd(gps_meas_params);
  return SUCCESS;
}



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
void rrcgps_init_procedure( void )
{
  rrcgps_init_data(); /* Initialize all the procedure variables */

  /* Register for state change  - When state changes from DCH
  to any other state we want to be notified since this procedure
  is active only in the DCH state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_GPS,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED  /* To State       */
                           );
}


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
)
{
  /* Process the A-GPS Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  uint16 deleted_meas_id = 0;
  
  /* Declare MCM config structure that goes to L1 */
  rrc_meas_params_struct_type  *gps_meas_params;
  
  /* Declare ptr to store message received from UTRAN */
  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrc_MeasurementControl_r7_IEs* rrc_mcm_ptr = NULL; 

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  uint8 transaction_id = INVALID_TRANSACTION_ID;
  gps_meas_params = &temp_gps;
  memset(gps_meas_params,0,sizeof(rrc_meas_params_struct_type)); 
  gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = FALSE;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.environ_ch_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = FALSE;
  gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.meas_start_incl = TRUE;
  
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)pos_mcm_ptr;
  transaction_id = meas_ctrl_tr_id;

  /* By default, it is set to 0 => 1 */
  gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = 0;

  if(!(dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.t
    == T_rrc_MeasurementControl_criticalExtensions_4_later_than_r4))
  {
    return FAILURE;
  }

  rrc_mcm_ptr =  &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.r7.measurementControl_r7;

  /* Set Transaction Id and Meas Id */
  p_info_ptr->meas_id = gps_meas_params->u.meas_ctrl_params.meas_id =
    (uint8) rrc_mcm_ptr->measurementIdentity;
  
  gps_meas_params->u.meas_ctrl_params.config_conf_ind = TRUE;
  
  gps_meas_params->u.meas_ctrl_params.config_ack_num = (uint8) transaction_id;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan.method = (uint8)RRC_GPS;
  gps_meas_params->params_type = (uint8)RRC_MEAS_CTRL_PARMS;
  

  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r7_setup))
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_SETUP;
    
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,measurementReportingMode)))
    {
      /* Send Meas Ctrl Failure */
      WRRC_MSG0_ERROR("Meas Reporting Mode missing for SETUP");
      
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
    }
    else
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode =
       (uint8) RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    if(FAILURE == rrcgps_fill_mcm_info_r7(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.setup->u.
      up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id, FALSE))
    {
      return FAILURE;
    }
  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r7_modify))
  {
    /* Addtl Meas processing later. Reporting crit stored in db later */
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_MODIFY;
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r7_IEs,measurementReportingMode))
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode = (uint8)
        RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
      & rrc_MeasurementCommand_r7_modify,measurementType))
    {
      if(FAILURE == rrcgps_fill_mcm_info_r7(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.modify->
         measurementType.u.up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id,
         TRUE))
      {
        return FAILURE;
      }
    } /* End of if of Meas Type present */
  } /* End of modify */
  else
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8) RRC_MEAS_DELETE;
    /* At present just store meas id in meas db */
    for ( count = 0; count < MAX_POS_MEAS; count++ )
    {
      if (pos_meas_id_list[count].meas_id == gps_meas_params->u.meas_ctrl_params.meas_id)
      {
        WRRC_MSG1_HIGH("POS: Meas Id %d deleted from DB", gps_meas_params->u.meas_ctrl_params.meas_id);
        pos_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        pos_meas_id_list[count].meas_validity = NOT_PRESENT;
        pos_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          pos_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        break;
      }
    } /* End of for */
    
    deleted_meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&deleted_meas_id);
    
    rrcgps_send_lsm_cmd(gps_meas_params);
    return SUCCESS;
  }
  
  rrcgps_send_lsm_cmd(gps_meas_params);
  return SUCCESS;

}


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
)
{
  meas_validity_enum_type pos_meas_validity = NOT_PRESENT;
  
  rrc_UE_Positioning_ReportingQuantity_r7* rpt_quan_ptr = NULL;

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  rpt_crit_enum_type rpt_mode;

  boolean bs_failed = FALSE;

  rpt_quan_ptr = &up_Measurement_ptr->ue_positioning_ReportingQuantity;

  meas_ctrl_params_ptr->rpt_quan_incl = TRUE;
  
  
  /* Get Pos Method */
  meas_ctrl_params_ptr->rpt_quan.method_type =
    (uint8) RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  if (rrcgps_verify_positioning_capability_r7(rpt_quan_ptr)
    == FAILURE)
  {
    /* Send Meas Ctrl Failure */
    WRRC_MSG0_ERROR("POS: Pos Capability error");
    
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
    /* Set error t */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
    
  }
  
  /* Get Reporting Quantity */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,
    rrc_UE_Positioning_ReportingQuantity_r7,horizontalAccuracy))
  {
    meas_ctrl_params_ptr->rpt_quan.hor_accuracy_incl = TRUE;
    
    /* Set the 7 bits */
    meas_ctrl_params_ptr->rpt_quan.horizontal_accuracy = (uint8)rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                               &bs_failed, &rpt_quan_ptr->horizontalAccuracy );
  }

  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r7,velocityRequested))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r7,gANSSPositioningMethods))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r7,gANSSTimingOfCellWanted))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rc_UE_Positioning_ReportingQuantity_r7,gANSSCarrierPhaseMeasurementRequested)))
  {
    /*TBD*/
    WRRC_MSG0_HIGH("Rel 7 IE's Support TBD");
  }
  
  /* AdditionalAssistanceDataRequest */
  if (rpt_quan_ptr->additionalAssistanceDataReq == TRUE)
  {
    meas_ctrl_params_ptr->rpt_quan.additional_assistance_data_request = TRUE;
  }
  
  if (rpt_quan_ptr->gps_TimingOfCellWanted == TRUE)
  {
    meas_ctrl_params_ptr->rpt_quan.sfn_gps_timing_required = TRUE;
  }
  
  
  /* Get Reporting Criteria */
  if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_r7_ue_positioning_ReportingCriteria))
  {
    WRRC_MSG0_ERROR("POS: Event trigg crit unsupported");
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
    /* Set error t */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_r7_noReporting))
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_NO_RPT;
  }
  else
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_PERIODIC_RPT;
    
    /* Get reporting criteria and interval */
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)rrc_ReportingAmount_ra_Infinity;
 
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)
      up_Measurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingAmount;
  
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_interval = (uint8)
      up_Measurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingInterval;
  }
  if(up_Measurement_ptr->m.ue_positioning_GPS_AssistanceDataPresent )
  {
  /* Now call LSM fn to store Meas Object if FEATURE_MMGPS; call a local function to do the same if FEATURE_CGPS_xxx */
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
    if (rrcgps_convert_assistance_data_r7(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
      &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)
  #else /* this will only be FEATURE_MMGPS */
  /*To be Required from GPS guys*/
/*  if (lsm_convert_assistance_data_r7(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
    &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)*/
  #endif
    {
      WRRC_MSG0_ERROR("POS: Assistance Data conversion failed");
    
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
    
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
    
      return FAILURE;
    }
    meas_ctrl_params_ptr->gps_assistance_data_incl = TRUE;
  }
  
  /* Check if Meas Validity present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(up_Measurement_ptr,
    & rrc_UE_Positioning_Measurement_r7,measurementValidity))
  {
    WRRC_MSG1_HIGH("POS:Meas validity %d (0/d:1/f:2/a)", up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Store Measurement Validity */
    pos_meas_validity = TVM_RET_MEAS_VALIDITY(up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Check if Meas needs to be restarted */
    if (((pos_meas_validity == STATE_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_FACH)) ||
       ((pos_meas_validity == ALL_STATES_BUT_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_DCH)))
    {
      WRRC_MSG0_HIGH("POS: RRC val, MCM val differ");
      meas_ctrl_params_ptr->meas_start_incl = FALSE;
    }
  }    

  switch(meas_ctrl_params_ptr->rpt_crit.rpt_mode)
  {
    case 0: rpt_mode = PERIODIC;
     break;
    case 1: rpt_mode = EVENT;
     break;
    case 2: rpt_mode = ADDTL;
     break;
    case 3: rpt_mode = NOT_AVAILABLE;
     break;
    default:break;
  } 
  
  /* Store Pos Parameters in RRC Database */
  if(rrcmeas_config_pos_db(pos_info_ptr, &pos_meas_validity, &rpt_mode, modify_meas_params_incl) == FALSE)
  {
    return FAILURE;
  }
  return SUCCESS;
}

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
)
{

  rrc_positioning_method_type_enum_type pos_method_type;

  tm_umts_cp_wcdma_ue_pos_capability_struct_type pos_capability;  
  
  uecomdef_status_e_type verify_status = SUCCESS;

  
  pos_method_type = RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  /* First get positioning capability from LSM */
  if(rrcgps_cgps_ue_pos_capability_cb == NULL)
  {
    WRRC_MSG0_ERROR("GPS Call back function is NULL");
    return FAILURE;
  }
  else
  {
    rrcgps_cgps_ue_pos_capability_cb(&pos_capability);
  }
  
  WRRC_MSG1_HIGH("POS:LSM/TM returned capability %d",
    pos_capability.network_assisted_gps_support);

  if ( ( rpt_quan_ptr->gps_TimingOfCellWanted == TRUE  ) &&
       ( pos_capability.gps_timing_of_cell_frames_supported == FALSE )
     )
  {
    verify_status = FAILURE;
  }

  if ((rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_otdoa) &&
      (pos_capability.ue_based_otdoa_supported == FALSE))
  {
    WRRC_MSG0_ERROR("Pos Method OTDOA unsupported");
    verify_status = FAILURE;
  }

  if ( rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_cellID )
  {
    WRRC_MSG0_ERROR("Pos Method CELL ID unsupported");
    verify_status = FAILURE;
  }

  if ( verify_status == SUCCESS ) /* continue examining if still good */
  {

    switch ( pos_capability.network_assisted_gps_support )
    {
      
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED:
      #endif
      {
        /* Reject any UE Assisted Requests */
        if (pos_method_type == RRC_UE_ASSISTED )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=UE BASED, Req=UE_ASSISTED");
        }

        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED:
      #endif

      {
        /* Reject UE Based if we don't support standalone */
        if ( ( pos_method_type == RRC_UE_BASED ) && 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NW BASED, Req=UE_BASED, No Standalone");
        }
        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_BOTH:
      #endif
      {
        /* Never Reject */
        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NONE:
      #endif
      {
        /* Reject everything unless UE Based is requested and standalone is supported */
        if ( ( pos_method_type == RRC_UE_ASSISTED ) || 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NONE, Req=UE_ASSISTED or No Standalone");
        }
        break;
      }

      default:
        break;
    }

  } /* if verify_status is still SUCCESS */

  WRRC_MSG1_HIGH("POS:UE cap check status %d",verify_status);
  
  return verify_status;
}

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
  rrc_assistance_data_struct_type* rrc_ptr )
{
  uint32 q_Cnt;
  uint8  sat_info_cnt = 0;
  boolean bs_failed = FALSE;

  /* Zero out RRC structure */
  memset( (void *) rrc_ptr, 0, sizeof( rrc_assistance_data_struct_type ) );

  /* Acq Assistance */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData_r7,ue_positioning_GPS_AcquisitionAssistance))
  {
    rrc_UE_Positioning_GPS_AcquisitionAssistance_r7 *asn1_acq_assist = 
      &asn1_ptr->ue_positioning_GPS_AcquisitionAssistance;
    rrc_gps_acq_assistance_struct_type *rrc_acq_assist = 
      &rrc_ptr->gps_acq_assistance;

    rrc_AcquisitionSatInfoList *asn1_info_ptr;
    rrc_gps_sat_info_struct_type *rrc_info_ptr;

    rrc_ptr->gps_acq_assistance_incl = TRUE;
    rrc_acq_assist->gps_reference_time = asn1_acq_assist->gps_ReferenceTime;

    /* GPS - UTRAN Reference Time */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_acq_assist,
        rrc_UE_Positioning_GPS_AcquisitionAssistance_r7,utran_GPSReferenceTime))
    {
      rrc_acq_assist->gps_umts_reference_time_incl = TRUE;
      rrc_acq_assist->gps_umts_reference_time.msb_timing =
        asn1_acq_assist->utran_GPSReferenceTime.utran_GPSTimingOfCell.ms_part;
      rrc_acq_assist->gps_umts_reference_time.lsb_timing =
        asn1_acq_assist->utran_GPSReferenceTime.utran_GPSTimingOfCell.ls_part;
      rrc_acq_assist->gps_umts_reference_time.sfn =
        asn1_acq_assist->utran_GPSReferenceTime.sfn;

      /* Primary Scrambling Code */
      if (RRC_MSG_COMMON_BITMASK_IE( asn1_acq_assist->utran_GPSReferenceTime,
        rrc_UTRAN_GPSReferenceTime,modeSpecificInfo))
      {
         rrc_acq_assist->gps_umts_reference_time.gps_ref_psc_incl = TRUE;
         
        if(asn1_acq_assist->utran_GPSReferenceTime.modeSpecificInfo.t == T_rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd)
        {
           /* Only FDD mode supported in UE, grab primary scrambling code */
           rrc_acq_assist->gps_umts_reference_time.psc =
           asn1_acq_assist->utran_GPSReferenceTime.modeSpecificInfo.u.fdd->referenceIdentity.primaryScramblingCode;
        }
        else
        {
          WRRC_MSG0_ERROR("TDD info given in MCM. Send failure.");
          return FALSE;
        }
      }
    }

    /* Traverse Satellite Info List */
    sat_info_cnt = 0;
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_AcquisitionAssistance.satelliteInformationList;
    rrc_info_ptr = &rrc_ptr->gps_acq_assistance.gps_sat_info[ 0 ];
    
    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if( asn1_info_ptr == NULL )
        break;
    
      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->doppler = asn1_info_ptr->elem[q_Cnt].doppler0thOrder;
    
      /* Extra Doppler Info */
      if ( asn1_info_ptr->elem[q_Cnt].m.extraDopplerInfoPresent )
      {
        rrc_info_ptr->extra_doppler_info_incl = TRUE;
        rrc_info_ptr->extra_doppler_info.doppler_first_order =
          asn1_info_ptr->elem[q_Cnt].extraDopplerInfo.doppler1stOrder;
        rrc_info_ptr->extra_doppler_info.Doppler_uncertainity =
          asn1_info_ptr->elem[q_Cnt].extraDopplerInfo.dopplerUncertainty;
      }
    
      rrc_info_ptr->code_phase = asn1_info_ptr->elem[q_Cnt].codePhase;
      rrc_info_ptr->integer_code_phase = asn1_info_ptr->elem[q_Cnt].integerCodePhase;
      rrc_info_ptr->gps_bit_number = asn1_info_ptr->elem[q_Cnt].gps_BitNumber;
      rrc_info_ptr->search_window = asn1_info_ptr->elem[q_Cnt].codePhaseSearchWindow;
    
      /* Azimuth and Elevation */
      if ( asn1_info_ptr->elem[q_Cnt].m.azimuthAndElevationPresent )
      {
        rrc_info_ptr->azimuth_elevation_incl = TRUE;
        rrc_info_ptr->azimuth_elevation_info.azimuth =
          asn1_info_ptr->elem[q_Cnt].azimuthAndElevation.azimuth;
        rrc_info_ptr->azimuth_elevation_info.elevation =
          asn1_info_ptr->elem[q_Cnt].azimuthAndElevation.elevation;
      }
    
      /* Update sat info ptr with next element in the list */
      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_acq_assist->no_of_gps_sat = sat_info_cnt;
    if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_acq_assist,
        rrc_UE_Positioning_GPS_AcquisitionAssistance_r7,ue_Positioning_GPS_ReferenceTimeUncertainty))
    {
      /*TBD*/
      WRRC_MSG0_HIGH("Rel7 IE's TBD");
    }
  }
  
  /* Almanac */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r7,
    ue_positioning_GPS_Almanac))
  {
    rrc_UE_Positioning_GPS_Almanac *asn1_almanac =
	  &asn1_ptr->ue_positioning_GPS_Almanac;
    rrc_gps_almanac_struct_type *rrc_almanac =
	  &rrc_ptr->gps_almanc;

    rrc_AlmanacSatInfoList *asn1_info_ptr;
	rrc_almanac_sat_info_struct_type *rrc_info_ptr; 

    rrc_ptr->gps_almanac_incl = TRUE;
	rrc_almanac->wn_a = 
	  ( uint8 )rrcgps_oss_bitstring_to_int( sizeof( uint8 ), &bs_failed, &asn1_almanac->wn_a );

    /* Global Health */
    if ( asn1_almanac->m.sv_GlobalHealthPresent )
    {
      uint16 num_bits = asn1_almanac->sv_GlobalHealth.numbits;
      uint8 num_bytes; 
      uint8 *dest_ptr = (uint8 *)&rrc_almanac->global_health[0];
      uint8 *src_ptr = (uint8 *)asn1_almanac->sv_GlobalHealth.data;
    
      /* This is a special case where we can not use rrcgps_oss_bitstring_to_int(). The Global
         Health could be hundreds of bytes long, therefore we need to handle this data 
    	 in this function */
    
      num_bytes = num_bits / 8;

      /* Determine if we need an extra byte for any straggler bits */
      if ( num_bits % 8 )
        num_bytes++;
    
      /* Copy global health data */
      if ( num_bytes <= GLOBAL_HEALTH_ARRAY_SIZE )
      {
        rrc_almanac->global_health_incl = TRUE;
        rrc_almanac->global_health_num_bits = num_bits;
      
        /* Copy global health bits */
        for ( q_Cnt = num_bytes; q_Cnt; q_Cnt-- )
        {
          *dest_ptr++ = *src_ptr++;
        }
      }
    }

    /* Traverse Almanac List */
	 
    sat_info_cnt = 0;
    asn1_info_ptr = &asn1_almanac->almanacSatInfoList;
    rrc_info_ptr = &rrc_almanac->almanac_sat_info_list[0];

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if ( asn1_info_ptr == NULL )
        break;
    
      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->data_id = (uint8) asn1_info_ptr->elem[q_Cnt].dataID;

      rrc_info_ptr->a_Sqrt = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].a_Sqrt );
      rrc_info_ptr->deltaI = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].deltaI );
      rrc_info_ptr->e = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].e );
      rrc_info_ptr->m0 = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].m0 );
      rrc_info_ptr->omega0 = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omega0 );
      rrc_info_ptr->omega_dot = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omegaDot );
      rrc_info_ptr->omega =
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omega );
      rrc_info_ptr->sat_health = 
        (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].satHealth );
      rrc_info_ptr->t_oa =
        (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].t_oa );
      rrc_info_ptr->af0 =
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].af0 );
      rrc_info_ptr->af1 =
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].af1 );


      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_almanc.no_of_almanac_sat = sat_info_cnt;
  }

  /* Nav Model */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r7,
    ue_positioning_GPS_NavigationModel))
  {
    rrc_navigation_model_sat_info_list_struct_type *rrc_info_ptr;
    rrc_NavigationModelSatInfoList *asn1_info_ptr;

    rrc_ptr->gps_navigation_model_incl = TRUE;

    /* Traverse NavModel List */
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_NavigationModel.navigationModelSatInfoList;
    rrc_info_ptr = &rrc_ptr->gps_navigation_model.satellite_info[0];
    sat_info_cnt = 0;

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
        /* Check to see if we have reached the end of the linked list */
        if ( asn1_info_ptr == NULL )
          break;

      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->satellite_status = asn1_info_ptr->elem[q_Cnt].satelliteStatus;
       
      /* Ephemeris Parameters */
      if ( asn1_info_ptr->elem[q_Cnt].m.ephemerisParameterPresent )
      {
        rrc_info_ptr->ephermeral_clock_params_incl = TRUE;
        rrc_info_ptr->ephermeral_clock_params.a_Sqrt =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.a_Sqrt );
        rrc_info_ptr->ephermeral_clock_params.af0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af0 );
        rrc_info_ptr->ephermeral_clock_params.af1 = 
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af1 );
        rrc_info_ptr->ephermeral_clock_params.af2 =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af2 );
        rrc_info_ptr->ephermeral_clock_params.aodo =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.aodo );
        rrc_info_ptr->ephermeral_clock_params.c_ic =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_ic );
        rrc_info_ptr->ephermeral_clock_params.c_is =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_is );
        rrc_info_ptr->ephermeral_clock_params.c_rc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_rc );
        rrc_info_ptr->ephermeral_clock_params.c_rs =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_rs );
        rrc_info_ptr->ephermeral_clock_params.c_uc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_uc );
        rrc_info_ptr->ephermeral_clock_params.c_us =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_us );
        rrc_info_ptr->ephermeral_clock_params.code_on_l2 =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.codeOnL2 );
        rrc_info_ptr->ephermeral_clock_params.delta_n =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.delta_n );
        rrc_info_ptr->ephermeral_clock_params.e =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.e );
        rrc_info_ptr->ephermeral_clock_params.fit_interval =
            (boolean) rrcgps_oss_bitstring_to_int( sizeof(boolean), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.fitInterval );
        rrc_info_ptr->ephermeral_clock_params.i0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.i0 );
        rrc_info_ptr->ephermeral_clock_params.iDot =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.iDot );
            rrc_info_ptr->ephermeral_clock_params.iodc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.iodc );
        rrc_info_ptr->ephermeral_clock_params.l2_pflag =
            (boolean) rrcgps_oss_bitstring_to_int( sizeof(boolean), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.l2Pflag );
        rrc_info_ptr->ephermeral_clock_params.m0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.m0 );
        rrc_info_ptr->ephermeral_clock_params.omega =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omega );
        rrc_info_ptr->ephermeral_clock_params.omega0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omega0 );
        rrc_info_ptr->ephermeral_clock_params.omegaDot =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omegaDot );
        rrc_info_ptr->ephermeral_clock_params.sat_health =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.satHealth );
        rrc_info_ptr->ephermeral_clock_params.t_gd =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_GD );
        rrc_info_ptr->ephermeral_clock_params.t_oc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_oc );
        rrc_info_ptr->ephermeral_clock_params.t_oe =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_oe );
        rrc_info_ptr->ephermeral_clock_params.ura_index =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.uraIndex );
      }

      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_navigation_model.no_of_satellites = sat_info_cnt;

  }

  /* Real Time Integrity */
  if( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r7,
    ue_positioning_GPS_Real_timeIntegrity))
  {
    rrc_BadSatList *asn1_info_ptr;
    uint8 *bad_sat_ptr;

    rrc_ptr->gps_real_time_integrity_incl = TRUE;

    /* Fill in RTI values */
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_Real_timeIntegrity;
    bad_sat_ptr = (uint8 *)&rrc_ptr->gps_rti.satellite_id[0];

    sat_info_cnt = 0;

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if ( asn1_info_ptr == NULL )
          break;

      *bad_sat_ptr = (uint8) asn1_info_ptr->elem[q_Cnt];

      
      bad_sat_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_rti.no_of_satellites = sat_info_cnt;
  }
    
  /* Reference Location */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r7,
    ue_positioning_GPS_ReferenceLocation))
  {
    rrc_gps_reference_loc_struct_type *rrc_ref_loc =
	  &rrc_ptr->gps_ref_loc;
    rrc_EllipsoidPointAltitudeEllipsoide *asn1_ref_loc =
	  &asn1_ptr->ue_positioning_GPS_ReferenceLocation.ellipsoidPointAltitudeEllipsoide;

    rrc_ptr->gps_ref_loc_incl = TRUE;
    rrc_ref_loc->altitude = asn1_ref_loc->altitude;
    rrc_ref_loc->altitude_direction = asn1_ref_loc->altitudeDirection;
    rrc_ref_loc->confidence = asn1_ref_loc->confidence;
    rrc_ref_loc->latitude = asn1_ref_loc->latitude;
    rrc_ref_loc->latitude_sign = asn1_ref_loc->latitudeSign;
    rrc_ref_loc->longitude = asn1_ref_loc->longitude;
    rrc_ref_loc->orientation_major_axis = (uint8) asn1_ref_loc->orientationMajorAxis;
    rrc_ref_loc->uncertainity_altitude = asn1_ref_loc->uncertaintyAltitude;
    rrc_ref_loc->uncertainity_semi_major = (uint8) asn1_ref_loc->uncertaintySemiMajor;
    rrc_ref_loc->uncertainity_semi_minor = (uint8) asn1_ref_loc->uncertaintySemiMinor;
  }

  /* Reference Time */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r7,
    ue_positioning_GPS_ReferenceTime))
  {
    rrc_gps_reference_time_struct_type *rrc_ref_time =
	  &rrc_ptr->gps_ref_time;
    rrc_UE_Positioning_GPS_ReferenceTime_r7 *asn1_ref_time =
	  &asn1_ptr->ue_positioning_GPS_ReferenceTime;

    rrc_ptr->gps_ref_time_incl = TRUE;
    rrc_ref_time->time = asn1_ref_time->gps_tow_1msec;
    rrc_ref_time->week = asn1_ref_time->gps_Week;
	
    /* GPS Drift Rate */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        utran_GPS_DriftRate))
    {
      rrc_ref_time->gps_drift_rate_incl = TRUE;
      rrc_ref_time->gps_drift_rate =
      asn1_ref_time->utran_GPS_DriftRate;
    }

    /* GPS - UTRAN reference time */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        utran_GPSReferenceTime))
    {
      rrc_ref_time->gps_utran_reference_time_incl = TRUE;
      rrc_ref_time->gps_utran_ref_time.lsb_timing =
        asn1_ref_time->utran_GPSReferenceTime.utran_GPSTimingOfCell.ls_part;
      rrc_ref_time->gps_utran_ref_time.msb_timing =
        asn1_ref_time->utran_GPSReferenceTime.utran_GPSTimingOfCell.ms_part;
      rrc_ref_time->gps_utran_ref_time.sfn =
        asn1_ref_time->utran_GPSReferenceTime.sfn;

      /* Primary Scrambling Code */ 
      if (RRC_MSG_COMMON_BITMASK_IE( asn1_ref_time->utran_GPSReferenceTime,
        rrc_UTRAN_GPSReferenceTime,modeSpecificInfo))
      {
        rrc_ref_time->gps_utran_ref_time.gps_ref_psc_incl = TRUE;

        if(asn1_ref_time->utran_GPSReferenceTime.modeSpecificInfo.t == T_rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd)
        {
          /* UE Only supports FDD, grab primary scrambling code */
          rrc_ref_time->gps_utran_ref_time.psc =
          asn1_ref_time->utran_GPSReferenceTime.modeSpecificInfo.u.fdd->referenceIdentity.primaryScramblingCode;
        }
        else
        {
          WRRC_MSG0_ERROR("TDD info given in MCM. Send failure.");
          return FALSE;
        }
      }
    }

    /* SFN TOW Uncertainty */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        sfn_tow_Uncertainty))
    {
      rrc_ref_time->sfn_tow_uncertainity_incl = TRUE;
      rrc_ref_time->sfn_tow_uncertainity = asn1_ref_time->sfn_tow_Uncertainty;
    }

    /* GPS TOW Assistance List */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        gps_TOW_AssistList))
    {
      rrc_GPS_TOW_AssistList *asn1_info_ptr;
      rrc_gps_tow_assist_struct_type *rrc_info_ptr;

      asn1_info_ptr = &asn1_ref_time->gps_TOW_AssistList;
      rrc_info_ptr = &rrc_ref_time->gps_tow_assist[0];

      /* Traverse TOW Assist List */
      sat_info_cnt = 0;
      for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
      {
        /* Check to see if we have reached the end of the linked list */
        if ( asn1_info_ptr == NULL )
          break;

        rrc_info_ptr->satellite_identity = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
        rrc_info_ptr->tlm_msg = 
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].tlm_Message );
        rrc_info_ptr->tlm_rsvd = 
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].tlm_Reserved );
        rrc_info_ptr->alert = asn1_info_ptr->elem[q_Cnt].alert;
        rrc_info_ptr->anti_spoof = asn1_info_ptr->elem[q_Cnt].antiSpoof;

        rrc_info_ptr++;
        sat_info_cnt++;                                                       
      }

      rrc_ref_time->no_of_gps_tow_assist = sat_info_cnt;
    }
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        ue_Positioning_GPS_ReferenceTimeUncertainty))
    {
      /*TBD*/
      WRRC_MSG0_HIGH("Rel 7 GPS IE's TBD");
    }
  }

  /* Reference Cell Info */

  /* DGPS Corrections - Not currently supported by RRC layer */

  /* UTC Model - Not currently supported by RRC layer */

  /* IONO Model */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r7,
    ue_positioning_GPS_IonosphericModel))
  {
    rrc_gps_iono_struct_type *rrc_iono = &rrc_ptr->gps_iono;

    rrc_UE_Positioning_GPS_IonosphericModel *asn1_iono = 
    &asn1_ptr->ue_positioning_GPS_IonosphericModel;

    rrc_ptr->gps_iono_incl = TRUE;

    rrc_iono->alfa0 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha0 );
    rrc_iono->alfa1 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha1 );
    rrc_iono->alfa2 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha2 );
    rrc_iono->alfa3 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha3 );
    rrc_iono->beta0 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta0 );
    rrc_iono->beta1 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta1 );
    rrc_iono->beta2 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta2 );
    rrc_iono->beta3 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta3 );
  }

  /** UTC model ***/
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r7,
    ue_positioning_GPS_UTC_Model))
  {
    rrc_gps_utc_struct_type *rrc_utc = &rrc_ptr->gps_utc_model;

    rrc_UE_Positioning_GPS_UTC_Model *asn1_utc = &asn1_ptr->ue_positioning_GPS_UTC_Model;

    rrc_ptr->gps_utc_model_incl = TRUE;

    rrc_utc->q_A0   = (uint32)rrcgps_oss_bitstring_to_int(sizeof(uint32), &bs_failed, &asn1_utc->a0);
    rrc_utc->q_A1 =   (uint32)rrcgps_oss_bitstring_to_int(sizeof(uint32), &bs_failed, &asn1_utc->a1);
    rrc_utc->u_DeltaTls = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->delta_t_LS );
    rrc_utc->u_DeltaTlsf = (uint8)rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->delta_t_LSF);
    rrc_utc->u_DN = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->dn );
    rrc_utc->u_Tot = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->t_ot );
    rrc_utc->u_WNlsf = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->wn_lsf );
    rrc_utc->u_WNt = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->wn_t );

  } 

  /* If any of the bitstring conversions failed, indicate this to caller by returning
     false. */
  if ( bs_failed )
  {
    #ifndef WIN32
    WRRC_MSG0_ERROR("ASN.1 to RRC structure conversion encountered errors");
    #endif
    return( FALSE );
  }
  else
    return( TRUE );
}

#endif

/*===========================================================================

FUNCTION   RRCGPS_PROCESS_MCM_R8

DESCRIPTION

    This function processes the rel 8 Measurement Control Message for
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
)
{
  /* Process the A-GPS Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  uint16 deleted_meas_id = 0;
  
  /* Declare MCM config structure that goes to L1 */
  rrc_meas_params_struct_type  *gps_meas_params;
  
  /* Declare ptr to store message received from UTRAN */
  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrc_MeasurementControl_r8_IEs* rrc_mcm_ptr = NULL; 

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  uint8 transaction_id = INVALID_TRANSACTION_ID;
  gps_meas_params = &temp_gps;
  memset(gps_meas_params,0,sizeof(rrc_meas_params_struct_type));  
  gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = FALSE;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.environ_ch_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = FALSE;
  gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.meas_start_incl = TRUE;
  
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)pos_mcm_ptr;
  transaction_id = meas_ctrl_tr_id;

  /* By default, it is set to 0 => 1 */
  gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = 0;

  if(!(dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.t
    == T_rrc_MeasurementControl_criticalExtensions_4_later_than_r4))
  {
    return FAILURE;
  }

  rrc_mcm_ptr =  &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.measurementControl_r8;

  /* Set Transaction Id and Meas Id */
  p_info_ptr->meas_id = gps_meas_params->u.meas_ctrl_params.meas_id =
    (uint8) rrc_mcm_ptr->measurementIdentity;
  
  gps_meas_params->u.meas_ctrl_params.config_conf_ind = TRUE;
  
  gps_meas_params->u.meas_ctrl_params.config_ack_num = (uint8) transaction_id;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan.method = (uint8)RRC_GPS;
  gps_meas_params->params_type = (uint8)RRC_MEAS_CTRL_PARMS;
  
  WRRC_MSG1_HIGH("POS: GPS Meas choice %d",rrc_mcm_ptr->measurementCommand.t);

  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r8_setup))
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_SETUP;
    
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,measurementReportingMode)))
    {
      /* Send Meas Ctrl Failure */
      WRRC_MSG0_ERROR("Meas Reporting Mode missing for SETUP");
      
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
    }
    else
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode =
       (uint8) RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    if(FAILURE == rrcgps_fill_mcm_info_r8(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.setup->u.
      up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id, FALSE))
    {
      WRRC_MSG0_ERROR("filling gps info failed");
      return FAILURE;
    }
  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r8_modify))
  {
    /* Addtl Meas processing later. Reporting crit stored in db later */
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_MODIFY;
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r8_IEs,measurementReportingMode))
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode = (uint8)
        RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_r8_modify,measurementType))
    {
      if(FAILURE == rrcgps_fill_mcm_info_r8(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.modify->
         measurementType.u.up_Measurement, &(gps_meas_params->u.meas_ctrl_params), transaction_id,
         TRUE))
      {
        WRRC_MSG0_ERROR("filling gps info failed");
        return FAILURE;
      }
    } /* End of if of Meas Type present */
  } /* End of modify */
  else
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8) RRC_MEAS_DELETE;
    /* At present just store meas id in meas db */
    for ( count = 0; count < MAX_POS_MEAS; count++ )
    {
      if (pos_meas_id_list[count].meas_id == gps_meas_params->u.meas_ctrl_params.meas_id)
      {
        WRRC_MSG1_HIGH("POS: Meas Id %d deleted from DB", gps_meas_params->u.meas_ctrl_params.meas_id);
        pos_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        pos_meas_id_list[count].meas_validity = NOT_PRESENT;
        pos_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          pos_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        break;
      }
    } /* End of for */
    
    deleted_meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&deleted_meas_id);
    WRRC_MSG0_HIGH("POS: Sending GPS Meas delete Request");
    rrcgps_send_lsm_cmd(gps_meas_params);
    return SUCCESS;
  }
  
  
  WRRC_MSG0_HIGH("POS: Sending GPS Meas Request");
  rrcgps_send_lsm_cmd(gps_meas_params);
  return SUCCESS;

}


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
)
{
  meas_validity_enum_type pos_meas_validity = NOT_PRESENT;
  
  rrc_UE_Positioning_ReportingQuantity_r8* rpt_quan_ptr = NULL;

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  rpt_crit_enum_type rpt_mode;
  boolean bs_failed = FALSE;

  rpt_quan_ptr = &up_Measurement_ptr->ue_positioning_ReportingQuantity;

  meas_ctrl_params_ptr->rpt_quan_incl = TRUE;
  
  
  /* Get Pos Method */
  meas_ctrl_params_ptr->rpt_quan.method_type =
    (uint8) RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  if (rrcgps_verify_positioning_capability_r8(rpt_quan_ptr)
    == FAILURE)
  {
    /* Send Meas Ctrl Failure */
    WRRC_MSG0_ERROR("POS: Pos Capability error");
    
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
    /* Set error t */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
    
  }
  #ifdef FEATURE_GANSS_SUPPORT
  else if((rrc_ganss_supported == TRUE) && (up_Measurement_ptr->m.ue_positioning_GANSS_AssistanceDataPresent))
  {
    rrc_UE_Positioning_GANSS_AssistanceData_r8* ganss_assist_data = &up_Measurement_ptr->ue_positioning_GANSS_AssistanceData;
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r8,
                                       ganssGenericDataList))
    {
      rrc_GANSSGenericDataList_r8 *ganss_gen_list_ptr = NULL;
      uint8 index =0;
      boolean valid_ganss_id_found = FALSE;

      ganss_gen_list_ptr = &(ganss_assist_data->ganssGenericDataList);

  
  
      for(index =0; (index < ganss_gen_list_ptr->n) && (index < MAX_GANSS); index++)
      {
        /*ganssId---Only Glonass is supported*/
        if(ganss_gen_list_ptr->elem[index].m.ganssIdPresent && ganss_gen_list_ptr->elem[index].ganssId ==RRC_GLONASS)
        {
           valid_ganss_id_found = TRUE;
        }
      }
      if(!valid_ganss_id_found)
      {
           //Not sending MCF here
           WRRC_MSG0_HIGH("POS: GANSS ID Not Supported");
          return FAILURE;
      }
     }
  }
  #endif
  /* Get Reporting Quantity */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,
    rrc_UE_Positioning_ReportingQuantity_r8,horizontalAccuracy))
  {
    meas_ctrl_params_ptr->rpt_quan.hor_accuracy_incl = TRUE;
    
    /* Set the 7 bits */
    meas_ctrl_params_ptr->rpt_quan.horizontal_accuracy = (uint8)rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                               &bs_failed, &rpt_quan_ptr->horizontalAccuracy );
  }

  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,verticalAccuracy))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSPositioningMethods))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSTimingOfCellWanted))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSCarrierPhaseMeasurementRequested)))
  {
    /*TBD*/
    WRRC_MSG0_HIGH("Rel 8 IE's Support TBD");
  }
#ifdef FEATURE_GANSS_SUPPORT
if(rrc_ganss_supported == TRUE)
{
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,verticalAccuracy))
  {
    meas_ctrl_params_ptr->rpt_quan.r8_mcm = TRUE;
    meas_ctrl_params_ptr->rpt_quan.vert_accuracy_incl = TRUE;
    meas_ctrl_params_ptr->rpt_quan.vertical_accuracy = (uint32) rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                               &bs_failed, &rpt_quan_ptr->verticalAccuracy );
  }
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,velocityRequested))
  {
    meas_ctrl_params_ptr->rpt_quan.velocity_Req_inc = TRUE;
    meas_ctrl_params_ptr->rpt_quan.velocity_Requested = rpt_quan_ptr->velocityRequested;
  }
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSPositioningMethods))
  {
    meas_ctrl_params_ptr->rpt_quan.ganss_pos_method_incl = TRUE;
    meas_ctrl_params_ptr->rpt_quan.ganss_pos_method = ( uint16 )rrcgps_oss_bitstring_to_int( sizeof( uint16 ), 
                                               &bs_failed, &rpt_quan_ptr->gANSSPositioningMethods ); 
  }  
  
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSTimingOfCellWanted))
  {
    meas_ctrl_params_ptr->rpt_quan.ganss_timing_cell_wanted_incl = TRUE;
    meas_ctrl_params_ptr->rpt_quan.ganss_timing_cell_wanted = ( uint32 )rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                               &bs_failed, &rpt_quan_ptr->gANSSTimingOfCellWanted ); 
  }  
} 
#endif  
  /* AdditionalAssistanceDataRequest */
  if (rpt_quan_ptr->additionalAssistanceDataReq == TRUE)
  {
    meas_ctrl_params_ptr->rpt_quan.additional_assistance_data_request = TRUE;
  }
  
  if (rpt_quan_ptr->gps_TimingOfCellWanted == TRUE)
  {
    meas_ctrl_params_ptr->rpt_quan.sfn_gps_timing_required = TRUE;
  }
  
  
  /* Get Reporting Criteria */
  if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_r7_ue_positioning_ReportingCriteria))
  {
    WRRC_MSG0_ERROR("POS: Event trigg crit unsupported");
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
    /* Set error t */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_r7_noReporting))
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_NO_RPT;
  }
  else
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_PERIODIC_RPT;
    
    /* Get reporting criteria and interval */
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)rrc_ReportingAmount_ra_Infinity;
  
   
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)
      up_Measurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingAmount;
    
  
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_interval = (uint8)
      up_Measurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingInterval;
  }
  if(up_Measurement_ptr->m.ue_positioning_GPS_AssistanceDataPresent )
  {
  /* Now call LSM fn to store Meas Object if FEATURE_MMGPS; call a local function to do the same if FEATURE_CGPS_xxx */
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
    if (rrcgps_convert_assistance_data_r7(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
      &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)
  #else /* this will only be FEATURE_MMGPS */
  /*To be Required from GPS guys*/
/*  if (lsm_convert_assistance_data_r7(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
    &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)*/
  #endif
    {
      WRRC_MSG0_ERROR("POS: Assistance Data conversion failed");
    
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
    
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
    
      return FAILURE;
    }
    meas_ctrl_params_ptr->gps_assistance_data_incl = TRUE;
  }
  
  /* Check if Meas Validity present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(up_Measurement_ptr,
    & rrc_UE_Positioning_Measurement_r8,measurementValidity))
  {
    WRRC_MSG1_HIGH("POS:Meas validity %d (0/d:1/f:2/a)", up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Store Measurement Validity */
    pos_meas_validity = TVM_RET_MEAS_VALIDITY(up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Check if Meas needs to be restarted */
    if (((pos_meas_validity == STATE_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_FACH)) ||
       ((pos_meas_validity == ALL_STATES_BUT_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_DCH)))
    {
      WRRC_MSG0_HIGH("POS: RRC val, MCM val differ");
      meas_ctrl_params_ptr->meas_start_incl = FALSE;
    }
  }    
#ifdef FEATURE_GANSS_SUPPORT  
if(rrc_ganss_supported == TRUE)
{
  rrcgps_fill_meas_ctrl_ganss_assistance_data(meas_ctrl_params_ptr, up_Measurement_ptr);
}
#endif  
  /* Store Pos Parameters in RRC Database */

  switch(meas_ctrl_params_ptr->rpt_crit.rpt_mode)
  {
    case 0: rpt_mode = PERIODIC;
        break;
    case 1: rpt_mode = EVENT;
        break;
    case 2: rpt_mode = ADDTL;
        break;
    case 3: rpt_mode = NOT_AVAILABLE;
        break;
    default:break;
  } 


//CBSS rpt_mode is not assigned so junk value is getting fillled & even thoug return value is being passed TRUE, it is being as FALSE
  if(rrcmeas_config_pos_db(pos_info_ptr, &pos_meas_validity, &rpt_mode, modify_meas_params_incl) == FALSE)
  {
    return FAILURE;
  }
  return SUCCESS;
}


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
)
{

  rrc_positioning_method_type_enum_type pos_method_type;

  tm_umts_cp_wcdma_ue_pos_capability_struct_type pos_capability;  
  
  uecomdef_status_e_type verify_status = SUCCESS;

  
  pos_method_type = RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  /* First get positioning capability from LSM */

  if(rrcgps_cgps_ue_pos_capability_cb == NULL)
  {
    WRRC_MSG0_ERROR("GPS Call back function is NULL");
    return FAILURE;
  }
  else
  {
    rrcgps_cgps_ue_pos_capability_cb(&pos_capability);
  
  }
  WRRC_MSG1_HIGH("POS:LSM/TM returned capability %d",
    pos_capability.network_assisted_gps_support);

  if ( ( rpt_quan_ptr->gps_TimingOfCellWanted == TRUE  ) &&
       ( pos_capability.gps_timing_of_cell_frames_supported == FALSE )
     )
  {
    verify_status = FAILURE;
  }

  if ((rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_otdoa) &&
      (pos_capability.ue_based_otdoa_supported == FALSE))
  {
    WRRC_MSG0_ERROR("Pos Method OTDOA unsupported");
    verify_status = FAILURE;
  }

  if ( rpt_quan_ptr->positioningMethod == rrc_PositioningMethod_cellID )
  {
    WRRC_MSG0_ERROR("Pos Method CELL ID unsupported");
    verify_status = FAILURE;
  }

  
  
  #ifdef FEATURE_GANSS_SUPPORT
  if(rrc_ganss_supported == TRUE)
  {
   boolean bs_failed = FALSE;
   uint8 cnt;
   boolean GANSS_MODE_VALID = FALSE;
  if( (rpt_quan_ptr->m.gANSSPositioningMethodsPresent == TRUE) 
     && (verify_status != FAILURE)) 
  {
    uint16 ganss_pos_method = ( uint16 )rrcgps_oss_bitstring_to_int( sizeof( uint16 ), 
                                               &bs_failed, (ASN1BitStr32 *)&rpt_quan_ptr->gANSSPositioningMethods );

    WRRC_MSG1_HIGH("GANSS: GANSS Positining method is %d",ganss_pos_method);
     // Check if ganss positioning method is not GPS or GLONASS -- We can add MACRO in for 1 and 16 here
    if(!((ganss_pos_method & GANSS_POS_METHOD_GPS) || ((ganss_pos_method & GANSS_POS_METHOD_GPS) &&(ganss_pos_method & GANSS_POS_METHOD_GLONASS))))
    {
       verify_status = FAILURE;
    }

    WRRC_MSG1_HIGH("GANSS: GANSS Supported list incl is %d",pos_capability.ganss_supported_list_incl);
    if(pos_capability.ganss_supported_list_incl ==TRUE)
    {
       //check for gps/ganss mode disabled via GPS Lock
      for(cnt=0;cnt<MAX_GANSS;cnt++)
      {
        WRRC_MSG1_HIGH("GANSS: GANSS Mode is %d",pos_capability.ganss_supported_list[cnt].ganss_mode);
        if(pos_capability.ganss_supported_list[cnt].ganss_mode != GANSS_MODE_NONE)
        {
           GANSS_MODE_VALID = TRUE;
           break;
        }
      }
      // If all GANSS MODE are NONE then we send MCF
      if(GANSS_MODE_VALID == FALSE)
      {
         WRRC_MSG0_HIGH("GANSS MODE INVALID");
         verify_status = FAILURE;
      }
    }
      /*Check GANSS mode only when Ganss positioning method is present*/
  if(pos_capability.ganss_supported_list_incl ==TRUE)
  {
    for(cnt=0;(cnt<pos_capability.list_cnt) && (verify_status != FAILURE) ;cnt++)
    {
          WRRC_MSG2_HIGH("GANSS : GANSS Cap=%d, Req=%d",pos_capability.ganss_supported_list[cnt].ganss_mode,rpt_quan_ptr->methodType);
      switch(rpt_quan_ptr->methodType)
      {
        case rrc_UE_Positioning_MethodType_ue_Assisted:
        if(pos_capability.ganss_supported_list[cnt].ganss_mode == GANSS_MODE_UE_BASED)
        {
           verify_status = FAILURE;
        }
        break;

        case rrc_UE_Positioning_MethodType_ue_Based:
        if(pos_capability.ganss_supported_list[cnt].ganss_mode == GANSS_MODE_NW_BASED)
        {
           verify_status = FAILURE;
        }
        break;
      }
    }
  }
  }
  }
  #endif

  
  if ( verify_status == SUCCESS ) /* continue examining if still good */
  {

    switch ( pos_capability.network_assisted_gps_support )
    {
     
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED:
      #endif
      {
        /* Reject any UE Assisted Requests */
        if (pos_method_type == RRC_UE_ASSISTED )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=UE BASED, Req=UE_ASSISTED");
        }

        break;
      }


      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED:
      #endif
      {
        /* Reject UE Based if we don't support standalone */
        if ( ( pos_method_type == RRC_UE_BASED ) && 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NW BASED, Req=UE_BASED, No Standalone");
        }
        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_BOTH:
      #endif
      {
        /* Never Reject */
        break;
      }

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NONE:
      #endif
      {
        /* Reject everything unless UE Based is requested and standalone is supported */
        if ( ( pos_method_type == RRC_UE_ASSISTED ) || 
             ( pos_capability.standalone_location_method_supported == FALSE ) )
        {
          verify_status = FAILURE;
          WRRC_MSG0_HIGH("POS: REJECT Cap=NONE, Req=UE_ASSISTED or No Standalone");
        }
        break;
      }

      default:
        break;
    }

  } /* if verify_status is still SUCCESS */

  WRRC_MSG1_HIGH("POS:UE cap check status %d",verify_status);  
  
  return verify_status;
}

#ifdef FEATURE_GANSS_SUPPORT
/*====================================
FUNCTION   rrcgps_fill_measured_results_ganss_incl

DESCRIPTION
  This function fills the measurement report extn recieved from LSM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=====================================
*/

 void rrcgps_fill_measured_results_ganss_incl(rrc_UL_DCCH_Message* msg_ptr,rrc_meas_rpt_struct_type* lsm_meas_ptr)
 {
 rrc_GANSSGenericMeasurementInfo *ganss_gen_meas_info_ptr = NULL;
    rrc_GANSSMeasurementSignalList  *ganss_meas_sigList_ptr = NULL;
    rrc_GANSSMeasurementParameters *ganss_meas_paramlist_ptr = NULL;
#ifdef FEATURE_WCDMA_REL8    
    rrc_GANSSGenericMeasurementInfo_v860ext *ganss_gen_meas_info_r8_ptr = NULL;
    rrc_GANSSMeasurementSignalList_v860ext *ganss_meas_sigList_r8_ptr = NULL;
    rrc_GANSSMeasurementParameters_v860ext *ganss_meas_paramlist_r8_ptr = NULL;
#endif   
    uint8 generic_meas_idx = 0, meas_siglist_idx = 0, meas_param_idx = 0, multi_path_ind_type = 0;
    
    rrcgps_set_bitmask_IE_ext770(msg_ptr);
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->m.ue_positioning_Ganss_MeasuredResultsPresent = 1;
	
    WRRC_MSG1_HIGH("GANSS: REFERENCE TIME is %d",lsm_meas_ptr->ganss_measured_results.referenceTime);
    switch(lsm_meas_ptr->ganss_measured_results.referenceTime)
    {
      case GANSS_REFERENCE_TIME_ONLY:
      case GPS_REFERENCE_TIME_ONLY:
	  	
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
            v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
            measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
            referenceTime.t = T_rrc_UE_Positioning_GANSS_MeasuredResults_referenceTime_ganssReferenceTimeOnly;

        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
            v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
            measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
            referenceTime.u.ganssReferenceTimeOnly = rtxMemAllocTypeZ(&enc_ctxt,rrc_GANSSReferenceTimeOnly);
        
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
            v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
            measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
            referenceTime.u.ganssReferenceTimeOnly->gANSS_tod = lsm_meas_ptr->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod;
        
        if(lsm_meas_ptr->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_timeId_incl == TRUE)
        {
          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
            v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
            measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
            referenceTime.u.ganssReferenceTimeOnly->m.gANSS_timeIdPresent = 1;

          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
            measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
             referenceTime.u.ganssReferenceTimeOnly->gANSS_timeId = 
              lsm_meas_ptr->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_timeId;
        }
        if(lsm_meas_ptr->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod_uncertainty_incl == TRUE)
        {
          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
            measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
             referenceTime.u.ganssReferenceTimeOnly->m.gANSS_tod_uncertaintyPresent = 1;
        
          msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
            measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
             referenceTime.u.ganssReferenceTimeOnly->gANSS_tod_uncertainty = 
              lsm_meas_ptr->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod_uncertainty;
        }
        break;
      default:
        WRRC_MSG0_ERROR("Invalid choice\n");
        break;
    }   
    ganss_gen_meas_info_ptr = &(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
     v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
      measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Ganss_MeasuredResults.
      ganssGenericMeasurementInfo);
#ifdef FEATURE_WCDMA_REL8
    RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
      v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions, v860NonCriticalExtensions);

  

    RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
      v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
      v860NonCriticalExtensions.measurementReport_v860ext, measuredResults);

    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
      v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
      v860NonCriticalExtensions.measurementReport_v860ext.measuredResults.t = T_rrc_MeasuredResults_v860ext_ue_positioning_MeasuredResults;

    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        v860NonCriticalExtensions.measurementReport_v860ext.measuredResults.u.ue_positioning_MeasuredResults =
         rtxMemAllocTypeZ(&enc_ctxt,rrc_UE_Positioning_MeasuredResults_v860ext);

    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        v860NonCriticalExtensions.measurementReport_v860ext.measuredResults.u.ue_positioning_MeasuredResults->m.ue_positioning_Ganss_MeasurementResultsPresent = 1;
    
    ganss_gen_meas_info_r8_ptr = &(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
     v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
      v860NonCriticalExtensions.measurementReport_v860ext.measuredResults.u.ue_positioning_MeasuredResults->
      ue_positioning_Ganss_MeasurementResults.ganssGenericMeasurementInfo);    
#endif

    
    ganss_gen_meas_info_ptr->n = lsm_meas_ptr->ganss_measured_results.meas_cnt;

    /*Allocating the GANSSGenericMeasurementInfo list*/
    ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_gen_meas_info_ptr,rrc_GANSSGenericMeasurementInfo_element);
    
    if(ganss_gen_meas_info_ptr->elem == NULL)
    {
      ERR_FATAL("Memory allocation failed",0,0,0);
    }
    
    /*Loop to fill the GANSSGenericMeasurementInfo list*/
    while(ganss_gen_meas_info_ptr->n > generic_meas_idx && generic_meas_idx < MAX_GANSS)
    {
      if(lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].ganssId_incl == TRUE)
      {
        ganss_gen_meas_info_ptr->elem[generic_meas_idx].m.ganssIdPresent = 1;
        ganss_gen_meas_info_ptr->elem[generic_meas_idx].ganssId = lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].ganssId;
      }
      
      ganss_meas_sigList_ptr = &ganss_gen_meas_info_ptr->elem[generic_meas_idx].ganssMeasurementSignalList;
      
      ganss_meas_sigList_ptr->n = lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].list_cnt;

      /*Allocating the GANSSMeasurementSignalList */
      ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_meas_sigList_ptr,rrc_GANSSMeasurementSignalList_element);
    
      if(ganss_meas_sigList_ptr->elem == NULL)
      {
        ERR_FATAL("Memory allocation failed",0,0,0);
      }
      meas_siglist_idx = 0;
      
      /*Loop to fill the GANSSMeasurementSignalList */
      while(ganss_meas_sigList_ptr->n > meas_siglist_idx && meas_siglist_idx < MAX_SGN_TYPE)
      {
        if(lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
            ganssSignalId_incl == TRUE)
        {
          ganss_meas_sigList_ptr->elem[meas_siglist_idx].m.ganssSignalIdPresent = 1;
          ganss_meas_sigList_ptr->elem[meas_siglist_idx].ganssSignalId = lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].
            ganssMeasurementSignalList[meas_siglist_idx].ganssSignalId;
        }
        if(lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
            ganssCodePhaseAmbiguity_incl == TRUE)
        {
          ganss_meas_sigList_ptr->elem[meas_siglist_idx].m.ganssCodePhaseAmbiguityPresent = 1;
          ganss_meas_sigList_ptr->elem[meas_siglist_idx].ganssCodePhaseAmbiguity = lsm_meas_ptr->ganss_measured_results.
            ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].ganssCodePhaseAmbiguity;
        }

        ganss_meas_paramlist_ptr = &ganss_meas_sigList_ptr->elem[meas_siglist_idx].ganssMeasurementParameters;

        ganss_meas_paramlist_ptr->n =  lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].
          ganssMeasurementSignalList[meas_siglist_idx].meas_param_cnt;

        /*Allocating the GANSSMeasurementParameters list */
        ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_meas_paramlist_ptr,rrc_GANSSMeasurementParameters_element);
    
        if(ganss_meas_paramlist_ptr->elem == NULL)
        {
          ERR_FATAL("Memory allocation failed",0,0,0);
        }
        meas_param_idx = 0;

        /*Loop to fill the GANSSMeasurementParameters list */
        while(ganss_meas_paramlist_ptr->n > meas_param_idx)
        {
          ganss_meas_paramlist_ptr->elem[meas_param_idx].satId = lsm_meas_ptr->ganss_measured_results.
           ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
            ganssMeasurementParameters[meas_param_idx].satId;

          ganss_meas_paramlist_ptr->elem[meas_param_idx].cSurNzero = lsm_meas_ptr->ganss_measured_results.
           ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
            ganssMeasurementParameters[meas_param_idx].cSurNzero;

          multi_path_ind_type = lsm_meas_ptr->ganss_measured_results.
                  ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
                  ganssMeasurementParameters[meas_param_idx].multipathIndicator;

          switch(multi_path_ind_type)
          {
            case RRC_GPS_NOT_MEASURED:
              ganss_meas_paramlist_ptr->elem[meas_param_idx].multipathIndicator = 
                 rrc_GANSSMeasurementParameters_multipathIndicator_nm;
              break;
            case RRC_GPS_LOW:
               ganss_meas_paramlist_ptr->elem[meas_param_idx].multipathIndicator =
                  rrc_GANSSMeasurementParameters_multipathIndicator_low;
              break;
            case RRC_GPS_MEDIUM:
               ganss_meas_paramlist_ptr->elem[meas_param_idx].multipathIndicator =
                rrc_GANSSMeasurementParameters_multipathIndicator_medium;
              break;
            case RRC_GPS_HIGH:
               ganss_meas_paramlist_ptr->elem[meas_param_idx].multipathIndicator =
                 rrc_GANSSMeasurementParameters_multipathIndicator_high;
              break;
            default:
              WRRC_MSG0_HIGH("Invalid path indicator type");
          }

          ganss_meas_paramlist_ptr->elem[meas_param_idx].ganssCodePhase = lsm_meas_ptr->ganss_measured_results.
                  ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
                  ganssMeasurementParameters[meas_param_idx].ganssCodePhase;
          
          if(lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].
            ganssMeasurementSignalList[meas_siglist_idx].ganssMeasurementParameters[meas_param_idx].ganssIntegerCodePhase_incl == TRUE)
          {
            ganss_meas_paramlist_ptr->elem[meas_param_idx].m.ganssIntegerCodePhasePresent = 1;
            ganss_meas_paramlist_ptr->elem[meas_param_idx].ganssIntegerCodePhase = lsm_meas_ptr->ganss_measured_results.
              ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
               ganssMeasurementParameters[meas_param_idx].ganssIntegerCodePhase;
          }
          ganss_meas_paramlist_ptr->elem[meas_param_idx].codePhaseRmsError = lsm_meas_ptr->ganss_measured_results.
           ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
            ganssMeasurementParameters[meas_param_idx].codePhaseRmsError;
          
          ganss_meas_paramlist_ptr->elem[meas_param_idx].doppler = lsm_meas_ptr->ganss_measured_results.
           ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
            ganssMeasurementParameters[meas_param_idx].doppler;
          
          meas_param_idx++;  
        }
        meas_siglist_idx++;
      }
      
      generic_meas_idx++;
    }
#ifdef FEATURE_WCDMA_REL8

    ganss_gen_meas_info_r8_ptr->n = lsm_meas_ptr->ganss_measured_results.meas_cnt;

    /*Allocating the GANSSGenericMeasurementInfo-v860ext list*/
    ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_gen_meas_info_r8_ptr,rrc_GANSSGenericMeasurementInfo_v860ext_element);

    if(ganss_gen_meas_info_r8_ptr->elem == NULL)
    {
      ERR_FATAL("Memory allocation failed",0,0,0);
    }
    generic_meas_idx = 0;
    meas_siglist_idx = 0;
    meas_param_idx = 0;
    /*Loop to fill the GANSSGenericMeasurementInfo-v860ext list*/
    while(ganss_gen_meas_info_r8_ptr->n > generic_meas_idx && generic_meas_idx < MAX_GANSS)
    { 
      ganss_meas_sigList_r8_ptr = &ganss_gen_meas_info_r8_ptr->elem[generic_meas_idx].ganssMeasurementSignalList;
      
      ganss_meas_sigList_r8_ptr->n = lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].list_cnt;

      /*Allocating the GANSSMeasurementSignalList-v860ext */
      ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_meas_sigList_r8_ptr,rrc_GANSSMeasurementSignalList_v860ext_element);

      if(ganss_meas_sigList_r8_ptr->elem == NULL)
      {
        ERR_FATAL("Memory allocation failed",0,0,0);
      }
      meas_siglist_idx = 0;
      
      /*Loop to fill the GANSSMeasurementSignalList-v860ext */
      while(ganss_meas_sigList_r8_ptr->n > meas_siglist_idx && meas_siglist_idx < MAX_SGN_TYPE)
      {
        if(lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
            ganssCodePhaseAmbiguityExt_incl == TRUE)
        {
          ganss_meas_sigList_r8_ptr->elem[meas_siglist_idx].m.ganssCodePhaseAmbiguityExtPresent = 1;
          ganss_meas_sigList_r8_ptr->elem[meas_siglist_idx].ganssCodePhaseAmbiguityExt = lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].
            ganssMeasurementSignalList[meas_siglist_idx].ganssCodePhaseAmbiguityExt;
        }       

        ganss_meas_paramlist_r8_ptr = &ganss_meas_sigList_r8_ptr->elem[meas_siglist_idx].ganssMeasurementParameters;

        ganss_meas_paramlist_r8_ptr->n =  lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].
          ganssMeasurementSignalList[meas_siglist_idx].meas_param_cnt;

        /*Allocating the GANSSMeasurementParameters-v860ext list */
        ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_meas_paramlist_r8_ptr,rrc_GANSSMeasurementParameters_v860ext_element);

        if(ganss_meas_paramlist_r8_ptr->elem == NULL)
        {
          ERR_FATAL("Memory allocation failed",0,0,0);
        }
        meas_param_idx = 0;

        /*Loop to fill the GANSSMeasurementParameters-v860ext list */
        while(ganss_meas_paramlist_r8_ptr->n > meas_param_idx)
        {          
          if(lsm_meas_ptr->ganss_measured_results.ganssGenericMeasurementInfo[generic_meas_idx].
            ganssMeasurementSignalList[meas_siglist_idx].ganssMeasurementParameters[meas_param_idx].ganssIntegerCodePhaseExt_incl == TRUE)
          {
            ganss_meas_paramlist_r8_ptr->elem[meas_param_idx].m.ganssIntegerCodePhaseExtPresent = 1;
            ganss_meas_paramlist_r8_ptr->elem[meas_param_idx].ganssIntegerCodePhaseExt = lsm_meas_ptr->ganss_measured_results.
              ganssGenericMeasurementInfo[generic_meas_idx].ganssMeasurementSignalList[meas_siglist_idx].
               ganssMeasurementParameters[meas_param_idx].ganssIntegerCodePhaseExt;
          }
          meas_param_idx++;  
        }
        meas_siglist_idx++;
      }      
      generic_meas_idx++;
    }

#endif 
}

/*===============================================
FUNCTION   rrcgps_fill_measured_results_pos_err_incl

DESCRIPTION
  This function fills the measured results 
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==================================
*/
void rrcgps_fill_measured_results_pos_err_incl(rrc_UL_DCCH_Message* msg_ptr,rrc_meas_rpt_struct_type *lsm_meas_ptr)
{
rrc_UE_Positioning_Error_r7 *rrc_gps_pos_error_r7_ptr = NULL;
    rrc_positioning_error_struct_type *lsm_pos_error_ptr = NULL;    
#ifdef FEATURE_WCDMA_REL8
    rrc_UE_Positioning_Error_v860ext  *rrc_gps_pos_err_r8_ptr = NULL;
#endif

    lsm_pos_error_ptr = &lsm_meas_ptr->pos_error;

    if(!(lsm_pos_error_ptr->error_cause == rrc_UE_Positioning_ErrorCause_r7_notEnoughGANSS_Satellites) &&
       !(lsm_pos_error_ptr->error_cause == rrc_UE_Positioning_ErrorCause_r7_notAccomplishedGANSS_TimingOfCellFrames) &&
       !(lsm_pos_error_ptr->error_cause == rrc_UE_Positioning_ErrorCause_assistanceDataMissing))
    {
       WRRC_MSG0_HIGH("Error cause not related to GANSS filling legacy IEs");
       RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults,rrc_UE_Positioning_MeasuredResults,ue_positioning_Error);

       //rrc_UE_Positioning_Error* rrc_gps_pos_error_ptr = &msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error;
       WRRC_MSG1_HIGH("GANSS: Pos error : Error Cause is %d",lsm_pos_error_ptr->error_cause);
       switch(lsm_pos_error_ptr->error_cause)
       {
         case 0: 
           msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error.errorReason = rrc_UE_Positioning_ErrorCause_notEnoughOTDOA_Cells;
         break;
         case 1: 
           msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error.errorReason = rrc_UE_Positioning_ErrorCause_notEnoughGPS_Satellites;
         break;
         case 3: 
           msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error.errorReason =rrc_UE_Positioning_ErrorCause_notAccomplishedGPS_TimingOfCellFrames;
         break;
         case 4: 
           msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error.errorReason = rrc_UE_Positioning_ErrorCause_undefinedError;
         break;
         case 5: 
           msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error.errorReason = rrc_UE_Positioning_ErrorCause_requestDeniedByUser;
         break;
         case 6: 
           msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error.errorReason = rrc_UE_Positioning_ErrorCause_notProcessedAndTimeout;
         break;
         case 7: 
           msg_ptr->message.u.measurementReport.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error.errorReason = rrc_UE_Positioning_ErrorCause_referenceCellNotServingCell;
         break;
         default: 
         break;/* no action */
       }

    }
    else if((lsm_pos_error_ptr->error_cause == rrc_UE_Positioning_ErrorCause_assistanceDataMissing) && 
            (lsm_pos_error_ptr->addtl_assistance_data_incl) &&
            (!lsm_pos_error_ptr->ganss_addtl_assistance_data_incl))
    {
          WRRC_MSG0_HIGH("GANSS: Additional assistance data is not for GANSS");
          RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
          ue_positioning_MeasuredResults,
          rrc_UE_Positioning_MeasuredResults,ue_positioning_Error);

          if(rrcgps_validate_pos_error(&lsm_meas_ptr->pos_error) == FALSE)
          {
             WRRC_MSG0_HIGH("POS: Ignore Meas Report");
             rrc_free(msg_ptr);
             return;
          }
          rrcgps_set_pos_error(&lsm_meas_ptr->pos_error,
          &msg_ptr->message.u.measurementReport.measuredResults.u.
          ue_positioning_MeasuredResults->ue_positioning_Error);
    }
    else
    {
    WRRC_MSG0_HIGH("GANSS: Filling R7/R8 IEs");
    rrcgps_set_bitmask_IE_ext770(msg_ptr);

    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
       v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
       measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->m.ue_positioning_ErrorPresent = 1;

    rrc_gps_pos_error_r7_ptr = &msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
     v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
     measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error;
#ifdef FEATURE_WCDMA_REL8
if(lsm_pos_error_ptr->error_cause == rrc_UE_Positioning_ErrorCause_assistanceDataMissing)
{
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions, v860NonCriticalExtensions);
    
  
    
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.
          measurementReport_v860ext, measuredResults);
    
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.
          measurementReport_v860ext.measuredResults.t = T_rrc_MeasuredResults_v860ext_ue_positioning_MeasuredResults;
        
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.
          measurementReport_v860ext.measuredResults.u.ue_positioning_MeasuredResults = 
               rtxMemAllocTypeZ(&enc_ctxt,rrc_UE_Positioning_MeasuredResults_v860ext);
        
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.
          measurementReport_v860ext.measuredResults.u.ue_positioning_MeasuredResults->m.ue_positioning_ErrorPresent = 1;
    
        rrc_gps_pos_err_r8_ptr = &msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
          v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.
          measurementReport_v860ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_Error;
}       
#endif
    switch(lsm_pos_error_ptr->error_cause)
    {
      case 0: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_notEnoughOTDOA_Cells;
          break;
      case 1: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_notEnoughGPS_Satellites;
          break;
      case 2: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_assistanceDataMissing;
          break;
      case 3: rrc_gps_pos_error_r7_ptr->errorReason =rrc_UE_Positioning_ErrorCause_r7_notAccomplishedGPS_TimingOfCellFrames;
          break;
      case 4: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_undefinedError;
          break;
      case 5: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_requestDeniedByUser;
          break;
      case 6: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_notProcessedAndTimeout;
          break;
      case 7: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_referenceCellNotServingCell;
          break;
#ifdef FEATURE_GANSS_SUPPORT
      case 8: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_notEnoughGANSS_Satellites;
          break;
      case 9: rrc_gps_pos_error_r7_ptr->errorReason = rrc_UE_Positioning_ErrorCause_r7_notAccomplishedGANSS_TimingOfCellFrames;
          break;
#endif
      default: break;/* no action */
    }

    if((lsm_pos_error_ptr->addtl_assistance_data_incl == TRUE) &&
       (rrc_gps_pos_error_r7_ptr->errorReason == rrc_UE_Positioning_ErrorCause_assistanceDataMissing))
    {
      rrc_gps_pos_error_r7_ptr->m.ue_positioning_GPS_additionalAssistanceDataRequestPresent = 1;

      if(lsm_pos_error_ptr->addtl_assistance_data.almanac_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.almanacRequest = TRUE;
      }
      if(lsm_pos_error_ptr->addtl_assistance_data.utc_model_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.utcModelRequest = TRUE;
      }
      if(lsm_pos_error_ptr->addtl_assistance_data.ionospheric_model_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.ionosphericModelRequest = TRUE;
      }
      if(lsm_pos_error_ptr->addtl_assistance_data.navigation_model_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navigationModelRequest = TRUE;
      }
      if (lsm_pos_error_ptr->addtl_assistance_data.dgps_corrections_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.dgpsCorrectionsRequest = TRUE;
      }
      if (lsm_pos_error_ptr->addtl_assistance_data.reference_location_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.referenceLocationRequest = TRUE;
      }
      if (lsm_pos_error_ptr->addtl_assistance_data.reference_time_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.referenceTimeRequest = TRUE;
      }
      if (lsm_pos_error_ptr->addtl_assistance_data.aquisition_assistance_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.aquisitionAssistanceRequest = TRUE;
      }
      if (lsm_pos_error_ptr->addtl_assistance_data.real_time_integrity_request_incl == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.realTimeIntegrityRequest = TRUE;
      }
      if (lsm_pos_error_ptr->addtl_assistance_data.nac_model_data_req_incl == TRUE)
      {
        rrc_SatDataList *sat_list_ptr = NULL;
        uint8 cnt =0;
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.m.navModelAddDataRequestPresent =1;
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navigationModelRequest = TRUE;
        
        /* GPS Week */
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navModelAddDataRequest.gps_Week =
          lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.gps_week;
        
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navModelAddDataRequest.gps_Toe =
          lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.gps_tow;
        
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.navModelAddDataRequest.tToeLimit =
          lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.age_tolerance;
        
        rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.
          navModelAddDataRequest.satDataList.n = 0;
        
        
        if ((lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites >0) &&
            (lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites <=MAX_NO_OF_SATELLITES)
           )
        {
        
        /* Assign head of list */
        sat_list_ptr = &rrc_gps_pos_error_r7_ptr->ue_positioning_GPS_additionalAssistanceDataRequest.
          navModelAddDataRequest.satDataList  ;
        sat_list_ptr->n =lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites;

        ALLOC_ASN1ARRAY2(&enc_ctxt,sat_list_ptr,rrc_SatData);
        
        if(sat_list_ptr->elem == NULL)
        {
          ERR_FATAL("Failed to allocate memory",0,0,0);
        }    
        
        for (cnt =0; cnt < lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.no_of_satellites; cnt++)
        {
          sat_list_ptr->elem[cnt].satID = lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.sat_data[cnt].sat_id;
          
          sat_list_ptr->elem[cnt].iode = lsm_pos_error_ptr->addtl_assistance_data.navModelAddDataRequest.sat_data[cnt].iode;
        }
      }
     }
    }
    if((lsm_pos_error_ptr->ganss_addtl_assistance_data_incl == TRUE) && 
        (rrc_gps_pos_error_r7_ptr->errorReason == rrc_UE_Positioning_ErrorCause_assistanceDataMissing))
    {
      rrc_GanssRequestedGenericAssistanceDataList *ganss_gen_list = NULL;
      uint8 cnt = 0;
      
      rrc_gps_pos_error_r7_ptr->m.ue_positioning_GANSS_additionalAssistanceDataRequestPresent = 1;
      
      if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganssReferenceTime == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.ganssReferenceTime = TRUE;
      }
      if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganssreferenceLocation == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.ganssreferenceLocation = TRUE;
      }
      if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganssIonosphericModel == TRUE)
      {
        rrc_gps_pos_error_r7_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.ganssIonosphericModel = TRUE;
      }

      if(lsm_pos_error_ptr->ganss_addtl_assistance_data.list_cnt == 0)
      {     
        return;
      }      
      ganss_gen_list = &rrc_gps_pos_error_r7_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.ganssRequestedGenericAssistanceDataList;

      ganss_gen_list->n = lsm_pos_error_ptr->ganss_addtl_assistance_data.list_cnt;

      ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_gen_list,rrc_GanssReqGenericData);
      
      if(ganss_gen_list->elem == NULL)
      {
        ERR_FATAL("Failed to allocate memory",0,0,0);
      }

      for(cnt=0;cnt < lsm_pos_error_ptr->ganss_addtl_assistance_data.list_cnt; cnt++)
      {
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssId_incl == TRUE)
        {
          ganss_gen_list->elem[cnt].m.ganssIdPresent = 1;
          ganss_gen_list->elem[cnt].ganssId = lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssId;
        }

        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssRealTimeIntegrity == TRUE)
        {
          ganss_gen_list->elem[cnt].ganssRealTimeIntegrity = TRUE;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssDifferentialCorrection_incl == TRUE)
        {
          ganss_gen_list->elem[cnt].m.ganssDifferentialCorrectionPresent = 1;
          /*TBD ASN1Bitstring*/
          ganss_gen_list->elem[cnt].ganssDifferentialCorrection.numbits = 8;
    
          ganss_gen_list->elem[cnt].ganssDifferentialCorrection.data[0] =
             lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssDifferentialCorrection;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssAlmanac == TRUE)
        {
          ganss_gen_list->elem[cnt].ganssAlmanac = TRUE;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModel == TRUE)
        {
          ganss_gen_list->elem[cnt].ganssNavigationModel = TRUE;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssTimeModelGNSS_GNSS_incl == TRUE)
        {
          ganss_gen_list->elem[cnt].m.ganssTimeModelGNSS_GNSSPresent = 1;
          /*TBD ASN1Bitstring*/
          
          ganss_gen_list->elem[cnt].ganssTimeModelGNSS_GNSS.numbits = 8;
          ganss_gen_list->elem[cnt].ganssTimeModelGNSS_GNSS.data[0] =
           lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssTimeModelGNSS_GNSS;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssReferenceMeasurementInfo == TRUE)
        {
          ganss_gen_list->elem[cnt].ganssReferenceMeasurementInfo = TRUE;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssUTCModel == TRUE)
        {
          ganss_gen_list->elem[cnt].ganssUTCModel = TRUE;
        }
        if((lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModel == TRUE) ||
           (lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModelAdditionalData_incl == TRUE)
          )
        {
          rrc_SatellitesListRelatedDataList *sat_list_ptr = NULL;
          uint8 sat_list_idx = 0;
          ganss_gen_list->elem[cnt].m.ganssNavigationModelAdditionalDataPresent = 1;

          ganss_gen_list->elem[cnt].ganssNavigationModelAdditionalData.ganssWeek = 
            lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModelAdditionalData.ganssWeek;

          ganss_gen_list->elem[cnt].ganssNavigationModelAdditionalData.ganssToe = 
           lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModelAdditionalData.ganssToe;

          ganss_gen_list->elem[cnt].ganssNavigationModelAdditionalData.t_toeLimit = 
           lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModelAdditionalData.t_toeLimit;

          if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModelAdditionalData.
              satellitesListRelatedDataList_incl == TRUE)
          {
            ganss_gen_list->elem[cnt].ganssNavigationModelAdditionalData.m.satellitesListRelatedDataListPresent = 1;
            sat_list_ptr = &ganss_gen_list->elem[cnt].ganssNavigationModelAdditionalData.satellitesListRelatedDataList;

            if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssNavigationModelAdditionalData.
                sat_cnt == 0)
            {
              continue;
            }
            sat_list_ptr->n = lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].
              ganssNavigationModelAdditionalData.sat_cnt;

            ALLOC_ASN1ARRAY2(&enc_ctxt,sat_list_ptr,rrc_SatellitesListRelatedData);
            
            if(sat_list_ptr->elem == NULL)
            {
              ERR_FATAL("Failed to allocate memory",0,0,0);
            }
            for(sat_list_idx=0; sat_list_idx < sat_list_ptr->n; sat_list_idx++)
            {
              sat_list_ptr->elem[sat_list_idx].satId = lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].
              ganssNavigationModelAdditionalData.satellitesListRelatedDataList[sat_list_idx].satId;

              sat_list_ptr->elem[sat_list_idx].iod = lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].
              ganssNavigationModelAdditionalData.satellitesListRelatedDataList[sat_list_idx].iod;                
            }
          }
        }        
      }
    }
#ifdef FEATURE_WCDMA_REL8
    if((lsm_pos_error_ptr->ganss_addtl_assistance_data_incl == TRUE) && 
       (rrc_gps_pos_error_r7_ptr->errorReason == rrc_UE_Positioning_ErrorCause_assistanceDataMissing))
    {
      rrc_GanssRequestedGenericAssistanceDataList_v860ext *ganss_gen_list_ptr = NULL;
      uint8 cnt = 0;
      
      rrc_gps_pos_err_r8_ptr->m.ue_positioning_GANSS_additionalAssistanceDataRequestPresent = 1;
      if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganssAddIonoModelReq_incl == TRUE)
      {
        rrc_gps_pos_err_r8_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.m.ganssAddIonoModelReqPresent = 1;
        /*TBD ASN1Bitstring*/

        rrc_gps_pos_err_r8_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.ganssAddIonoModelReq.numbits = 2;
        rrc_gps_pos_err_r8_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.ganssAddIonoModelReq.data[0] = 
          lsm_pos_error_ptr->ganss_addtl_assistance_data.ganssAddIonoModelReq;
        
      }

      if(lsm_pos_error_ptr->ganss_addtl_assistance_data.list_cnt == 0)
      {     
        return;
      }
      ganss_gen_list_ptr = &rrc_gps_pos_err_r8_ptr->ue_positioning_GANSS_additionalAssistanceDataRequest.
        ganssRequestedGenericAssistanceDataList;

      ganss_gen_list_ptr->n = lsm_pos_error_ptr->ganss_addtl_assistance_data.list_cnt;

      ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_gen_list_ptr,rrc_GanssReqGenericData_v860ext);
      
      if(ganss_gen_list_ptr->elem == NULL)
      {
        ERR_FATAL("Failed to allocate memory",0,0,0);
      }
      for(cnt=0;cnt < lsm_pos_error_ptr->ganss_addtl_assistance_data.list_cnt; cnt++)
      {
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].sbasId_incl == TRUE)
        {
          ganss_gen_list_ptr->elem[cnt].m.sbasIdPresent = 1;
          ganss_gen_list_ptr->elem[cnt].sbasId = lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].sbasId;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssAddNavigationModel_incl == TRUE)
        {
          ganss_gen_list_ptr->elem[cnt].m.ganssAddNavigationModelPresent = 1;
          ganss_gen_list_ptr->elem[cnt].ganssAddNavigationModel = rrc_GanssReqGenericData_v860ext_ganssAddNavigationModel_true;
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssAddUTCmodel_incl == TRUE)
        {
          ganss_gen_list_ptr->elem[cnt].m.ganssAddUTCmodelPresent = 1;
          ganss_gen_list_ptr->elem[cnt].ganssAddUTCmodel = rrc_GanssReqGenericData_v860ext_ganssAddUTCmodel_true;          
        }
        if(lsm_pos_error_ptr->ganss_addtl_assistance_data.ganss_Req_gen_assis_Data[cnt].ganssAuxInfo_incl == TRUE)
        {
          ganss_gen_list_ptr->elem[cnt].m.ganssAuxInfoPresent = 1;
          ganss_gen_list_ptr->elem[cnt].ganssAuxInfo = rrc_GanssReqGenericData_v860ext_ganssAuxInfo_true; 
        }
      }
    }
  #endif
}
}

/*==============================================
FUNCTION   rrcgps_fill_lsm_assistance_data

DESCRIPTION
  This function fills the lsm assistance data  
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===============================================
*/
void rrcgps_fill_lsm_assistance_data(rrc_DL_DCCH_Message *dcch_msg_ptr,rrc_meas_params_struct_type *lsm_assistance_data_ptr)
{

#ifdef FEATURE_WCDMA_REL7
  rrc_AssistanceDataDelivery_v770ext_IEs *assistance_data_delievery_r7_ptr = NULL;
#endif
#ifdef FEATURE_WCDMA_REL8
  rrc_AssistanceDataDelivery_v860ext_IEs *assistance_data_delievery_r8_ptr = NULL;
#endif
boolean bs_failed = FALSE;
WRRC_MSG0_HIGH("GANSS: Filling LSM assistance data");

#ifdef FEATURE_WCDMA_REL7
  /* Get the pointer to the actual MC message */
  if((dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.m.v3a0NonCriticalExtensionsPresent) &&
     (dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent) &&
     (dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent) &&
     (dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
      m.v770NonCriticalExtensionPresent))
  {
    assistance_data_delievery_r7_ptr = &(dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      v4b0NonCriticalExtensions.v770NonCriticalExtension.assistanceDataDelivery_v770ext);
    /*UE-Positioning-GPS-AssistanceData-v770ext*/
    if(assistance_data_delievery_r7_ptr->m.ue_Positioning_GPS_AssistanceDataPresent)
    {
      lsm_assistance_data_ptr->u.assistance_data_params.ref_time_uncertainty_incl = TRUE;
      /*UE-Positioning-GPS-ReferenceTime-v770ext*/
      if(assistance_data_delievery_r7_ptr->ue_Positioning_GPS_AssistanceData.m.ue_positioning_GPS_ReferenceTimePresent)
      {
        if(assistance_data_delievery_r7_ptr->ue_Positioning_GPS_AssistanceData.ue_positioning_GPS_ReferenceTime.m.
            ue_Positioning_GPS_ReferenceTimeUncertaintyPresent)
        {
           lsm_assistance_data_ptr->u.assistance_data_params.ref_time_uncert.ref_time_incl = TRUE;
           lsm_assistance_data_ptr->u.assistance_data_params.ref_time_uncert.ref_time = assistance_data_delievery_r7_ptr->ue_Positioning_GPS_AssistanceData.
            ue_positioning_GPS_ReferenceTime.ue_Positioning_GPS_ReferenceTimeUncertainty;
        }        
      }
      /*UE-Positioning-GPS-AcquisitionAssistance-v770ext*/
      if(assistance_data_delievery_r7_ptr->ue_Positioning_GPS_AssistanceData.m.ue_positioning_GPS_AcquisitionAssistancePresent)
      {
        if(assistance_data_delievery_r7_ptr->ue_Positioning_GPS_AssistanceData.ue_positioning_GPS_AcquisitionAssistance.m.
            ue_Positioning_GPS_ReferenceTimeUncertaintyPresent)
        {           
           lsm_assistance_data_ptr->u.assistance_data_params.ref_time_uncert.acq_assitance_incl = TRUE;
           lsm_assistance_data_ptr->u.assistance_data_params.ref_time_uncert.acq_assitance = assistance_data_delievery_r7_ptr->ue_Positioning_GPS_AssistanceData.
            ue_positioning_GPS_AcquisitionAssistance.ue_Positioning_GPS_ReferenceTimeUncertainty;
        }
      }
    }
    /*UE-Positioning-GANSS-AssistanceData*/
    if(assistance_data_delievery_r7_ptr->m.ue_positioning_GANSS_AssistanceDataPresent)
    {
      lsm_assistance_data_ptr->u.assistance_data_params.ganss_ref_time_incl = TRUE;
      /*UE-Positioning-GANSS-ReferenceTime*/
      if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.m.ue_positioning_GANSS_ReferenceTimePresent)
      {        
        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time_incl = TRUE;

        if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
          m.ganssDayPresent)
        {
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.day_incl = TRUE;
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.day = 
            assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.ganssDay;          
        }

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.ganss_tod = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.ganssTod;
        
        if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
           m.ganssTimeIdPresent)
        {
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.ganss_Time_Id_incl = TRUE;
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.ganss_Time_Id =
            assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.ganssTimeId;
        }
        
        if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
           m.utran_ganssreferenceTimePresent)
        {
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.utran_ganss_ref_time_incl = TRUE;
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.utran_ganss_ref_time.timing_Of_Cell_Frames =
            assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
             utran_ganssreferenceTime.timingOfCellFrames;

          if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
            utran_ganssreferenceTime.mode.t == T_rrc_UE_Positioning_GANSS_ReferenceTime_mode_fdd)
          {
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.utran_ganss_ref_time.fdd_psc =
            assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
            utran_ganssreferenceTime.mode.u.fdd->primary_CPICH_Info.primaryScramblingCode;
          }
          
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.utran_ganss_ref_time.reference_Sfn =
            assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
             utran_ganssreferenceTime.referenceSfn;
        }

        if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
           m.tutran_ganss_driftRatePresent)
        {
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.tutran_ganss_driftRate_incl = TRUE;
          lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_time.tutran_ganss_driftRate = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ue_positioning_GANSS_ReferenceTime.
            tutran_ganss_driftRate;
        }
      }
      /*uePositioningGanssReferencePosition*/
      if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.m.uePositioningGanssReferencePositionPresent)
      {
        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos_incl = TRUE;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.latitude_sign =
         assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.latitudeSign;
        
        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.latitude = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.latitude;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.longitude = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.longitude;
        
        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.altitude_direction = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.altitudeDirection;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.altitude = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.altitude;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.uncertainity_semi_major = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.uncertaintySemiMajor;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.uncertainity_semi_minor = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.uncertaintySemiMinor;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.orientation_major_axis = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.orientationMajorAxis;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.uncertainity_altitude = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.uncertaintyAltitude;

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_ref_pos.confidence = 
          assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.uePositioningGanssReferencePosition
         .ellipsoidPointAltitudeEllipsoide.confidence;        
      }
      /*GANSSGenericDataList*/
      if(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.m.ganssGenericDataListPresent)
      {
        rrc_GANSSGenericDataList *ganss_gen_list_ptr = NULL;
        uint8 index =0;
        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list_incl = TRUE;

        ganss_gen_list_ptr = &(assistance_data_delievery_r7_ptr->ue_positioning_GANSS_AssistanceData.ganssGenericDataList);

        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.list_cnt = ganss_gen_list_ptr->n;
        
        for(index =0; (index < ganss_gen_list_ptr->n) && (index < MAX_GANSS); index++)
        {
          /*ganssId*/
          if(ganss_gen_list_ptr->elem[index].m.ganssIdPresent)
          {
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ganss_id_incl = TRUE;
            
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ganssId =
            ganss_gen_list_ptr->elem[index].ganssId;
          }
          /*GANSSTimeModelsList*/
          if(ganss_gen_list_ptr->elem[index].m.ganssTimeModelsListPresent)
          {
            rrc_GANSSTimeModelsList *ganss_time_modlist_ptr =NULL;
            uint8 time_mode_idx = 0;
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list_incl = TRUE;
            
            ganss_time_modlist_ptr = &(ganss_gen_list_ptr->elem[index].ganssTimeModelsList);

            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].list_cnt =
              ganss_time_modlist_ptr->n;


//Condition should be for MAX_GANSS-1
            for(time_mode_idx=0;(time_mode_idx < ganss_time_modlist_ptr->n) && (time_mode_idx < MAX_GANSS);time_mode_idx++)
            {            
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_model_ref_time =
               ganss_time_modlist_ptr->elem[time_mode_idx].ganss_timeModelreferenceTime;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_t_a0 =
               ganss_time_modlist_ptr->elem[time_mode_idx].ganss_t_a0;

              if(ganss_time_modlist_ptr->elem[time_mode_idx].m.ganss_t_a1Present)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_t_a1_incl = TRUE;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_t_a1 =
                 ganss_time_modlist_ptr->elem[time_mode_idx].ganss_t_a1;
              }
              switch(ganss_time_modlist_ptr->elem[time_mode_idx].gnss_to_id)
              {
                case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_gps:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_GPS;                    
                  break;
                case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_galileo:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_GALILEO;
                  break;
                case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_qzss:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_QZSS;
                  break;
                case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_glonass:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_GLONASS;
                  break;
                default:
                  break;
              }
              if(ganss_time_modlist_ptr->elem[time_mode_idx].m.ganss_wk_numberPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_wk_number_incl = TRUE;
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_wk_number =
                 ganss_time_modlist_ptr->elem[time_mode_idx].ganss_wk_number;
              }
            } 
          }
          /*UE-Positioning-DGANSSCorrections*/
          if(ganss_gen_list_ptr->elem[index].m.uePositioningDGANSSCorrectionsPresent)
          {
            rrc_DGANSSInfoList *dganss_list_ptr = NULL;
            uint8 info_list_index = 0;
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections_incl = TRUE;
            
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_ref_time =
              ganss_gen_list_ptr->elem[index].uePositioningDGANSSCorrections.dganssreferencetime;

            dganss_list_ptr = &(ganss_gen_list_ptr->elem[index].uePositioningDGANSSCorrections.dganssInfoList);

            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.list_cnt =
              dganss_list_ptr->n;

            for(info_list_index=0;(info_list_index<MAX_SGN_TYPE) && (info_list_index<dganss_list_ptr->n); info_list_index++)
            {             
            
              if(dganss_list_ptr->elem[info_list_index].m.ganssSignalIdPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  sig_id_incl = TRUE;
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                 sig_id = dganss_list_ptr->elem[info_list_index].ganssSignalId;
              }
              switch(dganss_list_ptr->elem[info_list_index].ganssStatusHealth)
              {
                case rrc_GANSS_Status_Health_udre_scale_1dot0:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_UDRE_1_0;
                  break;
                case rrc_GANSS_Status_Health_udre_scale_0dot75:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_UDRE_0_75;
                  break;
                case rrc_GANSS_Status_Health_udre_scale_0dot5:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_UDRE_0_5;                  
                  break;
                case rrc_GANSS_Status_Health_udre_scale_0dot3:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_UDRE_0_3;
                  break;
                case rrc_GANSS_Status_Health_udre_scale_0dot2:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_UDRE_0_2;
                  break;
                case rrc_GANSS_Status_Health_udre_scale_0dot1:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_UDRE_0_1;
                  break;
                case rrc_GANSS_Status_Health_no_data:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_NO_DATA;
                  break;
                case rrc_GANSS_Status_Health_invalid_data:
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    status_health = RRC_INVALID_DATA;
                  break;
                default:
                  break;
              }
              if(dganss_list_ptr->elem[info_list_index].m.dgansssignalInformationListPresent)
              {
                uint8 sig_infolist_idx =0;
                rrc_DGANSSSignalInformationList *dganss_sig_infolist_ptr = NULL;
                
                dganss_sig_infolist_ptr = &dganss_list_ptr->elem[info_list_index].dgansssignalInformationList;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list_incl = TRUE;
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  sat_cnt = dganss_sig_infolist_ptr->n;

                for(sig_infolist_idx = 0; (sig_infolist_idx < dganss_sig_infolist_ptr->n) && (sig_infolist_idx < MAX_GANSS_SAT);sig_infolist_idx++)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].sat_id = dganss_sig_infolist_ptr->elem[sig_infolist_idx].satId;

                  
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].iode = (uint16)rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &dganss_sig_infolist_ptr->elem[sig_infolist_idx].iode_dganss );

                  switch(dganss_sig_infolist_ptr->elem[sig_infolist_idx].udre)
                  {
                    case rrc_UDRE_lessThan1:
                     lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                      dganss_sig_info_list[sig_infolist_idx].udre = LESS_THEN_1;
                      break;
                    case rrc_UDRE_between1_and_4:
                     lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                      dganss_sig_info_list[sig_infolist_idx].udre = BET_1_AND_4;
                      break;
                    case rrc_UDRE_between4_and_8:
                     lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                      dganss_sig_info_list[sig_infolist_idx].udre = BET_4_AND_8;
                      break;
                    case rrc_UDRE_over8:
                     lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                      dganss_sig_info_list[sig_infolist_idx].udre = OVER_8;
                      break;
                    default:
                      break;
                  }

//CBSS - These values defined as REAL can take values REAL(655.04..655.04 by step of 0.32                  
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].prc = dganss_sig_infolist_ptr->elem[sig_infolist_idx].ganss_prc;

//CBSS - These values defined as REAL can take values Real(-4.064..4.064 by step of 0.032)                  
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].rrc = dganss_sig_infolist_ptr->elem[sig_infolist_idx].ganss_rrc;
                }
              }
            }              
          }
          /*UE-Positioning-GANSS-RealTimeIntegrity*/
          if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSRealTimeIntegrityPresent)
          {
            rrc_UE_Positioning_GANSS_RealTimeIntegrity *ganss_rtime_ptr = NULL;
            uint8 rt_index = 0;

            ganss_rtime_ptr = &(ganss_gen_list_ptr->elem[index].uePositioningGANSSRealTimeIntegrity);

            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].RealTime_Integrity_incl = TRUE;

            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].sat_cnt = 
              ganss_rtime_ptr->n;
            
            for(rt_index=0; (rt_index < ganss_rtime_ptr->n) && (rt_index <MAX_GANSS_SAT); rt_index++)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].RealTime_Integrity[rt_index].bad_ganss_satId = 
                ganss_rtime_ptr->elem[rt_index].bad_ganss_satId;

              if(ganss_rtime_ptr->elem[rt_index].m.bad_ganss_signalIdPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].RealTime_Integrity[rt_index].
                  bad_ganss_signalId_incl =  TRUE;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].RealTime_Integrity[rt_index].
                  bad_ganss_signalId = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &ganss_rtime_ptr->elem[rt_index].bad_ganss_signalId);
             
              }
            }            
          }
          /*UE-Positioning-GANSS-ReferenceMeasurementInfo*/
          if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSReferenceMeasurementInfoPresent)
          {
            rrc_GANSSSatelliteInformationList *ganss_sat_info_ptr = NULL;
            uint8 sat_index = 0;

            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info_incl = TRUE;
            
            if(ganss_gen_list_ptr->elem[index].uePositioningGANSSReferenceMeasurementInfo.m.ganssSignalIdPresent)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.ganss_sig_Id_incl = TRUE;
              
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.ganss_sig_Id =
               ganss_gen_list_ptr->elem[index].uePositioningGANSSReferenceMeasurementInfo.ganssSignalId;
            }

            ganss_sat_info_ptr = &(ganss_gen_list_ptr->elem[index].uePositioningGANSSReferenceMeasurementInfo.satelliteInformationList);

            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_cnt = 
              ganss_sat_info_ptr->n;

            for(sat_index=0; (sat_index<ganss_sat_info_ptr->n) && (sat_index<MAX_GANSS_SAT); sat_index++)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].satellite_id =
                ganss_sat_info_ptr->elem[sat_index].ganssSatId;
              
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].doppler =
               ganss_sat_info_ptr->elem[sat_index].dopplerZeroOrder;

              if(ganss_sat_info_ptr->elem[sat_index].m.extraDopplerPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                  extra_doppler_info_incl = TRUE;

//CBSS -- Can take values	Real (-0.2..0.1 by step of 1/210)

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                  extra_doppler_info.doppler_first_order = ganss_sat_info_ptr->elem[sat_index].extraDoppler.dopplerFirstOrder;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                  extra_doppler_info.Doppler_uncertainity = ganss_sat_info_ptr->elem[sat_index].extraDoppler.dopplerUncertainty;
              }

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].code_phase =
               ganss_sat_info_ptr->elem[sat_index].codePhase;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                integer_code_phase = ganss_sat_info_ptr->elem[sat_index].integerCodePhase;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                search_window = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &ganss_sat_info_ptr->elem[sat_index].codePhaseSearchWindow);                

              if(ganss_sat_info_ptr->elem[sat_index].m.azimuthandElevationPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                  azimuth_elevation_incl = TRUE;
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                  azimuth_elevation_info.azimuth = ganss_sat_info_ptr->elem[sat_index].azimuthandElevation.azimuth;


//CBSS Real(0..78.75 by step of 11.25)
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                  azimuth_elevation_info.elevation = ganss_sat_info_ptr->elem[sat_index].azimuthandElevation.elevation;
              }              
            }            
          }
        }
      }
    }
  }    
#endif

#ifdef FEATURE_WCDMA_REL8
  /* Get the pointer to the actual MC message */
  if((dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.m.v3a0NonCriticalExtensionsPresent) &&
     (dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent) &&
     (dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent) &&
     (dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
      m.v770NonCriticalExtensionPresent) && (dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
      v770NonCriticalExtension.m.v860NonCriticalExtensionPresent))
  {
    assistance_data_delievery_r8_ptr = &(dcch_msg_ptr->message.u.assistanceDataDelivery.u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      v4b0NonCriticalExtensions.v770NonCriticalExtension.v860NonCriticalExtension.assistanceDataDelivery_v860ext);

    /*UE-Positioning-GANSS-AssistanceData-v860ext*/
    if(assistance_data_delievery_r8_ptr->m.ue_positioning_GANSS_AssistanceData_v860extPresent)
    {
      lsm_assistance_data_ptr->u.assistance_data_params.ganss_ref_time_incl = TRUE;

      /*GANSSGenericDataList-v860ext*/
      if(assistance_data_delievery_r8_ptr->ue_positioning_GANSS_AssistanceData_v860ext.m.ganssGenericDataListPresent)
      {
        rrc_GANSSGenericDataList_v860ext *gen_datalist_ext_ptr = NULL;
        uint8 index =0;
        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list_incl = TRUE;

        gen_datalist_ext_ptr = &(assistance_data_delievery_r8_ptr->ue_positioning_GANSS_AssistanceData_v860ext.
          ganssGenericDataList);
        
        lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.list_cnt = gen_datalist_ext_ptr->n;
        
        for(index =0; (index<gen_datalist_ext_ptr->n) && (index<MAX_GANSS); index++)
        {
          /*UE-Positioning-GANSS-AddNavigationModels*/
          if(gen_datalist_ext_ptr->elem[index].m.uePositioningGANSSAddNavigationModelsPresent)
          {
            rrc_Ganss_Sat_Info_AddNavList *ganss_sat_infolist_ptr = NULL;
            uint8 sat_index = 0;
            
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
              add_navigation_models_incl = TRUE;

            if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddNavigationModels.m.non_broadcastIndicationPresent)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_navigation_models.non_broadcast_inc = TRUE;

              if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddNavigationModels.non_broadcastIndication == 
                   rrc_UE_Positioning_GANSS_AddNavigationModels_non_broadcastIndication_true)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.non_broadcast_ind = TRUE;
              }              
            }
            ganss_sat_infolist_ptr = &(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddNavigationModels.
              ganssSatInfoNavList);

            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_navigation_models.sat_cnt = ganss_sat_infolist_ptr->n;

            for(sat_index=0;(sat_index < ganss_sat_infolist_ptr->n) && (sat_index<MAX_GANSS_SAT); sat_index++)
            {              
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_navigation_models.sat_info_list[sat_index].sat_id = 
                ganss_sat_infolist_ptr->elem[sat_index].satId;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_navigation_models.sat_info_list[sat_index].sv_health = 
               rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &ganss_sat_infolist_ptr->elem[sat_index].svHealth);              

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_navigation_models.sat_info_list[sat_index].iod =
               rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &ganss_sat_infolist_ptr->elem[sat_index].iod);              

              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.m.navClockModelPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.utc_model_type = NAV_CLK_MODEL;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clock_incl = TRUE;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navToc =                 
                  rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navToc);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navaf2 =                 
                  rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navaf2);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navaf1 =                 
                  rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navaf1);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navaf0 =                 
                  rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navaf0);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navTgd =                 
                  rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navTgd);
              }
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.m.cnavClockModelPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.utc_model_type = CNAV_CLK_MODEL;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnavClockModel_inc = TRUE;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavToc =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavToc);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavTop =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavTop);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavURA0 =
                rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavURA0);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavURA1 =
                rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavURA1);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavURA2 =
                rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavURA2);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavAf2 =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavAf2);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavAf1 =
                rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavAf1);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavAf0 =
                rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavAf0);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavTgd =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavTgd);

                if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl1cpPresent)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cp_inc = TRUE;

                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cp =
                   rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl1cp); 
                  
                }
                if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl1cdPresent)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cd_inc = TRUE;

                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cd =
                   rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl1cd);
                }
                if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl1caPresent)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1ca_inc = TRUE;

                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1ca =
                   rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl1ca);
                  
                }
                if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl2cPresent)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl2c_inc = TRUE;

                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl2c =
                   rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl2c);
                }
                if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl5i5Present)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5i5_inc = TRUE;

                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5i5 =
                   rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl5i5);
                }
                if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl5q5Present)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5q5_inc = TRUE;

                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5q5 =
                   rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl5q5);
                }
                 
              }
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.m.glonassClockModelPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.utc_model_type = GLONASS_CLK_MODEL;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.glo.glonass_Clock_incl = TRUE;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.glo.glonass_clk_model.gloTau =
                 rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.gloTau); 

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.glo.glonass_clk_model.gloGamma =
                 rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.gloGamma); 

                if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.m.gloDeltaTauPresent)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.glo.glonass_clk_model.gloDeltaTau_incl
                    = TRUE;
                  
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_clk_model.u.glo.glonass_clk_model.gloDeltaTau =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.gloDeltaTau);
                }
              }
              //sbasClockModel currently not supported
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.m.glonassECEFPresent)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF_incl = TRUE;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloEn =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloEn);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloP1 =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloP1);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloP2 =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloP2);

                if(ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.m.gloMPresent)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloM_incl=TRUE;
                  
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloM =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloM);                
                }

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloX =
                   rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloX);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloXdot =
                   rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloXdot);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloXdotdot =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloXdotdot);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloY =
                   rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloY);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloYdot =
                   rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloYdot);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloYdotdot =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloYdotdot);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloZ =
                   rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloZ);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloZdot =
                   rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloZdot);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   add_navigation_models.sat_info_list[sat_index].ganss_orbit_model.glonass_ECEF.gloZdotdot =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloZdotdot);
                
              }
            }            
          }
          /*UE-Positioning-GANSS-Almanac-v860ext*/
          if(gen_datalist_ext_ptr->elem[index].m.uePositioningGANSSAlmanacPresent)
          {
            
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
              GANSS_Almanac_incl = TRUE;

            if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAlmanac.m.alm_keplerianGLONASSPresent)
            {
              rrc_GANSS_SAT_Info_Almanac_GLOkpList *ganss_almanaclist_ptr = NULL;
              uint8 almanac_set_index = 0;
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               GANSS_Almanac.almanac_set_incl = TRUE;

              ganss_almanaclist_ptr = &(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAlmanac.alm_keplerianGLONASS.
               sat_info_GLOkpList);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               GANSS_Almanac.sat_cnt = ganss_almanaclist_ptr->n;
              
              for(almanac_set_index=0; 
                  (almanac_set_index<MAX_GANSS_SAT) && (almanac_set_index<ganss_almanaclist_ptr->n);
                  almanac_set_index++)
              {                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmNA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmNA);  
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmnA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmnA); 
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmHA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmHA); 
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmLambdaA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmLambdaA);
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmTlambdaA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmTlambdaA);  
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmDeltaIA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmDeltaIA);     
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAkmDeltaTA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAkmDeltaTA);    
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmDeltaTdotA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmDeltaTdotA);  
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmEpsilonA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmEpsilonA); 
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmOmegaA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmOmegaA); 
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmTauA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmTauA);     
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  GANSS_Almanac.almanac_set[almanac_set_index].gloAlmCA =
                  rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmCA);
                
                if(ganss_almanaclist_ptr->elem[almanac_set_index].m.gloAlmMAPresent)
                {
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   GANSS_Almanac.almanac_set[almanac_set_index].gloAlmMA_incl = TRUE;
                  
                  lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                   GANSS_Almanac.almanac_set[almanac_set_index].gloAlmMA =
                   rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmMA);                  
                }                  
              }
            }              
          }
          /*UE-Positioning-GANSS-AddUTCModels*/
          if(gen_datalist_ext_ptr->elem[index].m.uePositioningGANSSAddUTCModelsPresent)
          {
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
              add_utc_models_incl = TRUE;

            if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.m.utcModel1Present)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_utc_models.utc_model_type = UTC_MODEL1;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcA0 = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcA0);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcA1 = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcA1);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcA2 = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcA2);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcDeltaTls = rrcgps_oss_bitstring_to_int( sizeof(uint8),&bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcDeltaTls);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcTot = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcTot);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcWNot = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcWNot);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcWNlsf = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcWNlsf);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcDN = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcDN);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_1.utcDeltaTlsf = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel1.utcDeltaTlsf);
             
            }
            if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.m.utcModel2Present)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_utc_models.utc_model_type = UTC_MODEL2;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_2.nA = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel2.nA);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_2.tauC = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel2.tauC);

              if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel2.m.deltaUT1Present)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_utc_models.u.utc_model_2.deltaUT1_incl = TRUE;

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_utc_models.u.utc_model_2.b1 = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel2.deltaUT1.b1);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_utc_models.u.utc_model_2.b2 = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel2.deltaUT1.b2);                
              }

              if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel2.m.kpPresent)
              {
                 lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 add_utc_models.u.utc_model_2.kp_incl = TRUE;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                  add_utc_models.u.utc_model_2.kp = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel2.kp);               
              }
              
            }
            if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.m.utcModel3Present)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                add_utc_models.utc_model_type = UTC_MODEL3;

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcA1wnt = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcA1wnt);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcA0wnt = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcA0wnt);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcTot = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcTot);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcWNt = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcWNt);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcDeltaTls = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcDeltaTls);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcWNlsf = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcWNlsf);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcDN = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcDN);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcDeltaTlsf = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcDeltaTlsf);

              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               add_utc_models.u.utc_model_3.utcStandardID = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAddUTCModels.utcModel3.utcStandardID);
            }
          }
          /*UE-Positioning-GANSS-AuxiliaryInfo*/
          if(gen_datalist_ext_ptr->elem[index].m.uePositioningGANSSAuxiliaryInfoPresent)
          {
            uint8 sat_index,n_count;
            lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
              Auxiliary_Info_incl = TRUE;
            


            if(gen_datalist_ext_ptr->elem[index].uePositioningGANSSAuxiliaryInfo.t == 
                T_rrc_UE_Positioning_GANSS_AuxiliaryInfo_ganssID1)
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               Auxiliary_Info.Auxiliary_type = GANSS_ID1;

              n_count = lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               Auxiliary_Info.sat_cnt = gen_datalist_ext_ptr->elem[index].
                uePositioningGANSSAuxiliaryInfo.u.ganssID1->n;
              
              for(sat_index=0;(sat_index<MAX_GANSS_SAT) && (sat_index<n_count);sat_index++)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 Auxiliary_Info.u.ganssID1[sat_index].sv_id = gen_datalist_ext_ptr->elem[index].
                 uePositioningGANSSAuxiliaryInfo.u.ganssID1->elem[sat_index].svID;
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 Auxiliary_Info.u.ganssID1[sat_index].signals_Available = 
                 rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAuxiliaryInfo.u.
                   ganssID1->elem[sat_index].signalsAvailable);    
              }              
            }
            else
            {
              lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               Auxiliary_Info.Auxiliary_type = GANSS_ID3;

              n_count = lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
               Auxiliary_Info.sat_cnt = gen_datalist_ext_ptr->elem[index].uePositioningGANSSAuxiliaryInfo.
               u.ganssID3->n;
              
              for(sat_index=0;(sat_index<MAX_GANSS_SAT) && (sat_index<n_count);sat_index++)
              {
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 Auxiliary_Info.u.ganssID3[sat_index].sv_id = gen_datalist_ext_ptr->elem[index].
                 uePositioningGANSSAuxiliaryInfo.u.ganssID3->elem[sat_index].svID;
                
                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 Auxiliary_Info.u.ganssID3[sat_index].signals_Available = 
                 rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &gen_datalist_ext_ptr->elem[index].uePositioningGANSSAuxiliaryInfo.u.
                   ganssID3->elem[sat_index].signalsAvailable);

                lsm_assistance_data_ptr->u.assistance_data_params.ganss_assist_data.ganss_data_list[index].
                 Auxiliary_Info.u.ganssID3[sat_index].channel_Number = gen_datalist_ext_ptr->elem[index].
                 uePositioningGANSSAuxiliaryInfo.u.ganssID3->elem[sat_index].channelNumber;                
              
              }              
            }
          }          
        }
      }
    }
  }
#endif
}


/*================================================
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

================================================
*/
void rrcgps_fill_meas_ctrl_ganss_assistance_data(rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,rrc_UE_Positioning_Measurement_r8 *up_Measurement_ptr)
{
boolean bs_failed = FALSE;
WRRC_MSG0_HIGH("GANSS: Filling meas ctrl Ganss Assistance Data ");
if (RRC_MSG_COMMON_BITMASK_IE_PTR(up_Measurement_ptr,
    & rrc_UE_Positioning_Measurement_r8,ue_positioning_GANSS_AssistanceData))
  {
    rrc_UE_Positioning_GANSS_AssistanceData_r8* ganss_assist_data = NULL;
    
    meas_ctrl_params_ptr->ganss_assistance_data_incl = TRUE;
    
    ganss_assist_data = &up_Measurement_ptr->ue_positioning_GANSS_AssistanceData;
    
    /*UE-Positioning-GANSS-ReferenceTime*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r8,
                                       ue_positioning_GANSS_ReferenceTime))
    {
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time_incl = TRUE;
      
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.ganssDayPresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.day_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.day = 
          (uint32) ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssDay;
                                                               
      }
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_tod = 
        ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssTod;
      
      /*Not supported by MSM*/
      #if 0
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.ganssTodUncertaintyPresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_tod_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_tod = 
                              ( uint32 )rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                  &bs_failed, &ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssTod );
                                                               
      }
      #endif
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.ganssTimeIdPresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_Time_Id_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_Time_Id = 
          ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssTimeId;
        
      }
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.utran_ganssreferenceTimePresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time.timing_Of_Cell_Frames = 
                      ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.timingOfCellFrames;
        
        if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.mode.t == 
             T_rrc_UE_Positioning_GANSS_ReferenceTime_mode_fdd)
        {
           meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time.fdd_psc = 
               ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.mode.u.fdd->
                                    primary_CPICH_Info.primaryScramblingCode;
        }
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time.reference_Sfn =
          ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.referenceSfn;
      }
       /*START FROm HERE*/
      /*Not supported*/
      #if 0
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.tutran_ganss_driftRatePresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.tutran_ganss_driftRate_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.tutran_ganss_driftRate = 
          ganss_assist_data->ue_positioning_GANSS_ReferenceTime.tutran_ganss_driftRate;
      }
      #endif      
      
    }
    
    /*uePositioningGanssReferencePosition*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r8,
                                       uePositioningGanssReferencePosition))
    {
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos_incl = TRUE;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.latitude_sign =
       ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.latitudeSign;
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.latitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.latitude;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.longitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.longitude;
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.altitude_direction = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.altitudeDirection;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.altitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.altitude;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.uncertainity_semi_major = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.uncertaintySemiMajor;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.uncertainity_semi_minor = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.uncertaintySemiMinor;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.orientation_major_axis = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.orientationMajorAxis;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.uncertainity_altitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.uncertaintyAltitude;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.confidence = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.confidence;        
    }
    /*UE-Positioning-GANSS-AddIonoModel*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r8,
                                       uePositioningGanssAddIonoModel))
    {
      /*UE-Positioning-GPS-IonosphericModel*/
      rrc_UE_Positioning_GPS_IonosphericModel *alpha_beta_ptr = NULL; 
             
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model_incl = TRUE;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.data_id = 
        ( uint8 )rrcgps_oss_bitstring_to_int( sizeof( uint8 ),&bs_failed,
                  &ganss_assist_data->uePositioningGanssAddIonoModel.dataID);

      alpha_beta_ptr = &ganss_assist_data->uePositioningGanssAddIonoModel.alpha_beta_parameters;
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa0 =
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed,&alpha_beta_ptr->alpha0 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa1 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->alpha1 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa2 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->alpha2 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa3 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->alpha3 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta0 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta0 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta1 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta1 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta2 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta2 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta3 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta3 );
      

    }
    /*GANSSGenericDataList-r8*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r8,
                                       ganssGenericDataList))
    {
      rrc_GANSSGenericDataList_r8 *ganss_gen_list_ptr = NULL;
      uint8 index =0;
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list_incl = TRUE;      

      ganss_gen_list_ptr = &(ganss_assist_data->ganssGenericDataList);

      meas_ctrl_params_ptr->ganss_assistance_data.list_cnt = ganss_gen_list_ptr->n;
      
      for(index =0; (index < ganss_gen_list_ptr->n) && (index < MAX_GANSS); index++)
      {
        /*ganssId*/
        if(ganss_gen_list_ptr->elem[index].m.ganssIdPresent)
        {
          rrcgps_fill_generic_data_ganss_id(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].ganssId,index);
        }
        /*GANSSTimeModelsList*/
        if(ganss_gen_list_ptr->elem[index].m.ganssTimeModelsListPresent)
        {
            rrcgps_fill_generic_data_time_model_list(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].ganssTimeModelsList,index);
        }
        /*UE-Positioning-DGANSSCorrections*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningDGANSSCorrectionsPresent)
        {
          rrc_DGANSSInfoList *dganss_list_ptr = NULL;
          uint8 info_list_index = 0;
          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections_incl = TRUE;
          
          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_ref_time =
            ganss_gen_list_ptr->elem[index].uePositioningDGANSSCorrections.dganssreferencetime;

          dganss_list_ptr = &(ganss_gen_list_ptr->elem[index].uePositioningDGANSSCorrections.dganssInfoList);

          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.list_cnt =
            dganss_list_ptr->n;

          for(info_list_index=0;(info_list_index<MAX_SGN_TYPE) && (info_list_index<dganss_list_ptr->n); info_list_index++)
          {             
          
            if(dganss_list_ptr->elem[info_list_index].m.ganssSignalIdPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                sig_id_incl = TRUE;
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
               sig_id = dganss_list_ptr->elem[info_list_index].ganssSignalId;
            }
            switch(dganss_list_ptr->elem[info_list_index].ganssStatusHealth)
            {
              case rrc_GANSS_Status_Health_udre_scale_1dot0:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_1_0;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot75:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_75;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot5:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_5;                  
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot3:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_3;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot2:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_2;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot1:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_1;
                break;
              case rrc_GANSS_Status_Health_no_data:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_NO_DATA;
                break;
              case rrc_GANSS_Status_Health_invalid_data:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_INVALID_DATA;
                break;
              default:
                break;
            }
            if(dganss_list_ptr->elem[info_list_index].m.dgansssignalInformationListPresent)
            {
              uint8 sig_infolist_idx =0;
              rrc_DGANSSSignalInformationList *dganss_sig_infolist_ptr = NULL;
              
              dganss_sig_infolist_ptr = &dganss_list_ptr->elem[info_list_index].dgansssignalInformationList;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                dganss_sig_info_list_incl = TRUE;
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                sat_cnt = dganss_sig_infolist_ptr->n;

              for(sig_infolist_idx = 0; (sig_infolist_idx < dganss_sig_infolist_ptr->n) && (sig_infolist_idx < MAX_GANSS_SAT);sig_infolist_idx++)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].sat_id = dganss_sig_infolist_ptr->elem[sig_infolist_idx].satId;

                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].iode = (uint16)rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &dganss_sig_infolist_ptr->elem[sig_infolist_idx].iode_dganss );

                switch(dganss_sig_infolist_ptr->elem[sig_infolist_idx].udre)
                {
                  case rrc_UDRE_lessThan1:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = LESS_THEN_1;
                    break;
                  case rrc_UDRE_between1_and_4:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = BET_1_AND_4;
                    break;
                  case rrc_UDRE_between4_and_8:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = BET_4_AND_8;
                    break;
                  case rrc_UDRE_over8:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = OVER_8;
                    break;
                  default:
                    break;
                }
                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].prc = dganss_sig_infolist_ptr->elem[sig_infolist_idx].ganss_prc;
                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].rrc = dganss_sig_infolist_ptr->elem[sig_infolist_idx].ganss_rrc;
              }
            }
          }              
        }
        /*UE-Positioning-GANSS-AddNavigationModels*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSAddNavigationModelsPresent)
        {
           rrcgps_fill_generic_data_add_navigation_models(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSAddNavigationModels,index);
        }
        
        /*UE-Positioning-GANSS-RealTimeIntegrity*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSRealTimeIntegrityPresent)
        {
          rrcgps_fill_generic_data_real_time_integrity(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSRealTimeIntegrity,index);
        }
        /*UE-Positioning-GANSS-ReferenceMeasurementInfo*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSReferenceMeasurementInfoPresent)
        {
          rrcgps_fill_generic_data_ref_meas_info(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSReferenceMeasurementInfo,index);
        }
        /*UE-Positioning-GANSS-Almanac-r8*/                  
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSAlmanacPresent)
        {
          rrcgps_fill_generic_data_almanac_r8(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSAlmanac,index);
        }

        /*UE-Positioning-GANSS-AddUTCModels*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSAddUTCModelsPresent)
        {
          rrcgps_fill_generic_data_add_utc_models(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSAddUTCModels,index);
        }
        /*UE-Positioning-GANSS-AuxiliaryInfo*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSAuxiliaryInfoPresent)
        {
          rrcgps_fill_generic_data_aux_info(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSAuxiliaryInfo,index);
        }
      }
    }    
  }
}

/* ===============================================================
FUNCTION   rrcgps_fill_measured_result_pos_est_incl

DESCRIPTION
  This function fills the measured results if positioning estimate included
  

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None


==================================================================*/

boolean rrcgps_fill_measured_result_pos_est_incl(rrc_UL_DCCH_Message* msg_ptr,rrc_meas_rpt_struct_type *lsm_meas_ptr)
 {
 //rrc_UE_Positioning_PositionEstimateInfo_v770ext_r7 *rrc_gps_pos_est_ptr = NULL;
    rrc_positioning_estimate_struct_type *lsm_pos_est_ptr = NULL ;

    ASN1BitStr32 *temp = NULL;    
    
    rrcgps_set_bitmask_IE_ext770(msg_ptr);

        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->m.ue_positioning_PositionEstimateInfoPresent =1;
        
        msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.
        referenceTimeOptions.t = T_rrc_UE_Positioning_PositionEstimateInfo_v770ext_referenceTimeOptions_earlier_than_r7;

    if(lsm_meas_ptr->positioning_estimate.positioning_estimate != GANSS_REFERENCE_TIME_ONLY)
    {
    if (rrcgps_validate_pos_estimate(&lsm_meas_ptr->positioning_estimate) == FALSE)
    {
      
      rrc_free(msg_ptr);
      return FAILURE;
    }

    RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_positioning_MeasuredResults,
      rrc_UE_Positioning_MeasuredResults,ue_positioning_PositionEstimateInfo);
    
    rrcgps_set_pos_estimate(&lsm_meas_ptr->positioning_estimate,
      &msg_ptr->message.u.measurementReport.measuredResults.u.
      ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo);
    
   }
  if(lsm_meas_ptr->positioning_estimate.positioning_estimate == GANSS_REFERENCE_TIME_ONLY)
  {
    //rrcgps_set_bitmask_IE_ext770(msg_ptr);
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->m.ue_positioning_PositionEstimateInfoPresent =1;
        
    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.
        referenceTimeOptions.t = T_rrc_UE_Positioning_PositionEstimateInfo_v770ext_referenceTimeOptions_r7;

    msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
        v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.
        referenceTimeOptions.u.r7 = rtxMemAllocTypeZ(&enc_ctxt,rrc_UE_Positioning_PositionEstimateInfo_v770ext_r7);

    if(rrcgps_validate_pos_estimate_r7(&lsm_meas_ptr->positioning_estimate) == FALSE)
    {
      
      rrc_free(msg_ptr);
      return FAILURE;
    }
        
    rrcgps_set_pos_estimate_r7(&lsm_meas_ptr->positioning_estimate,
      (rrc_UE_Positioning_PositionEstimateInfo_v770ext_r7 *)&msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
      v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
      measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.
      referenceTimeOptions.u.r7->referenceTime);
  }

    lsm_pos_est_ptr = &lsm_meas_ptr->positioning_estimate;

    /*TBD to check the correct logic*/
    temp = &msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
       v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.
        positionData;

    temp->numbits = 16;
    temp->data[0] = lsm_pos_est_ptr->positionData[0];
    temp->data[1] = lsm_pos_est_ptr->positionData[1];

    if(lsm_pos_est_ptr->velocityEstimate_incl == TRUE)
    {
      rrc_VelocityEstimate *rrc_gps_pos_vel_ptr = NULL;

       msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
       v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.m.velocityEstimatePresent = 1;
      
      rrc_gps_pos_vel_ptr = &msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
       v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
        measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.
        velocityEstimate;
      
      switch(lsm_pos_est_ptr->velocityEstimate.velocity_estimate_choice)
      {
        case HORIZONTAL_VELOCITY:
          rrc_gps_pos_vel_ptr->t =  T_rrc_VelocityEstimate_horizontalVelocity;

          rrc_gps_pos_vel_ptr->u.horizontalVelocity
            =rtxMemAllocTypeZ(&enc_ctxt,rrc_HorizontalVelocity);

          rrc_gps_pos_vel_ptr->u.horizontalVelocity->bearing = lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing;
          rrc_gps_pos_vel_ptr->u.horizontalVelocity->horizontalSpeed = lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed;
          break;

        case HORIZONTAL_WITH_VERTICAL_VELOCITY:
          rrc_gps_pos_vel_ptr->t = T_rrc_VelocityEstimate_horizontalWithVerticalVelocity;
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocity            
           =rtxMemAllocTypeZ(&enc_ctxt,rrc_HorizontalWithVerticalVelocity);

          switch(lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection)
          {
            case UPWARD:
              rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocity->verticalSpeedDirection =
                rrc_HorizontalWithVerticalVelocityAndUncertainty_verticalSpeedDirection_upward;
              break;
            case DOWNWARD:
              rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocity->verticalSpeedDirection =
                rrc_HorizontalWithVerticalVelocityAndUncertainty_verticalSpeedDirection_downward;
              break;
            default:

              break;
          }      
          
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocity->bearing = lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing;
          
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocity->horizontalSpeed
            = lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed;
          
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocity->verticalSpeed = lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeed;
          break;

        case HORIZONTAL_VELOCITY_WITH_UNCERTAINTY:
          rrc_gps_pos_vel_ptr->t = T_rrc_VelocityEstimate_horizontalVelocityWithUncertainty;
          rrc_gps_pos_vel_ptr->u.horizontalVelocityWithUncertainty            
           =rtxMemAllocTypeZ(&enc_ctxt,rrc_HorizontalVelocityWithUncertainty);

          rrc_gps_pos_vel_ptr->u.horizontalVelocityWithUncertainty->bearing = lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing;
          
          rrc_gps_pos_vel_ptr->u.horizontalVelocityWithUncertainty->horizontalSpeed 
            = lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed;
          
          rrc_gps_pos_vel_ptr->u.horizontalVelocityWithUncertainty->horizontalSpeedUncertainty 
            = lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeedUncertainty;
          
          break;

        case HORIZONTAL_WITH_VERTICAL_VELOCITY_AND_UNCERTAINTY:
          rrc_gps_pos_vel_ptr->t = T_rrc_VelocityEstimate_horizontalWithVerticalVelocityAndUncertainty;
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocityAndUncertainty            
           =rtxMemAllocTypeZ(&enc_ctxt,rrc_HorizontalWithVerticalVelocityAndUncertainty);

          switch(lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalSpeedDirection)
          {
            case UPWARD:
              rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocityAndUncertainty->verticalSpeedDirection =
                rrc_HorizontalWithVerticalVelocityAndUncertainty_verticalSpeedDirection_upward;
              break;
            case DOWNWARD:
              rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocityAndUncertainty->verticalSpeedDirection =
                rrc_HorizontalWithVerticalVelocityAndUncertainty_verticalSpeedDirection_downward;
              break;
            default:
              break;
          }
          
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocityAndUncertainty->bearing 
            = lsm_pos_est_ptr->velocityEstimate.velocity_type.bearing;
          
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocityAndUncertainty->horizontalSpeed 
            = lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalSpeed;
          
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocityAndUncertainty->horizontalUncertaintySpeed 
            = lsm_pos_est_ptr->velocityEstimate.velocity_type.horizontalUncertaintySpeed;
          
          rrc_gps_pos_vel_ptr->u.horizontalWithVerticalVelocityAndUncertainty->verticalUncertaintySpeed 
            = lsm_pos_est_ptr->velocityEstimate.velocity_type.verticalUncertaintySpeed;
          break;

        default:
         msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
         v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
         measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults->ue_positioning_PositionEstimateInfo.m.velocityEstimatePresent = 0;
          break;
      }
    }
 return SUCCESS;
}
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


void rrcgps_set_bitmask_IE_ext770(rrc_UL_DCCH_Message* msg_ptr)
{
       if(!RRC_CHECK_MSG_TYPE_IE(msg_ptr->message.u.measurementReport,v390nonCriticalExtensionsPresent))
       {
           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport,v390nonCriticalExtensions);
       }
       if(!RRC_CHECK_MSG_TYPE_IE(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions, laterNonCriticalExtensionsPresent))
       {
           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions, laterNonCriticalExtensions);
       }
       if(!RRC_CHECK_MSG_TYPE_IE(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions, v4b0NonCriticalExtensionsPresent))
       {
           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions, v4b0NonCriticalExtensions);
 
       }
           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions, v590NonCriticalExtensions);

           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
            v590NonCriticalExtensions, v5b0NonCriticalExtensions);

           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v5b0NonCriticalExtensions, v690NonCriticalExtensions);

           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions, v770NonCriticalExtensions);

           RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
           measurementReport_v770ext, measuredResults);
           
           msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
           measurementReport_v770ext.measuredResults.t = T_rrc_MeasuredResults_v770ext_ue_positioning_MeasuredResults;

           if(NULL == msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
              v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
              measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults)
           {
              msg_ptr->message.u.measurementReport.v390nonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
              v590NonCriticalExtensions.v5b0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
              measurementReport_v770ext.measuredResults.u.ue_positioning_MeasuredResults =
              rtxMemAllocTypeZ(&enc_ctxt,rrc_UE_Positioning_MeasuredResults_v770ext);
           }

}


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
void rrcgps_fill_generic_data_ganss_id(rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,uint8 ganss_id,uint8 index)
{
  meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ganss_id_incl = TRUE;
  meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ganssId =ganss_id;
}

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
void rrcgps_fill_generic_data_time_model_list(rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,rrc_GANSSTimeModelsList ganssTimeModelsList,uint8 index)
{
   rrc_GANSSTimeModelsList *ganss_time_modlist_ptr =NULL;
   uint8 time_mode_idx = 0;
   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list_incl = TRUE;
          
   ganss_time_modlist_ptr = &(ganssTimeModelsList);

   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].list_cnt = ganss_time_modlist_ptr->n;

//(time_mode_idx < MAX_GANSS-1) as per spec -- should be changed below
          for(time_mode_idx=0;(time_mode_idx < ganss_time_modlist_ptr->n) && (time_mode_idx < MAX_GANSS);time_mode_idx++)
          {            
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_model_ref_time =
             ganss_time_modlist_ptr->elem[time_mode_idx].ganss_timeModelreferenceTime;

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_t_a0 =
             ganss_time_modlist_ptr->elem[time_mode_idx].ganss_t_a0;

            if(ganss_time_modlist_ptr->elem[time_mode_idx].m.ganss_t_a1Present)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_t_a1_incl = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_t_a1 =
               ganss_time_modlist_ptr->elem[time_mode_idx].ganss_t_a1;
            }
            switch(ganss_time_modlist_ptr->elem[time_mode_idx].gnss_to_id)
            {
              case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_gps:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_GPS ;
                break;
              case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_galileo:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_GALILEO;
                break;
              case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_qzss:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_QZSS;
                break;
              case rrc_UE_Positioning_GANSS_TimeModel_gnss_to_id_glonass:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_to_id = GANSS_ID_GLONASS;
                break;
              default:
                break;
            }
            if(ganss_time_modlist_ptr->elem[time_mode_idx].m.ganss_wk_numberPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_wk_number_incl = TRUE;
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].time_mode_list[time_mode_idx].ganss_wk_number =
               ganss_time_modlist_ptr->elem[time_mode_idx].ganss_wk_number;
            }
          }
  
}

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
void rrcgps_fill_generic_data_add_navigation_models(rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,rrc_UE_Positioning_GANSS_AddNavigationModels uePositioningGANSSAddNavigationModels,uint8 index)
{
  rrc_Ganss_Sat_Info_AddNavList *ganss_sat_infolist_ptr = NULL;
          uint8 sat_index = 0;
          boolean bs_failed = FALSE;
          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].
            add_navigation_models_incl = TRUE;

          if(uePositioningGANSSAddNavigationModels.m.non_broadcastIndicationPresent)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.non_broadcast_inc = TRUE;

            if(uePositioningGANSSAddNavigationModels.non_broadcastIndication == 
                 rrc_UE_Positioning_GANSS_AddNavigationModels_non_broadcastIndication_true)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.non_broadcast_ind = TRUE;
            }              
          }
          ganss_sat_infolist_ptr = &(uePositioningGANSSAddNavigationModels.ganssSatInfoNavList);

          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].
              add_navigation_models.sat_cnt = ganss_sat_infolist_ptr->n;

          for(sat_index=0;(sat_index < ganss_sat_infolist_ptr->n) && (sat_index<MAX_GANSS_SAT); sat_index++)
          {              
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
              sat_info_list[sat_index].sat_id = ganss_sat_infolist_ptr->elem[sat_index].satId;

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
              sat_info_list[sat_index].sv_health = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
               &ganss_sat_infolist_ptr->elem[sat_index].svHealth);              

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
              sat_info_list[sat_index].iod = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
               &ganss_sat_infolist_ptr->elem[sat_index].iod);              
            /*NAVclockModel*/
            if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.m.navClockModelPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
                sat_info_list[sat_index].ganss_clk_model.utc_model_type = NAV_CLK_MODEL;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
                sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clock_incl = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
                sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navToc = rrcgps_oss_bitstring_to_int( sizeof(uint16), 
                 &bs_failed,&ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navToc);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
                sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navaf2 = rrcgps_oss_bitstring_to_int( sizeof(uint8), 
                  &bs_failed, &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navaf2);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
                sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navaf1 = rrcgps_oss_bitstring_to_int( sizeof(uint16), 
                  &bs_failed,&ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navaf1);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
                sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navaf0 = rrcgps_oss_bitstring_to_int( sizeof(uint32), 
                  &bs_failed, &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navaf0);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.
                sat_info_list[sat_index].ganss_clk_model.u.nav.nav_clk_model.navTgd = rrcgps_oss_bitstring_to_int( sizeof(uint8),
                  &bs_failed, &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.navClockModel.navTgd);
            }
            /*CNAVclockModel*/
            if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.m.cnavClockModelPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.utc_model_type = CNAV_CLK_MODEL;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnavClockModel_inc = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavToc = rrcgps_oss_bitstring_to_int( sizeof(uint16),&bs_failed,
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavToc);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavTop = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavTop);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavURA0 = rrcgps_oss_bitstring_to_int( sizeof(uint8),&bs_failed,
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavURA0);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavURA1 = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavURA1);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavURA2 = rrcgps_oss_bitstring_to_int( sizeof(uint8),&bs_failed,
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavURA2);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavAf2 = rrcgps_oss_bitstring_to_int( sizeof(uint16),&bs_failed,
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavAf2);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavAf1 = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed,
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavAf1);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavAf0 =rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavAf0);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.cnav.cnav_clk_model.cnavTgd = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavTgd);

              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl1cpPresent)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cp_inc = TRUE;

                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cp = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl1cp);                
              }
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl1cdPresent)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cd_inc = TRUE;

                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1cd = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl1cd);
              }
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl1caPresent)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1ca_inc = TRUE;

                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl1ca = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl1ca);
                
              }
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl2cPresent)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl2c_inc = TRUE;

                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl2c = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl2c);
              }
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl5i5Present)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5i5_inc = TRUE;

                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5i5 = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl5i5);
              }
              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.m.cnavISCl5q5Present)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5q5_inc = TRUE;

                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.cnav.cnav_clk_model.cnavISCl5q5 = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                   &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.cnavClockModel.cnavISCl5q5);
              }               
            }
            /*GLONASSclockModel*/
            if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.m.glonassClockModelPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.utc_model_type = GLONASS_CLK_MODEL;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.glo.glonass_Clock_incl = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.glo.glonass_clk_model.gloTau = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                 &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.gloTau); 

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_clk_model.u.glo.glonass_clk_model.gloGamma = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, 
                 &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.gloGamma); 

              if(ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.m.gloDeltaTauPresent)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.glo.glonass_clk_model.gloDeltaTau_incl = TRUE;
                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_clk_model.u.glo.glonass_clk_model.gloDeltaTau = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssClockModel.glonassClockModel.gloDeltaTau);
              }
            }
            /*sbasClockModel currently not supported*/
            
            /*UE-Positioning-GANSS-AddOrbitModels*/
            if(ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.m.glonassECEFPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF_incl = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloEn = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloEn);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloP1 = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloP1);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloP2 = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloP2);

              if(ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.m.gloMPresent)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_orbit_model.glonass_ECEF.gloM_incl=TRUE;
                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                  ganss_orbit_model.glonass_ECEF.gloM = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                    &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloM);                
              }

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloX = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloX);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloXdot = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloXdot);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloXdotdot = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloXdotdot);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloY = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloY);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloYdot = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloYdot);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloYdotdot = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloYdotdot);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloZ = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloZ);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloZdot = rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloZdot);

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_navigation_models.sat_info_list[sat_index].
                ganss_orbit_model.glonass_ECEF.gloZdotdot = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &ganss_sat_infolist_ptr->elem[sat_index].ganssOrbitModel.glonassECEF.gloZdotdot);
              
            }
          }
}
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
void rrcgps_fill_generic_data_real_time_integrity(rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,rrc_UE_Positioning_GANSS_RealTimeIntegrity uePositioningGANSSRealTimeIntegrity,uint8 index)
{
  rrc_UE_Positioning_GANSS_RealTimeIntegrity *ganss_rtime_ptr = NULL;
          uint8 rt_index = 0;
          boolean bs_failed = FALSE;
          ganss_rtime_ptr = &(uePositioningGANSSRealTimeIntegrity);

          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].RealTime_Integrity_incl = TRUE;

          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].sat_cnt = 
            ganss_rtime_ptr->n;
          
          for(rt_index=0; (rt_index < ganss_rtime_ptr->n) && (rt_index <MAX_GANSS_SAT); rt_index++)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].RealTime_Integrity[rt_index].bad_ganss_satId = 
              ganss_rtime_ptr->elem[rt_index].bad_ganss_satId;

            if(ganss_rtime_ptr->elem[rt_index].m.bad_ganss_signalIdPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].RealTime_Integrity[rt_index].
                bad_ganss_signalId_incl =  TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].RealTime_Integrity[rt_index].
                bad_ganss_signalId = rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, 
                  &ganss_rtime_ptr->elem[rt_index].bad_ganss_signalId);           
            }
          }    
}

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
void rrcgps_fill_generic_data_ref_meas_info(rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,rrc_UE_Positioning_GANSS_ReferenceMeasurementInfo uePositioningGANSSReferenceMeasurementInfo,uint8 index)
{
 rrc_GANSSSatelliteInformationList *ganss_sat_info_ptr = NULL;
          uint8 sat_index = 0;
          boolean bs_failed = FALSE;
          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info_incl = TRUE;
          
          if(uePositioningGANSSReferenceMeasurementInfo.m.ganssSignalIdPresent)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.ganss_sig_Id_incl = TRUE;
            
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.ganss_sig_Id =
             uePositioningGANSSReferenceMeasurementInfo.ganssSignalId;
          }

          ganss_sat_info_ptr = &(uePositioningGANSSReferenceMeasurementInfo.satelliteInformationList);

          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_cnt = 
            ganss_sat_info_ptr->n;

          for(sat_index=0; (sat_index<ganss_sat_info_ptr->n) && (sat_index<MAX_GANSS_SAT); sat_index++)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].satellite_id =
              ganss_sat_info_ptr->elem[sat_index].ganssSatId;
            
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].doppler =
             ganss_sat_info_ptr->elem[sat_index].dopplerZeroOrder;

            if(ganss_sat_info_ptr->elem[sat_index].m.extraDopplerPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                extra_doppler_info_incl = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                extra_doppler_info.doppler_first_order = ganss_sat_info_ptr->elem[sat_index].extraDoppler.dopplerFirstOrder;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                extra_doppler_info.Doppler_uncertainity = ganss_sat_info_ptr->elem[sat_index].extraDoppler.dopplerUncertainty;
            }

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].code_phase =
             ganss_sat_info_ptr->elem[sat_index].codePhase;

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
              integer_code_phase = ganss_sat_info_ptr->elem[sat_index].integerCodePhase;

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
              search_window = rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &ganss_sat_info_ptr->elem[sat_index].codePhaseSearchWindow);                

            if(ganss_sat_info_ptr->elem[sat_index].m.azimuthandElevationPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                azimuth_elevation_incl = TRUE;
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                azimuth_elevation_info.azimuth = ganss_sat_info_ptr->elem[sat_index].azimuthandElevation.azimuth;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].ref_meas_info.sat_info[sat_index].
                azimuth_elevation_info.elevation = ganss_sat_info_ptr->elem[sat_index].azimuthandElevation.elevation;
            }              
          } 
}

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

void rrcgps_fill_generic_data_almanac_r8(rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,rrc_UE_Positioning_GANSS_Almanac_r8 uePositioningGANSSAlmanac,uint8 index)
{
  boolean bs_failed = FALSE;
  meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].
            GANSS_Almanac_incl = TRUE;
          if(uePositioningGANSSAlmanac.m.alm_keplerianGLONASSPresent)
          {
            rrc_GANSS_SAT_Info_Almanac_GLOkpList *ganss_almanaclist_ptr = NULL;
            uint8 almanac_set_index = 0;
            
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set_incl = TRUE;

            ganss_almanaclist_ptr = &(uePositioningGANSSAlmanac.alm_keplerianGLONASS.sat_info_GLOkpList);

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.sat_cnt = ganss_almanaclist_ptr->n;
            
            for(almanac_set_index=0; 
                (almanac_set_index<MAX_GANSS_SAT) && (almanac_set_index<ganss_almanaclist_ptr->n);
                almanac_set_index++)
            {                
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmNA =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmNA);  
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmnA =
                rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmnA); 
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmHA =
                rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmHA); 
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmLambdaA =
                rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmLambdaA);
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmTlambdaA =
                rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmTlambdaA);  
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmDeltaIA =
                rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmDeltaIA);     
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAkmDeltaTA =
                rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAkmDeltaTA);    
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmDeltaTdotA =
                rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmDeltaTdotA);  
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmEpsilonA =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmEpsilonA); 
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmOmegaA =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmOmegaA); 
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmTauA =
                rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmTauA);     
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmCA =
                rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmCA);
              
              if(ganss_almanaclist_ptr->elem[almanac_set_index].m.gloAlmMAPresent)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmMA_incl = TRUE;

                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].GANSS_Almanac.almanac_set[almanac_set_index].gloAlmMA =
                 rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&ganss_almanaclist_ptr->elem[almanac_set_index].gloAlmMA);                  
              }                  
            }
          }           
}

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

void rrcgps_fill_generic_data_add_utc_models(rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,rrc_UE_Positioning_GANSS_AddUTCModels uePositioningGANSSAddUTCModels,uint8 index)
{
boolean bs_failed = FALSE;
 meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models_incl = TRUE;
          /*utcModel1*/
          if(uePositioningGANSSAddUTCModels.m.utcModel1Present)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.utc_model_type = UTC_MODEL1;

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcA0 = 
              rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &(uePositioningGANSSAddUTCModels.utcModel1.utcA0));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcA1 = 
              rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel1.utcA1));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcA2 = 
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &(uePositioningGANSSAddUTCModels.utcModel1.utcA2));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcDeltaTls = 
              rrcgps_oss_bitstring_to_int( sizeof(uint8),&bs_failed,&(uePositioningGANSSAddUTCModels.utcModel1.utcDeltaTls));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcTot = 
              rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel1.utcTot));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcWNot = 
              rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel1.utcWNot));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcWNlsf = 
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel1.utcWNlsf));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcDN = 
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel1.utcDN));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_1.utcDeltaTlsf = 
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel1.utcDeltaTlsf));
           
          }
          /*utcModel2*/
          if(uePositioningGANSSAddUTCModels.m.utcModel2Present)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.utc_model_type = UTC_MODEL2;

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_2.nA = 
              rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel2.nA));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_2.tauC = 
              rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel2.tauC));

            if(uePositioningGANSSAddUTCModels.utcModel2.m.deltaUT1Present)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_2.deltaUT1_incl = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_2.b1 =
               rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel2.deltaUT1.b1));

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_2.b2 = 
               rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel2.deltaUT1.b2));                
            }

            if(uePositioningGANSSAddUTCModels.utcModel2.m.kpPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_2.kp_incl = TRUE;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_2.kp =
               rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel2.kp));
            }
          }
          /*utcModel3*/
          if(uePositioningGANSSAddUTCModels.m.utcModel3Present)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.utc_model_type = UTC_MODEL3;

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcA1wnt =
              rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcA1wnt));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcA0wnt =
              rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcA0wnt));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcTot =
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcTot));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcWNt =
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcWNt));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcDeltaTls =
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcDeltaTls));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcWNlsf =
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcWNlsf));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcDN =
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcDN));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcDeltaTlsf =
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcDeltaTlsf));

            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].add_utc_models.u.utc_model_3.utcStandardID =
              rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAddUTCModels.utcModel3.utcStandardID));
          }
}

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

void rrcgps_fill_generic_data_aux_info(rrc_meas_ctrl_struct_type * meas_ctrl_params_ptr,rrc_UE_Positioning_GANSS_AuxiliaryInfo uePositioningGANSSAuxiliaryInfo,uint8 index)
{
boolean bs_failed = FALSE;
  uint8 sat_index=0,n_count=0;
          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info_incl = TRUE;

          if(uePositioningGANSSAuxiliaryInfo.t == 
              T_rrc_UE_Positioning_GANSS_AuxiliaryInfo_ganssID1)
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.Auxiliary_type = GANSS_ID1;

            n_count = meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.sat_cnt = 
              uePositioningGANSSAuxiliaryInfo.u.ganssID1->n;
            
            for(sat_index=0;(sat_index<MAX_GANSS_SAT) && (sat_index<n_count);sat_index++)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.u.ganssID1[sat_index].sv_id = 
                uePositioningGANSSAuxiliaryInfo.u.ganssID1->elem[sat_index].svID;
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.u.ganssID1[sat_index].signals_Available = 
               rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAuxiliaryInfo.u.
                 ganssID1->elem[sat_index].signalsAvailable));    
            }              
          }
          else
          {
            meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.Auxiliary_type = GANSS_ID3;

            n_count = meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.sat_cnt = 
              uePositioningGANSSAuxiliaryInfo.u.ganssID3->n;
            
            for(sat_index=0;(sat_index<MAX_GANSS_SAT) && (sat_index<n_count);sat_index++)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.u.ganssID3[sat_index].sv_id = 
                uePositioningGANSSAuxiliaryInfo.u.ganssID3->elem[sat_index].svID;
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.u.ganssID3[sat_index].signals_Available = 
               rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed,&(uePositioningGANSSAuxiliaryInfo.u.
                 ganssID3->elem[sat_index].signalsAvailable));

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].Auxiliary_Info.u.ganssID3[sat_index].channel_Number =
                uePositioningGANSSAuxiliaryInfo.u.ganssID3->elem[sat_index].channelNumber;
            }              
          }
}

#ifdef FEATURE_WCDMA_REL9
void rrcgps_fill_meas_ctrl_ganss_assistance_data_r9(rrc_meas_ctrl_struct_type *meas_ctrl_params_ptr,rrc_UE_Positioning_Measurement_r9 *up_Measurement_ptr)
{
  boolean bs_failed = FALSE;
  
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(up_Measurement_ptr,
    & rrc_UE_Positioning_Measurement_r9,ue_positioning_GANSS_AssistanceData))
  {
    rrc_UE_Positioning_GANSS_AssistanceData_r9* ganss_assist_data = NULL;
    
    meas_ctrl_params_ptr->ganss_assistance_data_incl = TRUE;
    
    ganss_assist_data = &up_Measurement_ptr->ue_positioning_GANSS_AssistanceData;
    
    /*UE-Positioning-GANSS-ReferenceTime*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r9,
                                       ue_positioning_GANSS_ReferenceTime))
    {
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time_incl = TRUE;
      
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.ganssDayPresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.day_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.day = 
          (uint32) ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssDay;
                                                               
      }
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_tod = 
        ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssTod;
      
      /*Not supported by MSM*/
      #if 0
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.ganssTodUncertaintyPresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_tod_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_tod = 
                              ( uint32 )rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                  &bs_failed, &ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssTod );
                                                               
      }
      #endif
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.ganssTimeIdPresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_Time_Id_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.ganss_Time_Id = 
          ganss_assist_data->ue_positioning_GANSS_ReferenceTime.ganssTimeId;
        
      }
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.utran_ganssreferenceTimePresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time.timing_Of_Cell_Frames = 
                      ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.timingOfCellFrames;
        
        if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.mode.t == 
             T_rrc_UE_Positioning_GANSS_ReferenceTime_mode_fdd)
        {
           meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time.fdd_psc = 
               ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.mode.u.fdd->
                                    primary_CPICH_Info.primaryScramblingCode;
        }
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.utran_ganss_ref_time.reference_Sfn =
          ganss_assist_data->ue_positioning_GANSS_ReferenceTime.utran_ganssreferenceTime.referenceSfn;
      }
       /*START FROm HERE*/
      /*Not supported*/
      #if 0
      if(ganss_assist_data->ue_positioning_GANSS_ReferenceTime.m.tutran_ganss_driftRatePresent)
      {
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.tutran_ganss_driftRate_incl = TRUE;
        meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_time.tutran_ganss_driftRate = 
          ganss_assist_data->ue_positioning_GANSS_ReferenceTime.tutran_ganss_driftRate;
      }
      #endif      
      
    }
    
    /*uePositioningGanssReferencePosition*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r9,
                                       uePositioningGanssReferencePosition))
    {
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos_incl = TRUE;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.latitude_sign =
       ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.latitudeSign;
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.latitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.latitude;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.longitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.longitude;
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.altitude_direction = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.altitudeDirection;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.altitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.altitude;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.uncertainity_semi_major = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.uncertaintySemiMajor;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.uncertainity_semi_minor = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.uncertaintySemiMinor;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.orientation_major_axis = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.orientationMajorAxis;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.uncertainity_altitude = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.uncertaintyAltitude;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_ref_pos.confidence = 
        ganss_assist_data->uePositioningGanssReferencePosition.ellipsoidPointAltitudeEllipsoide.confidence;        
    }
    /*UE-Positioning-GANSS-AddIonoModel*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r9,
                                       uePositioningGanssAddIonoModel))
    {
      /*UE-Positioning-GPS-IonosphericModel*/
      rrc_UE_Positioning_GPS_IonosphericModel *alpha_beta_ptr = NULL; 
             
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model_incl = TRUE;

      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.data_id = 
        ( uint8 )rrcgps_oss_bitstring_to_int( sizeof( uint8 ),&bs_failed,
                  &ganss_assist_data->uePositioningGanssAddIonoModel.dataID);

      alpha_beta_ptr = &ganss_assist_data->uePositioningGanssAddIonoModel.alpha_beta_parameters;
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa0 =
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed,&alpha_beta_ptr->alpha0 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa1 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->alpha1 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa2 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->alpha2 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.alfa3 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->alpha3 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta0 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta0 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta1 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta1 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta2 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta2 );
      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_addlono_model.alpha_beta_param.beta3 = 
        (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &alpha_beta_ptr->beta3 );
      

    }
    /*GANSSGenericDataList-r8*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r9,
                                       ganssGenericDataList))
    {
      rrc_GANSSGenericDataList_r9 *ganss_gen_list_ptr = NULL;
      uint8 index =0;

      
      meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list_incl = TRUE;      

      ganss_gen_list_ptr = &(ganss_assist_data->ganssGenericDataList);

      meas_ctrl_params_ptr->ganss_assistance_data.list_cnt = ganss_gen_list_ptr->n;
      
      for(index =0; (index < ganss_gen_list_ptr->n) && (index < MAX_GANSS); index++)
      {
        /*ganssId*/
        if(ganss_gen_list_ptr->elem[index].m.ganssIdPresent)
        {
            rrcgps_fill_generic_data_ganss_id(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].ganssId,index);
        }
        /*GANSSTimeModelsList*/
        if(ganss_gen_list_ptr->elem[index].m.ganssTimeModelsListPresent)
        {
          rrcgps_fill_generic_data_time_model_list(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].ganssTimeModelsList,index);
        }
        /*UE-Positioning-DGANSSCorrections*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningDGANSSCorrectionsPresent)
        {
          rrc_DGANSSInfoList_r9 *dganss_list_ptr = NULL;
          uint8 info_list_index = 0;
          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections_incl = TRUE;
          
          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_ref_time =
            ganss_gen_list_ptr->elem[index].uePositioningDGANSSCorrections.dganssreferencetime;

          dganss_list_ptr = &(ganss_gen_list_ptr->elem[index].uePositioningDGANSSCorrections.dganssInfoList);

          meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.list_cnt =
            dganss_list_ptr->n;

          for(info_list_index=0;(info_list_index<MAX_SGN_TYPE) && (info_list_index<dganss_list_ptr->n); info_list_index++)
          {             
          
            if(dganss_list_ptr->elem[info_list_index].m.ganssSignalIdPresent)
            {
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                sig_id_incl = TRUE;
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
               sig_id = dganss_list_ptr->elem[info_list_index].ganssSignalId;
            }
            switch(dganss_list_ptr->elem[info_list_index].ganssStatusHealth)
            {
              case rrc_GANSS_Status_Health_udre_scale_1dot0:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_1_0;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot75:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_75;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot5:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_5;                  
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot3:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_3;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot2:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_2;
                break;
              case rrc_GANSS_Status_Health_udre_scale_0dot1:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_UDRE_0_1;
                break;
              case rrc_GANSS_Status_Health_no_data:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_NO_DATA;
                break;
              case rrc_GANSS_Status_Health_invalid_data:
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  status_health = RRC_INVALID_DATA;
                break;
              default:
                break;
            }
            if(dganss_list_ptr->elem[info_list_index].m.dgansssignalInformationListPresent)
            {
              uint8 sig_infolist_idx =0;
              rrc_DGANSSSignalInformationList_r9 *dganss_sig_infolist_ptr = NULL;
              
              dganss_sig_infolist_ptr = &dganss_list_ptr->elem[info_list_index].dgansssignalInformationList;

              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                dganss_sig_info_list_incl = TRUE;
              
              meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                sat_cnt = dganss_sig_infolist_ptr->n;

              for(sig_infolist_idx = 0; (sig_infolist_idx < dganss_sig_infolist_ptr->n) && (sig_infolist_idx < MAX_GANSS_SAT);sig_infolist_idx++)
              {
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].sat_id = dganss_sig_infolist_ptr->elem[sig_infolist_idx].satId;

                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].iode = (uint16)rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &dganss_sig_infolist_ptr->elem[sig_infolist_idx].iode_dganss );

                switch(dganss_sig_infolist_ptr->elem[sig_infolist_idx].udre)
                {
                  case rrc_UDRE_lessThan1:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = LESS_THEN_1;
                    break;
                  case rrc_UDRE_between1_and_4:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = BET_1_AND_4;
                    break;
                  case rrc_UDRE_between4_and_8:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = BET_4_AND_8;
                    break;
                  case rrc_UDRE_over8:
                   meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                    dganss_sig_info_list[sig_infolist_idx].udre = OVER_8;
                    break;
                  default:
                    break;
                }
                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].prc = dganss_sig_infolist_ptr->elem[sig_infolist_idx].ganss_prc;
                
                meas_ctrl_params_ptr->ganss_assistance_data.ganss_data_list[index].DGANSS_corrections.dganss_info[info_list_index].
                  dganss_sig_info_list[sig_infolist_idx].rrc = dganss_sig_infolist_ptr->elem[sig_infolist_idx].ganss_rrc;
              }
            }
          }              
        }
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSRealTimeIntegrityPresent)
        {
          rrcgps_fill_generic_data_real_time_integrity(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSRealTimeIntegrity,index);
        }
        /*UE-Positioning-GANSS-ReferenceMeasurementInfo*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSReferenceMeasurementInfoPresent)
        {
          rrcgps_fill_generic_data_ref_meas_info(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSReferenceMeasurementInfo,index);
        }
        /*UE-Positioning-GANSS-Almanac-r8*/                  
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSAlmanacPresent)
        {
          rrcgps_fill_generic_data_almanac_r8(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSAlmanac,index);
        }
        	
        /*UE-Positioning-GANSS-AddUTCModels*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSAddUTCModelsPresent)
        {
          rrcgps_fill_generic_data_add_utc_models(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSAddUTCModels,index);
        }
        /*UE-Positioning-GANSS-AuxiliaryInfo*/
        if(ganss_gen_list_ptr->elem[index].m.uePositioningGANSSAuxiliaryInfoPresent)
        {
          rrcgps_fill_generic_data_aux_info(meas_ctrl_params_ptr,ganss_gen_list_ptr->elem[index].uePositioningGANSSAuxiliaryInfo,index);
        }
      }
    }    
  }
}
#endif
#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

#endif


#ifdef FEATURE_WCDMA_REL9

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
  rrc_assistance_data_struct_type* rrc_ptr )
{
  uint32 q_Cnt;
  uint8  sat_info_cnt = 0;
  boolean bs_failed = FALSE;

  /* Zero out RRC structure */
  memset( (void *) rrc_ptr, 0, sizeof( rrc_assistance_data_struct_type ) );

  /* Acq Assistance */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_ptr,
    rrc_UE_Positioning_GPS_AssistanceData_r9,ue_positioning_GPS_AcquisitionAssistance))
  {
    rrc_UE_Positioning_GPS_AcquisitionAssistance_r7 *asn1_acq_assist = 
      &asn1_ptr->ue_positioning_GPS_AcquisitionAssistance;
    rrc_gps_acq_assistance_struct_type *rrc_acq_assist = 
      &rrc_ptr->gps_acq_assistance;

    rrc_AcquisitionSatInfoList *asn1_info_ptr;
    rrc_gps_sat_info_struct_type *rrc_info_ptr;

    rrc_ptr->gps_acq_assistance_incl = TRUE;
    rrc_acq_assist->gps_reference_time = asn1_acq_assist->gps_ReferenceTime;

    /* GPS - UTRAN Reference Time */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_acq_assist,
        rrc_UE_Positioning_GPS_AcquisitionAssistance_r7,utran_GPSReferenceTime))
    {
      rrc_acq_assist->gps_umts_reference_time_incl = TRUE;
      rrc_acq_assist->gps_umts_reference_time.msb_timing =
        asn1_acq_assist->utran_GPSReferenceTime.utran_GPSTimingOfCell.ms_part;
      rrc_acq_assist->gps_umts_reference_time.lsb_timing =
        asn1_acq_assist->utran_GPSReferenceTime.utran_GPSTimingOfCell.ls_part;
      rrc_acq_assist->gps_umts_reference_time.sfn =
        asn1_acq_assist->utran_GPSReferenceTime.sfn;

      /* Primary Scrambling Code */
      if (RRC_MSG_COMMON_BITMASK_IE( asn1_acq_assist->utran_GPSReferenceTime,
        rrc_UTRAN_GPSReferenceTime,modeSpecificInfo))
      {
         rrc_acq_assist->gps_umts_reference_time.gps_ref_psc_incl = TRUE;
         
        if(asn1_acq_assist->utran_GPSReferenceTime.modeSpecificInfo.t == T_rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd)
        {
           /* Only FDD mode supported in UE, grab primary scrambling code */
           rrc_acq_assist->gps_umts_reference_time.psc =
           asn1_acq_assist->utran_GPSReferenceTime.modeSpecificInfo.u.fdd->referenceIdentity.primaryScramblingCode;
        }
        else
        {
          WRRC_MSG0_ERROR("TDD info given in MCM. Send failure.");
          return FALSE;
        }
      }
    }

    /* Traverse Satellite Info List */
    sat_info_cnt = 0;
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_AcquisitionAssistance.satelliteInformationList;
    rrc_info_ptr = &rrc_ptr->gps_acq_assistance.gps_sat_info[ 0 ];
    
    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if( asn1_info_ptr == NULL )
        break;
    
      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->doppler = asn1_info_ptr->elem[q_Cnt].doppler0thOrder;
    
      /* Extra Doppler Info */
      if ( asn1_info_ptr->elem[q_Cnt].m.extraDopplerInfoPresent )
      {
        rrc_info_ptr->extra_doppler_info_incl = TRUE;
        rrc_info_ptr->extra_doppler_info.doppler_first_order =
          asn1_info_ptr->elem[q_Cnt].extraDopplerInfo.doppler1stOrder;
        rrc_info_ptr->extra_doppler_info.Doppler_uncertainity =
          asn1_info_ptr->elem[q_Cnt].extraDopplerInfo.dopplerUncertainty;
      }
    
      rrc_info_ptr->code_phase = asn1_info_ptr->elem[q_Cnt].codePhase;
      rrc_info_ptr->integer_code_phase = asn1_info_ptr->elem[q_Cnt].integerCodePhase;
      rrc_info_ptr->gps_bit_number = asn1_info_ptr->elem[q_Cnt].gps_BitNumber;
      rrc_info_ptr->search_window = asn1_info_ptr->elem[q_Cnt].codePhaseSearchWindow;
    
      /* Azimuth and Elevation */
      if ( asn1_info_ptr->elem[q_Cnt].m.azimuthAndElevationPresent )
      {
        rrc_info_ptr->azimuth_elevation_incl = TRUE;
        rrc_info_ptr->azimuth_elevation_info.azimuth =
          asn1_info_ptr->elem[q_Cnt].azimuthAndElevation.azimuth;
        rrc_info_ptr->azimuth_elevation_info.elevation =
          asn1_info_ptr->elem[q_Cnt].azimuthAndElevation.elevation;
      }
    
      /* Update sat info ptr with next element in the list */
      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_acq_assist->no_of_gps_sat = sat_info_cnt;
    if (RRC_MSG_COMMON_BITMASK_IE_PTR( asn1_acq_assist,
        rrc_UE_Positioning_GPS_AcquisitionAssistance_r7,ue_Positioning_GPS_ReferenceTimeUncertainty))
    {
      /*TBD*/
      MSG_LOW("Rel7 IE's TBD",0,0,0);
    }
  }
  
  /* Almanac */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r9,
    ue_positioning_GPS_Almanac))
  {
    rrc_UE_Positioning_GPS_Almanac *asn1_almanac =
	  &asn1_ptr->ue_positioning_GPS_Almanac;
    rrc_gps_almanac_struct_type *rrc_almanac =
	  &rrc_ptr->gps_almanc;

    rrc_AlmanacSatInfoList *asn1_info_ptr;
	rrc_almanac_sat_info_struct_type *rrc_info_ptr; 

    rrc_ptr->gps_almanac_incl = TRUE;
	rrc_almanac->wn_a = 
	  ( uint8 )rrcgps_oss_bitstring_to_int( sizeof( uint8 ), &bs_failed, &asn1_almanac->wn_a );

    /* Global Health */
    if ( asn1_almanac->m.sv_GlobalHealthPresent )
    {
      uint16 num_bits = asn1_almanac->sv_GlobalHealth.numbits;
      uint8 num_bytes; 
      uint8 *dest_ptr = (uint8 *)&rrc_almanac->global_health[0];
      uint8 *src_ptr = (uint8 *)asn1_almanac->sv_GlobalHealth.data;
    
      /* This is a special case where we can not use rrcgps_oss_bitstring_to_int(). The Global
         Health could be hundreds of bytes long, therefore we need to handle this data 
    	 in this function */
    
      num_bytes = num_bits / 8;

      /* Determine if we need an extra byte for any straggler bits */
      if ( num_bits % 8 )
        num_bytes++;
    
      /* Copy global health data */
      if ( num_bytes <= GLOBAL_HEALTH_ARRAY_SIZE )
      {
        rrc_almanac->global_health_incl = TRUE;
        rrc_almanac->global_health_num_bits = num_bits;
      
        /* Copy global health bits */
        for ( q_Cnt = num_bytes; q_Cnt; q_Cnt-- )
        {
          *dest_ptr++ = *src_ptr++;
        }
      }
    }

    /* Traverse Almanac List */
	 
    sat_info_cnt = 0;
    asn1_info_ptr = &asn1_almanac->almanacSatInfoList;
    rrc_info_ptr = &rrc_almanac->almanac_sat_info_list[0];

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if ( asn1_info_ptr == NULL )
        break;
    
      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->data_id = (uint8) asn1_info_ptr->elem[q_Cnt].dataID;

      rrc_info_ptr->a_Sqrt = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].a_Sqrt );
      rrc_info_ptr->deltaI = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].deltaI );
      rrc_info_ptr->e = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].e );
      rrc_info_ptr->m0 = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].m0 );
      rrc_info_ptr->omega0 = 
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omega0 );
      rrc_info_ptr->omega_dot = 
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omegaDot );
      rrc_info_ptr->omega =
        (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].omega );
      rrc_info_ptr->sat_health = 
        (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].satHealth );
      rrc_info_ptr->t_oa =
        (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].t_oa );
      rrc_info_ptr->af0 =
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].af0 );
      rrc_info_ptr->af1 =
        (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].af1 );


      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_almanc.no_of_almanac_sat = sat_info_cnt;
  }

  /* Nav Model */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r9,
    ue_positioning_GPS_NavigationModel))
  {
    rrc_navigation_model_sat_info_list_struct_type *rrc_info_ptr;
    rrc_NavigationModelSatInfoList *asn1_info_ptr;

    rrc_ptr->gps_navigation_model_incl = TRUE;

    /* Traverse NavModel List */
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_NavigationModel.navigationModelSatInfoList;
    rrc_info_ptr = &rrc_ptr->gps_navigation_model.satellite_info[0];
    sat_info_cnt = 0;

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
        /* Check to see if we have reached the end of the linked list */
        if ( asn1_info_ptr == NULL )
          break;

      rrc_info_ptr->satellite_id = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
      rrc_info_ptr->satellite_status = asn1_info_ptr->elem[q_Cnt].satelliteStatus;
       
      /* Ephemeris Parameters */
      if ( asn1_info_ptr->elem[q_Cnt].m.ephemerisParameterPresent )
      {
        rrc_info_ptr->ephermeral_clock_params_incl = TRUE;
        rrc_info_ptr->ephermeral_clock_params.a_Sqrt =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.a_Sqrt );
        rrc_info_ptr->ephermeral_clock_params.af0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af0 );
        rrc_info_ptr->ephermeral_clock_params.af1 = 
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af1 );
        rrc_info_ptr->ephermeral_clock_params.af2 =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.af2 );
        rrc_info_ptr->ephermeral_clock_params.aodo =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.aodo );
        rrc_info_ptr->ephermeral_clock_params.c_ic =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_ic );
        rrc_info_ptr->ephermeral_clock_params.c_is =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_is );
        rrc_info_ptr->ephermeral_clock_params.c_rc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_rc );
        rrc_info_ptr->ephermeral_clock_params.c_rs =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_rs );
        rrc_info_ptr->ephermeral_clock_params.c_uc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_uc );
        rrc_info_ptr->ephermeral_clock_params.c_us =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.c_us );
        rrc_info_ptr->ephermeral_clock_params.code_on_l2 =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.codeOnL2 );
        rrc_info_ptr->ephermeral_clock_params.delta_n =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.delta_n );
        rrc_info_ptr->ephermeral_clock_params.e =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.e );
        rrc_info_ptr->ephermeral_clock_params.fit_interval =
            (boolean) rrcgps_oss_bitstring_to_int( sizeof(boolean), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.fitInterval );
        rrc_info_ptr->ephermeral_clock_params.i0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.i0 );
        rrc_info_ptr->ephermeral_clock_params.iDot =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.iDot );
            rrc_info_ptr->ephermeral_clock_params.iodc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.iodc );
        rrc_info_ptr->ephermeral_clock_params.l2_pflag =
            (boolean) rrcgps_oss_bitstring_to_int( sizeof(boolean), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.l2Pflag );
        rrc_info_ptr->ephermeral_clock_params.m0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.m0 );
        rrc_info_ptr->ephermeral_clock_params.omega =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omega );
        rrc_info_ptr->ephermeral_clock_params.omega0 =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omega0 );
        rrc_info_ptr->ephermeral_clock_params.omegaDot =
            (uint32) rrcgps_oss_bitstring_to_int( sizeof(uint32), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.omegaDot );
        rrc_info_ptr->ephermeral_clock_params.sat_health =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.satHealth );
        rrc_info_ptr->ephermeral_clock_params.t_gd =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_GD );
        rrc_info_ptr->ephermeral_clock_params.t_oc =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_oc );
        rrc_info_ptr->ephermeral_clock_params.t_oe =
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.t_oe );
        rrc_info_ptr->ephermeral_clock_params.ura_index =
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].ephemerisParameter.uraIndex );
      }

      rrc_info_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_navigation_model.no_of_satellites = sat_info_cnt;

  }

  /* Real Time Integrity */
  if( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r9,
    ue_positioning_GPS_Real_timeIntegrity))
  {
    rrc_BadSatList *asn1_info_ptr;
    uint8 *bad_sat_ptr;

    rrc_ptr->gps_real_time_integrity_incl = TRUE;

    /* Fill in RTI values */
    asn1_info_ptr = &asn1_ptr->ue_positioning_GPS_Real_timeIntegrity;
    bad_sat_ptr = (uint8 *)&rrc_ptr->gps_rti.satellite_id[0];

    sat_info_cnt = 0;

    for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
    {
      /* Check to see if we have reached the end of the linked list */
      if ( asn1_info_ptr == NULL )
          break;

      *bad_sat_ptr = (uint8) asn1_info_ptr->elem[q_Cnt];

      
      bad_sat_ptr++;
      sat_info_cnt++;
    }

    rrc_ptr->gps_rti.no_of_satellites = sat_info_cnt;
  }
    
  /* Reference Location */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r9,
    ue_positioning_GPS_ReferenceLocation))
  {
    rrc_gps_reference_loc_struct_type *rrc_ref_loc =
	  &rrc_ptr->gps_ref_loc;
    rrc_EllipsoidPointAltitudeEllipsoide *asn1_ref_loc =
	  &asn1_ptr->ue_positioning_GPS_ReferenceLocation.ellipsoidPointAltitudeEllipsoide;

    rrc_ptr->gps_ref_loc_incl = TRUE;
    rrc_ref_loc->altitude = asn1_ref_loc->altitude;
    rrc_ref_loc->altitude_direction = asn1_ref_loc->altitudeDirection;
    rrc_ref_loc->confidence = asn1_ref_loc->confidence;
    rrc_ref_loc->latitude = asn1_ref_loc->latitude;
    rrc_ref_loc->latitude_sign = asn1_ref_loc->latitudeSign;
    rrc_ref_loc->longitude = asn1_ref_loc->longitude;
    rrc_ref_loc->orientation_major_axis = (uint8) asn1_ref_loc->orientationMajorAxis;
    rrc_ref_loc->uncertainity_altitude = asn1_ref_loc->uncertaintyAltitude;
    rrc_ref_loc->uncertainity_semi_major = (uint8) asn1_ref_loc->uncertaintySemiMajor;
    rrc_ref_loc->uncertainity_semi_minor = (uint8) asn1_ref_loc->uncertaintySemiMinor;
  }

  /* Reference Time */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r9,
    ue_positioning_GPS_ReferenceTime))
  {
    rrc_gps_reference_time_struct_type *rrc_ref_time =
	  &rrc_ptr->gps_ref_time;
    rrc_UE_Positioning_GPS_ReferenceTime_r7 *asn1_ref_time =
	  &asn1_ptr->ue_positioning_GPS_ReferenceTime;

    rrc_ptr->gps_ref_time_incl = TRUE;
    rrc_ref_time->time = asn1_ref_time->gps_tow_1msec;
    rrc_ref_time->week = asn1_ref_time->gps_Week;
	
    /* GPS Drift Rate */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        utran_GPS_DriftRate))
    {
      rrc_ref_time->gps_drift_rate_incl = TRUE;
      rrc_ref_time->gps_drift_rate =
      asn1_ref_time->utran_GPS_DriftRate;
    }

    /* GPS - UTRAN reference time */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        utran_GPSReferenceTime))
    {
      rrc_ref_time->gps_utran_reference_time_incl = TRUE;
      rrc_ref_time->gps_utran_ref_time.lsb_timing =
        asn1_ref_time->utran_GPSReferenceTime.utran_GPSTimingOfCell.ls_part;
      rrc_ref_time->gps_utran_ref_time.msb_timing =
        asn1_ref_time->utran_GPSReferenceTime.utran_GPSTimingOfCell.ms_part;
      rrc_ref_time->gps_utran_ref_time.sfn =
        asn1_ref_time->utran_GPSReferenceTime.sfn;

      /* Primary Scrambling Code */ 
      if (RRC_MSG_COMMON_BITMASK_IE( asn1_ref_time->utran_GPSReferenceTime,
        rrc_UTRAN_GPSReferenceTime,modeSpecificInfo))
      {
        rrc_ref_time->gps_utran_ref_time.gps_ref_psc_incl = TRUE;

        if(asn1_ref_time->utran_GPSReferenceTime.modeSpecificInfo.t == T_rrc_UTRAN_GPSReferenceTime_modeSpecificInfo_fdd)
        {
          /* UE Only supports FDD, grab primary scrambling code */
          rrc_ref_time->gps_utran_ref_time.psc =
          asn1_ref_time->utran_GPSReferenceTime.modeSpecificInfo.u.fdd->referenceIdentity.primaryScramblingCode;
        }
        else
        {
          WRRC_MSG0_ERROR("TDD info given in MCM. Send failure.");
          return FALSE;
        }
      }
    }

    /* SFN TOW Uncertainty */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        sfn_tow_Uncertainty))
    {
      rrc_ref_time->sfn_tow_uncertainity_incl = TRUE;
      rrc_ref_time->sfn_tow_uncertainity = asn1_ref_time->sfn_tow_Uncertainty;
    }

    /* GPS TOW Assistance List */
    if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        gps_TOW_AssistList))
    {
      rrc_GPS_TOW_AssistList *asn1_info_ptr;
      rrc_gps_tow_assist_struct_type *rrc_info_ptr;

      asn1_info_ptr = &asn1_ref_time->gps_TOW_AssistList;
      rrc_info_ptr = &rrc_ref_time->gps_tow_assist[0];

      /* Traverse TOW Assist List */
      sat_info_cnt = 0;
      for ( q_Cnt = 0; ((q_Cnt < MAX_NO_OF_SATELLITES)&& (asn1_info_ptr->n > q_Cnt)); q_Cnt++ )
      {
        /* Check to see if we have reached the end of the linked list */
        if ( asn1_info_ptr == NULL )
          break;

        rrc_info_ptr->satellite_identity = (uint8) asn1_info_ptr->elem[q_Cnt].satID;
        rrc_info_ptr->tlm_msg = 
            (uint16) rrcgps_oss_bitstring_to_int( sizeof(uint16), &bs_failed, &asn1_info_ptr->elem[q_Cnt].tlm_Message );
        rrc_info_ptr->tlm_rsvd = 
            (uint8) rrcgps_oss_bitstring_to_int( sizeof(uint8), &bs_failed, &asn1_info_ptr->elem[q_Cnt].tlm_Reserved );
        rrc_info_ptr->alert = asn1_info_ptr->elem[q_Cnt].alert;
        rrc_info_ptr->anti_spoof = asn1_info_ptr->elem[q_Cnt].antiSpoof;

        rrc_info_ptr++;
        sat_info_cnt++;                                                       
      }

      rrc_ref_time->no_of_gps_tow_assist = sat_info_cnt;
    }
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ref_time, rrc_UE_Positioning_GPS_ReferenceTime_r7,
        ue_Positioning_GPS_ReferenceTimeUncertainty))
    {
      /*TBD*/
      MSG_LOW("Rel 7 GPS IE's TBD",0,0,0);
    }
  }

  /* Reference Cell Info */

  /* DGPS Corrections - Not currently supported by RRC layer */

  /* UTC Model - Not currently supported by RRC layer */

  /* IONO Model */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r9,
    ue_positioning_GPS_IonosphericModel))
  {
    rrc_gps_iono_struct_type *rrc_iono = &rrc_ptr->gps_iono;

    rrc_UE_Positioning_GPS_IonosphericModel *asn1_iono = 
    &asn1_ptr->ue_positioning_GPS_IonosphericModel;

    rrc_ptr->gps_iono_incl = TRUE;

    rrc_iono->alfa0 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha0 );
    rrc_iono->alfa1 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha1 );
    rrc_iono->alfa2 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha2 );
    rrc_iono->alfa3 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->alpha3 );
    rrc_iono->beta0 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta0 );
    rrc_iono->beta1 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta1 );
    rrc_iono->beta2 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta2 );
    rrc_iono->beta3 = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_iono->beta3 );
  }

  /** UTC model ***/
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(asn1_ptr, rrc_UE_Positioning_GPS_AssistanceData_r9,
    ue_positioning_GPS_UTC_Model))
  {
    rrc_gps_utc_struct_type *rrc_utc = &rrc_ptr->gps_utc_model;

    rrc_UE_Positioning_GPS_UTC_Model *asn1_utc = &asn1_ptr->ue_positioning_GPS_UTC_Model;

    rrc_ptr->gps_utc_model_incl = TRUE;

    rrc_utc->q_A0   = (uint32)rrcgps_oss_bitstring_to_int(sizeof(uint32), &bs_failed, &asn1_utc->a0);
    rrc_utc->q_A1 =   (uint32)rrcgps_oss_bitstring_to_int(sizeof(uint32), &bs_failed, &asn1_utc->a1);
    rrc_utc->u_DeltaTls = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->delta_t_LS );
    rrc_utc->u_DeltaTlsf = (uint8)rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->delta_t_LSF);
    rrc_utc->u_DN = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->dn );
    rrc_utc->u_Tot = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->t_ot );
    rrc_utc->u_WNlsf = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->wn_lsf );
    rrc_utc->u_WNt = (uint8) rrcgps_oss_bitstring_to_int(sizeof(uint8), &bs_failed, &asn1_utc->wn_t );

  } 

  /* If any of the bitstring conversions failed, indicate this to caller by returning
     false. */
  if ( bs_failed )
  {
    #ifndef WIN32
    WRRC_MSG0_ERROR("ASN.1 to RRC structure conversion encountered errors");
    #endif
    return( FALSE );
  }
  else
    return( TRUE );
}


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
)
{
  meas_validity_enum_type pos_meas_validity = NOT_PRESENT;
  
  rrc_UE_Positioning_ReportingQuantity_r8* rpt_quan_ptr = NULL;

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  rpt_crit_enum_type rpt_mode;
  boolean bs_failed = FALSE;

  rpt_quan_ptr = &up_Measurement_ptr->ue_positioning_ReportingQuantity;

  meas_ctrl_params_ptr->rpt_quan_incl = TRUE;
  
  
  /* Get Pos Method */
  meas_ctrl_params_ptr->rpt_quan.method_type =
    (uint8) RET_POS_METHOD_TYPE(rpt_quan_ptr->methodType);
  
  if (rrcgps_verify_positioning_capability_r8(rpt_quan_ptr)
    == FAILURE)
  {
    /* Send Meas Ctrl Failure */
    
    
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
    /* Set error t */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
    
  }
  #ifdef FEATURE_GANSS_SUPPORT
  else if((rrc_ganss_supported == TRUE) && (up_Measurement_ptr->m.ue_positioning_GANSS_AssistanceDataPresent))
  {
    rrc_UE_Positioning_GANSS_AssistanceData_r9* ganss_assist_data = &up_Measurement_ptr->ue_positioning_GANSS_AssistanceData;
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ganss_assist_data, rrc_UE_Positioning_GANSS_AssistanceData_r9,
                                       ganssGenericDataList))
    {
      rrc_GANSSGenericDataList_r9 *ganss_gen_list_ptr = NULL;
      uint8 index =0;
      boolean valid_ganss_id_found = FALSE;

      ganss_gen_list_ptr = &(ganss_assist_data->ganssGenericDataList);
  
  
  
      for(index =0; (index < ganss_gen_list_ptr->n) && (index < MAX_GANSS); index++)
      {
        /*ganssId---Only Glonass is supported*/
        if(ganss_gen_list_ptr->elem[index].m.ganssIdPresent && ganss_gen_list_ptr->elem[index].ganssId ==RRC_GLONASS)
        {
           valid_ganss_id_found = TRUE;
        }
      }
      if(!valid_ganss_id_found)
      {
           //Not sending MCF here
           WRRC_MSG0_HIGH("POS: GANSS ID Not Supported");
          return FAILURE;
      }
     }
  }
  #endif
  /* Get Reporting Quantity */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,
    rrc_UE_Positioning_ReportingQuantity_r8,horizontalAccuracy))
  {
    meas_ctrl_params_ptr->rpt_quan.hor_accuracy_incl = TRUE;
    
    /* Set the 7 bits */
    

meas_ctrl_params_ptr->rpt_quan.horizontal_accuracy = (uint8)rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                               &bs_failed, &rpt_quan_ptr->horizontalAccuracy );
  }

  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,verticalAccuracy))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSPositioningMethods))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSTimingOfCellWanted))||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSCarrierPhaseMeasurementRequested)))
  {
    /*TBD*/
    MSG_LOW("Rel 9 IE's Support TBD",0,0,0);
  }
  #ifdef FEATURE_GANSS_SUPPORT
if(rrc_ganss_supported == TRUE)
  {
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,verticalAccuracy))
  {
    meas_ctrl_params_ptr->rpt_quan.r8_mcm = TRUE;
    meas_ctrl_params_ptr->rpt_quan.vert_accuracy_incl = TRUE;
    meas_ctrl_params_ptr->rpt_quan.vertical_accuracy = (uint32) rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                               &bs_failed, &rpt_quan_ptr->verticalAccuracy );
  }
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,velocityRequested))
  {
    meas_ctrl_params_ptr->rpt_quan.velocity_Req_inc = TRUE;
    meas_ctrl_params_ptr->rpt_quan.velocity_Requested = rpt_quan_ptr->velocityRequested;
  }
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSPositioningMethods))
    {
    meas_ctrl_params_ptr->rpt_quan.ganss_pos_method_incl = TRUE;
    meas_ctrl_params_ptr->rpt_quan.ganss_pos_method = ( uint16 )rrcgps_oss_bitstring_to_int( sizeof( uint16 ), 
                                               &bs_failed, &rpt_quan_ptr->gANSSPositioningMethods ); 
  }  
  
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rpt_quan_ptr,rrc_UE_Positioning_ReportingQuantity_r8,gANSSTimingOfCellWanted))
        {
    meas_ctrl_params_ptr->rpt_quan.ganss_timing_cell_wanted_incl = TRUE;
    meas_ctrl_params_ptr->rpt_quan.ganss_timing_cell_wanted = ( uint32 )rrcgps_oss_bitstring_to_int( sizeof( uint32 ), 
                                               &bs_failed, &rpt_quan_ptr->gANSSTimingOfCellWanted ); 
        }
      }
#endif  
  /* AdditionalAssistanceDataRequest */
  if (rpt_quan_ptr->additionalAssistanceDataReq == TRUE)
      {
    meas_ctrl_params_ptr->rpt_quan.additional_assistance_data_request = TRUE;
      }
  
  if (rpt_quan_ptr->gps_TimingOfCellWanted == TRUE)
  {
    meas_ctrl_params_ptr->rpt_quan.sfn_gps_timing_required = TRUE;
     }
  
  
  /* Get Reporting Criteria */
  if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_r7_ue_positioning_ReportingCriteria))
  {
    WRRC_MSG0_ERROR("POS: Event trigg crit unsupported");
    /* Set transaction ID */
    rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
    /* Set error t */
    rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement;
    
    /* Send MCF msg to UTRAN */
    rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
    
    return FAILURE;
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(up_Measurement_ptr->reportCriteria,
    rrc_UE_Positioning_ReportCriteria_r7_noReporting))
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_NO_RPT;
  }
  else
  {
    meas_ctrl_params_ptr->rpt_crit.rpt_mode = (uint8)RRC_PERIODIC_RPT;
    
    /* Get reporting criteria and interval */
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)rrc_ReportingAmount_ra_Infinity;
  
   
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_amount = (uint8)
      up_Measurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingAmount;
    
  
    meas_ctrl_params_ptr->rpt_crit.u.periodic_crit.rpt_interval = (uint8)
      up_Measurement_ptr->reportCriteria.u.periodicalReportingCriteria->reportingInterval;
  }
  if(up_Measurement_ptr->m.ue_positioning_GPS_AssistanceDataPresent )
  {
  /* Now call LSM fn to store Meas Object if FEATURE_MMGPS; call a local function to do the same if FEATURE_CGPS_xxx */
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
    if (rrcgps_convert_assistance_data_r9(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
      &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)
  #else /* this will only be FEATURE_MMGPS */
  /*To be Required from GPS guys*/
/*  if (lsm_convert_assistance_data_r7(&up_Measurement_ptr->ue_positioning_GPS_AssistanceData,
    &meas_ctrl_params_ptr->gps_assistance_data) == FALSE)*/
  #endif
    {
      WRRC_MSG0_ERROR("POS: Assistance Data conversion failed");
    
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
    
      /* Set faliure t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_protocolError;
    
      rrcmeas_mcf.error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
    
      return FAILURE;
    }
    meas_ctrl_params_ptr->gps_assistance_data_incl = TRUE;
  }
  
  /* Check if Meas Validity present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(up_Measurement_ptr,
    & rrc_UE_Positioning_Measurement_r9,measurementValidity))
  {
    WRRC_MSG1_HIGH("POS:Meas validity %d (0/d:1/f:2/a)", up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Store Measurement Validity */
    pos_meas_validity = TVM_RET_MEAS_VALIDITY(up_Measurement_ptr->measurementValidity.ue_State);
    
    /* Check if Meas needs to be restarted */
    if (((pos_meas_validity == STATE_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_FACH)) ||
       ((pos_meas_validity == ALL_STATES_BUT_DCH) && (rrcmeas_current_substate == RRCMEAS_CELL_DCH)))
    {
      WRRC_MSG0_HIGH("POS: RRC val, MCM val differ");
      meas_ctrl_params_ptr->meas_start_incl = FALSE;
    }
  }    
#ifdef FEATURE_GANSS_SUPPORT  
  if(rrc_ganss_supported)
  {
    rrcgps_fill_meas_ctrl_ganss_assistance_data_r9(meas_ctrl_params_ptr, up_Measurement_ptr);
  }
#endif  
  /* Store Pos Parameters in RRC Database */

  switch(meas_ctrl_params_ptr->rpt_crit.rpt_mode)
  {
    case 0: rpt_mode = PERIODIC;
        break;
    case 1: rpt_mode = EVENT;
        break;
    case 2: rpt_mode = ADDTL;
        break;
    case 3: rpt_mode = NOT_AVAILABLE;
        break;
    default:break;
  } 


//CBSS rpt_mode is not assigned so junk value is getting fillled & even thoug return value is being passed TRUE, it is being as FALSE
  if(rrcmeas_config_pos_db(pos_info_ptr, &pos_meas_validity, &rpt_mode, modify_meas_params_incl) == FALSE)
  {
    return FAILURE;
  }
  return SUCCESS;
}

/*===========================================================================

FUNCTION   RRCGPS_PROCESS_MCM_R9

DESCRIPTION

    This function processes the rel 9 Measurement Control Message for
    A-GPS Measurements.

DEPENDENCIES

    None.

RETURN VALUE

    FAILURE: If Meas Ctrl is invalid
    SUCCESS: If Meas Ctrl is valid.

SIDE EFFECTS

    None

===========================================================================*/

uecomdef_status_e_type rrcgps_process_mcm_r9
(
  void* pos_mcm_ptr,
  pos_info_struct_type* p_info_ptr
  ,uint8 meas_ctrl_tr_id
)
{
  /* Process the A-GPS Measurement Control Message */
  uint8 count = 0, cnt = 0;
  
  uint16 deleted_meas_id = 0;
  
  /* Declare MCM config structure that goes to L1 */
  rrc_meas_params_struct_type  *gps_meas_params;

  
  /* Declare ptr to store message received from UTRAN */
  rrc_DL_DCCH_Message* dcch_msg_ptr = NULL;

  rrc_MeasurementControl_r9_IEs* rrc_mcm_ptr = NULL; 

  /* Stores meas ctrl failure parameters */
  rrcmeas_mcf_struct_type rrcmeas_mcf;
  
  uint8 transaction_id = INVALID_TRANSACTION_ID;
  
  rrc_MeasurementControl_r9 *mcm_r9 = NULL;
  uint8 meas_id_val = 0;

  gps_meas_params = &temp_gps;
  memset(gps_meas_params,0,sizeof(rrc_meas_params_struct_type));
  gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = FALSE;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.environ_ch_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.hor_accuracy_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.sfn_gps_timing_required = FALSE;
  gps_meas_params->u.meas_ctrl_params.rpt_quan.additional_assistance_data_request = FALSE;
  gps_meas_params->u.meas_ctrl_params.gps_assistance_data_incl = FALSE;
  gps_meas_params->u.meas_ctrl_params.meas_start_incl = TRUE;
  
  /* Store the message */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)pos_mcm_ptr;
  transaction_id = meas_ctrl_tr_id;

  /* By default, it is set to 0 => 1 */
  gps_meas_params->u.meas_ctrl_params.rpt_crit.u.periodic_crit.rpt_amount = 0;

  if(!(dcch_msg_ptr->message.u.measurementControl.u.later_than_r3.criticalExtensions.t
    == T_rrc_MeasurementControl_criticalExtensions_4_later_than_r4))
  {
    return FAILURE;
  }

  mcm_r9 = &dcch_msg_ptr->message.u.measurementControl.u.
    later_than_r3.criticalExtensions.u.later_than_r4.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.later_r8_criticalExtensions.u.r9;


  rrc_mcm_ptr = &mcm_r9->measurementControl_r9;
  /* Check if extended measurement is supported. If meas id extension is given then
     override the meas id given in legacy IE */
  if((rrc_meas_id_extn_support) && (TRUE == rrcmeas_check_if_meas_id_extn_is_given(mcm_r9,&meas_id_val)))
  {
    p_info_ptr->meas_id = meas_id_val;
  }
  else
  {
    p_info_ptr->meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
  }

  gps_meas_params->u.meas_ctrl_params.meas_id = p_info_ptr->meas_id ;
  
  /* Set Transaction Id and Meas Id */
  gps_meas_params->u.meas_ctrl_params.config_conf_ind = TRUE;
  
  gps_meas_params->u.meas_ctrl_params.config_ack_num = (uint8) transaction_id;
  
  gps_meas_params->u.meas_ctrl_params.rpt_quan.method = (uint8)RRC_GPS;
  gps_meas_params->params_type = (uint8)RRC_MEAS_CTRL_PARMS;
  
  WRRC_MSG1_HIGH("POS: GPS Meas choice %d",rrc_mcm_ptr->measurementCommand.t);

  
  if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,
    rrc_MeasurementCommand_r9_setup))
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_SETUP;
    
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,measurementReportingMode)))
    {
      /* Send Meas Ctrl Failure */
      WRRC_MSG0_ERROR("Meas Reporting Mode missing for SETUP");
      
      /* Set transaction ID */
      rrcmeas_mcf.rrc_transaction_id = transaction_id;
      
      /* Set error t */
      rrcmeas_mcf.error_choice = T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      
      /* Send MCF msg to UTRAN */
      rrcmeas_send_meas_ctrl_failure_msg(&rrcmeas_mcf);
      
      return FAILURE;
    }
    else
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode =
       (uint8) RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }

    if(FAILURE == rrcgps_fill_mcm_info_r9(p_info_ptr,rrc_mcm_ptr->measurementCommand.u.setup->u.up_Measurement , &gps_meas_params->u.meas_ctrl_params, transaction_id, FALSE, mcm_r9))
    {
      
      return FAILURE;
    }
  } /* End of if of set up */
  else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_mcm_ptr->measurementCommand,rrc_MeasurementCommand_r9_modify))
  {
    /* Addtl Meas processing later. Reporting crit stored in db later */
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8)RRC_MEAS_MODIFY;
    
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr, rrc_MeasurementControl_r9_IEs,measurementReportingMode))
    {
      gps_meas_params->u.meas_ctrl_params.transfer_mode_incl = TRUE;
      gps_meas_params->u.meas_ctrl_params.tranfer_mode = (uint8)
        RET_POS_RPT_TYPE(rrc_mcm_ptr->measurementReportingMode.measurementReportTransferMode);
    }
    
    /* Check if meas type present */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_mcm_ptr->measurementCommand.u.modify,
         rrc_MeasurementCommand_r9_modify,measurementType))
    {
      if(FAILURE == rrcgps_fill_mcm_info_r9(p_info_ptr, rrc_mcm_ptr->measurementCommand.u.modify->
         measurementType.u.up_Measurement, &gps_meas_params->u.meas_ctrl_params, transaction_id,
         TRUE, mcm_r9))
      {
        
        return FAILURE;
      }
    } /* End of if of Meas Type present */
  } /* End of modify */
  else
  {
    gps_meas_params->u.meas_ctrl_params.meas_cmd = (uint8) RRC_MEAS_DELETE;
    /* At present just store meas id in meas db */
    for ( count = 0; count < MAX_POS_MEAS; count++ )
    {
      if (pos_meas_id_list[count].meas_id == gps_meas_params->u.meas_ctrl_params.meas_id)
      {
        WRRC_MSG1_HIGH("POS: Meas Id %d deleted from DB", gps_meas_params->u.meas_ctrl_params.meas_id);
        pos_meas_id_list[count].meas_id = INVALID_MEAS_ID;
        pos_meas_id_list[count].meas_validity = NOT_PRESENT;
        pos_meas_id_list[count].rpt_crit = NOT_AVAILABLE;
        
        /* Also initialize additional meas */
        for ( cnt = 0; cnt < MAX_ADDITIONAL_MEAS; cnt++ )
          pos_meas_id_list[count].additional_meas[cnt] = INVALID_MEAS_ID;
        break;
      }
    } /* End of for */
    
    deleted_meas_id = (uint16) rrc_mcm_ptr->measurementIdentity;
    
    /* Delete Addtl Meas if any from the DB */
    rrcmeas_delete_addtl_meas(&deleted_meas_id);
    WRRC_MSG0_HIGH("POS: Sending GPS Meas delete Request");
    rrcgps_send_lsm_cmd(gps_meas_params);
    return SUCCESS;
  }
  
  rrcgps_send_lsm_cmd(gps_meas_params);
  return SUCCESS;

}

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
)
{

  /* Check if it is a Release */
  if (pos_info_ptr->pos_meas_cmd == RRC_MEAS_DELETE)
  {
    if (rrcgps_process_mcm_r9(msg_ptr, pos_info_ptr, meas_ctrl_tr_id) == SUCCESS)
      return SUCCESS;
    else
      return FAILURE;
  }

  /* Now route it */
  switch(pos_info_ptr->pos_method_type)
  {
  case RRC_GPS:
  case RRC_OTDOAorGPS:
    if (rrcgps_process_mcm_r9(msg_ptr, pos_info_ptr, meas_ctrl_tr_id) == SUCCESS)
      return SUCCESS;
    else
      return FAILURE;

  default:
    /* Not possible */
    break;


  } /* End of switch */
  return FAILURE;
}

#endif


#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

#endif  /* FEATURE_CGPS_UMTS_CP_WCDMA */
