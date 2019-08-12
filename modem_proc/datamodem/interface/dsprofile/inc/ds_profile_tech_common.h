/******************************************************************************
  @file    ds_profile_tech_common.h
  @brief   

  DESCRIPTION
  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/inc/ds_profile_tech_common.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/12   gs      Added A clenaup function pointer to struct tech_fntbl_type
05/31/11   bvd     Added changes for supporting non-persistent profile and other profile attributes
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/17/11   ss      Added support for Iface linger.
06/28/10   ss      Fixed compiler warnings.
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/
#ifndef DS_PROFILE_TECH_COMMON_H
#define DS_PROFILE_TECH_COMMON_H

#include "ds_profile.h"
#include "ds_util.h"
#include "customer.h"
/*---------------------------------------------------------------------------
                           DECLARATIONS
---------------------------------------------------------------------------*/
#define DSI_SUCCESS 0
#define DSI_FAILURE 1

/* Macro to convert identifier to mask */
#define CONVERT_IDENT_TO_MASK( a, ident ) { \
  a = (uint64)( (uint64)1 << ident ); \
}

/*--------------------------------------------------------------------------- 
   Each tech should have concrete implementations of below virtual
   functions and return the structure as part of tech init
---------------------------------------------------------------------------*/
typedef struct
{
  /* 
	 Create Profile
         tech - technology
         config_ptr - config type for persistent property and other profile attributes
   	 num        - output parameter, profile number
  */
  ds_profile_status_etype (*create)(
    ds_profile_tech_etype  tech,
    ds_profile_config_type *config_ptr,
    ds_profile_num_type    *num          
  );

  /* 
	 Delete a Profile
     tech - technology
   	 num - profile number to be deleted
     boolean - if true, default profile can be deleted
  */
  ds_profile_status_etype (*del)(
    ds_profile_tech_etype tech,
    ds_profile_num_type  num, 
    boolean enable_deleting_default_profile,
    ds_profile_subs_etype subs
  );

  /* 
    Clean up when library exits  
    tech - technology
  */
  ds_profile_status_etype (*cleanup)(
    ds_profile_tech_etype tech
  );


  /*
	 Allocate blob memory on 
   	 begin_transaction
         tech - technology
  */
  void * (*alloc)(
    ds_profile_tech_etype      tech
  );

  /* 
   	 Dealloc blob memory on	end_transaction
   	 ptr - input parameter, ptr to blob
  */
  ds_profile_status_etype (*dealloc)(
    void *ptr
  );

  /* 
   	 Set identified params in the blob
         tech - technology
   	 blob - input parameter, pointer to profile blob
   	 ident - input parameter, identifier for parameter to be set
     info - input parameter, value to be set
  */
  ds_profile_status_etype (*set_param)( 
    ds_profile_tech_etype       tech,
    void                        *blob,
    ds_profile_identifier_type   ident,
    const ds_profile_info_type  *info
  );

  /* 
   	 Get identified params from the blob
    tech - technology
	 blob - input parameter, pointer to profile blob
   	 ident - input parameter, identifier for parameter to be fetched
     info - output parameter, to store value of parameter fetched
  */
  ds_profile_status_etype (*get_param)( 
    ds_profile_tech_etype       tech,  
    void                        *blob,
    ds_profile_identifier_type   ident,
    ds_profile_info_type        *info
  );

  /* 
   	 Read profile into memory
         tech - technology
   	 num - input parameter, profile to be read
   	 blob - output parameter, to store profile 
  */
  ds_profile_status_etype (*profile_read)(
    ds_profile_tech_etype tech,
    ds_profile_num_type   num,
    void                 *blob,
    ds_profile_subs_etype subs
  );

  /* 
   	 Write profile to persistant storage
         tech - technology
   	 num - input parameter, profile to be written
   	 blob - input parameter
  */
  ds_profile_status_etype (*profile_write)(
    ds_profile_tech_etype tech,
    ds_profile_num_type   num,
    void                 *blob
  );

  /* 
   	 Reset value of param to default
         tech - technology
         num - input parameter, profile to be written
         ident - input parameter, identifier for parameter to be fetched
  */
  ds_profile_status_etype (*reset_param)(
    ds_profile_tech_etype       tech,
    ds_profile_num_type         num,
    ds_profile_identifier_type  ident,
    ds_profile_subs_etype       subs,
    boolean mark_valid
  );

  /* 
   	 Reset all parameters to default values
         tech - technology
  */
  ds_profile_status_etype (*reset_profile_to_default)(  
    ds_profile_tech_etype  tech,
    ds_profile_num_type    num,
    ds_profile_subs_etype subs
  );

  /*
    Set a profile as default 
    tech - technology
    family - EMBEDDED, TETHERED, etc
    num - input parameter, profile to be written
  */  
  ds_profile_status_etype (*set_default_profile)(  
    ds_profile_tech_etype  tech,
    uint32                 family, 
    ds_profile_num_type    num
  );

  /* 
    Get the default profile number 
    tech - technology
    family - EMBEDDED, TETHERED, etc
    num - output parameter, default profile  
  */  
  ds_profile_status_etype (*get_default_profile)(
    ds_profile_tech_etype  tech,
    uint32                 family, 
    ds_profile_num_type   *num
  );

  /* 
    Set a profile as default for given subscription
    tech - technology
    subs_id - subscription id
    family - EMBEDDED, TETHERED, etc
    num - input parameter, default profile  
 */
  ds_profile_status_etype (*set_default_profile_per_subs)(
    ds_profile_tech_etype   tech,
    uint32                  family,
    ds_profile_subs_etype  subs_id,
    ds_profile_num_type     num
  );

  /* 
    Get the default profile number for the given subscription
    tech - technology
    subs_id - subscription id
    family - EMBEDDED, TETHERED, etc
    num - output parameter, default profile  
  */
  ds_profile_status_etype (*get_default_profile_per_subs)(
    ds_profile_tech_etype   tech,
    uint32                  family,
    ds_profile_subs_etype  subs_id,
    ds_profile_num_type     *num
  );

  /* 
    Validate profile number 
    tech - technology
    num - profile num
  */
  ds_profile_status_etype (*validate_profile_num)(
    ds_profile_tech_etype   tech,
    ds_profile_num_type num,
    ds_profile_subs_etype subs
  );

  /* 
    Get profiles number range 
    tech - technology
    min - output param, minimum profile in range
    max - output param, maximum profile in range
  */
  void ( *get_num_range)(
    ds_profile_tech_etype   tech,
    uint16 *min, 
    uint16 *max
  );

  /* 
    Get list of profiles 
    tech - technology
    hndl - list handle
    lst  - list type
  */
  ds_profile_status_etype (*get_list)(
    ds_profile_tech_etype   tech,
    ds_util_list_hndl_type hndl,
    ds_profile_subs_etype subs,
    ds_profile_list_type  *lst
  );

  /* 
    Get list node
    tech - technology
    list_info - list info
  */
  ds_profile_status_etype (*get_list_node)(
    ds_profile_tech_etype      tech,
    ds_util_itr_hndl_type  hndl,
    ds_profile_list_info_type  *list_info
  );

#ifdef FEATURE_DATA_LTE
  /* Update LTE attach PDN Profile Params */
  ds_profile_status_etype ( *update_lte_attach_pdn_list_profiles)(
    ds_profile_subs_etype subs
  );
#endif /* FEATURE_DATA_LTE */

}tech_fntbl_type;


/* get / set function pointers */
typedef struct 
{
  ds_profile_identifier_type  ident;

  ds_profile_status_etype (*set_fn)(
    void                        *blob,
    uint64                      mask,
    const ds_profile_info_type  *info    /* input parameter */
  );

  ds_profile_status_etype (*get_fn)(
    const void            *blob,
    ds_profile_info_type  *info    /* output parameter */
  );
}dsi_profile_acc_mut_fn_type;

/* Identifier description */
typedef struct
{
  ds_profile_identifier_type  uid;  /* unique id to identify Profile Param */
  uint16  len;  /* length of identified Profile Param  */
}dsi_profile_params_desc_type;

#endif /* DS_PROFILE_TECH_COMMON_H */
