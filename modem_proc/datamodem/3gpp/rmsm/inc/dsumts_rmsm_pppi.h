#ifndef DSUMTS_RMSM_PPPI_H
#define DSUMTS_RMSM_PPPI_H
/*===========================================================================

                      D S U M T S _ R M S M _ P P P I  . H

DESCRIPTION
  The Data Services RM State Machine(PDP-PPP) specific source file. 
  This contains the state machine specific data definitions 
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/dsumts_rmsm_pppi.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/20/11    ttv     Merged silent redial support changes for UMTS tethered calls.
03/04/09    sa      AU level CMI modifications.
02/14/08    vk      Graceful termination of PDP-PPP calls.
11/15/07    asn     Added support for new feature MT-PDP terminating in TE
04/18/03    vsk     Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#ifdef FEATURE_DATA 
#include "dstask_v.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "dsat_v.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)                    
/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Declaration of the states of the RM state machine for PDP-PPP calls.
---------------------------------------------------------------------------*/
typedef enum
{
  DSUMTS_RMSM_PPP_MIN_STATE = -1,
  DSUMTS_RMSM_PPP_NULL_STATE = 0,                     /* Initial state    */
  DSUMTS_RMSM_PPP_WAITING_FOR_UM_UP_STATE = 1,        /* Um coming up     */
  DSUMTS_RMSM_PPP_UM_RM_UP_STATE = 2,                 /* Rm and Um up     */
  DSUMTS_RMSM_PPP_WAITING_FOR_UM_DOWN_STATE = 3,      /* Um going down    */
#ifdef FEATURE_DATA_TE_MT_PDP
  DSUMTS_RMSM_PPP_RINGING_TE = 4,                     /* Um wants MT pkt call */
#endif /*FEATURE_DATA_TE_MT_PDP*/
  DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE = 5,
  DSUMTS_RMSM_PPP_MAX_STATE
} dsumts_rmsm_pppi_state_type;


/*---------------------------------------------------------------------------
  Structure to hold pdp-ppp state machine specific information 
---------------------------------------------------------------------------*/
typedef struct 
{
  dsumts_rmsm_pppi_state_type state;                 /* current state      */
  void* um_ppp_ev_handle;      /* Handle for registering for UM PPP events */

}dsumts_rmsm_pppi_specific_info;


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


#endif /*defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)*/
#endif /* FEATURE_DATA */
#endif /* DSUMTS_RMSM_PPPI_H */
