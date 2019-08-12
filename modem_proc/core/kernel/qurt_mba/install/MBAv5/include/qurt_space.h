#ifndef QURT_SPACE_H
#define QURT_SPACE_H
/*======================================================================
                        qurt_space.h

GENERAL DESCRIPTION
  Prototypes of QuRT process control APIs

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_space.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-11-25 	George			Add Header file
 ======================================================================*/



/**
 * The qurt_spawn() function create and start a process from ELF file of given name. The slash symbols 
 * "/" or "\" will be ignored. Please do not include directory name in input. 
 *
 * @param name      ELF file name of the executable. Name shall not contain directories, e.g.
 *                  use "dsp2.elf", instead of "/prj/qct/.../dsp2.elf"
 *
 * @param return    address space ID (0-32) if operation successful.
 *                  negative error code if failed
 *
*/
int qurt_spawn(const char * name);

/**
 * The qurt_getpid() function returns process ID (address space ID) of the current process.
 *
 * @param return    address space ID (0-32) 
 *
*/
int qurt_getpid(void);

/**
 * The qurt_space_switch() function context switches to the process of given process ID. This is a priviledged
 * API. It only works if the caller is from Guest OS.
 *
 * @param asid      [0..31] process ID (address space ID) of destination process space
 *
 * @param return    address space ID of previous process if operation successful. 
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
 * @param return    address space ID of child process that changing status if operation successful. 
 *                  negative error code if failed
 *
*/

int qurt_wait(int *status);

#endif
