
/*!
  @file
  ds_eps_stats.h

  @brief
  Lower layer/Bearer Context interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_stats.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/15   ms      create this file for modem power statistics

===========================================================================*/

#ifndef DS_EPS_STATS_H
#define DS_EPS_STATS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_LTE

#include "ds_3gpp_bearer_context.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_EPS_CONFIG_MODEM_POWER_STATS

DESCRIPTION
  This function informs lte ml1 to enable/disable modem power stats
 
PARAMETERS
  boolean - enable_modem_power_stats 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_eps_config_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  boolean enable_modem_power_stats
);

/*===========================================================================
FUNCTION DS_EPS_REQUEST_MODEM_POWER_STATS

DESCRIPTION
  This function requests lte ml1 to send modem power stats
 
PARAMETERS
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_eps_request_modem_power_stats
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================
FUNCTION DS_EPS_GET_MODEM_POWER_STATS

DESCRIPTION
  This function returns cached tx modem power stats
 
PARAMETERS
  boolean - enable_modem_power_stats 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_eps_get_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX] 
);

/*===========================================================================
FUNCTION DS_EPS_STATS_MODEM_POWER_INFO_CNF_HDLR

DESCRIPTION
  This function is used to receive LTE ML1 power statistics information and
  cache it loacally for ds3g to query

PARAMETERS
  msgrtype  - UMID of the message sent
  dsmsg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if the message header is correct and we can handle the message
  FALSE - otherwise

  Note: We will not return FALSE for errors in the message payload

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_stats_modem_power_info_cnf_hdlr
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type  *dsmsg_ptr
);

#endif /* FEATURE_DATA_LTE */
#endif /* DS_EPS_STATS_H */

