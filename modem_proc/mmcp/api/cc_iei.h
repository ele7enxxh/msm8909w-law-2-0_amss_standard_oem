#ifndef cc_iei_h
#define cc_iei_h

/*===========================================================================
                     COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited..

                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cc_iei.h_v   1.6   12 Jul 2002 17:10:02   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cc_iei.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
25-Jul-2001  VT    Added the following elements for CS Data in the struct, 
                     ie_bearer_capability_T: Octet6d & Octet6e

01-Apr-2002  VT    GSM EFR: Added Octet3a, Octet3b, & Octet3c in 
                    ie_bearer_capability_T.
                   Actually merged the above changes from rev 1.4.1.0 (branch)of this file.

07/02/02     CD    Added ie_cause_of_no_CLI_T structure

05/10/05     HS    Added support for FEATURE_CCBS

11/08/02     AB    Added new IEI for FEATURE_REL5

===========================================================================*/
/*
 * repeat IE
 */

typedef struct ie_repeat_tag
{
   byte  repeat_data;
} ie_repeat_T;

/*
 * More data IE
 */

typedef struct ie_more_data_tag
{
   byte  Iei;
} ie_more_data_T;
/*
 * CLIR suppression
 */

typedef struct ie_clir_suppression_tag
{
   byte Iei;
} ie_clir_suppression_T;

/*
 * CLIR suppression
 */

typedef struct ie_clir_invocation_tag
{
   byte Iei;
} ie_clir_invocation_T;

/*
 * Reverse call setup direction
 */

typedef struct ie_reverse_call_setup_dir_tag
{
   byte Iei;
} ie_reverse_call_setup_dir_T;


/* Codec List IE */
typedef struct ie_codecs_list_tag
{
   byte Iei;
   byte Length;
   byte data[66];  /* tag + length + 16 x 4 sysID */
} ie_codecs_list_T;

/* Stream ID */
typedef struct ie_stream_id_tag
{
   byte Iei;
   byte Length;
   byte ID;
} ie_stream_id_T;

/* Network CC Capability */
typedef struct ie_network_cc_capability_tag
{
   byte Iei;
   byte Length;
   byte max_no_bearer_dtmf;
   byte max_speech_bearer;
} ie_network_cc_capability_T;

/*
 * Reverse call setup direction
 */

typedef struct ie_immediate_modify_ind_tag
{
   byte Iei;
} ie_immediate_modify_ind_T;

/*
 * Network-initiated Service Upgrade indicator
 */

typedef struct ie_nw_initiated_svc_upgrade_ind_tag
{
   byte Iei;
} ie_nw_initiated_svc_upgrade_ind_T;



/*
 * Bearer capability
 */

typedef struct ie_bearer_capability_tag
{
   byte  Iei;
   byte  Length;        /* length of bearer capability IE */
   byte  Octet3;        /* radio channel req, coding std, transfer */
                        /* mode & capability  */
   byte  Octet3a; /*  \                                                     */
   byte  Octet3b; /*   |-> Octets with coding & (GSM) speech ver indication */
   byte  Octet3c; /*  /                                                     */

   byte  Octet4;        /* structure, dupl. mode, config, NIRR, establi */
   byte  Octet5;        /* rate adaption, signalling access protocol */
   byte  Octet6;        /* user info layer 1 protocol, sync/async */
   byte  Octet6a;       /* NStop, negotiation, NData bits, user rate */
   byte  Octet6b;       /* intermed. rate, NIC on Tx, NIC on Rx, parity */
   byte  Octet6c;       /* connection element, modem type     */
   byte  Octet6d;       /* Other modem type, Fixed network user rate */
   byte  Octet6e;       /* Acceptable channel codings, Max number of 
                                               traffic channels  */
   byte  Octet6f;     

   byte  Octet7;        /* user info layer 2 protocol */

} ie_bearer_capability_T;

/* bit pattern for bearer capability values */

/*
 * keypad facility IE
 */

typedef struct ie_keypad_facility_tag
{
   byte  Iei;
   byte  IA5Char;      /* 0,1,2,3,4,5,6,7,8,9,A,B,C,D,*,# */
} ie_keypad_facility_T;

/* bit 1 - establishment - always 0 */

/* Contents of other octets not defined - assumed not used by CC/MN */
 
/* Cause values are specified elsewhere (cause.h) */

/* 
 * facility IE
 */
typedef struct ie_facility_tag
{
   byte Iei;
   byte Length;
   byte data[MAX_COMPONENTS_LENGTH];
} ie_facility_T;


typedef struct ie_progress_indicator_tag
{
   byte  Iei;
   byte  Length;
   byte  CodstdLocation;
   byte  ProgressDescription;
} ie_progress_indicator_T;
/*
 * conected number IE
 */

typedef struct ie_connected_number_tag
{
   byte  Iei;
   byte  Length;
   byte  TypeNumplanid;      /* type of number, number plan id */
   byte  PresScreeningInd;  /* presentation indicator, screening indicator */
   byte  Digit[10];           /* two digits per byte */
} ie_connected_number_T;

/*
 * Auxiliary state IE
 */

typedef struct ie_auxiliary_state_tag
{
   byte  Iei;
   byte  length;
   byte  states;
} ie_auxiliary_state_T;


/*
 * call control capabilities IE
 */

typedef struct ie_cc_capabilities_tag
{
   byte  Iei;
   byte  length;
   byte  DTMF;      /* 0 = earlier protocol versions, 1 = DTMF supported */
} ie_cc_capabilities_T;


/*
 * signal IE
 */

typedef struct ie_signal_tag
{
   byte  Iei;
   byte  SignalValue;
} ie_signal_T;

/*
 * low layer compatibility
 */

typedef struct ie_low_layer_compatibility_tag
{
   byte  Iei;
   byte  Length;
   byte  Compatibility[16];
} ie_low_layer_compatibility_T;
/*
 * high layer compatibility
 */

typedef struct ie_high_layer_compatibility_tag
{
   byte  Iei;
   byte  Length;
   byte  Octet3;        /* coding std, interpretation, presentation method */
                        /* of protocol profile  */
   byte  HighLevelCharIdentification;
   byte  ExtendedHighLevelCharIdentification;

} ie_high_layer_compatibility_T;

typedef struct ie_notification_indicator_tag
{
   byte   Iei;
   byte   notification_description;
}ie_notification_indicator_T;
/*
 * connected subaddress IE
 */

typedef struct ie_connected_subaddress_tag
{
   byte  Iei;
   byte  Length;
   byte  TypeOe;                /* type of subaddress, odd/even indicator */
   byte  SubaddressInfo[20];

} ie_connected_subaddress_T;
/*
 * calling party BCD no
 */

#define  CC_MAX_CALLING_PTY_BCD_NUM_LENGTH  0x0C 
/* 12 octets is the max value that the Calling Party BCD IE Length can have */

typedef struct ie_calling_party_bcd_no_tag
{
   byte  Iei;
   byte  Length;
   byte  TypeNumplanid;      /* type of number, number plan id */
   byte  PresScreeningInd;  /* presentation indicator, screening indicator */
   byte  Digit[CC_MAX_CALLING_PTY_BCD_NUM_LENGTH - 2];
} ie_calling_party_bcd_no_T;
/* 
 * called party subaddress
 */
#define  CC_MAX_CALLED_PTY_SUBADDR_LENGTH  0x15
/* 21 octets is the max value that the Called Party SUBAddress IE Length can have */

typedef struct ie_called_party_subaddress_tag
{
   byte  Iei;
   byte  Length;
   byte  TypeOe;                /* type of subaddress, odd/even indicator */
   byte  SubaddressInfo[CC_MAX_CALLED_PTY_SUBADDR_LENGTH - 1];
} ie_called_party_subaddress_T;

/* 
 * calling party subaddress
 */
 
#define  CC_MAX_CALLING_PTY_SUBADDR_LENGTH  0x15
#define  CC_MAX_REDIRECTING_PTY_SUBADDR_LENGTH  CC_MAX_CALLING_PTY_SUBADDR_LENGTH
/* 21 octets is the max value that the Calling Party SUBAddress IE Length can have */
/* Same holds for the Redirecting Party SubAddress IE Length */

/*
 * called party BCD no
 */
#define  CC_MAX_CALLED_PTY_BCD_NUM_LENGTH  0x29 
/* 41 octets is the max value that the Called Party BCD IE Length can have */

typedef struct ie_called_party_bcd_no_tag
{
   byte  Iei;
   byte  Length;
   byte  TypeNumplanid;      /* type of number, number plan id */
   byte  Digit[CC_MAX_CALLED_PTY_BCD_NUM_LENGTH - 1];
} ie_called_party_bcd_no_T;

#define IEI_CONNECTED_SUBADDRESS_MAX_LENGTH  23
#define IEI_CONNECTED_NUMBER_MAX_LENGTH  14
#define IEI_PROGRESS_INDICATOR_LENGTH 4
#define IEI_AUXILIARY_STATES_LENGTH 3
#define IEI_HIGH_LAYER_COMPATIB_MAX_LENGTH 5

typedef struct ie_calling_party_subaddress_tag
{
   byte  Iei;
   byte  Length;
   byte  TypeOe;                /* type of subaddress, odd/even indicator */
   byte  SubaddressInfo[CC_MAX_CALLING_PTY_SUBADDR_LENGTH - 1];

} ie_calling_party_subaddress_T;
/*
 * Redirected party BCD no
 */

#define  CC_MAX_REDIRECTING_PTY_BCD_NUM_LENGTH  0x11 
/* 17 octets is the max value that the Calling Party BCD IE Length can have */

typedef struct ie_redirecting_party_bcd_no_tag
{
   byte  Iei;
   byte  Length;
   byte  TypeNumplanid;      /* type of number, number plan id */
   byte  PresScreeningInd;  /* presentation indicator, screening indicator */
   byte  Digit[CC_MAX_REDIRECTING_PTY_BCD_NUM_LENGTH - 2];
} ie_redirecting_party_bcd_no_T;



/*
 * user-user ie
 */

typedef struct ie_user_usern_tag
{
   byte  Iei;
   byte  Length;
   byte  UuProtocolDescriminator;
   byte  UserUserInformation[MAX_USER_USER_DATA_LENGTH];   /* limited to 32 for setup, alerting,
                                           connect, disconnect, release, and 
                                           release complete messages    */
} ie_user_user_T;

/*
 * SS version indicator
 */

typedef struct ie_ss_version_ind_tag
{
   byte  Iei;
   byte  Length;
   byte  SsVersionInfo[MAX_OCTETS_L3_MESSAGE-2];

} ie_ss_version_ind_T;


/*
 * Cause of no CLI IE
 */

typedef struct ie_cause_of_no_CLI_tag
{
    byte Iei;
    byte Length;
    byte cause;
}ie_cause_of_no_CLI_T;


/*
 * Alerting Pattern IE
 */
 
typedef struct ie_alerting_pattern_tag
{
    byte Iei;
    byte Length;
    byte alerting_pattern_value;
}ie_alerting_pattern_T;

/*
 * Network CC Capability IE
 */

typedef struct ie_network_CC_CAP_tag
{
    byte Iei;
    byte Length;
    byte capability; /* MCS - Multicall supports */
}ie_network_CC_CAP_T;



/*
 * Allowed actions IE
 */

typedef struct ie_allowed_actions_tag
{
    byte Iei;
    byte Length;
    byte cause;   /* coded as TS 24.008 cl. 10.5.4.27 octet 3 */
} ie_allowed_actions_T;


typedef struct ie_recall_type_tag   /* TS24.008 10.5.4.21a */
{
   byte Iei;
   byte RecallType;
} ie_recall_type_T;

typedef struct ie_setup_container_tag
{
   byte Length;
   byte data[MAX_OCTETS_L3_MESSAGE-3];
} ie_setup_container_T;



#endif
