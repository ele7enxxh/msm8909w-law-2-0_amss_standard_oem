#ifndef ERRNO_CODES_H
#define ERRNO_CODES_H
/*===========================================================================

               E R R O R   N U M B E R   D E F I N I T I O N S

DESCRIPTION
  This file contains the definition of the return codes (error numbers).
 

Copyright (c) 2000-2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/errno_codes.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/03    gr     Updated with more codes.
04/24/02    gr     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/

/*---------------------------------------------------------------------------
  #undef the macros defined here to prevent a redefinition warning in case
  of collision with definitions in system header files.
---------------------------------------------------------------------------*/
#undef EEOF
#undef EINTR
#undef EBADF
#undef EFAULT
#undef EWOULDBLOCK
#undef EAGAIN
#undef EAFNOSUPPORT
#undef EPROTOTYPE
#undef ESOCKNOSUPPORT
#undef EPROTONOSUPPORT
#undef EMFILE
#undef EOPNOTSUPP
#undef EADDRINUSE
#undef EDESTADDRREQ
#undef EINPROGRESS
#undef EISCONN
#undef EIPADDRCHANGED
#undef ENOTCONN
#undef ECONNREFUSED
#undef ETIMEDOUT
#undef ECONNRESET
#undef ECONNABORTED
#undef EPIPE
#undef ENETDOWN
#undef EMAPP
#undef EBADAPP
#undef SOCKEXIST
#undef EINVAL
#undef ENOMEMORY
#undef ENOMEM
#undef ENOENT
#undef ESRCH
#undef EHOSTNOTFOUND
#undef ETRYAGAIN
#undef ENORECOVERY
#undef ENOADDRESS
#undef EMSGTRUNC
#undef ENOROUTE
#undef ESHUTDOWN
#undef E2BIG
#undef ENOSYS
#undef EMSGSIZE
#undef EALREADY
#undef EDEADLK
#undef ENOLINK
#undef EIDRM
#undef EEXIST
#undef EUSERS
#undef EOVERFLOW
#undef EISNAM
#undef ELOOP
#undef EISDIR
#undef ENODEV
#undef ENOTEMPTY
#undef ESPIPE
#undef ENOPROTOOPT
#undef ENETISCONN
#undef ENETINPROGRESS
#undef ENONET
#undef ENETCLOSEINPROGRESS
#undef ENETEXIST
#undef ENETUNREACH
#undef ENOTDIR
#undef ENODATA
#undef EFBIG
#undef EADDRNOTAVAIL
#undef EROFS
#undef EACCES
#undef ENAMETOOLONG
#undef ENOSPC
#undef EHOSTUNREACH
#undef ENOBUFS
#undef ENFILE
#undef ETIME
#undef EBADFD
#undef EMLINK
#undef ENOTSOCK
#undef EBUSY
#undef ENETRESET
#undef ECOMM
#undef EDQUOT
#undef EPROTO
#undef EHOSTDOWN
#undef EPERM
#undef EIO
#undef ENXIO
#undef EDEADLOCK
#undef ENONET

/* This set of errno codes have values that match the values in ds_errno.h.
*/
#define EEOF                   0      /* End of file                    */
#define EINTR                 99      /* Interruption during system
                                         call produced fault            */
#define EBADF                100      /* Invalid socket descriptor      */
#define EFAULT               101      /* Invalid buffer or argument     */
#define EWOULDBLOCK          102      /* Operation would block          */
#define EAGAIN               EWOULDBLOCK
#define EAFNOSUPPORT         103      /* Address family not supported   */
#define EPROTOTYPE           104      /* Wrong protocol for socket
                                         type                           */
#define ESOCKNOSUPPORT       105      /* Socket parm not supported      */
#define EPROTONOSUPPORT      106      /* Protocol not supported         */
#define EMFILE               107      /* No more sockets available for
                                          opening                       */
#define EOPNOTSUPP           108      /* Operation not supported        */
#define EADDRINUSE           109      /* Address already in use         */
#define EDESTADDRREQ         110      /* Destination address required   */
#define EINPROGRESS          111      /* Connect in progress            */
#define EISCONN              112      /* Connection already established */
#define EIPADDRCHANGED       113      /* IP addr changed, causing TCP reset */
#define ENOTCONN             114      /* socket not connected           */
#define ECONNREFUSED         115      /* Connection attempt refused     */
#define ETIMEDOUT            116      /* Connection attempt timed out   */
#define ECONNRESET           117      /* Connection reset               */
#define ECONNABORTED         118      /* Connection aborted             */
#define EPIPE                119      /* Broken pipe                    */
#define ENETDOWN             120      /* Network unavailable            */
#define EMAPP                121      /* No more applications available */
#define EBADAPP              122      /* Invalid application ID         */
#define SOCKEXIST            123      /* There are existing sockets     */
#define EINVAL               124      /* Invalid operation              */

/*---------------------------------------------------------------------------
                          DNS Lookup Error Messages
---------------------------------------------------------------------------*/
#define ENAMEERR             125      /* Domain Name Error              */
#define ENAMEMISS            126      /* Domain Name not found          */
#define ENETERR              127      /* Network is not opened          */
#define ENOMEMORY            128      /* Out of memory                  */
#define EINUSE               129      /* The resolver is in used now.   */
#define ENOMEM         ENOMEMORY      /* Out of memory                  */

/*---------------------------------------------------------------------------
                        dss_sendto() Error Conditions
---------------------------------------------------------------------------*/
#define EMSGSIZE             130     /* The message is too large.       */

/*---------------------------------------------------------------------------
                            Socket Option Errors
---------------------------------------------------------------------------*/
#define ENOPROTOOPT          131  /* Option unknown at level indicated  */

/*---------------------------------------------------------------------------
                        More DNS Lookup Error Messages
---------------------------------------------------------------------------*/
#define EHOSTNOTFOUND        ENAMEERR /* Host not found                 */
#define ETRYAGAIN            EINUSE   /* Temporary error                */
#define ENORECOVERY          132      /* Irrecoverable error            */
#define ENOADDRESS           133      /* No address for domain name     */

/*---------------------------------------------------------------------------
                    More Transport Error Condition Values
---------------------------------------------------------------------------*/
#define EMSGTRUNC            134  /* msg truncated, supplied buffer too
                                     small                              */

/*---------------------------------------------------------------------------
                      IP Routing Error Condition Values
---------------------------------------------------------------------------*/
#define ENOROUTE             135  /* No route to destination found      */

/*---------------------------------------------------------------------------
                       Shutdown Error Condition Values
---------------------------------------------------------------------------*/
#define ESHUTDOWN            136      /* Cannot send after transport
                                         endpoint shutdown              */

/*---------------------------------------------------------------------------
                           Network Subsystem Errors
---------------------------------------------------------------------------*/
#define ENETISCONN           200 /* Subsystem established and available */
#define ENETINPROGRESS       201 /* Subsystem establishment in progress */
#define ENONET               202       /* Network unavailable           */
#define ENETCLOSEINPROGRESS  203                      /* PPP is closing */
#define ENETEXIST            204    /* Existing net subsystem resources */


/* These errno codes do not exist in ds_errno.h.
*/
#define ENOENT               800      /* No such file or directory      */
#define E2BIG                801      /* Arg list too long              */
#define ENOSYS               802      /* Function not implemented       */
#define EALREADY             803      /* Operation already in progress  */
#define EDEADLK              804      /* Resource deadlock would occur  */
#define ENOLINK              805      /* Link has been severed          */
#define EEXIST               806      /* File exists                    */
#define EUSERS               807      /* Too many users                 */
#define EOVERFLOW            808      /* Value too large for defined
                                         data type                      */
#define EISNAM               809      /* Is a named type file           */
#define ELOOP                810      /* Too many symbolic links
                                         encountered                    */
#define EISDIR               811      /* Is a directory                 */
#define ENODEV               812      /* No such device                 */
#define ENOTEMPTY            813      /* Directory not empty            */
#define ESPIPE               814      /* Illegal seek                   */
#define ENETUNREACH          815      /* Network is unreachable         */
#define ENOTDIR              816      /* Not a directory                */
#define ENODATA              817      /* No data available              */
#define EFBIG                818      /* File too large                 */
#define EADDRNOTAVAIL        819      /* Cannot assign requested addr   */
#define EROFS                820      /* Read-only file system          */
#define EACCES               821      /* Permission denied              */
#define ENAMETOOLONG         822      /* File name too long             */
#define ENOSPC               823      /* No space left on device        */
#define EHOSTUNREACH         824      /* No route to host               */
#define ENOBUFS              825      /* No buffer space available      */
#define ENFILE               826      /* File table overflow            */
#define ETIME                827      /* Timer expired                  */
#define EBADFD               828      /* File descriptor in bad state   */
#define EMLINK               829      /* Too many links                 */
#define ENOTSOCK             830      /* Socket operation on non-socket */
#define EBUSY                831      /* Device or resource busy        */
#define ENETRESET            832      /* Network dropped connection
                                         because of reset               */
#define ECOMM                833      /* Communication error on send    */
#define EDQUOT               834      /* Quota exceeded                 */
#define EPROTO               835      /* Protocol error                 */
#define EHOSTDOWN            836      /* Host is down                   */
#define EPERM                837      /* Operation not permitted        */
#define EIO                  838      /* I/O error                      */
#define ENXIO                839      /* No such device or address      */
#define EDEADLOCK            EDEADLK

#endif /* ERRNO_CODES_H */

