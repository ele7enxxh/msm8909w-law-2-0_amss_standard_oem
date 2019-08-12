/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               Location Manager (LM) Interface Header File

GENERAL DESCRIPTION
  This file contains the LM (Location Manager) data structure 
  definitions and prototypes for all functions that are visible to other tasks.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/lm/inc/lm_iface.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $ 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef LM_IFACE_H
#define LM_IFACE_H


/*
 * Include files
*/

#include "gps_types.h"
#include "gps_consts.h"
#include "aries_os_globals.h"
#include "gps_common.h"
#include "sm_api.h"
#include "sm_gpsdiag_api.h"
#include "lm_api.h"
/*
 * Constant definitions
*/

typedef enum
{
  /* IPC 0... C_USR_MSG_ID_FIRST-1 is reserved by OS */
  C_SM_LM_MSG_FIRST = ARIES_MSG_ID_TM_LM_END,  

  C_SM_LM_MSG_SESSION_REQUEST,    /* LM-TM session related request from TM to LM */
  C_SM_LM_MSG_REQUEST,            /* LM-TM session independent request from TM to LM */
  C_SM_LM_MSG_INFO,               /* LM-TM session independent info from TM to LM */

  C_SM_LM_DIAG_GSC_MSG,           /* GPS Session Control (GSC) DIAG packet from GPSDIAG to LM */ 

  C_SM_LM_SBAS_START,
  C_SM_LM_SBAS_STOP,

  C_SM_LM_SV_NO_EXIST,            /* Sv No Exist List updated from TM to LM */

  C_SM_LM_FORCE_DPO_FOR_POWER_MEAS,/* Force DPO for Power Meas Cmd from TM to LM */

  C_SM_LM_MSG_END 
} lm_ipc_message_type;

#define IS_LM_MSG(x) (((x)> C_SM_LM_MSG_FIRST) && \
                       ((x)< C_SM_LM_MSG_END) )
/*
 * Structure definitions
*/


/*
 * Function definitions
*/


#endif /* LM_IFACE_H */

