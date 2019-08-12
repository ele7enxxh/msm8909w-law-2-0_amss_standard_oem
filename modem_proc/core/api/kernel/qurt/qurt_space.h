#ifndef QURT_SPACE_H
#define QURT_SPACE_H
/**
  @file qurt_space.h
  @brief Prototypes of QuRT process control APIs

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/


/** This flag is a request to the OS to suspend the processes just before calling main() */
#define SPAWNN_FLAG_SUSPEND_ON_STARTUP 0x1

/**
 * The qurt_spawn_args() function create and start a process from ELF file of given name. The slash symbols 
 * "/" or "\" will be ignored. Please do not include directory name in input. Additionally this function
 * accepts the the SPAWN flags. Multiple SPAWN flags can be specified by OR'ing the the flags.
 *
 * @param name      ELF file name of the executable. Name shall not contain directories, e.g.
 *                  use "dsp2.elf", instead of "/prj/qct/.../dsp2.elf"
 *
 * @param return    address space ID (0-32) if operation successful.
 *                  negative error code if failed
 *
*/

int qurt_spawn_flags(const char * name, int flags);

/**
   The qurt_spawn() function creates and starts a process from an ELF file of the specified name. The slash symbols 
   "/" or "\" will be ignored. Do not include the directory name in the input. 
  
   @param name      ELF file name of the executable. Name shall not contain directories, e.g.
                    use "dsp2.elf", instead of "/prj/qct/.../dsp2.elf"
  
   @return
   Address space ID (0-32) -- Success. \m
   Negative error code -- Failure.
  
*/
#define qurt_spawn(name) qurt_spawn_flags(name,0)

/**
 * The qurt_getpid() function returns process ID (address space ID) of the current process.
 *
 * @return    address space ID (0-32) 
 *
*/
#define qurt_getpid qurt_process_get_id

/**
 * The qurt_space_switch() function context switches to the process of given process ID. This is a priviledged
 * API. It only works if the caller is from Guest OS.
 *
 * @param asid      [0..31] process ID (address space ID) of destination process space
 *
 * @return    address space ID of previous process if operation successful. 
 *                  negative error code if failed
 *
*/
int qurt_space_switch(int asid);

/**
 * The qurt_wait() function  waits for status change in a child process. It could be used by parent
 * process to block on any child process terminates.
 *
 * @param status    Pointer to status variable. The variable provides the status value of child process. 
 *                  The value comes from exit() system call made by child process.
 *
 * @return    address space ID of child process that changing status if operation successful. 
 *                  negative error code if failed
 *
*/

int qurt_wait(int *status);

#endif
