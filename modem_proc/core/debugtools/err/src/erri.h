#ifndef ERRI_H
#define ERRI_H

/*===========================================================================

                    Error Handling Service Internal Header File

Description

Copyright (c) 2009 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/erri.h#1 $

===========================================================================*/

#include "comdef.h"
#include "err.h"
#include "errlog.h"
#include "err_types.h"

#include "err_hw_qdsp6.h"

#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt.h"

#include "err_qurt.h"

/* Following values should be accomodated only in a byte. 
 * Cannot be greater than 255  */

typedef enum
{
  ERR_TYPE_ERR_FATAL = 0,
  ERR_TYPE_NON_RECOVERABLE_EXCEPTION = 1,
  ERR_TYPE_MAX = 255
} err_type_e;


extern coredump_type coredump;

#ifndef TMS_UTILS_BUF_SZ_ZERO   
#define TMS_UTILS_BUF_SZ_ZERO       0
#endif

#define ERR_GET_PID_TO_UPPER_8_BITS             ( (qurt_getpid()<<8) & 0xFF00 )
#define ERR_RAISE_EXCEPTION_ARG(x)              ( QURT_ECODE_UPPER_ERR_SERVICES | ERR_GET_PID_TO_UPPER_8_BITS | (x) )
#define ERR_GET_ERR_TYPE_FROM_EXCEPTION_ARG(x)  (0xFF & x )
#define ERR_GET_PID_FROM_EXCEPTION_ARG(x)       ( (0xFF00 & x ) >> 8 )

#define ERR_TEST_FOR_ERR_FATAL(x) \
        ( (QURT_ECODE_UPPER_ERR_SERVICES | ERR_TYPE_ERR_FATAL) == (0x00FF00FF & x) )

/* Temporary Definition .. modify it after kernel change */
#ifndef QURT_ECODE_UPPER_ERR_SERVICES
#define QURT_ECODE_UPPER_ERR_SERVICES QURT_ECORE_UPPER_ERR_SERVICES
#endif 

boolean err_log_store ( word , const char* , boolean );
void err_nv_log_init ( void );
void err_pause_usec(uint32 usec);

#endif /* ERRI_H */
