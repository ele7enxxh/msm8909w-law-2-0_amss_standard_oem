#ifndef MACDLSUBTASK_H
#define MACDLSUBTASK_H
/*================================================================================

    D O W N L I N K   M A C : S U B T A S K   H E A D E R   F I L E


GENERAL DESCRIPTION

  This module contains the declaration of the functions and data types
  defined in downlink MAC subtask file.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002,2004-2006,2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macdlsubtask.h_v   1.5   11 Jun 2002 18:01:34   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/macdlsubtask.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
01/02/12   grk     Feature cleanup
10/18/10   ssg     Added changes for new WCDMA-L2 interface.
05/07/09   ssg     Updated Copyright Information
04/26/06   gsc     Added support for HSDPA re-config optimizations
11/03/05   sk/gs   Added function proc_dl_mac_config_at_l1_failure_signal
08/08/05   gsc     Removed process_dl_mac_hs_config_complete_signal().
02/15/05   gsc     Added HSDPA support under FEATURE_HSDPA feature flag.
12/08/04   sk      Fixed compiler warnings.
09/04/02   sk      Added declaration for function init_dl_mac();
08/24/02   sk      Removed declaration for function maci_post_ul_config_cnf(). MAC DL
                   directly posts L1 the cnf signal.
06/06/02   sk      Added function declarations for proc_dl_mac_config_signal (),
                   maci_post_ul_config_cnf() under FEATURE_MAC_ACTIVATION_TIME

==================================================================================*/
/*==============================================================================================
 FUNCTION NAME  init_dl_mac()

 PURPOSE  Initializes the DL MAC parameters related to ciphering

 DESCRIPTION
===============================================================================================*/
void init_dl_mac(void);

/*==============================================================================================
 FUNCTION NAME  proc_dl_mac_commands

 PURPOSE        Process DL MAC Commands

 DESCRIPTION
===============================================================================================*/
void    proc_dl_mac_commands
(
    /* Command ID */
    wcdma_l2_dl_cmd_enum_type     cmd_id,

    /* Pointer to command data */
    wcdma_l2_dl_cmd_data_type     *cmd_data_ptr
);

/*==========================================================================
 FUNCTION NAME  proc_dl_mac_hfn_req

 PURPOSE        Process DL MAC HFN Request

 PARAMETERS     None

 DESCRIPTION
===========================================================================*/
extern  void    proc_dl_mac_hfn_req
(
    void
);

/*==============================================================================================
 FUNCTION NAME  proc_dl_mac_config_signal

 FUNCTION   This function processes the config signal received from the physical layer.
===============================================================================================*/
void proc_dl_mac_config_signal (void);


#include "wcdma_variation.h"
/*==============================================================================================
 FUNCTION NAME  proc_dl_mac_hs_config_signal

 FUNCTION   This function processes the MAC-hs config signal received from the physical layer.
===============================================================================================*/
void proc_dl_mac_hs_config_signal (void);

/*==============================================================================================
 FUNCTION NAME  proc_dl_mac_config_at_l1_failure_signal

 FUNCTION   This function processes the config failed signal received from the physical layer so
            that mac_hs_reset can be performed if pending.
===============================================================================================*/
void proc_dl_mac_config_at_l1_failure_signal (void);

#endif

