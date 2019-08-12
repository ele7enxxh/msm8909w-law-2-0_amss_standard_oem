/*!
  @file
  rflm_dm_handle.h

  @brief
  The header file include definitions to the RFLM handle type.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/dm/inc/rflm_dm_handle.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/14   aw      Moved device_id and sub_id to handle meta
08/04/14   cj      Added Sub id in rflm handle to support tx sharing feature
02/11/14   pl      Added disassocation API
10/18/13   qwu     Added mulple script buffers
09/11/13   ka      Added fbrx DM support
09/06/13   gy      Added dm support for rxagc
09/06/13   jyw     Added init support
08/15/13   hdz     Increased the handle script buf from 16 to 50
08/08/13   vrb     Increase the number of RFLM handles to 16 (11 Rx, 5 Tx)
07/26/13   jyw     Created the initial version
===========================================================================*/

#ifndef RFLM_DM_HANDLE
#define RFLM_DM_HANDLE

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm_dm_api.h"
#include "rflm_dm_buf_mgr_t.h"
#include "rflm_txagc.h"
#include "rflm_rxagc_api.h"
#ifndef FEATURE_D3925_FED
#include "rflm_api_fbrx.h"
#endif
#ifdef FEATURE_RF_AUTOPIN
#include "rflm_api_autopin.h"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief this is the usr data ptr saved in the handle
*/
typedef uint32* rflm_dm_handle_usr_data_t;


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  /*!< meta information for the lm handle */
  rflm_dm_handle_meta_s meta; 

  rflm_dm_handle_status_e status;

  /*!< the dtr buffer ptr, based on the handle type, it could be either tx DTR 
     or rx DTR buffer*/
  uint32* dtr_buf_ptr; 

  /*! < script pool, idx is mapping to an enum */
  // payload pointer
  uint32* scripts[RFLM_MAX_NUM_HANDLE_SCRIPT_BUF];

  /*! < common txagc ptr, valid only for lm tx handles */
  //int32 txagc_id;
  rflm_txagc_event_template_t* txagc_ptr;
  //void* commontxagc_ptr;
  //txagc_handle_id;

  /*! < common rxagc ptr, valid only for lm rx handles */
  rflm_rxagc_event_template_t* rxagc_ptr;
 
  /*! < fbrx data ptr, valid only for lm tx handles */
#ifndef FEATURE_D3925_FED
  rflm_fbrx_dm_template_t* fbrx_ptr;
#endif

#ifdef FEATURE_RF_AUTOPIN
  rflm_autopin_dm_template_t* autopin_ptr;
#endif
    
  /*! < user data ptr */
  rflm_dm_handle_usr_data_t usr_ptr;  

  /*! < taskq handle */
  void *tq_ptr;
} rflm_dm_handle_s; 



class HandlePool: public MyBufMgrTemplate<rflm_dm_handle_s, RFLM_HANDLE_COUNTS>
{
public:
  HandlePool();
  void preset_buf(void);
  rflm_dm_handle_id_t get_new_handle (const rflm_dm_handle_meta_s* input_meta_ptr, 
                                      rflm_dm_handle_usr_data_t usr_ptr, 
                                      rflm_txagc_event_template_t* txagc_id,
                                      rflm_rxagc_event_template_t* rxagc_id,
#ifndef FEATURE_D3925_FED
                                      rflm_fbrx_dm_template_t* fbrx_id,
#endif
#ifdef FEATURE_RF_AUTOPIN
                                      rflm_autopin_dm_template_t* autopin_id,
#endif
                                      uint32* dtr_buf_ptr);
  boolean delete_handle(rflm_dm_handle_id_t handle_id);
  boolean delete_handle_and_scripts(rflm_dm_handle_id_t handle_id);
  boolean get_meta_info(rflm_dm_handle_id_t handle_id, rflm_dm_handle_meta_s* meta_ptr);
  rflm_dm_handle_meta_s * get_meta_ptr(rflm_dm_handle_id_t handle_id);
  boolean associate_script(rflm_dm_handle_id_t handle_id, uint32 script_idx, uint32* script_buf_ptr);
  boolean disassociate_script(rflm_dm_handle_id_t handle_id, uint32 script_idx, uint32* script_buf_ptr);
  int32 get_handle_pool_size(void);
  boolean validate_handle_id(rflm_dm_handle_id_t handle_id); 
  uint32* get_script_buf_ptr(rflm_dm_handle_id_t handle_id, uint32 script_idx);
  uint32* get_dtr_buf_ptr(rflm_dm_handle_id_t handle_id);
  rflm_dm_handle_usr_data_t get_usr_data_ptr(rflm_dm_handle_id_t handle_id);
  rflm_txagc_event_template_t* get_txagc_ptr(rflm_dm_handle_id_t handle_id);
  rflm_rxagc_event_template_t* get_rxagc_ptr(rflm_dm_handle_id_t handle_id);

  void * get_tq_ptr(rflm_dm_handle_id_t handle_id);
  boolean store_tq_ptr(rflm_dm_handle_id_t handle_id, void * tq_ptr);

#ifndef FEATURE_D3925_FED
  rflm_fbrx_dm_template_t* get_fbrx_ptr(rflm_dm_handle_id_t handle_id);
#endif

#ifdef FEATURE_RF_AUTOPIN
  rflm_autopin_dm_template_t* get_autopin_ptr(rflm_dm_handle_id_t handle_id);
#endif

#ifdef FEATURE_TX_SHARING
  boolean store_device_info(rflm_dm_handle_id_t handle_id, rflm_dm_device_id_t dev_id,rflm_dm_sub_id_t sub_id);
#endif

};

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* RFLM_DM_HANDLE */
