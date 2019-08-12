#ifndef HDRIND_H
#define HDRIND_H
/*===========================================================================

                 H D R   I N D I C A T I O N   R O U T E R
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Give Indication module.

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrind.h_v   1.24   14 Mar 2003 11:06:34   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrind.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/11   smd     SU API cleanup update.
09/23/11   smd     SU API cleanup.
09/24/11   cnx     Support HDRIND_AMP_PREREG_ZONE_CHANGED.
07/27/11   cnx     Merged Optimized Handoff changes.
06/03/11   cnx	   Renamed HDRIND_SCP_EHRPD_CREDENTIALS_CHANGED.   
05/18/11   wsh     Added network load balancing support
08/17/10   sju     Changed fusion feature flag. 
07/23/10   kss     Added support for ConnectionFailureReport based on SAR
07/21/10   cnx     Added support for GAUP LoadInformationSupported attribute
06/10/10   pba     IRAT Measurement feature.
05/19/10   pba     Added support for Reverse Time Transfer.
03/02/10   pba     Added support for LTE to HDR reselection
11/12/09   sju     Added support for SVDO.
10/30/09   pba     Added ehrpd support.
08/12/08   sju     Added new indication HDRIND_CCMAC_SCC_PKT_RXED
09/03/08   etv     Added HDRIND_SCM_PHY_AN_GAUP.
09/14/07   cc      Replaced HDRIND_RMAC3_... indications by HDRIND_RMAC_....
08/30/07   etv     Added HDRIND_RTC_RESTART_TIMER_EXPIRED.
08/15/07   etv     Added HDRIND_PHY_CONFIG_COMPLETE & HDRIND_PHY_PROT_NEG_FAILED
08/23/07   pba     Do not do DDARF for rude wakeups.
08/21/07   wsh     Made changes to support Silent Redial for sending CR+RoR 
07/23/07   sju     Added HDRIND_SCHEDULER_GROUP_CHANGED
06/19/07   cc      Added support for AT-initiated FTC MAC Config.
06/14/07   kss     Added support for AT-initiated AC MAC config.
06/13/07   etv     Added HDRIND_OUTAGE_IMMINENT for Outage Notification.
05/29/07   mpa     Added HDRIND_AMP_FAILED_NEW_NAI.
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
04/05/07   pba     Updated 7500 metacomments.
03/27/07   wsh     Added support for new prior session handling
02/01/07   sju     Added support for QOS HHO
01/08/07   yll     Added support for CSNA
11/16/06   sju     Obsoleted HDRIND_FCP_ALL_STREAMS_CLOSED indication.
09/14/06   pba     Added new indication HDRIND_IDLE_NETWORK_LOST, payload data
                   for HDRIND_LMAC_AWAKE.
07/24/06   mpa     Updated 7500 metacomments.
06/14/06   etv     Added AT Gaup Indications for HDRIDLE.
07/12/06   mpa     Added sys_lost_reason for NETWORK_LOST ind.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
05/03/06   ljl     Added support for hybrid access
04/21/06   hal     Added HDRIND_SCP_NO_PREV_SESSION indication
                   Added SESSION_CHANGED reasons to the HDRIND_SCM_SESSION_CHANGED 
                   indication
04/10/06   yll     Added support for Minimal UATI Mode.
04/06/06   kss     Added HDRIND_BCMCS_SLEEP_OK ind.
03/22/06   pba     Added new indications for AN GAUP fail.
02/23/06   hal     Added new indication to support AN GAUP of AMP attribs
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
12/13/05   pba     Obsoleted SCP_RECONFIGURED indication
12/06/05   etv     Added new indications for AT-init GAUP and Neg support for 
                   AMAC and Generic Multi-mode Capab Disc protocol.
11/15/05   mpa     Added HDRIND_IDLE_MONITOR_CONNECTION_FAILED ind.
11/04/05   etv     Added HDRIND_RMAC_FLOW_<DE>ACTIVATED and FLOW_MAPPING_CHANGED
10/28/05   mpa     Added HDRIND_ALMP_CONNECTION_CLOSED ind.
10/04/05   pba     Obsolete HDRIND_SCP_PREV_SESSION_OPENED
09/30/05   mpa     Obsolete HDRIND_OVHD_BC_UPDATED.
09/19/05   mpa     Obsolete HDRIND_BCMCS_FREQ_CHANGE_INITIATED.
09/07/05   etv     Added support for AT-init GAUP for RMAC ST-1.
08/22/05   etv     Added support for AN Init GAUP for FMAC and SCM support.
08/22/05   mpa     Added hdrovhd_bc_updated indication for BCMCS
08/07/05   mpa     Added AT Initiated GAUP indications for MRLP.
06/21/05   etv     Added ccmac_interrupted indication to the enum.
11/29/04   grl     Added traffic suspend and resume indicators.
11/09/04   dna     Added HDRIND_IDLE_CONNECTION_ABORTED to allow 
                   the connection setup to be aborted.
11/08/04   pba     Added HDRIND_RUP_EXIT_HDR_MODE,HDRIND_RUP_CHANGE_HDR_SYSTEMS
11/04/04   mpa     Added HDRIND_SCM_RMAC_GAUP, HDRIND_SCM_MRLP_GAUP.
11/01/04   mpa     Added HDRIND_BCMCS_FREQ_CHANGE_INITIATED.
09/09/04   mpa     Added HDRIND_PROT_NEG_FAILED, HDRIND_CONFIG_COMPLETE,
                   HDRIND_CONFIG_REQ (generic AT init negotiation)
08/26/04   mpa     Added HDRIND_AMP_FAILED_NEW_ESN.
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
02/23/04   mpa     Added access_inhibited to ind data union
11/10/02    km     Added HDRIND_LMAC_HDR_USABLE support.
07/21/03   mpa     Merged in 6500 baseline
06/26/03   mpa     Added idle_is_conn_deny to ind data union
05/15/03   mpa     Added HDRIND_LUP_ALN_TRIGGER
03/12/03   mpa     Added close_reason and removed prot_failed_attrib id from
                   ind data union (superseded by scp_fail_ptr)
01/30/03   mpa     Added HDRIND_LMAC_RESELECT
01/29/03   kss     Add HDRIND_LMAC_CC_MISSED indication.
12/10/02   dna     Added support for HDRIDLES_SUSPEND_STATE
08/21/02   mpa     Added HDRIND_LMAC_ACCESS_ENTRY_HANDOFF
03/29/02   dna     Added HDRIND_CCMAC_SLEEP_CAPSULE_DONE
02/13/02   vas     Added HDRIND_FCP_ALL_STREAMS_CLOSED
11/30/01   om      Added security layer support
09/04/01   dna     Added HDRIND_RUP_CONNECTION_INITIATED based on DDR-46
                   to reset SLP before reliable RTCACK message comes.
                   Rename DRC supervision indications.
08/17/01   vas     Added HDRIND_FCP_DATA_READY_RCVD. Added stream_id to
                   ind data union
07/10/01   dna     added payload for idle_connection_closed indication
                   other code cleanup changes
04/02/01   nmn     removed structure that is not needed and added comment for
                   scp_fail_ptr
03/29/01   dna     New indications for LUP
03/19/01   dna     Added support for IS-856 compliant session layer indication
                   data
03/10/01   sh      Added Idle and Session related Indications and
                   attribute_id to indication data
02/27/01   vas     Added "ovhd_updated_is_first_after_acq" to ind data union.
02/07/01   dna     Added ALMP_RETURNING_TO_IDLE indication
09/07/00   dna     link_id is passed with some indications.
05/01/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrsrchtypes.h"
#include "hdrhai.h"
#include "hdrmac.h"
#include "hdrcom_api.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/




/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION HDRIND_GIVE_IND

DESCRIPTION
  This function gives the specified indication to all the protocols that
  are registered to receive it.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Name of the indication to be broadcast
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrind_give_ind
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIND_GET_IND_DATA_SIZE

DESCRIPTION
  This function returns the data size of the specificed indication name

DEPENDENCIES
  None

PARAMETERS
  ind_name - indication name

RETURN VALUE
  Size of the indication data

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 hdrind_get_ind_data_size
(
  hdrind_ind_name_enum_type ind_name
);

#endif /* HDRIND_H */
