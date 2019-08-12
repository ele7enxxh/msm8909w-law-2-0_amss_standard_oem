#ifndef PS_PPP_CONFIG_H
#define PS_PPP_CONFIG_H
/*===========================================================================

                          P S _ P P P _ C O N F I G . H

DESCRIPTION

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/MM_DATA/ps_ppp_ipcp.h_v 1.3 12 Feb 2003 14:30:58   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_ppp_config.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/07/09    pp     Initial file. Created as part of CMI Phase-4: SU Level 
                   API Effort.

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
//LCP
#define LCP_REQ_TRY    20     /* REQ attempts                              */
#define LCP_NAK_TRY     3     /* NAK attempts                              */
#define LCP_TERM_TRY  2     /* pseudo-tries on TERM REQ - code actually 
                               only retries once, but timeout is this value
                               times the standard c-req timeout to special 
                               case the fsm_timeout code .                 */
#define LCP_TERM_MULT 3     /* multiplier for LCP TERM-REQ retry timeout   */
#define LCP_TIMEOUT  1000     /* milliseconds to wait for response         */

#define LCP_TERM_TO  500    /* timeout for sending term requests          */

#include "datamodem_variation.h"
#ifdef FEATURE_JCDMA_1X
#undef LCP_TIMEOUT
#undef LCP_REQ_TRY

#define LCP_TIMEOUT   3000   /* milliseconds to wait for response          */
#define LCP_REQ_TRY     10   /* the number of retries to attempt           */
#endif /* FEATURE_JCDMA_1X */

//IPCP
#define IPCP_REQ_TRY    20    /* REQ attempts                              */
#define IPCP_NAK_TRY     3    /* NAK attempts                              */
#define IPCP_TERM_TRY    3    /* tries on TERM REQ                         */
#define IPCP_TIMEOUT  1000    /* milliseconds to wait for response         */

#ifdef FEATURE_JCDMA_1X
#undef  IPCP_REQ_TRY
#undef  IPCP_TIMEOUT

#define IPCP_REQ_TRY    10    /* REQ attempts                              */
#define IPCP_TIMEOUT  3000    /* milliseconds to wait for response         */
#endif /* FEATURE_JCDMA_1X */

//IPV6CP
#define IPV6CP_REQ_TRY    20    /* REQ attempts                              */
#define IPV6CP_NAK_TRY     3    /* NAK attempts                              */
#define IPV6CP_TERM_TRY    3    /* tries on TERM REQ                         */
#define IPV6CP_TIMEOUT  1000    /* milliseconds to wait for response         */

//AUTH
/*---------------------------------------------------------------------------
  The time-out value for the CHAP. This will be given a value of 3 seconds.
  The value should be given in milliseconds.
---------------------------------------------------------------------------*/
#define PPP_AUTH_TIMER_VAL 3000

/*---------------------------------------------------------------------------
  Constant to load the initial value of the Retry counter. If mobile does
  not receive a reply from the PDSN, then it will retry this number of times
  before giving up
---------------------------------------------------------------------------*/
#define PPP_AUTH_RETRY_COUNTER_VAL 5

#ifdef FEATURE_JCDMA_1X

#undef PPP_AUTH_RETRY_COUNTER_VAL

#define PPP_AUTH_RETRY_COUNTER_VAL 9
#endif /* FEATURE_JCDMA_1X */

#endif /* PS_PPP_CONFIG_H */
