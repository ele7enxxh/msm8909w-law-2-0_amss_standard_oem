/*!
  @file
  ds_3gpp_network_override_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_network_override_hdlr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/13   vb      Handling of network overriden ESM causecodes

===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_3gpp_network_override_hdlr.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef struct
{
  ds_network_overridden_ip_apn_s ntw_overridden_ip_apn[DS_MAX_APN_NUM];
} ds_network_override_hdlr_per_subs_info_type;

static ds_network_override_hdlr_per_subs_info_type 
         *ds_network_override_hdlr_per_subs_info_p[DS3GSUBSMGR_SUBS_ID_MAX] = {NULL};

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_GET_SUBS_INFO_HNDL

DESCRIPTION
  This function fetches the handle to the subscription specific information
  of this module.

PARAMETERS
   subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static ds_network_override_hdlr_per_subs_info_type* 
         ds_network_override_hdlr_get_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_SET_SUBS_INFO_HNDL

DESCRIPTION
  This function sets the handle to the subscription specific information
  of this module.

PARAMETERS 
   hndl    - Handle to the subscription specific information
   subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds_network_override_hdlr_set_subs_info_hndl
(
  ds_network_override_hdlr_per_subs_info_type* hndl,
  sys_modem_as_id_e_type                       subs_id
);

ds_network_overridden_ip_apn_s ntw_overridden_ip_apn[DS_MAX_APN_NUM] = {{NULL}};

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
)
{
  uint8                           apn_loop_index = 0;
  ds_network_overridden_ip_apn_s* ntw_overridden_ip_apn = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ntw_overridden_ip_apn = ds_network_override_hdlr_get_apn_tbl_hndl(subs_id);

  if (ntw_overridden_ip_apn == NULL)
  {
    DS_3GPP_MSG1_ERROR("apn_hndl is NULL for subs id: %d", subs_id);
    return;
  }

/*------------------------------------------------------------------------- 
  Initialize the structure to NULL
 -------------------------------------------------------------------------*/
  for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {
    if(ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p
                          != NULL)
    {
      modem_mem_free(ntw_overridden_ip_apn[apn_loop_index].
                      ds_network_overridden_ip_apn_dyn_p,MODEM_MEM_CLIENT_DATA);

      ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p
                          = NULL;
    }
  }
} /*ds_3gpp_network_override_hdlr_reset*/

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
)
{
  ds_network_overrridden_ip_type_e ret_val = PDP_UNDEFINED;
  uint8                            apn_loop_index = 0;
  ds_network_overridden_ip_apn_s*  ntw_overridden_ip_apn = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ntw_overridden_ip_apn = 
    ds_network_override_hdlr_get_apn_tbl_hndl(subs_id);

  if (ntw_overridden_ip_apn == NULL)
  {
    DS_3GPP_MSG1_ERROR("apn_hndl is NULL for subs id: %d", subs_id);
    return ret_val;
  }

  if(apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("APN ptr is NULL");
    return ret_val;
  }

 /*--------------------------------------------------------------------- 
   Find the match corresponding to the passed apn and return the ip type
 ---------------------------------------------------------------------*/
  for(apn_loop_index = 0;apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {
    if(ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p 
                         != NULL)
    {
      if(strncasecmp((char*)ntw_overridden_ip_apn[apn_loop_index].
                      ds_network_overridden_ip_apn_dyn_p->apn,(char *)apn,
                      DS_UMTS_MAX_APN_STRING_LEN+1) == 0)
      {
        DS_3GPP_MSG1_MED("Match occured for apn:%s",apn);
        ret_val = ntw_overridden_ip_apn[apn_loop_index].
                          ds_network_overridden_ip_apn_dyn_p->ntw_overridden_ip;

        break;
      }
    }
  }

  DS_3GPP_MSG1_HIGH("Overridden IP type returned is: %d",ret_val);
  return ret_val;
} /*ds_3gpp_network_override_hdlr_get_overridden_ip*/

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
)
{
  boolean                          entry_updated = FALSE;
  uint8                            apn_loop_index = 0;
  ds_network_overridden_ip_apn_s*  ntw_overridden_ip_apn = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ntw_overridden_ip_apn = 
    ds_network_override_hdlr_get_apn_tbl_hndl(subs_id);

  if (ntw_overridden_ip_apn == NULL)
  {
    DS_3GPP_MSG1_ERROR("apn_hndl is NULL for subs id: %d", subs_id);
    return entry_updated;
  }

  if(apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("APN ptr is NULL");
    return entry_updated;
  }
  /*--------------------------------------------------------------------- 
          Find the match corresponding to the passed apn, If no match occurs
          create a new entry. Ideally it must always be a fresh entry
          Currently print an error msg if a match occurs.
  ---------------------------------------------------------------------*/
  for(apn_loop_index = 0;apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
  {
    if(ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p 
                                                                        != NULL)
    {
      if(strncasecmp((char*)ntw_overridden_ip_apn[apn_loop_index].
          ds_network_overridden_ip_apn_dyn_p->apn,(char *)apn,
                               DS_UMTS_MAX_APN_STRING_LEN+1) == 0)
      {
        DS_3GPP_MSG2_ERROR("Match occured for apn:%s, ntw_overridden_ip is %d",
                           apn,
                           ntw_overridden_ip_apn[apn_loop_index].
                           ds_network_overridden_ip_apn_dyn_p->ntw_overridden_ip);

   /*----------------------------------------------------------------------- 
     If overridden IP type is undefined, clean up the entry and free memory
     else, store the overridden ip type
   -----------------------------------------------------------------------*/
        if(overridden_ip == PDP_UNDEFINED)
        {
          modem_mem_free(ntw_overridden_ip_apn[apn_loop_index].
                      ds_network_overridden_ip_apn_dyn_p,MODEM_MEM_CLIENT_DATA);
                      ntw_overridden_ip_apn[apn_loop_index].
                      ds_network_overridden_ip_apn_dyn_p = NULL;
        }
        else
        {
          ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p
                                       ->ntw_overridden_ip = overridden_ip;
        }

        entry_updated = TRUE;
        break;
      }
    }
  }

  if((!entry_updated) && (overridden_ip != PDP_UNDEFINED))
  {
    DS_3GPP_MSG0_MED("Entry does not exist, Creating a new entry");
    for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM ;apn_loop_index++)
    {
      if(ntw_overridden_ip_apn[apn_loop_index].
                                 ds_network_overridden_ip_apn_dyn_p == NULL)
      {
        ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p 
                                      = (ds_network_overridden_ip_apn_dyn_s *)
        modem_mem_alloc(sizeof(ds_network_overridden_ip_apn_dyn_s),MODEM_MEM_CLIENT_DATA);

        if(ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p == NULL)
        {
          DS_3GPP_MSG0_ERROR("Heap allocation failure");
          return entry_updated;
        }

        (void)strlcpy((char*)ntw_overridden_ip_apn[apn_loop_index].
                       ds_network_overridden_ip_apn_dyn_p->apn,
                      (char*)apn,DS_UMTS_MAX_APN_STRING_LEN+1);

        ntw_overridden_ip_apn[apn_loop_index].ds_network_overridden_ip_apn_dyn_p->
                                           ntw_overridden_ip = overridden_ip;

        entry_updated = TRUE;
        DS_3GPP_MSG1_MED("Overridden apn entry created at index:%d",apn_loop_index);
        break;
      }
    }
  }

  if(apn_loop_index == DS_MAX_APN_NUM)
  {
    DS_3GPP_MSG0_ERROR("Set override ip failed, MAx entries filled");
  }

  return entry_updated;
}/* ds_3gpp_network_override_hdlr_set_overridden_ip*/

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_GET_SUBS_INFO_HNDL

DESCRIPTION
  This function fetches the handle to the subscription specific information
  of this module.

PARAMETERS
   subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static ds_network_override_hdlr_per_subs_info_type* 
         ds_network_override_hdlr_get_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_network_override_hdlr_per_subs_info_type *hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    hndl = ds_network_override_hdlr_per_subs_info_p[subs_id];
  }

  return hndl;
} /* ds_network_override_hdlr_get_subs_info_hndl */

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_SET_SUBS_INFO_HNDL

DESCRIPTION
  This function sets the handle to the subscription specific information
  of this module.

PARAMETERS 
   hndl    - Handle to the subscription specific information
   subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds_network_override_hdlr_set_subs_info_hndl
(
  ds_network_override_hdlr_per_subs_info_type* hndl,
  sys_modem_as_id_e_type                       subs_id
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    ds_network_override_hdlr_per_subs_info_p[subs_id] = hndl;
  }

  return;
} /* ds_network_override_hdlr_set_subs_info_hdl */

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
ds_network_overridden_ip_apn_s* ds_network_override_hdlr_get_apn_tbl_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_network_override_hdlr_per_subs_info_type* hndl = NULL;
  ds_network_overridden_ip_apn_s*              apn_hndl = NULL;          
  /*---------------------------------------------------------------------*/

  hndl = ds_network_override_hdlr_get_subs_info_hndl(subs_id);

  if (hndl != NULL)
  {
    apn_hndl = hndl->ntw_overridden_ip_apn;
  }

  return apn_hndl;
} /* ds_network_override_hdlr_get_apn_tbl_hndl */

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
)
{
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id();
  /*--------------------------------------------------------------------*/

  /*======================================================================
     During powerup, we allocate Network Override Handler structure only for
     DDS.
   
     If Dual SIM / Triple SIM Device mode event is received at
     a later stage, we will allocate the structure for other
     subscriptions. 
  ======================================================================*/

  ds_3gpp_network_override_hdlr_per_subs_init(subs_id);
} /* ds_3gpp_network_override_hdlr_init */

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
)
{

  ds_network_override_hdlr_per_subs_info_type* per_subs_info_p = NULL;
  /*-----------------------------------------------------------------------*/

  per_subs_info_p = ds_network_override_hdlr_get_subs_info_hndl(subs_id);

  do
  {
    if (per_subs_info_p != NULL)
    {
      DS_3GPP_MSG1_ERROR ("Memory already allocated for subs id: %d", subs_id );
      break;
    }
    else    
    {
      per_subs_info_p = 
        (ds_network_override_hdlr_per_subs_info_type*)
          modem_mem_alloc (sizeof(ds_network_override_hdlr_per_subs_info_type),
                           MODEM_MEM_CLIENT_DATA);

      if (per_subs_info_p == NULL)
      {
        DS_3GPP_MSG1_ERROR ("Memory allocation failed for subs id: %d", 
                            subs_id );
        break;
      }

      memset(per_subs_info_p->ntw_overridden_ip_apn, 0, 
             sizeof(ds_network_overridden_ip_apn_s)*DS_MAX_APN_NUM);

      ds_network_override_hdlr_set_subs_info_hndl(per_subs_info_p, subs_id);
    }
  }
  while (0); 
} /* ds_3gpp_network_override_hdlr_init */

#ifdef TEST_FRAMEWORK
#error code not present
#endif