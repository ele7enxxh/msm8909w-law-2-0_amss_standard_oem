
/*!
  @file
  batt_efs.c

  @brief
  This file implements the NV functions for LMTSMGR BATT module.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/batt/src/batt_efs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/13   rj      mcs_limitsmgr.conf EFS file optimization
09/19/12   ag      DSI 0 should be considered default with bias as 0
05/06/12   ag     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "fs_sys_types.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "stringl.h"
#include "cxm.h"
#include "lmtsmgr_efs.h"
#include "lmtsmgr_task.h"
#include "modem_mem.h"
#include "batt_efs.h"


/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
/* SAR LUT pointers for GSM(high priority) and LTE(low priority) */
batt_lut_type batt_gsm_lte_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_LTE_BANDS];
batt_tech_comb_lut_type batt_gsm_lte_info = {
  {
    LIMITSMGR_BCL_GSM2_HIGH_LTE_LOW,
  },
  (void *)&batt_gsm_lte_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_LTE_BANDS,
};

/* SAR LUT pointers for GSM(high priority) and TDSCDMA(low priority) */
batt_lut_type batt_gsm_tdscdma_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_TDSCDMA_BANDS];
batt_tech_comb_lut_type batt_gsm_tdscdma_info = {
  {
    LIMITSMGR_BCL_GSM2_HIGH_TDSCDMA_LOW,
  },
  (void *)&batt_gsm_tdscdma_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_TDSCDMA_BANDS,
};

batt_tech_comb_lut_type* batt_tables[CXM_TECH_MAX][CXM_TECH_MAX] = {
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {NULL, NULL, NULL, &batt_gsm_lte_info, &batt_gsm_tdscdma_info, NULL, NULL},
};

/* Default LUT state */                              
batt_lut_type batt_default_luts = {
  DEFAULT_MAX_PLIMIT,     /* Battery limit */
  FALSE                   /* Initially invalid */
};

/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  batt_efs_store_record

=============================================================================*/
/*!
    @brief
    Stores a BATTERY CURRENT LIMITING record in the global LUT list.
 
    @return
    None
*/
/*===========================================================================*/
void batt_efs_store_record
(
  batt_efs_record_type*  battery_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  batt_tech_comb_lut_type* batt_techs_lut;
  uint32 hband, lband, index;
  batt_lut_type* batt_lut; 

  hband = battery_rec->hprio_band;
  lband = battery_rec->lprio_band;

  /* Get the simult tech lut pointer */
  batt_techs_lut = batt_tables[hprio_tech][lprio_tech];
  if (batt_techs_lut == NULL || batt_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    return;
  }

  batt_lut = (batt_lut_type*)(batt_techs_lut->luts);
  index = hband * batt_techs_lut->num_tech2_bands + lband;

  if ((batt_lut+index)->valid == FALSE)
  {
    /* Store the battery limit and set flag to TRUE */
    (batt_lut+index)->battery_limit = 
                                           battery_rec->hprio_limit;
    (batt_lut+index)->valid = TRUE;
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    return;
  }
  
}


/*=============================================================================

  FUNCTION:  batt_efs_validate_record

=============================================================================*/
/*!
    @brief
    Validates a BATTERY CURRENT LIMITING record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean batt_efs_validate_record 
(
  batt_efs_record_type*  battery_rec,
  cxm_tech_type  hprio_tech,
  cxm_tech_type  lprio_tech
)
{
  if (battery_rec == NULL)
  {
    return FALSE;
  }

  if ( FALSE == lmtsmgr_efs_validate_tech_band( hprio_tech, 
                                            battery_rec->hprio_band) )
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band( lprio_tech, 
                                           battery_rec->lprio_band) )
  {
    return FALSE;
  }

  return TRUE;
}

/*=============================================================================

  FUNCTION:  batt_read_efs

=============================================================================*/
/*!
    @brief
    Reads all records from the Battery current limiting EFS file  
 
    @return
    None
*/
/*===========================================================================*/
void batt_read_efs 
( 
  const char *fname,
  cxm_tech_type  hprio_tech,
  cxm_tech_type  lprio_tech
)
{
  int32      fd;
  batt_efs_record_type  battery_rec;
  int result;

  boolean rec_valid = FALSE;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(fname, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read ( fd, (void *)&battery_rec, 
                        sizeof(batt_efs_record_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading BATT record",efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(batt_efs_record_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(batt_efs_record_type));
      break; 
    }
    
    /* Read was succesful. Now validate the data */
    rec_valid = batt_efs_validate_record(&battery_rec, hprio_tech, lprio_tech);
    
    if (rec_valid == TRUE)
    {
      /* Store the record in heap and assign pointer in global lut list */
      batt_efs_store_record(&battery_rec, hprio_tech, lprio_tech);  
    }
  }

  (void)efs_close(fd);
}

/*=============================================================================

  FUNCTION:  batt_efs_set_all_luts_to_default

=============================================================================*/
/*!
    @brief
    Sets the global luts to default 
 
    @return
    None
*/
/*===========================================================================*/
static void batt_efs_set_all_luts_to_default
(
  batt_tech_comb_lut_type* lut_list
)
{
  int i, j, index;
  batt_lut_type* batt_luts;
  uint32 hprio_bands;
  uint32 lprio_bands;

  if (lut_list == NULL || lut_list->luts == NULL)
  {
    return;
  }

  hprio_bands = lut_list->num_tech1_bands;
  lprio_bands = lut_list->num_tech2_bands; 

  for(i = 0; i < hprio_bands; i++)
  {
    for(j = 0; j < lprio_bands; j++)
    {
      index = (i * lprio_bands + j) ;

      /* Get the pointer to the LUT */
      batt_luts = (batt_lut_type*)lut_list->luts + index;
      memscpy((void *)batt_luts, 
              sizeof(batt_lut_type),
              (void *)&batt_default_luts, 
              sizeof(batt_lut_type)); 
    }
  }
}

/*=============================================================================

  FUNCTION:  batt_efs_set_default_lut

=============================================================================*/
/*!
    @brief
    Sets the global luts to default 
 
    @return
    None
*/
/*===========================================================================*/
static void batt_efs_set_default_lut (void)
{
  cxm_tech_type i, j;
  batt_tech_comb_lut_type* luts;

  for (i=0; i<CXM_TECH_MAX; i++)
  {
    if (batt_tables[i] != NULL)
    {
      for(j=0; j<CXM_TECH_MAX; j++)
      {
        /* Is this a valid scenario ? */
        if (batt_tables[i][j] != NULL)
        {
          luts = batt_tables[i][j];
          
          /* Set all the LUTs to default for this simult tech  */
          batt_efs_set_all_luts_to_default(luts);
        }
      }
    }
  }
}
/*=============================================================================

  FUNCTION:  batt_efs_init

=============================================================================*/
/*!
    @brief
    Initializes BATT EFS
 
    @return
    None
*/
/*===========================================================================*/
void batt_efs_init (void)
{
  cxm_tech_type tech1, tech2;
  batt_tech_comb_lut_type* tbl_entry;

  /* Set the LUTs to default */
  batt_efs_set_default_lut();

  /* Read the files one by one... */
  for (tech1=0; tech1<CXM_TECH_MAX; tech1++)
  {
    for(tech2=0; tech2<CXM_TECH_MAX; tech2++)
    {
      tbl_entry = batt_tables[tech1][tech2];

      /* If there is no entry, then continue to the next */
      if (tbl_entry == NULL)
      {
        continue;
      }

      /* Read the battery current limiting EFS file */
      if (tbl_entry->files.batt_limit_fname != NULL)
      {
        batt_read_efs(tbl_entry->files.batt_limit_fname, tech1, tech2);
      }
    }
  }
}

/*=============================================================================

  FUNCTION:  batt_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the current Battery current Tx limits.
 
    @return
    None
*/
/*===========================================================================*/
boolean batt_efs_lookup
(
  batt_efs_lookup_in* input,
  batt_efs_lookup_out* output
)
{
  batt_tech_comb_lut_type* tables;
  batt_lut_type* luts;
  batt_lut_type* batt_lut;
  uint32 index = 0;
  
  /* Check input/output ptr*/
  if (input == NULL || output == NULL)
  {
    return FALSE;
  }

  if ((input->hptech >= CXM_TECH_MAX) || (input->lptech >= CXM_TECH_MAX))
  {
    return FALSE;
  }
   
  /* Check if tables exist */
  if ( batt_tables[input->hptech] == NULL  || 
       batt_tables[input->hptech][input->lptech] == NULL )
  {
    return FALSE;
  }

  tables = batt_tables[input->hptech][input->lptech];

  /* check bands */
  if ( (input->hpband >= tables->num_tech1_bands) || 
       (input->lpband >= tables->num_tech2_bands) )
  {
    LMTSMGR_MSG_2(ERROR,"Incorrect band %d %d", input->hpband, input->lpband);
    return FALSE;
  }
 
  /* Assume initially limit wont be available */
  output->hplimit_valid = FALSE;
  
  /* Get the BATTERY LIMIT LUT */
  index = input->hpband * tables->num_tech2_bands + input->lpband;
  luts = (batt_lut_type *)(tables->luts);
  batt_lut = luts + index;
  
  if (batt_lut == NULL)
  {
    /* There is no lookup table for this band combination */
    return FALSE;
  }

  /* Get the hplimit if any... */
  if (batt_lut->valid)
  {
    output->hplimit = batt_lut->battery_limit;
    output->hplimit_valid = TRUE;
  }
  
  return TRUE; 
}