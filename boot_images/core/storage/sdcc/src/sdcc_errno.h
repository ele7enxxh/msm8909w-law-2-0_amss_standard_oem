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
 * Copyright (c) 2008-2011, 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_errno.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2014-10-22   sb      Add new error codes
2011-10-20   rh      Change to Defines
2010-03-02   yg      Added another SDCC_STATUS: SDCC_ERR_SPS_MODE 
2009-10-16   sc      Added power-on write protection feature
2009-09-17   sc      Put back the deprecated enum constant SDCC_ERR_DATA_TIMEOUT
2009-08-06   sc      Updated SDCC_ERR_DATA_TIMEOUT to SDCC_ERR_TIMEOUT
2009-06-18   rh      Adding common interface extraction tags
2009-03-02   sc      Replaced SDCC_ERR_SWITCH_INTF with SDCC_ERR_SWITCH
2008-10-23   vin     Added SDCC_ERR_CARD_REMOVED
2008-05-21   sc      Initial version

=============================================================================*/

/* enumeration values for errors that are used in SDCC */
#define SDCC_NO_ERROR                         0
#define SDCC_ERR_UNKNOWN                      1
#define SDCC_ERR_CMD_TIMEOUT                  2
#define SDCC_ERR_DATA_TIMEOUT                 3
#define SDCC_ERR_TIMEOUT                      3
#define SDCC_ERR_CMD_CRC_FAIL                 4
#define SDCC_ERR_DATA_CRC_FAIL                5
#define SDCC_ERR_CMD_SENT                     6
#define SDCC_ERR_PROG_DONE                    7
#define SDCC_ERR_CARD_READY                   8 
#define SDCC_ERR_INVALID_TX_STATE             9
#define SDCC_ERR_SET_BLKSZ                    10
#define SDCC_ERR_SDIO_R5_RESP                 11
#define SDCC_ERR_DMA                          12
#define SDCC_ERR_READ_FIFO                    13
#define SDCC_ERR_WRITE_FIFO                   14
#define SDCC_ERR_ERASE                        15
#define SDCC_ERR_SDIO                         16
#define SDCC_ERR_SDIO_READ                    17
#define SDCC_ERR_SDIO_WRITE                   18
#define SDCC_ERR_SWITCH                       19
#define SDCC_ERR_INVALID_PARAM                20
#define SDCC_ERR_CARD_UNDETECTED              21
#define SDCC_ERR_FEATURE_UNSUPPORTED          22
#define SDCC_ERR_SECURE_COMMAND_IN_PROGRESS   23 
#define SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED     24
#define SDCC_ERR_ABORT_READ_SEC_CMD           25
#define SDCC_ERR_CARD_INIT                    26
#define SDCC_ERR_CARD_REMOVED                 27
#define SDCC_ERR_PWR_ON_WRITE_PROT            28
#define SDCC_ERR_WP_VIOLATION                 29
#define SDCC_ERR_SPS_MODE                     30 
#define SDCC_ERR_DML_INIT                     31
#define SDCC_ERR_SPS_GET_EVENT                32
#define SDCC_ERR_SPS_WRITING_DESCRIPTOR       33

typedef int SDCC_STATUS;
#endif /* __SDCC_ERRNO_H */

