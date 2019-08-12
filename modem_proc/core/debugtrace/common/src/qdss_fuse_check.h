/*=========================================================================
                       QDSS e-fuse check

GENERAL DESCRIPTION 
   The interface to check efuses associated with the QDSS for accessing its 
   trace functionality is implemented. The fuses that are checked include 
   DBGEN, SPIDEN, NIDEN and SPNIDEN. The functionality controlled by the fuses 
   are as follows. DBGEN controls non-secure invasive debugging, SPIDEN controls
   secured invasive debugging, NIDEN controls non-secure non-invasive debugging 
   and SPNIDEN controls secure non-invasive debugging.

   According to the ARM Coresight architecture, in all cases, the fuse 
   setting for invasive debugging overrides settings for non-invasive debugging 
   regardless of the actual setting for non-invasive fuse.
    
 
EXTERNALIZED FUNCTIONS
   qdss_fuse_trace_access
 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   Requires System FPB clock to access the efuse registers.
   

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*========================================================================== 
 $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/common/src/qdss_fuse_check.h#1 $
==========================================================================*/  

typedef enum{
   QDSS_TRACE_ACCESS_NONE=0,
   QDSS_TRACE_ACCESS_NONSECURE_MODE=1,
   QDSS_TRACE_ACCESS_SECURE_MODE=2,
   QDSS_TRACE_ACCESS_ALL_MODES=3,
   QDSS_TRACE_ACCESS_8BITS=0x7F
}eqdss_trace_ret;



/*
Input: None

Description:
   Checks the efuse associated with QDSS and returns a boolean on whether trace
   acsess is possible or not regardless of the security mode.

Return: 
   0 - Trace access not possible
   1 - Trace access is possible
*/
unsigned int qdss_fuse_trace_access(void);



/*
Input: None

Description:
   The specific security mode in which trace hardware is accessible is returned 
   by this function. Accesses are guarded by efuse settings. 

Return:
   QDSS_TRACE_ACCESS_ALL_MODES - Trace access is possible under both secure and 
   non-secure mode.

   QDSS_TRACE_ACCESS_NONE - Trace access is not possible in any mode.

   QDSS_TRACE_ACCESS_SECURE_MODE - Trace access is possible in secure mode only.

   QDSS_TRACE_ACCESS_NONSECURE_MODE - Trace access is possible in non-secure 
   mode only.
*/
eqdss_trace_ret qdss_fuse_trace_access_mode(void);
