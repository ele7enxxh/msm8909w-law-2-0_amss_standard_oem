#ifndef RFLM_CMN_RXAGC_H
#define RFLM_CMN_RXAGC_H
/*
  @file
  rflm_cmn_rxagc.h

  @brief
  This file contains RFLM RxAGC definitions, enumerations and prototypes.

  @details
  This file defines RFLM RxAGC interfaces, APIs, and common structures

  @addtogroup RFLM_RXAGC
  @{
*/

/*=============================================================================

                R F L M CMN RXAGC   H E A D E R   F I L E 
       
Copyright (c) 2013-2014  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rxagc/inc/rflm_cmn_rxagc.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------- 
07/30/14   zhw    Map CMN LM buffer count to RFLM RX BUF counts
07/21/14   zhw    Cmn RxAGC memory reduction
12/16/13   sty    Added rflm_cmn_rxagc_init_module                  
                  Tied RFLM_CMN_RXAGC_MAX_LM_BUFFERS to RXLM_MAX_BUFFERS
11/6/13    sar    Updated comments.
11/05/13   sar    Increased Max LM buffers to 9.
09/05/13   gy     Added dm support for rxagc
07/22/13   sty    Added function prototypes
07/03/13   sty    Initial revision

==============================================================================*/
#include "rflm_dm_dtr_buf.h"
#include "rflm_cmn_rxagc_data.h"
#include <qurt.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The common RxAGC module supports these many number of WB buffers. Map to
  RFLM RX BUFFER counts
*/
#define RFLM_CMN_RXAGC_MAX_LM_BUFFERS                  RFLM_DM_DTR_RX_BUF_COUNTS              

/*----------------------------------------------------------------------------*/
rflm_rxagc_event_template_t*
rflm_cmn_rxagc_allocate_and_init_wb_buffer
(
  const rflm_tech_id_t tech
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_deallocate_wb_buffer
(
  rflm_rxagc_event_template_t* rxagc_ptr
);

/*----------------------------------------------------------------------------*/
rflm_rxagc_cmn_ctrl_out_t
rflm_cmn_rxagc_execute_script
(
  int dummy_arg
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_set_data
(
  void* ptr_to_wb_buffer,
  rflm_cmn_rxagc_cmd_e cmd,
  const void* payload
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_get_data
(
  void* ptr_to_wb_buffer,
  rflm_cmn_rxagc_cmd_e cmd,
  void* payload
);

/*----------------------------------------------------------------------------*/
boolean
rflm_cmn_rxagc_init_module
(
  void
);

/*----------------------------------------------------------------------------*/
qurt_mutex_t*
rflm_cmn_rxagc_access_mutex_get_ptr
(
  void
);

#ifdef __cplusplus
}
#endif

/*!   @} */

#endif /* #ifndef RFLM_CMN_RXAGC_H */
