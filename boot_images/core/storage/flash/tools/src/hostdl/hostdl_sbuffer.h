#ifndef HOSTDL_SBUFFER_H_
#define HOSTDL_SBUFFER_H_

/*==================================================================
 *
 * FILE:        hostdl_sbuffer.h
 *
 * SERVICES:    None
 *
 * DESCRIPTION:
 *   This header file contains externalized definitions from the stream
 *   buffering layer.
 *
 *        Copyright © 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_sbuffer.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "hostdl_packet.h"
#include "comdef.h"

/* defines for buffering of re-transmitted packets */
#define MAX_GAPS  (NUMBER_OF_PACKETS + (NUMBER_OF_PACKETS*2))


extern void sbuffer_init (void);

extern int stream_buffer (uint8 * data, uint32 addr, uint32 len);

#endif /* HOSTDL_SBUFFER_H_ */
