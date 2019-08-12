/*=======================================================================*//**
 * @file        qhsusb_log.c
 * @author:     shreyasr
 * @date        13-Mar-2012
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) Logging implementation.
 *
 * @details     This file contains the debug log APIs which can be used for
 *				getting log information
 * @note        
 *
 *              Copyright 2012 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/common/qhsusb_log.c#1 $ 
// $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
// 
// when         who        what, where, why
// --------   ---        ----------------------------------------------------------
// 06/13/12   shreyasr 		Initial revision
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "qhsusb_log.h"
#include <string.h>

#ifdef QHSUSB_ENABLE_LOGGING
static qhsusb_generic_log_type qhsusb_circular_buffer[QHSUSB_GENERIC_LOG_SIZE];

static uint32 qhsusb_circular_buffer_index; 
static uint32 count;
#endif

void qhsusb_log_init()
{
#ifdef QHSUSB_ENABLE_LOGGING
	/*Allocate the memory for the circular buffer*/
	memset(qhsusb_circular_buffer,0,sizeof(qhsusb_generic_log_type)*QHSUSB_GENERIC_LOG_SIZE);
	qhsusb_circular_buffer_index = 0;
	count = 0;
#endif
return;
}

void qhsusb_log(uint32 id_0, uint32 offset_0,uint32 param_0)
{
#ifdef QHSUSB_ENABLE_LOGGING  
  count =count + 1;
  qhsusb_circular_buffer[qhsusb_circular_buffer_index].tick = count;
  qhsusb_circular_buffer[qhsusb_circular_buffer_index].id_0 = (qhsusb_generic_log_enum)(id_0 + offset_0);
  qhsusb_circular_buffer[qhsusb_circular_buffer_index].param_0 = param_0;
  qhsusb_circular_buffer_index = (qhsusb_circular_buffer_index + 1) % QHSUSB_GENERIC_LOG_SIZE;
#endif /*QHSUSB_ENABLE_LOGGING*/
 return;
}

