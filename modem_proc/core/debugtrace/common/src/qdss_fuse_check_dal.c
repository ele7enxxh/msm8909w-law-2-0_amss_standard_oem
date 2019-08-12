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
   setting for invasive debugging overrides settings for non-invasive debugging.
    
 
EXTERNALIZED FUNCTIONS
   qdss_fuse_trace_access
 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   Requires System FPB clock to access the efuse registers.
   

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*========================================================================== 
 $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/common/src/qdss_fuse_check_dal.c#1 $
==========================================================================*/  



#include "qdss_fuse_check.h"
#include "halhwio_qdss.h"
#include "qdss_fuse_regs.h"
#include "DALSys.h"


/*
 * Define the page size for HWIO regions - 4K.
 */
#define HWIO_PAGE_SIZE 0x1000

/*
 * Define macro to get the page base and offset given an address.
 */
#define HWIO_PAGE_BASE(nAddr)   ((nAddr) & ~(HWIO_PAGE_SIZE - 1))

#define HWIO_PAGE_OFFSET(nAddr) ((nAddr)& (HWIO_PAGE_SIZE - 1))

static volatile uint32 SECURITY_CONTROL_BASE_PTR=NULL;


static DALResult qdss_efuse_memmap(uint32 physAddr,
                                  uint32 size,
                                  DALSYSMemHandle *phMem,
                                  DALSYSMemInfo *pMemInfo)
{
   uint32 offset;

   offset=HWIO_PAGE_OFFSET(physAddr);

   if (DAL_SUCCESS!=DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  HWIO_PAGE_BASE(physAddr),
                                  size, phMem, NULL ))
   {
      return DAL_ERROR;
   }

   DALSYS_MemInfo(*phMem, pMemInfo);
   pMemInfo->VirtualAddr    += offset;
   pMemInfo->PhysicalAddr   += offset;
   pMemInfo->dwLen          -= offset;
   
   return DAL_SUCCESS;
}



/*
Input: None

Description:
   Checks the efuse associated with QDSS and returns a boolean on whether trace
   access is possible or not regardless of the specific security mode of 
   debugging(NS=0/1). The function checks the following OEM fuses: 
   DAP_SPNIDEN_DISABLE, DAP_SPIDEN_DISABLE, DAP_NIDEN_DISABLE,DAP_DBGEN_DISABLE,
   DAP_DEVICEEN_DISABLE, ALL_DEBUG_DISABLE

Return: 
   0 - Trace access not possible
   1 - Trace access is possible
*/


unsigned int qdss_fuse_trace_access(void)
{
   unsigned int status=FALSE;

   DALSYSMemHandle hMem;
   DALSYSMemInfo MemInfo;
   if(SECURITY_CONTROL_BASE_PTR==NULL)
   {
      
      if(DAL_SUCCESS!=qdss_efuse_memmap(SECURITY_CONTROL_BASE_PHYS,
                     SECURITY_CONTROL_BASE_SIZE,
                     &hMem,
                     &MemInfo
                     ))
      {
         return FALSE;
      }
      if(MemInfo.dwMappedLen==SECURITY_CONTROL_BASE_SIZE)
      {
         SECURITY_CONTROL_BASE_PTR=MemInfo.VirtualAddr;
      }
      else
      {
         return FALSE;
      }
   }
   
   status =           !(HWIO_INF(OEM_CONFIG1,DAP_SPNIDEN_DISABLE));
   status = status && !(HWIO_INF(OEM_CONFIG1,DAP_SPIDEN_DISABLE));
   status = status && !(HWIO_INF(OEM_CONFIG1,DAP_NIDEN_DISABLE));
   status = status && !(HWIO_INF(OEM_CONFIG1,DAP_DBGEN_DISABLE));
   status = status && !(HWIO_INF(OEM_CONFIG1,DAP_DEVICEEN_DISABLE));
   status = status && !(HWIO_INF(OEM_CONFIG1,ALL_DEBUG_DISABLE));
  
   return status;
}
