/**********************************************************************
 * sdcc_hc_stub.c
 *
 * Empty functions for targets that do not support SDHCi.
 *
 * Copyright (c) 2013
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header:
$DateTime:

when        who     what, where, why
----------  ---     --------------------------------------------------- 
2013-09-03  bn      Initial release

======================================================================*/

#include "sdcc_hc.h"

/* SDHCi configuration flags. SDHCi is disabled for targets that */
/* do not support SDHCi */
const boolean SDCCSdhciEnable = FALSE;
const boolean DMASupportEnable = FALSE;

/******************************************************************************
* Name: sdcc_init_sdhci_mode
*
* Description:
*    Stub function for targets that do not support SDHCi 
*
******************************************************************************/
boolean
sdcc_init_sdhci_mode (uint32 driveno)
{
   return FALSE;
}

/******************************************************************************
* Name: sdcc_hc_set_clock
*
* Description:
*    Stub function for targets that do not support SDHCi 
*
*****************************************************************************/
SDCC_STATUS sdcc_hc_set_clock (uint32 driveno, uint32 freq_hz)
{
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_do_transfer_sdhci_mode
*
* Description:
*    Stub function for targets that do not support SDHCi
*
*****************************************************************************/
SDCC_STATUS sdcc_do_transfer_sdhci_mode (sdcc_dev_type *p_device, 
                                         sdcc_cmd_type *p_command,
                                         byte *buffer,
                                         uint16 transfer_size)
{
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_hc_config_buswidth
*
* Description:
*    Stub function for targets that do not support SDHCi 
* 
******************************************************************************/
SDCC_STATUS sdcc_hc_config_buswidth (uint32 driveno, SDCC_MMC_BUSWIDTH buswidth)
{
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_command_sdhci_mode
*
* Description:
*    Stub function for targets that do not support SDHCi 
*
******************************************************************************/
SDCC_STATUS sdcc_command_sdhci_mode (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *cmd)
{ 
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_hc_enable_ddr50
*
* Description:
*    Stub function for targets that do not support SDHCi 
*
******************************************************************************/
SDCC_STATUS sdcc_hc_enable_ddr50 (uint32 driveno)
{
   return SDCC_NO_ERROR;
}
