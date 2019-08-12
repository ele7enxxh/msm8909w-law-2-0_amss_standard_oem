#ifndef HOSTDL_STARTUP_H
#define HOSTDL_STARTUP_H

/*==================================================================
 *
 * FILE:        hostdl_startup.h
 *
 * SERVICES:    None
 *
 * DESCRIPTION:
 *   This header file consists the common definitions used during the boot-up
 *   time.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/16/11     jz      ehostdl image size reduction
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

/* STACK DEFINITIONS - These stacks are defined to be the same size as
 * they are defined in startup.s.  However, where they are used, they are used
 * with divide by 4 and are defined to be dword, not byte.
 * This prevents unaligned access.
 * Used as flashprg stack (FLSH_Stack_Size). Allocate 16K for the
 * stack.  A largish stack is required because
 * the packet buffer will be allocated on the stack.
 */

#define SVC_Stack_Size   0x1000

/*  Define other stacks, these can probably be combined to use the same stack */

#define IRQ_Stack_Size   0x800
#define Abort_Stack_Size 0x100
#define Undef_Stack_Size 0x100

#endif /* HOSTDL_STARTUP_H */
