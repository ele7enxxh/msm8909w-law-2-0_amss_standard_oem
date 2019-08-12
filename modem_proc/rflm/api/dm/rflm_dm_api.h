/*!
  @file
  rflm_dm_api.h

  @brief
  This header file contains the RFLM DM(data manager) API

*/

/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/dm/rflm_dm_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/14   aw      fixed device_id typo and added rflm_dm_store_device_info
07/29/14   cj      Added changes for Tx sharing feature
05/22/14   rmb     Add disassociation of script buffer API.
04/02/2014 qwu     Reduce DM Padding Size
02/11/14   pl      Add disassociation API
02/07/14   jc      Increased tech buf size
12/12/13   ka      Add enum support for FBRx
11/26/13   ra      Add support for Common usage
11/25/13   Saul    CCS. New CSS Data Buffer Allocation For All Techs.
11/19/13   cj      Added IRAT script type
11/07/13   cvd     Added declaration for  rflm_dm_validate_handle_type
10/18/13   qwu     Added multiple script buffers
10/09/13   ka      Add tech data action enum
09/25/13   JJ      Remove one item in rflm_dm_usr_e
09/11/13   ka      Added fbrx DM support
09/04/13   gy      Added definition for rflm_dm_get_rxagc_buf_ptr
08/29/13   sty     Added definitions for rflm_dm_get/set_rxagc_data
08/01/13   jyw     Rename the user data with handle tech data
07/26/13   jyw     Created the initial version
===========================================================================*/

#ifndef RFLM_DM_API
#define RFLM_DM_API


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM)) 
#error code not present
#endif
  #include <atomic_ops.h>
#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM)) 
#error code not present
#endif

/*===========================================================================
						   CONSTANTS
===========================================================================*/
#include "rflm_dm_modem_defs.h"

#define TECH_DATA_POOL_SIZE                RFLM_HANDLE_COUNTS
#define TECH_DATA_ACCESS_HISTORY_SIZE      4
#define BIN_SIZE_IN_WORD                   8

/* script buffer filling pattern */
#define SCRIPT_BUF_FILL_PATTERN            0x5a5a5a5a

#define RFLM_DM_INVALID_HANDLE_ID -1

#define RFLM_INVALID_DEVICE -1

#define GET_SCRIPT_PAYLOAD_PTR_FROM_META_PTR(buf_ptr) (uint32 *)(buf_ptr + sizeof (rflm_dm_script_buf_params_s)/4)
#define GET_SCRIPT_META_PTR_FROM_PAYLOAD_PTR(payload_ptr) (uint32 *) (payload_ptr - sizeof (rflm_dm_script_buf_params_s)/4)
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief RFLM handle id type
*/
typedef int32 rflm_dm_handle_id_t;


/*! @brief RFLM handle device id type
*/
typedef uint32 rflm_dm_device_id_t; 

#ifdef FEATURE_TX_SHARING
/*! @brief RFLM handle sub id type
*/
typedef uint32 rflm_dm_sub_id_t; 

#endif

/*!< @brief we need some ID to indentiy which script has been executed */
typedef void (*script_exe_callback_func_t)(int32 idx, void *arg);

/*! @brief RFLM handle status enum
*/
typedef enum 
{
  RFLM_HANDLE_STATUS_UNINIT = 0,
  RFLM_HANDLE_STATUS_ACTIVE,
  RFLM_HANDLE_MAXNUM_STATUS
} rflm_dm_handle_status_e;


/*! @brief RFLM handle type enum
*/
typedef enum
{
  RFLM_RX_HANDLE = 0,
  RFLM_TX_HANDLE,
  RFLM_IRAT_HANDLE,
  RFLM_MAX_NUM_HANDLE_TYPES
} rflm_dm_handle_type_e;

/*! @brief RFLM DM running mode
*/
typedef enum
{
  RFLM_DM_DEFAULT = 0,
  RFLM_DM_MAX_MODES
} rflm_dm_cfg_mode_e;


/*! @brief The script format associated with a RFLM handle; used in the meta
*/
typedef enum
{
  SCRIPT_FMT_ISS_SEQ= 0,
  SCRIPT_FMT_DEVICE_FMT,
  SCRIPT_MAX_NUM_FMTS
}rflm_dm_script_fmt_e;


/*! @brief The script type when associated with a RFLM handle; used in the meta
*/
typedef enum
{
  SCRIPT_TYPE_RX_TUNE = 0,
  SCRIPT_TYPE_TX_TUNE,
  SCRIPT_TYPE_PA,
  SCRIPT_TYPE_LNA,
  SCRIPT_TYPE_WXE_TRIGGER,
  SCRIPT_TYPE_WXE_PRELOAD,
  SCRIPT_TYPE_WXE_CMD = SCRIPT_TYPE_WXE_TRIGGER, // TODO: Need to remove
  SCRIPT_TYPE_WXE = SCRIPT_TYPE_WXE_PRELOAD, // TODO: Need to remove
  SCRIPT_TYPE_COMMON,
  SCRIPT_MAX_NUM_TYPES
}rflm_dm_script_type_e;


/*! @brief General user ID used in the DM
*/
typedef enum
{
  RFLM_NO_USR = 0,
  RFLM_USR_LTE,
  RFLM_USR_CDMA,
  RFLM_USR_C2K = RFLM_USR_CDMA,
  RFLM_USR_EVDO,
  RFLM_USR_GSM,
  RFLM_USR_UMTS,
  RFLM_USR_TDSCDMA,
  RFLM_USR_COMMON,
  RFLM_USR_FBRX,
  RFLM_MAX_NUM_USRS
} rflm_dm_usr_e;

/*! @brief Action enum used to get tech data
*/
typedef enum
{
  RFLM_DM_TECH_DATA_NO_ACTION = 0,
  RFLM_DM_TECH_DATA_GET_PTR   = 1,
  RFLM_DM_TECH_DATA_GET_DATA  = 2
} rflm_dm_get_tech_data_action_e;


/*! @brief RFLM handle meta data struct @todo add more supporting data
*/
typedef struct
{
  /*!< external user data */
  boolean external_user_data;

  /*!< Handle owner */
  rflm_dm_usr_e owner;

  /*!< Handle type */
  rflm_dm_handle_type_e type;

  /*!< Device ID */
  union
  {
	  rflm_dm_device_id_t device_id;
	  rflm_dm_device_id_t devide_id; /* DO NOT use devide_id. Keep it just to compile existing code */
  };

#ifdef FEATURE_TX_SHARING
  /*!< subscriber_id */
  rflm_dm_sub_id_t sub_id;
#endif
  
}rflm_dm_handle_meta_s;


/*! @brief Script meta info structure type saved in the DM along with the script
    content */
typedef struct
{
  // IMPORTANT!: FIXED location - DO NOT change ordering in this structure (Implemention depends on the ordering)
  /*!< Actual size of allocated buff; DM will update it after allocation and use it to locate a buff.*/
  uint16 buff_size_in_byte;

  /*!< array idx in one script buffer pool; Used by DM internally*/
  uint16 array_idx;

  /*!< actual script usage;*/
  uint16 actual_size_in_byte;

  /*!< requested MAX size and used to decide a proper buffer; Provided by user */
  uint16 size_in_byte;

  /*!< Initialized to -1 when allocated; will be auto-updated when the script is associated with a handle*/
  int16 handle_id;                              

  /*!< After PLL settling, FW needs this time and it is an estimate; Provided by user*/
  uint32 effect_time;

  /*!< desired event start time; provided by user */
  uint32 desired_start_time;

  /*!< desired event done time; provided by user */
  uint32 desired_done_time;

  /*!< actual event start time; currently not supported */
  uint32 act_start_time;

  /*!< actual event done time; updated by ISR callback */
  uint32 act_done_time;

  /*!< event seq id used by resource table; provideed by user(IRAT, etc.) */
  uint32 event_seq_id;

  /*!< resoure bit mask; provided by user (IRAT)*/
  uint32 resource_bitmask;

   /*!< priority; provided by user (IRAT)*/
  uint16 priority;

#ifdef RFLM_DM_SCRIPT_MULTI_ASSOCIATE_EN
  /*!< bit mask for associated handles; bit position == handle id  */
  atomic_word_t handle_bitmask;

  /*!< number of association; need to init to 0 when allocate */
  atomic_word_t associate_counter;

  /*!< store script index for each associated handle; bit pos == script index */
  atomic64_word_t script_idx_bitmask[RFLM_HANDLE_COUNTS];
#endif

  /*!< list of all event seq_id this event it depending on and will be initialized to -1 (N/A); provided by user.*/
  int32 dep_list[RFLM_DM_MAX_DEPENDENCY_NUM_PER_EVENT];

  /*!< Script buffer user; provided by user */
  rflm_dm_usr_e owner;
 
  /*!< RF script, lna script, pa template; provided by user */
  rflm_dm_script_type_e type;

  /*!< RFC format, issue-seq format, or others; provided by user */
  rflm_dm_script_fmt_e fmt;

} rflm_dm_script_meta_s; 

typedef struct
{
  uint32 max_time;
  uint32 min_time;
  uint32 average;

  /*!< To be expended ... */

} rflm_dm_script_profile_data_s;



/*! @brief script usage book keeping 
*/
typedef struct
{
  /*!< the most recent allocation time */
  // uint32 last_alloc_time;

  /*!< the most recent deallocation time */
  // uint32 last_dealloc_time;

  /*!<  the most recent association time */
  // uint32 last_link_time;
 
  /*!< the most recent execution time */
  // uint32 last_exe_time;

  /*!< counter for concurrent access */
  atomic_word_t access_count;

  /*!< Indicate if the buffer is in use */
  boolean is_in_use;
} rflm_dm_script_book_keep_info_s; 


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/


typedef struct 
{
  // FIXED location for pointer reference

  /*!< associated idx in the buffer array; this helps in the delete operation */
  // int32 script_id;

  /*!<  meta info updated at the request time */
  rflm_dm_script_meta_s meta; 

  /*!< call back function ptr */
  script_exe_callback_func_t cb_ptr;

  /*!< usaage book keeping infomation */
  rflm_dm_script_book_keep_info_s usage_info; 

  /* profiling struct */
  // rflm_dm_script_profile_data_s pdata;
}rflm_dm_script_buf_params_s;


/*! @brief Tech buf meta info structure type saved in the DM along with user tech
    buf */
typedef struct
{
  /*!< Tech buffer user */
  rflm_dm_usr_e owner;
 
  /*!< Requested size */
  uint32 size_in_byte;
} rflm_dm_tech_buf_meta_s; 

/*! @brief Script type enum is used to enable RFLM to locate the correct 
    prepopulated script in RFLM DM memory */
typedef enum
{
  RFLM_MEAS_DM_PRELOAD_SCRIPT,
  RFLM_MEAS_DM_TRIGGER_SCRIPT,
  RFLM_MEAS_DM_NONTRX_SCRIPT,
  RFLM_MEAS_MAX_DM_SCRIPT_TYPES
}rflm_meas_dm_script_type_e;



#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

  FUNCTION:  rflm_dm_init

===========================================================================*/
/*!
    @brief
    The function initialize the RFLM DM (data manager) module

    @detail
    The configuration mode be used in certain test cases in which RFSW 
    is not availabe

    @return
    none
*/
/*=========================================================================*/
extern void rflm_dm_init
(
  /* DM configuration mode */
  rflm_dm_cfg_mode_e mode
);


/*===========================================================================

  FUNCTION:  rflm_dm_allocate_script_buf

===========================================================================*/
/*!
    @brief
    The function allocated a new script buffer from the buffer pool.
  

    @detail
    The allocated script buffer can be associated with a RFLM handle but not 
    necessary. 

    @return
    The address ptr to the new script buffer; if NULL the allocation failed.
*/
/*=========================================================================*/
extern uint32* rflm_dm_allocate_script_buf
(
  /*!< the meta is needed to be filled by the requester, it includes the info
    of the size in byte*/
  rflm_dm_script_meta_s* input_meta_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_deallocate_script_buf

===========================================================================*/
/*!
    @brief
    The funciton de-allocate a script buf.
  

    @detail
    The deallocated script buf goes back to the free buffer pool.

    @return
    boolean indicating if the deallocation is successful.
*/
/*=========================================================================*/
extern boolean rflm_dm_deallocate_script_buf
(
  /* The buffer starting address to be deallocated */
  uint32* buf_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_allocate_handle

===========================================================================*/
/*!
    @brief
    The function allocates an RFLM handle. The handle info is descrbied in the meta
    pointer.

    @detail

    @return
    The handle_id is returned. if -1 returned, then allocation is failed
*/
/*=========================================================================*/
extern rflm_dm_handle_id_t rflm_dm_allocate_handle
(
  /* The meta data associated with new handle */
  rflm_dm_handle_meta_s *meta_ptr
);



/*===========================================================================

  FUNCTION:  rflm_dm_deallocate_handle

===========================================================================*/
/*!
    @brief
    The function de-allocate an RFLM handle. 

    @detail
    The de-allocated handle is returned in the free handle pool.

    @return
    Boolean indicating if the deallocation is successful. The deallocation to a 
    inactive handle or invalid handle returns FALSE.
*/
/*=========================================================================*/
extern boolean rflm_dm_deallocate_handle
(
  /*!< The handle to be released */
  rflm_dm_handle_id_t handle_id 
);

/*===========================================================================

  FUNCTION:  rflm_dm_deallocate_handle_and_scripts

===========================================================================*/
/*!
    @brief
    The function de-allocate an RFLM handle and all its scripts. 

    @detail
    The de-allocated handle is returned in the free handle pool and all its scripts
	are removed as well.

    @return
    Boolean indicating if the deallocation is successful. The deallocation to a 
    inactive handle or invalid handle returns FALSE.
*/
/*=========================================================================*/
extern boolean rflm_dm_deallocate_handle_and_scripts
(
  /*!< The handle to be released */
  rflm_dm_handle_id_t handle_id 
);

/*===========================================================================

  FUNCTION:  rflm_dm_associate_script

===========================================================================*/
/*!
    @brief
    The funciton associates a script to an active RFLM handle identified by the
    handl_id.

    @detail
    The cb_ptr points to the function to be executed after the CCS has dispatched 
    the script.

    @return
    Boolean indicating if the association is successful. if the RFLM handle is
    invalid or inactive then returns FALSE.
*/
/*=========================================================================*/
extern boolean rflm_dm_associate_script
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Script buffer ptr */
  uint32 *buf_ptr, 

  /*!< The location index of the script in the script arrary */
  uint32 script_idx, 

  /*! Script execution call back  */
  script_exe_callback_func_t cb_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_disassociate_script

===========================================================================*/
/*!
    @brief
    The funciton disassociates a script from an active RFLM handle identified by the
    handl_id.

    @detail

    @input
    rflm_dm_handle_id_t: handle ID
    uint32*: pointer to payload
    uint32: script id

    @return
    Boolean indicating if the disassociation is successful. 
*/
/*=========================================================================*/
extern boolean rflm_dm_disassociate_script
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Script buffer ptr */
  uint32 *payload_ptr, 

  /*!< The location index of the script in the script arrary */
  uint32 script_idx 
);


/*===========================================================================

  FUNCTION:  rflm_dm_disassociate_and_deallocate_script

===========================================================================*/
/*!
    @brief
    The funciton disassociates a script to an active RFLM handle identified by the
    handl_id.

    @detail
    The cb_ptr points to the function to be executed after the CCS has dispatched 
    the script.

    @return
    Boolean indicating if the disassociation is successful. if the RFLM handle is
    invalid or inactive then returns FALSE.
*/
/*=========================================================================*/
extern boolean rflm_dm_disassociate_and_deallocate_script
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Script buffer ptr */
  uint32 *buf_ptr, 

  /*!< The location index of the script in the script arrary */
  uint32 script_idx
);

/*===========================================================================

  FUNCTION:  rflm_dm_get_handle_info

===========================================================================*/
/*!
    @brief
    The function returns the meta info of the handle referenced by the handle id.
  

    @detail
    The function is expcted to be used only for the debug purpose or unit test
    purpose.

    @return
    Boolean indicating if the read is successful, FALSE for an invalid handle_id
*/
/*=========================================================================*/
extern boolean rflm_dm_get_handle_info
(
  /*!< Handle ID */
  rflm_dm_handle_id_t handle_id,

  /*!< Output meta info about the handle */
  rflm_dm_handle_meta_s* output_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_validate_handle_type

===========================================================================*/
/*!
    @brief
    The function validates that the handle referenced by handle id is of the
    correct handle type.
  
    @detail
    The function compares handle_id's rflm_dm_handle_meta_s.type against handle_type
    and returns TRUE if they are the same and FALSE otherwise.
    

    @return
    TRUE if handle_id's rflm_dm_handle_meta_s.type is the same as handle_type.
    FALSE otherwise.
*/
/*=========================================================================*/
extern boolean rflm_dm_validate_handle_type
(
  /*!< Handle ID */
  rflm_dm_handle_id_t handle_id,

  /*!< Handle Type */
  rflm_dm_handle_type_e handle_type
);



/*===========================================================================

  FUNCTION:  rflm_dm_get_script_buf_ptr 

===========================================================================*/
/*!
    @brief
    The function returns the script buffer address based on the handle_id 
    and the script index.

    @detail

    @return
    if handle_id or script_idx is invalid, NULL is returned otherwise the
    script buffer payload address is returned.
*/
/*=========================================================================*/
extern uint32* rflm_dm_get_script_buf_ptr
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 
  /*!< script index */
  uint32 script_idx
);

extern boolean rflm_dm_validate_script_buf
(
  uint32* payload_ptr
);


extern boolean rflm_dm_set_script_buf_cb
(
  uint32* payload_ptr,
  script_exe_callback_func_t cb_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_allocate_tech_buf

===========================================================================*/
/*!
    @brief
    The function allocates a new tech buffer
  
    @detail

    @return
    The ptr to the new tech buffer; if NULL the allocation failed.
*/
/*=========================================================================*/
extern uint32* rflm_dm_allocate_tech_buf
(
  /*!< the meta is needed to be filled by the requester, it includes the info
    of the size in byte*/
  const rflm_dm_tech_buf_meta_s* input_meta_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_deallocate_tech_buf

===========================================================================*/
/*!
    @brief
    The funciton de-allocates a tech buffer.
  

    @detail

    @return
    Boolean indicating if the de-allocation is successful.
*/
/*=========================================================================*/
extern boolean rflm_dm_deallocate_tech_buf
(
  /*!< Tech buffer ptr */
  uint32* buf_ptr 
);


/*===========================================================================

  FUNCTION:  rflm_dm_set_handle_tech_data

===========================================================================*/
/*!
    @brief
    The function writes to the tech data per handle
  

    @detail
    The act enum defines the corresponsing actions

    @return
    Boolean indicating if the write is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_set_handle_tech_data
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Output user data ptr */
  const void *output_tech_data_ptr,

  /*!< Action enum per technology */
  uint32 act_enum 
); 


/*===========================================================================

  FUNCTION:  rflm_dm_get_handle_tech_data

===========================================================================*/
/*!
    @brief
    The funciton reads the tech data from the handle container.
  

    @detail
    The act enumeration defines the corresponsing actions; It is defined per
    technology.

    @return
    Boolean indicating if the read is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_get_handle_tech_data
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Input user data ptr */
  void *input_tech_data_ptr,

  /*!< Action enum per technology */
  uint32 act_enum 
); 


/*===========================================================================

  FUNCTION:  rflm_dm_set_txagc_data

===========================================================================*/
/*!
    @brief
    The function writes the common TX agc data from the RFSW 

    @detail
    The act enum defines the corresponsing actions; the action enum is defined
    by the common txagc module.

    @return
    boolean indicating if the write is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_set_txagc_data
(

  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Action enum */
  uint32 act_enum,

  /*!< void pointer to the data */
  const void *input_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_get_txagc_data

===========================================================================*/
/*!
    @brief
    The function reads the common TX agc data from the RFLM handle container.
  

    @detail
    The action enum defines the corresponsing actions

    @return
    boolean indicating if the read is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_get_txagc_data
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Output data ptr*/
  void *output_ptr,

  /*!< Action enum */
  uint32 act_enum
);


/*===========================================================================

  FUNCTION:  rflm_dm_get_tx_agc_ptr

===========================================================================*/
/*!
    @brief
    The function return the txagc handle address from the handle pool based on 
    the handle index 

    @detail

    @return
    txagc handle address pointer stored in the handle pool
*/
/*=========================================================================*/
extern void* rflm_dm_get_tx_agc_ptr
(
  /* RFLM handle ID */
  rflm_dm_handle_id_t handle_id 
);


/*===========================================================================

  FUNCTION:  rflm_dm_get_dtr_buf_addr

===========================================================================*/
/*!
    @brief
    The function returns the DTR buffer address from the handle pool based on 
    the RFLM handle id

    @detail

    @return
    DTR buffer address; if NULL, the handle_id is not valid
*/
/*=========================================================================*/
extern uint32* rflm_dm_get_dtr_buf_addr
(
  /* RFLM handle ID */
  rflm_dm_handle_id_t handle_id 
);

/*===========================================================================

  FUNCTION:  rflm_dm_get_txagc_data

===========================================================================*/
/*!
  @brief
  The function reads the common RX AGC data from the RFLM handle container.
  
  @detail
  The action enum defines the corresponsing actions

  @return
  boolean indicating if the read is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_get_rxagc_data
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Action enum */
  uint32 act_enum,

  /*!< Output data ptr*/
  void *output_ptr
);

/*===========================================================================

  FUNCTION:  rflm_dm_set_rxagc_data

===========================================================================*/
/*!
  @brief
  API called when data in RxAGC module needs to be set

  @detail
  The act_enum defines the corresponsing actions - enums defined in RxAGC
  module 

  @return
  TRUE on success and FALSE on failure
*/
/*=========================================================================*/
extern boolean rflm_dm_set_rxagc_data
(

  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Action enum */
  uint32 act_enum,

  /*!< void pointer to the data */
  const void *input_ptr
);

/*===========================================================================

  FUNCTION:  rflm_dm_get_rxagc_buf_ptr

===========================================================================*/
/*!
    @brief
    The function return the rxagc handle address from the handle pool based on 
    the handle index 

    @detail

    @return
    rxagc handle address pointer stored in the handle pool
*/
/*=========================================================================*/
extern void* rflm_dm_get_rxagc_buf_ptr
(
  /* RFLM handle ID */
  rflm_dm_handle_id_t handle_id 
);

#ifndef FEATURE_D3925_FED
/*===========================================================================

  FUNCTION:  rflm_dm_get_fbrx_data

===========================================================================*/
/*!
  @brief
  The function reads the common FBRx data from the RFLM handle container.
  
  @detail
  The action enum defines the corresponsing actions

  @return
  boolean indicating if the read is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_get_fbrx_data
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Action enum */
  uint32 act_enum,

  /*!< Output data ptr*/
  void *output_ptr
);

/*===========================================================================

  FUNCTION:  rflm_dm_set_fbrx_data

===========================================================================*/
/*!
  @brief
  API called when data in FBRx module needs to be set

  @detail
  The act_enum defines the corresponsing actions - enums defined in FBRx
  module 

  @return
  TRUE on success and FALSE on failure
*/
/*=========================================================================*/
extern boolean rflm_dm_set_fbrx_data
(

  /*!< RFLM handle ID */
  rflm_dm_handle_id_t handle_id, 

  /*!< Action enum */
  uint32 act_enum,

  /*!< void pointer to the data */
  const void *input_ptr
);

/*===========================================================================

  FUNCTION:  rflm_dm_get_fbrx_buf_ptr

===========================================================================*/
/*!
    @brief
    The function return the fbrx handle address from the handle pool based on 
    the handle index 

    @detail

    @return
    fbrx handle address pointer stored in the handle pool
*/
/*=========================================================================*/
extern void* rflm_dm_get_fbrx_buf_ptr
(
  /* RFLM handle ID */
  rflm_dm_handle_id_t handle_id 
);
#endif

extern boolean rflm_dm_set_autopin_data
(

   /*!< RFLM handle ID */
   rflm_dm_handle_id_t handle_id, 

   /*!< Action enum */
   uint32 act_enum,

   /*!< void pointor to the data */
   const void *input_ptr
);

extern boolean rflm_dm_get_autopin_data
(

   /*!< RFLM handle ID */
   rflm_dm_handle_id_t handle_id, 

   /*!< Action enum */
   uint32 act_enum,

   /*!< void pointor to the data */
   void *output_ptr
);


extern boolean rflm_dm_dup_autopin_data
(
  /*!< RFLM handle ID */
  rflm_dm_handle_id_t new_handle_id, 

  rflm_dm_handle_id_t old_handle_id
);  


extern void* rflm_dm_get_autopin_buf_ptr
(
  /* RFLM handle ID */
  rflm_dm_handle_id_t handle_id 
);


/*===========================================================================

  FUNCTION:  rflm_dm_get_script_meta

===========================================================================*/
/*!
    @brief
    The funciton returns script meta pointer based on handle_id and script_id.
  
    @detail

    @input
    uint32:  handle_id
    uint32:  script_id

    @return
    rflm_dm_script_meta_s *: Pointer to the script meta.
*/
/*=========================================================================*/
extern rflm_dm_script_meta_s * rflm_dm_get_script_meta
(
  rflm_dm_handle_id_t handle_id, 
  uint32 script_id
); 


/*===========================================================================

  FUNCTION: rflm_dm_allocate_script_buf_for_handle 

===========================================================================*/
/*!
    @brief
    The function allocated a new script buffer and associate it with a handle.
  

    @detail
   
    @input 
    rflm_dm_script_meta_s*: script buffer meta info
    rflm_dm_handle_id_t: handle_id
    uint32: script idx, 
    script_exe_callback_func_t:  cb_ptr
    
    @return
    uint32 *: payload pointer
*/
/*=========================================================================*/
extern uint32 * rflm_dm_allocate_script_buf_for_handle
(
   /*!< the meta is needed to be filled by the requester, it includes the info
	 of the size in byte*/
   rflm_dm_script_meta_s* input_meta_ptr,
    /*!< RFLM handle ID */
   rflm_dm_handle_id_t handle_id, 
   /*!< The location index of the script in the script arrary */
   uint32 script_idx, 
   /*! Script execution call back  */
   script_exe_callback_func_t cb_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_set_handle_info

===========================================================================*/
/*!
    @brief
    The function updates the meta info of the handle referenced by the handle id.
  
    @input
    rflm_dm_handle_id_t: handle_id,
    rflm_dm_handle_meta_s *: meta_ptr

    @return
    Boolean indicating if the update is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_set_handle_info
(
  /*!< Handle ID */
  rflm_dm_handle_id_t handle_id,

  /*!< Input meta info about the handle*/
  rflm_dm_handle_meta_s * meta_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_script_buf_access_start_indicate

===========================================================================*/
/*!
    @brief
    The function is to detect and prevent concurrent access to a script buf.
	It indicates if it is Ok to access script buf, 
	and increase access count when Ok to access.

  
    @input
    uint32 *: script payload ptr

    @return
    Boolean: indicating if Ok to access script buf
*/
/*=========================================================================*/
extern boolean rflm_dm_script_buf_access_start_indicate(
   uint32 *script_payload_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_script_buf_access_end_indicate

===========================================================================*/
/*!
    @brief
    The function is to decrease access count when access is done.

    @input
    uint32 *: script payload ptr

    @return
*/
/*=========================================================================*/
extern void rflm_dm_script_buf_access_end_indicate(
   uint32 * script_payload_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_store_handle_tq

===========================================================================*/
/*!
    @brief
    The function store task queue pointer for handle
 
    @input
    rflm_dm_handle_id_t: handle id
    void *: tq_ptr

    @return
    Boolean indicating if the store is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_store_handle_tq
(
  /*!< Handle ID */
  rflm_dm_handle_id_t handle_id,

  /*!< input taskq pointer */
  void * tq_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_get_handle_tq

===========================================================================*/
/*!
    @brief
    The function returns task queue pointer for handle
  

    @detail
    @input
    rflm_dm_handle_id_t: handle id

    @return
    void *: task queue handle.
			  callers need to cast to rflm_ccs_rf_tq_handle_t *
*/
/*=========================================================================*/
extern  void * rflm_dm_get_handle_tq
(
  /*!< Handle ID */
  rflm_dm_handle_id_t handle_id
);
   

/*===========================================================================

  FUNCTION:  rflm_dm_validate_handle

===========================================================================*/
/*!
    @brief
    The function validates handle id.
  

    @detail

    @return
    Boolean: indicating if the handle_id is valid
*/
/*=========================================================================*/
extern boolean rflm_dm_validate_handle
(
 /*!< Handle ID */
  rflm_dm_handle_id_t handle_id
);

extern void rflm_dm_set_device_id
(
 /*!< Handle ID */
  rflm_dm_handle_id_t handle_id,
 /*!< Device ID */
  int32 device_id
);

extern int32 rflm_dm_get_device_id
(
 /*!< Handle ID */
  rflm_dm_handle_id_t handle_id
);

extern void rflm_dm_set_populated_flag
(
 /*!< Handle ID */
  rflm_dm_handle_id_t handle_id,
 /*!< Populated flag */
  boolean populated_flag
);

extern boolean rflm_dm_get_populated_flag
(
 /*!< Handle ID */
  rflm_dm_handle_id_t handle_id
);

#ifdef RFLM_DM_SCRIPT_MULTI_ASSOCIATE_EN
/*===========================================================================

  FUNCTION:  rflm_dm_script_buf_get_associate_count

===========================================================================*/
/*!
    @brief
    The function is to return associate count

    @input
    uint32 *: script payload ptr

    @return
    uint32: associate count
*/
/*=========================================================================*/
extern uint32 rflm_dm_script_buf_get_associate_count
(
  uint32 * script_payload_ptr 
);

/*===========================================================================

  FUNCTION:  rflm_dm_script_buf_get_handle_associate_bitmask

===========================================================================*/
/*!
    @brief
    The function is to return handle_associate_bitmask

    @input
    uint32 *: script payload ptr

    @return
    uint32: handle_associate_bitmask
*/
/*=========================================================================*/
extern uint32 rflm_dm_script_buf_get_handle_associate_bitmask
(
   uint32 * script_payload_ptr
);


/*===========================================================================

  FUNCTION:  rflm_dm_script_buf_get_script_associate_bitmask_for_handle

===========================================================================*/
/*!
    @brief
    The function is to return handle_associate_bitmask

    @input
    uint32 *: script payload ptr
	rflm_dm_handle_id_t: handle_id

    @return
    uint32: handle_associate_bitmask
*/
/*=========================================================================*/
extern uint64 rflm_dm_script_buf_get_script_associate_bitmask_for_handle
(
   uint32 * script_payload_ptr,
   rflm_dm_handle_id_t handle_id
);

/*===========================================================================

  FUNCTION:  rflm_dm_store_device_info

===========================================================================*/
/*!
    @brief
    The function stores task device info for handle
 
    @input
    rflm_dm_handle_id_t: handle id
    rflm_dm_device_id_t: device id
    rflm_dm_sub_id_t   : subscriber id

    @return
    Boolean indicating if the store is successful
*/
/*=========================================================================*/
extern boolean rflm_dm_store_device_info
(
  /*!< Handle ID */
  rflm_dm_handle_id_t handle_id,

  /*!<input device id */
  rflm_dm_device_id_t dev_id,

  /*!<input sub id */
  rflm_dm_sub_id_t sub_id
);


#endif

#ifdef __cplusplus
}
#endif

#endif /* RFLM_DM_API */
