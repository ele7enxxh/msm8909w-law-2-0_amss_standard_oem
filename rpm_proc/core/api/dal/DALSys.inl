#ifndef DALSYS_INL
#define DALSYS_INL
/*==============================================================================
@file DALSys.inl

                  DAL System lib API

        Copyright (c) 2008,2010 Qualcomm Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/core/pkg/rpm/bf/dev/karlm.CR_472278_v2/core/api/dal/DALSys.inl#1 $
=============================================================================*/

#include "string.h"
#include "comdef.h"
#include <stdlib.h>
#include "DALDeviceId.h"

//
// OBJECT MANAGEMENT
//
// We don't actually ever create something that allocates memory to be cleaned
// up at the moment, so we don't need to fill in this function.
//
inline DALResult
DALSYS_DestroyObject(DALSYSObjHandle hObj)
{
  return DAL_SUCCESS;
}

//
// LOCKS
//
// The RPM is using a single-threaded model, with interrupt priority masking
// ("locking") as the only exclusivity.
//
inline DALResult
DALSYS_SyncCreate(uint32 dwAttribs,
                  DALSYSSyncHandle *phSync,
                  DALSYSSyncObj *pSyncObj)
{
    (void) dwAttribs;
    (void) pSyncObj;

    *phSync = (DALSYSSyncHandle)1;
    return DAL_SUCCESS;
}

inline void
DALSYS_SyncEnter(DALSYSSyncHandle hSync)
{
    INTLOCK();
}

inline void
DALSYS_SyncLeave(DALSYSSyncHandle hSync)
{
    INTFREE();
}

//
// HEAP FUNCTIONS
//
// The RPM Cortex-M3 image is using the ARM standard heaps.
//
inline DALResult
DALSYS_Malloc(uint32 dwSize, void **ppMem)
{
    if(!ppMem)
        return DAL_ERROR;

    *ppMem = malloc(dwSize);

    return DAL_SUCCESS;
}

inline DALResult
DALSYS_Free(void *pmem)
{
    return DAL_SUCCESS;
}

//
// EVENTS
//
// Nobody on the RPM is using DAL events for real (note the lack of
// EventCreate) but some codepaths try to call EventCtrlEx.  Even though
// they'll never be hit at runtime, the linker sees that it's theoretically
// possible (through referenced function pointers) and therefore requires this
// to be present.
//
inline DALResult DALSYS_EventCtrlEx(DALSYSEventHandle hEvent, uint32 dwCtrl, uint32 dwParam, void *pPayload, uint32 dwPayloadSize)
{
  assert(false);
  return DAL_ERROR;
}

//      
// MISC UTILITIES
//
// Logging should largely follow a different path; we simply stub the function
// here so clock can build.
//
// I'm not sure what was wrong with busywait() but apparently we need to
// provide a wrapper of a different name... might look into just #defining this
// somewhere, since the compiler won't be able to inline this call across the
// compilation units (and link time code gen make code huge).
//      
inline void DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, const char * pszFmt, ...) {}

inline DALResult DAL_DeviceDetach(DalDeviceHandle *hDevice)
{
  return DAL_SUCCESS;
}

inline DALResult DAL_DeviceAttachEx(const char *pszArg, DALDEVICEID DeviceId,
                             DALInterfaceVersion ClientVersion,DalDeviceHandle **phDevice)
{
  if(DALDEVICEID_HWIO == DeviceId)
    *phDevice = (DalDeviceHandle *)1; // spmi requires this one to succeed with a non-null handle
  else if(DALDEVICEID_CHIPINFO == DeviceId)
  {
    *phDevice = (DalDeviceHandle *)0; // clock regime requires this one to fail with a null handle and failure...
    return DAL_ERROR;
  }
  else
    *phDevice = (DalDeviceHandle *)1; // ...but MPROC likes theirs to pass with non-null handles

  return DAL_SUCCESS;     
}

inline DALResult DAL_DeviceAttach(DALDEVICEID DeviceId, DalDeviceHandle **phDevice)      
{
  return DAL_DeviceAttachEx(NULL, DeviceId, 0, phDevice);
}

#endif //DALSYS_INL

