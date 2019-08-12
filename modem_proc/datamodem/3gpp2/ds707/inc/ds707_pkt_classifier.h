#ifndef DS707_PKT_CLASSIFIER_H
#define DS707_PKT_CLASSIFIER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

GENERAL DESCRIPTION
  Pakcet classifier for on target verfication without actual signal

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2004-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pkt_classifier.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/25/14   hr      Initial version 

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "ps_ppp.h"
#include "cm.h"
#include "dsm.h"
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_ppp_sig.h"
#include "ds707_pdn_context.h"
#include "ds707_data_session_profile.h"

#ifdef FEATURE_DATA_PKT_CLASSIFIER
/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_SET_AN_PPP_CONFIG

DESCRIPTION
  This function stores the AN PPP configurations
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_set_an_ppp_config
(
  ppp_dev_opts_type *ppp_config_info
);

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_ROUTE_AN_TX_UM_DATA

DESCRIPTION
  This function parses the AN tx data packet and routes it.
  LCP packets are reverted back to AN.
  IPCP packets (1X/HRPD) are routed to SN.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_route_an_tx_um_data
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_ROUTE_UM_TX_SN_DATA

DESCRIPTION
  This function parses the UM tx data packet, modifies it and routes it.
  LCP packets are reverted back to SN.
  IPCP packets (1X/HRPD) are routed to AN.
  VSNCP packets (eHRPD) are modified and reverted back to SN.
  Ping packets are modified and reverted back to SN.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_route_um_tx_sn_data
(
  dsm_watermark_type *wm_ptr
);

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_CM_CALL_CONNECTED

DESCRIPTION
  Simulate call connected from CM
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_cm_call_connected
(
  cm_client_id_type  ds3g_cm_client_id,
  cm_call_id_type    call_id
);

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_IS_EHRPD_MODE

DESCRIPTION
  Return if packet classifier is being tested under eHRPD mode
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if eHRPD mode is being tested
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_classifier_is_ehrpd_mode
(
  void 
);

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_INIT

DESCRIPTION
  This function is called during DS task boot up to read NVs to determine
  if packet classifier testing is enabled, and the RAT being tested on
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_init
(
  void
);
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
#endif/* DS707_PKT_CLASSIFIER_H */
