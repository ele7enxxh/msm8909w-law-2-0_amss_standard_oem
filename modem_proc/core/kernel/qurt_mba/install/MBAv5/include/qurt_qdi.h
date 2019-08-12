#ifndef QDI_H
#define QDI_H

/*======================================================================
                        qurt_qdi.h

GENERAL DESCRIPTION
  Prototypes of QuRT Driver Invocation API functions      

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
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_qdi.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-11-25 	Jack			Add Header file
 ======================================================================*/

#include "qurt_qdi_constants.h"
#include "qurt_qdi_imacros.h"

#define QDI_INVOKE_ARGS \
   void *, int, \
   qurt_qdi_arg_t, qurt_qdi_arg_t, qurt_qdi_arg_t, qurt_qdi_arg_t, \
   qurt_qdi_arg_t, qurt_qdi_arg_t, qurt_qdi_arg_t, qurt_qdi_arg_t

typedef union {
  void *ptr;
  int num;
} qurt_qdi_arg_t;

typedef int (*qurt_qdi_pfn_invoke_t)(QDI_INVOKE_ARGS);

typedef struct {
  qurt_qdi_pfn_invoke_t invoke;
} *qurt_qdi_obj_t, qurt_qdi_iobject;

typedef struct {
  qurt_qdi_obj_t *objptr;
  unsigned maxobj;
} qurt_qdi_table_t;

/**
 * User-visible API to open a QDI device driver.
 *
 * qurt_qdi_open() is the primary mechanism by which a driver user can
 *  obtain a QDI handle.  The user provides the name of the driver
 *  to the qurt_qdi_open() call, and gets back a handle referencing
 *  the named driver.
 * qurt_qdi_open() takes one mandatory parameter -- the driver name.
 *  A 2nd and 3rd parameter are optional and should follow the
 *  POSIX open() convention.  The 2nd parameter, if present,
 *  may indicate the handle access which is requested (read-only,
 *  write-only, or read-write, for instance) and other flags such
 *  as whether the call should create a new device or only open
 *  an existing device.  The 3rd parameter, if present, may
 *  indicate the permissions to be configured when a new device
 *  is created.  Note that QDI, unlike POSIX open(), does not
 *  mandate the existence of the 2nd parameter; also, QDI does
 *  not specify the format of the 2nd and 3rd parameters, but
 *  drivers are encouraged to stick with the POSIX semantics.
 *
 * @param  name   Device name to be opened
 * @param  flags  Optional 2nd parameter (POSIX flags)
 * @param  mode   Optional 3rd parameter (POSIX mode)
 *
 * @return non-negative integer handle if successful
 * @return negative error code if the driver could not be opened
 */
// int qurt_qdi_open();
#define qurt_qdi_open(p,...) ({int _QDMTCAT(qurt_qdi_qo,_QDMCNT(a,b,p,##__VA_ARGS__))_QDMLL((_QDMUO),_QDMT,p,##__VA_ARGS__);\
                              _QDMTCAT(qurt_qdi_qo,_QDMCNT(a,b,p,##__VA_ARGS__))_QDMLL((_QDMUO),_QDME,p,##__VA_ARGS__);})

/**
 * User-visible API to invoke an operation on a QDI device driver.
 *
 * qurt_qdi_handle_invoke() is the primary API used to access a QDI
 *  device driver.  The user provides a QDI handle and an integer
 *  method number, along with 0 to 8 optional 32-bit arguments.
 *  The device driver invocation function is invoked with the
 *  same method number and 0 to 8 optional arguments, and the
 *  return value from the invocation function is passed back to
 *  the user as the return value of qurt_qdi_handle_invoke().
 *
 * @param  handle Integer handle to a QDI device driver
 * @param  method Integer method number passed to the device driver
 * @param  arg1   Optional 1st parameter to device driver
 * @param  arg2   Optional 2nd parameter to device driver
 * @param  arg3   Optional 3rd parameter to device driver
 * @param  arg4   Optional 4th parameter to device driver
 * @param  arg5   Optional 5th parameter to device driver
 * @param  arg6   Optional 6th parameter to device driver
 * @param  arg7   Optional 7th parameter to device driver
 * @param  arg8   Optional 8th parameter to device driver
 *
 * @return integer value defined by the device driver itself
 * @return -1 return value should be reserved for errors
 */
// int qurt_qdi_handle_invoke();
#define qurt_qdi_handle_invoke(h,m,...) ({int _QDMTCAT(qurt_qdi_qhi,_QDMCNT(a,b,h,m,##__VA_ARGS__))_QDMLL((_QDMUA),_QDMT,h,m,##__VA_ARGS__);\
                                         _QDMTCAT(qurt_qdi_qhi,_QDMCNT(a,b,h,m,##__VA_ARGS__))_QDMLL((_QDMUA),_QDME,h,m,##__VA_ARGS__);})

/**
 * User-visible API to write to a QDI handle.
 *
 * qurt_qdi_write() is a predefined invocation routine for drivers which
 *  support a POSIX-like "write" functionality.
 * Note that qurt_qdi_write(handle, buf, len) is equivalent to:
 *  qurt_qdi_handle_invoke(handle, QDI_WRITE, handle, buf, len);
 *
 * @param  handle Integer handle to be written
 * @param  buf    Buffer from which bytes should be written
 * @param  len    Number of bytes to be written from buffer
 *
 * @return non-negative integer number of bytes which were written
 * @return negative error code if the write could not take place
 */
int qurt_qdi_write(int handle, const void *buf, unsigned len);

/**
 * User-visible API to read from a QDI handle.
 *
 * qurt_qdi_read() is a predefined invocation routine for drivers which
 *  support a POSIX-like "read" functionality.
 * Note that qurt_qdi_read(handle, buf, len) is equivalent to:
 *  qurt_qdi_handle_invoke(handle, QDI_READ, handle, buf, len);
 *
 * @param  handle Integer handle to be read
 * @param  buf    Buffer into which bytes should be read
 * @param  len    Number of bytes to be read into buffer
 *
 * @return non-negative integer number of bytes which were read
 * @return negative error code if the read could not take place
 */
int qurt_qdi_read(int handle, void *buf, unsigned len);

/**
 * User-visible API to close a QDI handle.
 *
 * qurt_qdi_close() should be called when the user is done using a
 *  QDI-based handle.  When this is called, the driver can release
 *  any resources held and perform other necessary cleanup
 *  operations.
 * Note that qurt_qdi_close(handle) is equivalent to:
 *  qurt_qdi_handle_invoke(handle, QDI_CLOSE, handle);
 *
 * @param  handle Integer handle to be released
 *
 * @return 0 if the handle was released with no problems
 * @return negative error code if the handle could not be released properly
 */
int qurt_qdi_close(int handle);

/**
 * Increment the QDI "local" count for the current thread.
 *
 * qurt_qdi_enter_local() is used to "notify" the QDI infrastructure
 *  that subsequent QDI operations should be treated as in-process
 *  local operations.  Pointers and handles will be assumed to be
 *  local to this process and no security checks will be performed.
 * The primary reason to call qurt_qdi_enter_local() would be to invoke
 *  QDI handles from within a QDI invocation function.  For instance,
 *  if an instance of driver Foo internally uses an instance of
 *  driver Bar for any purpose, it may use qurt_qdi_enter_local()
 *  before invoking a handle of driver Bar.  This will cause the
 *  driver Bar to treat its invocation as coming from a local
 *  trusted caller.
 * If the current state is not LOCAL, this call transitions
 *  to a LOCAL state.
 * Each call to qurt_qdi_enter_local() should be balanced by a
 *  subsequent call to qurt_qdi_leave_local().
 *
 * @return void
 */
void qurt_qdi_enter_local(void);

/**
 * Decrement the QDI "local" count for the current thread.
 *
 * qurt_qdi_leave_local() is used to undo the effects of a previous
 *  qurt_qdi_enter_local() call.
 * If the current local count is 1, this call transitions to
 *  either a TRUSTED state or an UNTRUSTED state, depending
 *  on the value of the trusted count.
 *
 * @return void
 */
void qurt_qdi_leave_local(void);

/**
 * Increment the QDI "trusted" count for the current thread.
 *
 * qurt_qdi_enter_trusted() is used to "notify" the QDI infrastructure
 *  that the current thread may relax its protection checking.
 * The primary reason to call qurt_qdi_enter_trusted() would be to
 *  increase performance if the current QDI invocation is coming
 *  from a "trusted" process.  Make sure that you understand the
 *  ramifications of setting QDI into a trusted mode; improper
 *  use of this call may allow untrusted code to elevate its
 *  privileges.
 * If the current state is UNTRUSTED, this call transitions to
 *  a TRUSTED state.
 * Each call to qurt_qdi_enter_trusted() should be balanced by a
 *  subsequent call to qurt_qdi_leave_trusted().
 *
 * @return void
 */
void qurt_qdi_enter_trusted(void);

/**
 * Decrement the QDI "trusted" count for the current thread.
 *
 * qurt_qdi_leave_trusted() is used to undo the effects of a previous
 *  qurt_qdi_enter_trusted() call.
 * If the current state is TRUSTED with a trusted count of 1,
 *  this call transitions to an UNTRUSTED state.
 *
 * @return void
 */
void qurt_qdi_leave_trusted(void);

/**
 * Allocate a QDI handle which references the provided QDI object.
 *
 * qurt_qdi_new_handle_from_obj() allocates a QDI handle which refers to
 *  a particular QDI object.  The primary use of this function is
 *  to handle QDI_OPEN processing, but other QDI methods are permitted
 *  to create handles and pass them to the user, as the device
 *  implementation might require.
 * When in a LOCAL state, this function allocates a handle from
 *  the local handle table.
 * When in a TRUSTED or UNTRUSTED state, this function allocates
 *  a handle from the local handle table and a handle from the
 *  remote handle table; the remote handle is initialized with a
 *  reference to the local handle.  This is the recommended way
 *  to generate remote handles which refer to local objects.
 * If there is a problem with allocating the handle, an error
 *  may be returned -- this generally means that one of the
 *  handle tables is full.
 *
 * @param  object Pointer to QDI object for which a handle is requested
 *
 * @return non-negative integer handle if successful
 * @return negative error code if the handle could not be allocated
 */
int qurt_qdi_new_handle_from_obj(void *object);

/**
 * Release a QDI handle, making the handle available for reuse.
 *
 * qurt_qdi_release_handle() releases a QDI handle, and should be
 *  called during QDI_CLOSE processing; there may also be other
 *  situations where a handle is released, but the implementation
 *  should be certain that no more invocations will be attempted
 *  using the handle which is being released.
 * When in a LOCAL state, this function releases a handle from
 *  the local handle table.
 * When in a TRUSTED or UNTRUSTED state, this function releases a
 *  handle from the remote handle table.  This generally results
 *  in releasing a local handle as well, and this is the only
 *  reliable way to release a remote handle which refers to
 *  a local handle.
 * If there is a problem with releasing the handle, an error
 *  may be returned -- this can happen if the handle has already
 *  been released.
 *
 * @param  handle Integer handle to be released
 *
 * @return 0 if the handle was released with no problems
 * @return negative error code if the handle could not be released properly
 */
int qurt_qdi_release_handle(int handle);

/**
 * Copy data from a possibly untrusted source buffer to a trusted
 *  destination buffer.
 *
 * qurt_qdi_copyin() takes arguments similar to memcpy() or memmove().
 * When in a LOCAL or TRUSTED state, it behaves like memcpy() or
 *  memmove() except for the return value, which is the integer 0.
 * When in an UNTRUSTED state, this function may fail to copy the
 *  entire buffer due to a security violation.  If it detects a
 *  security violation, the function returns a negative integer to
 *  indicate an error; it may perform a partial copy and still return
 *  an error (for instance, if the source buffer has 4K of
 *  readable memory, but an 8K copy was requested).
 *
 * @param  dst    Pointer to the trusted destination buffer
 * @param  src    Pointer to the possibly untrusted source buffer
 * @param  len    Number of bytes to be copied
 *
 * @return 0 if the buffer was entirely copied with no failure
 * @return negative error code if the buffer could not be copied
 */
int qurt_qdi_copyin(void *dst, const void *src, unsigned len);

/**
 * Copy data from a trusted source buffer to a possibly untrusted
 *  destination buffer.
 *
 * qurt_qdi_copyout() takes arguments similar to memcpy() or memmove().
 * When in a LOCAL or TRUSTED state, it behaves like memcpy() or
 *  memmove() except for the return value, which is the integer 0.
 * When in an UNTRUSTED state, this function may fail to copy the
 *  entire buffer due to a security violation.  If it detects a
 *  security violation, the function returns a negative integer to
 *  indicate an error; it may perform a partial copy and still return
 *  an error (for instance, if the destination buffer has 4K of
 *  writable memory, but an 8K copy was requested).
 *
 * @param  dst    Pointer to the possibly untrusted destination buffer
 * @param  src    Pointer to the trusted source buffer
 * @param  len    Number of bytes to be copied
 *
 * @return 0 if the buffer was entirely copied with no failure
 * @return negative error code if the buffer could not be copied
 */
int qurt_qdi_copyout(void *dst, const void *src, unsigned len);

/**
 * Prepare for direct manipulation of a possibly untrusted buffer
 *  provided by a QDI client.
 *
 * qurt_qdi_lock_buffer() is used to permit a trusted driver to safely
 *  access memory which is provided by a possibly untrusted client.
 * Prior to directly accessing a memory buffer provided by a client,
 *  a QDI driver should use qurt_qdi_lock_buffer() to obtain a safe buffer
 *  pointer for access.  This fulfills two requirements -- first, it
 *  checks that the entire buffer is accessible to the client; second,
 *  it ensures that the pointer remains valid for the remainder of the
 *  QDI driver operation.
 * The perms parameter is a bit mask integer requesting read or write
 *  access for the buffer in question:
 *    QDI_PERM_R to request read access
 *    QDI_PERM_W to request write access
 *    QDI_PERM_RW to request read/write access
 * When in a LOCAL or TRUSTED state, this function does no checking or
 *  locking, but just returns the user's pointer as *obuf and returns
 *  0 to indicate no error.
 * When in an UNTRUSTED state, this function may return an error, which
 *  indicates that the buffer cannot be safely accessed by the driver.
 *  Generally, this indicates that the user passed a buffer for which
 *  the requested access permission was not available.  If this function
 *  returns non-zero, generally the QDI invocation should be failed
 *  cleanly, with an appropriate error code returned to the client (this
 *  is analogous to the POSIX standard of returning EFAULT to indicate
 *  that accessing a memory buffer would cause a fault from user mode).
 *  If the function returns 0 (success), then the QDI driver should use
 *  the pointer at *obuf to perform its memory accesses rather than using
 *  the pointer passed in as buf -- even if the user process changes the
 *  mapping of memory at buf, the mapping of memory at *obuf should remain
 *  valid until the QDI invocation completes.
 */
int qurt_qdi_lock_buffer(void *buf, unsigned len, unsigned perms, void **obuf);

void *qurt_qdi_usermalloc(unsigned size);
void qurt_qdi_userfree(void *ptr);

/**
 * Register a QDI device with the generic QDI object in
 *  the current QDI context.
 *
 * A string name and an opener object must be provided.
 * If the string ends with a '/' (forward slash), it is
 *  treated as a name prefix to be matched.
 * If the string does not end with a '/', it is treated
 *  as an exact name to be matched.
 *
 * @param  name   Device name or device name prefix
 * @param  opener Pointer to "opener" object for the device
 *
 * @return 0 if device was successfully registered
 * @return negative error code if device was not registered
 */
int qurt_qdi_register_devname(const char *name, void *opener);

#endif
