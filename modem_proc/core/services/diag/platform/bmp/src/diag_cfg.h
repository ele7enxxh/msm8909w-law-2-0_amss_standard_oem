#ifndef DIAG_CFG_H
#define DIAG_CFG_H
/*===========================================================================

                                  D I A G _ C F G . H

  GENERAL DESCRIPTION
  Contains configuration information for diag per platform

Copyright (c) 2009 - 2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/platform/bmp/src/diag_cfg.h#1 $

when        who    what, where, why
--------    ---   -------------------------------------------------------------
07/14/15    sa    Added support for time sync feature.
07/17/14    xy    Utilized optimized time get API  
06/05/14    rh    Support for 8kB commands and responses
04/29/13    ph    Added a limit on active listeners.
01/16/13    rh    Changed diagbuf NRT drain parameters 
01/03/13    sg    Command Req/Response Channel separation
05/10/12    rh    Added separate heap for Diag responses. 
10/26/11    hm    Extended Event/Log Listener APIs added  
02/28/11    sg    Power saving changes
02/28/11    vs    Increased event heap size 
12/10/10    vs    Reverted 8 byte alignement for q6
11/18/10    vs    Moved definition of DIAGBUF_NEXT_ALIGNED_BYTE here
11/10/10    mad   Increased event heap size to 8K
10/27/10    SG    Moved DIAG_TX_SLEEP_THRESHOLD_DEFAULT,DIAG_TX_SLEEP_TIME_DEFAULT
                  to here from diagbuf_v.h
06/07/10    VG    Updated DIAGBUF_SIZE to be able to configure it in custdiag.h
03/10/10    JV    Reduced size of diag heap back to 8K. Use system heap for
                  stress test.
03/03/10    as    Increased Diag heap to accomadate Ondevice mask processing
02/16/10    vs    Changed diagbuf size to 128K for 9k q6
01/18/10    JV    Reduce the size of diagbuf to 32K
07/17/09    JV    Created for BMP
===========================================================================*/
#include "diag_v.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" /* For stubbed ts_get(); for Q6 modem */
#else
#include "time_svc.h" /* for time_get(); for Q6 apps and all other targets */
#endif


#define DIAGBUF_NEXT_ALIGNED_BYTE(p) (((p) + 3) & ~3)


/* Not adding the DIAG_MAX_TX_PKT_SIZ and DIAG_MAX_RX_PKT_SIZ here because 
they need to be exposed to clients. Currently defined in diagpkt.h */

/* DIAGBUF_SIZE is the size, in bytes, of diag's output buffer.  If this buffer
 * fills up, logs and messages will be dropped.  It is recommended to make this
 * number a power of 2 to allow the compiler to optimize the arithmetic using
 * this number to use shift operations instead of brute force math.
 *
 * Note: This must be larger than DIAG_MAX_TX_PKT_SIZ. */

#ifndef DIAGBUF_SIZE
#define DIAGBUF_SIZE (1024*32*4)
#endif

#ifndef DIAG_TX_SLEEP_THRESHOLD_DEFAULT
#define DIAG_TX_SLEEP_THRESHOLD_DEFAULT (1024*16)
#endif

#ifndef DIAG_TX_SLEEP_TIME_DEFAULT
#define DIAG_TX_SLEEP_TIME_DEFAULT 10
#endif

#define DIAGBUF_COMMIT_THRESHOLD (1024*8)	               /* # of bytes committed to Diag buffer before draining. */

/* The size of the DIAG heap.  Currently, it is only used for response 
   packets and allocating tasks for stress testing.  DIAGBUF_SIZE only 
   impacts MSG and LOG packets. */
#ifndef DIAG_HEAP_SIZE   
#define DIAG_HEAP_SIZE (1024 * 8)
#endif
#define DIAG_RSP_HEAP_SIZE (1024 * 16)
/* These are the non real time values that are used when diag is in power savings mode*/
#define DIAGBUF_COMMIT_THRESHOLD_NRT (1024*64)

#define DIAG_TX_SLEEP_THRESHOLD_NRT  (1024*32)

#define DIAG_TX_SLEEP_TIME_NRT       5 

#define EVENT_RPT_PKT_SIZE_NRT (1000)

#define DIAG_EVENT_HEAP_SIZE (1024 * 16)
#define DIAG_EVENT_SEC_HEAP_SIZE (1024 * 4)
#define DIAG_LISTENERS_HEAP_SIZE (1024 * 8)

#define DIAG_MAX_ACTIVE_LISTENERS ( 18 )     				/*Arbitrary limit on active listeners to avoid exhaustion of system heap */

#define DIAG_HDLC_PAD_LEN (150)                            /* # of bytes padding to account for hdlc/crc bytes when filling DSM item */

/* Configurations for Diag Fwd task */
#ifndef DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT
  #define DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT (10)         /* X number of DSMs to drain before suspending */
#endif

#ifndef DIAG_FWD_TX_SLEEP_TIME_DEFAULT
  #define DIAG_FWD_TX_SLEEP_TIME_DEFAULT (10)              /* # of ms to sleep after draining X number of DSMs */
#endif

#endif /* DIAG_CFG_H */


