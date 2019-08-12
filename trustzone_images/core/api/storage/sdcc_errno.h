#ifndef __SDCC_ERRNO_H
#define __SDCC_ERRNO_H
/******************************************************************************
 * File: sdcc_errno.h
 *
 * Services: 
 *    Public API header for SDCC error code
 *
 * Description:
 *    This file contains the SDCC error code.
 *
 * Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/sdcc_errno.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2010-09-07   yg      Adding error code for SPS usage and DML init
2010-03-02   yg      Added another SDCC_STATUS: SDCC_ERR_SPS_MODE 
2009-10-16   sc      Added power-on write protection feature
2009-09-17   sc      Put back the deprecated enum constant SDCC_ERR_DATA_TIMEOUT
2009-08-06   sc      Updated SDCC_ERR_DATA_TIMEOUT to SDCC_ERR_TIMEOUT
2009-06-18   rh      Adding common interface extraction tags
2009-03-02   sc      Replaced SDCC_ERR_SWITCH_INTF with SDCC_ERR_SWITCH
2008-10-23   vin     Added SDCC_ERR_CARD_REMOVED
2008-05-21   sc      Initial version

=============================================================================*/

/*SDCC_EXTRACT_COMMON_BEGIN*/
/* enumeration values for errors that are used in SDCC */
typedef enum
{
   SDCC_NO_ERROR = 0,
   SDCC_ERR_UNKNOWN,
   SDCC_ERR_CMD_TIMEOUT,
   SDCC_ERR_DATA_TIMEOUT, /* SDCC_ERR_DATA_TIMEOUT is deprecated */
   SDCC_ERR_TIMEOUT = SDCC_ERR_DATA_TIMEOUT,
   SDCC_ERR_CMD_CRC_FAIL,              
   SDCC_ERR_DATA_CRC_FAIL,
   SDCC_ERR_CMD_SENT,
   SDCC_ERR_PROG_DONE,
   SDCC_ERR_CARD_READY,
   SDCC_ERR_INVALID_TX_STATE,
   SDCC_ERR_SET_BLKSZ,
   SDCC_ERR_SDIO_R5_RESP,
   SDCC_ERR_DMA,
   SDCC_ERR_READ_FIFO,
   SDCC_ERR_WRITE_FIFO,
   SDCC_ERR_ERASE,
   SDCC_ERR_SDIO,
   SDCC_ERR_SDIO_READ,
   SDCC_ERR_SDIO_WRITE,
   SDCC_ERR_SWITCH,
   SDCC_ERR_INVALID_PARAM,
   SDCC_ERR_CARD_UNDETECTED,
   SDCC_ERR_FEATURE_UNSUPPORTED,
   SDCC_ERR_SECURE_COMMAND_IN_PROGRESS,
   SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED,
   SDCC_ERR_ABORT_READ_SEC_CMD,
   SDCC_ERR_CARD_INIT,
   SDCC_ERR_CARD_REMOVED,
   SDCC_ERR_PWR_ON_WRITE_PROT,
   SDCC_ERR_WP_VIOLATION,
   SDCC_ERR_SPS_MODE_USED,
   SDCC_ERR_DML_INIT,
   SDCC_ERR_SPS_GET_EVENT,
   SDCC_ERR_SPS_WRITING_DESCRIPTOR
} SDCC_STATUS;
/*SDCC_EXTRACT_COMMON_END*/

#endif /* __SDCC_ERRNO_H */

