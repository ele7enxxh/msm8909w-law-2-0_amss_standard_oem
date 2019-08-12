#ifndef _QUPLOG_H_
#define _QUPLOG_H_
/*
===========================================================================

FILE:   QupLog.h

DESCRIPTION:
    This file contains the API for the QUP Log services 


===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/qup/inc/QupLog.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
07/10/12 LK     Created

===========================================================================
        Copyright c 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "QupTypes.h"
#include "stdarg.h"

typedef void* QUPLOG_DEV_IMP_HANDLE;



typedef struct QupLogSvcType
{
   QUPLOG_DEV_IMP_HANDLE (*pfnGetReadHandle)(uint32 uDevAddr);
   QUPLOG_DEV_IMP_HANDLE (*pfnGetWriteHandle)(uint32 uDevAddr);
} QupLogSvcType;

#define QUPLOG_LOG_WRITE(address,...) 

#define QUPLOG_LOG_READ(address,...)

#endif //_QUPLOG_H_

