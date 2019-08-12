#ifndef RR_L2_H
#define RR_L2_H

/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_l2.h#1 $    $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11-02-10   sjw     Enhanced support for G+G paging collision
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
10-03-30   tjw     Explicitly include dependent headers to make SU API files
                   stand alone
02/11/03   JAC     Added short header support
02/14/03   RG      Added support for G2W blind Handover
08/05/02   rmc     Changed suspend_reason to forced_suspend for dl_suspend_req_T
07/29/02   Rao     Added suspend reason in dl_suspend_req for InterRAT HO
05/30/02   rmc     Removed #define for N201_MAX already defined in sys_cnst.h
04/29/02   rmc     Changed DL_UNIT_DATA_IND to include TC value

===========================================================================*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "sys_cnst.h"
#include "sys_type.h"
#include "rr_l2_l1.h"
#include "geran_dual_sim.h"
#include "geran_api_rr_l2.h" /* Now provides FRAME_NUMBER_T */

/************************************/
/* define the N201 values           */
/* according to GSM 04.06 5.8.3     */
/************************************/

#define N201_SACCH      18
#define N201_DCCH       20
#define N201_SACCH_BTER 21 /* Short L2 header,Bter format (Measurement Information) */
#define N201_CCCH       23

/**********************************/
/* Service Access Point parameter */
/**********************************/

typedef uint8 SAPI_T;

#define SAPI0 0
#define SAPI3 3

/**********************************/
/* Establish Mode parameter       */
/**********************************/

typedef uint8 establish_mode_T;

#define CONTENTION_RESOLUTION 0
#define NORMAL_ESTABLISHMENT  1

/**********************************/
/* Release Mode parameter         */
/**********************************/

typedef uint8 release_mode_T;

#define NORMAL_RELEASE    0
#define LOCAL_END_RELEASE 1

/**********************************/
/* Error Cause parameter          */
/**********************************/

typedef uint8 error_code_T;

#define LAPDM_ERROR_SUCCESS 0 //NO_ERROR

#define T200_EXPIRY                         (LAPDM_ERROR_SUCCESS+1)
#define REESTABLISH_REQ                     (LAPDM_ERROR_SUCCESS+2)
#define UNSOLICITED_UA_RESPONSE             (LAPDM_ERROR_SUCCESS+3)
#define UNSOLICITED_DM_RESPONSE             (LAPDM_ERROR_SUCCESS+4)
#define UNSOLICITED_DM_RESPONSE_MULTIFRAME  (LAPDM_ERROR_SUCCESS+5)
#define UNSOLICITED_SUPERVISORY_RESPONSE    (LAPDM_ERROR_SUCCESS+6)
#define SEQUENCE_ERROR                      (LAPDM_ERROR_SUCCESS+7)
#define U_FRAME_INCORRECT_PARAMETERS        (LAPDM_ERROR_SUCCESS+8)
#define S_FRAME_INCORRECT_PARAMETERS        (LAPDM_ERROR_SUCCESS+9)
#define I_FRAME_INCORRECT_M_BIT             (LAPDM_ERROR_SUCCESS+10)
#define I_FRAME_INCORRECT_LENGTH            (LAPDM_ERROR_SUCCESS+11)
#define FRAME_NOT_IMPLEMENTED               (LAPDM_ERROR_SUCCESS+12)
#define SABM_COMMAND_MULTIFRAME_EST         (LAPDM_ERROR_SUCCESS+13)
#define SABM_COMMAND_INFO_FIELD_NOT_ALLOWED (LAPDM_ERROR_SUCCESS+14)
#define UNSOLICITED_DM_RESPONSE_PAR         (LAPDM_ERROR_SUCCESS+15)
#define HANDOVER_OUT_OF_RANGE               (LAPDM_ERROR_SUCCESS+16)
#define N200_EXCEEDED_PAR                   (LAPDM_ERROR_SUCCESS+17)
#define OTHER_ERROR                         (LAPDM_ERROR_SUCCESS+18)
#define CHANNEL_MODE_CHANGED                (LAPDM_ERROR_SUCCESS+19)

typedef enum
{
   DL_DATA_REQ,               /* 0x00 */
   DL_ESTABLISH_REQ,          /* 0x01 */
   DL_RANDOM_ACCESS_REQ,      /* 0x02 */
   DL_RECONNECT_REQ,          /* 0x03 */
   DL_RELEASE_REQ,            /* 0x04 */
   DL_RESUME_REQ,             /* 0x05 */
   DL_SUSPEND_REQ,            /* 0x06 */
   DL_UNIT_DATA_REQ,          /* 0x07 */
   MDL_RELEASE_REQ,           /* 0x08 */

   DL_DATA_IND,               /* 0x09 */
   DL_ESTABLISH_CNF,          /* 0x0a */
   DL_ESTABLISH_IND,          /* 0x0b */
   DL_RANDOM_ACCESS_CNF,      /* 0x0c */
   DL_RECONNECT_CNF,          /* 0x0d */
   DL_RELEASE_CNF,            /* 0x0e */
   DL_RELEASE_IND,            /* 0x0f */
   DL_RESUME_CNF,             /* 0x10 */
   DL_SUSPEND_CNF,            /* 0x11 */
   DL_UNIT_DATA_IND,          /* 0x12 */
   MDL_ERROR_IND,             /* 0x13 */
   DL_DATA_ACK_IND,           /* 0x14 */
   DL_DATA_MUX_IND            /* 0x15 */
}rr_l2_message_id_enum_T;


/**********************************************************/
/* DL_ESTABLISH_REQUEST primitive :                       */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request the establishment of multiple frame operations */
/* between two data link layer entities                   */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   establish_mode_T  establish_mode;
   uint8             l3_message_length;
   uint8             layer3_message[N201_MAX];
   gas_id_t          gas_id;
} dl_establish_req_T;

/**********************************************************/
/* DL_ESTABLISH_INDICATION primitive :                    */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Indication of (re)establishment of multiple frame      */
/* operations between two data link layer entities        */
/**********************************************************/

typedef struct
{
   IMH_T              message_header;
   SAPI_T             SAPI;
   l2_channel_type_T  l2_channel_type;
   boolean            reestablishment;
   uint8              num_discarded_msgs;
   gas_id_t           gas_id;
} dl_establish_ind_T;


/**********************************************************/
/* DL_ESTABLISH_CONFIRM primitive :                       */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Confirmation of the establishment of multiple frame    */
/* operations between two data link layer entities        */
/**********************************************************/

typedef struct
{
   IMH_T            message_header;
   SAPI_T           SAPI;
   establish_mode_T establish_mode;
   gas_id_t         gas_id;
} dl_establish_cnf_T;


/**********************************************************/
/* DL_RELEASE_REQUEST primitive :                         */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request of the termination of multiple frame           */
/* operations between two data link layer entities        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   release_mode_T    release_mode;
   gas_id_t          gas_id;
} dl_release_req_T;

/**********************************************************/
/* DL_RELEASE_INDICATION primitive :                      */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Indication of the termination of multiple frame        */
/* operations between two data link layer entities        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   release_mode_T    release_mode;
   gas_id_t          gas_id;
} dl_release_ind_T;

/**********************************************************/
/* DL_RELEASE_CONFIRMATION primitive :                    */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Confirmation of the termination of multiple frame      */
/* operations between two data link layer entities        */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   release_mode_T    release_mode;
   gas_id_t          gas_id;
} dl_release_cnf_T;


/**********************************************************/
/* DL_SUSPEND_REQUEST primitive :                         */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request the suspension of the multiple frame           */
/* operation while changing a dedicated channel           */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   boolean           forced_suspend;
  gas_id_t           gas_id;
} dl_suspend_req_T;

/**********************************************************/
/* DL_SUSPEND_CONFIRM primitive :                         */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Confirm the suspension of the multiple frame           */
/* operation while changing a dedicated channel           */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   gas_id_t          gas_id;
} dl_suspend_cnf_T;


/**********************************************************/
/* DL_RESUME_REQUEST primitive :                          */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request the resumption of the multiple frame           */
/* operation                                              */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   boolean           forced_resume;
   uint8             l3_message_length;
   uint8             layer3_message[N201_MAX];
   boolean           l1_already_connected;
   gas_id_t          gas_id;
} dl_resume_req_T;


/**********************************************************/
/* DL_RESUME_CONFIRM primitive :                          */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Confirm the resumption of the multiple frame           */
/* operation                                              */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   gas_id_t          gas_id;
} dl_resume_cnf_T;


/**********************************************************/
/* DL_RECONNECT_REQUEST primitive :                       */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request the restoration of the multiple frame          */
/* operation on the old channel after failure             */
/* of the channel change                                  */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8             l3_message_length;
   uint8             layer3_message[N201_MAX];
   gas_id_t          gas_id;
} dl_reconnect_req_T;

/**********************************************************/
/* DL_RECONNECT_CONFIRM primitive :                       */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Confirm the restoration of the multiple frame          */
/* operation on the old channel after failure             */
/* of the channel change                                  */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   gas_id_t          gas_id;
} dl_reconnect_cnf_T;

/**********************************************************/
/* DL_DATA_REQUEST primitive :                            */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request that a message unit be sent using the          */
/* procedures for multiple acknowledge of information     */
/* transfer                                               */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8             l3_message_length;
   uint8             l3_message_number;
   uint8             layer3_message[MAX_OCTETS_L3_MESSAGE];
   gas_id_t          gas_id;
} dl_data_req_T;

/**********************************************************/
/* DL_DATA_ACK_IND primitive :                            */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Informs RR that a particular msg has been sent and     */
/* acknowledged by the network                            */
/* RR should set the l3_message_number to 0 in            */
/* dl_data_req if the msg is an RR msg                    */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   uint8             l3_message_number;
   gas_id_t          gas_id;
} dl_data_ack_ind_T;

/**********************************************************/
/* DL_DATA_INDICATION primitive :                         */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Indicates the arrival of a message unit received       */
/* by means of acknowledge information tranfer            */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8             l3_message_length;
   uint8             layer3_message[MAX_OCTETS_L3_MESSAGE];
   gas_id_t          gas_id;
} dl_data_ind_T;

/**********************************************************/
/* DL_UNIT_DATA_REQUEST primitive :                       */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request that a message unit be sent using the          */
/* procedures for multiple unacknowledge information      */
/* transfer service                                       */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8             l3_message_length;
   boolean           short_header;
   uint8             layer3_message[N201_MAX];
   gas_id_t          gas_id;
} dl_unit_data_req_T;

/**********************************************************/
/* DL_UNIT_DATA_INDICATION primitive :                    */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Indicates the arrival of a message unit received       */
/* by means of unaknowledge information tranfer service   */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   boolean           own_paging_block;
   uint8             tc; /* only applicable for SI messages */
   ARFCN_T           channel_number;
   uint8             l3_message_length;
   boolean           short_header;
   uint8             layer3_message[N201_MAX];
   gas_id_t          gas_id;
   boolean           shared_msg;
} dl_unit_data_ind_T;


/**********************************************************/
/* DL_RANDOM_ACCESS_REQUEST primitive :                   */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Request the transmission of a random access burst      */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8             random_access_content;
   boolean           egprs_ra_content_present;
   uint16            egprs_ra_content;
   uint8             egprs_training_seq;
   boolean           pwr_reduction_valid;
   uint8             pwr_reduction_value;
   boolean           emergency_call;
   boolean           rach_for_ps_traffic;
   gas_id_t          gas_id;
} dl_random_access_req_T;

/**********************************************************/
/* DL_RANDOM_ACCESS_CONFIRM primitive :                   */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Notify layer 3 that a random access burst has been     */
/* sent                                                   */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   FRAME_NUMBER_T    frame_number;
   l2_channel_type_T l2_channel_type;
   uint16            random_access_content;
   geran_result_t    result;
   gas_id_t          gas_id;
} dl_random_access_cnf_T;

/**********************************************************/
/* MDL_RELEASE_REQUEST primitive :                        */
/* FROM :  RR                                             */
/* TO   :  LAYER 2                                        */
/* DESCRIPTION :                                          */
/* Initiate the abnormal local end release of a data link */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   gas_id_t          gas_id;
} mdl_release_req_T;

/**********************************************************/
/* MDL_ERROR_INDICATION primitive :                       */
/* FROM :  LAYER 2                                        */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Indicate that there is an error in the data link layer */
/* procedures that can not be resolved by normal          */
/* exception handling procedures                          */
/**********************************************************/

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   error_code_T      error_code;
   gas_id_t          gas_id;
} mdl_error_ind_T;


#endif



