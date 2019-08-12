/******************************************************************************
  @file    ds_profile_db_internal.h
  @brief   Database 

  DESCRIPTION
  Internal APIs for profiles database. Should be used by mode handlers and other internal
  components. 
  ! Not for external client usage !  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/13   ez      Created the module. First version of the file.

===========================================================================*/

#ifndef DS_PROFILE_DB_INTERNAL_H
#define DS_PROFILE_DB_INTERNAL_H

#include "comdef.h"
#include "ds_profile_db.h"

/*===========================================================================
   Function declarations
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "dstask_v.h"
#include "ds3geventmgr.h"
#include "ds_profile_3gpp.h"
#include "ds3gsubsmgr.h"

#define DB_ARRAY_NUM_ELEM(x) (sizeof (x) / sizeof (*(x)))

typedef enum {
  DS_PROFILE_DB_NOTIFY_CMD  
} ds_profile_db_callback_operation_type;


typedef struct {
  void** profiles;
  unsigned int profiles_count;
  int event;
  uint32 subs;
} ds_profile_db_notify_cmd_type;

typedef struct {
  ds_profile_db_callback_operation_type operation_type;
  union {
    ds_profile_db_notify_cmd_type    notify_cmd;    
  };
} ds_profile_db_callback_cmd_type;

typedef struct {
  uint32 subs;
  uint32 slot_id;
} ds_profile_nv_refresh_cmd_type;

typedef struct
{
  sys_modem_device_mode_e_type  curr;
  sys_modem_device_mode_e_type  prev;
} ds_profile_db_device_mode;

typedef ds_profile_db_result_type (*validate_fn_type)(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type*  info,
  const void*                     profile_params,
  ...);

typedef struct {
  ds_profile_db_result_type (*set_default_profile_num)(
    ds_profile_db_profile_num_type num,
    ds_profile_db_family_type family
  );

   ds_profile_db_result_type (*set_default_profile_num_per_subs)(
    ds_profile_db_profile_num_type num,
    ds_profile_db_family_type family,
    ds_profile_db_subs_type   subs
  );

  ds_profile_db_result_type (*get_default_profile_num)(
    ds_profile_db_profile_num_type* num,
    ds_profile_db_family_type family
  );

  ds_profile_db_result_type (*get_default_profile_num_per_subs)(
    ds_profile_db_profile_num_type* num,
    ds_profile_db_family_type family,
    ds_profile_db_subs_type   subs
  );

  ds_profile_db_result_type (*get_is_tech_param_set)(
    ds_profile_db_profile_num_type num,
    ds_profile_db_subs_type    subs,
    ds_profile_db_ident_type   ident,
    boolean *is_tech_param_set
  );

  ds_profile_db_result_type (*get_profiles_range)(
    ds_profile_db_3gpp2_profile_subtype  sub_tech,
    ds_profile_db_profile_num_type*      min_num,
    ds_profile_db_profile_num_type*      max_num
  );

  ds_profile_db_result_type (*get_profile)(
    ds_profile_db_profile_num_type num,
    void* tech_params, 
    ds_profile_db_gen_params_type* gen_params,
    ds_profile_db_subs_type subs
  );


  void (*get_default_profile)(
    void* params
  );

  ds_profile_db_result_type (*validate_general)(
    const void* profile
  );

  validate_fn_type  validation_function;

  boolean is_registered;

}tech_func_table_type;

ds_profile_db_result_type ds_profile_db_init(void);


/* !!! VERY IMPORTANT !!! */
/* This should *only* be called after callback server task was either closed or it's command queue was emptied 
   Also, at the point where user can no longer invoke ds profile API's */
void ds_profile_db_deinit(void);

/* special API, should be used by mode handlers only 
   2 main differences from the public API :

   1. Invoked synchronously, i.e as part of the ds_profile_db API that caused the update
   2. treat_tech_mask_as_OR - if TRUE tech_mask is treated as a combination of simple tech's
      rather than profile with complex tech. 
      For simple tech profiles in the system, this means that callback will be invoked if 
      profiles technology is *any* of the specified in tech_mask
      For complex profiles in the system, this means that callback will be invoked if the 
      changed part it *any* of the specified in tech_mask. 

      For example:
      tech_mask = TECH_3GPP | TECH_COMMON; changed profile - simple 3GPP 
      callback invoked

      tech_mask = TECH_3GPP | TECH_COMMON; changed profile - EPC (3GPP+3GPP2+COMMON)
      callback invoked if the changed part is either 3GPP or COMMON (or both) */

ds_profile_db_callback_id_type ds_profile_db_register_for_notifications_MH(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,   
  boolean treat_tech_mask_as_OR,
  ds_profile_db_cb_type cback,
  void *user_data
);

ds_profile_db_result_type ds_profile_db_register_function_table_MH(
  tech_func_table_type *tech_table,
  ds_profile_db_supported_tech_mask tech_mask
);

tech_func_table_type* ds_profile_db_get_tech_tbl(
  ds_profile_db_tech_type tech
);

void ds_profile_db_init_power_up_finished(void);

void ds_profile_db_cmd_hdlr (
  ds_cmd_type  *cmd_ptr
);

void ds_profile_db_ds3g_cmd_hdlr(
  ds3geventmgr_event_type        event_id,
  ds3geventmgr_filter_type       *filter_info_ptr,
  void                           *event_info_ptr,
  void                           *data_ptr
); 

void ds_profile_db_ds3g_subsmgr_event_cb(
  ds3gsubsmgr_event_type        event_id,
  ds3gsubsmgr_event_info_type  *event_info_ptr,
  void                         *data_ptr
);


#ifdef __cplusplus
}
#endif

#endif /* DS_PROFILE_DB_INTERNAL_H */
