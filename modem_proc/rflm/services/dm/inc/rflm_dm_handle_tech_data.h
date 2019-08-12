/*!
  @file
  rflm_dm_handle_tech_data.h

  @brief
  The header file contains the type definition to the tech buffer.

  @detail
  OPTIONAL detailed description of this C header file.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/dm/inc/rflm_dm_handle_tech_data.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/02/14    pv     Added support for single TQ operation for all DL CA.
12/13/13    dw     Add inclusion of rflm_wcdma_sw_api.h
10/18/13   qwu     Changed char to uint32
10/09/13    ka     Added wcdma tech buffer type
7/19/13    jyw     initial check in
===========================================================================*/

#ifndef RFLM_DM_HANDLE_TECH_DATA_H
#define RFLM_DM_HANDLE_TECH_DATA_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm_dm_api.h"
#include "rflm_dm_buf_mgr_t.h"
#include "rflm_dm_handle_tech_data.h"
#include "rflm_wcdma_sw_api.h"
#include "rflm_lte.h"

/*!@todo include the tech data type header file here */

#define TECH_DATA_SIZE_UINT32 (sizeof(rflm_wcdma_dm_buf_type) + 3 )/4

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Access history type 
*/
typedef enum
{
  RFLM_TECH_DATA_CREATED = 0,
  RFLM_TECH_DATA_UPDATED,
  RFLM_TECH_DATA_FETCHED,
  RFLM_TECH_DATA_DELETED,
  RFLM_TECH_MAX_NUM_ACCESS_TYPES
}rflm_dm_handle_tech_data_access_type_e;

/*! @brief RFSW access history type 
*/
typedef struct
{
  rflm_dm_handle_tech_data_access_type_e access_type;
  uint32 ustmr;
}rflm_dm_handle_tech_data_access_history_s;


/*! @brief Hanlde tech data structure 
*/
typedef union
{
  /*!< Generic DM type */
  uint32 uint32_ptr[TECH_DATA_SIZE_UINT32];
  rflm_wcdma_dm_buf_type wcdma_data;
  rflm_lte_dm_buf_type   lte_data;

  /*!@todo Tech data type add below */
}rflm_dm_handle_tech_data_u;


/*! @brief Access info structure 
*/
typedef struct
{
  /*!< current index */
  uint32 curr_idx;
    
  /*!< History array*/
  rflm_dm_handle_tech_data_access_history_s hist[TECH_DATA_ACCESS_HISTORY_SIZE];
}rflm_dm_handle_tech_data_access_info_s;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  /*!< Handle specific tech data*/
  rflm_dm_handle_tech_data_u tech_data;

  /*!< Barrier to detect the overwrite */
  uint32 barrier_pattern;

  /*!< access info structure*/
  rflm_dm_handle_tech_data_access_info_s access_info;

} rflm_dm_handle_tech_data_s; 



/*! @brief Tech data pool class definition derived from the generic buffer model */
class TechDataPool: private MyBufMgrTemplate<rflm_dm_handle_tech_data_s, TECH_DATA_POOL_SIZE>
{
public:
  TechDataPool(){init(); preset();}
  void preset(void);
  uint32* new_tech_data(void);
  boolean delete_tech_data(uint32* data_ptr);
  void update_access_history(uint32* data_ptr, uint32 access_type);
  boolean validate_buf(uint32* data_ptr);
};

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* RFLM_DM_HANDLE_TECH_DATA_H */
