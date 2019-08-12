#ifndef DSERRNO_H
#define DSERRNO_H
/*===========================================================================

              D A T A   S E R V I C E S   E R R O R N O ' S

DESCRIPTION

 The Data Services Errno header file.  Contains #defines for the various
 errors we can see in the system.

Copyright (c) 1998-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dserrno.h_v   1.2   13 Feb 2003 12:05:14   ssinghai  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dserrno.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/10   aa      Added error codes for BCMCSDB errors.
01/23/06   mct     Added new errno for QOS unaware system.
08/12/05   sv      Added new errno for going dormant.
06/13/05   vp      Added error codes for ICMP errors.
02/13/03   ss      Added DS_ESHUTDOWN.
08/30/02   om      Added DS_ENOROUTE.
06/25/02   ak      First revision.
===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
                    Transport Error Condition Values
---------------------------------------------------------------------------*/
#define DS_EEOF               0                             /* end of file */
#define DS_EINTR             99  /* Reserve for error in BSD clbk function */
#define DS_EBADF            100               /* Invalid socket descriptor */
#define DS_EFAULT           101              /* Invalid buffer or argument */
#define DS_EWOULDBLOCK      102                   /* Operation would block */
#define DS_EAFNOSUPPORT     103            /* Address family not supported */
#define DS_EPROTOTYPE       104          /* Wrong protocol for socket type */
#define DS_ESOCKNOSUPPORT   105          /* Socket parameter not supported */
#define DS_EPROTONOSUPPORT  106                  /* Protocol not supported */
#define DS_EMFILE           107   /* No more sockets available for opening */
#define DS_EOPNOTSUPP       108                 /* Operation not supported */
#define DS_EADDRINUSE       109                  /* Address already in use */
#define DS_EADDRREQ         110            /* Destination address required */
#define DS_EINPROGRESS      111    /* Connection establishment in progress */
#define DS_EISCONN          112          /* Connection already established */
#define DS_EIPADDRCHANGED   113   /* IP address changed, causing TCP reset */
#define DS_ENOTCONN         114                    /* socket not connected */
#define DS_ECONNREFUSED     115              /* Connection attempt refused */
#define DS_ETIMEDOUT        116            /* Connection attempt timed out */
#define DS_ECONNRESET       117                        /* Connection reset */
#define DS_ECONNABORTED     118                      /* Connection aborted */
#define DS_EPIPE            119                             /* Broken pipe */
#define DS_ENETDOWN         120           /* Network subsystem unavailable */
#define DS_EMAPP            121          /* no more applications available */
#define DS_EBADAPP          122                  /* Invalid application ID */
#define DS_SOCKEXIST        123              /* there are existing sockets */
#define DS_EINVAL           124                       /* invalid operation */

/*---------------------------------------------------------------------------
                          DNS Lookup Error Messages
---------------------------------------------------------------------------*/
#define DS_NAMEERR        125                       /* Domain Name Error */
#define DS_NAMEMISS       126                   /* Domain Name not found */
#define DS_NETERR         127                   /* Network is not opened */
#define DS_NOMEMORY       128                           /* Out of memory */
#define DS_INUSE          129            /* The resolver is in used now. */

#define DS_ENOMEM         DS_NOMEMORY                   /* Out of memory */

/*---------------------------------------------------------------------------
                        dss_sendto() Error Conditions
---------------------------------------------------------------------------*/
#define DS_EMSGSIZE       130           /* The message is too large.       */

/*---------------------------------------------------------------------------
                            Socket Option Errors
---------------------------------------------------------------------------*/
#define DS_ENOPROTOOPT 131 /* The option is unknown at the level indicated */

/*---------------------------------------------------------------------------
                        More DNS Lookup Error Messages
---------------------------------------------------------------------------*/
#define DS_EHOSTNOTFOUND  DS_NAMEERR                     /* Host not found */
#define DS_ETRYAGAIN      DS_INUSE                      /* Temporary error */
#define DS_ENORECOVERY    132                       /* Irrecoverable error */
#define DS_ENOADDRESS     133            /* No address for the domain name */

/*---------------------------------------------------------------------------
                    More Transport Error Condition Values
---------------------------------------------------------------------------*/

#define DS_EMSGTRUNC      134  /* msg truncated, supplied buffer too small */

/*---------------------------------------------------------------------------
                      IP Routing Error Condition Values
---------------------------------------------------------------------------*/

#define DS_ENOROUTE       135  /* No route to destination found */

/*---------------------------------------------------------------------------
                       Shutdown Error Condition Values
---------------------------------------------------------------------------*/
#define DS_ESHUTDOWN      136 /* can't send after shutting down write-half */

/*---------------------------------------------------------------------------
                            Extra ICMP(v6) errors.
---------------------------------------------------------------------------*/
#define DS_ENETUNREACH    137                 /* Network is unreachable    */
#define DS_EHOSTUNREACH   138                 /* No route to host          */
#define DS_EHOSTDOWN      139                 /* Host is down              */
#define DS_ENONET         140                 /* Machine is not on network */
#define DS_EPROTO         141                 /* Protocol error            */
#define DS_EACCES         142                 /* Permission denied         */
/*---------------------------------------------------------------------------
                                  QOS Errors
---------------------------------------------------------------------------*/
#define DS_EQOSUNAWARE    150         /* Mobile is in a QOS Unaware System */

/*---------------------------------------------------------------------------
                                  IPv6 Errors
---------------------------------------------------------------------------*/
#define DS_ENOSRCADDR       175       /* No valid src addr could be found  */
#define DS_EPREFIXEXHAUSTED 176       /* Max Prefix generated from 
                                         delegated prefix*/

/*---------------------------------------------------------------------------
                          Network Subsystem Errors
---------------------------------------------------------------------------*/
#define DS_ENETISCONN           200 /* subsystem established and available */
#define DS_ENETINPROGRESS       201 /* subsystem establishment in progress */
#define DS_ENETNONET            202       /* network subsystem unavailable */
#define DS_ENETCLOSEINPROGRESS  203                      /* PPP is closing */
#define DS_ENETEXIST            204    /* existing net subsystem resources */
#define DS_ENETGOINGDORMANT     205              /* Physlink going dormant */

/*---------------------------------------------------------------------------
                          More DNS subsytem Errors
---------------------------------------------------------------------------*/
#define DS_ETRUNCATED           206  /* Response is truncated */
#define DS_ESYSTEM              207  /* System error occurred */
#define DS_EMAXQUERIES          208  /* Reached max number of queries supported */

/*---------------------------------------------------------------------------
                              BCMCSDB Errors
---------------------------------------------------------------------------*/
#define DS_EDBOVERFLOW           220 /* The DB is full, no further updates  */
#define DS_EDBNOOVERWRITE        221 /* Value exists and overwrite is false */

/*---------------------------------------------------------------------------
                              Handoff Errors
---------------------------------------------------------------------------*/
#define DS_ESAMESRATTRAT         250 /* Srat and Trat are same */

/*---------------------------------------------------------------------------
  Using 0x7FFF to indicate no error. Since DS_EEOF has a value of 0,
  can't set ps_errno to 0 to indicate that there is no error. It is required
  to indicate no error as even when dss_read() returns 0, app is still
  required to look at ps_errno to figure out if EOF has reached
---------------------------------------------------------------------------*/
#define DS_ENOERR               0x7FFF

#endif /* DSERRNO_H */
