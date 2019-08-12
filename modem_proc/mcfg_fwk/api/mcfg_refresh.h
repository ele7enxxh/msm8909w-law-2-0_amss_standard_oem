/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	mcfg_refresh.h

GENERAL DESCRIPTION

  MCFG header file for auto refrsh
  
Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_refresh.h#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2014/04/11   cys     Create API file

===========================================================================*/
#ifndef _MCFG_REFRESH_H_
#define _MCFG_REFRESH_H_

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_INDEX_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_REFRESH_INDEX_MIN = 0,
  MCFG_REFRESH_INDEX_0 = MCFG_REFRESH_INDEX_MIN,   /**< index 0 */
  MCFG_REFRESH_INDEX_1,                            /**< index 1 */
  MCFG_REFRESH_INDEX_2,                            /**< index 2 */
  MCFG_REFRESH_INDEX_MAX = MCFG_REFRESH_INDEX_2,
  MCFG_REFRESH_INDEX_INVALID = 0x7F,
} mcfg_refresh_index_type;

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_INDEX_MASK_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_REFRESH_INDEX_MASK_NONE  = 0,                          /**< mask for nothing */
  MCFG_REFRESH_INDEX_MASK_0     = 1 << MCFG_REFRESH_INDEX_0,  /**< mask for index 0 */
  MCFG_REFRESH_INDEX_MASK_1     = 1 << MCFG_REFRESH_INDEX_1,  /**< mask for index 1 */
  MCFG_REFRESH_INDEX_MASK_2     = 1 << MCFG_REFRESH_INDEX_2,  /**< mask for index 2 */
  MCFG_REFRESH_INDEX_MASK_ALL   = (MCFG_REFRESH_INDEX_MASK_0 |
                                   MCFG_REFRESH_INDEX_MASK_1 |
                                   MCFG_REFRESH_INDEX_MASK_2),/**< mask for all */
} mcfg_refresh_index_mask_type;

typedef uint32 mcfg_refresh_group_type;  /**< reserved */

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_ORDER_E_TYPE
-------------------------------------------------------------------------------*/
typedef enum mcfg_refresh_order_e
{
  /* do not use for registration */
  MCFG_REFRESH_ORDER_INIT = 0,
  /* do not use for registration */
  MCFG_REFRESH_ORDER_MIN = 1,  
  /* Top */         
  MCFG_REFRESH_ORDER_10 = MCFG_REFRESH_ORDER_MIN, 

  MCFG_REFRESH_ORDER_20,  
     
  MCFG_REFRESH_ORDER_30,   
    
  MCFG_REFRESH_ORDER_40,
  /* Default ORDER for refresh registration */     
  MCFG_REFRESH_ORDER_50,  
                          
  MCFG_REFRESH_ORDER_60,  
     
  MCFG_REFRESH_ORDER_70, 
      
  MCFG_REFRESH_ORDER_80,  
  /* Bottom */
  MCFG_REFRESH_ORDER_90,     
  /* do not use for registration */
  MCFG_REFRESH_ORDER_MAX = MCFG_REFRESH_ORDER_90, 
} mcfg_refresh_order_e_type;  

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_TYPE_E_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_REFRESH_TYPE_MIN  = 0,  
  MCFG_REFRESH_TYPE_SLOT = MCFG_REFRESH_TYPE_MIN,   /**< refresh type for slot specific items */
  MCFG_REFRESH_TYPE_SUBS,                           /**< refresh type for subscription specific items */
  MCFG_REFRESH_TYPE_SLOT_N_SUBS,                    /**< refresh type for slot & sub specific items */
  MCFG_REFRESH_TYPE_MAX  = MCFG_REFRESH_TYPE_SLOT_N_SUBS,  
} mcfg_refresh_type_e_type;  

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_TYPE_E_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_REFRESH_UIM_ICCID = 0,   /**< ICCID update from MMGSDI */
  MCFG_REFRESH_UIM_IMSI,        /**< IMSI update from MMGSDI */
  MCFG_REFRESH_UIM_SESSION,     /**< SESSION update from MMGSDI */
} mcfg_refresh_uimdata_e_type;  

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_REFRESH_TYPE_E_TYPE
-------------------------------------------------------------------------------*/
typedef struct
{
  mcfg_refresh_type_e_type type;            /**< Refresh for slot or sub specific items */
  mcfg_refresh_index_type  slot_index;      /**< Slot index for this refresh */
  mcfg_refresh_index_type  sub_index;       /**< Subscription index for this refresh */
  uint32                   reference_id;    /**< Reserved. */
} mcfg_refresh_info_s_type;

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_REFRESH_STATUS_E_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_REFRESH_SUCCESS  = 0,  
  MCFG_REFRESH_STATUS_INTERNAL_ERROR = 1,       /**< refresh status for internal error */
  MCFG_REFRESH_STATUS_INCOMPAT_FEATURE_MODE,    /**< refresh status for incompatible feature mode error */
  MCFG_REFRESH_STATUS_INCOMPAT_DEVICE_MODE,     /**< refresh status for incompatible device mode error */
  MCFG_REFRESH_STATUS_SUB_IS_ACTIVE,            /**< refresh status for sub is still active error */
  MCFG_REFRESH_STATUS_INVALID_SUB,              /**< refresh status for invalid subscription error */
  MCFG_REFRESH_STATUS_INVALID_REFRESH_TYPE,     /**< refresh status for invalid refresh type error */
} mcfg_refresh_status_e_type;  

/* Client task is expected to copy over the p_info and put the refresh update into 
its own command queue */
typedef boolean (*mcfg_refresh_cb_fn)(mcfg_refresh_info_s_type *p_info);

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_REFRESH_REGISTRATION_S_TYPE
-------------------------------------------------------------------------------*/
typedef struct
{
  mcfg_refresh_order_e_type    order;        /**< Refresh order ID registed for refresh */
  mcfg_refresh_index_mask_type slot_mask;    /**< Mask for slot(s) to recieve refresh update */
  mcfg_refresh_index_mask_type sub_mask;     /**< Mask for sub(s) to recieve refresh update */
  mcfg_refresh_group_type      group;        /**< Reserved. */
  mcfg_refresh_cb_fn           cb;           /**< Callback function to receive refresh update */
} mcfg_refresh_registration_s_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_REFRESH_DATA_TYPE
-------------------------------------------------------------------------------*/
typedef struct 
{
  int32   data_len;    /**< Length of the data. */
  uint8  *data_ptr;    /**< Pointer to the data. */
} mcfg_refresh_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MCFG_REFRESH_UIMDATA_S_TYPE
-------------------------------------------------------------------------------*/
typedef struct
{
  mcfg_refresh_uimdata_e_type type;         /**< Type of update from MMGSDI */
  mcfg_refresh_index_type     slot_index;   /**< Slot index for update from MMGSDI */
  mcfg_refresh_index_type     sub_index;    /**< Subscription index for update from MMGSDI */
  mcfg_refresh_data_type      iccid;        /**< ICCID info from MMGSDI */
  mcfg_refresh_data_type      imsi;         /**< IMSI info from MMGSDI */
  uint32                      reference_id; /**< Reference ID */
} mcfg_refresh_uimdata_s_type;

/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/* ==========================================================================
FUNCTION:       MCFG_REFRESH_REGISTER
========================================================================== */
/**
  This function is used to register with MCFG to receive refresh update through
  registered callback function for MCFG switching
 
  @param[in] p_reg:  Registration info

  @return
  TRUE for success, otherwise FALSE

  @dependencies
  The client must have a valid client ID.
*/
boolean mcfg_refresh_register
(
  mcfg_refresh_registration_s_type *p_reg
);

/* ==========================================================================
FUNCTION:       MCFG_REFRESH_DONE
========================================================================== */
/**
  This function is used to notify MCFG task that registered client has
  completed refresh operation. The client is expected to return the same
  reference id passed through registered callback
 
  @param[in]reference_id:  reference_id

  @return
  TRUE for success, otherwise FALSE

  @dependencies
  None
*/
boolean mcfg_refresh_done
(
  uint32 reference_id
);

/* ==========================================================================
FUNCTION:       MCFG_REFRESH_DONE_W_STATUS
========================================================================== */
/**
  This function is used to notify MCFG task that registered client has
  completed refresh operation. The client is expected to return the same
  reference id passed through registered callback with status
 
  @param[in]reference_id:  reference_id
        [in]status: status

  @return
  TRUE for success, otherwise FALSE

  @dependencies
  None
*/
boolean mcfg_refresh_done_w_status
(
  uint32 reference_id,
  mcfg_refresh_status_e_type status
);

/* ==========================================================================
FUNCTION:       mcfg_refresh_update_from_uim
========================================================================== */
/**
  This function is used for UIM/MMGSDI to notify MCFG task for ICCID, IMSI
  or session update
 
  @param[in] p_data:  uimdata

  @return
  TRUE for success, otherwise FALSE

  @dependencies
  None
*/
boolean mcfg_refresh_update_from_uim
(
  mcfg_refresh_uimdata_s_type *p_data
);

/* ==========================================================================
FUNCTION:       MCFG_NOTIFY_DEACTIVATE_SUCCESS
========================================================================== */
/**
  This function is used for MMOC to notify MCFG task of inactive state for
  specified subscription
 
  @param[in] sub:  mask for sub(s) being deactivated

  @return
  TRUE for success, otherwise FALSE

  @dependencies
  None
*/
boolean mcfg_notify_deactivate_success
(
  mcfg_refresh_index_mask_type sub_mask
);

/* ==========================================================================
FUNCTION:       MCFG_REFRESH_AUTOSELECT_ENABLED
========================================================================== */
/**
  Checks whether UIM-based automatic MCFG_SW selection is enabled.
 
  @param[in] slot:  Slot index 

  @return
  TRUE for success, otherwise FALSE

  @dependencies
  None
*/
boolean mcfg_refresh_autoselect_enabled
(
  mcfg_refresh_index_type slot_index
);

#endif /* _MCFG_REFRESH_H_ */