#ifndef IXTIMEOFDAY_H
#define IXTIMEOFDAY_H
/*============================================================================
IxTimeOfDay

ABSTRACT
   The IxTimeOfDay interface definitions.

DESCRIPTION
   This service will provide basic time services.


Copyright (c) 2005 - 2008  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

#include "comdef.h"

/*
** Structure to store Julian time.
*/
typedef struct
{
   uint16                 year;           /* Year [1980 .. 2100]                     */
   uint16                 month;          /* Month of year [1..12]                   */
   uint16                 day;            /* Day of month [1..31]                    */
   uint16                 hour;           /* Hour of day [0..23]                     */
   uint16                 minute;         /* Minute of hour [0..59]                  */
   uint16                 second;         /* Second of minute [0..59]                */
   uint16                 day_of_week;    /* Day of the week [0..6] [Monday..Sunday] */
} IxTimeOfDay_JulianTimeT;

/*
** Definitions for "C" files. Since the C++ API cannot be called from a C file, we
** provide C wrappers. See the comments for the corresponding C++ methods for
** information on what each function does.
*/
#ifndef __cplusplus
typedef struct
{
   uint32 dummy;
} IxTimeOfDay;

typedef struct
{
   uint32 dummy;
} IxCallback;

void                      Call_IxTimeOfDay_Init( IxTimeOfDay *obj, 
                                                 uint32 RecvIpcSigId );

uint64                    Call_IxTimeOfDay_GetLocalTime( IxTimeOfDay *obj );

void                      Call_IxTimeOfDay_SetLocalTimeOffset( IxTimeOfDay *obj,
                                                               IxCallback *pcb, 
                                                               int32 offset,
                                                               uint32 task_ptr,
                                                               uint32 task_wait_sig );

int32                     Call_IxTimeOfDay_GetLocalTimeOffset( IxTimeOfDay *obj );

uint64                    Call_IxTimeOfDay_GetUserTime( IxTimeOfDay *obj );

void                      Call_IxTimeOfDay_SetUserTime( IxTimeOfDay *obj, 
                                                        IxCallback *pcb,
                                                        uint32 new_user_time,
                                                        uint32 task_ptr,
                                                        uint32 task_wait_sig  );

uint64                    Call_IxTimeOfDay_GetSysTime( IxTimeOfDay *obj );

void                      Call_IxTimeOfDay_SetSysTime( IxTimeOfDay *obj, 
                                                       IxCallback *pcb,
                                                       uint32 new_sys_time,
                                                       uint32 task_ptr,
                                                       uint32 task_wait_sig  );

IxTimeOfDay_JulianTimeT   Call_IxTimeOfDay_GetJulianTime( IxTimeOfDay *obj );

uint64                    Call_IxTimeOfDay_JulianToSeconds( IxTimeOfDay *obj,
                                                            IxTimeOfDay_JulianTimeT *julianPtr );

IxTimeOfDay_JulianTimeT   Call_IxTimeOfDay_SecondsToJulian( IxTimeOfDay *obj, uint64 secs );

IxTimeOfDay*              Call_IxTimeOfDay_New( void );

void                      Call_IxTimeOfDay_Delete( IxTimeOfDay * obj );

#else //__cplusplus
/*
** Definitions for "C++" files
*/

#include "IxBase.h"
#include "IxCallbackDef.h"

class IxTimeOfDay 
{

public:

/* Constructor
 */
IxTimeOfDay( void );

/*==============================================================
FUNCTION:  IxTimeOfDay::GetLocalTime()

DESCRIPTION:
  Returns the local time in milliseconds since the epoch.
  The epoch is 00:00 UTC, Jan 6, 1980.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  Local time in milliseconds since the epoch.

SIDE EFFECTS:
  None
==============================================================*/
uint64    GetLocalTime( void );

/*==============================================================
FUNCTION:  IxTimeOfDay::SetLocalTimeOffset()

DESCRIPTION:
  Sets the local time offset relative to UTC.

INPUT/OUTPUT PARAMETERS:
  @param IxCallback *pcb: callback
  @param int32 offset:    offset to set in seconds relative to UTC.
  @param uint32 task_ptr: Calling task's TCB pointer.
  @param uint32 task_wait_sig: Task signal to wait for when reading NV.

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==============================================================*/
void SetLocalTimeOffset
( 
  IxCallback *pcb, 
  int32 offset,
  uint32 task_ptr,
  uint32 task_wait_sig
);

/*==============================================================
FUNCTION:  IxTimeOfDay::GetLocalTimeOffset()

DESCRIPTION:
  Returns the local time offset in seconds from UTC.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  Local time offset in seconds from UTC.

SIDE EFFECTS:
  None
==============================================================*/
int32     GetLocalTimeOffset( void );

/*==============================================================
FUNCTION:  IxTimeOfDay::GetUserTime()

DESCRIPTION:
  Returns the user time in milliseconds since the epoch.
  The epoch is 00:00 UTC, Jan 6, 1980.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  User time in milliseconds since the epoch.

SIDE EFFECTS:
  None
==============================================================*/
uint64    GetUserTime( void );

/*==============================================================
FUNCTION:  IxTimeOfDay::SetUserTime()

DESCRIPTION:
  Sets the user time.

INPUT/OUTPUT PARAMETERS:
  @param IxCallback *pcb: callback
  @param int32 offset:    time to set in seconds since the epoch.
  @param uint32 task_ptr: Calling task's TCB pointer.
  @param uint32 task_wait_sig: Task signal to wait for when reading NV.

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==============================================================*/
void SetUserTime
( 
  IxCallback *pcb, 
  uint32 new_user_time,
  uint32 task_ptr,
  uint32 task_wait_sig
);

/*==============================================================
FUNCTION:  IxTimeOfDay::GetSystemTime()

DESCRIPTION:
  Returns the system time in milliseconds since the epoch.
  The epoch is 00:00 UTC, Jan 6, 1980.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  System time in milliseconds since the epoch.

SIDE EFFECTS:
  None
==============================================================*/
uint64    GetSysTime( void );

/*==============================================================
FUNCTION:  IxTimeOfDay::SetSysTime()

DESCRIPTION:
  Sets the system time.

INPUT/OUTPUT PARAMETERS:
  @param IxCallback *pcb: callback
  @param int32 offset:    time to set in seconds since the epoch.
  @param uint32 task_ptr: Calling task's TCB pointer.
  @param uint32 task_wait_sig: Task signal to wait for when reading NV.

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==============================================================*/
void SetSysTime
( 
  IxCallback *pcb, 
  uint32 new_sys_time,
  uint32 task_ptr,
  uint32 task_wait_sig
);

/*==============================================================
FUNCTION:  IxTimeOfDay::GetJulianTime()

DESCRIPTION:
  Returns the system time in Julian format.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  Structure containing Julian time

SIDE EFFECTS:
  None
==============================================================*/
IxTimeOfDay_JulianTimeT   GetJulianTime( void );

/*==============================================================
FUNCTION:  IxTimeOfDay::JulianToSeconds()

DESCRIPTION:
  Converts Julian time to seconds since the epoch.

INPUT/OUTPUT PARAMETERS:
  @param IxTimeOfDay_JulianTimeT *julianPtr: pointer to structure
     containing Julian time.

RETURN VALUE:
  Converted value

SIDE EFFECTS:
  None
==============================================================*/
uint64    JulianToSeconds( IxTimeOfDay_JulianTimeT *julianPtr );

/*==============================================================
FUNCTION:  IxTimeOfDay::SecondsToJulian()

DESCRIPTION:
  Converts time specified as seconds since the epoch to Julian time.

INPUT/OUTPUT PARAMETERS:
  @param uint64 secs: time in seconds since the epoch

RETURN VALUE:
  Structure containing Julian time.

SIDE EFFECTS:
  None
==============================================================*/
IxTimeOfDay_JulianTimeT   SecondsToJulian( uint64 secs );

};
#endif //_cplusplus

#endif //IXTIMEOFDAY_H
