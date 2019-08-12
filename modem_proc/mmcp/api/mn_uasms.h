#ifndef mn_uasms_h
#define mn_uasms_h

/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

  EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_uasms.h_v   1.5   17 Jun 2002 17:43:36   rgurumoo  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mn_uasms.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
7/12/01     CD     Initial version.  Transferred mn_uasms data from mn.h
08/16/01    AB     Renamed MMI references to CM.

9/27/01     CD     Deleted tp_message_reference from SM_RL_DATA_REQ_T
                   Deleted tp_smi, and sm_rl_success from SM_RL_REPORT_IND_T
                   Changed error_type to error_status in SM_RL_REPORT_IND_T
                   Changed type of RP_OA in SM_RL_DATA_IND_T to cm_called_party_bcd_no_T

                   Added definitions for error_status in SM_RL_REPORT_IND_T

9/28/01     CD     Added definition for RP_USER_DATA_LENGTH, changed TPDU dimension
                   in SM_RL_REPORT_IND_T from TPDU_SIZE to RP_USER_DATA_LENGTH

                   Changed TPDU_SIZE to 164

1/29/02     RG     Changed the short_message_identifier in the SM_RL_DATA_REQ message 
                   to be a 'word' instead of a 'byte'.  Added "word short_message_identifier" 
                   to the SM_RL_MEMORY_AVAILABILITY_REQ_T structure.  Changed the 
                   short_message_identifier in the SM_RL_REPORT_IND message to be a 
                   'word' instead of a 'byte'.  The above changes were done to give the 
                   UASMS the ability to correlate the memory availability requests 
                   with the corresponding reponses.

6/17/02     RG     Defined the base value ( MN_WMS_MT_MESSAGE_IDENTIFIER_BASE )
                   that is added to the SMS connection id and sent in 
                   SM_RL_REPORT_IND.short_message_identifier from MN to WMS during
                   MN layer and lower layer failures in an MT SMS.

16/02/06    NR     Adding new reject cause for handling CNM MO SMS timer expiry

04/09/07    RD     SMS optimization changes are incorporated
===========================================================================*/

/*****************************************************
 * Constants used in SM-RL primitive data structures *
 *****************************************************/

#define  TPDU_SIZE                   164   /* See 32.040 sec       */
#define  RP_USER_DATA_TPDU_LENGTH    232   /* See 24.011 sec 8.2.5.3 */

/** CNM uses following lower layer cause when UE is in WAIT_FOR_CP_ACK state
      for MT SMS and radio link fails */
#define NO_RESPONSE_FROM_NEWTWORK_RLF      128

/** CNM uses following lower layer cause when UE is in WAIT_FOR_CP_ACK state
      for MT SMS and TC1M_MT timer expires twice */
#define NO_RESPONSE_FROM_NEWTWORK_TC1M_MT      129


/*
* Error status used in SM_RL_REPORT_IND_T
*/

enum
{
    NO_ERROR_RP_ACK,
    NO_ERROR_ABORT_SM_COMPLETE,
    NO_ERROR_SMMA_ABORT_SENT,
    SMR_ERROR_NO_SM_RESOURCES,
    SMR_ERROR_TR1M_TIMEOUT,
    SMR_ERROR_TR2M_TIMEOUT,
    SMS_RP_ERROR,
    SMS_CP_ERROR,
    SMS_LL_ERROR,
    SMS_NO_RESPONSE_FROM_NETWORK,
    NO_ERROR_CP_ACK,
    SMS_RPT_RADIO_UNAVAILABLE,
    SMS_RADIO_LINK_ENDED_FOR_SUBMIT_REPORT, // This is used to fill error_status field of SM_RL_REPORT_IND_T in case of RLF when UE waits for final CP_ACK from NW in MT-SMS.
    SMS_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT // This is used to fill error_status field of SM_RL_REPORT_IND_T in case TC1M_MT timer expires for the second time.
};


/***************
 * Message IDs *
 ***************/
enum
{
   /* UASMS -> MN-SMS messages */

   UASMS_SMS_REQ                   = 0x30,    /* message group */
   UASMS_SM_DATA_REQ,
   UASMS_MEMORY_AVAIL_NOTIFICATION_REQ,
   UASMS_ABORT_SM_REQ,
   UASMS_SM_REPORT_REQ,
   UASMS_SM_LINK_CONTROL_REQ,
   UASMS_SM_UPDATE_TIMERF,

   UASMS_MT_SMS_RES                = 0x60,    /* message group */ 

   /* MN-SMS -> UASMS messages */

   UASMS_SMS_CONF                  = 0xA0,    /* message group */

   UASMS_SM_IND                    = 0xf0,    /* message group */
   UASMS_SM_DATA_IND,
   UASMS_SM_REPORT_IND,
   UASMS_SM_LINK_CONTROL_IND
};

/*
 * Link Control mode & type
 */
typedef enum 
{
   SMS_LINK_CONTROL_DISABLED = 0,
   SMS_LINK_CONTROL_ENABLED_ONE,
   SMS_LINK_CONTROL_ENABLED_ALL 
} sms_link_control_mode_T;

typedef enum
{
   SMS_LINK_CONTROL_INACTIVE = 0,
   SMS_LINK_CONTROL_ACTIVE, 
   SMS_LINK_CONTROL_TIMEOUT,

   SMS_LINK_CONTROL_RADIO_UNAVAILABLE,

   SMS_LINK_CONTROL_CONNECTION_RELEASED
} sms_link_control_status_T; 


typedef enum
{
 SMS_IMS_OK,
 SMS_IMS_TEMP_ERROR,
 SMS_IMS_PERM_ERROR
} sms_ims_cause_type_T;

typedef enum
{
 SMS_IMS_ERROR_IND,
 SMS_IMS_RPDU_IND
} sms_ims_msg_type_T;


typedef enum
{
  NAS_MSG_TRANSPORT_TYPE_GW,                                           /* For SMS received over GW */
  NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS,                                 /* For SMS received over IMS*/
  NAS_MSG_TRANSPORT_TYPE_MAX                                           /* Invalid */
} nas_msg_transport_type_e_type;

typedef struct
{
  boolean        present;
  byte           value;
}sms_ims_retry_interval_type_T;



/*************************************
 *     UASMS -to- MN primitives      *
 *************************************/
typedef   struct
{
   IMH_T                        message_header;
   /*
    Changed this from byte to word to be consistent
    with the introduction of the "word short_message_identifier"
    in the SM_RL_MEMORY_AVAILABLE_REQ_T.
   */
   word                         short_message_identifier;

   sys_modem_as_id_e_type       as_id;

   cm_called_party_bcd_no_T     RP_DA;
   byte                         tpdu_length;
   byte                         tpdu[TPDU_SIZE];
   nas_msg_transport_type_e_type    sms_transport_type;

}SM_RL_DATA_REQ_T;

typedef  struct
{
   IMH_T              message_header;
   connection_id_T    connection_id;
   boolean            sm_rl_success;
   byte               cause_value;
   boolean            diagnostics_present;
   byte               diagnostics;
   byte               tpdu_length;
   byte               tpdu[TPDU_SIZE];

   sys_modem_as_id_e_type  as_id;


}SM_RL_REPORT_REQ_T;    

typedef struct 
{
   IMH_T                            message_header;
   /*
    Added this for UASMS to correlate the report indications with
    memory availability request. 
   */
   word                             short_message_identifier;
   boolean                          abort;
   sys_modem_as_id_e_type           as_id;
   nas_msg_transport_type_e_type    sms_transport_type;

} SM_RL_MEMORY_AVAILABLE_REQ_T;

typedef struct 
{
   IMH_T    message_header;

   sys_modem_as_id_e_type  as_id;

} SM_RL_ABORT_SM_REQ_T;


typedef struct
{
   IMH_T    message_header;
   byte     timeout_in_seconds; 
   sms_link_control_mode_T   mode;

   sys_modem_as_id_e_type  as_id;

} SM_RL_LINK_CONTROL_REQ_T;

typedef struct
{
   IMH_T                   message_header;
   uint32                  timerf_value; 
   
   sys_modem_as_id_e_type  as_id;

} SM_RL_UPDATE_TIMERF_T;


/*************************************
 *     MN -to- UASMS primitives      *
 *************************************/
typedef   struct
{
   IMH_T                            message_header;
   connection_id_T                  connection_id;
   cm_called_party_bcd_no_T         RP_OA;
   byte                             tpdu_length;
// byte                             tpdu[TPDU_SIZE];
   byte                             tpdu[RP_USER_DATA_TPDU_LENGTH];
   nas_msg_transport_type_e_type    sms_transport_type;

   sys_modem_as_id_e_type  as_id;

}SM_RL_DATA_IND_T;

typedef  struct
{
   IMH_T    message_header;
   // Changed this from byte to word to be consistent
   // with the introduction of the "word short_message_identifier"
   // in the SM_RL_MEMORY_AVAILABLE_REQ_T.
   word     short_message_identifier;
   byte     error_status;
   byte     cause_value;
   word     ims_cause_value;
   boolean  diagnostics_present;
   byte     diagnostics;
   sms_ims_retry_interval_type_T  retry_interval;      
   byte     tpdu_length;
//   byte     tpdu[TPDU_SIZE];
   byte     tpdu[RP_USER_DATA_TPDU_LENGTH];


   /*
    * The following two members are introduced with 
    * the SMS Domain selection problem
    */
   boolean  permanent_error; /* Indicates any access class restriction or sim invalidated information*/
   mmcc_domain_name_T current_domain;/* the domain in which current SMS is tried*/


   sys_modem_as_id_e_type  as_id;

}SM_RL_REPORT_IND_T; 

typedef struct
{
   IMH_T                            message_header;
   sms_link_control_mode_T          mode;
   sms_link_control_status_T        status;

   sys_modem_as_id_e_type  as_id;

}SM_RL_LINK_CONTROL_REPORT_T;

/* The below feature should be defined only if LTE is defined. */


typedef struct
{
   cm_called_party_bcd_no_T  RP_DA;
   byte                      rpdu_length;
   byte                      rpdu[255];
   sys_modem_as_id_e_type    as_id;
}SM_RL_RPDU_T;

typedef struct
{
   sms_ims_msg_type_T             Ims_msg_type;
   sms_ims_cause_type_T           cause;
   word                           ims_cause_value;
   sms_ims_retry_interval_type_T  retry_interval;   
   byte                           rpdu_length;
   byte                           rpdu[255];
   sys_modem_as_id_e_type         as_id;
}SM_RL_IMS_MSG_T;

typedef enum 
{
   SMS_OK ,
   SMS_ACK_OK,
   SMS_TRANSACTION_ONGOING,
   SMS_TRANSACTION_INVALID,
   SMS_TRANSACTION_UNKNOWN
}sms_ims_state_e_type;


extern sms_ims_state_e_type UASMS_put_mn_ims_msg(SM_RL_IMS_MSG_T    ims_msg);



#endif


