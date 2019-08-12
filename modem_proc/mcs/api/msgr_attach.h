/*!
  @file   msgr_attach.h

  @brief  Message Router header file for attachments

  @detail
  Defines Message Router types and interfaces used for attachments.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/msgr_attach.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/13/10   ejv     Update header file inclusions.
12/08/10   zm      Feature cleanup - SU API effort.
11/09/10   zm      Moved file into mcs/api folder - SU API effort.
07/27/09   ejv     Remove qobject attachment references.
11/21/08   ejv     Initial Revision

===========================================================================*/

#ifndef MSGR_ATTACH_H
#define MSGR_ATTACH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>    /* Definition of basic types and macros */
#include <dsm_pool.h>  /* DSM item */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef uint8  msgr_attach_type_t;
typedef msgr_attach_type_t msgr_attach_type_type;

#define MSGR_ATTACH_NONE    0x00
#define MSGR_ATTACH_DSM     0x01
#define MSGR_ATTACH_GENERAL 0x02
#define MSGR_ATTACH_REFCNT  0x03

/*! @brief DSM attachment type
 */
typedef struct
{
  dsm_item_type *dsm_ptr;        /*!< DSM item to be shared */
  boolean        meta_info_incl; /*!< Indicates if DSM share info is present*/
  uint32         meta_info_ptr;  /*!< Ptr with Control mesg for sharing */
} msgr_dsm_attach_s;

/*! @brief Shared memory attachment type
 */
typedef struct
{
  void   *ptr;
  uint32  size;
} msgr_general_attach_s;

/*! @brief Ref count attachment type
 */
typedef struct
{
  void *refcnt_obj_ptr;
  //Space for other meta data        
} msgr_refcnt_attach_s;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Union of various attachment types
 */
typedef union
{
  msgr_dsm_attach_s      dsm;
  msgr_general_attach_s  general;
  msgr_refcnt_attach_s   refcnt;
} msgr_attach_data_u;

/*! @brief Data format for message attachments
 */
typedef struct
{
  msgr_attach_type_t      type;
  msgr_attach_data_u      data;
} msgr_attach_s;
typedef msgr_attach_s msgr_attach_struct_type;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* MSGR_ATTACH_H */
