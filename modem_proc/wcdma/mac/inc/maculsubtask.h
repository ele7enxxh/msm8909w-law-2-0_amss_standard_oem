#ifndef MACULSUBTASK_H
#define MACULSUBTASK_H
/*===============================================================================================

          U P L I N K   M A C : S U B T A S K   H E A D E R   F I L E 

                
GENERAL DESCRIPTION
  
  This module contains the declaration of the functions and data types
  defined in downlink MAC subtask file.
  
Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002, 2004, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

================================================================================================*/

/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/maculsubtask.h_v   1.8   10 Jul 2002 10:06:28   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/maculsubtask.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/06/12   mrg     RACH staus info from L1 to MAC indicaiton with signal based approch enhanced with double buffer.
01/02/12   grk     Feature cleanup
12/06/11   ksr     Fix to handle back to back RACH ABORT and RACH NO ACK to MAC from L1
                   Moving to command based interface from signal based approach.
10/18/10   ssg     Added changes for new WCDMA-L2 interface.
10/01/10   mrg     TVM event and periodic measurements handled with different timers
05/07/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
07/16/04   sk      Added prototype for mac_ul_handle_tfcs_control_timer_expiry() 
                   to handle the expiry of the control duration timer for TFCC message. 
                   Added under FEATURE_TFCS_CONTROL_ENABLE
08/24/02   sk      Removed function declaration proc_dl_mac_config_cnf_signal()
07/08/02   sk      Removed FEATURE_TRAFFIC_VOLUME_MEAS
06/06/02   sk      Added function declarations for proc_dl_mac_config_cnf_signal(), 
                   proc_ul_mac_config_signal() under FEATURE_MAC_ACTIVATION_TIME
06/19/01   ps      Converted UL TX Status and Build Frame Indications from commands to signals

===============================================================================================*/
#include "l2ultask.h"

/*================================================================================================
 FUNCTION NAME  init_ul_mac

 PURPOSE        Initialize UL MAC 

================================================================================================*/
void    init_ul_mac 
(
    void
);

/*===============================================================================================
 FUNCTION NAME  proc_ul_mac_commands

 PURPOSE        Process UL MAC Commands

 DESCRIPTION    
================================================================================================*/
void    proc_ul_mac_commands 
(
    /* Command ID */
    wcdma_l2_ul_cmd_enum_type     cmd_id, 
    
    /* Pointer to command data */
    wcdma_l2_ul_cmd_data_type     *cmd_data_ptr
);


/*=================================================================================================
 FUNCTION NAME  mac_ul_handle_tfcs_control_timer_expiry()

 PURPOSE    This function process expiry of the control duration
================================================================================================*/

void mac_ul_handle_tfcs_control_timer_expiry(void);



/*================================================================================================
 FUNCTION NAME  proc_ul_mac_hfn_req

 PURPOSE        Process UL MAC HFN Request

 PARAMETERS     None

 DESCRIPTION    
================================================================================================*/
extern  void    proc_ul_mac_hfn_req 
(
    void
);

/*===============================================================================================
 FUNCTION NAME  proc_ul_tx_status_ind 
 
 DESCRIPTION    This function processes transmit status received from PHY layer
    
===============================================================================================*/
extern  void    proc_ul_tx_status_ind
(
    void
);

/*==============================================================================================
 FUNCTION NAME  proc_build_ul_frame_ind

 FUNCTION       This function processes build UL frame event
===============================================================================================*/
extern  void    proc_build_ul_frame_ind
(
    void
);



/*===============================================================================
 FUNCTION NAME  mac_ul_handle_tvm_event_tmr

 PURPOSE    This function is called when MAC traffic volume timer signal
            expires every 10ms. This is used as a base timer and all the other
            timers like trigger_timer..etc are done using counters.
            This function checks for each event mode measurement identity , if the
            timer counters have expired and posts the measurement report to RRC.
================================================================================*/

void mac_ul_handle_tvm_event_tmr (void);


/*===============================================================================
 FUNCTION NAME  mac_ul_handle_tvm_periodic_tmr

 PURPOSE    This function is called when MAC traffic volume timer signal
            expires every (10ms* MAC_UL_PERIODIC_TVM_TIMER_SCALE). This is used as a 
            base timer and report_interval is computed using counters.
            This function checks for each periodic mode measurement identity if the 
            timer counters have expired posts the measurement report to RRC.
================================================================================*/

void mac_ul_handle_tvm_periodic_tmr (void);



/*==============================================================================================
 FUNCTION NAME  proc_ul_mac_config_signal

 FUNCTION   This function processes the config signal received from the physical layer.
===============================================================================================*/
void proc_ul_mac_config_signal (void);

#endif  /* MACULSUBTASK_H */


