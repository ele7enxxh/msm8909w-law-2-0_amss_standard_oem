#ifndef PS_DPM_CLAT_H
#define PS_DPM_CLAT_H
/*===========================================================================

                     PS_DPM_CLAT . H
DESCRIPTION
Header file describing DPM CLAT definitions.

Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/inc/ps_dpm_clat.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_dpm.h"
#include "ps_in.h"

#ifdef FEATURE_DATA_PS_464XLAT

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/**
  @brief This defines CLAT event per pdn
*/
typedef enum
{
  PS_DPM_CLAT_EV_PDN_CONTEXT_REGISTERED = 1,
  PS_DPM_CLAT_EV_PDN_CONTEXT_REREGISTERED = 2,
  PS_DPM_CLAT_EV_PDN_CONTEXT_REGISTRATION_FAILED = 3,
  PS_DPM_CLAT_EV_MAX
} ps_dpm_clat_ev_enum_type;

/**
  @brief This defines different V6 prefix length types for the 
         clat translation.
*/
typedef enum
{
  PS_DPM_CLAT_V6_PREFIX_LEN_96 = 0,
  PS_DPM_CLAT_V6_PREFIX_LEN_64 = 1,
  PS_DPM_CLAT_V6_PREFIX_LEN_56 = 2,
  PS_DPM_CLAT_V6_PREFIX_LEN_48 = 3,
  PS_DPM_CLAT_V6_PREFIX_LEN_40 = 4,
  PS_DPM_CLAT_V6_PREFIX_LEN_32 = 5,
  PS_DPM_CLAT_V6_PREFIX_LEN_MAX = 0xFF
} ps_dpm_clat_prefix_len_enum_type;

/**
  @brief CLAT context configuration that is common to
         all PDNs.
*/
typedef struct
{
  uint32   v4_client_subnet_mask;
  uint8    tos_ignore_bit_flag ;
  uint8    tos_override_val;
} ps_dpm_clat_global_cntxt_info_type;

/**
  @brief PDN specific clat configuration data for both 4to6 and 
         6to4 translation.
*/
typedef struct
{
  ps_iface_type                    * iface_ptr;
  uint32                             v4_client_addr;
  uint8                              v6_dev_prefix_len;
  uint8                              v6_plat_prefix_len;
  uint8                              v6_dev_addr_prefix_len;
  struct ps_in6_addr                 v6_dev_prefix;
  struct ps_in6_addr                 v6_plat_prefix;
  struct ps_in6_addr                 v6_dev_addr;
  void                             * client_pdn_handle;
} ps_dpm_clat_reg_pdn_cntxt_info_type;


/**
  @brief PDN specific clat configuration data for both 4to6 and 
         6to4 translation. 
*/
typedef struct
{
  ps_iface_type                     * clat_iface_ptr;
  uint32                              v4_client_addr;
  uint8                               v6_dev_prefix_len;
  uint8                               v6_plat_prefix_len;
  struct ps_in6_addr                  v6_dev_prefix;
  struct ps_in6_addr                  v6_plat_prefix;
  struct ps_in6_addr                  v6_dev_addr;
  uint8                               v6_dev_addr_prefix_len;
  void                              * client_pdn_handle;
  void                              * hw_pdn_handle;
  void                              * hw_v6_addr_handle;
} ps_dpm_clat_pdn_cntxt_info_type;

/**
  @brief This is callback provided by client(protocols) which is to
         be called when particular event is triggered.
*/
typedef void (* ps_dpm_clat_ev_cback_fptr_type)
(
  ps_dpm_clat_ev_enum_type         event_name,
  void                           * cb_data
);

/*===========================================================================

                         EXTERNAL FUNCTIONS

===========================================================================*/
/** @ps_dpm_clat_ev_cback_reg

  This function registers event callback for the client. Event callback
  would be invoked when CLAT specific events like registeration,
  deregistration or reconfiguration is successful.

  @datatypes
  ev_cback_ptr          : Event callback function
  
  @return
  
  @dependencies
*/
void ps_dpm_clat_ev_cback_reg
(
  ps_dpm_clat_ev_cback_fptr_type  ev_cback_ptr
);

/** @ps_dpm_clat_reg_pdn_context

  This function saves the information to perform 4to6 and 
  6to4 translation on dpm um info. When rm registers with
  dpm ,the pdn context is sent to hardware.

  @datatypes
  cntxt_info_ptr     : clat pdn context info
  
  @return
  0 success
  -1 failure

  @dependencies
  Data call must be up.
*/
int ps_dpm_clat_reg_pdn_context
(
  ps_dpm_clat_reg_pdn_cntxt_info_type   * cntxt_info_ptr
);

/** @ps_dpm_clat_reg_global_context

  This function sends the global configuration common
  across all CLAT pdn to hardware.

  @datatypes
  global_cntxt_info_ptr : clat global context info
  
  @return
  0 success
  -1 failure

  @dependencies
  None.
*/
int ps_dpm_clat_reg_global_context
(
  ps_dpm_clat_global_cntxt_info_type   * global_cntxt_info_ptr
);

/** @ps_dpm_clat_rereg_pdn_context

  This function reconfigures the information to perform 4to6 and 
  6to4 translation on dpm um info. 

  @datatypes
  clat_cntxt_info_ptr : clat pdn context info
  
  @return

  @dependencies
  Data call must be up.
*/
int ps_dpm_clat_rereg_pdn_context
( 
  ps_dpm_clat_reg_pdn_cntxt_info_type   * cntxt_info_ptr
);
#endif /* FEATURE_DATA_PS_464XLAT */
#endif /* PS_DPM_CLAT_H */