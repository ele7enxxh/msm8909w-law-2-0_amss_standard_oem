
/*!
  @file
  lmtsmgr_efs.c

  @brief
  This file implements the NV functions for LMTS MGR module.

  @ingroup per_implementation
*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/utils/src/lmtsmgr_efs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/21/13   jm      Adding DDR management support
09/19/13   rj      Adding support for GSM to ASID mapping
05/20/13   rj      mcs_limitsmgr.conf EFS file optimization
04/06/13   ag      Support for DSDA WWAN Coexistence
09/19/12   ag      DSI 0 should be considered default with bias as 0
05/06/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "fs_sys_types.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "cxm.h"
#include "lmtsmgr_efs.h"
#include "lmtsmgr_task.h"

#include "sar_efs.h"
#include "wwan_coex.h"
#include "batt_efs.h"


/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/

/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  lmtsmgr_efs_validate_tech_band

=============================================================================*/
/*!
    @brief
    Validates tech, band information 
    @return
    None
*/
/*===========================================================================*/
boolean lmtsmgr_efs_validate_tech_band 
(
  cxm_tech_type  tech,
  uint32         band
)
{
  boolean valid = TRUE;

  switch (tech)
  {
    case CXM_TECH_ONEX:
      /* This is 1x */
      if (band >= (uint32)RFCOM_NUM_1X_BANDS)
      {
        valid = FALSE;
      }
      break;

    case CXM_TECH_GSM1:
    case CXM_TECH_GSM2:
    case CXM_TECH_GSM3:
      /* This is GSM */
      if (band >= (uint32)RFCOM_NUM_GSM_BANDS)
      {
        valid = FALSE;
      }
      break;

    case CXM_TECH_WCDMA:
      /* This is WCDMA */
      if (band >= (uint32)RFCOM_NUM_WCDMA_BANDS)
      {
        valid = FALSE;
      }
      break;

    case CXM_TECH_LTE:
      /* This is LTE */
      if (band >= (uint32)RFCOM_NUM_LTE_BANDS)
      {
        valid = FALSE;
      }
      break;

    case CXM_TECH_TDSCDMA:
      /* This is TDSCDMA */
      if (band >= (uint32)RFCOM_NUM_TDSCDMA_BANDS)
      {
        valid = FALSE;
      }
      break;

    case CXM_TECH_HDR:
      /* This is HDR */
      if (band >= (uint32)RFCOM_NUM_1X_BANDS)
      {
        valid = FALSE;
      }
      break;

    /* Invalid tech identifier */
    default:
      valid = FALSE;
      break;
  }

  return valid;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_efs_create_folders

=============================================================================*/
/*!
    @brief
    Creates all the folders required for SAR/Coex.
 
    @return
    None
*/
/*===========================================================================*/
static void lmtsmgr_efs_create_folders(void)
{
  int ret;

  /* Create mcs folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {
    LMTSMGR_MSG_1(ERROR, "Error %d creating mcs folder in efs", efs_errno);
    return ;
  }

  /* Create lmtsmgr folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/lmtsmgr", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {
    LMTSMGR_MSG_1(ERROR, "Error %d creating lmtsmgr folder in efs", efs_errno);
    return ;
  }

  /* Create coex folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/lmtsmgr/coex", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {    
    LMTSMGR_MSG_1(ERROR, "Error %d creating coex folder in efs", ret);
    return ;
  }

  /* Create coex folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/lmtsmgr/coex/desense", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {    
    LMTSMGR_MSG_1(ERROR, "Error %d creating Desense folder in efs", ret);
    return ;
  }

#ifdef FEATURE_MODEM_DDR_MGMT
  /* Create coex/ddr folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/lmtsmgr/coex/ddr", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {    
    LMTSMGR_MSG_1(ERROR, "Error %d creating EFS folder in efs", ret);
    return ;
  }
#endif /* DDR */

  /* Create sem folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/lmtsmgr/sem", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {
    LMTSMGR_MSG_1(ERROR, "Error %d creating sem folder in efs", ret);
    return ;
  }

  /* Create sar folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/lmtsmgr/sar", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {
    LMTSMGR_MSG_1(ERROR, "Error %d creating sar folder in efs", ret);
    return ;
  }

  /* Create battery folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/lmtsmgr/battery", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {
    LMTSMGR_MSG_1(ERROR, "Error %d creating battery folder in efs", ret);
    return ;
  }
  
}


/*=============================================================================

  FUNCTION:  lmtsmgr_update_conf_file

=============================================================================*/
/*!
    @brief
    Update limitsmgr_conf file with SAR/Coex efs file names
 
    @return
    None
*/
/*===========================================================================*/
static boolean lmtsmgr_update_conf_file(void)
{
  int32 fd;
  int8  result;
  size_t         data_size;
  struct fs_stat conf_file_info;
  boolean is_updated = FALSE;
 
  /* buffer for the list of file paths to be be written to the conf file */
  char	  *str = LIMITSMGR_UPDATE_CONF_FILE_WITH_EFS_NAMES;

  result = efs_stat(LMTSMGR_CONF_EFS_FILE, &conf_file_info);
  data_size = (size_t)strlen(str);
 
  if ( result != 0 || data_size != conf_file_info.st_size )
  {
    /* Open the sar CONF EFS file. If its not present then create it. */
    fd = efs_open(LMTSMGR_CONF_EFS_FILE, O_CREAT|O_AUTODIR|O_WRONLY|O_TRUNC, ALLPERMS);
  
     LMTSMGR_MSG_1(HIGH, "OPEN MCS conf EFS file to read %d", fd);
    if (fd < 0)
    {
      LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
      return is_updated;
    }

    /* Write the conf file */
    (void)efs_write(fd, str, data_size);

    efs_close(fd);
    is_updated = TRUE;
  }
  return is_updated;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_efs_init

=============================================================================*/
/*!
    @brief
    Initializes LMTSMGR EFS
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_efs_init (void)
{
  if (lmtsmgr_update_conf_file())
  {
    /* Create the necessary folders if they dont exist */
    lmtsmgr_efs_create_folders();
  }

  sar_efs_init();

  wwan_coex_efs_init();

  batt_efs_init();
}
