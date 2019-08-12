#ifndef RFCOMMON_AUTOPIN_DB_H
#define RFCOMMON_AUTOPIN_DB_H
/*!
  @file
  rfcommon_autopin_db.h
  
  @brief
 
*/

/*===========================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
------------------------------------------------------------------------------- 
07/20/15   cdb     Init version
============================================================================*/
#define AUTOPIN_DB_REVISION  (0xAA)
#define AUTOPIN_DB_LTE_IDX   (0)
#define AUTOPIN_DB_WCDMA_IDX (1)
#define AUTOPIN_DB_TDS_IDX   (2)
#define AUTOPIN_DB_C2K_IDX   (3)
#define AUTOPIN_DB_MAX_TECH (4)
#define AUTOPIN_DB_MAX_PATH_SIZE (32)

typedef PACK(struct)
{
  uint8  autopin_enable;
  uint16 loop_speed_acq;
  uint16 loop_speed_trk;
  uint8  pin_step;
  uint16 trk_thresh; 
} autopin_efs_record_t;

typedef struct {
  boolean valid;                                     /* if valid efs file was found and read */
  uint8   revision;                                  /* revision # read from file */
  autopin_efs_record_t record[AUTOPIN_DB_MAX_TECH];  /* data content of efs file */
} autopin_efs_data_t;

/* size of autopin_efs_record_t 
   byte for revision # 1
   bytes for autopin_enable 4
   bytes for loop_speed_acq: 8
   bytes for loop_speed_trk: 8
   bytes for pin_step: 4
   bytes for trk_thresh: 8
*/
#define AUTOPIN_DB_DAT_SIZE 33

autopin_efs_record_t* rfcommon_autopin_efs_get_record(rflm_tech_id_t tech);

#endif /* RFCOMMON_AUTOPIN_DB_H */

