#ifndef SRCHCMMEASPARMS_H
#define SRCHCMMEASPARMS_H

/*============================================================================
                            S R C H C M D R V . H
DESCRIPTION

Copyright (c) 2000 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchcmmeasparms.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
09/02/14    mm      Reduce uncertainty window size for SCH decode by half
09/12/11    yh      Remove macros that calculate num bursts
03/20/06    ms      Lint fixes
05/03/04    et      mainlined RF_MULTIMODE
04/08/04    gs      Added macros for uncertainty window for flexible
                    uncertainty feature.
01/05/04    gs      Replaced hardcoded constants with macros from gl1_hw.h
12/10/03    gs      Changed number of RSSI measurement limit per GAP from 3
                    6 in macro SRCHCM_GSM_RSSI_MEAS_PER_GAP_LIMIT under feature
                    FEATURE_CM_GSM_RSSI_MEAS_PER_GAP_LIMIT
11/20/03    jtn     For tlg14, changed values of SRCHCM_GSM_T_START_QS_PWR_SCAN and
                    SRCHCM_GSM_T_CLEAN_QS_PWR_SCAN.
12/04/02    gs      Added the feature FEATURE_CM_GSM_RSSI_MEAS_PER_GAP_LIMIT
                    It is replacement in file srchcmmeas.c for feature
                    FEATURE_CM_GSM_RSSI_MEAS_PER_GAP_LIMIT_3
                    Defined macro SRCHCM_GSM_RSSI_MEAS_PER_GAP_LIMIT to limit
                    the number of RSSI scans in gap.
10/21/02    gs      SCH decode search width macros are seperated into BSIC Id
                    for first SCH decode and BSIC reconf for subsequent ones.
10/11/02    gs      Updated the include of rfgsm.h for feature RF_MULTIMODE
                    to include rfm.h when this feature is defined.
10/08/02    gs      Corrected the calculation of number of GSM power scan to
                    exclude 1024 WCDMA chips and start and end.
10/07/02    gs      Power scan burst size now function of GSM RX tune time
                    taken from rfgsm.h
                    Added macro to calculate the number of RSSI scan done in
                    given WCDMA gap length.
08/09/02    gs      Created this file
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "gl1_hw.h"
#include "rfm.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* The following numbers are for ZIF */

/* CM start up time. CM startup start this many QS before GAP starts */
#define SRCHCM_GSM_CM_START_UP_TIME_QS (GL1_HW_CM_STARTUP_CMD_OVERHEAD_QS)

/* Power scan window start offset after gap start */
#define SRCHCM_GSM_T_START_QS_PWR_SCAN (GL1_HW_CM_STARTUP_CMD_DURATION_QS)
/* This includes the time required for cleanup */
#define SRCHCM_GSM_T_CLEAN_QS_PWR_SCAN ((GL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS) + (GL1_HW_CM_CLEANUP_CMD_DURATION_QS))

/* Size of power burst size */
#define SRCHCM_GSM_PWR_SCAN_BURST_SIZE_QS (GL1_HW_CM_PWR_MEAS_CMD_DURATION_QS)
/* Power scan burst win size (this include RF tune + burst size) */
#define SRCHCM_GSM_PWR_SCAN_BURST_MEAS_WIN_SIZE_QS ((GL1_HW_CM_PWR_MEAS_CMD_OVERHEAD_QS) + (SRCHCM_GSM_PWR_SCAN_BURST_SIZE_QS))

/* T start and cleanup for FCCH detect */
#define SRCHCM_GSM_T_START_QS_FCCH_DETECT ((GL1_HW_CM_STARTUP_CMD_DURATION_QS) + (GL1_HW_CM_ACQ_CMD_OVERHEAD_QS))
#define SRCHCM_GSM_T_CLEAN_QS_FCCH_DETECT ((GL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS) + (GL1_HW_CM_CLEANUP_CMD_DURATION_QS))

/* T start and cleanup for SCH detect */
#define SRCHCM_GSM_T_START_QS_SCH_DEC ((GL1_HW_CM_STARTUP_CMD_DURATION_QS) + (GL1_HW_CM_ASYNC_RX_CMD_OVERHEAD_QS))
#define SRCHCM_GSM_T_CLEAN_QS_SCH_DEC ((GL1_HW_CM_CLEANUP_CMD_OVERHEAD_QS) + (GL1_HW_CM_CLEANUP_CMD_DURATION_QS))
/* SCH burst size */
#define SRCHCM_GSM_SCH_BURST_SIZE_QS (GL1_HW_CM_ASYNC_RX_CMD_DURATION_QS)

/* SCH uncertaintity window or search width for initial identification */
#define SRCHCM_GSM_BSIC_ID_SCH_UNCERTAINTY_WIN_QS (GL1_HW_CM_SCH_SEARCH_WIDTH_QS / 4)
/* SCH decode window for initial identification */
#define SRCHCM_GSM_BSIC_ID_SCH_BURST_DEC_SZ \
          (SRCHCM_GSM_SCH_BURST_SIZE_QS + \
           (2 * SRCHCM_GSM_BSIC_ID_SCH_UNCERTAINTY_WIN_QS))
           
/* SCH uncertaintity window or search width for BSIC reconfirmation */
#define SRCHCM_GSM_BSIC_RECONF_SCH_UNCERTAINTY_WIN_QS (GL1_HW_CM_SCH_SEARCH_WIDTH_QS / 4)
/* SCH decode window for BSIC reconfirmation */
#define SRCHCM_GSM_BSIC_RECONF_SCH_BURST_DEC_SZ \
          (SRCHCM_GSM_SCH_BURST_SIZE_QS + \
           (2 * SRCHCM_GSM_BSIC_RECONF_SCH_UNCERTAINTY_WIN_QS))

#endif /* SRCHCMMEASPARMS_H */

