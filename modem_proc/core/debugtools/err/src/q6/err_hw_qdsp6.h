#ifndef ERR_HW_QDSP6_H
#define ERR_HW_QDSP6_H
 
/*===========================================================================

                    Error Handling Service Internal Header File

Description
 
Copyright (c) 2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/q6/err_hw_qdsp6.h#1 $
 
===========================================================================*/



#ifndef ERR_HW_QDSP6
#error BAD CONFIGURATION: CHECK DEFINITIONS
#endif


// MACRO to stall execution until write has been committed to memory
#define ERR_MEMORY_BARRIER()   __asm__ __volatile__( "barrier\n" )



#endif /* ERR_HW_QDSP6_H */
