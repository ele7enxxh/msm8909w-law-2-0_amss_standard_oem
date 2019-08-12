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

  Copyright (c) 2009-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_pdn_limit_hdlr.c#1 $

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

#ifdef FEATURE_DATA_LTE
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "modem_mem.h"
#include "mcfg_fs.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_eps_pdn_context.h"
#include "ds_3gpp_apn_table.h"
#include "ds3gmmgsdiif.h"
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define DEF_PDN_REQ_THROTTLING_TIME 15 /*Minutes*/

#define DS_3GPP_PDN_LIMIT_THROT_DIR_NAME "pdn_limit_throttle"

const char* ds_3gpp_pdn_limit_throttle_file_name[DS3GSUBSMGR_SUBS_ID_MAX] = 
            {"pdn_limit_throttle_params.txt","pdn_limit_throttle_params_1.txt"};

#define DS_3GPP_PDN_LIMIT_THROTTLE_FILE_NAME(ds_subs_id) \
          ds_3gpp_pdn_limit_throttle_file_name[ds_subs_id]

#define DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME 90000 /*EFS Flush Time*/
#define DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE 0xADBCCBDA

/* This structure stores PDN Connection Limit Information for a particular APN.
 
  max_pdn_conn_per_blk: Specifies the maximum number of PDN connections in a
                        time block that the UE is allowed to perform with
                        the network.
 
  max_pdn_conn_time:    Specifies the time duration in seconds during which
                        the UE counts the PDN connections already made.
 
  pdn_req_wait_time:    Specifies the minimum time interval between the new
                        PDN connection request and the last successful UE
                        initiated PDN disconnection.
 
  time_blk_size:        Specifies the time block duration during which at
                        maximum max_pdn_conn_per_blk successful PDN connection
                        requests are allowed to make for a given PDN from the
                        UE.
 
  pdn_conn_cntr:        This counter counts the number of successful PDN
                        connections since the PDNConnTimer is started.
 
  pdn_conn_tmr:         Timer which will be set to max_pdn_conn_time. 
 
*/

typedef struct
{
  byte              apn[DS_UMTS_MAX_APN_STRING_LEN+1]; /* Apn name */

  uint16            max_pdn_conn_per_blk; 
  uint16            max_pdn_conn_time; 
  uint16            pdn_req_wait_time;  
  uint16            time_blk_size;
  uint16            pdn_conn_cntr;
  rex_timer_type    pdn_conn_tmr;
  void*             client_data_ptr;
}ds_3gpp_pdn_limit_dyn_tbl_s;

typedef struct
{
  ds_3gpp_pdn_limit_dyn_tbl_s* pdn_limit_dyn_tbl[DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES];
  void*                        client_data_ptr;
} ds_3gpp_pdn_limit_tbl_s; 

typedef struct
{
  void   *data_ptr;
  uint32  data_len;
} ds_3gpp_pdn_limit_efs_data_buf_type;

/*=========================================================================== 
  This is the main structure which stores PDN Conn Limit information for all
  APNs for which PDN Conn Timer, PDN Wait Timer or PDN Throttling Timer is
  running.
=============================================================================*/
typedef struct
{
  ds_3gpp_pdn_limit_efs_info_s efs_info;
  ds_3gpp_pdn_limit_tbl_s      *pdn_limit_tbl;
}ds_3gpp_pdn_limit_info_s;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

static ds_3gpp_pdn_limit_info_s *ds_3gpp_pdn_limit_info_p[DS3GSUBSMGR_SUBS_ID_MAX]={NULL};

static ds_3gpp_pdn_limit_throt_info_type 
                   *ds_3gpp_pdn_limit_throt_info_ptr [DS3GSUBSMGR_SUBS_ID_MAX] = {NULL};

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_ALLOC_DYN_MEM

  DESCRIPTION
  This function allocates an entry in the PDN Limit Table.
  
  PARAMETERS  
  None.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index of the entry into the PDN Limit Table
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint8 ds_3gpp_pdn_limit_alloc_dyn_mem
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_DYN_MEM

  DESCRIPTION
  This function frees an entry in the PDN Limit Table.
  
  PARAMETERS  
  Index of the entry into the PDN Limit Table
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_dyn_mem
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Connection Timer Expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_pdn_conn_tmr_expiry_cb
(
  unsigned long callback_data
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_CONN_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Connection Timer expiry event.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_process_pdn_conn_tmr_expiry_cmd
(
  ds_3gpp_pdn_limit_dyn_tbl_s *pdn_limit_dyn_tbl_entry_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Req Throttling Timer Expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_pdn_req_throt_tmr_expiry_cb
(
  void* throt_sm_ptr,
  void* callback_data
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_REQ_THROT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Req Throttle Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_process_pdn_req_throt_tmr_expiry_cmd
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Wait Timer expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_pdn_wait_tmr_expiry_cb
(
  void* throt_sm_ptr,
  void* callback_data
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_WAIT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Wait Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_process_pdn_wait_tmr_expiry_cmd
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_PDN_REQ_THROTTLING_IN_PROGRESS

  DESCRIPTION
  This function checks whether the PDN Req Throttling Timer is running.
  
  PARAMETERS  
  Index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE or FALSE based on whether PDN is throttled
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_pdn_limit_is_pdn_req_throttling_in_progress
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_CURRENT_TIME_BLK

  DESCRIPTION
  This function returns the Current time block based on the Current
  PDN Connection Timer Value.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Time in milli-secs for which the PDN Conn Timer has run.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current Timer Block
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_current_time_blk
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id,
  uint32                 time_in_msec
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_PDN_WAIT_TMR_VAL

  DESCRIPTION
  This function returns the PDN Wait Timer Value for a given APN.
  
  PARAMETERS  
  callback data - Throttle SM Ptr from which we obtain APN information
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Wait Timer Value in milli-seconds
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_get_pdn_wait_tmr_val
(
  void* throt_sm_void_ptr,
  void* callback_data
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_THROTTLING_TMR_VAL

  DESCRIPTION
  This function returns the PDN Req Throttling Timer value.
  
  PARAMETERS  
  callback data 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Req Throttle Timer Value in milli-seconds
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_get_throttling_tmr_val
(
  void *throt_sm_ptr,
  void* cb_data
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_TIME_BLK_SIZE

  DESCRIPTION
  This function fetches the time block size in milli-seconds.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Time Block Size in millisecs.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_time_blk_size
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_PER_BLK

  DESCRIPTION
  This function fetches Max PDN Conn allowed per Block for a Given APN.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn allowed per Time Block.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

static uint16 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_per_blk
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_ALLOWED

  DESCRIPTION
  This function fetches Max PDN Conn allowed in MAX PDN Conn Time.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn Allowed in MAX PDN Conn Time.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_THROTTLING_TMR

  DESCRIPTION
  This function starts the PDN Req Throttling Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_start_throttling_tmr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_MAX_PDN_CONN_TIME

  DESCRIPTION
  This function gets thE Max PDN Conn Time in milli-seconds.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn Time in milli-seconds.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_get_max_pdn_conn_time
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_SUBS_ID

  DESCRIPTION
  This function fetches the subs id corresponding to a PDN Limit Dyn Tbl Entry
  
  PARAMETERS  
  PDN Limit Dyn Tbl Entry 
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Subs Id
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/

static sys_modem_as_id_e_type ds_3gpp_pdn_limit_get_subs_id
(
  ds_3gpp_pdn_limit_dyn_tbl_s *pdn_limit_dyn_tbl_entry_ptr 
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_TBL_ENTRY_PER_SUBS_ID

  DESCRIPTION
  This function fetches the PDN Limit Table Entry for a Particular subscription
  
  PARAMETERS  
  Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Limit Table Entry for a Particular subscription
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static ds_3gpp_pdn_limit_tbl_s* ds_3gpp_pdn_limit_get_tbl_entry_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_TBL_ENTRY_PER_SUBS_ID

  DESCRIPTION
  This function frees the PDN Limit Table Entry for a Particular subscription
  
  PARAMETERS  
  Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_tbl_entry_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_TBL_ENTRY_PER_APN

  DESCRIPTION
  This function fetches the PDN Limit Table Entry at a particular index and
  for a particular subscription
  
  PARAMETERS
  1. Index
  2. Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Limit Table Entry at a particular index and
  for a particular subscription
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static ds_3gpp_pdn_limit_dyn_tbl_s* ds_3gpp_pdn_limit_get_tbl_entry_per_apn
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_TBL_ENTRY_PER_APN

  DESCRIPTION
  This function frees the PDN Limit Table Entry at a particular index and
  for a particular subscription
  
  PARAMETERS
  1. Index
  2. Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_tbl_entry_per_apn
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_VALIDATE_CLEAR_ENTRY

  DESCRIPTION
  This function determines whether the entry corresponding to the given
  index and subs_id can be cleared from the PDN limit table.
  
  PARAMETERS
  1. Index into the PDN Limit Table.
  2. Subs id
 
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_pdn_limit_validate_clear_entry
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================


                               FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_CURRENT_PDN_CONN_TMR_VAL

  DESCRIPTION
  This function checks gets the current PDN Connection Timer Value.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current PDN Conn Timer Value.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  uint32                       current_timer_val;
  ds_3gpp_pdn_limit_dyn_tbl_s* per_apn_entry_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return 0;
  }

  current_timer_val = (uint32)rex_get_timer
    (&(per_apn_entry_ptr->pdn_conn_tmr));

  DS_3GPP_MSG2_MED("Current value of PDN Conn Timer at index %d is %d msecs",
                    index,current_timer_val);

  return current_timer_val;
}
/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_VALIDATE_EFS_ENTRY

  DESCRIPTION
  This function validates the Entry in EFS File
  
  PARAMETERS
  pdn_limit_efs_timer_info:               Entry to be validated 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If validation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_validate_efs_entry
(
  ds_3gpp_pdn_limit_throt_info_pdn_limit_timer_type *pdn_limit_efs_timer_info
)
{
  boolean                                            ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(pdn_limit_efs_timer_info == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL Pointer passed, Validation failure");
    return ret_val;
  }

  if(pdn_limit_efs_timer_info->throt_info_pdn_limit_timer_type_dyn_p != NULL)
  {
    if((pdn_limit_efs_timer_info->throt_info_pdn_limit_timer_type_dyn_p->cookie
        == DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE) &&
       (pdn_limit_efs_timer_info->throt_info_pdn_limit_timer_type_dyn_p->valid
        == TRUE))
    {
      ret_val = TRUE;
    }
  }
  else
  {
    DS_3GPP_MSG0_MED("Internal Dynamic Pointer invalid, Validation failure");
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_THROT_INFO_PER_SUBS

  DESCRIPTION
  This function gets the global PDN Limit Sampling and Throttling Struct
  
  PARAMETERS
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Global Structure per subs_id
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_3gpp_pdn_limit_throt_info_type* ds_3gpp_pdn_limit_get_throt_info_per_subs
(
  sys_modem_as_id_e_type                     subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((!ds3gsubsmgr_is_subs_id_valid(subs_id)))
  {
    return NULL;
  }

  return ds_3gpp_pdn_limit_throt_info_ptr[subs_id];
}
/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_SET_THROT_INFO_PER_SUBS

  DESCRIPTION
  This function sets the global PDN Limit Sampling and Throttling Struct
  
  PARAMETERS
  subs_id:                    Current active PS Subs.
  pdn_limit_throt_info:       Value to be saved
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_set_throt_info_per_subs
(
  sys_modem_as_id_e_type                     subs_id,
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info
) 
{
  boolean                                    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ((ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE) || 
      (pdn_limit_throt_info == NULL))
  {
    return ret_val;
  }

  ds_3gpp_pdn_limit_throt_info_ptr[subs_id] = pdn_limit_throt_info;

  ret_val = TRUE;

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_INIT_TIMER_TBL

  DESCRIPTION
  This function initializes the PDN Limit Timer Table
  
  PARAMETERS
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_init_timer_tbl
(
  sys_modem_as_id_e_type                     subs_id
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  uint8                                      index = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Initialize of Timer table failed",subs_id);
    return;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      pdn_limit_throt_info->pdn_limit_timer_tbl[index].
        throt_info_pdn_limit_timer_type_dyn_p = NULL;
    }
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_WRITE_INTO_EFS

  DESCRIPTION
  This function writes information into the EFS Sampling File
  
  PARAMETERS
  write_buf:                  Data to be written into EFS
  write_offset:               Offset at which data needs to be written
  subs_id:                    Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If write operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_write_into_efs
(
  ds_3gpp_pdn_limit_efs_data_buf_type      *write_buf,
  uint32                                    write_offset,
  sys_modem_as_id_e_type                    subs_id
)
{
  boolean                                    ret_val = FALSE;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  int32                                      fd = -1;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  if(write_buf == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid Write buffer passed");
    return ret_val;
  }
 
  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    fd = mcfg_fopen(pdn_limit_throt_info->efs_file_name,
                    MCFG_FS_O_RDWR,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type)subs_id);

    if(fd != -1)
    {
      if( mcfg_lseek(fd, write_offset, SEEK_SET, MCFG_FS_TYPE_EFS)
           == write_offset )
      {
        if( mcfg_fwrite(fd,
                        write_buf->data_ptr,
                        write_buf->data_len,
                        MCFG_FS_TYPE_EFS)
              == write_buf->data_len )
        {
          ret_val = TRUE;
        }
        else
        {
          DS_3GPP_MSG2_ERROR("Could not write: %d bytes on file: %d",
                             write_buf->data_len, fd);
        }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Unable to Seek the file to: %d from start", 
                           write_offset);
      }

      if(mcfg_fclose(fd, MCFG_FS_TYPE_EFS) != 0)
      {
        DS_3GPP_MSG1_ERROR("Unable to Close the file after write: %d",fd);
        ret_val = FALSE;
      }
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Unable to Open the file for write: %d",fd);
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CREATE_EFS_FILE

  DESCRIPTION
  This function creates the EFS Sampling File
  
  PARAMETERS
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If create operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_create_efs_file
(
  sys_modem_as_id_e_type                     subs_id
)
{
  int32                                      fd = -1;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  boolean                                    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    fd = mcfg_fopen(pdn_limit_throt_info->efs_file_name,
                    MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                    MCFG_FS_DEFFILEMODE,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type)subs_id);

    if(fd != -1)
    {
      if(mcfg_lseek(fd, 0, SEEK_SET, MCFG_FS_TYPE_EFS) == 0)
      {
        if(mcfg_fclose(fd, MCFG_FS_TYPE_EFS) == 0)
        {
          ret_val = TRUE;
        }
        else
        {
          DS_3GPP_MSG1_ERROR("Unable to close file: %d",fd);
        }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Unable to seek file to start: %d",fd);
      }
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Unable to open file: %d",fd);
    }
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CREATE_AND_INIT_PDN_LIMIT_THROT_FILE

  DESCRIPTION
  This function creates and initializes the EFS Sampling File with IMSI Info
  
  PARAMETERS
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_create_and_init_pdn_limit_throt_file
(
  sys_modem_as_id_e_type                     subs_id
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  ds_3gpp_pdn_limit_efs_data_buf_type        write_data;
  boolean                                    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    if(ds_3gpp_pdn_limit_create_efs_file (subs_id) == TRUE)
    {
      memset(&write_data, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));
      
      write_data.data_ptr = &(pdn_limit_throt_info->imsi_data);
      write_data.data_len = sizeof(nv_ehrpd_imsi_type);
      
      ret_val = ds_3gpp_pdn_limit_write_into_efs (&write_data, 0, subs_id);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Unable to create the File during INIT");
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_READ_INFO_FROM_EFS

  DESCRIPTION
  This function reads the EFS File to gather the persistant throttling info
  
  PARAMETERS
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If read operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_read_info_from_efs
(
  sys_modem_as_id_e_type                     subs_id
)
{
  int                                        fd = -1;
  uint8                                      index = 
                                             DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  boolean                                    ret_val = FALSE;
  nv_ehrpd_imsi_type                         imsi_info_on_file;
  throt_info_pdn_limit_timer_type_dyn_s      temp_throt_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    fd = mcfg_fopen(pdn_limit_throt_info->efs_file_name,
                    MCFG_FS_O_RDWR,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type)subs_id);

    //Close the file

    if(fd != -1)
    {
      memset(&imsi_info_on_file, 0, sizeof(nv_ehrpd_imsi_type));
      if(mcfg_fread(fd,
                    &imsi_info_on_file,
                    sizeof(nv_ehrpd_imsi_type),
                    MCFG_FS_TYPE_EFS)
           == sizeof(nv_ehrpd_imsi_type))
      {
        if(memcmp((void *)&imsi_info_on_file,
               (void *)&pdn_limit_throt_info->imsi_data,
               sizeof(nv_ehrpd_imsi_type)) == 0)
        {
          for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
          {
            memset(&temp_throt_info, 0, 
                   sizeof(throt_info_pdn_limit_timer_type_dyn_s));

            if(mcfg_fread(fd,
                    (void *)&temp_throt_info, 
                          sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                          MCFG_FS_TYPE_EFS)
                 == sizeof(throt_info_pdn_limit_timer_type_dyn_s))
            {
              
              if(temp_throt_info.valid == TRUE && 
                 temp_throt_info.cookie == 
                                   DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE)
              {
                pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                  throt_info_pdn_limit_timer_type_dyn_p = 
                  (throt_info_pdn_limit_timer_type_dyn_s *)modem_mem_alloc(
                     sizeof(throt_info_pdn_limit_timer_type_dyn_s), 
                     MODEM_MEM_CLIENT_DATA);

                if(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                  throt_info_pdn_limit_timer_type_dyn_p != NULL)
                {
                  memset(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                         throt_info_pdn_limit_timer_type_dyn_p, 0,
                         sizeof(throt_info_pdn_limit_timer_type_dyn_s));

                  memscpy((void *)pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                           throt_info_pdn_limit_timer_type_dyn_p,
                          sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                          (void *)&temp_throt_info,
                          sizeof(throt_info_pdn_limit_timer_type_dyn_s));
                  ret_val = TRUE;
                } /* Dynamic memory allocation successful */
                else
                {
                  DS_3GPP_MSG1_ERROR("Memory allocation failed for Index: %d",
                                     index);
                }
              } /* EFS entry valid for given index */
              else
              {
                DS_3GPP_MSG1_ERROR("Invalid Entry found for Index: %d",
                                   index);
              }
            } /* EFS read of throt info success */
            else
            {
              DS_3GPP_MSG2_ERROR("Could not read: %d bytes at index: %d",
                                 sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                                 index);
            }
          } /* For Loop */
        } /* IMSI Comparison success */
        else
        {
          DS_3GPP_MSG0_ERROR("IMSI Comparison failed, while reading EFS info");
        }
      } /* IMSI Read successful */
      else
      {
        DS_3GPP_MSG1_ERROR("Could not read: %d bytes for IMSI info",
                           sizeof(nv_ehrpd_imsi_type));
      }

      if(mcfg_fclose(fd, MCFG_FS_TYPE_EFS) != 0)
      {
        DS_3GPP_MSG1_ERROR("Unable to Close File: %d",fd);
      }
    } /* File Descriptor Valid*/
    else
    {
      DS_3GPP_MSG1_ERROR("Invalid File Descriptor, could not open file: %d",
                         fd);
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Unable to Extract PDN Limit global EFS struct");
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CLEAR_EFS_INFO

  DESCRIPTION
  This function clears the EFS File in case the information inside is
  in-valid
  
  PARAMETERS
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_clear_efs_info
(
   sys_modem_as_id_e_type                     subs_id
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  ds_3gpp_pdn_limit_efs_data_buf_type        write_buf;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    //CLose the File //No need to open file
    memset(&write_buf, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

    write_buf.data_ptr = &(pdn_limit_throt_info->imsi_data);
    write_buf.data_len = sizeof(nv_ehrpd_imsi_type);

    ds_3gpp_pdn_limit_write_into_efs(&write_buf, 0, subs_id);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Unable to Extract PDN Limit global EFS struct");
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_RESET_EFS_ENTRY

  DESCRIPTION
  This function clears the EFS Entry for a Particular APN
  
  PARAMETERS
  apn:                  APN whose entry needs to be cleared
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_reset_efs_entry 
(
 byte                                      *apn, 
 sys_modem_as_id_e_type                     subs_id
)
{
  uint8                                     index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = NULL;
  ds_3gpp_pdn_limit_efs_data_buf_type       write_buf;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  
  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index ++)
    {
      if(!ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index]))
      {
        DS_3GPP_MSG1_MED("Invalid Entry found at index: %d",index);
        continue;
      }

      DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                              "APN Passed: %s & APN Present on Struct: %s",
                              apn,pdn_limit_throt_info->pdn_limit_timer_tbl
                              [index].throt_info_pdn_limit_timer_type_dyn_p
                              ->apn_name);

      if(strncasecmp((const char *)apn,
                (const char *)pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                throt_info_pdn_limit_timer_type_dyn_p->apn_name,
                DS_UMTS_MAX_APN_STRING_LEN+1) == 0)
      {
        DS_3GPP_MSG1_MED("Match found at index: %d",index);

        memset(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
               throt_info_pdn_limit_timer_type_dyn_p, 0, 
               sizeof(throt_info_pdn_limit_timer_type_dyn_s));

        memset(&write_buf, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

        write_buf.data_ptr = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                             throt_info_pdn_limit_timer_type_dyn_p;

        write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

        ds_3gpp_pdn_limit_write_into_efs(&write_buf,
                       sizeof(throt_info_pdn_limit_timer_type_dyn_s)*index +
                       sizeof(nv_ehrpd_imsi_type), subs_id);

        modem_mem_free(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                       throt_info_pdn_limit_timer_type_dyn_p, 
                       MODEM_MEM_CLIENT_DATA);

        pdn_limit_throt_info->pdn_limit_timer_tbl[index].
               throt_info_pdn_limit_timer_type_dyn_p = NULL;
        break;
      }
    }

    if(index == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
    {
      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                              "No match found in EFS Info Struct for APN: %s",
                              apn);
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable to locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_EFS_SAMPLING_TIMER

  DESCRIPTION
  This function starts the EFS Sampling
  
  PARAMETERS
  calculate_sampling_timer:   Flag that tells whether Sampling timer needs
                              to be calculate or not
  callback data:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_efs_sampling_timer
(
  boolean                                   calculate_sampling_timer,
  sys_modem_as_id_e_type                    subs_id
)
{
  uint8                                     index = 0;
  uint64                                    curr_max_throt_timer = 0;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = NULL;
  uint32                                    pdn_limit_throt_timer_val = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  /*------------------------------------------------------------------------ 
   We select the Max throttling timer among all APN's and samples are 1/10th
   of the Max value
   
   If however the sample time is less than EFS Flush Time then we reset the
   sample time to EFS Flush Time
  -----------------------------------------------------------------------*/
  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    if(calculate_sampling_timer == TRUE)
    {
      for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
      {
        if(!(ds_3gpp_pdn_limit_validate_efs_entry(
           &pdn_limit_throt_info->pdn_limit_timer_tbl[index])))
        {
          DS_3GPP_MSG1_MED("Invalid Entry found at index: %d",index);
          continue;
        }

        curr_max_throt_timer = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                               throt_info_pdn_limit_timer_type_dyn_p->timer_val;

        if(curr_max_throt_timer > pdn_limit_throt_timer_val)
        {
          pdn_limit_throt_timer_val = curr_max_throt_timer;
        }
      }

      if(pdn_limit_throt_timer_val != 0)
      {
        pdn_limit_throt_info->sampling_timer_val = 
                                             (pdn_limit_throt_timer_val/10);

        if(pdn_limit_throt_info->sampling_timer_val < 
                                      DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME)
        {
          if(DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME > pdn_limit_throt_timer_val)
          {
            pdn_limit_throt_info->sampling_timer_val = 
                                              pdn_limit_throt_timer_val;
          }
          else
          {
            pdn_limit_throt_info->sampling_timer_val = 
                                       DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME;
          }
        }

        DS_3GPP_MSG1_MED("EFS Sampling Timer value is: %d",
                         pdn_limit_throt_info->sampling_timer_val);
      }
      else
      {
        DS_3GPP_MSG1_HIGH("Not Starting Sampling because timer value is"
                          ": %d",pdn_limit_throt_timer_val);
        pdn_limit_throt_info->sampling_timer_val = 0;
      }
    }
    rex_set_timer(&(pdn_limit_throt_info->sampling_timer),
                    pdn_limit_throt_info->sampling_timer_val);
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_SAMPLING_TIMER_EXP_CMD

  DESCRIPTION
  This function process the Sampling Timer expiry command by:
  1. Writing a new sample to EFS File
  2. Starting the sampling Timer again
  
  PARAMETERS
  subs_id:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_sampling_timer_exp_cmd
(
  sys_modem_as_id_e_type                    subs_id
)
{
  uint8                                     index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  int64                                     curr_time = 0;
  int64                                     time_elapsed = 0;
  int64                                     remaining_throttling_time = 0;
  boolean                                   sampling_needed = FALSE;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = NULL;
  ds_3gpp_pdn_limit_efs_data_buf_type       write_buf;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Unable to Start Sampling Timer",subs_id);
    return;
  }
  
  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  /*---------------------------------------------------------------------- 
   Since we have a single sampling timer running for all APN's
   
   We check whether Time-Elapsed is less than the remaining throttling
   timer for any APN, if not then we dont need to perform any more sampling
   
   Else
   we mark a variable and start the sampling timer
   ----------------------------------------------------------------------*/

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      if(!(ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index])))
      {
        DS_3GPP_MSG1_MED("Invalid entry found at index: %d", index);
        continue;
      }

      curr_time = time_get_secs()*(int64)1000;
      time_elapsed = ((int64)curr_time - pdn_limit_throt_info->
                     pdn_limit_timer_tbl[index].
                     throt_info_pdn_limit_timer_type_dyn_p->timestamp);

      remaining_throttling_time = pdn_limit_throt_info->
                                 pdn_limit_timer_tbl[index].
                                 throt_info_pdn_limit_timer_type_dyn_p->
                                 timer_val - (int64)time_elapsed;

      DS_3GPP_MSG3_MED("Current Time: %ld, Time Elapsed: %ld, Remaining Throttling"
                       " Time: %ld", curr_time, time_elapsed, 
                       remaining_throttling_time);

      if((remaining_throttling_time > 0))
      {
        if((time_elapsed > 0))
        {
          pdn_limit_throt_info->pdn_limit_timer_tbl[index].
          throt_info_pdn_limit_timer_type_dyn_p->timer_val -= time_elapsed;
        }
        pdn_limit_throt_info->pdn_limit_timer_tbl[index].
        throt_info_pdn_limit_timer_type_dyn_p->timestamp = curr_time;
        

        DS_3GPP_MSG2_MED("Writing following sample in EFS File, "
                         "Throttle Timer: %lu & Timestamp: %lu",
                         pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                         throt_info_pdn_limit_timer_type_dyn_p->timer_val,
                         pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                         throt_info_pdn_limit_timer_type_dyn_p->timestamp);

        memset(&write_buf, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

        write_buf.data_ptr =  pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                              throt_info_pdn_limit_timer_type_dyn_p;
        write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

        ds_3gpp_pdn_limit_write_into_efs(&write_buf,
                   sizeof(throt_info_pdn_limit_timer_type_dyn_s)*index +
                   sizeof(nv_ehrpd_imsi_type),subs_id);

        sampling_needed = TRUE;
      }
      else
      {
        DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_MED,"Throttling Timer expired for"
                                " APN: %s at ""index: %d",pdn_limit_throt_info->
                                pdn_limit_timer_tbl[index].
                                throt_info_pdn_limit_timer_type_dyn_p->apn_name,
                                index);
      }
    }
    if(sampling_needed == TRUE)
    {
      ds_3gpp_pdn_limit_start_efs_sampling_timer(FALSE, subs_id);
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable to locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_SAMPLING_TIMER_EXP_CB

  DESCRIPTION
  This function posts a cmd to DS task once it gets a cback saying EFS Sampling
  Timer has expired
  
  PARAMETERS
  callback data:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_sampling_timer_exp_cb
(
  unsigned long                 callback_data                           
)
{
  ds_cmd_type                   *cmd_ptr = NULL;
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  subs_id = (sys_modem_as_id_e_type)callback_data;

  DS_3GPP_MSG0_HIGH("Posting DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP");

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP;
  cmd_ptr->cmd_payload_ptr = (void *)subs_id;

  ds_put_cmd_ext(cmd_ptr);

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_START_CB

  DESCRIPTION
  This function posts a cmd to DS task once it gets a cback saying PDN req
  throttling timer is about to start
  
  PARAMETERS
  throt_sm_ptr:               Throttle SM pointer
  callback data:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_pdn_req_throt_tmr_start_cb
(
  void                         *throt_sm_ptr,
  void                         *callback_data                           
)
{
  ds_cmd_type                    *cmd_ptr;
  ds_3gpp_pdn_throt_sm_info_type *throt_sm_info_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_throt_sm_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG0_HIGH("Posting DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START");

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START;
  throt_sm_info_ptr = (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

  throt_sm_info_ptr->throt_sm_ptr = throt_sm_ptr;
  throt_sm_info_ptr->subs_id = (int32)callback_data;
  ds_put_cmd_ext(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_ALLOCATE_EFS_INFO_ENTRY

  DESCRIPTION
  This function allocates a block on heap memory for EFS Samples
 
  PARAMETERS
  None 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  throt_info_pdn_limit_timer_type_dyn_s: A memory block of this data type
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
throt_info_pdn_limit_timer_type_dyn_s* ds_3gpp_pdn_limit_allocate_efs_info_entry
(
  void
)
{
  throt_info_pdn_limit_timer_type_dyn_s       *pdn_limit_efs_info_entry = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pdn_limit_efs_info_entry = (throt_info_pdn_limit_timer_type_dyn_s *)
                             modem_mem_alloc(
                             sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                             MODEM_MEM_CLIENT_DATA);

  return pdn_limit_efs_info_entry;
  

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_REQ_TMR_START_CMD

  DESCRIPTION
  This function does the required processing when PDN request wait timer
  starts like:
 
  1. Writing into EFS ro make it persistant
  2. Start the sampling timer so that samples are recorded frequently
 
  
  PARAMETERS
  throt_sm_ptr:               Throttle SM pointer
  subs_id:                    Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_req_tmr_start_cmd
(
  void                                     *throt_sm_ptr,
  sys_modem_as_id_e_type                    subs_id
)
{
  uint8                                     index = 0;
  uint8                                     first_free_index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  uint8                                     efs_write_index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  uint32                                    sampling_time_left = 0;
  boolean                                   efs_write_needed = FALSE;
  ds_3gppi_throttle_sm_s                   *pdn_limit_throt_sm_ptr = NULL;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = NULL;
  ds_3gpp_pdn_limit_efs_data_buf_type       write_buf;
  throt_info_pdn_limit_timer_type_dyn_s    *pdn_limit_efs_info_entry = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Unable to Start Sampling Timer",subs_id);
    return;
  }
  
  pdn_limit_throt_sm_ptr = (ds_3gppi_throttle_sm_s *)throt_sm_ptr;
  if(pdn_limit_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG1_ERROR("Invalid PDN Throt SM Pointer as Input. "
                       "PDN Limit, Unable to Start Sampling Timer",subs_id);
    return;
  }
  /*------------------------------------------------------------------------
   We try to locate an Entry for the APN whose throttling timer is about to
   start,
   
   If Entry is found we update EFS File with an Entry for matched APN
   
   Else we create an Entry for the new APN and write it contents in EFS
   
   We also need to make sure to calculate & start the Sampling Timer only
   if it is not running currently.
  -------------------------------------------------------------------------*/
  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      if(!(ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index])))
      {
        if(first_free_index == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
        {
          DS_3GPP_MSG1_MED("First Free Entry found at index: %d",
                            index);
          first_free_index = index;
        }
        continue;
      }

      if(strncasecmp((const char *)pdn_limit_throt_sm_ptr->apn, 
                (const char *)pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                throt_info_pdn_limit_timer_type_dyn_p->apn_name,
                DS_UMTS_MAX_APN_STRING_LEN+1) == 0)
      {
        first_free_index = DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;

        if((pdn_limit_throt_info->pdn_limit_timer_tbl[index].
          throt_info_pdn_limit_timer_type_dyn_p->timer_val = 
                      pdn_limit_throt_sm_ptr->throt_tmr_val)>0)
        {
          pdn_limit_throt_info->pdn_limit_timer_tbl[index].
          throt_info_pdn_limit_timer_type_dyn_p->timestamp = time_get_secs()*
                                                                  (uint64)1000;

          memset(&write_buf,0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

          write_buf.data_ptr = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                                throt_info_pdn_limit_timer_type_dyn_p;
          write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

          efs_write_index = index;

          efs_write_needed = TRUE;
        }
        else
        {
          DS_3GPP_MSG0_HIGH("PDN Limit Throttle Timer is no longer valid");
        }

        break;
      }
    }

    if(first_free_index != DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
    {
      pdn_limit_efs_info_entry = ds_3gpp_pdn_limit_allocate_efs_info_entry ();

      if(pdn_limit_efs_info_entry != NULL)
      {
        memset(pdn_limit_efs_info_entry, 0, 
               sizeof(throt_info_pdn_limit_timer_type_dyn_s));

        pdn_limit_efs_info_entry->cookie = 
                           DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE;

        if((pdn_limit_efs_info_entry->timer_val = 
           pdn_limit_throt_sm_ptr->throt_tmr_val)>0)
        {
          pdn_limit_efs_info_entry->timestamp = time_get_secs()*(uint64)1000;
          pdn_limit_efs_info_entry->valid = TRUE;

          memscpy((void *)pdn_limit_efs_info_entry->apn_name,
                  DS_UMTS_MAX_APN_STRING_LEN+1,
                  (void *)pdn_limit_throt_sm_ptr->apn,
                  DS_UMTS_MAX_APN_STRING_LEN+1);

          write_buf.data_ptr = pdn_limit_efs_info_entry;
          write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

          efs_write_index = first_free_index;

          efs_write_needed = TRUE;

          pdn_limit_throt_info->pdn_limit_timer_tbl[first_free_index].
            throt_info_pdn_limit_timer_type_dyn_p = pdn_limit_efs_info_entry;
        }
        else
        {
          DS_3GPP_MSG0_HIGH("PDN Limit Throttle Timer is no longer valid");
          modem_mem_free(pdn_limit_efs_info_entry, MODEM_MEM_CLIENT_DATA);
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Memory Allocation failure, Could not allocate "
                           "EFS Entry !!!");
      }
    }

    if(efs_write_needed)
    {
      (void)ds_3gpp_pdn_limit_write_into_efs(&write_buf,
                 sizeof(throt_info_pdn_limit_timer_type_dyn_s)*efs_write_index +
                 sizeof(nv_ehrpd_imsi_type), subs_id);

      sampling_time_left = rex_get_timer(&pdn_limit_throt_info->sampling_timer);

      DS_3GPP_MSG1_MED("Remaining Sampling Timer: %d",sampling_time_left);
      if(sampling_time_left == 0)
      {
        ds_3gpp_pdn_limit_start_efs_sampling_timer (TRUE, subs_id);
      }
      else
      {
        DS_3GPP_MSG0_MED("Sampling Timer already in Progress, not re-starting");
      }
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
  }
}
/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CALCULATE_REM_THROT_TIME

  DESCRIPTION
  This function calculates the remaining throttling time after boot-up
  
  PARAMETERS  
  index: Index whose timer values need to be used for calculating throt time
  pdn_limit_throt_info: Global EFS Struct containing the EFS Info
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  uint32: Throttling Timer value
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_calculate_rem_throt_time
(
  uint8                                        index,
  ds_3gpp_pdn_limit_throt_info_type           *pdn_limit_throt_info,
  boolean                                      include_elapsed_time
)
{
  int64                                        time_rem = 0;
  int64                                        time_elapsed = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    DS_3GPP_MSG1_MED("Match found at index: %d",index);

    DS_3GPP_MSG2_MED("Throtting Timer Value from EFS: %lu & "
                     "Timestamp from EFS: %lu",
                     pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                     throt_info_pdn_limit_timer_type_dyn_p->timer_val,
                     pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                     throt_info_pdn_limit_timer_type_dyn_p->timestamp);

    DS_3GPP_MSG1_MED("Current Time-Stamp is %lu",time_get_secs()*(uint64)1000);

    time_elapsed = (time_get_secs()*(int64)1000 - 
                    pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                    throt_info_pdn_limit_timer_type_dyn_p->timestamp);

    if( (time_elapsed > 0) && include_elapsed_time )
    {
      time_rem = (pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                 throt_info_pdn_limit_timer_type_dyn_p->timer_val - 
                 (int64)time_elapsed);
    }
    else
    {
      time_rem = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                 throt_info_pdn_limit_timer_type_dyn_p->timer_val;
    }
    

    if((time_rem > 0) == FALSE)
    { 
      time_rem = 0;
    }
    
    DS_3GPP_MSG1_MED("Throttling Timer Value returned to throttle SM: %d",
                     time_rem);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Invalid Entry for Global EFS Structure");
  }

  return time_rem;
}
/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_THROTTLING_TMR_VAL_FROM_EFS

  DESCRIPTION
  This function returns the extracted throttling timer values from EFS after
  boot-up
  
  PARAMETERS  
  throt_sm_ptr:        Throttle SM Pointer
  callback_data:       Current active PS subs
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  uint32: Throttling Timer value
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_get_throttling_tmr_val_from_efs
(
  void                                      *throt_sm_ptr,
  void                                      *callback_data 
)
{
  uint8                                      index = 0;
  int32                                      time_rem = 0;
  byte                                      *apn = NULL;
  sys_modem_as_id_e_type                     subs_id = 
                                         (sys_modem_as_id_e_type)callback_data;
  ds_3gppi_throttle_sm_s                    *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Cannot extract Power-up throttling timer",
                       subs_id);
    return time_rem;
  }

  pdn_throt_sm_ptr = (ds_3gppi_throttle_sm_s *)throt_sm_ptr;
  if(pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid throt SM ptr, "
                       "Cannot extract Power-up throttling timer");
    return time_rem;
  }


  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      if(!ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index]))
      {
        DS_3GPP_MSG1_MED("Invalid Entry found at index: %d",index);
        continue;
      }

      DS_3GPP_MSG1_HIGH("Starting PDN Req throttling timer at index after "
                        "boot-up %d",index);

      apn = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
            throt_info_pdn_limit_timer_type_dyn_p->apn_name;

      if(strncasecmp((const char*)apn, (const char*)pdn_throt_sm_ptr->apn,
                      DS_UMTS_MAX_APN_STRING_LEN+1) == 0)
      {
        time_rem = ds_3gpp_pdn_limit_calculate_rem_throt_time(index,
                                                pdn_limit_throt_info, FALSE);

        break;
      }
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
  }

  return time_rem;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWER_UP_THROT_HDLR

  DESCRIPTION
  This function performs the Power Up throttling 
  
  PARAMETERS  
  subs_id: Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_power_up_throt_hdlr
(
  sys_modem_as_id_e_type                     subs_id
)
{
  uint8                                      index = 0;
  byte                                      *apn = NULL;
  ds_3gpp_pdn_throttle_sm_s                 *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Cannot perform Power-Up Throttling",subs_id);
    return;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {

      /*------------------------------------------------------------- 
        Allocate an Entry for PDN Limit Throttling in Throttle SM
       
        Register for Timer Start, Expiry cback and a cback to retrieve
        throttling timer value
       
        Perform APN Throttling
      -------------------------------------------------------------*/
      if(!ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index]))
      {
        DS_3GPP_MSG1_MED("Invalid Entry found at index: %d",index);
        continue;
      }

      apn = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
            throt_info_pdn_limit_timer_type_dyn_p->apn_name;

      DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                              "APN extracted from EFS Info is: %s",apn);

      if(ds_3gpp_pdn_limit_calculate_rem_throt_time(index,
                                       pdn_limit_throt_info, FALSE) > 0)
      {
        pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                           (apn, TRUE, TRUE, DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT, 
                            subs_id);

        if (pdn_throt_sm_ptr == NULL)
        {
          DS_3GPP_MSG0_ERROR("pdn_throt_sm_p is NULL");
          return;
        }

        ds_3gppi_throt_sm_register_timer_start_ev_cb(
           pdn_throt_sm_ptr->v4_throttle_sm,
           ds_3gpp_pdn_limit_pdn_req_throt_tmr_start_cb,
           (void *)subs_id);

        ds_3gppi_throt_sm_register_timer_expiry_ev_cb(
          pdn_throt_sm_ptr->v4_throttle_sm,
          ds_3gpp_pdn_limit_pdn_req_throt_tmr_expiry_cb,
          (void*)subs_id);

        ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
          pdn_throt_sm_ptr->v4_throttle_sm,
          ds_3gpp_pdn_limit_get_throttling_tmr_val_from_efs,
          (void*)subs_id);

        ds_3gpp_pdn_throt_perform_apn_throttling(pdn_throt_sm_ptr,
                                                        apn,
                                                        NULL,
                                                        DS_UMTS_PDP_IPV4V6,
                                                        subs_id,
                                                        TRUE,
                                                        NULL);
      }
      else
      {
        DS_3GPP_MSG0_HIGH("No Initiating throttling since remaining time is "
                           "zero");
      }
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
  }
}
/*==============================================================================
FUNCTION DS_3GPP_PDN_LIMIT_STORE_IMSI_INFO_AND_INIT_THROT_DATA 
 
DESCRIPTION
  This function processes the IMSI information passed and initializes the EFS File
  and its info 

PARAMETERS 
  imsi_inf          :  IMSI Info from MMGSDI 
  subs_id           :  Subscription ID
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  TRUE: If Successful
  FALSE: Otherwise
 
SIDE EFFECTS 
  None 

==============================================================================*/
boolean ds_3gpp_pdn_limit_store_imsi_info_and_init_throt_data
(
  nv_ehrpd_imsi_type                        *imsi_info,
  sys_modem_as_id_e_type                     subs_id
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  struct fs_stat                             fs_buf;
  boolean                                    ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Power up throttle Init failed",subs_id);
    ret_val = FALSE;
    return ret_val;
  }

  if(imsi_info == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid IMSI Info Provided");
    ret_val = FALSE;
    return ret_val;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    memscpy((void *)&pdn_limit_throt_info->imsi_data, 
            sizeof(nv_ehrpd_imsi_type),
            (void *)imsi_info,
            sizeof(nv_ehrpd_imsi_type));

    if( mcfg_fs_stat(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME,
                     &fs_buf,
                     MCFG_FS_TYPE_EFS,
                     (mcfg_fs_sub_id_e_type)subs_id) == MCFG_FS_STATUS_OK )
    {
      if( mcfg_fs_stat(pdn_limit_throt_info->efs_file_name,
                       &fs_buf,
                       MCFG_FS_TYPE_EFS,
                       (mcfg_fs_sub_id_e_type)subs_id) == MCFG_FS_STATUS_OK )
      {
        /*------------------------------------------------------------- 
         Read IMSI and Compare the IMSI,
           If not same clear the file
           Else perform power-up throttling
         -------------------------------------------------------------*/
        if(ds_3gpp_pdn_limit_read_info_from_efs (subs_id) == FALSE)
        {
          //Clear the EFS File, Write IMSI info only to File
          ds_3gpp_pdn_limit_clear_efs_info (subs_id);
        }
        else
        {
          ds_3gpp_pdn_limit_power_up_throt_hdlr (subs_id);
        }
      }
      else
      {
        //File Not Present, so create and initialize the File with IMSI
        if(ds_3gpp_pdn_limit_create_and_init_pdn_limit_throt_file 
           (subs_id) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Unable to create EFS File for PDN "
                             "Limit Handler");
          ret_val = FALSE;
        }
      }
    }
    else
    {
      //No Directory or File Present, create both and initialize File
      if( mcfg_mkdir(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME,
                     S_IREAD|S_IWRITE|S_IEXEC,
                     MCFG_FS_TYPE_EFS) == 0 )
      {
        if(ds_3gpp_pdn_limit_create_and_init_pdn_limit_throt_file 
           (subs_id) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Unable to create EFS File for PDN "
                             "Limit Handler");
          ret_val = FALSE;
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Unable to create EFS Directory for PDN Limit Handler");
        ret_val = FALSE;
      }
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
    ret_val = FALSE;
  }

  return ret_val;
}

/*==============================================================================
FUNCTION DS_3GPP_PDN_LIMIT_IMSI_INFO_AVAIL_CMD_HDLR
 
DESCRIPTION
  This function process the IMSI information available CMD so that to read IMSI
  information and perform power-up throttling if needed

PARAMETERS 
  subs_id           :  Subscription ID
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None 

==============================================================================*/
void ds_3gpp_pdn_limit_imsi_info_avail_cmd_hdlr
(
  sys_modem_as_id_e_type                     subs_id
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  nv_ehrpd_imsi_type                         imsi_info;
  boolean                                    imsi_info_available = FALSE;
  boolean                                    clean_up_needed = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Power up throttle Init failed",subs_id);
    return ;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    imsi_info_available = ds3g_mmgsdi_get_imsi_data_per_subs_id(&imsi_info,
                                                                subs_id);
    if(imsi_info_available == TRUE)
    {
      pdn_limit_throt_info->imsi_data_avail = TRUE;
      if(ds_3gpp_pdn_limit_store_imsi_info_and_init_throt_data(&imsi_info,
                                                            subs_id) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Unable to perform EFS operation after IMSI Ready "
                           "event cback, cleaning-up");
        clean_up_needed = TRUE;
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Unable to Extract IMSI info from MMGSDI "
                         "after receiving IMSI info ready event");
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unable locate Entry for EFS Global Structure for "
                       "subs: %d", subs_id);
  }

  if(clean_up_needed)
  {
    if(pdn_limit_throt_info != NULL)
    {
      rex_undef_timer(&pdn_limit_throt_info->sampling_timer);
      modem_mem_free(pdn_limit_throt_info, MODEM_MEM_CLIENT_DATA);
    }

    pdn_limit_throt_info = NULL;

    ds_3gpp_pdn_limit_throt_info_ptr[subs_id] = NULL;
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWER_UP_THROT_INIT

  DESCRIPTION
  This function performs Powerup Initialization to check for Persistant
  Throttling
  
  PARAMETERS  
  subs_id: Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_power_up_throt_init 
(
  sys_modem_as_id_e_type                     subs_id
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  struct fs_stat                             fs_buf;
  nv_ehrpd_imsi_type                         imsi_info;
  boolean                                    imsi_info_available = FALSE;
  boolean                                    clean_up_needed = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Power up throttle Init failed",subs_id);
    return;
  }

  if((pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id)) 
                                                                    == NULL)
  {
    pdn_limit_throt_info = (ds_3gpp_pdn_limit_throt_info_type *)modem_mem_alloc
                            (sizeof(ds_3gpp_pdn_limit_throt_info_type), 
                             MODEM_MEM_CLIENT_DATA);
  }
  /*------------------------------------------------------------------------- 
    Initialize sampling Timer, File name, IMSI Info per subs
   
    If IMSI Info is not present we wait for cback from MMGSDI for IMSI info
   
    Else
    1.  Check if Directory and File Exist
    2.  If they dont initialize the file with IMSI info
    3.  If they do read the appropriate value, compare IMSI and start
        throttling timer if needed
    4.  Throttling Timer gets it value from EFS, make sure that is read locally
        before throttling is started
  ---------------------------------------------------------------------------*/

  do
  {
    if(pdn_limit_throt_info != NULL)
    {
      if(ds_3gpp_pdn_limit_set_throt_info_per_subs (subs_id, 
                                                    pdn_limit_throt_info))
      {
        memset(pdn_limit_throt_info, 0, 
               sizeof(ds_3gpp_pdn_limit_throt_info_type));

        pdn_limit_throt_info->sampling_timer_val = 0xFFFFFFFF;

        rex_def_timer_ex(&pdn_limit_throt_info->sampling_timer,
                      (rex_timer_cb_type)ds_3gpp_pdn_limit_sampling_timer_exp_cb,
                      (unsigned long)subs_id);

        ds_3gpp_pdn_limit_init_timer_tbl (subs_id);

        strlcpy(pdn_limit_throt_info->efs_file_name,
                DS_3GPP_PDN_LIMIT_THROT_DIR_NAME,
                strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME)+1);

        pdn_limit_throt_info->efs_file_name[
          strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME)] = '/';

        strlcat(pdn_limit_throt_info->efs_file_name,
                DS_3GPP_PDN_LIMIT_THROTTLE_FILE_NAME(subs_id),
                (strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME)+1+
                strlen(DS_3GPP_PDN_LIMIT_THROTTLE_FILE_NAME(subs_id))+1));

        pdn_limit_throt_info->efs_file_name[\
          strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME) + 1 +\
          strlen(DS_3GPP_PDN_LIMIT_THROTTLE_FILE_NAME(subs_id))] = '\0';

        imsi_info_available = ds3g_mmgsdi_get_imsi_data_per_subs_id(&imsi_info, 
                                                                    subs_id);

        memset(&fs_buf, 0, sizeof(struct fs_stat));

        if(imsi_info_available == TRUE)
        {
          pdn_limit_throt_info->imsi_data_avail = imsi_info_available;

          if(ds_3gpp_pdn_limit_store_imsi_info_and_init_throt_data (&imsi_info,
                                                                 subs_id) == FALSE)
          {
            DS_3GPP_MSG0_ERROR("Throtting Data Init Failed, clean-up ");
            clean_up_needed = TRUE;
          }
        }
        else
        {
          DS_3GPP_MSG0_HIGH("Unable to Extract IMSI info from MMGSDI");
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Unable to set Info on global struct for PDN Limit Handler");
        clean_up_needed = TRUE;  
      }
    }
  }while(0);

  if(clean_up_needed)
  {
    if(pdn_limit_throt_info != NULL)
    {
      rex_undef_timer(&(pdn_limit_throt_info->sampling_timer));
      modem_mem_free(pdn_limit_throt_info, MODEM_MEM_CLIENT_DATA);
    }
    pdn_limit_throt_info = NULL;

    ds_3gpp_pdn_limit_throt_info_ptr[subs_id] = NULL;
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWERUP_INIT

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
void ds_3gpp_pdn_limit_powerup_init
(
   sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_limit_efs_info_s efs_info;
  /*-------------------------------------------------------------------------*/

  DS_3GPP_MSG0_MED("Initializing PDN Limit Handler");

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG0_ERROR("Subs Id is invalid");
    return;
  }
  
  efs_info.pdn_limit_is_enabled = FALSE;
  efs_info.pdn_req_throttling_time = DEF_PDN_REQ_THROTTLING_TIME;

  if (ds_3gpp_pdn_limit_info_p[subs_id] != NULL)
  {
    DS_LTE_MSG0_ERROR("Pdn connect Info already Initialzied");
    return;
  }

  ds_3gpp_pdn_limit_info_p[subs_id] = 
    (ds_3gpp_pdn_limit_info_s*)
      modem_mem_alloc(sizeof(ds_3gpp_pdn_limit_info_s), MODEM_MEM_CLIENT_DATA);

  if (ds_3gpp_pdn_limit_info_p[subs_id] != NULL)
  {

    if (ds_3gpp_cfg_get_subsid(DS_3GPP_PDN_CONN_LIMIT_HANDLING,
                        (unsigned char *)(&efs_info),
                        sizeof(efs_info),
                        subs_id) == -1)
    {
      DS_LTE_MSG0_ERROR("Cannot read EFS Config item"
                        "DS_3GPP_PDN_CONN_LIMIT_HANDLING, using "
                        "default as FALSE");
    }

 
    ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_limit_is_enabled = 
      efs_info.pdn_limit_is_enabled;
    ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_req_throttling_time = 
      efs_info.pdn_req_throttling_time;
    ds_3gpp_pdn_limit_info_p[subs_id]->pdn_limit_tbl = NULL;

    ds_3gpp_pdn_limit_power_up_throt_init(subs_id);

  }
  else
  {
    DS_LTE_MSG0_ERROR("Cannot allocate memory !");
  }
}

/*===========================================================================
  FUNCTION ds_3gpp_pdn_limit_clear_efs_info_entry_tbl

  DESCRIPTION
  This function clears EFS information stored in the internal table
  
  PARAMETERS  
  subs_id: Current active PS subscription
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_clear_efs_info_entry_tbl
(
  sys_modem_as_id_e_type                     subs_id
)
{
  uint8                                      index = 0;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
  boolean                                    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_3GPP_MSG0_MED("Clearing EFS Entry Table for PDN Limit Persistent throt");

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG0_ERROR("Subs Id is invalid");
    return ret_val;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index ++)
    {
      if(!ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index]))
      {
        DS_3GPP_MSG1_MED("Invalid Entry found at index: %d",index);
        continue;
      }

      modem_mem_free(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                     throt_info_pdn_limit_timer_type_dyn_p, 
                     MODEM_MEM_CLIENT_DATA);
      pdn_limit_throt_info->pdn_limit_timer_tbl[index].
        throt_info_pdn_limit_timer_type_dyn_p = NULL;

    }
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Unable to locate Persistent throttling Entry per subs");
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION ds_3gpp_pdn_limit_clear_persistent_throttle_info_per_subs

  DESCRIPTION
  This function clears PDN Limit persistent throttling information per subs
  ID
  
  PARAMETERS  
  subs_id: Current active PS subscription
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_clear_persistent_throttle_info_per_subs
(
  sys_modem_as_id_e_type                     subs_id
)
{
  boolean                                    ret_val = FALSE;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_3GPP_MSG0_MED("Clearing Persistent throttling info for PDN Limit Hdlr");

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG0_ERROR("Subs Id is invalid");
    return ret_val;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    rex_clr_timer(&pdn_limit_throt_info->sampling_timer);
    rex_undef_timer(&pdn_limit_throt_info->sampling_timer);

    if(ds_3gpp_pdn_limit_clear_efs_info_entry_tbl(subs_id) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Unable to clear entries of EFS info table");
    }

    modem_mem_free(pdn_limit_throt_info, MODEM_MEM_CLIENT_DATA);

    ds_3gpp_pdn_limit_throt_info_ptr[subs_id] = NULL;
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Unable to locate Persistent throttling Entry per subs");
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION ds_3gpp_pdn_limit_clear_per_subs_id

  DESCRIPTION
  This function clears pdn limit info per subs_id
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_clear_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_limit_tbl_s* pdn_limit_tbl_entry_ptr = NULL;
  uint32                   index = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(ds_3gpp_pdn_limit_clear_persistent_throttle_info_per_subs(subs_id) == 
     FALSE)
  {
    DS_3GPP_MSG0_ERROR("Unable to clean-up PDN Limit Persistent Throttle Info");
  }

  pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_subs_id(subs_id);
  if (pdn_limit_tbl_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("pdn_limit_tbl_entry_ptr is NULL");
    return;
  }

  while(index < DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {       
    if(pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index] != NULL)
  {
      (void)rex_clr_timer(&(pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index]->pdn_conn_tmr));
       rex_undef_timer(&(pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index]->pdn_conn_tmr));
       modem_mem_free ((void*)pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index], 
                      MODEM_MEM_CLIENT_DATA);
      pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index] = NULL;
  }
    index = index + 1;
  }
  modem_mem_free ((void*)pdn_limit_tbl_entry_ptr,MODEM_MEM_CLIENT_DATA);

  modem_mem_free ((void*)ds_3gpp_pdn_limit_info_p[subs_id], 
                      MODEM_MEM_CLIENT_DATA);
  ds_3gpp_pdn_limit_info_p[subs_id] = NULL; 

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_ENABLED

  DESCRIPTION
  This function checks whether PDN Limit handling is enabled.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if enabled. FALSE otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_enabled
(
  sys_modem_as_id_e_type          subs_id
)
{
  /*-------------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG0_ERROR("Subs Id is invalid");
    return FALSE;
  }


  if (ds_3gpp_pdn_limit_info_p[subs_id] != NULL)
  {
    DS_3GPP_MSG1_MED("PDN Limit is enabled: %d", 
                     ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_limit_is_enabled);
    return ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_limit_is_enabled;
  }
  else
  {
    DS_3GPP_MSG0_MED("PDN Limit is enabled: 0");
    return FALSE;
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_ALLOC_DYN_MEM

  DESCRIPTION
  This function allocates an entry in the PDN Limit Table.
  
  PARAMETERS  
  None.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index of the entry into the PDN Limit Table
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint8 ds_3gpp_pdn_limit_alloc_dyn_mem
(
  sys_modem_as_id_e_type subs_id
)
{
  uint32                        index = 0;
  ds_3gpp_pdn_limit_tbl_s     *pdn_limit_tbl_entry_ptr = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s *apn_based_tbl_ptr  = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_limit_is_enabled == TRUE);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG0_ERROR("Subs Id is invalid");
    return DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
  }


  pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_subs_id(subs_id);


  if (pdn_limit_tbl_entry_ptr == NULL)
  {
    pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_info_p[subs_id]->pdn_limit_tbl =
          (ds_3gpp_pdn_limit_tbl_s*)
            modem_mem_alloc(sizeof(ds_3gpp_pdn_limit_tbl_s), 
                            MODEM_MEM_CLIENT_DATA);

    if (pdn_limit_tbl_entry_ptr != NULL)
    {
      DS_3GPP_MSG1_HIGH("PDN Limit table allocated at subs id:%d",
                           subs_id);
      memset(pdn_limit_tbl_entry_ptr, 0, sizeof(ds_3gpp_pdn_limit_tbl_s));
      pdn_limit_tbl_entry_ptr->client_data_ptr = (void*)subs_id;
    }
    else
    {
      DATA_ERR_FATAL("Failed to allocate dyn memory");
      return DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
    }
  }

  for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
  {
    if(ds_3gpp_pdn_limit_get_tbl_entry_per_apn((uint8)index, subs_id) == NULL)
    {
      apn_based_tbl_ptr = pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index] =
        (ds_3gpp_pdn_limit_dyn_tbl_s *) modem_mem_alloc
        (sizeof(ds_3gpp_pdn_limit_dyn_tbl_s),MODEM_MEM_CLIENT_DATA);

      if (apn_based_tbl_ptr != NULL)
      {
        DS_3GPP_MSG1_HIGH("PDN Limit table dyn memory allocated at index:%d",
                           index);
        memset(apn_based_tbl_ptr , 0, sizeof(ds_3gpp_pdn_limit_dyn_tbl_s));

        apn_based_tbl_ptr ->client_data_ptr = (void*)index;

      	rex_def_timer_ex
        (
          &(apn_based_tbl_ptr ->pdn_conn_tmr),
          (rex_timer_cb_type) ds_3gpp_pdn_limit_pdn_conn_tmr_expiry_cb,
          (unsigned long)apn_based_tbl_ptr
        );
      }
      else
      {
        DATA_ERR_FATAL("Failed to allocate dyn memory!");
        return DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
      }
      break;
    }
  }

  if (index == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
  {
    DS_3GPP_MSG0_ERROR("Max PDN Limit TBL entries have been allocated");
  }

  return (uint8)index;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_DYN_MEM

  DESCRIPTION
  This function frees an entry in the PDN Limit Table.
  
  PARAMETERS  
  Index of the entry into the PDN Limit Table
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_dyn_mem
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  uint8                        i;
  /*-------------------------------------------------------------------------*/
  ds_3gpp_pdn_limit_free_tbl_entry_per_apn(index, subs_id);

  DS_3GPP_MSG1_HIGH("PDN Limit Table dyn memory at index:%d freed",index);

  for (i=0; i< DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; i++)
  {
    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn
                          (index, subs_id);

    if (per_apn_entry_ptr != NULL)
    {
      DS_3GPP_MSG0_MED("per_apn_entry_ptr is not NULL, breaking");
      break;
    }
  }

  if (i == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
  {
    DS_3GPP_MSG1_HIGH("All indexes are empty,"
                       "deallocate the PDN Limit Table for Subs %d", 
                       subs_id);

    ds_3gpp_pdn_limit_free_tbl_entry_per_subs_id(subs_id);
  }

  return; 
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_PER_BLK

  DESCRIPTION
  This function fetches Max PDN Conn allowed per Block for a Given APN.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn allowed per Time Block.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint16 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_per_blk
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return 0;
  }

  DS_3GPP_MSG2_MED("Max PDN Conn Per Blk for index %d is %d",
                    index, 
                    per_apn_entry_ptr->max_pdn_conn_per_blk);

  return per_apn_entry_ptr->max_pdn_conn_per_blk;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_TIME_BLK_SIZE

  DESCRIPTION
  This function fetches the time block size in mill-seconds.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Time Block Size in milli-secs.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_time_blk_size
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return 0;
  }

  DS_3GPP_MSG2_MED("Time Blk size for index %d is %d secs",
                    index, 
                    per_apn_entry_ptr->time_blk_size);

  return per_apn_entry_ptr->time_blk_size * 1000;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_ALLOWED

  DESCRIPTION
  This function fetches Max PDN Conn allowed in MAX PDN Conn Time.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn Allowed in MAX PDN Conn Time.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  uint32 max_pdn_conn_allowed = 0;
  /*-------------------------------------------------------------------------*/

  if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    DS_3GPP_MSG1_ERROR("Invalid index: %d, returning",index);
    return 0;
  }

  max_pdn_conn_allowed = 
    ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time (
     index, subs_id, ds_3gpp_pdn_limit_get_max_pdn_conn_time(index, subs_id));
                          
  DS_3GPP_MSG2_MED("Max PDN Conns allowed for index %d is %d",
                    index, 
                    max_pdn_conn_allowed);
                              
  return max_pdn_conn_allowed;

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_CURRENT_TIME_BLK

  DESCRIPTION
  This function returns the Current time block based on the Current
  PDN Connection Timer Value.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Time in mill-secs for which the PDN Conn Timer has run.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current Timer Block
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_current_time_blk
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id,
  uint32                 time_in_msec
)
{
  uint32 current_time_blk = 0;
  uint32 current_time_blk_size = 0;
  /*-------------------------------------------------------------------------*/

  if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    DS_3GPP_MSG1_ERROR("Invalid index: %d, returning",index);
    return 0;
  }

  // Put comments
  current_time_blk_size = ds_3gpp_pdn_limit_tbl_get_time_blk_size(index, subs_id);
  if (current_time_blk_size != 0)
  {
    current_time_blk = (time_in_msec + current_time_blk_size - 1) 
                         / current_time_blk_size;
  }

  return current_time_blk;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_PDN_REQ_WAIT_TIMER_IN_PROGRESS

  DESCRIPTION
  This function checks whether the PDN Req Wait Timer is running.
  
  PARAMETERS  
  Index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE or FALSE based on whether PDN Wait Timer is running
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_throttle_sm_s   *pdn_throt_sm_ptr = NULL;
  byte                        *apn = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return FALSE;
  }

  apn = per_apn_entry_ptr->apn;

  pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                     (apn, TRUE, FALSE, DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME,
                      subs_id);

  // Could not find PDN Req throttling SM. This means that throttling is not in
  // progress. 
  if (pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG1_HIGH("PDN Req Wait Timer is not in progress for index %d",
                       index);

    return FALSE;
  }

  DS_3GPP_MSG1_HIGH("PDN Req Wait Timer is in progress for index %d",index);
  return TRUE;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_TIME_REMAINING_IN_CURRENT_BLK

  DESCRIPTION
  This function fetches the time in milli-seconds remaining before the current
  time block ends.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Current time in milli-secs of the PDN Conn Timer.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Time in mill-seconds remaining before the current time block ends.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_tbl_get_time_remaining_in_current_blk
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id,
  uint32                 time_in_msec
)
{
  uint32 current_time_blk = 0;
  uint32 time_remaining_in_current_blk = 0;
  uint32 current_time_blk_end_time;
  /*-------------------------------------------------------------------------*/

  if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    DS_3GPP_MSG1_ERROR("Invalid index: %d, returning",index);
    return 0;
  }

  current_time_blk = 
    ds_3gpp_pdn_limit_tbl_get_current_time_blk(index,subs_id,time_in_msec);

  if (current_time_blk != 0)
  {
    current_time_blk_end_time = 
      (current_time_blk * ds_3gpp_pdn_limit_tbl_get_time_blk_size
                            (index, subs_id));

    if (time_in_msec <= current_time_blk_end_time)
    {
      time_remaining_in_current_blk = current_time_blk_end_time - time_in_msec;
    }
  }
  
  return time_remaining_in_current_blk;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_ALLOWED_IN_GIVEN_TIME

  DESCRIPTION
  This function fetches the Max PDN Connections allowed at any instant of time.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Current value of PDN Con Timer in mill-secs.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Connections allowed in given time.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id,
  uint32                 time_in_msec
)
{
  uint32 max_pdn_conn_allowed = 0;
  uint32 current_time_blk = 0;
  /*-------------------------------------------------------------------------*/

  if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    DS_3GPP_MSG1_ERROR("Invalid index: %d, returning",index);
    return 0;
  }

  current_time_blk = 
    ds_3gpp_pdn_limit_tbl_get_current_time_blk(index, subs_id, time_in_msec);

  max_pdn_conn_allowed =  
    current_time_blk * ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_per_blk
                         (index, subs_id);

  DS_3GPP_MSG3_MED("Max PDN Conns allowed in current time %d "
                   "msecs for index %d is %d", 
                    time_in_msec, index, max_pdn_conn_allowed);


  return max_pdn_conn_allowed;

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_FIND_ENTRY

  DESCRIPTION
  This function finds entry corresponding at a given APN in PDN Limit Table.
  
  PARAMETERS  
  APN Name
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index into the PDN limit Table.
  Returns DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY if cannot find an entry.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_pdn_limit_tbl_find_entry
(
  byte                   *apn,
  sys_modem_as_id_e_type  subs_id
)
{
  uint8                        loop = DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  if (ds_3gpp_pdn_limit_is_enabled(subs_id))
  {
    for( loop = 0; loop < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; loop++)
    {
      per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(loop, subs_id);

      if (per_apn_entry_ptr != NULL)
      {
        if(strncasecmp((char*)per_apn_entry_ptr->apn, 
                       (char *)apn,
                        DS_UMTS_MAX_APN_STRING_LEN+1) == 0)
        {
          DS_3GPP_MSG1_MED("Found entry %d in PDN Limit Tbl", loop);

          break;
        }
      }
    }

    if (loop == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
    {
      DS_3GPP_MSG0_MED("Did not find an entry in PDN Limit Tbl");
    }
  }
  return loop;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_FIND_OR_ADD_ENTRY

  DESCRIPTION
  This function tries to finds an entry corresponding to a given APN in the
  PDN Limit Table. If it cannot find an entry it creates a new entry in the
  PDN Limit Table.
  
  PARAMETERS  
  APN Name
  Max PDN Connections Per Block.
  Max PDN Connection Time.
  PDN Req Wait Time.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index into the PDN limit Table.
  Returns DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY if it cannot make an entry.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_pdn_limit_tbl_find_or_add_entry
(
  byte                   *apn,
  uint16                  max_pdn_conn_per_blk,
  uint16                  max_pdn_conn_time,
  uint16                  pdn_req_wait_time,
  sys_modem_as_id_e_type  subs_id
)
{
  uint8                        index = DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  if (ds_3gpp_pdn_limit_is_enabled(subs_id) == FALSE)
  {
    return index;
  }

  index = ds_3gpp_pdn_limit_tbl_find_entry(apn, subs_id);

  //Could not find an entry corresponding to the APN in PDN Limit TBL
  if (index == DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY )
  {
    //Allocate a new entry in the PDN Limit TBL
    index = ds_3gpp_pdn_limit_alloc_dyn_mem(subs_id);

    //Successfully allocated a new entry in PDN Limit TBL
    if (index != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn
                            (index, subs_id);

      if (per_apn_entry_ptr != NULL)
      {
        (void) strlcpy((char *) per_apn_entry_ptr->apn, 
                       (char *) apn, 
                       DS_UMTS_MAX_APN_STRING_LEN+1);

        per_apn_entry_ptr->max_pdn_conn_per_blk = max_pdn_conn_per_blk;

        per_apn_entry_ptr->max_pdn_conn_time = max_pdn_conn_time;

        per_apn_entry_ptr->pdn_req_wait_time = pdn_req_wait_time;

        per_apn_entry_ptr->time_blk_size = max_pdn_conn_time;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
        return DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
      }
    }
  }

  return index;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_APN_NAME

  DESCRIPTION
  This function updates the APN Name in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New APN name.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_update_apn_name
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id,
  char*                  apn
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                          "APN name: %s updated for index %d in PDN Limit Tbl",
                          apn,
                          index);

  (void) strlcpy((char *) per_apn_entry_ptr->apn, apn, 
                   DS_UMTS_MAX_APN_STRING_LEN+1);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_REMOVE_ENTRY
  DESCRIPTION
  This function removes entry at index 0 from PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_remove_entry
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  if (ds_3gpp_pdn_limit_validate_clear_entry(index, subs_id))
  {
    DS_3GPP_MSG1_MED("Removing entry at index %d",index);

    ds_3gpp_pdn_limit_free_dyn_mem(index, subs_id);
  }
}


/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Connection Timer Expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_pdn_conn_tmr_expiry_cb
(
  unsigned long callback_data
)
{
  ds_cmd_type *cmd_ptr = NULL;
  void        **data_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(void*));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG0_HIGH("Posting DS_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP_CMD");

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP;
  data_ptr = (void**)cmd_ptr->cmd_payload_ptr;
  *data_ptr = (void*)callback_data;

  ds_put_cmd_ext(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_CONN_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Connection Timer expiry event.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_process_pdn_conn_tmr_expiry_cmd
(
  ds_3gpp_pdn_limit_dyn_tbl_s *pdn_limit_dyn_tbl_entry_ptr
)
{
  uint32                       index = 0;
  sys_modem_as_id_e_type       subs_id = SYS_MODEM_AS_ID_NONE;
  /*-------------------------------------------------------------------------*/

  DS_3GPP_MSG1_HIGH("PDN Conn Timer expired at index %d",index);

  subs_id = ds_3gpp_pdn_limit_get_subs_id(pdn_limit_dyn_tbl_entry_ptr);
  index = (uint32)pdn_limit_dyn_tbl_entry_ptr->client_data_ptr;

  ds_3gpp_pdn_limit_reset_pdn_conn_cntr(index, subs_id);
  ds_3gpp_pdn_limit_stop_pdn_wait_tmr(index, subs_id);
  ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
}


/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_TIME

  DESCRIPTION
  This function updates the Max PDN Conn Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Time
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_time
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id,
  uint16                 max_pdn_conn_time
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  per_apn_entry_ptr->max_pdn_conn_time = max_pdn_conn_time;

  per_apn_entry_ptr->time_blk_size = max_pdn_conn_time;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_PER_BLOCK

  DESCRIPTION
  This function updates the Max PDN Conn Per Block in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Per Block.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_per_blk
(
  uint8 index,
  sys_modem_as_id_e_type subs_id,
  uint16 max_pdn_conn_per_blk
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }


  per_apn_entry_ptr->max_pdn_conn_per_blk = max_pdn_conn_per_blk;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_PDN_REQ_WAIT_TIME

  DESCRIPTION
  This function updates the PDN Req Wait Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New PDN Req Wait Time.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_pdn_req_wait_time
(
  uint8 index,
  sys_modem_as_id_e_type subs_id,
  uint16 pdn_req_wait_time
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  per_apn_entry_ptr->pdn_req_wait_time = pdn_req_wait_time;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_CONN_TMR

  DESCRIPTION
  This function starts the PDN Conn Timer if the PDN throttling timer
  is not running and if the PDN Conn Timer has not yet started.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_pdn_conn_tmr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id 
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  DS_3GPP_MSG1_HIGH("Starting PDN Conn Timer at index %d",index);

  if (ds_3gpp_pdn_limit_is_pdn_req_throttling_in_progress(index, subs_id) ==
        FALSE )
  {
    rex_set_timer(&(per_apn_entry_ptr->pdn_conn_tmr),
                  ds_3gpp_pdn_limit_get_max_pdn_conn_time(index, subs_id));
  }
}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_LIMIT_CLR_PDN_CONN_TMR 

DESCRIPTION
  This function clears the PDN connection timer

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_limit_clr_pdn_conn_tmr
(
   uint8                             index,
   sys_modem_as_id_e_type            subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s        *per_apn_entry_ptr  = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  DS_3GPP_MSG0_MED("Clearing pdn conn tmr");

  (void)rex_clr_timer(&(per_apn_entry_ptr->pdn_conn_tmr));

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_PDN_REQ_THROTTLING_IN_PROGRESS

  DESCRIPTION
  This function checks whether the PDN Req Throttling Timer is running.
  
  PARAMETERS  
  Index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE or FALSE based on whether PDN is throttled
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_pdn_req_throttling_in_progress
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_throttle_sm_s   *pdn_throt_sm_ptr = NULL;
  byte                        *apn = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return FALSE;
  }

  apn = per_apn_entry_ptr->apn;

  pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                       (apn, TRUE, FALSE, DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT, 
                          subs_id);

  // Could not find PDN Req throttling SM. This means that throttling is not in
  // progress. 
  if (pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG1_HIGH("PDN Req throttling is not in progress for index %d",
                      index);

    return FALSE;
  }

  DS_3GPP_MSG1_HIGH("PDN Req Throttling is in progress for index %d",index);
  return TRUE;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Req Throttling Timer Expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_pdn_req_throt_tmr_expiry_cb
(
  void* throt_sm_ptr,
  void* callback_data
)
{
  ds_cmd_type                    *cmd_ptr;
  ds_3gpp_pdn_throt_sm_info_type *throt_sm_info_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_throt_sm_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG0_HIGH("Posting DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP");

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP;
  throt_sm_info_ptr = (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

  throt_sm_info_ptr->throt_sm_ptr = throt_sm_ptr;
  throt_sm_info_ptr->subs_id = (int32)callback_data;
  ds_put_cmd_ext(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_STOP_SAMPLING_TIMER_IF_NEEDED

  DESCRIPTION
  This function stops the sampling timer if no APN's are being throttled
  
  PARAMETERS  
  subs_id: Current Active PS Subs
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_stop_sampling_timer_if_needed
(
  sys_modem_as_id_e_type                      subs_id
)
{
  ds_3gpp_pdn_limit_throt_info_type          *pdn_limit_throt_info = NULL;
  uint8                                       index = 
                                           DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid sub id: %d. "
                       "PDN Limit, Unable to Stop Sampling Timer",subs_id);
    return;
  }

  pdn_limit_throt_info = ds_3gpp_pdn_limit_get_throt_info_per_subs(subs_id);

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      if(ds_3gpp_pdn_limit_validate_efs_entry(&pdn_limit_throt_info->
                                               pdn_limit_timer_tbl[index]))
      {
        break;
      }
    }

    if(index == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
    {
      DS_3GPP_MSG0_HIGH("Stopping Sampling Timer since Throtting Timer Exp");
      rex_clr_timer(&pdn_limit_throt_info->sampling_timer);
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Unable to Extract PDN Limit global EFS struct");
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_REQ_THROT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Req Throttle Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_process_pdn_req_throt_tmr_expiry_cmd
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gppi_throttle_sm_s      *ds_3gppi_throt_sm_ptr = NULL;
  uint8                        index = 0;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  char                         apn[DS_UMTS_MAX_APN_STRING_LEN+1];

  /*------------------------------------------------------------------------*/

  ds_3gppi_throt_sm_ptr = (ds_3gppi_throttle_sm_s*)throt_sm_ptr;

  if (ds_3gppi_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_3gppi_throt_sm_ptr is NULL");
    return;
  }

  memset(apn, 0, DS_UMTS_MAX_APN_STRING_LEN+1);
  strlcpy(apn, (const char*)ds_3gppi_throt_sm_ptr->apn,
          DS_UMTS_MAX_APN_STRING_LEN+1);

  ds_3gpp_pdn_limit_reset_efs_entry (ds_3gppi_throt_sm_ptr->apn, subs_id);

  ds_3gpp_pdn_throt_clear_throttle_state
    (ds_3gppi_throt_sm_ptr->apn, DS_UMTS_PDP_IPV4V6, subs_id, 
       DS_3GPP_THROT_RAT_MASK_GLOBAL);

  ds_3gpp_pdn_limit_stop_sampling_timer_if_needed (subs_id);

  if ((index = ds_3gpp_pdn_limit_tbl_find_entry((byte *)apn, subs_id))
       != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {

    DS_3GPP_MSG1_HIGH("PDN Req throttling tmr expired at index %d",index);

    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

    if (per_apn_entry_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
      return;
    }

    ds_3gpp_pdn_limit_reset_pdn_conn_cntr(index, subs_id);

    ds_3gpp_pdn_limit_start_pdn_conn_tmr(index, subs_id);

  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Wait Timer expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/

static void ds_3gpp_pdn_limit_pdn_wait_tmr_expiry_cb
(
  void* throt_sm_ptr,
  void* callback_data
)
{
  ds_cmd_type                    *cmd_ptr;
  ds_3gpp_pdn_throt_sm_info_type *throt_sm_info_ptr = NULL;             
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_throt_sm_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG0_HIGH("Posting DS_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP_CMD");

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP;
  throt_sm_info_ptr = (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

  throt_sm_info_ptr->throt_sm_ptr = throt_sm_ptr;
  throt_sm_info_ptr->subs_id = (int32)callback_data;
  ds_put_cmd_ext(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_WAIT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Wait Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_process_pdn_wait_tmr_expiry_cmd
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gppi_throttle_sm_s      *ds_throt_sm_ptr = NULL;
  uint8                        index = 0;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*------------------------------------------------------------------------*/

  ds_throt_sm_ptr = (ds_3gppi_throttle_sm_s*)throt_sm_ptr;

  if (ds_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("throt_sm_p is NULL");
    return;
  }

  if ((index = ds_3gpp_pdn_limit_tbl_find_entry(ds_throt_sm_ptr->apn, subs_id))
       != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    DS_3GPP_MSG1_HIGH("PDN Req Wait tmr expired at index %d",index);

    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

    if (per_apn_entry_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
      return;
    }

    ds_3gpp_pdn_throt_clear_throttle_state
      (per_apn_entry_ptr->apn, DS_UMTS_PDP_IPV4V6, subs_id,
         DS_3GPP_THROT_RAT_MASK_GLOBAL);

    ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_THROTTLING_TMR

  DESCRIPTION
  This function starts the PDN Req Throttling Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_start_throttling_tmr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_throttle_sm_s *pdn_throt_sm_ptr = NULL;
  byte                      *apn = NULL;
  /*-------------------------------------------------------------------------*/

  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  if (ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_req_throttling_time !=0)
  {
    DS_3GPP_MSG1_HIGH("Starting PDN Req throttling timer at index %d",index);

    apn = per_apn_entry_ptr->apn;

    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                       (apn, TRUE, TRUE, DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT, 
                          subs_id);

    if (pdn_throt_sm_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("pdn_throt_sm_p is NULL");
      return;
    }

    ds_3gppi_throt_sm_register_timer_start_ev_cb(
       pdn_throt_sm_ptr->v4_throttle_sm,
       ds_3gpp_pdn_limit_pdn_req_throt_tmr_start_cb,
       (void *)subs_id);

    ds_3gppi_throt_sm_register_timer_expiry_ev_cb(
      pdn_throt_sm_ptr->v4_throttle_sm,
      ds_3gpp_pdn_limit_pdn_req_throt_tmr_expiry_cb,
      (void*)subs_id);

    ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
      pdn_throt_sm_ptr->v4_throttle_sm,
      ds_3gpp_pdn_limit_get_throttling_tmr_val,
      (void*)subs_id);

    ds_3gpp_pdn_throt_perform_apn_throttling(pdn_throt_sm_ptr,
                                                    apn,
                                                    NULL,
                                                    DS_UMTS_PDP_IPV4V6,
                                                    subs_id,
                                                    TRUE,
                                                    NULL);
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Did not start PDN Req throttling timer at "
                      "index %d as timer val was configured as 0",index);
  }

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_THROTTLING_TMR_VAL

  DESCRIPTION
  This function returns the PDN Req Throttling Timer value.
  
  PARAMETERS  
  callback data 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Req Throttle Timer Value in milli-seconds
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_get_throttling_tmr_val
(
  void *throt_sm_ptr,
  void* cb_data
)
{
  sys_modem_as_id_e_type subs_id;

  subs_id = (sys_modem_as_id_e_type)cb_data;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
      DS_3GPP_MSG0_ERROR("Subs Id is invalid");
      return 0;
  }

  /*-------------------------------------------------------------------------*/
  DS_3GPP_MSG1_MED("PDN Req throttling timer val is %d min",
                    ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_req_throttling_time);

  return ds_3gpp_pdn_limit_info_p[subs_id]->efs_info.pdn_req_throttling_time * 60 * 1000;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_WAIT_TMR

  DESCRIPTION
  This function starts the PDN Req Wait Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_pdn_wait_tmr
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_throttle_sm_s   *pdn_throt_sm_ptr = NULL;
  byte                        *apn = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  if (per_apn_entry_ptr->pdn_req_wait_time !=0)
  {
    DS_3GPP_MSG1_HIGH("Starting PDN Wait timer at index %d",index);

    apn = per_apn_entry_ptr->apn;

    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm
                       (apn,TRUE,TRUE, DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME,
                         subs_id);

    if (pdn_throt_sm_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("pdn_throt_sm_p is NULL");
      return;
    }

    ds_3gppi_throt_sm_register_timer_expiry_ev_cb(
      pdn_throt_sm_ptr->v4_throttle_sm,
      ds_3gpp_pdn_limit_pdn_wait_tmr_expiry_cb,
      (void*)subs_id);

    ds_3gppi_throt_sm_register_get_throttle_timer_val_cb(
      pdn_throt_sm_ptr->v4_throttle_sm,
      ds_3gpp_pdn_limit_get_pdn_wait_tmr_val,
      (void*)subs_id);

    ds_3gpp_pdn_throt_perform_apn_throttling(pdn_throt_sm_ptr,
                                                    apn,
                                                    NULL,
                                                    DS_UMTS_PDP_IPV4V6,
                                                    subs_id,
                                                    TRUE,
                                                    NULL);
  }

  else
  {
    DS_3GPP_MSG1_HIGH("Did not start PDN Req Wait timer at "
                      "index %d as timer val was configured as 0",index);
  }

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_STOP_PDN_WAIT_TMR

  DESCRIPTION
  This function stops the PDN Req Wait Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_stop_pdn_wait_tmr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{
  byte                          *apn = NULL;
  ds_3gpp_pdn_throttle_sm_type   sm_type;
  ds_3gpp_pdn_limit_dyn_tbl_s   *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }


  apn = per_apn_entry_ptr->apn;

  if (ds_3gpp_pdn_throt_get_sm_type(apn,subs_id,TRUE,&sm_type) &&
      sm_type == DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME )
  {

    DS_3GPP_MSG1_HIGH("Stopping PDN Wait timer at index %d",index);

    ds_3gpp_pdn_throt_clear_throttle_state
      (per_apn_entry_ptr->apn, DS_UMTS_PDP_IPV4V6, subs_id,
         DS_3GPP_THROT_RAT_MASK_GLOBAL);    
  }
  else
  {
    DS_3GPP_MSG1_HIGH("PDN Wait timer at index %d was not running. "
                      "No need to stop.", index);
  }

}
/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_PDN_WAIT_TMR_VAL

  DESCRIPTION
  This function returns the PDN Wait Timer Value for a given APN.
  
  PARAMETERS  
  callback data - Throttle SM Ptr from which we obtain APN information
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Wait Timer Value in milli-seconds
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_get_pdn_wait_tmr_val
(
  void* throt_sm_void_ptr,
  void* callback_data
)
{
  uint8                          index = 0;
  ds_3gppi_throttle_sm_s        *throt_sm_ptr = 
                                   (ds_3gppi_throttle_sm_s*)throt_sm_void_ptr;
  sys_modem_as_id_e_type         subs_id = 
                                   (sys_modem_as_id_e_type)callback_data;
  ds_3gpp_pdn_limit_dyn_tbl_s   *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  if ( (index = ds_3gpp_pdn_limit_tbl_find_entry(throt_sm_ptr->apn, subs_id)) != 
          DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

    if (per_apn_entry_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
      return 0;
    }

    DS_3GPP_MSG2_MED("PDN Wait timer value at index %d is %d secs",
                     index,
                     per_apn_entry_ptr->pdn_req_wait_time);

    return per_apn_entry_ptr->pdn_req_wait_time * 1000;
  }

  else
  {
    return 0;
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_MAX_PDN_CONN_TIME

  DESCRIPTION
  This function gets thE Max PDN Conn Time in milli-seconds.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn Time in milli-seconds.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_get_max_pdn_conn_time
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return 0;
  }

  DS_3GPP_MSG2_MED("PDN Conn Timer value at index %d is %d secs",
                   index,
                   per_apn_entry_ptr->max_pdn_conn_time);

  return per_apn_entry_ptr->max_pdn_conn_time * 1000;
}


/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_INCREMENT_PDN_CONN_CNTR

  DESCRIPTION
  This function increments the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_increment_pdn_conn_cntr
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{

  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  per_apn_entry_ptr->pdn_conn_cntr++;

  DS_3GPP_MSG2_HIGH("Incrementing PDN Conn Cntr value at index %d. "
                    "New value %d", index,
                     per_apn_entry_ptr->pdn_conn_cntr);

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_RESET_PDN_CONN_CNTR

  DESCRIPTION
  This function resets the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_reset_pdn_conn_cntr
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return;
  }

  DS_3GPP_MSG1_HIGH("Resetting PDN Conn Ctr at index %d",index);

  per_apn_entry_ptr->pdn_conn_cntr = 0;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_PDN_CONN_CNTR

  DESCRIPTION
  This function fetches the current PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current PDN Conn Cntr.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint16 ds_3gpp_pdn_limit_get_pdn_conn_cntr
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);

  if (per_apn_entry_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("per_apn_entry_ptr is NULL");
    return 0;
  }

  DS_3GPP_MSG2_MED("Current PDN Conn Ctr at index %d is %d",
                    index,per_apn_entry_ptr->pdn_conn_cntr);

  return per_apn_entry_ptr->pdn_conn_cntr;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CALL_BRINGUP_IS_ALLOWED

  DESCRIPTION
  This function checks whether call bringup is allowed for the given APN.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if call is allowed.
  FALSE otherwise.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_call_bringup_is_allowed
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  uint32 current_pdn_conn_tmr_val = 0;
  /*-------------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("Checking whether call bringup is allowed");

  if (ds_3gpp_pdn_limit_get_pdn_conn_cntr(index, subs_id) >=  
        ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed(index, subs_id))
  {
    DS_3GPP_MSG0_ERROR("Max PDN Conn allowed count is reached. "
                       "Rejecting the call and starting the throttling timer"); 
    ds_3gpp_pdn_limit_start_throttling_tmr(index, subs_id);
    return FALSE;  
  }

  current_pdn_conn_tmr_val = 
    ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val(index, subs_id);

  if (current_pdn_conn_tmr_val == 0)
  {
    DS_3GPP_MSG0_HIGH("PDN Conn Timer not yet started or expired, Allow call."); 
    return TRUE;
  }

  if (ds_3gpp_pdn_limit_get_pdn_conn_cntr(index, subs_id) >=  
      ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time
        (index, subs_id, current_pdn_conn_tmr_val))
  {
    DS_3GPP_MSG0_ERROR("Max PDN Conn allowed count for "
                       "current time slice is reached. Rejecting the call");
    return FALSE;  
  }

  DS_3GPP_MSG0_HIGH("All checks passed. Allow call");

  return TRUE;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_SUBS_ID

  DESCRIPTION
  This function fetches the subs id corresponding to a PDN Limit Dyn Tbl Entry
  
  PARAMETERS  
  PDN Limit Dyn Tbl Entry 
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Subs Id
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static sys_modem_as_id_e_type ds_3gpp_pdn_limit_get_subs_id
(
  ds_3gpp_pdn_limit_dyn_tbl_s *pdn_limit_dyn_tbl_entry_ptr 
)
{
  uint8  index;
  int    subs_id = SYS_MODEM_AS_ID_NONE;
  /*-------------------------------------------------------------------------*/

  do 
  {
    if (pdn_limit_dyn_tbl_entry_ptr == NULL)
    {
      break;
    }

    index = (uint32)pdn_limit_dyn_tbl_entry_ptr->client_data_ptr;

    if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      break;
    }

    for (subs_id=0; subs_id < DS3GSUBSMGR_SUBS_ID_MAX; subs_id++ )
    {
      if (ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id) ==
            pdn_limit_dyn_tbl_entry_ptr)
      {
        break;
      }
    }
  } while(0);

  return subs_id;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_TBL_ENTRY_PER_SUBS_ID

  DESCRIPTION
  This function fetches the PDN Limit Table Entry for a Particular subscription
  
  PARAMETERS  
  Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Limit Table Entry for a Particular subscription
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static ds_3gpp_pdn_limit_tbl_s* ds_3gpp_pdn_limit_get_tbl_entry_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_limit_tbl_s *pdn_limit_tbl_entry_ptr = NULL;
  /*-------------------------------------------------------------------------*/
  DS_3GPP_MSG1_MED("Getting Tbl Entry, Subs Id: %d", subs_id);

  do
  {
    if (ds_3gpp_pdn_limit_info_p[subs_id] == NULL)
    {
      DS_3GPP_MSG0_ERROR("ds_3gpp_pdn_limit_info_p is NULL");
      break;
    }

    if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      DS_3GPP_MSG0_ERROR("Subs Id is invalid");
      break;
    }  

    pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_info_p[subs_id]->pdn_limit_tbl;

    DS_3GPP_MSG2_MED("pdn_limit_tbl_entry_ptr: 0x%x for Subs Id: %d", 
                     pdn_limit_tbl_entry_ptr,subs_id);

  }while(0);

  return pdn_limit_tbl_entry_ptr;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_TBL_ENTRY_PER_SUBS_ID

  DESCRIPTION
  This function frees the PDN Limit Table Entry for a Particular subscription
  
  PARAMETERS  
  Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_tbl_entry_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  /*-------------------------------------------------------------------------*/
  do
  {
    if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      DS_3GPP_MSG0_ERROR("Subs Id is invalid");
      break;
    }

    if (ds_3gpp_pdn_limit_info_p[subs_id] == NULL)
    {
      DS_3GPP_MSG0_ERROR("ds_3gpp_pdn_limit_info_p is NULL");
      break;
    }

    DS_3GPP_MSG1_MED("Freeing entry for for Subs Id: %d", subs_id);

    modem_mem_free ((void*)ds_3gpp_pdn_limit_info_p[subs_id]->pdn_limit_tbl, 
                      MODEM_MEM_CLIENT_DATA);

    ds_3gpp_pdn_limit_info_p[subs_id]->pdn_limit_tbl = NULL;

  }while(0);

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_TBL_ENTRY_PER_APN

  DESCRIPTION
  This function fetches the PDN Limit Table Entry at a particular index and
  for a particular subscription
  
  PARAMETERS
  1. Index
  2. Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Limit Table Entry at a particular index and
  for a particular subscription
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static ds_3gpp_pdn_limit_dyn_tbl_s* ds_3gpp_pdn_limit_get_tbl_entry_per_apn
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s* pdn_limit_dyn_tbl_ptr = NULL;
  ds_3gpp_pdn_limit_tbl_s*     pdn_limit_tbl_entry_ptr = NULL;
  /*-------------------------------------------------------------------------*/
  DS_3GPP_MSG2_MED("Getting Tbl Entry, Subs Id: %d, Index: %d", subs_id, index);

  do
  {
    pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_subs_id(subs_id);

    if (pdn_limit_tbl_entry_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("pdn_limit_tbl_entry_ptr is NULL");
      break;
    }

    if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      DS_3GPP_MSG1_ERROR("Invalid index: %d",index);
      break;
    }

    pdn_limit_dyn_tbl_ptr = pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index];

  }while (0); 

  return pdn_limit_dyn_tbl_ptr;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_TBL_ENTRY_PER_APN

  DESCRIPTION
  This function frees the PDN Limit Table Entry at a particular index and
  for a particular subscription
  
  PARAMETERS
  1. Index
  2. Subs Id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_tbl_entry_per_apn
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_limit_tbl_s* pdn_limit_tbl_entry_ptr = NULL;
  /*-------------------------------------------------------------------------*/
  DS_3GPP_MSG2_MED("Freeing Tbl Entry, Subs Id: %d, Index: %d", subs_id, index);

  do
  {
    pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_subs_id(subs_id);

    if (pdn_limit_tbl_entry_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("pdn_limit_tbl_entry_ptr is NULL");
      break;
    }

    if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      DS_3GPP_MSG1_ERROR("Invalid index: %d",index);
      break;
    }

    rex_undef_timer(&(pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl
                      [index]->pdn_conn_tmr));

    modem_mem_free ((void*)pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index], 
                      MODEM_MEM_CLIENT_DATA);

    pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index] = NULL;

  }while (0); 

  return;

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_CMDS

  DESCRIPTION
  This function processes any commands that need to be handled by
  PDN Connection Limit Module.
  
  PARAMETERS
  1. Cmd Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_cmds
(
  const ds_cmd_type      *cmd_ptr
)
{
  ds_3gpp_pdn_throt_sm_info_type *throt_sm_info_ptr = NULL;
  /*------------------------------------------------------------------------*/

  if(cmd_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  switch (cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP:
    {
      if(cmd_ptr->cmd_payload_ptr == NULL)
      {
        DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
        return;
      }

      ds_3gpp_pdn_limit_process_pdn_conn_tmr_expiry_cmd(
        (ds_3gpp_pdn_limit_dyn_tbl_s*)(*((void**)cmd_ptr->cmd_payload_ptr)));
      break;
    }

    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP:
    {
      throt_sm_info_ptr = 
        (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

      if(throt_sm_info_ptr == NULL)
      {
        DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
        return;
      }

      ds_3gpp_pdn_limit_process_pdn_req_throt_tmr_expiry_cmd
        (throt_sm_info_ptr->throt_sm_ptr,throt_sm_info_ptr->subs_id);
      break;
    }
    case DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP:
    {
      throt_sm_info_ptr = 
        (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

      if(throt_sm_info_ptr == NULL)
      {
        DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
        return;
      }

      ds_3gpp_pdn_limit_process_pdn_wait_tmr_expiry_cmd
        (throt_sm_info_ptr->throt_sm_ptr,throt_sm_info_ptr->subs_id);
      break;
    }
    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START:
    {
      throt_sm_info_ptr = 
        (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

      if(throt_sm_info_ptr == NULL)
      {
        DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
        return;
      }

      ds_3gpp_pdn_limit_process_pdn_req_tmr_start_cmd
        (throt_sm_info_ptr->throt_sm_ptr,throt_sm_info_ptr->subs_id);
      break;
    }
    case DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP:
    {
      ds_3gpp_pdn_limit_process_sampling_timer_exp_cmd
        ((sys_modem_as_id_e_type)cmd_ptr->cmd_payload_ptr);
      break;
    }
    default:
    {
      DS_3GPP_MSG1_ERROR("Unrecognized cmd: %d, ignoring",cmd_ptr->hdr.cmd_id);
      break;
    }
  }

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_VALIDATE_CLEAR_ENTRY

  DESCRIPTION
  This function determines whether the entry corresponding to the given
  index and subs_id can be cleared from the PDN limit table.
  
  PARAMETERS
  1. Index into the PDN Limit Table.
  2. Subs id
 
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_pdn_limit_validate_clear_entry
(
  uint8                   index,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s* per_apn_entry_ptr = NULL;
  boolean                      clear_entry = TRUE;
  ds_pdn_context_s             *v4_pdn_cntx_p = NULL;
  ds_pdn_context_s             *v6_pdn_cntx_p = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index, subs_id);
  do
  {
    /* If no entry is found for the index, subs id passed in,
       we have nothing to clear*/
    if (per_apn_entry_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Did not find any entry in PDN Limit Table");
      clear_entry = FALSE;
      break;
    }

    /* If Throttling is in progress or PDN Connection Timer is running,
       dont clear the entry*/
    if (ds_3gpp_pdn_limit_is_pdn_req_throttling_in_progress(index, subs_id) ||
        ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress(index, subs_id) ||
        ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val(index, subs_id) != 0)
    {
      DS_3GPP_MSG1_HIGH("Not clearing entry at index %d ",index);
      clear_entry = FALSE;
      break;
    }

    /* If a V4 or a V6 PDN Context exists for the give APN, do not clear the entry.
       This is done to ensure that when the PDN Connection is terminated,
       PDN Wait Timer can be started */
    if (ds_pdn_cntxt_get_v4_pdn_context
          (per_apn_entry_ptr->apn, &v4_pdn_cntx_p, subs_id ) ||
        ds_pdn_cntxt_get_v6_pdn_context
          (per_apn_entry_ptr->apn, &v6_pdn_cntx_p, subs_id ))
    {
      DS_3GPP_MSG2_HIGH("Found a valid pdn context, v4: 0x%x, v6: 0x%x, "
                        "Call still active. Not clearing entry ", 
                         v4_pdn_cntx_p, v6_pdn_cntx_p);
      clear_entry = FALSE;
      break;
    }
    else
    {
      DS_3GPP_MSG0_HIGH("No PDNs active for given APN. Clearing entry ");
      clear_entry = TRUE;
    }
  }
  while (0);

  return clear_entry;
}

/*==============================================================================
FUNCTION DS_3GPP_PDN_CNTXT_VALIDATE_AND_UPDATE_PDN_LIMIT_ENTRY
 
DESCRIPTION
  This function initially checks if pdn limit parameters bring passed are non-
  default , 
    If Yes: Add an entry in PDN limit table
    If NO : Check all other profile for non-default pdn limit parameter
            with same apn
            If Yes : Add an entry in PDN limit table with non-default parameters
            If No  : Add an entry in PDN limit table default parameters
 
  Assumptions made 
  1: Non-default Profile PDN limit parameter will be updated over default Profile
     PDN limit parameters with the same APN
  2. First non-default Profile PDN limit parameters will be chosen over multiple
     non-default Profile parameter given they have same APN

PARAMETERS 
  uint8  Profile_id :  Profile ID being passed
  byte profiel_apn  :  APN name of Profile ID being passed
  subs_id           :  Subscription ID
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean DS3G_FAILURE : Failure to update PDN Limit Entry
          DS3G_SUCCESS : Sucessfully update PDN Limit Entry
 
SIDE EFFECTS 
  None 
  
==============================================================================*/
ds_umts_pdp_profile_status_etype ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry
(
 uint8                  profile_id,
 uint8                  *handle_id,
 sys_modem_as_id_e_type subs_id
)
{
  uint8                            profile_nums_arr[DS_UMTS_MAX_PDP_PROFILE_NUM];
  uint8                            profile_count,i;
  boolean                          ret_val = FALSE;
  byte                             profile_apn[DS_UMTS_MAX_APN_STRING_LEN+1];
  uint16                           max_pdn_conn_per_blk ; 
  uint16                           max_pdn_conn_time ; 
  uint16                           pdn_req_wait_time ;  
  ds_umts_pdp_profile_status_etype result = DS3G_FAILURE;
  ds_umts_pdp_profile_type         *pdp_profile_ptr = NULL; 
  
/*-----------------------------------------------------------------------------*/

  pdp_profile_ptr = (ds_umts_pdp_profile_type*) 
                     modem_mem_alloc(sizeof(ds_umts_pdp_profile_type),
                     MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
  -------------------------------------------------------------------------*/
  if(pdp_profile_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to pdp_profile_ptr failed");
    goto exit_with_error;
  }
  
  memset(profile_apn, 0, sizeof(DS_UMTS_MAX_APN_STRING_LEN+1));

  result = ds_umts_get_pdp_profile_context_info_apn_per_subs(profile_id,
                                       dsumts_subs_mgr_get_subs_id(subs_id),
                                                    profile_apn,
                                                    DS_UMTS_MAX_APN_STRING_LEN);
  if(result != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG0_ERROR("Invalid Profile num passed , no profile found");
    goto exit_with_error;
  }

  // Update Parameters with Profile values
  result = ds_umts_get_pdp_profile_max_pdn_conn_time_per_subs(profile_id,
                                                dsumts_subs_mgr_get_subs_id(subs_id),
                                                &max_pdn_conn_time);
  if(result != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG0_ERROR("Invalid  max_pdn_conn_time parameter");
    goto exit_with_error;
  }

  result = ds_umts_get_pdp_profile_max_pdn_conn_per_blk_per_subs(profile_id,
                                               dsumts_subs_mgr_get_subs_id(subs_id),
                                                        &max_pdn_conn_per_blk);
  if(result != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG0_ERROR("Invalid max_pdn_conn_per_blk parameter");
    goto exit_with_error;
  }
 
  result = ds_umts_get_pdp_profile_pdn_req_wait_time_per_subs(profile_id,
                                              dsumts_subs_mgr_get_subs_id(subs_id),
                                                     &pdn_req_wait_time);
  if(result != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG0_ERROR("Invalid pdn_req_wait_time parameter");
    goto exit_with_error;
  }

  // Verify pdn limit parameter for profile being passed is default 
  ret_val = ds_3gpp_pdn_context_is_pdn_limit_param_default(max_pdn_conn_per_blk,
                                                           max_pdn_conn_time,
                                                           pdn_req_wait_time);
  
  if (ret_val) 
  {
    // Enter this condition when Profile pdn limit param is default parameters 
    profile_count = ds_3gpp_profile_cache_get_valid_profile_numbers(
                                             dsumts_subs_mgr_get_subs_id(subs_id),
                                             profile_nums_arr, 
                                                         DS_UMTS_MAX_PDP_PROFILE_NUM);
    // Loop through all active profile     
    for (i=0; i< profile_count; i++)
    {
      if (profile_nums_arr[i] == profile_id) 
      {
        continue;
      }

      memset(pdp_profile_ptr, 0, sizeof(ds_umts_pdp_profile_type));

      result = ds_umts_get_pdp_profile_all_data_per_subs(profile_nums_arr[i],
                                       dsumts_subs_mgr_get_subs_id(subs_id), pdp_profile_ptr);

      if(result != DS_UMTS_PDP_SUCCESS)
      {
        continue;
      }
 
      // Compare APN names 
      ret_val = ds_3gpp_compare_apn_names((byte*)pdp_profile_ptr->context.apn,(byte*)profile_apn); 
      
      if((ret_val == TRUE) &&  
         (!ds_3gpp_pdn_context_is_pdn_limit_param_default(pdp_profile_ptr->max_pdn_conn_per_blk,
                                                          pdp_profile_ptr->max_pdn_conn_time,
                                                          pdp_profile_ptr->pdn_req_wait_time)))
      {
        // Found profile with non-default PDN limit param with same APN being passed
        DS_3GPP_MSG1_HIGH("PDN limit parameters updated with "
                          "profile %d info",profile_nums_arr[i]);
        max_pdn_conn_per_blk = pdp_profile_ptr->max_pdn_conn_per_blk ;
        max_pdn_conn_time = pdp_profile_ptr->max_pdn_conn_time ;
        pdn_req_wait_time = pdp_profile_ptr->pdn_req_wait_time  ;
        break;
      }
    }
  }
  else
  {
    DS_3GPP_MSG1_HIGH("PDN limit parameters updated with "
                      "profile %d info",profile_id);
  }
  // Add an entry into PDN limit table
  *handle_id = ds_3gpp_pdn_limit_tbl_find_or_add_entry
              ( profile_apn,
                max_pdn_conn_per_blk,
                max_pdn_conn_time,
                pdn_req_wait_time, 
                subs_id             
              );
                                         
  DS_3GPP_MSG3_HIGH("Updating PDN limit parameters "
                     "max_pdn_conn_per_blk %d , max_pdn_conn_time %d ,"
                     "pdn_req_wait_time %d " ,
                      max_pdn_conn_per_blk,
                      max_pdn_conn_time,
                      pdn_req_wait_time);

  if (pdp_profile_ptr != NULL) 
  {
    modem_mem_free(pdp_profile_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
  }
  return DS3G_SUCCESS;

exit_with_error:
  if (pdp_profile_ptr != NULL) 
  {
    modem_mem_free(pdp_profile_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
  }
  return DS3G_FAILURE; 
}/*ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_IS_PDN_LIMIT_PARAM_DEFAULT
 
DESCRIPTION
  This function verifies parameters present in profile being passed 
  are non default param

PARAMETERS 
  profile_info: Profile information
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean TRUE: PDN limit parameters present in profile are default
          FALSE: PDN limit parameters present in profile are non-default
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_pdn_context_is_pdn_limit_param_default
(
  uint16                max_pdn_conn_per_blk,
  uint16                 max_pdn_conn_time,
  uint16                 pdn_req_wait_time
)
{
  boolean result = FALSE;
  result = ((max_pdn_conn_per_blk != DS_UMTS_PDP_DEF_MAX_PDN_CONN_PER_BLK) ||
            (pdn_req_wait_time != DS_UMTS_PDP_DEF_PDN_REQ_WAIT_TIME) ||
            (max_pdn_conn_time != DS_UMTS_PDP_DEF_MAX_PDN_CONN_TIME));
  return !result;
}/*ds_3gpp_pdn_context_is_pdn_limit_param_default*/

#endif /* FEATURE_DATA_LTE */
