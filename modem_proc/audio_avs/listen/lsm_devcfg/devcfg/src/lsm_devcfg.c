/*========================================================================
   This file contains LSM device configuration functions

  Copyright (c) 2009-2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "DALSys.h"
#include "common.h"
#include "lsm_devcfg.h"
#include "qurt_elite.h"
#include <stringl.h>


ADSPResult lsm_devcfg_read_mmpm_data(lpasshwio_prop_lsm_mmpm_struct_t *pDev)
{
   ADSPResult rc = ADSP_EOK;
   lpasshwio_prop_lsm_mmpm_struct_t *p_lsm_mmpm_prop;
   char *devId = "LSM";

   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar prop_var;

   /* Get device handle */
   if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      /* Error handle */
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM DevCfg: Error establishing LPASS dev handles returned %d", rc);
      goto _bailout_default_cpp;
   }
   else
   {
      /* Read property ptr */
      if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp, "lsm_mmpm_prop_struct_ptr", 0, &prop_var)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM DevCfg: Error getting LSM LPASS Property Struct Ptr returned %d", rc);
         goto _bailout_default_cpp;
      }
      else
      {
         p_lsm_mmpm_prop = (lpasshwio_prop_lsm_mmpm_struct_t *)prop_var.Val.pStruct;

         /* Getting MMPM values*/
         if(p_lsm_mmpm_prop->sw_revision)
         {
            memscpy(pDev,
                  sizeof(lpasshwio_prop_lsm_mmpm_struct_t),
                  p_lsm_mmpm_prop,
                  sizeof(lpasshwio_prop_lsm_mmpm_struct_t));
            return ADSP_EOK;
         }
         else
         {
            // Use default CPP factor if sw_revision is not initialized properly
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LSM DevCfg: sw_revision %lu not initialized!", p_lsm_mmpm_prop->sw_revision);
         }
      }
   }

   _bailout_default_cpp:
      // Setting the first bw idx to max_uint64 and corresponding CPP to LSM_MMPM_DEFAULT_CPP_NUM, so that they will always be picked
      pDev->bw[0]  = UINT32_MAX;
      pDev->cpp[0] = LSM_MMPM_DEFAULT_CPP_NUM;

   return ADSP_EFAILED;
}
