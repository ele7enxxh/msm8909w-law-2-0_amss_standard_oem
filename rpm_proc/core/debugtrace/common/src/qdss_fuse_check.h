/*=========================================================================
                       QDSS e-fuse check

GENERAL DESCRIPTION 
   QDSS fuse check is now done using STM AUTHSTATUS register.
   This file has the original interface, which is currently used by clock
   drivers.
 
EXTERNALIZED FUNCTIONS
   qdss_fuse_trace_access
 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   Requires System FPB clock to access the efuse registers.
   

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*========================================================================== 
 $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/common/src/qdss_fuse_check.h#2 $
==========================================================================*/  

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

