#ifndef QURT_SHMEM_H
#define QURT_SHMEM_H

/*======================================================================
                        qurt_shmem.h

GENERAL DESCRIPTION
  Prototypes of QuRT inter-process shared memory APIs

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
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_shmem.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-11-25 	George			Add Header file
 ======================================================================*/


typedef unsigned int mode_t;

/**
 * The shm_open() function establish a connection between a shared memory object and a file descriptor.
 * The file descriptor is used by other functions such as mmap() to refer to that shared memory object.
 * 
 *
 * @param name      Pointer to string naming a shared memory object. Name has to start with "/shm/"
 * @param oflag     File status flags and file access modes of the open file description. Following
 *                  flags are defined in <fcntl.h> and supported:
 *                  O_RDONLY: oepn for read access only
 *                  O_RDWR: Open for read or write access
 *                  O_CREAT: If shared memory object doesn't exist, create one.
 * @param mode      Permission flags (currently ignored)
 *
 * @param return    file descriptor (positive number) if operation successful.
 *                  negative error code if failed
 *
*/

int shm_open(const char * name, int oflag, mode_t mode);

/**
 * The shm_close() function remove a connection between a shared memory object and a file descriptor.
 * If there is no file descriptor connects to the shared memory object, the shared memory object will
 * be deleted automatically. Shared memory object has same virtual address in any process. This is 
 * restriction of single virtual address space. 
 * 
 *
 * @param fd        File descriptor of shared memory object
 *
 * @param return    0 if operation successful.
 *                  negative error code if failed
 *
*/


int shm_close(int fd);

#endif
