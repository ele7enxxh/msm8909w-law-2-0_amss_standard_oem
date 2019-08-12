#ifndef ds_cap_v_h
#define ds_cap_v_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ds_cap.h_v   1.13   04 Feb 2002 11:53:02   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/ds_cap_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
10/25/04   AR     Added definitions ITC_OTHER, OITC_RDI

08/09/04   AR     Added definitions LLC_38400, LLC_57600

27-Jul-2001  VT   CS Data: Added the defines for DATA_COMPRESSION*, 
                    ACCESS_OCTET_ID, LAYER_1_OCTET_ID, FNUR_57600,
                    LAYER_2_OCTET_ID & RESERVED_OMT

                  Removed the unused (& confusing) defines for 
                    ACC_14400_NOT_ACCEPTABLE, ACC_9600_NOT_ACCEPTABLE &
                    ACC_4800_NOT_ACCEPTABLE.

08/15/01    AB    Renamed MMI to CM.

08/29/01    TS    Renamed PARITY defines because they conflicted with windows
                    defines.
08/31/01    TS    Renamed TRANSPARENT define to avoid conflict with windows 
                    define.

18-Sep-2001 VT   Corrected #define X75 to 0x0D as per TS 24.008 V3.6.0

09/30/02   dgy    Corrected the defines for Acceptable Channel Codings. 

11/9/01     CD   Per CM request added mn_cm_exp.h 

02/01/02    CD   Added definitiin of CM_LOW_LAYER_COMPATIBILITY per CM request

11/26/02    AB   Added other rate adaption definitions to support multimedia
                   services.    

04/17/03   dgy   Added definition of ASYMMETRY_INDICATION_UNUSED and 
                 LLC_28800.
===========================================================================*/
#include "ds_cap.h"
#include "mn_cm_exp_v.h"
/*Lower Layer Compatibility field definitions*/
#define CM_LOW_LAYER_COMPATIBILITY           18
/*see ETS 300 102-1 table 4.18 */

/*LLC structure*/
#define LLC_DEFAULT_STRUCTURE                 0
#define LLC_8KHZ_INTEGRITY                    1
#define LLC_SERVICE_DATA_UNIT_INTEGRITY       4
#define LLC_UNSTRUCTURED                      7
/*LLC symmetry*/
#define BIDIRECTIONAL_SYMMETRIC               0
/* (octet 5b V.120)*/
/*rate_adaption_header*/
#define RATE_ADAPTION_HEADER_NOT_INCLUDED     0
#define RATE_ADAPTION_HEADER_INCLUDED         1

/*multiple_frame_establishment_support*/
#define MULTIPLE_FRAME_ESTABLISHMENT_NOT_SUPPORTED  0
#define MULTIPLE_FRAME_ESTABLISHMENT_SUPPORTED      1

/*mode_of_operation*/
#define BIT_TRANSPARENT_MODE                   0
#define PROTOCOL_SENSITIVE_MODE                1

/*logical_link_identifier_negotiation*/
#define DEFAULT_LLI                            0
#define FULL_PROTOCOL_NEGOTIATION              1

/*assignor_assignee*/
#define ORIGINATOR_IS_DEFAULT_ASSIGNEE         0
#define ORIGINATOR_IS_ASSIGNOR_ONLY            1

/*in_band_out_band_negotiation*/
#define NEGOTIATION_WITH_USER_INFORMATION      0
#define NEGOTIATION_IN_BAND                    1




/* BS & TS defs */

#define BS_21                                 21
#define BS_22                                 22
#define BS_23                                 23
#define BS_24                                 24
#define BS_25                                 25
#define BS_26                                 26
#define BS_31                                 31
#define BS_32                                 32
#define BS_33                                 33
#define BS_34                                 34
#define BS_41                                 41
#define BS_42                                 42
#define BS_43                                 43
#define BS_44                                 44
#define BS_45                                 45
#define BS_46                                 46
#define BS_51                                 51
#define BS_52                                 52
#define BS_53                                 53
#define BS_61                                 61
#define BS_81                                 81

#define TS_11                                 11
#define TS_12                                 12
#define TS_21                                 21
#define TS_22                                 22
#define TS_23                                 23
#define TS_61                                 61
#define TS_62                                 62

#define MAX_CME_ERROR 32
#define MAX_CMS_ERROR 520

#define MAX_SMS_BODY_LEN 300;

#define PIN_LEN 8;

/********************************************************
 * These definitions are additions for Phase2+ 
 ********************************************************/


/* Network to MS direction: spare */

/* User Initiated Modification Indication */
#define UIMI_UP_TO_1_TCH                      1
#define UIMI_UP_TO_2_TCH                      2
#define UIMI_UP_TO_3_TCH                      3
#define UIMI_UP_TO_4_TCH                      4

/******** Declaration of supported DTE values ***********/

/* N.B. - These are different to the supported BC vales (defined below) *//*define structure to store low layer compatibility*/

typedef struct
{
   CM_IEI_T              IEI;
   byte                   coding_standard;
   byte                   information_transfer_capability;
   byte                   negotiation_indicator;                /*not used*/
   byte                   transfer_mode;
   byte                   information_transfer_rate;
   byte                   structure;                            /*not used*/
   byte                   configuration;                        /*not used*/
   byte                   establishment;                        /*not used*/
   byte                   symmetry;                             /*not used*/
   byte                   information_transfer_rate_d_to_o;     /*not used*/
   byte                   user_information_layer_1_protocol;
   byte                   sync_async;
   byte                   negotiation;
   byte                   user_rate;
   byte                   intermediate_rate;                    /*V.110 only*/
   byte                   NIC_on_Tx;                            /*V.110 only*/
   byte                   NIC_on_Rx;                            /*V.110 only*/
   byte                   flow_control_on_Tx;                   /*V.110 only*/
   byte                   flow_control_on_Rx;                   /*V.110 only*/
   byte                   rate_adaption_header;                 /*V.120 only*/
   byte                   multiple_frame_establishment_support; /*V.120 only*/
   byte                   mode_of_operation;                    /*V.120 only*/
   byte                   logical_link_identifier_negotiation;  /*V.120 only*/
   byte                   assignor_assignee;                    /*V.120 only*/
   byte                   in_band_out_band_negotiation;         /*V.120 only*/
   byte                   number_of_stop_bits;
   byte                   number_of_data_bits;
   byte                   parity;
   byte                   duplex_mode;
   byte                   modem_type;
   byte                   user_information_layer_2_protocol;
   byte                   optional_layer_2_protocol_information;
   byte                   user_information_layer_3_protocol;
   byte                   optional_layer_3_protocol_information;
} ds_low_layer_compatibility_T;

typedef struct
{
   byte user_rate;
   byte connection_element;
   byte sync_async;
} supported_capability_length_T;

#endif  /* ds_cap_h */


