#ifndef BOOT_FASTCALL_TZ_H
#define BOOT_FASTCALL_TZ_H

/*=============================================================================

                    Boot tz function header File

GENERAL DESCRIPTION
  This header file contains the public interface for tz functions
          
Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_fastcall_tz.h#1 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/24/14   jz       Stub out boot_fastcall_tz if SMC_SUPPORTED is not enabled in boot_target.h
02/11/14   kedara   Added TZ_OFFLINE_MEM_DUMP_CMD
09/25/13   aus      Added TZ_DEVICE_IS_RETAIL_UNLOCK_CMD for retail unlock check
02/19/13   kedara   Added TZ_SBL_XPU_LOCK to enable TZ xpu locking.
02/11/13   dh       Add TZ_ALLOWS_MEM_DUMP_CMD for ram dump permission check
11/03/11   kedara   Added smc commands to write-multiple, write and read fuses.
08/30/10   kedara   Initial version.

===========================================================================*/
#include "boot_target.h"

/*===========================================================================
                      TZ fastcall DEFINES
===========================================================================*/ 

/*  Tz service ID computation:
Service Id followed by function id ((uint32)(((s & 0x3ff) << 10) | (f & 0x3ff)))
#define TZBSP_KEY_WRITE_FUSE_ID TZBSP_SYSCALL_CREATE_CMD_ID(0x8, 0x06)
*/ 
#define TZ_WRITE_FUSE_CMD 0x2003
#define TZ_WRITE_MULT_FUSE_CMD 0x2004
#define TZ_READ_FUSE_CMD 0x2005
#define TZ_WRITE_ROLLBACK_FUSE_CMD 0x2006
#define TZ_SBL_XPU_LOCK 0x0409

/* 
#define TZBSP_MILESTONE_SET_ID \
        TZBSP_SYSCALL_CREATE_CMD_ID(0x1, 0x03)
*/ 
#define TZ_SBL_END_MILESTONE 0x0403


/* 
 tz ram dump permission check command
#define TZBSP_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID \
        TZBSP_SYSCALL_CREATE_CMD_ID(0x3, 0x0B)
*/ 
#define TZ_ALLOWS_MEM_DUMP_CMD  0x0C0B

/* 
 tz retail unlock check command
#define TZ_DUMP_IS_RETAIL_UNLOCK_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x0D)
*/ 
#define TZ_DEVICE_IS_RETAIL_UNLOCK_CMD  0x0C0D

/* 
 For offline ram dumps
 TZBSP_SYSCALL_CREATE_CMD_ID(0x1, 0x09) 
*/ 
#define TZ_OFFLINE_MEM_DUMP_CMD  0x0409

/*===========================================================================
                      TZ PIL XPU CONFIG DEFINES
===========================================================================*/

#define BOOT_TZBSP_PIL_AUTH_LPASS_QDSP6_PROC 0x1


/*===========================================================================

**  Function : boot_fastcall_tz
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This generic api  makes a smc call to Tz module and passes arguments
*    along with Tz service id and token.
*      
* @param[in] id : uint32  tz service Id
*            token: uint32 : token for TZ fastcall service
*            and optionally more arguments according to service
*  
* @par Dependencies
*    None
* 
* @retval
*    status of operation.
* 
* @par Side Effects
*    None
* 
*/
#ifdef SMC_SUPPORTED
int boot_fastcall_tz(uint32 id, uint32 token, ...);
#else
static inline int boot_fastcall_tz(uint32 id, uint32 token, ...)
{
  return -1;
}
#endif

/*===========================================================================

**  Function : boot_fastcall_tz_2arg
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This generic api  makes a smc call to Tz module and passes 2 arguments
*    along with Tz service id and token.
*      
* @param[in] xxid: tz service Id
*            xx0, xx1: uint32 arguments passed to TZ
*  
* @par Dependencies
*    None
* 
* @retval
*    status of operation.
* 
* @par Side Effects
*    None
* 
*/
#define boot_fastcall_tz_2arg(xxid, xx0, xx1) \
  boot_fastcall_tz((xxid << 12) |  2, 0xdeadbeef, (xx0), (xx1))


/*===========================================================================

**  Function : boot_fastcall_tz_4arg
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This generic api  makes a smc call to Tz module and passes 4 arguments
*    along with Tz service id and token.
*      
* @param[in] xxid: tz service Id
*            xx0, xx1, xx2, xx3, xx4: uint32 arguments passed to TZ
*  
* @par Dependencies
*    None
* 
* @retval
*    status of operation.
* 
* @par Side Effects
*    None
* 
*/
#define boot_fastcall_tz_4arg(xxid, xx0, xx1, xx2, xx3) \
  boot_fastcall_tz((xxid << 12) |  4, 0xdeadbeef, (xx0), (xx1), (xx2), (xx3))


/*===========================================================================

**  Function :  boot_check_sec_mode

** ==========================================================================
*/
/*!
* 
* @brief
*   This function checks the processor is in secure mode or not
* 
* @par Dependencies
*   None
*
* @retval
*   TRUE if processor is in secure mode
*   FALSE if processor is in non secure mode
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_check_sec_mode(void);

#endif /* BOOT_FASTCALL_TZ_H */
