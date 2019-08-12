/*!
  @file
  qsh_clt_dsm.h

  @brief
  Header file for DSM client implementation.
*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/image_qsh_ext.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 

==============================================================================*/

#ifndef IMAGE_QSH_EXT_H
#define IMAGE_QSH_EXT_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <qsh.h>

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/
#define IMAGE_QSH_MAJOR_VER 1
#define IMAGE_QSH_MINOR_VER 1

/*!
  @brief 
  Header for dump meta information.
*/
#define QSH_DUMP_META_HDR_LOG_TIME_SIZE_BYTES   32
#define QSH_DUMP_META_HDR_CHIPSET_SIZE_BYTES    32
#define QSH_DUMP_META_HDR_IMG_VER_SIZE_BYTES    64
#define QSH_DUMP_META_HDR_IMG_TIME_SIZE_BYTES   32


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/******************************************************************************
**  QSH DUMP TAG 
**    1. Dump tag to get the meta data information
******************************************************************************/
typedef enum
{
  IMAGE_QSH_DUMP_TAG_META_DATA = 0,
  IMAGE_QSH_DUMP_TAG_MAX = 31
}image_qsh_dump_tag_e;



typedef struct
{
  /*! revision */
  uint16  rev;

  /*! size of this struct in bytes */
  uint16  hdr_size_bytes;
  
  /*! date/time of dump */
  char    log_time[QSH_DUMP_META_HDR_LOG_TIME_SIZE_BYTES];
  
  /*! chipset name */
  char    chipset[QSH_DUMP_META_HDR_CHIPSET_SIZE_BYTES];
  
  /*! modem image version */
  char    img_version[QSH_DUMP_META_HDR_IMG_VER_SIZE_BYTES];
  
  /*! date/time of image */
  char    img_time[QSH_DUMP_META_HDR_IMG_TIME_SIZE_BYTES];
} image_qsh_dump_meta_hdr_info_s;


/*!
  @brief 
  Header for Image meta data.
*/
typedef struct 
{
  qsh_dump_tag_hdr_s hdr; /* this header will have a tag and length */
  image_qsh_dump_meta_hdr_info_s dump_meta_hdr_info;
}image_qsh_dump_tag_meta_data_s;


#endif /* IMAGE_QSH_EXT_H */
