#ifndef ECALL_DIAG_H_
#define ECALL_DIAG_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               E C A L L _ M S D 
                            H E A D E R   F I L E

GENERAL DESCRIPTION 
This file contains the API to originate an ECALL thru QXDM, 
using a dedicated command code for ECALL

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/inc/ecall_diag.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_APP

#include "cm.h"
#include "diagpkt.h"        /* for subsystem commands      */
#include "msg.h"
#include "ecall_app.h"

#if ( !(defined FEATURE_ECALL_LCU45) && (defined FEATURE_ECALL_HAS_V1_HEADER) )
#include "cmlog_v.h"
#endif /* FEATURE_ECALL_LCU45 */

/* Data defines */
#define CM_DIAG_MAX_ECALL_DIGITS 64
#define CM_DIAG_ECALL_CALL_ID_MAX  8
/*Moved from cmlog_v.h (private header file) to here */
#define CM_ECALL_CALL_ORIGINATION           22
#define CM_ECALL_CALL_END                   23

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/

typedef struct ecall_diag_test_config_struct {

  uint8 test_call;             /* 1 = Emergency call,        0 =  Test call */
  uint8 canned_msd;            /* 1 = Send Canned MSD,       0 = Send Real MSD */
  uint8 type_of_call;          /* 1 = Auto initiated ecall , 0 = Manual initiated ecall */

} ecall_diag_test_config_type;

/*---------------------------------------------------------------------------
    CM_ECALL_CALL_ORIGINATION -- SUBCOMMAND 23
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_ECALL_CALL_ORIGINATION)

 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* NEW */
 uint8 test_call; 
 uint8 canned_msd;
 uint8 type_of_call;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[CM_DIAG_MAX_ECALL_DIGITS];

DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE (CM, CM_ECALL_CALL_ORIGINATION)
  boolean status;
  byte    call_id;

DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_ECALL_CALL_END -- SUBCOMMAND 24
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_ECALL_CALL_END)
  byte         call_id_count;
  PACKED struct {
    uint8        call_id;
  } end_params[CM_DIAG_ECALL_CALL_ID_MAX];
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_ECALL_CALL_END)
  boolean  status;
DIAGPKT_RSP_END

/*===========================================================================
Function: ECALL_DIAG_CALL_END
Description:

       This function processes the call manager call end request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmlog_cm_call_end)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *ecall_diag_call_end 
(PACKED void* req_pkt, 
uint16 pkt_len
);

/*===========================================================================
Function: ecall_DIAG_CALL_ORIGINATION
Description:

       This function processes the ecall specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, ecall_diag_call_origination)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *ecall_diag_call_origination 
(PACKED void* req_pkt, 
uint16 pkt_len
);

#else

void ecall_diag_dummy_func(void);

#endif //End Featurisation

#endif /* ECALL_DIAG_H_ */
