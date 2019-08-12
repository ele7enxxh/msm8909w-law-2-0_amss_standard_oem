/*=======================================================================*//**
  @file         qhsusb_boot_dload.c

  @brief        Implements functionality related to the software download feature.

  @details      
                 
                Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/func/qhsusb_boot_dload.c#1 $

  when        who        what, where, why
  ----------  --------   ------------------------------------------------------
  2011-05-31  dpatnaik   initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_boot_dload.h"
#include "qhsusb_dci_api.h"
 
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

 /*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

 /*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_GET_QPST_COOKIE

DESCRIPTION
Provide the QPST Cookie Length and QPST Cookie Data to boot

DEPENDENCIES
None.

RETURN VALUE
TRUE if buffer pointer is valid, QPST cookie is returned
FALSE if buffer pointer is not valid/unallocated
FALSE if the feature FEATURE_QPST_COOKIE is not defined in builds file

SIDE EFFECTS
None
==============================================================================*/
boolean qhsusb_boot_dload_get_qpst_cookie (dload_qpst_cookie_type *dload_qpst_cookie)
{
	return FALSE;
}

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_MARK_DLOAD_INFO_INVALID

DESCRIPTION
Mark that the DLOAD related data is invalid

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
void qhsusb_boot_dload_mark_dload_info_invalid(qhsusb_dload_info_type* dload_info_ptr)
{
	if (dload_info_ptr != NULL)
	{
		/* We mark that the data is invalid by setting an invalid product_id */
		dload_info_ptr->qhsusb_product_id = QHSUSB_PRODUCT_ID_INVALID;
	}
}

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_IS_DLOAD_INFO_PID_VALID

DESCRIPTION
Checks if the DLOAD related PID is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the PID is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qhsusb_boot_dload_is_dload_info_pid_valid(const qhsusb_dload_info_type* dload_info_ptr)
{
	if (dload_info_ptr != NULL)
	{
	/* We verify that the data is valid according to the product_id validity */
		if (dload_info_ptr->pid_serial_num_cookie.magic_1 == PID_MAGIC_NUMBER)
		{
		  return TRUE;
		}
		else
		{ 
		  return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_IS_DLOAD_INFO_SERIAL_NUM_VALID

DESCRIPTION
Checks if the DLOAD related Serial Number is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the Serial Number is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qhsusb_boot_dload_is_dload_info_serial_num_valid(const qhsusb_dload_info_type* dload_info_ptr)
{
	if (dload_info_ptr != NULL)
	{
		/* We verify that the data is valid according to the product_id validity */
		if (dload_info_ptr->pid_serial_num_cookie.magic_2 == SERIAL_NUMBER_MAGIC_NUMBER)
		{
			return TRUE;
		}
		else
		{ 
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in HLOS, before going into 
download mode, and read in download mode.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data

SIDE EFFECTS

==============================================================================*/
qhsusb_dload_info_type* qhsusb_boot_dload_get_dload_info_ptr(void)
{
#ifndef BUILD_EHOSTDL
	return (qhsusb_dload_info_type*)qhsusb_get_shared_imem_address();
#else
    return NULL;
#endif /* BUILD_EHOSTDL */
}
