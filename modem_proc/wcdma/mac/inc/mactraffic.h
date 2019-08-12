#ifndef MACTRAFFIC_H
#define MACTRAFFIC_H

/*========================================================================================

   MAC - Traffic Volume measurements header file

DESCRIPTION
  This file contains the declaration of the functions implemented for the traffic volume
  measurement feature.
    
  Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2002-2005, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/mactraffic.h_v   1.1   10 Jul 2002 10:03:32   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/mactraffic.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------------------------
01/08/13    kc      API to provide validity of measurement ID
06/11/12    mrg     TVM optimization for MRAB 
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/05/12    mrg     HS-RACH & MAC-I Feature code integrated
10/01/10    mrg     TVM event and periodic measurements handled with different timers
05/07/09    ssg     Updated Copyright Information
04/29/09    av      Fixed compiler warnings in L2 
10/17/08    grk     Merged taxis changes to mainline.
01/11/08    mrg     Added prototype for mac_compute_traffic_volume_above_threshold() 
                    under FEATURE_TVM_IND_IN_CU feature flag
03/09/05    sk      Fixed all lint errors.
12/08/04    sk      Fixed compiler warnings.
07/23/04    sk      Added prototype for mac_ul_update_trch_info().
09/26/03    vk      Added declaration for mac_reset_periodic_event_trigger_counters()
06/26/03    sk      Added declarartion for populate_rach_trch_id().
08/16/02    sk      Added declaration of mac_ul_delete_all_traffic_volume_meas() to remove
                    compiler warnings.
07/08/02    sk      Removed FEATURE_TRAFFIC_VOLUME_MEAS. Added the declarations for
                    proc_block_dtch_tx_on_rach() and mac_ul_delete_traffic_volume_meas()
05/01/02    sk      Created the file to implement traffic volume measurement feature.
========================================================================================*/

/*=======================================================================================

                        INCLUDE FILES 

========================================================================================*/
#include "wcdma_variation.h"
#include "macinternal.h"

/*========================================================================================

                        EXTERN DATA DECLARATIONS

========================================================================================*/

/* Pointer to ordered config data of Traffic volume measurements and reporting */
extern mac_ul_traffic_meas_config_type   *ul_traffic_config_ptr;

extern rex_timer_type                     mac_ul_tvm_event_tmr_blk;

extern rex_timer_type                   mac_ul_tvm_periodic_tmr_blk;

extern maci_ul_traffic_cfg_index_type     ul_traffic_info_table[UE_NUM_TRAFFIC_MEAS];

extern boolean block_transmission;

/*========================================================================================

                        CONSTANT TYPE DATA DECLARATIONS

========================================================================================*/

/*========================================================================================

                        MACRO DEFINITIONS

========================================================================================*/

/* Uplink traffic volume measurement info table maintained internally by MAC */
/* The index (1... MAX_MEAS_ID(16)) is the same as the measurement id for the*/
/* traffic volume measurement and the stucture has the corresponding info    */
/* for that measurement identity */

#ifdef  __GNUC__
#define GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_idx)  &ul_traffic_info_table[meas_idx-MAC_UL_MIN_MEAS_ID]

/* MACRO to check Traffic volume measurement ID */
#define TRAFFIC_MEAS_ID_IS_INVALID(meas_id)\
            ((meas_id < MAC_UL_MIN_MEAS_ID) || (meas_id > MAC_UL_MAX_MEAS_ID)) 
#else  /* __GNUC__ */

#define GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_idx)  &ul_traffic_info_table[##meas_idx##-MAC_UL_MIN_MEAS_ID]

/* MACRO to check Traffic volume measurement ID */
#define TRAFFIC_MEAS_ID_IS_INVALID(meas_id)\
            ((##meas_id## < MAC_UL_MIN_MEAS_ID) || (##meas_id## > MAC_UL_MAX_MEAS_ID)) 
#endif /* __GNUC__ */

#define TRAFFIC_MAX_SAMPLES     26



/*-------------------------------------------------------------------------------------*/
/* This macro will subtract "dec" from "idx" and give the new index in a circular      */
/* array of size "interval"                                                            */
/*-------------------------------------------------------------------------------------*/
#define TRAFFIC_IDX_DIFF(idx,dec)  ((idx-dec+TRAFFIC_MAX_SAMPLES)%TRAFFIC_MAX_SAMPLES)

/*========================================================================================

                        FUNCTION DECLARATIONS

========================================================================================*/

#ifdef FEATURE_WCDMA_MRAB_TVM_MAC_OPT
#error code not present
#endif /* FEATURE_WCDMA_MRAB_TVM_MAC_OPT */

/*===============================================================================

FUNCTION MAC_UL_TRAFFIC_VOLUME_INIT

DESCRIPTION Initialise the measurement id config table.  
===============================================================================*/

void mac_ul_traffic_volume_init (void);

/*===================================================================================
 FUNCTION NAME  config_ul_mac_traffic_volume

 PURPOSE    This function process UL MAC traffic volume Command. It sends a config 
            confirmation message to RRC in the case that RRC has requested for it. 
            Then it processes the commands based on the command type : setup, modify, 
            delete, delete_all.
====================================================================================*/

void config_ul_mac_traffic_volume
  (/* Pointer to traffic volume config information */
  mac_ul_traffic_meas_config_type *traffic_cfg_ptr
  );
/*=====================================================================================
 FUNCTION NAME  mac_ul_block_rach_tx

 PURPOSE    This function blocks DTCH transmissions on RACH when the UE is in CELL_FACH
            state. It resumes transmissions when the IE Tx interruption after trigger 
            elapses. block_rach boolean indicate sthe action to be taken on receving 
            this command. If block_rach is TRUE then we block STCH transmissions on
            RACH else we resume transmissions.
=====================================================================================*/
void mac_ul_block_rach_tx (boolean block_rach );

/*====================================================================================
 FUNCTION NAME  proc_block_dtch_tx_on_rach()

 PURPOSE  If the IE is in cell_fach state then it can prohibit dtch transmissions on
        RACH. block_transmission is set to TRUE when we receive a BLOCK cmd from RRC
        The transmission is resumed when the Tx interruption timer expires 
=====================================================================================*/
void proc_block_dtch_tx_on_rach(rlc_ul_buf_status_type *);

/*======================================================================================
 FUNCTION NAME  mac_ul_delete_traffic_volume_meas

 PURPOSE    This function deletes the traffic volume measurement associated with the 
            measurement identity specified.
            This function returns a boolean. true indicates that the delete operation
            was sucessful.
======================================================================================*/

boolean mac_ul_delete_traffic_volume_meas(uint16 meas_id);

/*========================================================================================
 FUNCTION NAME  mac_ul_delete_all_traffic_volume_meas

 PURPOSE    This function deletes all the traffic volume measurements that have been done 
            so far. This function returns a boolean. True indicates that the delete_all
            function was sucessful.
========================================================================================*/

boolean mac_ul_delete_all_traffic_volume_meas (void);

/*==================================================================================
 FUNCTION NAME  populate_rach_trch_id

 PURPOSE: UTRAN can configure traffic volume measurements by specifying both the
 DCH and RACH objects in the measurement object list. In the case when the current ul 
 cctrch type is DCH and they have configured measurements for RACH, the trch_id will
 not be available for RACH. Hence this function is called when UE transitions to
 cell_fach state and a RACH id is required to do the measurements.
==================================================================================*/
boolean populate_rach_trch_id(void) ;


/*==================================================================================
 FUNCTION NAME  mac_reset_periodic_event_trigger_counters

 PURPOSE: Clears periodic and event trigger counters on state transition
==================================================================================*/
void mac_reset_periodic_event_trigger_counters(void);

/*==================================================================================
 FUNCTION NAME  mac_ul_update_trch_info

 PURPOSE: Update the transport channel list for TVM
==================================================================================*/
void mac_ul_update_trch_info(void);

/*==================================================================================
 FUNCTION NAME  mac_compute_traffic_volume_above_threshold
 
PURPOSE: In case of TVM meas id 4A, Checks whether transport channel buffer 
         occupancy is crossing the configured "reporting threshold"
         TRUE - TrCh BO > Reporting Threshold
         FALSE - TrCh BO < Reporting Threshold
         NOTE: It is used in case of Cell update with cause UL Data transmission
               RRC passes all the LC information as part of RACH Ptr
==================================================================================*/
boolean mac_compute_traffic_volume_above_threshold_cu
(
/* measurement id */
uint16                            meas_id,

/* pointer to Rach config */
mac_ul_rach_cctrch_config_type    *oc_rachptr

#ifdef FEATURE_WCDMA_HS_RACH
/* pointer to HS-Rach config */
,mac_hs_rach_config_s_type         *oc_hs_rachptr
#endif
);

/*===================================================================================
 FUNCTION NAME  maci_post_cfg_cnf_msg()

 PURPOSE    This function posts a config confirmation message to RRC
===================================================================================*/
void maci_post_cfg_cnf_msg
(
/* Acknowledgement number*/
uint16   config_ack_num,

/* Measurement identity for which ack is sent to RRC*/
uint16  meas_id,

/* Indicates if the configuration was succesful or not*/
boolean status
);

/*===================================================================================
 FUNCTION NAME  maci_post_traffic_volume_meas_report

 PURPOSE    This function posts the result of traffic volume measurements for a
            measurement identity.
===================================================================================*/
void maci_post_traffic_volume_meas_report
(
/* Contains the measurement results for a particular measurement id*/
mac_ul_traffic_meas_ind_type  *meas_ind_ptr
);

/*==================================================================================
 FUNCTION NAME  check_meas_report_qty

 PURPOSE    This function checks if measurement interval is specified in the
            case when we are asked to report the average or variance.

==================================================================================*/
boolean check_meas_report_qty
(
/* Configuration parameters recv'd from RRC */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr
);

/*==================================================================================
 FUNCTION NAME  check_report_mode

 PURPOSE    This function return TRUE if the report mode is periodic or event
            triggered else returns FALSE

==================================================================================*/
boolean check_report_mode
(
/* report mode */
mac_ul_traffic_report_mode_e_type report_mode
);

/*==================================================================================
 FUNCTION NAME  populate_object_list

 PURPOSE    This function finds all the uplink currently active transport channels
            and populates the transport channel information array. It returns the number
            of transport channels.
==================================================================================*/
uint16 populate_object_list
(
/* Array of transport channel type and id information*/
mac_ul_traffic_trch_info_type     trch_info[]

);

/*==================================================================================
 FUNCTION NAME  check_n_copy_event_trigger_params

 PURPOSE    This function copies the event trigger parameters for the respective
            transport channels after performing the check to see if the transport
            channels specified are valid. It copies the parameters only if they
            are valid. If the transport channel list is not specified then there
            is only one transport channel specific event criteria specified and
            it is applicable to all the  transport channels as indicated by the
            measurement object list.
==================================================================================*/

boolean check_n_copy_event_trigger_params
(
/* Pointer to Config parameters internally maintained by MAC*/
maci_ul_traffic_cfg_param             *int_tmp_cfg_ptr,

/* Pointer to Config parameters sent by RRC to MAC */
mac_ul_traffic_event_trigger_cfg_type *oc_event_cfg_ptr

);
/*==================================================================================
 FUNCTION NAME  check_n_copy_modify_event_trigger_params

 PURPOSE    This function copies the event trigger parameters based on the
            modified parameters received from UTRAN for the respective
            transport channels after performing the check to see if the transport
            channels specified are valid. It copies the parameters only if they
            are valid. If the transport channel list is not specified then there
            is only one transport channel specific event criteria specified and
            it is applicable to all the  transport channels as indicated by the
            measurement object list.
==================================================================================*/

boolean check_n_copy_modify_event_trigger_params
(
/* Pointer to Config parameters internally maintained by MAC*/
maci_ul_traffic_cfg_param             *int_tmp_cfg_ptr,

/* Pointer to Config parameters sent by RRC to MAC */
mac_ul_traffic_event_trigger_cfg_type *oc_event_cfg_ptr
);
/*==================================================================================
 FUNCTION NAME  check_n_copy_periodic_params

 PURPOSE  This function copies the periodic parameters only if the transport channels
          specified are valid. If the transport channel list is not specified then
          it populates the object list too.
==================================================================================*/
boolean check_n_copy_periodic_params
(
/* Pointer to the config information internally stored by MAC */
maci_ul_traffic_cfg_param                 *int_tmp_cfg_ptr,

/* Pointer to the config information received from RRC */
mac_ul_traffic_meas_config_type           *oc_traffic_cfg_ptr

);

/*==================================================================================
 FUNCTION NAME  mac_ul_setup_traffic_volume_meas

 PURPOSE    This function populates the traffic volume measurement table and starts
            traffic volume measurements for the corresponding id.
            This function returns a boolean. True indicates the setup was sucessful.
            It goes through all the parameters specified by RRC and checks to see
            if they are valid. If it is valid then it copies them to the local
            memory.
==================================================================================*/

boolean mac_ul_setup_traffic_volume_meas
(
/* Pointer to traffic volume config information */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr
);

/*========================================================================================
 FUNCTION NAME  compute_obj_list_raw_payload()

 PURPOSE        This function calculates the raw rlc payload for all the radio bearers.

=========================================================================================*/
void compute_obj_list_raw_payload(void  );
/*=============================================================================
 FUNCTION NAME  compute_obj_list_raw_payload_cu()

 PURPOSE        Calculates the raw rlc payload for all the radio bearers.
=============================================================================*/
void compute_obj_list_raw_payload_cu
(
#ifdef FEATURE_WCDMA_HS_RACH
    mac_hs_rach_config_s_type     *oc_hs_rachptr,
#endif
mac_ul_rach_cctrch_config_type    *oc_rachptr,

rlc_ul_buf_status_type            *int_traffic_ul_buf_status_ptr
);

/*==================================================================
 FUNCTION NAME proc_event_trigger_criteria_cu

 PURPOSE  For all the TrCh which are configured for meas Id 4A,
          TrCh BO is calculated and checked against the threshold
          True - BO > Threshold, FALSE - BO < Threshold
===================================================================*/
boolean proc_event_trigger_criteria_cu
(
/* Measurement identity */
uint16                          meas_id,

#ifdef FEATURE_WCDMA_HS_RACH
/* pointer to HS-Rach config */
mac_hs_rach_config_s_type         *oc_hs_rachptr,
#endif

/* pointer to Rach config */
mac_ul_rach_cctrch_config_type          *oc_rachptr,

/*Internal pointer to  UL Buffer status obtained from RLC */
rlc_ul_buf_status_type                           *int_traffic_ul_buf_status_ptr
);

/*========================================================================================
 FUNCTION NAME compute_trch_traffic_volume

 PURPOSE    This function computes the Trasnport channel traffic volume which is sum of
            buffer occupancies of RBs multiplexed onto the transport channel. The function
            returns the number of bytes so computed for the transport channel.
            Returns Buffer occupancy of all rbs multiplexed onto the same transport
            channel
=========================================================================================*/

uint32 compute_trch_traffic_volume
(
/* Compute transport channel traffic volume for this transport channel id */
tr_ch_id_type     trch_id
);

/*========================================================================================
 FUNCTION NAME  prepare_n_send_traffic_meas_report()

 PURPOSE        This function prepares the measurement report for the measurement id
                specified. This function calculates the average, variance or raw payload
                as specified in the report quantity.

=========================================================================================*/

void prepare_n_send_traffic_meas_report
(
/* Measurement identity */
uint16                          meas_id
);

/*====================================================================================
 FUNCTION NAME proc_event_trigger_criteria

 PURPOSE  For each transport channel listed in the report criteria for this
          measurement identity this function checks , if the transport channel traffic
          volume is greater than or lesser than the threshold value corresponding to
          the transport channel, based on the event (4A, 4B). It then sets a trigger
          and generates a report to be sent to RRC.

=======================================================================================*/

void proc_event_trigger_criteria
(
/* Measurement identity */
uint16                          meas_id
);

/*=======================================================================================
 FUNCTION NAME proc_periodic_mode_criteria

 PURPOSE  This function checks if it is time to send a report based on comparing
          periodic timer count and the report interval. Also it checks if there
          are any limits on the number of reports that MAC can send to RRC.

========================================================================================*/
void proc_periodic_mode_criteria
(
/* Measurement identity */
uint16                          meas_id
);

/*========================================================================================
 FUNCTION NAME  check_modify_params

 PURPOSE  This function checks the config information sent by RRC in the case of
          MODIFY cmd and sees if the parameters are valid. Returns TRUE if all the
          parameters which are modified are valid.
========================================================================================*/
boolean check_modify_params
(
/* Pointer to config information sent by RRC */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr,

/* POinter to config information internally maintained by MAC */
maci_ul_traffic_cfg_param       *int_tmp_cfg_ptr
);

/*========================================================================================
 FUNCTION NAME  mac_ul_modify_traffic_volume_meas

 PURPOSE    This function modifies the traffic volume measurement for this measurement
            identity.This function returns a boolean. True indicates that the modify
            operation was sucessful. The modify_status_ind is set to true after checking
            if the transport channels for which measurements have been requested for
            exists.
========================================================================================*/
boolean mac_ul_modify_traffic_volume_meas
(
/* Pointer to traffic volume config information */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr
);

/*====================================================================================
 FUNCTION NAME  mac_ul_save_traffic_volume_meas()

 PURPOSE  Svae the configuration information for this measurement identity and
 temporarily suspend measurements until a resume command is received for this
 meas_id.
=====================================================================================*/
boolean mac_ul_save_traffic_volume_meas(uint16 meas_id);

/*====================================================================================
 FUNCTION NAME  mac_ul_resume_traffic_volume_meas()

 PURPOSE  Resume measurementsfor this  identity and
=====================================================================================*/

boolean mac_ul_resume_traffic_volume_meas(uint16 meas_id);

/*==================================================================================
 FUNCTION NAME  mac_update_object_list

 PURPOSE    This function finds all the uplink currently active transport channels
            and populates the transport channel information array. It returns the number
            of transport channels.
==================================================================================*/
uint16 mac_update_object_list
(
  uint16                            ntrch,
  /* Array of transport channel type and id information*/
  mac_ul_traffic_trch_info_type     trch_info[]

);

/*==================================================================================
 FUNCTION NAME  MAC_UL_IS_MEAS_VALID

 PURPOSE    This function would return TRUE if meas_id is valid & is active per MAC TVM database.
==================================================================================*/
boolean mac_ul_is_meas_valid(uint16 meas_id);

#ifdef FEATURE_WCDMA_HS_RACH
/*========================================================================================
 FUNCTION NAME  populate_log_chan_info()

 PURPOSE        This function populates rlc_ids of logChs mapped on to HS-RACH TrCh
=========================================================================================*/
uint8 populate_hs_rach_log_chan_info
(
  mac_hs_rach_config_s_type *oc_hs_rachptr, 
  rlc_lc_id_type hs_rach_rlc_id[]
);
#endif
#endif
