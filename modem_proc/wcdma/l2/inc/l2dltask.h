#ifndef L2DLTASK_H
#define L2DLTASK_H
/*============================================================================================

                      D L  L A Y E R 2    T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with downlink layer 2 task.

Copyright (c) 1991- 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002, 2005-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================================*/

/*============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/L2/vcs/l2dltask.h_v   1.14   11 Jun 2002 18:10:10   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l2/inc/l2dltask.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------------------------
07/13/12   grk   MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
07/24/12   geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/24/12   zr    Added API to retrieve WCDMA L2 DL TCB pointer 
04/26/12   grk   Added new API file exposing L2 command structures
02/01/12   grk   Feture cleanup
10/18/10   ssg   Added changes for new WCDMA-L2 interface.
10/28/10   kvk   Removed featurization from l2_dl_cmd_enum_type to make it compatible with 
                 the defintion in wcdmamvsif.h
07/07/10   pj    Added changes to use a global base timer for status processing
05/06/10   pj    Added function prototypes 
12/04/09   ssg   New L2 interface
05/07/09   ssg   Updated Copyright Information
05/04/09   kvk   Added support to send the FC command for WINDOW size update 
                 rather than directly updating the WINDOW size from timer CB function
03/01/09   rm    Added new featurization for FEATURE_CMI.
12/18/08   ksr   L2 Mutex changes to avoid concurrency issues
05/16/08   ssg   Added new cmd CRLC_DL_CONTINUE_REQ.
03/17/08   ar    Introduced signal RLC_DL_FC_SIG under feature
                 FEATURE_DL_CPU_BASED_FC
02/01/08   pj    Changed FEATURE_MBMS flag to FEATURE_MODEM_MBMS.
11/30/07   ssg   Added mbms support under FEATURE_MBMS.
01/04/07   ssg   Added signal RLC_DL_LOG_CIPHER_PKT_SIG to log downlink cipher PDU packets.
11/10/06   vg    added new cmd for PDCP WM clenup
04/26/06   gsc   Added support for HSDPA Re-config optimizations under
                 FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION
11/11/05   pdv   Fix for CR79364
11/10/05   gsc/sk  Defined MAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND for HS->DCH failure handling
08/08/05   gsc   Removed MAC_HS_CONFIG_COMPLETE_SIG as it's no longer needed.
06/08/05   aw    Added new command RLC_HANDLE_SDU_DISCARD_TIMEOUT_EVENT to
                 handle RLC SDU discard timer when timer fires off.
05/03/05   mv    Added the command CRLC_DL_ABORT_CIPHER_CONFIG_REQ to abort the new
                 ciphering configuration.
                 Added the parameter rlc_abort_cipher_config_type to
                 l2_dl_cmd_data_type.
03/04/05   gsc   Merged HSDPA support from l2_msm6275 branch.
12/10/02   ttl   Added RLC_DL_TIMER_BACKUP_SIG for rlc backup timer.
07/31/02   ttl   Removed RLC_DL_AM0_TIMER_EXP_SIG, RLC_DL_AM1_TIMER_EXP_SIG,
                 RLC_DL_AM2_TIMER_EXP_SIG and RLC_DL_AM3_TIMER_EXP_SIG.
                 Removed RLCI_DL_10MS_TIMER.
06/06/02   sk    Added signal MAC_PHY_DL_CONFIG_IND. defined under FEATURE_MAC_ACTIVATION_TIME
05/14/02   ttl   Naming changed of wcdma_l2_dl_task.
05/13/02   ttl   Added RLC_DL_DEREGISTER_SRVC_REQ primitive.
04/10/02   ttl   Added definition of RLC_DL_AM3_TIMER_EXP_SIG.
03/15/02   sk    Defined values for RLC_DL_AM_LOG_TIMER_EXP_SIG signal.
11/09/01   sk    Defined the values for various timer signals to indicate that they have
                 expired (RLC_DL_AM0_TIMER_EXP_SIG, AM1, AM2) and added the timer_sig to
                 the enum l2_dl_cmd_enum_type
09/18/01   ttl   Added L2_DL_DOG_RPT_TIMER_SIG for watchdog.
06/18/01   rc    Added support for processing downlink PDUs in task or interrupt context.
06/07/01   rc    Added primitive RLCI_DL_RELEASE_ALL_REQ.
03/27/01   ttl   Added SDU discard callback and RLC reset primitives
03/01/01   ps    Modified to support AM RLC
01/06/01   ps    Removed the "rlcmacif.h" include file
11/22/00   ps    Initial release
============================================================================================*/

#include "wcdma_variation.h"
#include  "queue.h"
#include  "macrrcif_v.h"
#include  "l1sapcommon.h"
#include  "l1macdata.h"
#include  "rlcrrcif_v.h"
#include  "rlci.h"
#include  "rlcdsapif.h"
#include  "macrlcif.h"



/* Public portion of the API exists in the following files */
#include "wcdmamvsif.h"
#include "wcdmatmcif.h"

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*============================================================================================
                        DOWNLINK LAYER 2 SIGNAL DEFINITIONS
============================================================================================*/

/* This signal is set when an item is placed on l2_dl_cmd_q */
#define     L2_DL_CMD_Q_SIG         0x0001

/* This segnal is set to request DL MAC HFN */
#define     CMAC_DL_HFN_REQ_SIG     0x0002

/* This signal is set when there is data available on the downlink. */
#define     RLC_DL_DATA_IND_SIG     0x0004


#ifdef FEATURE_DOG
// L2 DL watchdog report timer sig.
#define     L2_DL_DOG_RPT_TIMER_SIG 0x0008
#endif

/* RLC UL backup Timer SIG  */
#define   RLC_DL_TIMER_BACKUP_SIG   0x0010

#ifdef FEATURE_RLC_QXDM_LOGGING

/* Signal to log downlink cipher packet */
#define   RLC_DL_LOG_CIPHER_PKT_SIG 0x0020

#define   RLC_DL_FC_SIG               0x0040
#define   RLC_DL_FC_TIMER_SIG         0x0080

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
/* This signal is set when an item is placed on wcdma_l2_dl_cmd_q */
#define     WCDMA_L2_DL_CMD_Q_SIG         0x0100
#endif

/* This signal is set when the log timer expires      */
#define   RLC_DL_AM_LOG_TIMER_EXP_SIG 0x0200

#endif /* FEATURE_RLC_QXDM_LOGGING */

/* This signal is set by L1 when DL MAC has to be configured */
#define  MAC_PHY_DL_CONFIG_IND  0x0400

/* This signal is set by L1 for MAC to do any cleanup when config command
   fails at L1. Only applicable id HS channel was to be configured at physical
   layer */
#define MAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND 0x0800

/* This signal is set by L1 when MAC HS has to be configured */
#define MAC_PHY_HS_CONFIG_IND 0x1000

/* This signal is set when there is data available on the downlink. */
#define     RLC_DL_DATA_REASSEMBLY_SIG     0x2000

#define RLC_DL_AM_BASE_TIMER_SIG 0x4000

/*===========================================================================
 FUNCTION     WCDMA_L2_DL_INIT_TCB

 DESCRIPTION
   This function populates the cached WCDMA L2 DL TCB pointer through rcinit. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern void wcdma_l2_dl_init_tcb(void);

/*===========================================================================
 FUNCTION     WCDMA_L2_DL_GET_TCB

 DESCRIPTION
   This function returns a pointer to the WCDMA L2 DL TCB. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   A pointer to the WCDMA L2 DL TCB.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern rex_tcb_type* wcdma_l2_dl_get_tcb(void);

#endif /* L2DLTASK_H */
