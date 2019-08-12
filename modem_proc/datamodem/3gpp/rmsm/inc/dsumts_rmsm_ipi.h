#ifndef DSUMTS_RMSM_IPI_H
#define DSUMTS_RMSM_IPI_H
/*===========================================================================

                     D S U M T S _ R M S M _ I P I . H

DESCRIPTION
  The Data Services RM State Machine(PDP-IP) specific header  file. 
  This contains the state machine specific function data definitions 
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/dsumts_rmsm_ipi.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    vs      Added support for 3GPP IPv6 over PPP
06/20/11    ttv     Merged silent redial support changes for UMTS tethered calls.
07/09/10    asn     Support for Arbitration
03/04/09    sa      AU level CMI modifications.
11/15/07    asn     Added support for new feature MT-PDP terminating in TE
09/09/06    rr      Added NV Item support for NBNS.
05/17/06    aw      Defined a new critical section for sm info structure.
12/02/05    rr      Renamed dsumtsps_rm_ppp_info to network_params_info_type
                    to support RMNET calls with legacy PPP calls.
08/13/03    vsk     added fields to generate IPCP naks in 
                    dsumts_rmsm_ipi_specific_info
04/22/03    vsk     code review cleanup. updated comments 
04/18/03    vsk     Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "dstask_v.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "dsat_v.h"
#include "dsm.h"
#include "dhcp6_sl_server_mgr.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)


/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Declaration of the states of the RM state machine for PDP-IP calls.
---------------------------------------------------------------------------*/
typedef enum
{
  DSUMTS_RMSM_IP_MIN_STATE = -1,
  DSUMTS_RMSM_IP_NULL_STATE = 0,                     /* Initial state     */
  DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE,            /* Um coming up      */
  DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE,        /* Rm PPP Coming up  */
  DSUMTS_RMSM_IP_RM_RECONFIGURING_STATE,             /* Rm Reconfiguring  */
  DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE,          /* Um going down     */
  DSUMTS_RMSM_IP_UM_RM_UP_STATE,                     /* Rm and Um up      */
  DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB,                 /* Um waitng for arb */     

  /* Waiting for RS/RA to complete */
  DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE,   

  /* Um Up, Waiting for IPv6 Autoconfig to end on Rm Side */
  DSUMTS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE,

  /* Um Down, Waiting for IPv6 Autoconfig to end on Rm Side */
  DSUMTS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE,

  DSUMTS_RMSM_IP_MAX_STATE
} dsumts_rmsm_ipi_state_type;


typedef struct
{

  uint32 state;
  boolean inited;
  ps_iface_ipfltr_handle_type  rm_fltr_handle;       /* Rm ICMP6 filters */
  void                         *um_prefix_update_buf_ptr;
  uint64                       gateway_iid;
  uint64                       te_iid;
  uint64                       prefix;

  dhcp6_sl_server_duid         dhcp6_server_duid;
  void *                       dhcp6_handle; /* handle from dhcp6 server */

}v6_sm_type;

/*---------------------------------------------------------------------------
  PDP-IP state machine specific data
---------------------------------------------------------------------------*/
typedef struct 
{
  dsumts_rmsm_ipi_state_type state;               /* current state        */

  v6_sm_type                 v6_sm_info;          /* IPv6 specific info   */


}dsumts_rmsm_ipi_specific_info;
   
   

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

#endif /*defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  || defined (FEATURE_DATA_LTE)*/
#endif /* DSUMTS_RMSM_IPI_H */
