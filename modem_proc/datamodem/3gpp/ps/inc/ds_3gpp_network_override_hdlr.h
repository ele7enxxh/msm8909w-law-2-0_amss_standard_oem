#ifndef DS3GPPNETWORKOVERRIDEHDLR_H
#define DS3GPPNETWORKOVERRIDEHDLR_H
/*!
  @file
  ds_3gpp_network_override_hdlr.h

  @brief

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_network_override_hdlr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/12/13   vb     Handling of network overriden ESM causecodes

===============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_apn_table.h"
#include "ds3gsubsmgr.h"

/*---------------------------------------------------------------------------
Enum's to contain the overridden network ip type
-----------------------------------------------------------------------------*/

typedef enum
{
  PDP_UNDEFINED,
  PDP_IPV4,
  PDP_IPV6,
}ds_network_overrridden_ip_type_e;

/*---------------------------------------------------------------------------
dynamic pointer structure containing apn name and the corresponding ip type 
that was overridden by the network
-----------------------------------------------------------------------------*/
typedef struct
{

  ds_network_overrridden_ip_type_e ntw_overridden_ip;
  byte                             apn[DS_UMTS_MAX_APN_STRING_LEN+1];  /* APN string */

}ds_network_overridden_ip_apn_dyn_s;

/*-----------------------------------------------------------------------------
Network overridden ip structure
-------------------------------------------------------------------------------*/
typedef struct
{
  ds_network_overridden_ip_apn_dyn_s *ds_network_overridden_ip_apn_dyn_p;
}ds_network_overridden_ip_apn_s;

extern ds_network_overridden_ip_apn_s ntw_overridden_ip_apn[DS_MAX_APN_NUM];

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_RESET

DESCRIPTION
  This function initializes the data structure to NULL

PARAMETERS
   subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_reset
(
  sys_modem_as_id_e_type subs_id
);

/*=========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_GET_OVERRIDDEN_IP

DESCRIPTION
  This function checks if the ip entry corresponding to the particular apn
  is overridden by the network and returns it to the client

PARAMETERS
   apn     - APN Name
   subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  ds_network_overrridden_ip_type_e 

SIDE EFFECTS
  None

===========================================================================*/

ds_network_overrridden_ip_type_e ds_3gpp_network_override_hdlr_get_overridden_ip
(
   byte                   *apn,
   sys_modem_as_id_e_type subs_id
);

/*=========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_SET_OVERRIDDEN_IP

DESCRIPTION
  This function sets the passed IP type based on the apn entry passed.
  If the apn entry does not exist, it would create a new entry

PARAMETERS
   byte                              *apn
   ds_network_overrridden_ip_type_e   ip_type
   sys_modem_as_id_e_type             subs_id

DEPENDENCIES
  None.

RETURN VALUE
  boolean 

SIDE EFFECTS
  None

===========================================================================*/

boolean ds_3gpp_network_override_hdlr_set_overridden_ip
(
   byte                              *apn,
   ds_network_overrridden_ip_type_e  overridden_ip,
   sys_modem_as_id_e_type            subs_id
);

/*=========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_INIT

DESCRIPTION
  This function initializes the Network Override handler.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean 

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_init
(
  void
);

/*=========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_PER_SUBS_INIT

DESCRIPTION
  This function initializes the Subscription Specific information of
  Network Override handler.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean 

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_per_subs_init
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_GET_APN_TBL_HNDL

DESCRIPTION
  This function fetches the handle to the Network Override Handler APN Table.

PARAMETERS
   subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
ds_network_overridden_ip_apn_s* 
         ds_network_override_hdlr_get_apn_tbl_hndl
(
  sys_modem_as_id_e_type subs_id
);

#endif
