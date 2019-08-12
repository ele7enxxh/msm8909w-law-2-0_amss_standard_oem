#ifndef PS_IPDC_DSMUTIL_H
#define PS_IPDC_DSMUTIL_H

/*!
  @file
  ipdc_dsmutil.h

  @brief
  This file contains the interface of dsm apis needed for IPDC library

  @author
  araina

*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/dc/ipdc/v1/inc/ps_ipdc_dsmutil.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/10/15   ar      initial revision

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <dsm_item.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief
    This Macro provides the compiler with branch prediction information.

    Calling IPDC_LIKELY(expr) informs the compiler that it is expected that
    expr == 1 <NOTE: expr has to be equal to 1 not greater than 1>.

    Calling IPDC_UNLIKELY(expr) informs the compiler that it is expected
    that expr is going to be 0, hence dont perform prefetching of instructions
    inside.

    @usage
    Use "if (IPDC_LIKELY(a==b))" {} if expression '(a==b)' is expected to be
    TRUE most of the time else use "if (IPDC_UNLIKELY(a==b)"
*/
#if defined(__GNUC__) && !defined(_lint)
  #define IPDC_LIKELY(x) __builtin_expect((x), 1)
  #define IPDC_UNLIKELY(x) __builtin_expect((x), 0)
#else
  #define IPDC_LIKELY(x) (x)
  #define IPDC_UNLIKELY(x) (x)
#endif /* ! (__GNUC__) */


/*==============================================================================

                         EXTERNAL VARIABLES

==============================================================================*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  ipdc_dsmutil_init

==============================================================================*/
/*!
    @brief
    Function initializes the ipdc dsmutil module. As part of initialization it 
    allocates some large pool DSM items.

    @return
    TRUE - if initialization is successful
    FALSE- if initialization fails
*/
/*============================================================================*/
EXTERN boolean ipdc_dsmutil_init
(
  void
);

/*==============================================================================

  FUNCTION:  ipdc_dsmutil_deinit

==============================================================================*/
/*!
    @brief
    Function de-initializes the ipdc dsmutil module. As part of 
    de-initialization it frees up the allocated dsm items.

    @return
    none
    
*/
/*============================================================================*/
EXTERN void ipdc_dsmutil_deinit
(
  void
);

/*==============================================================================

  FUNCTION:  ipdc_dsmutil_new_buffer

==============================================================================*/
/*!
    @brief
    Function allocates a large pool dsm item.
    - Allocated DSM item will NOT be a chain of dsm items

    @return
    dsm_item_type* - if DSM allocation is successful
    NULL           - if DSM allocation is unsuccessful
*/
/*============================================================================*/
EXTERN dsm_item_type* ipdc_dsmutil_new_buffer
(
  uint32 req_size_in_bytes /*!< requested size of the dsm item in bytes */
);

#endif