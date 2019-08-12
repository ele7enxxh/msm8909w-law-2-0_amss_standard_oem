#ifndef ds_cap_h
#define ds_cap_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ds_cap.h_v   1.13   04 Feb 2002 11:53:02   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/ds_cap.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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
#include "mn_cm_exp.h"


extern byte mn_get_als_itc_aux_speech(void);

/*bearer capability definitions
*******************************/

/*radio channel requirement definitions*/
#define FULL_RATE_ONLY                       1
#define DUAL_RATE_HALF_RATE_PREFERRED        2
#define DUAL_RATE_FULL_RATE_PREFERRED        3

/* Information Transfer Capability definitions - see GSM 04.08 table 10.72 */
#define ITC_SPEECH                           0  /* Speech */
#define ITC_UDI                              1  /* Unrestricted Digital Information */
#define ITC_3_1K                             2  /* 3.1 KHz audio ex PLMN */
#define ITC_FAX_G3                           3  /* Facsimile group 3 */
#define ITC_AUX_SPEECH                       (mn_get_als_itc_aux_speech())  /* Auxillary Speech */
/*
#define ITC_AUX_SPEECH                       4  
     ITC  for the AUX_SPEECH can also be 6 (new value). 
     This is now NV Controllable to be configured to 6 or 4.
*/
#define ITC_OTHER                            5  /* Other ITC (eg RDI) */

/* Other Information Transfer Capability definitions  */
#define OITC_RDI                             0  /* Restricted Digital Information */

/*structure definitions*/
#define SERVICE_DATA_UNIT_INTEGRITY          0
#define UNSTRUCTURED                         3

/*definition of duplex mode*/
#define HALF_DUPLEX                          0
#define FULL_DUPLEX                          1

/*rate adaption defs*/
#define V110_X30                             1
#define X31_FLAGSTUFFING                     2
#define NO_RATE_ADAPTION                     0
#define OTHER_RATE_ADAPTION                  3

/* other ITC */
#define RESTRICTED_DIGITAL_INFORMATION       0

/* other rate adaption defs */
#define V120                                 0
#define H223_H245                            1
#define PIAFS                                2

/* signalling access protocol defs */
#define I440_450                             1
#define X21                                  2
#define X28_DED_PAD_INDIVIDUAL_NUI           3
#define X28_DED_PAD_UNIVERSAL_NUI            4
#define X28_NON_DED_PAD                      5
#define X32                                  6

/*sync/ async defs*/
#define SYNCHRONOUS                          0
#define ASYNCHRONOUS                         1

/*character size*/
#define _7_BITS_PER_CHAR                     0
#define _8_BITS_PER_CHAR                     1

/*stop bits*/
#define _1_STOP_BIT                          0
#define _2_STOP_BITS                         1

/*user rate*/
#define USER_RATE_300                        1
#define USER_RATE_1200                       2
#define USER_RATE_2400                       3
#define USER_RATE_4800                       4
#define USER_RATE_9600                       5
#define USER_RATE_1200_75                    7
#define USER_RATE_14400                      8  /* See NOTE below */
/* 
 *NOTE: In these values, the defintion for 14400bd is for the DS/MMI use
 *      ONLY. In the structures representing the negotiated capability,
 *      there is no defined USER_RATE for 14400bd. For a 14.4kb link, 
 *      the Phase2+ structure FNUR is used instead. See 07.01 (Ph.2+)
 */


/*parity*/
#define NAS_PARITY_ODD                       0
#define NAS_PARITY_EVEN                      2
#define NAS_PARITY_NONE                      3
#define NAS_PARITY_FORCE_0                   4
#define NAS_PARITY_FORCE_1                   5

/*modem type*/
#define NO_MODEM                             0
#define V21                                  1
#define V22                                  2
#define V22_BIS                              3
#define V23                                  4
#define V26_TER                              5
#define V32                                  6
#define MT_UNDEFINED                         7
#define AUTO_TYPE_1                          8

/*connection element*/
#define TRANSPARENT_DATA                     0
#define NON_TRANSPARENT_DATA                 1
#define BOTH_TRANSPARENT_PREF                2
#define BOTH_NON_TRANSPARENT_PREF            3

/*layer 2 protocol*/
#define X25                                  0x06
#define ISO6429                              0x08
#define X75                                  0x0D
#define VIDEOTEX                             0x0A
#define CHARACTER_ORIENTED_PROTOCOL          0x0C

/*coding standard*/
#define GSM                                  0

/*transfer mode*/
#define CIRCUIT                              0
#define PACKET                               1

/*configuration*/
#define POINT_TO_POINT                       0

/* compression */
#define DATA_COMPRESSION_NOT_POSSIBLE_OR_ALLOWED  0
#define DATA_COMPRESSION_POSSIBLE_OR_ALLOWED      1

/* access identity */
#define ACCESS_OCTET_ID                             0

/* layer 1 identity */
#define LAYER_1_OCTET_ID                            1

/* layer 2 identity */
#define LAYER_2_OCTET_ID                            2

/*NIRR*/
#define NO_MEANING                           0
#define REQUESTED                            1

/*establishment*/
#define DEMAND                               0

/*user info l1 protocol*/
#define DEFAULT_LAYER_1_PROTOCOL             0

/*negotiation*/
#define IN_BAND_NEGOTIATION_NOT_POSSIBLE     0

/*intermediate rate*/
#define _8_KBIT                              2
#define _16_KBIT                             3

/*NIC on Tx options*/
#define NIC_ON_TX_NOT_REQUIRED               0
#define NIC_ON_TX_REQUIRED                   1

/*NIC on Rx options*/
#define NIC_ON_RX_NOT_SUPPORTED              0
#define NIC_ON_RX_SUPPORTED                  1

/* Asymmetry indication */
#define ASYMMETRY_INDICATION_UNUSED          3

/*LLC_coding_standard (octet 3)*/
#define LLC_CCITT                             0
#define LLC_OTHER_INTERNATIONAL               1
#define LLC_NATIONAL                          2
#define LLC_NETWORK                           3

/*info transfer capability (octet 3)*/
#define LLC_SPEECH                            0x00
#define LLC_UNRESTRICTED_DIGITAL              0x08
#define LLC_RESTRICTED_DIGITAL                0x09
#define LLC_3_1_KHZ_AUDIO                     0x10
#define LLC_7_KHZ_AUDIO                       0x11
#define LLC_VIDEO                             0x18

/*transfer mode (octet 4)*/
/* LLC_transfer_mode */
#define LLC_CIRCUIT                           0
#define LLC_PACKET                            2

/*LLC info transfer rate (octet 4)*/
#define LLC_64KBIT                            0x10
#define LLC_2_64KBIT                          0x11
#define LLC_384KBIT                           0x13
#define LLC_1536KBIT                          0x15
#define LLC_1920KBIT                          0x17


/*LLC configuration*/
#define POINT_TO_POINT                        0

/*LLC establishment*/
#define DEMAND                                0



/*LLC_user_information_layer_1_protocol (octet 5)*/
#define CCITT_V110                            0x01
#define G711_MU_LAW                           0x02
#define G711_A_LAW                            0x03
#define G721_ADPCM                            0x04
#define G722_G725_AUDIO                       0x05
#define G7XX_VIDEO                            0x06
#define NON_CCITT                             0x07
#define CCITT_V120                            0x08
#define CCITT_X31                             0x09

/*async/ sync for LLC (octet 5a)*/
#define LLC_SYNCHRONOUS                       0
#define LLC_ASYNCHRONOUS                      1

/*negotiation (octet 5a)*/
#define LLC_IN_BAND_NEGOTIATION_NOT_POSSIBLE  0
#define LLC_IN_BAND_NEGOTIATION_POSSIBLE      1

/*user rate for LLC element (octet 5a)*/
#define LLC_600                               0x01
#define LLC_1200                              0x02
#define LLC_2400                              0x03
#define LLC_3600                              0x04
#define LLC_4800                              0x05
#define LLC_7200                              0x06
#define LLC_8000                              0x07
#define LLC_9600                              0x08
#define LLC_14400                             0x09
#define LLC_16000                             0x0A
#define LLC_19200                             0x0B
#define LLC_32000                             0x0C
#define LLC_38400                             0x0D
#define LLC_48000                             0x0E
#define LLC_56000                             0x0F
#define LLC_64000                             0x10
#define LLC_57600                             0x12
#define LLC_28800                             0x13
#define LLC_134_5                             0x15
#define LLC_100                               0x16
#define LLC_75_1200                           0x17
#define LLC_1200_75                           0x18
#define LLC_50                                0x19
#define LLC_75                                0x1A
#define LLC_110                               0x1B
#define LLC_150                               0x1C
#define LLC_200                               0x1D
#define LLC_300                               0x1E
#define LLC_12000                             0x1F

/*intermediate rate for LLC element (octet 5b V.110)*/
#define LLC_NOT_USED                          0
#define LLC_8KBIT                             1
#define LLC_16KBIT                            2
#define LLC_32KBIT                            3

/*NIC on Tx options*/
#define LLC_NIC_ON_TX_NOT_REQUIRED             0
#define LLC_NIC_ON_TX_REQUIRED                 1

/*NIC on Rx options*/
#define LLC_NIC_ON_RX_NOT_SUPPORTED            0
#define LLC_NIC_ON_RX_SUPPORTED                1

/*flow control on transmission (octet 5b V.110)*/
#define LLC_TX_FLOW_CONTROL_NOT_REQUIRED      0
#define LLC_TX_FLOW_CONTROL_REQUIRED          1

/*flow control on reception (octet 5b V.110)*/
#define LLC_RX_FLOW_CONTROL_NOT_SUPPORTED     0
#define LLC_RX_FLOW_CONTROL_SUPPORTED         1

/*LLC number of stop bits (octet 5c)*/
#define LLC_1_STOP                            1
#define LLC_1_5_STOP                          2
#define LLC_2_STOP                            3

/*number of data bits for LLC (octet 5c)*/
#define LLC_5_DATA                            1
#define LLC_7_DATA                            2
#define LLC_8_DATA                            3

/*parity info for LLC (octet 5c)*/
#define LLC_PARITY_ODD                        0
#define LLC_PARITY_EVEN                       2
#define LLC_PARITY_NONE                       3
#define LLC_PARITY_FORCE_0                    4
#define LLC_PARITY_FORCE_1                    5

/*duplex mode for LLC (octet 5d)*/
#define LLC_HALF_DUPLEX                       0
#define LLC_FULL_DUPLEX                       1

/*modem type for LLC (octet 5d)*/
/* Thes values changed - see ETS 300 102-1 Dec 1990/A1: Apr 1993 */
#define LLC_V21                               0x11
#define LLC_V22                               0x12
#define LLC_V22_BIS                           0x13
#define LLC_V23                               0x14
#define LLC_V26                               0x15
#define LLC_V26_BIS                           0x16
#define LLC_V26_TER                           0x17
#define LLC_V27                               0x18
#define LLC_V27_BIS                           0x19
#define LLC_V27_TER                           0x1A
#define LLC_V29                               0x1B
#define LLC_V32                               0x1C
#define LLC_V32_BIS                           LLC_V32
#define LLC_V34                               0x1E
/*#define LLC_V35                               0x0D No longer defined*/
#define MAX_DISPLAY_TEXT_LEN 255
#define MAX_LENGTH 32000
#define MAX_SM_LENGTH 300


/********************************************************
 * These definitions are additions for Phase2+ 
 ********************************************************/

/* Other Modem Type */
#define OMT_V32bis                            1
#define OMT_V34                               2
#define RESERVED_OMT                          3

/* Fixed Network User Rate */
#define FNUR_9600                             0x01
#define FNUR_14400                            0x02
#define FNUR_19200                            0x03
#define FNUR_28800                            0x04
#define FNUR_38400                            0x05
#define FNUR_48K                              0x06
#define FNUR_56K                              0x07
#define FNUR_64K                              0x08
#define FNUR_33600                            0x09
#define FNUR_32K                              0x0A
#define FNUR_31200                            0x0B
#define FNUR_57600                            0x0C

/* Acceptable Channel Codings (MS to network) */
#define ACC_14400_ACCEPTABLE                  (0x40 >> 3)
#define ACC_9600_ACCEPTABLE                   (0x10 >> 3)
#define ACC_4800_ACCEPTABLE                   (0x08 >> 3)
/* Network to MS direction: spare */

/* Maximum Number of Traffic Channels (MS to network direction) */
#define MAX_NUMBER_OF_TCH_1                   0
#define MAX_NUMBER_OF_TCH_2                   1
#define MAX_NUMBER_OF_TCH_3                   2
#define MAX_NUMBER_OF_TCH_4                   3
#define MAX_NUMBER_OF_TCH_5                   4
#define MAX_NUMBER_OF_TCH_6                   5
#define MAX_NUMBER_OF_TCH_7                   6
#define MAX_NUMBER_OF_TCH_8                   7
/* Network to MS direction: spare */

/* Wanted Air Interface User Rate (MS to Network Direction) */
#define WAIUR_9600                            1
#define WAIUR_14400                           2
#define WAIUR_19200                           3
#define WAIUR_28800                           5
#define WAIUR_38400                           6
#define WAIUR_43200                           7
#define WAIUR_57600                           8

#endif  /* ds_cap_h */


