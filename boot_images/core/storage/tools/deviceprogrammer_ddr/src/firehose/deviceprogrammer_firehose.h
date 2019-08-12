/*==================================================================
 *
 * FILE:        deviceprogrammer_firehose.h
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2008-2013 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_firehose.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */

#ifndef DEVICEPROGRAMMER_FIREHOSE_H
#define DEVICEPROGRAMMER_FIREHOSE_H

void initFirehoseProtocol(void);
boolean initFirehoseStorage(void);
boolean isValidationMode(void);
void initFirehoseTransport(void);
void main_firehose(void) __attribute__((noreturn));
void logMessage(const char* format, ...);

void deviceprogrammer_zi_buffers (void);

#endif

