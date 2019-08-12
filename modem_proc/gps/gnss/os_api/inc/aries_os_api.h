/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Operating System API Interface Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  the Operating System API interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/os_api/inc/aries_os_api.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/06/15   sjk     Added if def gurads for C++ linkage for LB 2.0 code 
06/18/14    va     Added os_IpcMapSignals() and os_IpcUnmapSignals() functions
04/17/14   skm     NV Async Read changes  
02/04/13    kj     Added support for new DOG hearbeats 
06/13/11    gk     save timer group in the timer structure so we can call the 
11/08/14    am     os_SetLocalSignalByHnd was added
04/17/14   skm     NV Async Read changes  
02/04/13    kj     Added support for new DOG hearbeats 
06/13/11    gk     save timer group in the timer structure so we can call the 
                   proper destroy function
04/28/11   adi     extended dynamic memory allocation abstractions.
04/06/11   spn     Changes for Loc-API 2.0
03/17/11    rh     Removed featurization for os_TaskTcbGet()
09/22/10    kj     Added dynamic memory allocation abstractions
12/04/06    ah     Added NV read/write Rex signals to wait in os_IpcReceive().
06/30/06    ah     Changed ThreadId type from os_ThreadIdEnumType back to
                   uint32. Changed p_Data type from void* to uint8 array in
                   os_IpcMsgType. Added q_SrcThreadId into os_IpcMsgType.
                   Created os_TimerExpiryType struct, which is used as the
                   p_Data for Timer Expiry IPC msgs.
04/20/06    jd     Added q_SrcThreadId to os_IpcMsgType structure to
                   support IPC-based DM logging.
04/10/06    ah     Change ThreadId type from uint32 to os_ThreadIdEnumType.
03/28/06    ah     Added q_Param to os_TimerMsgType & os_TimerStart()
03/27/06    ah     Converted tabs to two spaces.
03/23/06    ah     os_TimerCreate() now takes a TimerId as parameter, along
                   with DestThreadId. Moved os_ThreadBlockType to source file.
03/14/06    ah     Initial creation of file for AMSS.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef OS_API_H
#define OS_API_H

/*
 * Include files
*/

#include "gps_variation.h"
#include <stdio.h>
#include <string.h>
#include <stringl/stringl.h>

#include "comdef.h"
#include "aries_os_globals.h"

#if defined(FEATURE_BLAST)
#include "blast.h"
#endif /* defined(FEATURE_BLAST) */

#if defined(FEATURE_QURT)
#include "qurt.h"
#endif /* defined (FEATURE_QURT) */

/* Enable support for using Rex kernel related functionality */
#ifndef __GRIFFON__
#define ARIES_USE_REX_KERNEL
#else
#define UBP_IPC_MESSAGE_PADDING_SIZE 1 // this is an alignment to make IPC messages aligned on 32 bit address (in 16 bit words)
#endif

#ifdef ARIES_USE_REX_KERNEL
#include "rex.h"
#include "timer.h"
#elif defined(__GRIFFON__)
#else
#error "No timer support"
#endif /* ARIES_USE_REX_KERNEL */

/*
 * Constant definitions
*/

#define os_IpcMsgType_id      0x01
#define os_TimerMsgType_id    0x02

#define CSC_RETRY_COUNT_MAX       100

/*
 *  GLOBAL EVENTS - 16 bits
 *
 *  All GLOBAL events are handled inside the OS API abstraction layer
 *  and converted into IPC messages.  No GPS code (outside the OS API
 *  abstraction) should clear or wait on GLOBAL events.  It IS ok,
 *  to set these events as appropriate or to convert them into a 32-bit
 *  event mask for use by non-GPS tasks in QCT modem builds.
 *
 *  NOTE - These constants are 32-bits long.
 *  The upper 16-bits are reserved for Global events.
 *
 *  DO NOT use the lower 16-bits for GLOBAL EVENTS!!!
 */
#define C_OS_FLAG_MESSAGE     0x00010000

#ifdef __GRIFFON__
#define C_OS_FLAG_TIMER       0x00020000
#else
#define C_OS_FLAG_MSGR_MSG    0x00020000 /* For MSGR messages registered by individual GNSS task */
#endif /* __GRIFFON__ */

#define C_OS_FLAG_QMI_EVENT   0x00040000
#define C_OS_FLAG_NV_WRITE    0x00080000

#define C_OS_FLAG_RESERVED4   0x00100000

#ifdef __GRIFFON__
#define C_OS_FLAG_NV_SYNC_OP  0x00200000
#define C_OS_FLAG_SEMAPHORE   0x00400000
#define C_OS_FLAG_TIMER_DELAY 0x00800000
#else
#define C_OS_FLAG_DOG_HB      0x00200000 /* DOG Heartbeat */
#define C_OS_FLAG_QMI_CCI_EVENT   0x00400000
#define C_OS_FLAG_RESERVED7   0x00800000
#endif /* __GRIFFON__ */

#define C_OS_FLAG_TASK_MSG    0x01000000 /* Used for TASK_OFFLINE task-related MSGR messages. Do not reuse. */
#define C_OS_FLAG_RESERVED9   0x02000000 /* Used for TASK_STOP signal. Do not reuse. */
#define C_OS_FLAG_RESERVED10  0x04000000 /* Used for TASK_START signal. Do not reuse. */
#define C_OS_FLAG_NV_READ     0x08000000 /* Used for NV Async Read completion*/
#define C_OS_FLAG_RESERVED12  0x10000000
#define C_OS_FLAG_RESERVED13  0x20000000
#define C_OS_FLAG_RESERVED14  0x40000000
#define C_OS_FLAG_RESERVED15  0x80000000

#ifdef FEATURE_INTLOCK_UNSUPPORTED
  #define GPS_MAX_ATOMIC_RETRIES 100
  #define GNSS_GPS_RTC_MUTEX_TAKE  gnss_gps_rtc_mutex_take_api()
  #define GNSS_GPS_RTC_MUTEX_GIVE  gnss_gps_rtc_mutex_give_api()

  #define GNSS_EXT_BLANK_MUTEX_TAKE  gnss_ext_blank_mutex_take_api()
  #define GNSS_EXT_BLANK_MUTEX_GIVE  gnss_ext_blank_mutex_give_api()

#else
  #define GNSS_GPS_RTC_MUTEX_TAKE  INTLOCK()
  #define GNSS_GPS_RTC_MUTEX_GIVE  INTFREE()

  #define GNSS_EXT_BLANK_MUTEX_TAKE  INTLOCK()
  #define GNSS_EXT_BLANK_MUTEX_GIVE  INTFREE()

#endif

/*Common String Utilty functions*/

#define GNSS_STRLEN     strlen
#define GNSS_STRLCPY    strlcpy
#define GNSS_STRLCAT    strlcat
#define GNSS_STRSTR     strstr
#define GNSS_SNPRINTF   snprintf
#define GNSS_STRTOK     strtok_r
#define GNSS_SCANUL     std_scanul
#define GNSS_STRNCMP    strncmp

#define GNSS_STD_OFFSETOF(type,member)     (((char*)(&((type*)1)->member))-((char*)1))


/* MACRO to define INLINE, some compilers might not support inline, so
   defining this, so its easier to resolve on compilers where it does not
   support
   WinCE does not support inline so defining it to NULL in case we are
   compiling on WinCE ( GRIFFON ) */
#ifndef __GRIFFON__
#define GNSS_INLINE inline
#else
#define GNSS_INLINE
#endif


/*
helper macros
*/

/* See definition of MSG_MED, MSG_SSID_DFLT, and MSG_LEGACY_MED in msg_diag_service.h */
/* the following macros emulate the legacy MSG_LOW, MSG_MED, MSG_HIGH and MSG_ERROR macros. These
are required when string formatting %s is required since MSG_MED macro does NOT support %s */
#define GPSOS_MSG_SPRINTF_1(gpsos_level, gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        MSG_SPRINTF_1(MSG_SSID_DFLT, (gpsos_level), (gpsos_fmt), (gpsos_param1), (gpsos_param2), (gpsos_param3))

#define GPSOS_MSG_SPRINTF_2(gpsos_level, gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        MSG_SPRINTF_2(MSG_SSID_DFLT, (gpsos_level), (gpsos_fmt), (gpsos_param1), (gpsos_param2), (gpsos_param3))

#define GPSOS_MSG_SPRINTF_3(gpsos_level, gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        MSG_SPRINTF_3(MSG_SSID_DFLT, (gpsos_level), (gpsos_fmt), (gpsos_param1), (gpsos_param2), (gpsos_param3))

#define GPSOS_MSG_SPRINTF_4(gpsos_level, gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        MSG_SPRINTF_4(MSG_SSID_DFLT, (gpsos_level), \
        (gpsos_fmt), \
        (gpsos_param1), \
        (gpsos_param2), \
        (gpsos_param3), \
        (gpsos_param4))

#define GPSOS_MSG_SPRINTF_5(gpsos_level, gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        MSG_SPRINTF_5(MSG_SSID_DFLT, (gpsos_level), \
        (gpsos_fmt), \
        (gpsos_param1), \
        (gpsos_param2), \
        (gpsos_param3), \
        (gpsos_param4), \
        (gpsos_param5))

#define GPSOS_MSG_SPRINTF_1_LOW(gpsos_fmt, gpsos_param1) \
        GPSOS_MSG_SPRINTF_1(MSG_LEGACY_LOW, (gpsos_fmt), (gpsos_param1))

#define GPSOS_MSG_SPRINTF_1_MED(gpsos_fmt, gpsos_param1) \
        GPSOS_MSG_SPRINTF_1(MSG_LEGACY_MED, (gpsos_fmt), (gpsos_param1))

#define GPSOS_MSG_SPRINTF_1_HIGH(gpsos_fmt, gpsos_param1) \
        GPSOS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, (gpsos_fmt), (gpsos_param1))

#define GPSOS_MSG_SPRINTF_1_ERROR(gpsos_fmt, gpsos_param1) \
        GPSOS_MSG_SPRINTF_1(MSG_LEGACY_ERROR, (gpsos_fmt), (gpsos_param1))


#define GPSOS_MSG_SPRINTF_2_LOW(gpsos_fmt, gpsos_param1, gpsos_param2) \
        GPSOS_MSG_SPRINTF_2(MSG_LEGACY_LOW, (gpsos_fmt), (gpsos_param1), (gpsos_param2))

#define GPSOS_MSG_SPRINTF_2_MED(gpsos_fmt, gpsos_param1, gpsos_param2) \
        GPSOS_MSG_SPRINTF_2(MSG_LEGACY_MED, (gpsos_fmt), (gpsos_param1), (gpsos_param2))

#define GPSOS_MSG_SPRINTF_2_HIGH(gpsos_fmt, gpsos_param1, gpsos_param2) \
        GPSOS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, (gpsos_fmt), (gpsos_param1), (gpsos_param2))

#define GPSOS_MSG_SPRINTF_2_ERROR(gpsos_fmt, gpsos_param1, gpsos_param2) \
        GPSOS_MSG_SPRINTF_2(MSG_LEGACY_ERROR, (gpsos_fmt), (gpsos_param1), (gpsos_param2))


#define GPSOS_MSG_SPRINTF_3_LOW(gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        GPSOS_MSG_SPRINTF_3(MSG_LEGACY_LOW, (gpsos_fmt), (gpsos_param1), (gpsos_param2), (gpsos_param3))

#define GPSOS_MSG_SPRINTF_3_MED(gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        GPSOS_MSG_SPRINTF_3(MSG_LEGACY_MED, (gpsos_fmt), (gpsos_param1), (gpsos_param2), (gpsos_param3))

#define GPSOS_MSG_SPRINTF_3_HIGH(gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        GPSOS_MSG_SPRINTF_3(MSG_LEGACY_HIGH, (gpsos_fmt), (gpsos_param1), (gpsos_param2), (gpsos_param3))

#define GPSOS_MSG_SPRINTF_3_ERROR(gpsos_fmt, gpsos_param1, gpsos_param2, gpsos_param3) \
        GPSOS_MSG_SPRINTF_3(MSG_LEGACY_ERROR, (gpsos_fmt), (gpsos_param1), (gpsos_param2), (gpsos_param3))

/*
 * Type definitions
*/

#ifdef __GRIFFON__
typedef void* os_ThreadHndType;
#else
typedef rex_tcb_type* os_ThreadHndType;
#endif

/*
 * Structure definitions
*/

/******************************
  IPC
*******************************/

typedef enum
{
  /* OS abstraction will use static buffer pools to guarantee buffer.
   * Failure to get this buffer will most likely Error fatal the code.
   */
  IPC_ALLOCATION_STATIC,

  /* A generic pool of buffers, where callee is expected to handle cases
   * when there is no buffer available.
   */
  IPC_ALLOCATION_DYNAMIC
} os_IpcAllocEnumType;

/* A typedef for generic IPC message data structure used across the system.
 */
typedef struct
{
  /* IPC message specific data */
  uint32               q_MsgId;         /* Unique msg ID within GPS subsystem */
  uint32               q_DestThreadId;  /* Destination ID where IPC msg is sent */
  uint32               q_SrcThreadId;   /* Source ID where IPC msg originated */

  /* Other useful data */
  uint32               q_Size;          /* Size of p_Data below in bytes */
  uint8                p_Data[1];       /* Pointer to any block of useful data */
} os_IpcMsgType;

/* A typedef for signal-originated IPC message data structure used across the system.
 */
typedef struct
{
  uint32               q_SignalId;      /* Global signal identifier */
} os_SignalMsgType;

/******************************
  Timers
*******************************/

typedef struct
{
  /* IPC message specific data */
  uint32               q_MsgId;         /* Unique msg ID within GPS subsystem */
  uint32               q_DestThreadId;  /* Destination ID where IPC msg will be sent */

  /* Other useful data */
  uint32               q_Size;          /* Size of timer data below in bytes */
  uint32               q_TimerId;       /* Unique timer ID within GPS subsystem */
  uint32               q_Param;         /* Unique count for every time timer starts */

#ifdef ARIES_USE_REX_KERNEL
  rex_timer_type  z_Timer;        /* Rex based timer structure */
  timer_group_type  *p_TimerGroup;
#endif /* ARIES_USE_REX_KERNEL */
  uint64 d_Dummy[15];            /*Add this dummy pointer to push Timer IPCs to IPC Pool 3*/
} os_TimerMsgType;

typedef struct
{
  uint32               q_TimerId;       /* Unique timer ID within GPS subsystem */
  uint32               q_Param;         /* Unique count for every time timer starts */
} os_TimerExpiryType;

typedef struct
{
  uint16               w_ClientType;    /* Unique client ID within GPS subsystem */
} os_AppTrackType;

/******************************
  Mutexes
*******************************/

typedef enum
{
  MUTEX_ISR_CONTEXT,              /* lock between thread & ISR */
  MUTEX_TASK_ONLY_CONTEXT,        /* lock between threads only */
  MUTEX_DATA_ONLY_CONTEXT         /* use rex critical sections */
} os_MutexEnumType;

typedef struct
{
  uint32            q_InitValue;  /* Nonzero if mutex is initialized */
  os_MutexEnumType  e_MutexType;  /* Mutex Type: one of three types */

#ifdef ARIES_USE_REX_KERNEL
  rex_crit_sect_type  z_CritSect; /* Rex based critical section structure */
#endif /* ARIES_USE_REX_KERNEL */

} os_MutexBlockType;


/******************************
  Dynamic Memory
*******************************/
typedef enum
{
  OS_MEM_SCOPE_TASK,
  OS_MEM_SCOPE_PROCESSOR,
  OS_MEM_SCOPE_INTERPROCESSOR
} os_MemScopeT;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function definitions
*/

/******************************
  IPC
*******************************/

/*
 ******************************************************************************
 * Function description:
 *   os_IpcStart() calls the IPC initialization routine for every task created.
 *   Also, global IPC dynamic memory pool init is done by the first task that
 *   starts IPC exactly once. This should be called at creation for each task.
 *
 * Parameters:
 *   q_ThreadId - Thread Id for task that needs to initialize IPC
 *
 * Return value:
 *   TRUE if q_ThreadId is a valid Thread Id value
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_IpcStart(uint32 q_ThreadId);

/*
 ******************************************************************************
 * Function description:
 *   os_IpcCreate() allocates a message buffer to be used for Inter-Process
 *   Communication. MsgId must be populated separately after IPC msg is created.
 * 
 *   **************************************************************************
 *   NOTE: Function ensures that IPC is memory is allocated. If memory cannot
 *   be allocated for any reason, it would trigger an error fatal
 *   This API should be used for all critical functions. If an IPC can be dropped
 *   use, os_RobustIpcCreate(), and handle the NULL return gracefully if it
 *   happens.
 *   **************************************************************************
 * 
 * Parameters:
 *   q_MsgSize - size of additional data to allocate in bytes (not including
 *               sizeof(os_IpcMsgType) bytes)
 *   e_MemType - memory type: dynamic buffer pools or static buffer pools
 *   q_DestThreadId - Thread Id where IPC msg is sent (used for static buffers)
 *
 * Return value:
 *   Pointer to a message buffer of IPC msg type
 *
 * Currently, only dynamic buffer pools are supported.
 *
 ******************************************************************************
*/
os_IpcMsgType* os_IpcCreate(uint32 q_MsgSize, os_IpcAllocEnumType e_MemType, uint32 q_DestThreadId);

/*
 ******************************************************************************
 * Function description:
 *   os_RobustIpcCreate() allocates a message buffer to be used for Inter-Process
 *   Communication. MsgId must be populated separately after IPC msg is created.
 * 
 *   **************************************************************************
 *   NOTE: This function can return a NULL pointer if IPC memory cannot be
 *   allocated. Caller function needs to handle the NULL pointer gracefully 
 *   **************************************************************************
 *
 * Parameters:
 *   q_MsgSize - size of additional data to allocate in bytes (not including
 *               sizeof(os_IpcMsgType) bytes)
 *   e_MemType - memory type: dynamic buffer pools or static buffer pools
 *   q_DestThreadId - Thread Id where IPC msg is sent (used for static buffers)
 *
 * Return value:
 *   Pointer to a message buffer of IPC msg type
 *
 * Currently, only dynamic buffer pools are supported.
 *
 ******************************************************************************
*/
os_IpcMsgType* os_RobustIpcCreate
(
   uint32 q_MsgSize, 
   os_IpcAllocEnumType e_MemType, 
   uint32 q_DestThreadId
);

/*
 ******************************************************************************
 * Function description:
 *   os_IpcSend() sends an IPC message to a destination thread.
 *
 * Parameters:
 *   p_IpcMsg - Pointer to the IPC Msg Structure
 *   q_DestThreadId - Destination Thread Id where IPC msg is sent
 *
 * Return value:
 *   TRUE if p_IpcMsg is a valid IPC Msg Struct and the send was successful
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_IpcSend(os_IpcMsgType *p_IpcMsg, uint32 q_DestThreadId);

/*
 ******************************************************************************
 * Function description:
 *   os_IpcReceive() receives an IPC message. It is a blocking call when no
 *   message is available, so it won't return until an IPC message is finally
 *   received. It is a generic receive function, and not msg_id specific.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   Pointer to a message buffer of IPC msg type
 *
 ******************************************************************************
*/
os_IpcMsgType* os_IpcReceive( void );

/*
 ******************************************************************************
 * Function description:
 *   os_IpcDelete() frees the IPC message buffer allocated from IPC create.
 *   The buffer is returned to the memory pool from which it was allocated.
 *
 * Parameters:
 *   p_IpcMsg - Pointer to the IPC Msg Structure
 *
 * Return value:
 *   TRUE if p_IpcMsg is a valid IPC Msg Struct and the delete was successful
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_IpcDelete(os_IpcMsgType *p_IpcMsg);

/*
 ******************************************************************************
 * Function description:
 *   os_IpcMapSignals() enables processing of signals within os_IpcReceive()
 *   function. When a mapped signal is received, the os_SignalMsgType message
 *   is produced.
 *
 *   If multiple signals are mapped to the same message id, each of them will
 *   generate own IPC message.
 *
 *   The function fails if the signal argument is wrong, or the signal is
 *   already mapped to IPC.
 *
 *   Only local signals (0-15) can be mapped.
 *
 *   If multiple instances of a signal are queued while that signal is blocked,
 *   only one instance is queued. The handling part shall not rely on the
 *   count of signal IPC messages received, and perform proper state check
 *   after each message as it may mask multiple signal events.
 *
 * Parameters:
 *   q_SignalsMask - Signal mask
 *   q_IpcMsgId - Identifier of message to generate when signal is encountered
 *
 * Return value:
 *   TRUE if q_SignalsMask and q_IpcMsgId are valid and signal is mapped
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_IpcMapSignals(uint32 q_SignalsMask, uint32 q_IpcMsgId);

/*
 ******************************************************************************
 * Function description:
 *   os_IpcUnmapSignals() disables processing of global signals within
 *   os_IpcReceive() function. When not mapped, the signal is either disabled
 *   or processed in a default manner.
 *
 *   The function fails if the signal argument is wrong, or any of the signal is
 *   not mapped to IPC.
 *
 * Parameters:
 *   q_SignalsMask - Signal mask
 *
 * Return value:
 *   TRUE if q_SignalsMask is valid and all signals are unmapped
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_IpcUnmapSignals(uint32 q_SignalsMask);

/******************************
  Timers
*******************************/

/*
 ******************************************************************************
 * Function description:
 *   os_TimerCreate() allocates a message buffer to be used for timers. It
 *   also initializes the timer so that it can be started/stopped at a later
 *   time.
 *
 * Parameters:
 *   q_TimerId - Timer Id which differentiates between different timers
 *   q_DestThreadId - Thread Id where IPC msg is sent when timer elapses
 *
 * Return value:
 *   Pointer to a message buffer of timer type
 *
 * For REX based systems, Rex extended timer needs to be defined to allow
 * IPC-based timers (i.e. must define FEATURE_REX_TIMER_EX to compile)
 *
 ******************************************************************************
*/
os_TimerMsgType* os_TimerCreate(uint32 q_TimerId, uint32 q_DestThreadId);


/*
******************************************************************************
* Function description:
*   os_TimerCreateInCgpsTimerGroup() allocates a message buffer to be used for timers. It
*   also initializes the timer so that it can be started/stopped at a later
*   time.
*   the timer is created in the cgps_timer_group.  This group is not disabled in LPM.
*
* Parameters:
*   q_TimerId - Timer Id which differentiates between different timers
*   q_DestThreadId - Thread Id where IPC msg is sent when timer elapses
*
* Return value:
*   Pointer to a message buffer of timer type
*
*
******************************************************************************
*/
os_TimerMsgType* os_TimerCreateInCgpsTimerGroup(uint32 q_TimerId, uint32 q_DestThreadId);


/*
 ******************************************************************************
 * Function description:
 *   os_TimerStart() starts a given timer to expire in the future.
 *
 * Parameters:
 *   p_Timer - Pointer to the Timer Structure
 *   q_Msecs - Number of milliseconds after which the timer should expire
 *   q_Param - value which identifies between different timer start instances
 *
 * Return value:
 *   TRUE if p_Timer is a valid Timer Structure
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_TimerStart(os_TimerMsgType *p_Timer, uint32 q_Msecs, uint32 q_Param);

/*
 ******************************************************************************
 * Function description:
 *   os_TimerStop() stops the given timer.
 *
 * Parameters:
 *   p_Timer - Pointer to the Timer Structure
 *
 * Return value:
 *   TRUE if p_Timer is a valid Timer Structure
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_TimerStop(os_TimerMsgType *p_Timer);

/*
 ******************************************************************************
 * Function description:
 *   os_TimerCheck() checks the current remaining count of a given timer.
 *
 * Parameters:
 *   p_Timer - Pointer to the Timer Structure
 *
 * Return value:
 *   count left of a given timer
 *
 ******************************************************************************
*/
uint32 os_TimerCheck(os_TimerMsgType *p_Timer);

/*
 ******************************************************************************
 * Function description:
 *   os_TimerDestroy() deallocates the timer which was allocated at create time.
 *
 * Parameters:
 *   p_Timer - Pointer to the Timer Structure
 *
 * Return value:
 *   TRUE if p_Timer is a valid Timer Structure
 *   FALSE otherwise
 *
 ******************************************************************************
*/
boolean os_TimerDestroy(os_TimerMsgType *p_Timer);


/******************************
  Mutexes
*******************************/

/*
 ******************************************************************************
 * Function description:
 *   os_MutexInit() creates a mutex which can be associated with a
 *   resource.
 *
 * Parameters:
 *   p_Mutex - Pointer to the Mutex Structure
 *   e_Type - Mutex type (Thread Only Context, ISR Context, Critical Sections)
 *
 * Return value:
 *   TRUE if initialized with no errors
 *   FALSE otherwise
 *
 * Mutexes are used to allow only one thread to enter the critical section
 * of code that is accessing shared data. Thus, a given data structure which
 * needs mutual exclusion will need to create a mutex to go with it. There
 * will be three different types of mutexes created; one which is accessed from
 * threads only, and another which is accessed between threads and the ISR, and
 * another which defines critical sections using Rex.
 *
 * For the mutex accessed from threads only, context switching will be disabled
 * if shared data is accessed between different processes - where one may
 * not necessarily be the owner of (and thus not have full control of) the
 * shared data structure. This is different from critical sections, where
 * context switching is still enabled (note that if a swapped in thread
 * tries to access the shared resource critical section, it will suspend).
 *
 ******************************************************************************
*/
boolean os_MutexInit(os_MutexBlockType *p_Mutex, os_MutexEnumType e_Type);

/*
 ******************************************************************************
 * Function description:
 *   os_MutexLock() locks the resource associated with p_Mutex if it
 *   is not already locked.
 *
 * Parameters:
 *   p_Mutex - Pointer to the Mutex Structure
 *
 * Return value:
 *   None
 *
 * Nested calls to os_MutexLock() are supported and will require the same
 * number of calls to os_MutexUnlock() to allow access to the mutexed
 * resource again.
 *
 * For REX based systems, this function will do one of three things:
 *
 * - case "MUTEX_ISR_CONTEXT"
 *
 *   INTLOCK() is used to disable interrupts until after its subsequent call
 *   to os_MutexUnlock(). Context switching is still allowed. Used for mutex
 *   between thread and ISR.
 *
 * - case "MUTEX_TASK_ONLY_CONTEXT"
 *
 *   TASKLOCK() is used to disable context switching until its subsequent
 *   call to os_MutexUnlock(). Processing stays within the current task while
 *   interrupts can still occur. Good to use for inter-process synchronization.
 *   Least efficient; use as a last resort.
 *
 * - case "MUTEX_DATA_ONLY_CONTEXT"
 *
 *   Critical sections are used. Used to attempt to enter the critical section
 *   If successful, proceed to access the critical section. If not, the calling
 *   task will be suspended since the critical section is unavailable (already
 *   locked out), since context switching is still allowed. Interrupts can
 *   still occur. More efficient (much faster as far as lock times); use this
 *   mutex for all inter-thread synchronization. Critical sections are limited,
 *   however, since it cannot be shared across processes.
 *
 ******************************************************************************
*/
void os_MutexLock(os_MutexBlockType *p_Mutex);

/*
 ******************************************************************************
 * Function description:
 *   os_MutexUnlock() unlocks the resource associated with p_Mutex. If there
 *   is a thread waiting on the mutex then the mutex gets assigned to that
 *   waiting thread which is at the head of the queue.
 *
 * Parameters:
 *   p_Mutex - Pointer to the Mutex Structure
 *
 * Return value:
 *   None
 *
 * This routines supports nested lock/unlock operations.
 *
 * For REX based systems, this function will do one of three things:
 *
 * - case "MUTEX_ISR_CONTEXT"
 *
 *   INTFREE() is used to enable interrupts again.
 *
 * - case "MUTEX_TASK_ONLY_CONTEXT"
 *
 *   TASKFREE() is used to re-enable context switching. Processing can now
 *   switch between different tasks again.
 *
 * - case "MUTEX_DATA_ONLY_CONTEXT"
 *
 *   Critical sections previously used. Release the critical section that the
 *   task is holding. If a higher priority task is waiting on the critical
 *   section, then when this task releases that critical section, the higher
 *   priority task will be swapped in.
 *
 ******************************************************************************
*/
void os_MutexUnlock(os_MutexBlockType *p_Mutex);

#ifndef __GRIFFON__

/*
 ******************************************************************************
 * Function: MUTEXes used in mgp.
 *
 * Description:
 *
 *  Various mutex locks used to safeguard critical regions.
 *  - Implemented (as macro) using INTLOCK() in targets that support the operation
 *  - otherwise (in QDSP6) implemented by Mutexes
 *
 * Parameters:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/


#if  !defined(FEATURE_INTLOCK_UNSUPPORTED)
  #define GNSS_DETERMINISTIC_TIMING_ENTER INTLOCK();
  #define GNSS_DETERMINISTIC_TIMING_EXIT  INTFREE();
  #define GNSS_DETERMINISTIC_TIMING_ENTER_NO_RTC_MUTEX INTLOCK();
  #define GNSS_DETERMINISTIC_TIMING_EXIT_NO_RTC_MUTEX  INTFREE();
#else
  /* Feature INTLOCK is not supported and CSC is supported */
  /* To obtain a deterministic timing context, use the
     blast supported interface of looking at the trace marker at the
     start of critical timing section, and at the end of the critical
     timing section, ensure that there are no context switches.
     If there are any context switches, go through the process or running
     the code of the critical section again (for upto CSC_RETRY_COUNT_MAX times)

     The the critical timing to be maintained correctly, we should not have
     MUTEX locks from within the critical timing section.
     (Having a MUTEX in this context is similar to calling a MUTEX lock from ISR context).

     At many places in the GNSS software, the deterministic timing enter macros
     are used to latch the 32KHz Slow clock counter edge and a GNSS_RTC.
     To latch GNSS_RTC, we need to access the GNSS_GPS_RTC_MUTEX (to protect
     against clock being switched off from another task)
     If the RTC MUTEX is un-available during the critical timing reads, there is
     no guarantee that BLAST trace marker is properly incremented to indicate a time delay
     So, there could be a timing error in associating the edge of 32KHz clock with
     GNSS_RTC.

     As an example, in the follwoing sequence of operations, BLAST would not detect a
     context switch, so GNSS code will assume that the timing relation is consistent,
     where as there is an unaccounted for delay in steps (d) to (f) below.

     (a) Enter the Deterministic timing section. Grab the CSC counter
     (b) Wait for the edge of 32KHz osc, and read the 32KHz counter
     (c) Grab RTC MUTEX before the read.
     (d) If the MUTEX is not available, BLAST KERNEL figures out the next task to run
         and modifies the stack, priorities etc.
     (e) BLAST KERNEL, now tries to block on the MUTEX, but the MUTEX is not available,
         because the other task has released the MUTEX.
     (f) Since the MUTEX is available, KERNEL returns with MUTEX lock and continues execution
         Blast trace marker is not incremented in this case.
     (g) Read GNSS_RTC
     (h) Release MUTEX.

     To avoid this uncontrolled delay in the steps (d) through (f), grab the RTC MUTEX in Step(a)
     and then release it in Step (h).
  */


  #if defined(FEATURE_BLAST)

    #define GNSS_DETERMINISTIC_TIMING_ENTER                  \
    {                                                        \
      uint32 q_retry_count = 0;                              \
      uint32 q_csc;                                          \
      GNSS_GPS_RTC_MUTEX_TAKE;                               \
      do                                                     \
      {                                                      \
        q_csc = blast_get_trace_marker(); /* fetch CSC */

    #define GNSS_DETERMINISTIC_TIMING_EXIT            \
      /* make sure context not switched out, redo operations if interrupted */     \
      } while(blast_has_preempt_trace( q_csc) &&      \
              (++q_retry_count<CSC_RETRY_COUNT_MAX)); \
      GNSS_GPS_RTC_MUTEX_GIVE;                               \
      if( q_retry_count >= CSC_RETRY_COUNT_MAX )      \
        MSG_FATAL( "DT - BLAST CSC max retries",0,0,0);    \
    }

    #define GNSS_DETERMINISTIC_TIMING_ENTER_NO_RTC_MUTEX     \
    {                                                        \
      uint32 q_retry_count = 0;                              \
      uint32 q_csc;                                          \
      do                                                     \
      {                                                      \
        q_csc = blast_get_trace_marker(); /* fetch CSC */

    #define GNSS_DETERMINISTIC_TIMING_EXIT_NO_RTC_MUTEX      \
      /* make sure context not switched out, redo operations if interrupted */     \
      } while(blast_has_preempt_trace( q_csc) &&      \
              (++q_retry_count<CSC_RETRY_COUNT_MAX)); \
      if( q_retry_count >= CSC_RETRY_COUNT_MAX )      \
        MSG_FATAL( "DT - BLAST CSC max retries",0,0,0);    \
    }

  #else
    #if defined(FEATURE_QURT)
    #define GNSS_DETERMINISTIC_TIMING_ENTER                  \
    {                                                        \
      uint32 q_retry_count = 0;                              \
      uint32 q_csc;                                          \
      GNSS_GPS_RTC_MUTEX_TAKE;                               \
      do                                                     \
      {                                                      \
        q_csc = qurt_trace_get_marker(); /* fetch CSC */

    #define GNSS_DETERMINISTIC_TIMING_EXIT            \
      /* make sure context not switched out, redo operations if interrupted */     \
      } while(qurt_trace_changed( q_csc, 0x3 ) &&      \
              (++q_retry_count<CSC_RETRY_COUNT_MAX)); \
      GNSS_GPS_RTC_MUTEX_GIVE;                               \
      if( q_retry_count >= CSC_RETRY_COUNT_MAX )      \
        MSG_FATAL( "DT - BLAST CSC max retries",0,0,0);    \
    }

    #define GNSS_DETERMINISTIC_TIMING_ENTER_NO_RTC_MUTEX     \
    {                                                        \
      uint32 q_retry_count = 0;                              \
      uint32 q_csc;                                          \
      do                                                     \
      {                                                      \
        q_csc = qurt_trace_get_marker(); /* fetch CSC */

    #define GNSS_DETERMINISTIC_TIMING_EXIT_NO_RTC_MUTEX      \
      /* make sure context not switched out, redo operations if interrupted */     \
      } while(qurt_trace_changed( q_csc, 0x3 ) &&      \
              (++q_retry_count<CSC_RETRY_COUNT_MAX)); \
      if( q_retry_count >= CSC_RETRY_COUNT_MAX )      \
        MSG_FATAL( "DT - BLAST CSC max retries",0,0,0);    \
    }
    #else
       /* Feature INTLOCK is not supported and CSC is not supported*/
      #error " BLAST as well as QURT are not supported"
    #endif/* defined(FEATURE_QURT) */
  #endif /* defined(FEATURE_BLAST) */
#endif /* !defined FEATURE_INTLOCK_UNSUPPORTED */



/* Below block replaces TASKLOCK for QDSP6 targets.
  GNSS_TASKLOCK and GNSS_TASKFREE macros ensure that no context
  switches happen within these macros (for QDSP6 targets).
  In other words, this takes care of INTLOCK as well.
  An interrupt is also considered as context switch.If there is a
  context switch then we retry and get timetick value again.
*/
#if  !defined(FEATURE_INTLOCK_UNSUPPORTED)
  #define GNSS_TASKLOCK  TASKLOCK();
  #define GNSS_TASKFREE  TASKFREE();
#else
  /* Feature INTLOCK is not supported and CSC is supported */
  #if defined(FEATURE_BLAST)

    #define GNSS_TASKLOCK                                    \
    {                                                        \
      uint32 q_retry_count = 0;                              \
      uint32 q_csc;                                          \
      do                                                     \
      {                                                      \
        q_csc = blast_get_trace_marker(); /* fetch CSC */

    #define GNSS_TASKFREE                                    \
      /* make sure context not switched out, redo operations if interrupted */     \
      } while(blast_has_preempt_trace( q_csc) &&      \
              (++q_retry_count<CSC_RETRY_COUNT_MAX)); \
      if( q_retry_count >= CSC_RETRY_COUNT_MAX )      \
        MSG_FATAL("TASK - BLAST CSC max retries",0,0,0);    \
    }
  #else
    #if defined(FEATURE_QURT)
    #define GNSS_TASKLOCK                                    \
    {                                                        \
      uint32 q_retry_count = 0;                              \
      uint32 q_csc;                                          \
      do                                                     \
      {                                                      \
        q_csc = qurt_trace_get_marker(); /* fetch CSC */

    #define GNSS_TASKFREE                                    \
      /* make sure context not switched out, redo operations if interrupted */     \
      } while(qurt_trace_changed( q_csc, 0x3 ) &&      \
              (++q_retry_count<CSC_RETRY_COUNT_MAX)); \
      if( q_retry_count >= CSC_RETRY_COUNT_MAX )      \
        MSG_FATAL("TASK - BLAST CSC max retries",0,0,0);    \
    }
    #else
       /* Feature INTLOCK is not supported and CSC is not supported*/
      #error " BLAST as well as QURT are not supported"
    #endif/* defined(FEATURE_QURT) */
  #endif /* defined(FEATURE_BLAST) */
#endif /* !defined FEATURE_INTLOCK_UNSUPPORTED */


#ifdef FEATURE_INTLOCK_UNSUPPORTED
void gnss_gps_rtc_mutex_take_api(void);
void gnss_gps_rtc_mutex_give_api(void);

void gnss_ext_blank_mutex_take_api(void);
void gnss_ext_blank_mutex_give_api(void);

boolean gnss_gps_rtc_mutex_init (void);
boolean gnss_ext_blank_mutex_init (void);

#endif  /* FEATURE_INTLOCK_UNSUPPORTED */
#endif //__GRIFFON__

/******************************
  Dynamic Memory
*******************************/

/*
 ******************************************************************************
 * Function description:
 *   os_MemAlloc() allocates a block of memory from the heap
 *
 * Parameters:
 *   size - Number of bytes to allocate
 *   scope - TASK, PROCESSOR, or INTERPROCESSOR
 *
 * Return value:
 *    Returns a pointer to the newly allocated block or NULL if the
 *    block could not be allocated
 *
 ******************************************************************************
*/
void * os_MemAlloc(
              size_t          size,
              os_MemScopeT    scope );


/*
 ******************************************************************************
 * Function description:
 *   os_MemFree() deallocates a previously allocated block of memory
 *
 * Parameters:
 *   ptr - Pointer to Pointer of a previously allocated block of memory. Done so
 *         that the caller does not have a dangling pointer.
 *
 * Return value:
 *    None
 *
 ******************************************************************************
*/
void os_MemFree(
              void **  ptr_ptr );

/*
 ******************************************************************************
 * Function description:
 *   os_MemReAlloc() resizes a previously allocated block of memory from the
 *   heap (ptr) to new specified size (size) while preserving the block's
 *   content.
 *
 *   If the block is shortened, bytes are discarded off the end.  If the block
 *   is lengthened, the new bytes added are not initialized and will have
 *   garbage values.
 *
 *   If ptr is NULL, the function behaves exactly like os_MemAlloc(). If ptr
 *   is not NULL and size is 0, the function behaves exactly like os_MemFree()
 *
 *   If the block cannot be resized, ptr is not NULL and size is not 0, then
 *   NULL is returned and the original block is left untouched.  If the ptr
 *   block IS successfully resized and the returned value is different from
 *   the ptr value passed in, the old block passed in must be considered
 *   deallocated and no longer use-able (i.e. do NOT try to os_MemFree() it!)
 *   This function will reallocate size bytes if it can't grow the block in
 *   place.
 *
 * Parameters:
 *   ptr   - block previously allocated from the heap or NULL pointer
 *   size  - New size (in bytes) of the previously allocated memory
 *   scope - TASK, PROCESSOR, or INTERPROCESSOR
 *
 * Return value:
 *    Returns a pointer to the beginning of the resized block of memory
 *    (which may be different than ptr) or NULL if the block cannot be resized
 *    in place.
 *
 ******************************************************************************
*/
void * os_MemReAlloc(
                     void **         ptr_ptr,
                     size_t          size,
                     os_MemScopeT    scope );


/*-------------------------------------------------------------------
 *  OS API Signal Handling
 *
 *  GPS code now must build and execute properly in SOC builds as well
 *  as QCT Modem builds.  SOC builds are built upon an ex-Atheros
 *  UPOS layer.  There are features available in UPOS which are not available
 *  in QCT modem builds (ie REX) and vice-versa.  Callisto code needs
 *  to be designed to depend on the subset of features support by both.
 *
 *  An original ARIES design goal was to design every task with a single
 *  point wait at the top of the task loop and use ONLY IPC messaging
 *  for inter-task communication.  This works fine for communicating
 *  between GPS tasks.  It breaks down when communicating with
 *  non-GPS tasks which do not limit themselves to IPC messaging.  This OS
 *  API abstraction layer was intentionally designed to NOT support
 *  signals.  Because of this, several GPS module developers
 *  inserted code into the codebase which calls REX directly to overcome
 *  the "limitations" of the OS API module.  This created problems
 *  when moving to SOC builds which did not support REX.  It is desirable
 *  to have all Callisto code using the OS API abstraction, so it is
 *  easily supported on both SOC and QCT modem builds.
 *
 *  In order to adequately support the needs for signal handling in
 *  QCT modem builds, a new Signal handling abstraction is needed in
 *  the OS API module.
 *
 *  A significant difference between UPOS and REX (and other OS's used
 *  on QCT modem builds) is that UPOS Signalling only supports 16-bit
 *  event flag masks, but REX supports 32-bits.  It is EXTREMELY important
 *  that no GPS code be designed to depend upon 32-bit masks.  To this end,
 *  the new Signal Handling abstraction is being designed to support
 *  8 bits of "GPS system-wide global signals" (things like
 *  IPC_MSG_RCVD, UPOS_TIMER_RCVD, etc.) which are supported for all threads
 *  (whether they use them or not) and 8 bits of "per-task signals" which can be
 *  assigned as needed by individual threads.  The abstraction will attempt to
 *  make it difficult (or impossible) to attempt to use more than 16 bits total
 *  and to mix up global and local signals.
 *
 *  QCT Modem SW global signals are still supported in QCT Modem SW
 *  builds, but those signals are hidden inside the OS API implementation
 *  OS API clients have no access to thos signals.
 *
 *  When communicating TO other QCT modem SW threads, it may be necessary
 *  to send an event mask with a bit set in the upper half of a 16-bit
 *  word.  This will not be accommodated in this abstraction layer.  Instead,
 *  developers will be allowed to call REX directly in these cases.  The
 *  plan is to not allow this in any files deliver to SOC builds, so SOC
 *  builds do not need REX "stubs".
 *-------------------------------------------------------------------*/


/*
 ******************************************************************************
 * os_ClrLocalSignal()
 *
 * This non-blocking function clears one or more LOCAL signals and returns
 * the value of those signals before they were cleared
 *
 * Parameters:
 *   clrMask - Mask of signals to be cleared ("1" means it is to be cleared)
 *
 * Returns:
 *   prevMask - Mask of signals that were cleared by this function (ie. they
 *              were set before calling this function AND were cleared by
 *              this function.
 ******************************************************************************
 */
uint8 os_ClrLocalSignal(
              uint8   clrMask );


/*
 ******************************************************************************
 * os_SetLocalSignal()
 *
 * This non-blocking function sets one or more LOCAL signals for a GPS task
 *
 * Parameters:
 *   destThreadId - Thread ID to be signaled
 *   setMask      - Mask of signals to be set ("1" means it is to be set)
 ******************************************************************************
 */
void os_SetLocalSignal(
              uint32      destThreadId,
              uint8       setMask );

/*
 ******************************************************************************
 * os_SetLocalSignalByHnd()
 *
 * This non-blocking function sets one or more LOCAL signals for a GPS task
 * using task handler
 *
 * Parameters:
 *   p_DestThreadHnd - Thread handler to be signaled
 *   setMask      - Mask of signals to be set ("1" means it is to be set)
 ******************************************************************************
 */
void os_SetLocalSignalByHnd(
              os_ThreadHndType p_DestThreadHnd,
              uint8 setMask );

/*
 ******************************************************************************
 * os_WaitLocalSignal()
 *
 * This function blocks the calling thread until one or more
 * LOCAL signals are set
 *
 * Parameters:
 *   waitMask - Mask of signals upon which to wait.  ("1" means wait on that signal)
 *
 *   sigMask - Mask of signals that were waited on AND are set
 ******************************************************************************
 */
uint8 os_WaitLocalSignal(
              uint8   waitMask );


/*-------------------------------------------------------------------
 *  SLEEP
 *-------------------------------------------------------------------*/

/*
 ******************************************************************************
 * os_SleepMs()
 *
 * This function sleeps for a caller specified number of milliseconds
 *
 ******************************************************************************
 */
void os_SleepMs( uint32 ms );

/*
 ******************************************************************************
 * os_SleepMs_NonDeferrable()
 *
 * This function sleeps for a caller specified number of milliseconds
 * (using non-deferrable timer for sleep, will wake up even in power collapse)
 *
 * return TRUE if this sleep operation is successful, otherwise FALSE
 * (for example, if system doesn't support non-deferrable sleep)
 ******************************************************************************
 */
boolean os_SleepMs_NonDeferrable( uint32 ms );


/*-------------------------------------------------------------------
 *  MISC
 *-------------------------------------------------------------------*/

/*
 ******************************************************************************
 * os_Self()
 *
 * This function returns a pointer to the calling task's TCB.
 *
 * Returns:
 *   Pointer to the calling task's TCB
 ******************************************************************************
 */
#ifdef __GRIFFON__
void * os_Self( void );
#else
rex_tcb_type * os_Self( void );
#endif

/*
 ******************************************************************************
 * os_TaskStopRegister()
 *
 * Registers the current task for the proper STOP signal
 *
 ******************************************************************************
 */
void os_TaskStopRegister( void );

/*
 ******************************************************************************
 * os_TaskReadyAck()
 *
 * Informs task controller current task has completed its initialization.
 *
 ******************************************************************************
 */
void os_TaskReadyAck( void );

/*
 ******************************************************************************
 * os_TaskStopAck()
 *
 * Informs the task controller that the current task has completed its stop procedure.
 *
 ******************************************************************************
 */
void os_TaskStopAck( void );

#ifdef __GRIFFON__
void os_Init(void);
#endif //__GRIFFON__

/*
 ******************************************************************************
 * os_autodog_enable_self
 *
 * Performs initialization needed to set up the auto dog service for a task.
 * When auto-dog is on, the task does not need to wake up periodically to
 * kick the dog when making a blocking call. OS takes care of notifying
 * Dog that the task is blocked and that there is no need to monitor the
 * task until further notice. When the task is scheduled again, Dog is
 * notified that the task is ready to be monitored again.
 *
 * Returns:
 *   None
 ******************************************************************************
 */
void os_autodog_enable_self(void);

/*
 ******************************************************************************
 * os_DogHeartbeatRegister()
 *
 * Registers a task with the DOG heartbeat mechanism.  This will cause a
 * DOG heartbeat event/signal to be sent to the calling task at a periodic rate
 * to be determined by the DOG module itself (a standard default used by all
 * tasks using this mechanism).  This signal will be handled by the OS API
 * framework by reporting "I'm Alive" to the DOG heartbeat mechanism.  The task
 * itself need not set a timer nor handle the DOG heartbeat event/signal.  It
 * is handled by the OS API framework.
 *
 * Returns:
 *   None
 ******************************************************************************
 */
void os_DogHeartbeatRegister(void);

/*
 ******************************************************************************
 * os_DogHeartbeatDeregister()
 *
 * Deregisters a task with the DOG heartbeat mechanism.
 *
 * Returns:
 *   None
 ******************************************************************************
 */
void os_DogHeartbeatDeregister(void);

/*
 ******************************************************************************
 * os_DogHeartbeatReport()
 *
 * Tasks use this API to respond to the DOG heartbeat . ( "PONG" mechanism for the
 * DOG "PING")
 *
 * Returns:
 *   None
 ******************************************************************************
 */
void os_DogHeartbeatReport(void);

/*
 ******************************************************************************
 * os_KillSelf()
 *
 * Kills a task by removing it from the list of active tasks.  If a task
 * makes a call to os_kill_task to kill itself, then after the TCB is
 * removed from the list, a call to the scheduler is made to find a task to
 * run in place of the killed task.
 *
 * Returns:
 *   None
 ******************************************************************************
 */
void os_KillSelf(void);

/*
 ******************************************************************************
 * Function os_GetCpuUsageStatus
 *
 * Description:
 *
 *  Check how busy the CPU was in the recent past. If above a threshold return
 *  FALSE, suggesting that the task yield the CPU for a few ms before continuing.
 *
 * Parameters:
 *
 *  *p_StartTimeTicks - For the caller to get the start time.
 *
 * Return value:
 *
 *  Boolean - FALSE indicates the caller should try to yield CPU. TRUE indicates
 *  no need to yield CPU.
 *
 ******************************************************************************
*/
boolean os_GetCpuUsageStatus( uint32 *p_StartTimeTicks );

/*
 ******************************************************************************
 * Function os_RecordCpuUsageTime
 *
 * Description:
 *
 *  Compute the CPU usage using the start time anc curr time. Store the values
 *  in the respective task's circular buffer.
 *
 * Parameters:
 *
 *  q_StartTimeTicks - Start time of the caller's IPC processing.
 *  q_ThreadId - Thread id of the caller
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void os_RecordCpuUsageTime( uint32 q_StartTimeTicks, os_ThreadIdEnumType q_ThreadId );

/*
 ******************************************************************************
 * Function os_GitConfigure
 *
 * Description:
 *
 *  Configure the CPU usage threshold value and window size.
 *
 * Parameters:
 *
 *  q_ThresholdTicks - CPU usage threshold value.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void os_GitConfigure(uint32 q_ThresholdTicks);

/*
 ******************************************************************************
 * os_CurrentThreadIdGet()
 *
 * This is an helper function which calls os_ThreadIdGet().
 * This function is visible to outside modules.
 * 
 * Returns:
 *   thread id
 ******************************************************************************
 */

uint32 os_CurrentThreadIdGet();

#ifdef __cplusplus
}
#endif

#endif /* OS_API_H */
