#ifndef CMUTIL_H
#define CMUTIL_H
/*===========================================================================

         C A L L   M A N A G E R   U T I L I T Y   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMUTIL.C


Copyright (c) 2001 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cmutil.h#1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
06/26/13   ss      Fix cmutil_number_cmp, Params Changed
05/02/13   ss      New API for converting ASCII to BCD
01/04/11   rm      CMI-4: Removing #ifdef's
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
07/26/09   rn/sk   Centralised call control changes
                   Added macros, enums to support parsing functionality.
04/30/09   rk      Moving MAX_DISPLAY_TEXT_LEN macro definition to cm.h
01/20/08   fj      Move APIs to cm.h
09/15/08   np      Added HPCD Rev B.  
03/17/08   ka      Fixing plus to digit conversion when converted num
                   results in dtmf for SMS.
01/04/08   ka      Adding handset based plus code dial feature
01/09/07   ka      Changing 0xFFFFFFFF to 0x7FFFFFFF to satisfy C++ compilers
11/23/05   ka      Giving SIP error access to UI clients.
07/25/05   sk      Added constant CHAR_CR.
07/08/05   ic      Moved several function declarations to cm.h
08/18/04   ic      Constants used by cm_util_set_default_voice_bearer_cap()
                   now defined under FEATURE_GSM || FEATURE_WCDMA
08/11/04   ka      Utility function for reading bearer capability structure
07/27/04   ws      Add functions to default Orig parameters.
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
09/11/02   RI      Added prototype for cm_util_subaddr_to_ascii().
03/25/02   HD      Added macro for ISDIGIT.
02/08/02   PRK     Changed the cm_util_sys_id_match function prototype.
01/23/02   PRK     Replaced cmutil prefix with cm_util prefix.
                   Added sys_id utility functions.
11/21/01   RI      Modified function declaration of
                   cmutil_phone_ascii_to_phone_bcd().
09/13/01   RI      Initial Release
===========================================================================*/
#include "comdef.h"
#include "cm.h"       /* Needed for cm_sys_id_s_type */
#include "cm_v.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define LCD_COLUMNS       12
#define CHAR_CR           0x0D

#define BCD_LEN    0
#define BCD_NT_NPI 1
#define BCD_NUM    2
#define BCD_INVALID_BYTE           0x99
#define BCD_STAR                   0x0A
#define BCD_HASH                   0x0B
#define BCD_END_FLAG               0x0F

#define ISDIGIT(X) ((X >= '0') && (X <= '9'))

/* bearer capability definitions */
#define FULL_RATE_ONLY                       1
#define DUAL_RATE_FULL_RATE_PREFERRED        3

/* Information Transfer Capability definitions - see GSM 04.08 table 10.72 */
#define ITC_SPEECH                0  /* Speech */
#define ITC_UDI                   1  /* Unrestricted Digital Information */
#define ITC_3_1K                  2  /* 3.1 KHz audio ex PLMN */
#define ITC_FAX_G3                3  /* Facsimile group 3 */
//#define ITC_AUX_SPEECH            4  /* Auxillary Speech */
#define ITC_OTHER                 5  /* Other ITC (eg RDI) */

/*
** The definition of the Information Transfer Capability (octet 3, bits 3..1)
** of the Bearer Capability Information Element - BCIE (as defined in GSM 04.08)
** shall be extended to set the following bit to denote LINE_01 / LINE_02
**
** Bits: 3 2 1 (1 is least significant bit)
**
**       0 0 0 - Speech           (LINE_01)
**       1 0 0 - Auxiliary speech (LINE_02)
**
** The below defines Orange settings for Information Transfer Capability field
*/
#define CMALS_GSM_LL_SPEECH                  0x00
#define CMALS_ORANGE_GSM_LL_AUXILIARY_SPEECH 0x04
#define CMALS_ORANGE_GSM_LL_AUXILIARY_SPEECH_SIX 0x06



/*coding standard*/
#define GSM                                  0

/*transfer mode*/
#define CIRCUIT                              0

#define LTM_INVALID 127
  /* Invalid LTM. Giving it a high enough value */

#define  NETWORK_SOLUTION_ABSENT 0
  /* Network solution for sid range not present */

#define  NETWORK_SOLUTION_PRESENT 1
  /* Network solution for sid range is present */

#define  DRR_STRING             "*77"
  /* Starting prefix for Direct routing
  ** roamer to roamer number
  */

#define  CMUTIL_PLUS_REPL_DIGIT_LEN 6
  /* Len of digits that can replace plus */

#define CMUTIL_MCC_OF_CHINA 460
  /* MCC value of China */

#define  CMUTIL_IDD_DIGIT_LEN 6
  /* Len of international dialed digit*/


#define CMUTIL_COUNTRY_CODE_LEN  7
  /* Should be maximum size country_code can potentially have */

/* Constants for origination message translation, From 1x CP*/
#define CMUTIL_DTMF_FOR_DIAL_0    0xA
#define CMUTIL_DTMF_FOR_DIAL_STAR 0xB
#define CMUTIL_DTMF_FOR_DIAL_PND  0xC
#define CMUTIL_DTMF_FOR_DIAL_NULL 0x0

 
#define CMUTIL_MCC_LOOP_LEN 30
  /*  The length of MCC loop */

/* Maximum MMI string length for BSG */
#define CMUTIL_SUPS_MAX_BSG_LEN            2
/* Maximum MMI string length for Sups Service code */
#define CMUTIL_SUPS_MAX_SC_LEN             3
/* Maximum MMI string length for SIA part of Service Information */
#define CMUTIL_SUPS_MAX_SIA_LEN            80  
/* Changed from 20 to accomodate GCF 27.22.4.11.1 Seq 1.4 */
#define CMUTIL_SUPS_MAX_SIB_LEN            4   /* PWD LEN */
#define CMUTIL_SUPS_MAX_SIC_LEN            4   /* PWD LEN */
/*Minimum digits in supplementary/ussd string*/
#define CMUTIL_MIN_SUPS_DIGITS             2
/*Maximum number of tokens for the sups string */
#define CMUTIL_SUPS_MAX_TOKENS             6
#define CMUTIL_SUPS_BSG_BM( val )          (val << 16)

/* MMI Constants for Sups service code per 3GPP TS 22.030 */
#define CMUTIL_SUPS_LOCAL_SC_ALL               ""
#define CMUTIL_SUPS_LOCAL_SC_CF                "002"
#define CMUTIL_SUPS_LOCAL_SC_CFC               "004"
#define CMUTIL_SUPS_LOCAL_SC_CFU               "21"
#define CMUTIL_SUPS_LOCAL_SC_CFB               "67"
#define CMUTIL_SUPS_LOCAL_SC_CFNRY             "61"
#define CMUTIL_SUPS_LOCAL_SC_CFNRC             "62"
#define CMUTIL_SUPS_LOCAL_SC_CLIP              "30"
#define CMUTIL_SUPS_LOCAL_SC_CLIR              "31"
#define CMUTIL_SUPS_LOCAL_SC_COLP              "76"
#define CMUTIL_SUPS_LOCAL_SC_COLR              "77"
#define CMUTIL_SUPS_LOCAL_SC_CW                "43"
#define CMUTIL_SUPS_LOCAL_SC_CNAP              "300"
#define CMUTIL_SUPS_LOCAL_SC_B                 "330"
#define CMUTIL_SUPS_LOCAL_SC_OB                "333"
#define CMUTIL_SUPS_LOCAL_SC_IB                "353"
#define CMUTIL_SUPS_LOCAL_SC_BAOC              "33"
#define CMUTIL_SUPS_LOCAL_SC_BOIC              "331"
#define CMUTIL_SUPS_LOCAL_SC_BOICEXHC          "332"
#define CMUTIL_SUPS_LOCAL_SC_BAIC              "35"
#define CMUTIL_SUPS_LOCAL_SC_BICROAM           "351"
#define CMUTIL_SUPS_LOCAL_BSG_REG_PASSWD       "03"
#define CMUTIL_SUPS_LOCAL_SC_CCBS              "37"

/* MMI Constants for Basic Service Group per 3GPP TS 22.030 */
/* Tele Services */
#define CMUTIL_SUPS_LOCAL_BSG_ALL              ""
#define CMUTIL_SUPS_LOCAL_BSG_ALL_TS           "10"
#define CMUTIL_SUPS_LOCAL_BSG_TELEPHONY        "11"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_DATA         "12"
#define CMUTIL_SUPS_LOCAL_BSG_FAX              "13"
#define CMUTIL_SUPS_LOCAL_BSG_SMS              "16"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_TELE_EX_SMS  "19"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_ALL    "50"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_1      "51"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_2      "51"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_3      "53"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_4      "54"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_5      "55"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_6      "56"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_7      "57"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_8      "58"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_9      "59"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_A      "60"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_B      "61"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_C      "62"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_D      "63"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_E      "64"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_F      "65"

/* Bearer Services */
#define CMUTIL_SUPS_LOCAL_BSG_ALL_BS           "20"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_ASYNC        "21"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_SYNC         "22"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_DC_SYNC      "24"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_DC_ASYNC     "25"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_DED_PACKET   "26"
#define CMUTIL_SUPS_LOCAL_BSG_ALL_DED_PAD      "27"
/*#define CMUTIL_SUPS_LOCAL_BSG_12_KBIT        "29"  -- Not supported*/
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_ALL    "70"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_1      "71"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_2      "71"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_3      "73"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_4      "74"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_5      "75"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_6      "76"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_7      "77"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_8      "78"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_9      "79"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_A      "80"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_B      "81"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_C      "82"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_D      "83"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_E      "84"
#define CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_F      "85"


/* Network value for Basic Service Group as per 3GPP TS 09.02*/
typedef enum
{
  CMUTIL_SUPS_BSG_ALL_SERVICES =0x00 | (CMUTIL_SUPS_BSG_BM(NO_CODE)),
  /*
  ** BEARER-SERVICES per 3GPP TS 09.02
  */
  CMUTIL_SUPS_BSG_ALL_BEARER_SERVICES     = allBearerServices |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_DATA_CDA_SERVICES   = allDataCDA_Services |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDA_300BPS         = dataCDA_300bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDA_1200BPS        = dataCDA_1200bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDA_1200_75BPS     = dataCDA_1200_75bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDA_2400BPS        = dataCDA_2400bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDA_4800BPS        = dataCDA_4800bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDA_9600BPS        = dataCDA_9600bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_DATA_CDS_SERVICES   = allDataCDS_Services |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDS_1200BPS        = dataCDS_1200bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDS_2400BPS        = dataCDS_2400bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDS_4800BPS        = dataCDS_4800bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_CDS_9600BPS        = dataCDS_9600bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_PAD_ACCESS_CA_SERVICES  = allPadAccessCA_Services |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PAD_ACCESS_CA_300BPS    = padAccessCA_300bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PAD_ACCESS_CA_1200BPS   = padAccessCA_1200bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PAD_ACCESS_CA_1200_75BPS= padAccessCA_1200_75bps |
                                             (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PAD_ACCESS_CA_2400BPS   = padAccessCA_2400bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PAD_ACCESS_CA_4800BPS   = padAccessCA_4800bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PAD_ACCESS_CA_9600BPS   = padAccessCA_9600bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_DATA_PDS_SERVICES   = allDataPDS_Services |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_PDS_2400BPS        = dataPDS_2400bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_PDS_4800BPS        = dataPDS_4800bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_DATA_PDS_9600BPS        = dataPDS_9600bps |
                                            (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_ALTERNATE_SPEECH_DATA_CDA=allAlternateSpeech_DataCDA |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_ALTERNATE_SPEECH_DATA_CDS=allAlternateSpeech_DataCDS |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_SPEECH_FOLLWOED_BYDATA_CDA=allSpeechFollowedByDataCDA |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_SPEECH_FOLLWOED_BYDATA_CDS=allSpeechFollowedByDataCDS |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_ASYNCHRONOUS =allDataCircuitAsynchronous |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_SYNCHRONOUS = allDataCircuitSynchronous |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_ASYNCHRONOUS_SERVICES = allAsynchronousServices |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_SYNCHRONOUS_SERVICES  = allSynchronousServices |
                                              (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_BS =0xD0 |(CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_1   = plmn_specificBS_1 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_2   = plmn_specificBS_2 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_3   = plmn_specificBS_3 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_4   = plmn_specificBS_4 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_5   = plmn_specificBS_5 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_6   = plmn_specificBS_6 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_7   = plmn_specificBS_7 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_8   = plmn_specificBS_8 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_9   = plmn_specificBS_9 |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_A   = plmn_specificBS_A |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_B   = plmn_specificBS_B |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_C   = plmn_specificBS_C |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_D   = plmn_specificBS_D |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_E   = plmn_specificBS_E |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_F   = plmn_specificBS_F |
                                         (CMUTIL_SUPS_BSG_BM(BS_CODE)),
/*
** TELE-SERVICES per 3GPP TS 09.02
*/
  CMUTIL_SUPS_BSG_ALL_TELESERVICES     = allTeleservices |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_ALL_SPEECH_TRANSMISSION_SERVCES=
               allSpeechTransmissionservices |(CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_TELEPHONY            = telephony |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_EMERGENCY_CALLS      = emergencyCalls |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_ALL_SMS_SERVICES     = allShortMessageServices |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_SHORT_MSG_MT_PP      = shortMessageMT_PP |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_SHORT_MSG_MO_PP      = shortMessageMO_PP |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_ALL_FAX_TRANSMISSION_SERVICES  =
        allFacsimileTransmissionServices |(CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_FAX_GROUP3_AND_ALTER_SPEECH    =
        facsimileGroup3AndAlterSpeech |(CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_AUTO_FAX_GROUP3       = automaticFacsimileGroup3 |
                                          (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_FAX_GROUP4            = facsimileGroup4 |
                                         ( CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_ALL_DATA_TELESERVICES = allDataTeleservices |
                                          (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_ALL_TELESERVICES_EXCEPT_SMS  = allTeleservices_ExeptSMS |
                                            (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_TS = allPLMN_specificTS |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_1   = plmn_specificTS_1 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_2   = plmn_specificTS_2 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_3   = plmn_specificTS_3 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_4   = plmn_specificTS_4 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_5   = plmn_specificTS_5 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_6   = plmn_specificTS_6 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_7   = plmn_specificTS_7 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_8   = plmn_specificTS_8 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_9   = plmn_specificTS_9 |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_A   = plmn_specificTS_A |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_B   = plmn_specificTS_B |
                                        (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_C   = plmn_specificTS_C |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_D   = plmn_specificTS_D |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_E   = plmn_specificTS_E |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE)),
  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_F   = plmn_specificTS_F |
                                         (CMUTIL_SUPS_BSG_BM(TS_CODE))

}cmutil_sups_basic_service_e_type;

/*Password Information*/
typedef struct
{
  char    ss_password[MAX_PWD_CHAR+1];
    /**< Nul terminated Current password */

  char    ss_new_password[MAX_PWD_CHAR+1];
    /**< Nul terminated New password */

  char    ss_new_password_again[MAX_PWD_CHAR+1];
    /**< Nul terminated New password entered again */

}cmutil_sups_passwd_s_type;

/* Sups Registration Information */
/* Will be used in case of call forwarding
   wherin user has to enter a number */
typedef struct
{
  char      data[CM_MAX_NUMBER_CHARS+1];
    /**< Nul terminated call Forward number */

  uint32    len;
    /**< Length of the Forwarded number string */

  uint32    nr_timer;
    /**< No reply timer */

}cmutil_sups_reg_s_type; 

/*Unstructured Supplementary Service Data*/
typedef struct
{
  uint32    length;
    /**< Length of the ussd MMI string */

  char      data[MAX_USS_CHAR_ASCII+1];
    /**< Nul terminated ussd MMI string */

} cmutil_sups_ussd_s_type;

/*Supplementary Service Start Info*/
typedef struct
{
  cm_sups_cmd_e_type                  ss_operation;
    /**< Sups operation Register/Activate etc */

  ss_operation_code_T                 ss_code;
    /**< Network value of service code */

  cmutil_sups_basic_service_e_type    basic_service; 
    /**< Network value of Basic service group */

  union
  {
    cmutil_sups_reg_s_type         reg;
    /**< Registration data used in call forwarding case */

    cmutil_sups_passwd_s_type      passwd;
    /**< Password information used in change password */

    cmutil_sups_ussd_s_type        ussd;
    /**< USSD MMI data entered by user */

    uint32                         ccbsIndex;
    /**< Completion of calls to busy subscriber index
    ** This is in accordance with 3GPP TS 22.093 */

  }req;
}cmutil_sups_start_s_type;

/*Supplementary structure filled and returned by the parsing function*/
typedef struct cmutil_sups_params_s
{
     cm_sups_cmd_e_type              sups_cmd;
    /* < Indicates what sups command the dialed digits is modified to. */

  union
  {
    cm_erase_ss_params_s_type        erase_ss_params;
    /**< Parameters applicable for Sups Erasure */

    cm_reg_ss_params_s_type          reg_ss_params;
    /**< Parameters applicable for Sups Registration */

    cm_act_ss_params_s_type          activate_ss_params;
    /**< Parameters applicable for Sups Activation */

    cm_deact_ss_params_s_type        deactivate_ss_params;
    /**< Parameters applicable for Sups Deactivation */

    cm_interr_ss_params_s_type       interrogate_ss_params;
    /**< Parameters applicable for Sups Interrogation */

    cm_uss_params_s_type             uss_params;
    /**< Parameters applicable for Sups USSD operation */

    cm_reg_pwd_params_s_type         reg_pwd_params;
    /**< Parameters applicable for Sups Register password */

  } sups_params;
}cmutil_sups_params_s_type;

/*Return value of Sups Parsing function*/
typedef enum 
{

  CMUTIL_PARSE_SUPS_RESULT_NONE = -1,
    /* Bounds checking */

  CMUTIL_PARSE_SUPS_RESULT_SUCCESS,
    /* Sups parse successful */

  CMUTIL_PARSE_SUPS_RESULT_ERR_REGEX_NO_MATCH,
    /* String doesn't match Regex */

  CMUTIL_PARSE_SUPS_RESULT_ERR_REGEX_ILLEGAL_ENTRY,
    /* Illegal Regex entry */

  CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT,
    /* Input sups string passed is invalid */

  CMUTIL_PARSE_SUPS_RESULT_ERR_NUL_PTR,
    /* Mem allocation failed or string is NULL  */

  CMUTIL_PARSE_SUPS_RESULT_MAX

}cmutil_parse_sups_result_e_type;

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/**===========================================================================

@FUNCTION: cmutil_parse_sups_string

@DESCRIPTION
  Parses call independent supplementary service control string. The input sups
  string passed should fall under one of the following categories
  Register, Activate, Erase, Interrogate, Deactivate, Register Password, USSD.

@DEPENDENCIES
  None.

@RETURNS
  Returns a structure cm_util_sups_params_s_type filled with sups params and 
  sups operation of type cm_sups_cmd_e_type. Also returns an value of type 
  cm_util_parse_sups_result_e_type indicating success/failure

@SIDE_EFFECT
  None

@x
===========================================================================*/
cmutil_parse_sups_result_e_type   cmutil_parse_sups_string(
                                                             
    cm_num_s_type    sups_num_str,
    /*Input Sups string*/
 
    uint8    sups_str_len,
    /*Input sups string length*/
 
    cmutil_sups_params_s_type    *sups_params
    /*Output Sups params*/
);


/*===========================================================================

FUNCTION: cmutil_get_sys_bcmcs_max_flows

DESCRIPTION
  This function returns the max value of 
  cmutil_get_sys_bcmcs_max_flows

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 cmutil_get_sys_bcmcs_max_flows( void );


/*===========================================================================

FUNCTION: cm_util_cm_max_emergency_num_count

DESCRIPTION
  This function returns the max value of the cm_max_emergency_num_count

DEPENDENCIES
  FEATURE_GSM ||FEATURE_WCDMA

RETURN VALUE
 maximum no. of emergency count based on definition of feature

SIDE EFFECTS
  None  
===========================================================================*/
extern uint8 cm_util_cm_max_emergency_num_count(void);
/* cm_util_cm_max_emergency_num_count() */


#ifdef CM_DEBUG
#error code not present
#endif

/*===========================================================================

FUNCTION: cmutil_num_of_subs_avail

DESCRIPTION
  This function returns the number of subscription available

DEPENDENCIES
  NONE

RETURN VALUE
  number of subs avail

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 cmutil_num_of_subs_avail(void);

/*===========================================================================

FUNCTION: cmutil_all_subs_avail

DESCRIPTION
  This function returns true if all subscription is available

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean cmutil_all_subs_avail(void);

/*===========================================================================

FUNCTION: cmutil_all_card_avail

DESCRIPTION
  This function returns true if all cards are available or not

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean cmutil_all_card_avail(void);

/*===========================================================================
FUNCTION cmutil_copy_char_array_to_str

DESCRIPTION
To copy a character array to a string that need to be terminated wiht '\0'

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cmutil_copy_char_array_to_str(char *pDest, size_t dest_size, char const *pSrc, size_t src_len);


/*===========================================================================

FUNCTION cmutil_read_policyman_ueMode

DESCRIPTION
  This function read the UE mode from policymanager


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern sys_ue_mode_e_type cmutil_read_policyman_ueMode( void );

/*===========================================================================

FUNCTION: cm_util_get_curr_systime

DESCRIPTION
  This function returns the current time with reference to start of time,
  sicne EPOCH by using API time_genoff_opr().
  However, time returned by it cannot be altered by network ovet the air.

DEPENDENCIES
  None

RETURN VALUE
 current system time

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 cm_util_get_curr_systime(void);

/*===========================================================================

FUNCTION cm_data_high_priority_sub

DESCRIPTION
  This function returns the sub with high data priority


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_data_high_priority_sub( void );


#endif
