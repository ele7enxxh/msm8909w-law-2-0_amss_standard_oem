/*!
  @file
  ds_pdn_limit_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_roaming_hdlr.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
05/15/13     vs      Initial File
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "ds_3gpp_roaming_hdlr.h"
#include "dsumtspdpregint.h"
#include "list.h"
#include "modem_mem.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_hdlr.h"
#include "ps_sys_conf.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_plmn_hdlr.h"
#include "ds_3gpp_device_ev_hdlr.h"
#include "ds_3gpp_nv_manager.h"
#include "ds3gdsdif.h"
#include "ds3g_xml_parser.h"
#include "dsutil.h"
#include "ds3gmmgsdiif.h"

#ifdef FEATURE_DATA_LTE
#include "ds_eps_kamgr_ext.h"
#endif /*FEATURE_DATA_LTE*/
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------- 
  Maximum value of PDN Disconnect Wait Time. If PDN Disconnection Wait Time is
  configured to be 0xFF, it is the client app's responsibility to deregister with
  the network (which would inturn trigger PDN Disconnect Request).
 -----------------------------------------------------------------------------*/
#define DS_3GPP_ROAMING_MAX_PDN_DISCON_WAIT_TIME 0xFF

/*---------------------------------------------------------------------------- 
  This structure defines an item in the PDN Disconnect Wait Timer List.
-----------------------------------------------------------------------------*/
typedef struct
{
  list_link_type     link;         /* Pointer to the next item in list */
  uint8              profile_num;  /* Profile number for which the wait timer
                                      was running */
  rex_timer_type    *timer_ptr;    /* PDN Disconnect Wait Timer */
}ds_3gpp_roaming_wait_timer_item_type;

/*---------------------------------------------------------------------------- 
  This structure defines an item in the Home PLMN List
-----------------------------------------------------------------------------*/
typedef struct
{
  list_link_type     link;     /* Pointer to the next item in list */
  sys_plmn_id_s_type plmn_id;  /* Home PLMN Id*/
} ds_3gpp_roaming_home_plmn_list_item_type;

/*---------------------------------------------------------------------------- 
  This structure defines all the information related to the Roaming Handler
  Module.
-----------------------------------------------------------------------------*/
typedef struct
{
  struct subs_index_s
  {
    boolean    roaming_status;  /* Whether UE is in Roaming or not */
    list_type  home_plmn_list;  /* List of Home PLMNs */
    list_type  excp_home_plmn_list; /* List of exception Home PLMNs,
                                      higher priority than Home PLMN list*/
  } subs_index[DS3GSUBSMGR_SUBS_ID_MAX]; 

  list_type    wait_timer_list; /* PDN Disconnect Wait Timer List */

} ds_3gpp_roaming_info_type;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*---------------------------------------------------------------------------- 
  This variable declares all the information related to the Roaming Handler
  Module.
-----------------------------------------------------------------------------*/
static ds_3gpp_roaming_info_type ds_3gpp_roaming_info = {{{NULL}}};

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
  
/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_BLOCK_APN_PER_PROFILE

  DESCRIPTION
  This function blocks given APN corresponding to given profile.
  
  PARAMETERS  
  Subscription id
  Profile Information Ptr
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_block_apn_per_profile
(
  sys_modem_as_id_e_type     subs_id,
  ds_umts_pdp_profile_type   *profile_info_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_BLOCK_APNS

  DESCRIPTION
  This function blocks all APNs which are disallowed during Roaming.
  
  PARAMETERS  
  Subscription id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_block_apns
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UNBLOCK_APN_PER_PROFILE

  DESCRIPTION
  This function unblocks APN corresponding to the given profile.
  
  PARAMETERS  
  Subscription id
  Profile Information Ptr
  Boolean Pointer set to True if the attach apn is unblocked
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_unblock_apn_per_profile
(
  sys_modem_as_id_e_type    subs_id,
  ds_umts_pdp_profile_type  *profile_info_ptr,
  boolean*                  is_attach_apn_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UNBLOCK_APNS

  DESCRIPTION
  This function unblocks all APNs which were disallowed during Roaming.
  
  PARAMETERS  
  Subscription id
  Boolean Pointer set to True if the attach apn is unblocked
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_unblock_apns
(
  sys_modem_as_id_e_type subs_id,
  boolean*               is_attach_apn_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ADVERTISE_THROT_INFO

  DESCRIPTION
  This function advertises Throttling Information to all clients and
  disables lte if needed
  
  PARAMETERS
  1. disable_lte_if_needed: Flag indicating whether LTE needs to be disabled
  2. Subscription id
  3. Boolean set to True if the attach apn is unblocked
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/ 
static void ds_3gpp_roaming_advertise_throt_info
(
  boolean                disable_lte_if_needed,
  sys_modem_as_id_e_type subs_id,
  boolean                is_attach_apn
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ALLOCATE_AND_START_WAIT_TIMER

  DESCRIPTION
  This function tries to allocate and start the wait timer.
  
  PARAMETERS  
  Profile for which the wait timer is configured.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE, if Wait Timer started or no need to Start Wait Timer
  FALSE, if Wait Timer Did not Start
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static boolean ds_3gpp_roaming_allocate_and_start_wait_timer
(
  ds_umts_pdp_profile_type   *profile_info_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_WAIT_TIMER_EXP_CB

  DESCRIPTION
  This function callback is invoked when the PDN Disconnect wait timer
  expires.
  
  PARAMETERS
 
  Callback Data
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_wait_timer_exp_cb
(
  uint32 cb_data
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_IS_PLMN_FOUND_IN_LIST

  DESCRIPTION
  This function checks whether the PLMN can be found in a given PLMN List.
  
  PARAMETERS
  PLMN Id
  PLMN List Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Boolean.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_roaming_is_plmn_found_in_list
(
  sys_plmn_id_s_type          plmn_id,
  list_type                  *list_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_VALIDATE_MCC_MNC

  DESCRIPTION
  This function validates MCC-MNC read from EFS file
  
  PARAMETERS  mcc_mnc - MCC and MNC info in string format "MCC-MNC"
    
  DEPENDENCIES None.
  
  RETURN VALUE TRUE - validation suceeds
               FALSE - validation fails
   
  SIDE EFFECTS None
  
===========================================================================*/
boolean ds_3gpp_roaming_validate_mcc_mnc
(
  char    *mcc_mnc
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CONVERT_MCC_MNC_TO_PLMN_FORMAT

  DESCRIPTION
  This function converts MCC-MNC to PLMN Format
  
  PARAMETERS  mcc_mnc - MCC and MNC info in string format "MCC-MNC"
              plmn_id_buf - buffer contains return value - plmn id
    
  DEPENDENCIES None.
  
  RETURN VALUE TRUE - convertion succeeds
               FALSE - convertion fails
   
  SIDE EFFECTS None
  
===========================================================================*/
boolean ds_3gpp_roaming_convert_mcc_mnc_to_plmn_format
(
  char                  *mcc_mnc,
  sys_plmn_id_s_type    *plmn_id_buf
);

/*===========================================================================
 FUNCTION DS_3GPP_ROAMING_PROCESS_PLMN_LIST_CHG_CMD

  DESCRIPTION
  This function is used to process the change in PLMN List.
  
  PARAMETERS  
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
static void ds_3gpp_roaming_process_plmn_list_chg_cmd
(
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_CURRENT_PLMN_LIST

  DESCRIPTION
  This function gets the current PLMN List. If current PLMN is same as RPLMN
  or part of EPLMN list, copy RPLMN and EPLMN list to current PLMN list.
  Otherwise, copy current PLMN to current PLMN list.
  
  PARAMETERS
  Subscription Id
  New PLMN Id
  Current PLMN List
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.

  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_roaming_get_current_plmn_list
(
  sys_modem_as_id_e_type    subs_id,
  sys_plmn_id_s_type        new_plmn_id,
  sys_plmn_list_s_type     *curr_plmn_list_p
);

/*===========================================================================


                               FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_POWERUP_INIT

  DESCRIPTION
  This function performs Powerup Initialization of the Module
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_hdlr_powerup_init
(
  void
)
{
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  ds_3gpp_roaming_hdlr_powerup_init_per_subs_id(subs_id);
  list_init(&ds_3gpp_roaming_info.wait_timer_list);
}/*ds_3gpp_roaming_hdlr_powerup_init*/

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_READ_EFS_CONFIG_PER_SUBS_ID

  DESCRIPTION This function reads HPLMN list from EFS config file
              per subscription. It reads MCC+MNC from EFS, converts to
              PLMN value and caches PLMN to home_plmn_list and
              excp_home_plmn_list.
  
  PARAMETERS  sys_modem_as_id_e_type  subs_id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_hdlr_read_efs_config_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3g_xml_element             *xml_root = NULL;
  char                         *tag = NULL;
  list_type                     element_list;
  list_type                     plmn_element_list;
  ds3g_xml_list_info_type      *list_entry_ptr = NULL;
  ds3g_xml_list_info_type      *plmn_list_entry_ptr = NULL;
  char                         *plmn_id_str = NULL;
  sys_plmn_id_s_type            plmn_id;
  ds_3gpp_roaming_home_plmn_list_item_type  *list_item_ptr = NULL; 
  uint8                         i = 0;  
  list_type                    *plmn_list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------
  Generate XML root
  ----------------------------------------------------------------------*/
  xml_root = ds3g_xml_parse(subs_id, DS_3GPP_HOME_PLMN_LIST_FILE);

  if (xml_root != NULL )
  {
    memset(&element_list, 0, sizeof(list_type));
    memset(&plmn_element_list, 0, sizeof(plmn_element_list));
    for( i = 0; i < DS_3GPP_PLMN_LIST_NUM; i++ )
    {
      /*----------------------------------------------------------------------
       First loop: read home_plmn_list; second loop: read exception_hplmn_list.
      ----------------------------------------------------------------------*/
      if ( i == 0 )
      {
        tag = DS_3GPP_HOME_PLMN_LIST_TAG;
        plmn_list_ptr = &(ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list);
      }
      else
      {
        tag = DS_3GPP_EXCEPTION_HPLMN_LIST_TAG;
        plmn_list_ptr = &(ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list);
}

      /*----------------------------------------------------------------------
      Based on the tag, get XML element list with the same tag.
      ----------------------------------------------------------------------*/
      if ( ds3g_xml_get_element_by_tag(xml_root, 
                                       tag, 
                                       &element_list ) == TRUE )
      {
        /*----------------------------------------------------------------------
         Get HPLMN_list XML element. 
        ----------------------------------------------------------------------*/
        list_entry_ptr = (ds3g_xml_list_info_type*)
                                       list_peek_front(&element_list);
        while( list_entry_ptr != NULL )
        {
          /*------------------------------------------------------------------
             Get MCC-MNC XML element
          ------------------------------------------------------------------*/
          if (ds3g_xml_get_element_by_tag(list_entry_ptr->element,
                                          DS_3GPP_PLMN_NAME_TAG,
                                          &plmn_element_list) == TRUE )
          {
            plmn_list_entry_ptr = (ds3g_xml_list_info_type*)
                                          list_peek_front(&plmn_element_list);
            while( plmn_list_entry_ptr != NULL )
            {
              /*------------------------------------------------------------------
               Read MCC-MNC from XML file and convert to PLMN id format
              ------------------------------------------------------------------*/
              plmn_id_str = ds3g_xml_get_text_content(
                                          plmn_list_entry_ptr->element);
              if( ds_3gpp_roaming_convert_mcc_mnc_to_plmn_format(plmn_id_str,
                                                                 &plmn_id) == TRUE )
              {
                /*------------------------------------------------------------------
                 Save to Home PLMN list
                ------------------------------------------------------------------*/
                list_item_ptr = modem_mem_alloc(
                                sizeof(ds_3gpp_roaming_home_plmn_list_item_type),
                                MODEM_MEM_CLIENT_DATA );
                if( list_item_ptr == NULL )
                {
                  ASSERT(0);
                }

                memset(list_item_ptr, 0, sizeof(ds_3gpp_roaming_home_plmn_list_item_type));
                memscpy(&(list_item_ptr->plmn_id), sizeof(sys_plmn_id_s_type),
                        &plmn_id, sizeof(sys_plmn_id_s_type));
                list_push_front(plmn_list_ptr,&(list_item_ptr->link));
              }

              /*------------------------------------------------------------------
               Loop next MCC-MNC XML element
              ------------------------------------------------------------------*/
              plmn_list_entry_ptr = (ds3g_xml_list_info_type*)
                                          list_peek_next(&plmn_element_list,
                                                         &(plmn_list_entry_ptr->link));
            }
           /*------------------------------------------------------------------
            Free memory of MCC-MNC XML elements list
           ------------------------------------------------------------------*/
           ds3g_xml_free_elements_list(&plmn_element_list);
         }
         list_entry_ptr = (ds3g_xml_list_info_type*)
                                    list_peek_next(&element_list,
                                                   &(list_entry_ptr->link));
       } 
       /*------------------------------------------------------------------
         Free memory of HPLMN_list XML elements list
       ------------------------------------------------------------------*/
       ds3g_xml_free_elements_list(&element_list);
      }
    }
    /*------------------------------------------------------------------
    Free allocated memory by XML parser for xml_root
    ------------------------------------------------------------------*/
    ds3g_xml_free_element(xml_root);
  }

  /*------------------------------------------------------------------
  EFS verification check: if only exception PLMN list is configured, 
  it is wrong configuration. Free exception PLMN list.
  ------------------------------------------------------------------*/
  if((ds_3gpp_roaming_get_excp_home_plmn_list_size(subs_id) != 0 ) &&
     (ds_3gpp_roaming_get_home_plmn_list_size(subs_id)) == 0)
  {
    DS_3GPP_MSG0_ERROR("EFS PLMN wrong configuration: only exception"
                       " PLMN list is configured");
    plmn_list_ptr = &ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list;
    if( plmn_list_ptr != NULL )
    {
      list_item_ptr = (ds_3gpp_roaming_home_plmn_list_item_type*)
                                             list_pop_front(plmn_list_ptr);
}
    while( list_item_ptr != NULL )
    {
      modem_mem_free(list_item_ptr, MODEM_MEM_CLIENT_DATA);
      list_item_ptr = (ds_3gpp_roaming_home_plmn_list_item_type*)
                                           list_pop_front(plmn_list_ptr);
    }
    list_destroy(&(ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list));
  }

  return;
}/*ds_3gpp_roaming_hdlr_read_efs_config_per_subs_id*/

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_POWERUP_INIT_PER_SUBS_ID

  DESCRIPTION
  This function performs initialization of the roaming module per subscription
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_roaming_hdlr_powerup_init_per_subs_id
(
  sys_modem_as_id_e_type     subs_id 
)
{
  ds_3gpp_roaming_efs_info_s                 efs_info;
  int                                        j = 0;
  ds_3gpp_roaming_home_plmn_list_item_type  *home_plmn_list_item_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  DS_3GPP_MSG1_LOW("3GPP Roaming Hdlr Initialization on subs_id %d",
                   subs_id);


  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  if ((ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list.list_initialized)||
      (ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list.
                                                        list_initialized))
  {
    DS_3GPP_MSG0_MED("DS_3GPP_CFG_HOME_PLMN_LIST is alreaady initialized "
                       "exiting");
    return;
  }

  /*----------------------------------------------------------------------
  Initiate home_plmn_list and exception home_plmn_list
  ----------------------------------------------------------------------*/
  list_init(&(ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list));
  list_init(&(ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list));

  /*----------------------------------------------------------------------
  Read EFS file and populate home_plmn_list and exception home_plmn_list. 
  ----------------------------------------------------------------------*/
  ds_3gpp_roaming_hdlr_read_efs_config_per_subs_id(subs_id);

  /*----------------------------------------------------------------------
  Read NV72561 (modem/data/3gpp/ps/home_plmn_list) and populate 
  home_plmn_list. 
  ----------------------------------------------------------------------*/
  efs_info.num_valid_entries = 0;
  
  memset((void*)efs_info.mcc_mnc_info, 0, 
           sizeof(ds_3gpp_roaming_mcc_mnc_info_s)*
             DS_3GPP_ROAMING_MAX_SUPPORTED_HOME_PLMNS);

  if (ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_HOME_PLMN_LIST,
                      (unsigned char *)(&efs_info),
                      sizeof(efs_info),
                      subs_id) == -1)
  {
    DS_3GPP_MSG0_LOW("Cannot read EFS Config item"
                     "DS_3GPP_CFG_HOME_PLMN_LIST, default beahvior is to not "
                     "throttle on roaming networks");
    return;
  }
  
    for (j=0; j < efs_info.num_valid_entries; j++)
    {
      home_plmn_list_item_ptr = 
        modem_mem_alloc(sizeof(ds_3gpp_roaming_home_plmn_list_item_type),
                        MODEM_MEM_CLIENT_DATA );

      if (home_plmn_list_item_ptr != NULL)
      {
        home_plmn_list_item_ptr->plmn_id =  
          ds_3gpp_roaming_convert_to_plmn_format(efs_info.mcc_mnc_info[j]);

        list_push_front(&(ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list), 
                        &(home_plmn_list_item_ptr->link));
      }
      else
      {
      DS_3GPP_MSG0_LOW("Cannot allocate memory !");
      }
    }
} /* ds_3gpp_roaming_hdlr_powerup_init */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_CLEAR_PER_SUBS_ID

  DESCRIPTION
  This function performs resetting of the roaming module per subscription
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_hdlr_clear_per_subs_id
(
  sys_modem_as_id_e_type     subs_id 
)
{
  list_type                             *list_ptr = NULL;
  ds_3gpp_roaming_home_plmn_list_item_type  *current_home_plmn_list_ptr = NULL;
  ds_3gpp_roaming_home_plmn_list_item_type  *next_home_plmn_list_item_ptr = NULL;  

  DS_3GPP_MSG0_LOW("3GPP Roaming Clearing Per Subsid");

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  /*----------------------------------------------------------------------
  Clear home_plmn_list 
  ----------------------------------------------------------------------*/
  list_ptr = &ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list;

  if(list_ptr != NULL)
  { 
    current_home_plmn_list_ptr = 
      (ds_3gpp_roaming_home_plmn_list_item_type*)list_peek_front( list_ptr );

    while (current_home_plmn_list_ptr != NULL)
    {
      next_home_plmn_list_item_ptr = 
           list_peek_next(list_ptr, &(current_home_plmn_list_ptr->link));

      list_pop_item(list_ptr, &(current_home_plmn_list_ptr->link) ); 
      modem_mem_free(current_home_plmn_list_ptr,MODEM_MEM_CLIENT_DATA);
      current_home_plmn_list_ptr = next_home_plmn_list_item_ptr;

    }
    list_destroy(&ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list);
  }

  /*----------------------------------------------------------------------
  Clear excp_home_plmn_list
  ----------------------------------------------------------------------*/
  list_ptr = &ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list;

  if(list_ptr != NULL)
  {
    current_home_plmn_list_ptr = 
        (ds_3gpp_roaming_home_plmn_list_item_type*)list_peek_front( list_ptr );

    while (current_home_plmn_list_ptr != NULL)
    {
      next_home_plmn_list_item_ptr =
          list_peek_next(list_ptr, &(current_home_plmn_list_ptr->link));

      list_pop_item(list_ptr, &(current_home_plmn_list_ptr->link));
      modem_mem_free(current_home_plmn_list_ptr, MODEM_MEM_CLIENT_DATA);
      current_home_plmn_list_ptr = next_home_plmn_list_item_ptr;
    }
    list_destroy(&(ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list));
  }

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_STATUS

  DESCRIPTION
  This function tells whether the UE is Roaming or not.
  
  PARAMETERS  
  Subscription Id 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE,  if UE is roaming
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_roaming_get_status
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean status = FALSE;
  /*--------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }  

  ds3gpp_enter_global_crit_section();
  status = ds_3gpp_roaming_info.subs_index[subs_id].roaming_status;
  ds3gpp_leave_global_crit_section();

  return status;
} /* ds_3gpp_roaming_get_status */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_SET_STATUS

  DESCRIPTION
  This function sets the roaming status.
  
  PARAMETERS  
  Subscription Id
  Roaming Status
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_set_status
(
  sys_modem_as_id_e_type subs_id,
  boolean                status
)
{
  boolean                               prev_status = FALSE;
  ds3gdevmgr_device_settings_info_type  device_settings_info;
  boolean                               is_attach_apn = FALSE;
  boolean                               is_attach_apn_unblock_roaming = FALSE;
  boolean                               is_attach_apn_data_settings = FALSE;
  sys_sys_mode_e_type                   sys_mode = SYS_SYS_MODE_NONE;
  ds3geventmgr_filter_type              filter_info = {0};
  ds3geventmgr_event_info_type          event_info = {{0}};
  /*--------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  memset(&device_settings_info, 0, 
         sizeof(ds3gdevmgr_device_settings_info_type));

  ds3gpp_enter_global_crit_section();

  prev_status = ds_3gpp_roaming_info.subs_index[subs_id].roaming_status;
  ds_3gpp_roaming_info.subs_index[subs_id].roaming_status = status;

  ds3gpp_leave_global_crit_section();

  DS_3GPP_MSG2_HIGH("Roaming status. Old: %d, New: %d ", 
                    prev_status,
                    status);

  if (status != prev_status)
  {
    if (status == TRUE)
    {
      ds_3gpp_roaming_block_apns(subs_id);
    }
    else
    {
      ds_3gpp_roaming_unblock_apns(subs_id,&is_attach_apn_unblock_roaming);
    }

    if (ds3gdevmgr_get_device_settings(&device_settings_info, subs_id))
    {
      ds3gpp_device_ev_hdlr_process_data_settings(&device_settings_info,
                                                  &is_attach_apn_data_settings);
    } 
   
    is_attach_apn = (is_attach_apn_unblock_roaming || is_attach_apn_data_settings);
    ds_3gpp_roaming_advertise_throt_info(status, subs_id,is_attach_apn);

    DS_3GPP_MSG0_LOW("Roaming Status change, invoking KAMGR");
#ifdef FEATURE_DATA_LTE
    ds_eps_kamgr_handle_roaming_status_chg(subs_id);
#endif /*FEATURE_DATA_LTE*/

    /* Notify clients of roaming status change */
    memset(&filter_info,0,sizeof(ds3geventmgr_filter_type));
    memset(&event_info,0,sizeof(ds3geventmgr_event_info_type));

    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                                        ds3gsubsmgr_subs_id_cm_to_ds(subs_id);
    filter_info.tech = PS_SYS_TECH_ALL;

    event_info.roaming_status_info.is_roaming = status;
    event_info.roaming_status_info.sys_mode 
      = ds3g_get_current_network_mode_ex(subs_id);

    ds3geventmgr_set_event_info(DS3GEVENTMGR_ROAMING_STATUS_EV,
                                (void*)&event_info,
                                &filter_info);

    /*notify DSD of roaming status change for all 3GPP RATs*/
    for (sys_mode = SYS_SYS_MODE_NO_SRV; sys_mode < SYS_SYS_MODE_MAX; sys_mode++)
    {
      if (DS_3GPP_IS_3GPP_CURRENT_MODE(sys_mode)) 
      {
        ds3gdsdif_notify_dsd_roam_status_chg_event(sys_mode, subs_id, status); 
      }     
    }
  }

  ds_3gpp_roaming_advertise_apns_blocked_due_to_roaming(subs_id, FALSE);

} /* ds_3gpp_roaming_set_status */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_VALIDATE_MCC_MNC

  DESCRIPTION
  This function validates MCC-MNC read from EFS file
  
  PARAMETERS  mcc_mnc - MCC and MNC info in string format "MCC-MNC"
    
  DEPENDENCIES None.
  
  RETURN VALUE TRUE - validation suceeds
               FALSE - validation fails
   
  SIDE EFFECTS None
  
===========================================================================*/
boolean ds_3gpp_roaming_validate_mcc_mnc
(
  char      *mcc_mnc
)
{
  int        i;
  /*---------------------------------------------------------------------*/

  if ((mcc_mnc == NULL) || (strlen(mcc_mnc) != DS_3GPP_MCC_MNC_SIZE ))
  {
    DS_3GPP_MSG0_LOW("mcc_mnc passed is NULL or size is wrong ");
    return FALSE;
  }

  /* valid MCC: all three digits: 0-9 */
  for (i = 0; i < 3; i++)
  {
    if (mcc_mnc[i] - '0' < 0 || mcc_mnc[i] - '0' > 9) 
    {
      DS_3GPP_MSG0_LOW("mcc passed is invalid ");
      return FALSE;
    }
  }

  /* valid MNC: 1)*** 2) first two digits: 0-9, third digit: 0-9 or F */
  if (mcc_mnc[4] == '*' && mcc_mnc[5] == '*' && mcc_mnc[6] == '*')
  {
    return TRUE;
  }

  for (i = 4; i < 7; i++)
  {
    if (i == 6 && mcc_mnc[i] == 'F')
    {
      break;
    }
    else if ((mcc_mnc[i] - '0' < 0 || mcc_mnc[i] - '0' > 9))
    {
      DS_3GPP_MSG0_LOW("mnc passed is invalid ");
      return FALSE;
    }
  }

  return TRUE;
}/* ds_3gpp_roaming_validate_mcc_mnc */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CONVERT_TO_PLMN_FORMAT

  DESCRIPTION
  This function converts MCC MNC to PLMN Format
  
  PARAMETERS  
  MCC MNC Info
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PLMN Id
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_id_s_type ds_3gpp_roaming_convert_to_plmn_format
(
  ds_3gpp_roaming_mcc_mnc_info_s info
)
{
  sys_plmn_id_s_type plmn_id;
  /*---------------------------------------------------------------------*/

  /* ----------------------------------------
  ** PLMN octet 1 = MCC digit 2 | MCC digit 1
  ** ---------------------------------------- */
  plmn_id.identity[0] = (byte) ((info.mcc[1] << 4) + info.mcc[0]);

  /* ----------------------------------------
  ** PLMN octet 2 = MNC digit 3 | MCC digit 3
  ** ---------------------------------------- */
   plmn_id.identity[1] = (byte) ((info.mnc[2] << 4) + info.mcc[2]);

  /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   plmn_id.identity[2] = (byte) ((info.mnc[1] << 4) + info.mnc[0]);

   return plmn_id;
} /* ds_3gpp_roaming_convert_to_plmn_format */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CONVERT_MCC_MNC_TO_PLMN_FORMAT

  DESCRIPTION
  This function converts MCC-MNC to PLMN Format
  
  PARAMETERS  mcc_mnc - MCC and MNC info in string format "MCC-MNC"
              plmn_id_buf - buffer contains return value - plmn id
    
  DEPENDENCIES None.
  
  RETURN VALUE TRUE - convertion succeeds
               FALSE - convertion fails
   
  SIDE EFFECTS None
  
===========================================================================*/
boolean ds_3gpp_roaming_convert_mcc_mnc_to_plmn_format
(
  char                  *mcc_mnc,
  sys_plmn_id_s_type    *plmn_id_buf
)
{
  ds_3gpp_roaming_mcc_mnc_info_s     info;
  uint8                              i = 0;
/*------------------------------------------------------------------------*/
  if (plmn_id_buf == NULL)
  {
    DS_3GPP_MSG0_LOW("plmn_id_buf passed is NULL ");
    return FALSE;
  }
   
  if (!ds_3gpp_roaming_validate_mcc_mnc(mcc_mnc))
  {
    return FALSE;   
  }
  /*---------------------------------------------------------------------- 
   Save MCC-MNC string to ds_3gpp_roaming_mcc_mnc_info_s format.
  ---------------------------------------------------------------------*/   
  memset(&info, 0, sizeof(ds_3gpp_roaming_mcc_mnc_info_s));
     
  for( i=0; i<3; i++)
  {
    info.mcc[i] = mcc_mnc[i] - '0';
    /*-------------------------------------------------------------------- 
     If MNC digit is '*', save as 0xE which means any.
     If MNC digit is 'F', save as 0xF which means invalid.
    ---------------------------------------------------------------------*/ 
    if( mcc_mnc[i+4] == '*')
    {
      info.mnc[i] = 0xE;
    }
    else if( mcc_mnc[i+4] == 'F')
    {
      info.mnc[i] = 0xF;
    }
    else
    {
      info.mnc[i] = mcc_mnc[i+4] - '0';
    }
  }
  
  DATA_3GPP_MSG6(MSG_LEGACY_HIGH,
                 "MCC %x%x%x, MNC %x%x%x",
                 info.mcc[0], info.mcc[1], info.mcc[2],
                 info.mnc[0], info.mnc[1], info.mnc[2]);

  /*---------------------------------------------------------------------- 
   Translate PLMN in ds_3gpp_roaming_mcc_mnc_info_s format to
   sys_plmn_id_s_type format.
  ---------------------------------------------------------------------*/ 
  *plmn_id_buf = ds_3gpp_roaming_convert_to_plmn_format(info);

  return TRUE;
}/*ds_3gpp_roaming_convert_mcc_mnc_to_plmn_format*/

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_IS_PLMN_MATCH

  DESCRIPTION  Compare two PLMN IDs and determines if they are equal.
               The PLMN IDs may contain wildcard MNC FFF.
  
  PARAMETERS   plmn_1                  The first PLMN ID.
               plmn_2                  The second PLMN ID.
    
  DEPENDENCIES None.
  
  RETURN VALUE TRUE - PLMN matches
               FALSE - PLMN does not match
   
  SIDE EFFECTS None.
  
===========================================================================*/
boolean ds_3gpp_roaming_is_plmn_match
(
  sys_plmn_id_s_type  plmn_1,
  sys_plmn_id_s_type  plmn_2
)
{
  uint32     plmn1_mcc_digit_1 = plmn_1.identity[0] & 0x0F;
  uint32     plmn1_mcc_digit_2 = plmn_1.identity[0] / 0x10;
  uint32     plmn1_mcc_digit_3 = plmn_1.identity[1] & 0x0F;
  uint32     plmn1_mnc_digit_1 = plmn_1.identity[2] & 0x0F;
  uint32     plmn1_mnc_digit_2 = plmn_1.identity[2] / 0x10;
  uint32     plmn1_mnc_digit_3 = plmn_1.identity[1] / 0x10;

  uint32     plmn2_mcc_digit_1 = plmn_2.identity[0] & 0x0F;
  uint32     plmn2_mcc_digit_2 = plmn_2.identity[0] / 0x10;
  uint32     plmn2_mcc_digit_3 = plmn_2.identity[1] & 0x0F;
  uint32     plmn2_mnc_digit_1 = plmn_2.identity[2] & 0x0F;
  uint32     plmn2_mnc_digit_2 = plmn_2.identity[2] / 0x10;
  uint32     plmn2_mnc_digit_3 = plmn_2.identity[1] / 0x10;
  boolean    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( (plmn1_mcc_digit_1 == plmn2_mcc_digit_1) &&
       (plmn1_mcc_digit_2 == plmn2_mcc_digit_2) &&
       (plmn1_mcc_digit_3 == plmn2_mcc_digit_3) )
  {
    /*---------------------------------------------------------------------- 
     If one PLMN id has MNC - EEE which means it is matching with any MNC.
    ---------------------------------------------------------------------*/ 
    if( ((plmn1_mnc_digit_1 == 0x0E) && (plmn1_mnc_digit_2 == 0x0E)
        &&(plmn1_mnc_digit_3 == 0x0E)) || ((plmn2_mnc_digit_1 == 0x0E) &&
        (plmn2_mnc_digit_2 == 0x0E) &&(plmn2_mnc_digit_3 == 0x0E)) )
    {
      ret_val = TRUE;
    }
    else if( (plmn1_mnc_digit_1 == plmn2_mnc_digit_1) &&
             (plmn1_mnc_digit_2 == plmn2_mnc_digit_2) &&
             (plmn1_mnc_digit_3 == plmn2_mnc_digit_3))
    {
      ret_val = TRUE;
    }
  }

  DATA_3GPP_MSG7(MSG_LEGACY_HIGH,
                 "PLMN1 %d, %d, %d, PLMN2 %d, %d, %d, match? %d",
                 plmn_1.identity[0], plmn_1.identity[1], plmn_1.identity[2],
                 plmn_2.identity[0], plmn_2.identity[1], plmn_2.identity[2],
                 ret_val);

  return ret_val;
}/*ds_3gpp_roaming_is_plmn_match*/

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_INFORM_NEW_PLMN_INFO

  DESCRIPTION
  This function is used to notify the Roaming Module of PLMN change.
  
  PARAMETERS  
  Subscription Id
  New PLMN Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_inform_new_plmn_info
(
  sys_modem_as_id_e_type subs_id,
  sys_plmn_id_s_type     new_plmn_id  
)
{
  list_type                                  *list_ptr = NULL;
  boolean                                     match_excp_found = FALSE;
  boolean                                     roaming_status = TRUE;
  boolean                                     check_ehplmn_list = FALSE;
  mmgsdi_session_id_type                      session_id;
  sys_plmn_list_s_type                        curr_plmn_list;
  int                                         i;  
  /*--------------------------------------------------------------------*/

  DS_3GPP_MSG1_HIGH("Processing new PLMN Info on subs id %d", subs_id );

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  /*---------------------------------------------------------------------- 
    If neither of the following configures is configured,
    UE listens to CM roaming indication.
    1. New extended EFS file - /data/3gpp/home_plmn_list.xml
    2. NV72561.
    3. NV73590.
  --------------------------------------------------------------------*/
  check_ehplmn_list = ds_3gpp_nv_manager_get_check_ehplmn_list(subs_id);


  /*-------------------------------------------------------------------- 
    If EHPLMN list NV is enabled and SIM is not ready return from this
    point.
    Roaming/Home PLMN decision will be made when SIM is ready
   --------------------------------------------------------------------*/
  if((check_ehplmn_list) &&
     (ds3g_mmgsdi_get_session_id_from_sub_id(subs_id,&session_id) == FALSE))
  {
    DS_3GPP_MSG0_HIGH("Subscription is not ready and check_ehplmn is TRUE, "
                      "return"); 
    return;
  }


  if (ds_3gpp_roaming_get_home_plmn_list_size(subs_id) == 0 &&
      check_ehplmn_list == FALSE)
  {
    DS_3GPP_MSG0_HIGH("Home PLMN List was not configured, ignore. ");
    return;
  }

  memset(&curr_plmn_list, 0 ,sizeof(sys_plmn_list_s_type));
  ds_3gpp_roaming_get_current_plmn_list(subs_id, new_plmn_id, &curr_plmn_list);  
  
  for (i = 0; i < curr_plmn_list.length; i++)
  {
    match_excp_found = FALSE;

    /* check if any PLMN from current PLMN List is in EHPLMN List */
    if (check_ehplmn_list)
    {
      if (ds_3gpp_is_plmn_in_ehplmn_list(curr_plmn_list.plmn[i], subs_id))
      {
        roaming_status = FALSE;
        break;
      }
    }

    /*-------------------------------------------------------------------- 
      Check if PLMN is in excp_home_plmn_list, if yes, continue to check next                                                                                                .
      PLMN in current PLMN list.
    --------------------------------------------------------------------*/
    list_ptr = &ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list;
    match_excp_found = 
          ds_3gpp_roaming_is_plmn_found_in_list(curr_plmn_list.plmn[i], list_ptr);

    /*-------------------------------------------------------------------- 
      PLMN is not in excp_home_plmn_list, check if PLMN is in home_plmn_list.
    --------------------------------------------------------------------*/
    if (match_excp_found == FALSE)
    {
      /*-------------------------------------------------------------------- 
        Check if PLMN is in home_plmn_list.
      --------------------------------------------------------------------*/
      list_ptr = &ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list;
      if(ds_3gpp_roaming_is_plmn_found_in_list(curr_plmn_list.plmn[i], list_ptr))
      {
        roaming_status = FALSE;
        break;
      }
    }
  }

  DS_3GPP_MSG1_HIGH("Roaming Status: %d (1: Roaming, 0: Home)", roaming_status);
  ds_3gpp_roaming_set_status(subs_id, roaming_status);

  return;
} /* ds_3gpp_roaming_inform_new_plmn_info */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UPDATE_PLMN_INFO_ON_SUBS_READY

  DESCRIPTION
  This function is invoked when subscription ready ev is received.
  This function calls roaming_inform_new_plnn_onfo
  which will then take action to perform blocking/unblocking.
  
  PARAMETERS  
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_update_plmn_info_on_subs_ready
(
  sys_modem_as_id_e_type subs_id
)
{
  sys_plmn_id_s_type zero_plmn_id;
  sys_plmn_id_s_type current_plmn_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - -*/

  /*--------------------------------------------------------------------------
    Memset the Zero PLMN Id to 0
   --------------------------------------------------------------------------*/
  memset((void*)&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  ds_3gpp_pdn_cntx_get_current_plmn_ex((&current_plmn_id),
                                        subs_id);

  if(memcmp((void*)&(current_plmn_id),
              (void*)&(zero_plmn_id),
              sizeof(sys_plmn_id_s_type)) != 0)
  {
    DS_3GPP_MSG0_HIGH("Updating PLMN info on subs_ready ev");
    ds_3gpp_roaming_inform_new_plmn_info(subs_id,current_plmn_id);
  }

  return;
}/*ds_3gpp_roaming_update_plmn_info_on_subs_ready*/



/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_FLUSH_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the entire wait timer list.
  
  PARAMETERS  
  None.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_roaming_flush_wait_timer_list
(
  void
)
{
  list_type                             *list_ptr = NULL;
  ds_3gpp_roaming_wait_timer_item_type  *current_list_item_ptr = NULL;
  ds_3gpp_roaming_wait_timer_item_type  *next_list_item_ptr = NULL;
  /*---------------------------------------------------------------------*/

  list_ptr = &ds_3gpp_roaming_info.wait_timer_list;
  
  current_list_item_ptr = 
    (ds_3gpp_roaming_wait_timer_item_type*)list_peek_front( list_ptr );

  while (current_list_item_ptr != NULL)
  {
    next_list_item_ptr = 
      list_peek_next(list_ptr, &(current_list_item_ptr->link));


    DS_3GPP_MSG1_LOW("Clearing the wait timer memory for profile %d", 
                      current_list_item_ptr->profile_num);

    rex_clr_timer(current_list_item_ptr->timer_ptr);

    rex_delete_timer_ex(current_list_item_ptr->timer_ptr);

    list_pop_item(list_ptr, &(current_list_item_ptr->link) );

    modem_mem_free(current_list_item_ptr,MODEM_MEM_CLIENT_DATA);

    current_list_item_ptr = next_list_item_ptr;
    
  }
  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_CLEAR_WAIT_TIMER_ENTRY

  DESCRIPTION
  This function is called to clear the wait timer corresponding to the given
  profile.
  
  PARAMETERS  
  Profile Num
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
void ds_3gpp_roaming_clear_wait_timer_entry
(
  uint8 profile_num  
)
{
  list_type                             *list_ptr = NULL;
  ds_3gpp_roaming_wait_timer_item_type  *current_list_item_ptr = NULL;
  /*---------------------------------------------------------------------*/

  list_ptr = &ds_3gpp_roaming_info.wait_timer_list;
  
  current_list_item_ptr = 
    (ds_3gpp_roaming_wait_timer_item_type*)list_peek_front( list_ptr );

  while (current_list_item_ptr != NULL)
  {
    if ( current_list_item_ptr->profile_num == profile_num )
    {
      DS_3GPP_MSG1_LOW("Clearing the wait timer memory for profile %d",
                        profile_num);

      rex_clr_timer(current_list_item_ptr->timer_ptr);

      rex_delete_timer_ex(current_list_item_ptr->timer_ptr);

      list_pop_item(list_ptr, &(current_list_item_ptr->link) );

      modem_mem_free(current_list_item_ptr,MODEM_MEM_CLIENT_DATA);

      break;
    }

    current_list_item_ptr = 
      list_peek_next(list_ptr, &(current_list_item_ptr->link));
  }

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_WAIT_TIMER_EXP_CB

  DESCRIPTION
  This function callback is invoked when the PDN Disconnect wait timer
  expires.
  
  PARAMETERS  
  Callback Data
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_wait_timer_exp_cb
(
  uint32 cb_data
)
{
  uint8                                  profile_num = cb_data;
  list_type                             *list_ptr = NULL;
  ds_3gpp_roaming_wait_timer_item_type  *current_list_item_ptr = NULL;
  /*---------------------------------------------------------------------*/

  DS_3GPP_MSG1_MED("Wait timer expired, Profile number: %d. "
                    "Try to tear down the PDN", profile_num);

  ds_3gpp_pdn_cntx_teardown_pdn_per_profile
    (profile_num, PS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING);    

  list_ptr = &ds_3gpp_roaming_info.wait_timer_list;
  
  current_list_item_ptr = 
    (ds_3gpp_roaming_wait_timer_item_type*)list_peek_front( list_ptr );

  while (current_list_item_ptr != NULL)
  {
    if ( current_list_item_ptr->profile_num == profile_num )
    {
      DS_3GPP_MSG0_LOW("Clearing the wait timer memory");

      rex_delete_timer_ex(current_list_item_ptr->timer_ptr);

      list_pop_item(list_ptr, &(current_list_item_ptr->link) );

      modem_mem_free(current_list_item_ptr,MODEM_MEM_CLIENT_DATA);

      break;
    }

    current_list_item_ptr = 
      list_peek_next(list_ptr, &(current_list_item_ptr->link));
  }
  
} /* ds_3gpp_roaming_wait_timer_exp_cb */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ALLOCATE_AND_START_WAIT_TIMER

  DESCRIPTION
  This function tries to allocate and start the wait timer.
  
  PARAMETERS  
  Profile for which the wait timer is configured.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE, if Wait Timer started or no need to Start Wait Timer
  FALSE, if Wait Timer Did not Start
   
  SIDE EFFECTS 
  None.
===========================================================================*/ 

static boolean ds_3gpp_roaming_allocate_and_start_wait_timer
(
  ds_umts_pdp_profile_type   *profile_info_ptr
)
{
  ds_3gpp_roaming_wait_timer_item_type *wait_timer_list_item_ptr = NULL;
  uint8                                 pdn_discon_wait_time = 0;
  uint8                                 profile_num = 0;
  boolean                               ret_val = FALSE;
  /*----------------------------------------------------------------*/

  ASSERT(profile_info_ptr != NULL);

  pdn_discon_wait_time = profile_info_ptr->pdn_discon_wait_time;
  profile_num = profile_info_ptr->context.pdp_context_number;

  DS_3GPP_MSG2_HIGH("Profile number: %d, PDN Discon Wait Time: %d minutes", 
                    profile_num, pdn_discon_wait_time );

  if (pdn_discon_wait_time != 0)
  {
    if (pdn_discon_wait_time == DS_3GPP_ROAMING_MAX_PDN_DISCON_WAIT_TIME)
    {
      DS_3GPP_MSG0_LOW("Responsibility of Client APP to perform dereg "
                        "when it deems necessary. ");

      ret_val = TRUE;
    }
    else
    {
      wait_timer_list_item_ptr = 
        modem_mem_alloc(sizeof(ds_3gpp_roaming_wait_timer_item_type),
                            MODEM_MEM_CLIENT_DATA );

      if (wait_timer_list_item_ptr != NULL)
      {
        wait_timer_list_item_ptr->timer_ptr =  
          rex_create_timer_ex (ds_3gpp_roaming_wait_timer_exp_cb, 
                               (uint32)profile_num);

        if (wait_timer_list_item_ptr->timer_ptr != NULL)
        {
          wait_timer_list_item_ptr->profile_num = profile_num;

          list_push_front(&ds_3gpp_roaming_info.wait_timer_list, 
                              &(wait_timer_list_item_ptr->link));

          rex_set_timer(wait_timer_list_item_ptr->timer_ptr, 
                        pdn_discon_wait_time * 60 * 1000);

          DS_3GPP_MSG2_HIGH("Started the PDN Disconnect Wait Timer "
                            "for %d minutes, profile: %d", 
                            pdn_discon_wait_time, profile_num );
          ret_val = TRUE;
        }
        else
        {
          DS_3GPP_MSG0_LOW("Cannot allocate wait timer !");
        }      
      }
      else
      {
        DS_3GPP_MSG0_LOW("Cannot allocate memory !");
      }
    }
  }

  return ret_val;
} /* ds_3gpp_roaming_allocate_and_start_wait_timer */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_BLOCK_APN_PER_PROFILE

  DESCRIPTION
  This function blocks given APN corresponding to given profile
  because of Roaming.
  
  PARAMETERS  
  Subscription id
  Profile Information Ptr
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_block_apn_per_profile
(
  sys_modem_as_id_e_type     subs_id,
  ds_umts_pdp_profile_type   *profile_info_ptr
)
{
  ds_pdn_context_state_e            state = DS_PDN_CONTEXT_STATE_DOWN;
  boolean                           tear_down_pdn = TRUE;
  ds_pdn_context_s*                 pdn_cntxt_p = NULL;
  ds_3gpp_iface_s                   *ds_v4_iface_ptr = NULL;
  ds_3gpp_iface_s                   *ds_v6_iface_ptr = NULL;
  /*---------------------------------------------------------------------------*/

  ASSERT(profile_info_ptr != NULL);

  pdn_cntxt_p = ds_pdn_cntxt_get_pdn_context_from_cid
                  (profile_info_ptr->context.pdp_context_number,subs_id);

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
  {
    state = pdn_cntxt_p->ds_pdn_context_dyn_p->state;
    DS_3GPP_MSG1_HIGH("PDN Cntxt is in %d state", state);
  }

  switch (state)
  {
    case DS_PDN_CONTEXT_STATE_UP:
    {
      ds_v4_iface_ptr = 
        pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
          [DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

      ds_v6_iface_ptr = 
        pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
          [DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

      if ( (ds_v4_iface_ptr != NULL && 
            PS_IFACE_IS_IN_USE(&(ds_v4_iface_ptr->ps_iface))) ||
          (ds_v6_iface_ptr != NULL && 
           PS_IFACE_IS_IN_USE(&(ds_v6_iface_ptr->ps_iface))) )
      {

        if (ds_3gpp_roaming_allocate_and_start_wait_timer (profile_info_ptr))
        {
          tear_down_pdn = FALSE;
        }
      }
      break;
    }

    case DS_PDN_CONTEXT_STATE_DOWN:
    case DS_PDN_CONTEXT_STATE_GOING_DOWN:
    {
      tear_down_pdn = FALSE;
      break;
    }

    default:
      break;
  }

  if (tear_down_pdn)
  {
    ds_3gpp_pdn_cntx_teardown_pdn_per_profile 
      (profile_info_ptr->context.pdp_context_number, 
         PS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING);
  }

  ds_3gpp_pdn_throt_perform_roaming_throttling
    (profile_info_ptr->context.apn, subs_id);

} /* ds_3gpp_roaming_block_apn_per_profile */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_BLOCK_APNS

  DESCRIPTION
  This function blocks all APNs which are disallowed during Roaming.
  
  PARAMETERS  
  Subscription id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_block_apns
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_umts_pdp_profile_type               profile_info;
  ds_umts_pdp_profile_status_etype       result = DS_UMTS_PDP_FAIL;
  int                                    i = 0; 
  ds3gsubsmgr_subs_id_e_type             subs_index = 0;
  uint8                                  profile_nums_arr[DS_UMTS_MAX_PDP_PROFILE_NUM];
  uint8                                  profile_count = 0;
  /*----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  profile_count = 
    ds_3gpp_profile_cache_get_valid_profile_numbers(
      dsumts_subs_mgr_get_subs_id((sys_modem_as_id_e_type)subs_index), 
      profile_nums_arr, DS_UMTS_MAX_PDP_PROFILE_NUM);

  for (i=0; i< profile_count; i++)
  {
    memset(&profile_info, 0, sizeof(ds_umts_pdp_profile_type));

    result = ds_umts_get_pdp_profile_all_data_per_subs(profile_nums_arr[i], 
                               dsumts_subs_mgr_get_subs_id(subs_id), &profile_info);

    if (result != DS_UMTS_PDP_SUCCESS)
    {
      continue;
    }

    DS_3GPP_MSG2_HIGH("Roaming Disallowed Flag: %d, Profile Id: %d ",
                      profile_info.roaming_disallowed, profile_nums_arr[i]);

    if (profile_info.roaming_disallowed)
    {
      ds_3gpp_roaming_block_apn_per_profile(subs_id, &profile_info);
    }
  } /* for (i=0; i< profile_count; i++) */

} /* ds_3gpp_roaming_block_apns */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UNBLOCK_APN_PER_PROFILE

  DESCRIPTION
  This function unblocks APN corresponding to the given profile.
  
  PARAMETERS  
  Subscription id
  Profile Information Ptr
  Boolean Pointer set to True if the attach apn is unblocked
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_unblock_apn_per_profile
(
  sys_modem_as_id_e_type    subs_id,
  ds_umts_pdp_profile_type  *profile_info_ptr,
  boolean*                  is_attach_apn_ptr
)
{
  /*-------------------------------------------------------------------------*/


  if (is_attach_apn_ptr == NULL)
  {
    DS_3GPP_MSG0_LOW("is_attach_apn_ptr is NULL !");
    return;
  }

  if (profile_info_ptr != NULL)
  {
    ds_3gpp_roaming_clear_wait_timer_entry
      ((uint8)profile_info_ptr->context.pdp_context_number);

    ds_3gpp_pdn_throt_unblock_roaming_throttling
      (profile_info_ptr->context.apn,subs_id, FALSE,is_attach_apn_ptr);
  }
  else
  {
    DS_3GPP_MSG0_LOW("Profile info ptr is NULL !");
  }
} /* ds_3gpp_roaming_unblock_apn_per_profile */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_UNBLOCK_APNS

  DESCRIPTION
  This function unblocks all APNs which were disallowed during Roaming.
  
  PARAMETERS  
  Subscription id
  Boolean Pointer set to True if the attach apn is unblocked
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
static void ds_3gpp_roaming_unblock_apns
(
  sys_modem_as_id_e_type subs_id,
  boolean*               is_attach_apn_ptr
)
{
  /*---------------------------------------------------------------------*/


  if (is_attach_apn_ptr == NULL)
  {
    DS_3GPP_MSG0_LOW("is_attach_apn_ptr is NULL !");
    return;
  }

  ds_3gpp_roaming_flush_wait_timer_list();

  ds_3gpp_pdn_throt_unblock_roaming_throttling(NULL, subs_id, TRUE,is_attach_apn_ptr);

} /* ds_3gpp_roaming_unblock_apns */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ADVERTISE_APNS_BLOCKED_DUE_TO_ROAMING

  DESCRIPTION
  This function advertises all Roaming Blocked APNs to its clients
  
  PARAMETERS  
  Subscription id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/  
void ds_3gpp_roaming_advertise_apns_blocked_due_to_roaming
(
  sys_modem_as_id_e_type subs_id,
  boolean                advertise_empty_list
)
{
  ds_umts_pdp_profile_type                       profile_info;
  ds_umts_pdp_profile_status_etype               result = DS_UMTS_PDP_FAIL;
  int                                            i = 0; 
  ds3gsubsmgr_subs_id_e_type                     subs_index = 0;
  int16                                          ps_errno = 0;
  ps_sys_roaming_disallowed_info_type            roaming_disallowed_info;
  uint8                                          num_valid_apns = 0;
  uint8                                         profile_nums_arr[DS_UMTS_MAX_PDP_PROFILE_NUM];
  uint8                                         profile_count = 0;
  /*----------------------------------------------------------------------*/

  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  memset(&roaming_disallowed_info, 0, 
         sizeof(ps_sys_roaming_disallowed_info_type));

  if (ds_3gpp_roaming_get_status(subs_id) && advertise_empty_list == FALSE )
  {
    profile_count = ds_3gpp_profile_cache_get_valid_profile_numbers
      (dsumts_subs_mgr_get_subs_id((sys_modem_as_id_e_type)subs_index),
       profile_nums_arr, DS_UMTS_MAX_PDP_PROFILE_NUM);

    for (i=0; i < profile_count; i++)
    {
      memset(&profile_info, 0, sizeof(ds_umts_pdp_profile_type));

      result = 
        ds_umts_get_pdp_profile_all_data_per_subs(profile_nums_arr[i], 
          dsumts_subs_mgr_get_subs_id((sys_modem_as_id_e_type)subs_index), 
          &profile_info);

      if (result != DS_UMTS_PDP_SUCCESS)
      {
        continue;
      }

      DS_3GPP_MSG2_HIGH("Roaming Disallowed Flag: %d, Profile Id: %d ",
                        profile_info.roaming_disallowed, profile_nums_arr[i]);

      if (profile_info.roaming_disallowed && num_valid_apns < PS_SYS_MAX_APNS)
      {
        memscpy(roaming_disallowed_info.apn_list[num_valid_apns].apn_name , 
                PS_SYS_MAX_APN_LEN,
                profile_info.context.apn, 
                strlen((const char*)profile_info.context.apn) + 1);

       DATA_3GPP_MSG_SPRINTF_1
          (MSG_LEGACY_HIGH,"Preparing Roaming Disallowed Ind for APN: %s",
             roaming_disallowed_info.apn_list[num_valid_apns].apn_name);

        num_valid_apns++;

      }
    }

    if (num_valid_apns > 0)
    {
      roaming_disallowed_info.is_roaming_disallowed = TRUE;
      roaming_disallowed_info.num_apn_names = num_valid_apns;
    }
  } /* if (ds_3gpp_roaming_get_status(subs_id)) */

  DS_3GPP_MSG1_LOW("Roaming Disallowed Ind. Number of Blocked APNs: %d ",
                    roaming_disallowed_info.num_apn_names);

  if (0 > ps_sys_conf_set_ex(PS_SYS_TECH_3GPP,
                             PS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO,
                             (ps_sys_subscription_enum_type)
                              ds3gsubsmgr_subs_id_ds3g_to_ds(subs_index),
                              &roaming_disallowed_info,
                              &ps_errno))
  {
    DS_3GPP_MSG0_LOW("Sending Roaming Disallowed Info Failed");
  }
} /* ds_3gpp_roaming_advertise_apns_blocked_due_to_roaming */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_ADVERTISE_THROT_INFO

  DESCRIPTION
  This function advertises Throttling Information to all clients and
  disables lte if needed
  
  PARAMETERS
  1. disable_lte_if_needed: Flag indicating whether LTE needs to be disabled
  2. Subscription id
  3. Boolean set to True if the attach apn is unblocked
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/ 
static void ds_3gpp_roaming_advertise_throt_info
(
  boolean                disable_lte_if_needed,
  sys_modem_as_id_e_type subs_id,
  boolean                is_attach_apn
)
{
  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);

#ifdef FEATURE_DATA_LTE
  if (disable_lte_if_needed)
  {
    ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable(subs_id);
  }
  else
  {
    if (is_attach_apn)
    {
      ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
    }
  }
#endif /* FEATURE_DATA_LTE */

} /* ds_3gpp_roaming_advertise_throt_info */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_PROCESS_DISALLOWED_FLAG_CHG_CMD

  DESCRIPTION
  This function is used to process the change in Roaming Disallowed Flag
  
  PARAMETERS  
  Profile Number
  Roaming Disallowed Flag
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_roaming_process_disallowed_flag_chg_cmd
(
  uint16                 profile_num,
  boolean                roaming_disallowed_flag,
  sys_modem_as_id_e_type subs_id
)
{
  ds_umts_pdp_profile_type           profile_info;
  ds_umts_pdp_profile_status_etype   result = DS_UMTS_PDP_FAIL;     
  boolean                            is_attach_apn = FALSE;   
  /*------------------------------------------------------------------------*/

  ds3gpp_enter_global_crit_section();
  if (ds_3gpp_roaming_get_status(subs_id) == FALSE)
  {
    ds3gpp_leave_global_crit_section();
    DS_3GPP_MSG0_HIGH("UE is not roaming. "
                      "No need to process change to disallowed flag");
    return;
  }
  ds3gpp_leave_global_crit_section();
  
  result = ds_umts_get_pdp_profile_all_data_per_subs(profile_num, 
                            dsumts_subs_mgr_get_subs_id(subs_id), &profile_info);
  
  if (result != DS_UMTS_PDP_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot read profile num %d ", profile_num);
    return;
  }
  
  if (roaming_disallowed_flag == TRUE)
  {
    ds_3gpp_roaming_block_apn_per_profile(subs_id, &profile_info);   
  }
  else
  {
    ds_3gpp_roaming_unblock_apn_per_profile(subs_id, &profile_info,&is_attach_apn);
  }

  ds_3gpp_roaming_advertise_throt_info(roaming_disallowed_flag, 
                                       subs_id,
                                       is_attach_apn);
  
  ds_3gpp_roaming_advertise_apns_blocked_due_to_roaming(subs_id, FALSE);  

  return;
} /* ds_3gpp_roaming_process_disallowed_flag_chg_cmd */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_PROCESS_PLMN_LIST_CHG_CMD

  DESCRIPTION
  This function is used to process the change in PLMN List.
  
  PARAMETERS  
  Subscription Id
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
static void ds_3gpp_roaming_process_plmn_list_chg_cmd
(
  sys_modem_as_id_e_type   subs_id
)
{
  sys_plmn_id_s_type       current_plmn_id;
  sys_plmn_id_s_type       zero_plmn_id;
  /*------------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  memset(&current_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  memset(&zero_plmn_id, 0, sizeof(sys_plmn_id_s_type));

  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn_id, subs_id);

  if(memcmp(&(current_plmn_id),
            &(zero_plmn_id),
            sizeof(sys_plmn_id_s_type)) != 0)
  {
    ds_3gpp_roaming_inform_new_plmn_info(subs_id, current_plmn_id);
  }

  return;
} /* ds_3gpp_roaming_process_plmn_list_chg_cmd */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_HDLR_PROCESS_CMDS

  DESCRIPTION
  This function processes any commands that need to be handled by
  Roaming Handler Module.
  
  PARAMETERS
  1. Cmd Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_hdlr_process_cmds
(
  const ds_cmd_type      *cmd_ptr
)
{
  ds_3gpp_roam_info_type  *roam_info_ptr = NULL;
  sys_modem_as_id_e_type  *subs_id_ptr = NULL;

  ASSERT(cmd_ptr != NULL);
  ASSERT(cmd_ptr->cmd_payload_ptr != NULL);

  switch (cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_ROAMING_DISALLOWED_FLAG_CHG:
    {
      roam_info_ptr = (ds_3gpp_roam_info_type*)cmd_ptr->cmd_payload_ptr;

      ds_3gpp_roaming_process_disallowed_flag_chg_cmd(
         roam_info_ptr->profile_num,
         roam_info_ptr->roaming_disallowed_flag,
         roam_info_ptr->subs_id);
      break;
    }

    case DS_CMD_3GPP_PLMN_LIST_CHG:
    {
      subs_id_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
      ds_3gpp_roaming_process_plmn_list_chg_cmd(*subs_id_ptr);
      break;
    }

    default:
    {
      DS_3GPP_MSG1_LOW("Unrecognized cmd: %d, ignoring",cmd_ptr->hdr.cmd_id);
      break;
    }
  }

  return;
} /* ds_3gpp_roaming_hdlr_process_cmds */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_IS_APN_DISALLOWED

  DESCRIPTION
  This function checks whether PDN Connection to given APN is allowed or not.
  
  PARAMETERS
  1. APN Name
  2. Subscription Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE, if PDN Connection is allowed
  FALSE, otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_roaming_is_apn_disallowed
(
  byte                   *apn,
  sys_modem_as_id_e_type subs_id
)
{
  ds_umts_pdp_profile_type                       profile_info;
  ds_umts_pdp_profile_status_etype               result = DS_UMTS_PDP_FAIL;
  int                                            i; 
  boolean                                        ret_val = FALSE;
  uint8                                         profile_nums_arr[DS_UMTS_MAX_PDP_PROFILE_NUM];
  uint8                                         profile_count = 0;

  do
  {
    ds3gpp_enter_global_crit_section();
    if (ds_3gpp_roaming_get_status(subs_id) == FALSE)
    {
      ds3gpp_leave_global_crit_section();
      DS_3GPP_MSG0_HIGH("UE is not roaming. "
                        "No need to process change to disallowed flag");
      break;
    }
    ds3gpp_leave_global_crit_section();

    profile_count = ds_3gpp_profile_cache_get_valid_profile_numbers(
                                                 dsumts_subs_mgr_get_subs_id(subs_id),
                                                 profile_nums_arr, 
                                                                     DS_UMTS_MAX_PDP_PROFILE_NUM);


    for (i=0; i< profile_count; i++)
    {
      memset(&profile_info, 0, sizeof(ds_umts_pdp_profile_type));
      result = ds_umts_get_pdp_profile_all_data_per_subs(profile_nums_arr[i],
                                 dsumts_subs_mgr_get_subs_id(subs_id), &profile_info);

      if (result != DS_UMTS_PDP_SUCCESS)
      {
        continue;
      }

      DS_3GPP_MSG2_HIGH("Roaming Disallowed Flag: %d, Profile Id: %d ",
                         profile_info.roaming_disallowed, profile_nums_arr[i]);

      if ( memcmp(profile_info.context.apn, apn, strlen((const char*)apn) + 1) == 0 )
      {
        if (profile_info.roaming_disallowed)
        {
          ret_val = TRUE;
          break;
        }
      }
    }
  } while (0); 
  
  return ret_val;
} /* ds_3gpp_roaming_is_apn_disallowed */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_HOME_PLMN_LIST_SIZE

  DESCRIPTION
  This function gets the size of the Home PLMN List
  
  PARAMETERS
  Subscription Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Number of Home PLMNs configured
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_roaming_get_home_plmn_list_size
(
  sys_modem_as_id_e_type subs_id
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return 0;
  }  

 return list_size(&ds_3gpp_roaming_info.subs_index[subs_id].home_plmn_list);
} /* ds_3gpp_roaming_get_home_plmn_list_size */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_EXCP_HOME_PLMN_LIST_SIZE

  DESCRIPTION
  This function gets the size of the exception Home PLMN List
  
  PARAMETERS
  Subscription Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Number of Home PLMNs configured
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_roaming_get_excp_home_plmn_list_size
(
  sys_modem_as_id_e_type subs_id
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG0_LOW("Subs Id is invalid");
    return 0;
  }  

 return list_size(&ds_3gpp_roaming_info.subs_index[subs_id].excp_home_plmn_list);
} /* ds_3gpp_roaming_get_excp_home_plmn_list_size */

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_IS_PLMN_FOUND_IN_LIST

  DESCRIPTION
  This function checks whether the PLMN can be found in a given PLMN List.
  
  PARAMETERS
  PLMN Id
  PLMN List Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Boolean.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_roaming_is_plmn_found_in_list
(
  sys_plmn_id_s_type          plmn_id,
  list_type                  *list_ptr
)
{
  boolean                                     ret = FALSE;
  ds_3gpp_roaming_home_plmn_list_item_type   *item_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  if (list_ptr == NULL)
  {
    return FALSE;
  }

  item_ptr = (ds_3gpp_roaming_home_plmn_list_item_type*)
                 list_peek_front( list_ptr );

  while (item_ptr != NULL)
  {
    if (ds_3gpp_roaming_is_plmn_match(plmn_id, item_ptr->plmn_id))
    {
      ret = TRUE;
      break;
    }
    item_ptr = list_peek_next(list_ptr, &(item_ptr->link));
  }

  return ret;
}/*ds_3gpp_roaming_is_plmn_found_in_list*/

/*===========================================================================
  FUNCTION DS_3GPP_ROAMING_GET_CURRENT_PLMN_LIST

  DESCRIPTION
  This function gets the current PLMN List. If current PLMN is same as RPLMN
  or part of EPLMN list, copy RPLMN and EPLMN list to current PLMN list.
  Otherwise, copy current PLMN to current PLMN list.
  
  PARAMETERS
  Subscription Id
  New PLMN Id
  Current PLMN List
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_roaming_get_current_plmn_list
(
  sys_modem_as_id_e_type     subs_id,
  sys_plmn_id_s_type         new_plmn_id,
  sys_plmn_list_s_type       *curr_plmn_list_p
)
{
  sys_plmn_id_s_type         rplmn;
  sys_plmn_list_s_type       eplmn_list;
  boolean                    match_found = FALSE;
  int                        i;
  /*-----------------------------------------------------------------------*/

  if (curr_plmn_list_p == NULL)
  {
    return;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  memset(&rplmn, 0, sizeof(sys_plmn_id_s_type)); 
  memset(&eplmn_list, 0, sizeof(sys_plmn_list_s_type));

  /*-------------------------------------------------------------------------
    Get EPLMN list and RPLMN
  -------------------------------------------------------------------------*/
  ds_3gpp_get_eplmn_list_per_sub(subs_id,
                                 &eplmn_list,
                                 &rplmn);

  /* Compare current PLMN with RPLMN and EPLMN list */
  if (sys_plmn_match(rplmn, new_plmn_id))
  {
    match_found = TRUE;
  }
  else
  {
    for (i = 0; i < eplmn_list.length; i++)
    {
      if (sys_plmn_match(eplmn_list.plmn[i], new_plmn_id))
      {
        match_found = TRUE;
        break;
      }
    }
  }

  /* If current PLMN is same as RPLMN or part of EPLMN list, copy RPLMN and
     EPLMN list to current PLMN list. Otherwise, copy current PLMN to current
     PLMN list */
  if (match_found)
  {
    memscpy(curr_plmn_list_p,
            sizeof(sys_plmn_list_s_type),
            &eplmn_list,
            sizeof(sys_plmn_list_s_type));
    memscpy(&curr_plmn_list_p->plmn[eplmn_list.length],
            sizeof(sys_plmn_id_s_type),
            &rplmn,
            sizeof(sys_plmn_id_s_type));
    curr_plmn_list_p->length++;
  }
  else
  {
    memscpy(&curr_plmn_list_p->plmn[0],
            sizeof(sys_plmn_id_s_type),
            &new_plmn_id,
            sizeof(sys_plmn_id_s_type));
    curr_plmn_list_p->length = 1;
  }
}/* ds_3gpp_roaming_get_current_plmn_list */