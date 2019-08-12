/*! \file pm_version.c 
*  \n
*  \brief This file contains PMIC version auto-detection implementation
*  \n
*  \n &copy; Copyright 2010-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/framework/src/pm_version.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/15   mr      Removed MPSS PMIC SW Compilation Warnings (CR-799430)
05/15/13   kt      New PMIC Version driver format.
02/14/13   kt      Added pm_get_pbs_info API.
01/26/13   kt      Added pm_get_pmic_info API.
01/24/13   kt      Adding device index param to pmic model and revision APIs.
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "DALGlbCtxt.h"
#include "DALSys.h"

#include "pm_version.h"
#include "pm_utils.h"
#include "pm_comm.h"
#include "pm_malloc.h"
#include "device_info.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* QC PMIC Peripheral type value */
#define PM_TYPE_QC_DEVICE        0x51

/*
 * Format of the pm_pbs_info_smem_type structure. Increments once 
 * for every structure change. 
 */
#define PM_PBS_INFO_SMEM_FORMAT   4

/** 
  @struct pm_pbs_info_smem_type
  @brief Structure for the shared memory location which is used
         to store PMIC PBS related information such as PBS Lot
         ID, ROM Version, RAM Version, Fab Id, Wafer Id, X coord
         and Y coord. PBS ROM/RAM Revision id and Variant (or
         Branch) id are stored in last 16 bits(upper and lower 8
         bits) of rom_version and ram_version.
 */
typedef PACKED struct
{
  uint32         format;                           /* Starts from 1 and increments if we add more data */
  uint8          lot_id[PM_PBS_INFO_NUM_LOT_IDS];  /* PBS Lot Id */
  uint32         rom_version;                      /* PBS ROM Version number */
  uint32         ram_version;                      /* PBS RAM Version number */
  uint32         fab_id;                           /* PBS Fab Id */
  uint32         wafer_id;                         /* PBS Wafer Id */
  uint32         x_coord;                          /* PBS X Coord */
  uint32         y_coord;                          /* PBS Y Coord */
  uint32         test_pgm_rev;                     /* PBS Test PGM Rev */
  pm_pbs_info_mfgid_type  mfg_id;                  /* PBS MGF ID */
} pm_pbs_info_smem_type;

/** 
  @struct pm_pbs_info_glb_ctxt_type
  @brief Global context data structure for sharing the pbs info 
         across processors.
 */
typedef struct
{
  DALGLB_HEADER             dal_glb_header;
  pm_pbs_info_smem_type     pbs_info_glb_arr[PM_MAX_NUM_PMICS];
} pm_pbs_info_glb_ctxt_type;

/* Static global variable to store the pmic device info */
static pm_device_info_type pm_device_info_arr[PM_MAX_NUM_PMICS];

/* Static global variable to store the pmic device primary slave id value */
static uint32 pm_primary_slave_id[PM_MAX_NUM_PMICS];

/* Flag to check if PMIC Version driver is initialized */
static boolean pm_version_initialized = FALSE;

/*
 * Num of Slaves per pmic should be fixed for a target.
 * Current plan is to support fixed number of slaves per pmic,
 * with the existing pmic config we cannot support a
 * target having both 1 and 2 slaves per pmic on the same target
 */
static const uint8 pm_num_slaves_per_pmic = 2;

extern boolean bPMICStub;
extern uint32  pm_model_info[][3];
/*=========================================================================

                FUNCTION DEFINITIONS

==========================================================================*/
/**
 * @name pm_version_detect
 *
 * @brief Please refer to device_info.h file for further info on
 *        this function.
 */
pm_err_flag_type pm_version_detect(void)
{
  pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
  uint32            slave_id = 0;
  uint8             pmic_index = 0;
  uint16            base_addr = 0x0100;
  uint16            metal_rev_addr = base_addr + 0x02;
  const             uint8 num_of_bytes = 4;
  uint8             rev_id[num_of_bytes] = {0};

  uint32 (*pm_model_arr)[3] = pm_model_info;

  /* This function needs to be called only once to read the PMIC
   * info and store it */
  if (pm_version_initialized)
  {
    return err_flag;
  }

  DALSYS_memset(pm_device_info_arr, 0, sizeof(pm_device_info_arr));
  DALSYS_memset(pm_primary_slave_id, 0, sizeof(pm_primary_slave_id));

  for(slave_id = 0; slave_id < PM_MAX_NUM_SLAVE_IDS;
        slave_id = slave_id + pm_num_slaves_per_pmic, pmic_index++)
  {
    if(pmic_index >= PM_MAX_NUM_PMICS)
    {
      break;
    }

    /* PMIC Init is not required on RUMI and initial VIRTIO Validation */
    if (TRUE == bPMICStub)
    {
      if(0 == pm_model_arr[pmic_index][0])
      {
        pm_version_initialized = TRUE;
        return err_flag;
      }

      pm_device_info_arr[pmic_index].ePmicModel = pm_model_arr[pmic_index][0];
      pm_device_info_arr[pmic_index].nPmicAllLayerRevision = pm_model_arr[pmic_index][1];
      pm_device_info_arr[pmic_index].nPmicMetalRevision = pm_model_arr[pmic_index][2];
      pm_primary_slave_id[pmic_index] = slave_id;

      continue;
    }

    /* Read PMIC Device revision information */
    err_flag = pm_comm_read_byte_array(slave_id, metal_rev_addr, num_of_bytes, rev_id, 0);

    /* We must find Valid QC PMIC at Slave ID 0 */
    if (0 == slave_id && (PM_ERR_FLAG__SUCCESS != err_flag || PM_TYPE_QC_DEVICE != rev_id[2]) )
    {
      pm_version_initialized = TRUE;
	  /* If SPMI Read succeed, but found Non-QC device */
      if(PM_ERR_FLAG__SUCCESS == err_flag)
      {
        err_flag = PM_ERR_FLAG__INVALID_PMIC_MODEL;
      }
      return err_flag;
    }
    else if (PM_ERR_FLAG__SUCCESS != err_flag || PM_TYPE_QC_DEVICE != rev_id[2])
    {
      /* Once it's confirmed that the QC PMIC device corresponding to the
         slave id doesn't exist, then disregard the error flag returned
         from SPMI read operation and continue to check for other slave ids */
      err_flag = PM_ERR_FLAG__SUCCESS;
      continue;
    }

    /* Store PMIC chip revison information in static global array */
    pm_device_info_arr[pmic_index].ePmicModel = (pm_model_type) rev_id[3];
    pm_device_info_arr[pmic_index].nPmicAllLayerRevision = (uint32) rev_id[1];
    pm_device_info_arr[pmic_index].nPmicMetalRevision = (uint32) rev_id[0];

    /* Store PMIC Device Primary slave id value */
    pm_primary_slave_id[pmic_index] = slave_id;

    /* TODO - Need to confirm if we really need this piece of code for AllLayerRev correction */
    if (pm_device_info_arr[pmic_index].nPmicAllLayerRevision < 1)
    {
      pm_device_info_arr[pmic_index].nPmicAllLayerRevision = pm_device_info_arr[pmic_index].nPmicAllLayerRevision + 1;
    }
  }

  pm_version_initialized = TRUE;
  return err_flag;
}

/**
 * @name pm_get_pmic_model
 *
 * @brief This is a public API. Please refer to pm_version.h
 *        file for further info on this function.
 */
pm_model_type pm_get_pmic_model(uint8 pmic_index)
{
  pm_model_type model = PMIC_IS_INVALID;

  if((!pm_version_initialized) ||  (pmic_index >= PM_MAX_NUM_PMICS))
  {
    return model;
  }

  model = pm_device_info_arr[pmic_index].ePmicModel;

  return model;
}

/**
 * @name pm_get_pmic_revision
 *
 * @brief This is a public API. Please refer to pm_version.h
 *        file for further info on this function.
 */
uint8 pm_get_pmic_revision(uint8 pmic_index)
{
  uint8 all_layer_rev = 0;

  if((!pm_version_initialized) || (pmic_index >= PM_MAX_NUM_PMICS))
  {
    return all_layer_rev;
  }

  all_layer_rev = pm_device_info_arr[pmic_index].nPmicAllLayerRevision;

  return all_layer_rev;
}

/**
 * @name pm_get_pmic_info
 *
 * @brief This is a public API. Please refer to pm_version.h
 *        file for further info on this function.
 */
pm_err_flag_type pm_get_pmic_info(uint8 pmic_index, pm_device_info_type* pmic_device_info_ptr)
{
  if((NULL == pmic_device_info_ptr) || (!pm_version_initialized) || (pmic_index >= PM_MAX_NUM_PMICS))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  pmic_device_info_ptr->ePmicModel = pm_device_info_arr[pmic_index].ePmicModel;
  pmic_device_info_ptr->nPmicAllLayerRevision = pm_device_info_arr[pmic_index].nPmicAllLayerRevision;
  pmic_device_info_ptr->nPmicMetalRevision = pm_device_info_arr[pmic_index].nPmicMetalRevision;

  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_get_pmic_option_pin_info(uint8 pmic_index, uint8 *pmic_opt_pin_data)
{
    pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type reg_data = 0;

    if((!pm_version_initialized) ||  (pmic_index >= PM_MAX_NUM_PMICS))
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    /* Read PMIC Option pin information */
    err_flag = pm_comm_read_byte(pm_primary_slave_id[pmic_index], 0x108, &reg_data, 0 );

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      *pmic_opt_pin_data = reg_data;
    }

    return err_flag;
}

/** 
 * @name pm_get_pbs_info 
 *  
 * @brief This is a public API. Please refer to pm_version.h 
 *        file for further info on this function.
 */
pm_err_flag_type pm_get_pbs_info(uint8 pmic_index, pm_pbs_info_type* pbs_info_ptr)
{
  static pm_pbs_info_glb_ctxt_type *pbs_glb_ctxt_ptr = NULL;

  if((NULL == pbs_info_ptr) || (!pm_version_initialized) || (pmic_index >= PM_MAX_NUM_PMICS)) 
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Ensure global context has been initialized */
  if(DAL_SUCCESS != DALGLBCTXT_Init())
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if (NULL == pbs_glb_ctxt_ptr)
  {
    /* Look-up the global context */
    if(DAL_SUCCESS != DALGLBCTXT_FindCtxt("PM_PBS_INFO", (void **)&pbs_glb_ctxt_ptr))
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
  }

  /* Format 0 indicates pbs info read failed in sbl */
  if ((pbs_glb_ctxt_ptr->pbs_info_glb_arr == NULL) || (pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].format == 0))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Major format increments breaks the backward compatibility */
  if (((uint32)((pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].format)>>16)) != 0)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Get the PBS info from global context and return it */
  DALSYS_memcpy(pbs_info_ptr->nLotId,pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].lot_id,sizeof(pbs_info_ptr->nLotId));
  pbs_info_ptr->nPBSROMVersion = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].rom_version;
  pbs_info_ptr->nPBSRAMVersion = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].ram_version;
  pbs_info_ptr->nFabId = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].fab_id;
  pbs_info_ptr->nWaferId = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].wafer_id;
  pbs_info_ptr->nXcoord = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].x_coord;
  pbs_info_ptr->nYcoord = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].y_coord;
  pbs_info_ptr->nTestPgmRev = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].test_pgm_rev;
  pbs_info_ptr->eMfgId.major = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].mfg_id.major;
  pbs_info_ptr->eMfgId.minor = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].mfg_id.minor;
  pbs_info_ptr->eMfgId.shrink = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].mfg_id.shrink;

  return PM_ERR_FLAG__SUCCESS;
}

/**
 * @name pm_get_slave_id
 *
 * @brief Please refer to device_info.h file for further info on
 *        this function.
 */
pm_err_flag_type pm_get_slave_id(uint8 pmic_index, uint8 slave_id_index, uint32* slave_id_ptr)
{
  if( (slave_id_ptr == NULL) || (!pm_version_initialized) ||
      (pmic_index >= PM_MAX_NUM_PMICS) || (slave_id_index > 1) )
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if (slave_id_index == 1)
  {
    /* Need to add 1 to the primary slave id to get secondary slave id */
    *slave_id_ptr = pm_primary_slave_id[pmic_index] + 1;
  }
  else
  {
    *slave_id_ptr = pm_primary_slave_id[pmic_index];
  }

  return PM_ERR_FLAG__SUCCESS;
}

uint8 pm_get_num_slaves_per_pmic(void)
{
  return pm_num_slaves_per_pmic;
}

