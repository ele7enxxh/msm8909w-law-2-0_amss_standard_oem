#ifndef networks_h
#define networks_h

/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/networks.h_v   1.3   19 Mar 2001 17:43:32   tschwarz  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/networks.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------



===========================================================================*/



#define MAX_SHORT_NET_NAME_LEN   8
#define MAX_LONG_NET_NAME_LEN    16
#define MAX_COUNTRY_NAME_LEN     3
#define TBA                      9999   /* To Be Announced */

typedef enum
{
   NET_UNKNOWN_TYPE,
   NET_GSM_900,
   NET_DCS_1800,
   NET_PCS_1900,
   NET_GSM_SAT
}
network_type_T;

#include "mmcp_variation.h"
#if defined m22000_c
char * net_type_string[] =
{ "??", "GSM-900", "GSM-1800", "GSM-1900", "GSM-SAT", "???", "???" };
#endif

typedef struct
{
   word           mcc;    /* Mobile Country Code: 3 digits, TBA = don't know */
   word           mnc;    /* Mobile Network Code: 3 digits in PCS, else 2    */
   network_type_T type;         /* Type of Network                           */
   char *         country_text; /* Country Abbreviation                      */
   char *         short_name;   /* Short Name for Handset display            */
   char *         long_name;    /* Long Name for Handset display             */
}
network_name_T;

/* Fixes a compilation problem */
#if defined m70000_c || defined m50000_c || (defined m22000_c)

const network_name_T network_names[] =
{
   /****************
   **** Austria ****
   ****************/
   /* Mobilkom Austria AG   Colour Code=0   SIM=894301 */
   { 232,  1, NET_GSM_900, "A", "A1", "A1" },
   /* Max Mobil Telecommunications Service   Colour Code=4   SIM=894303 */
   { 232,  3, NET_GSM_900, "A", "max.", "A max." },

   /****************
   **** Albania ****
   ****************/
   /* Albanian Mobile Communications   Colour Code=1   SIM=8935501 */
   { 276,  1, NET_GSM_900, "AL", "A M C", "AMC - AL" },

   /****************
   **** Andorra ****
   ****************/
   /* Servei De Tele. DAndorra   Colour Code=3   SIM=8937603 */
   { 213,  3, NET_GSM_900, "AND", "M-AND", "STA-MOBILAND" },

   /***************************
   **** Armenia (Republic) ****
   ***************************/
   /* ArmenTel */
   { 283,  1, NET_GSM_900, "ARM", "ARMMO1", "ARMMO1" },

   /******************
   **** Australia ****
   ******************/
   /* Telstra Corporation Limited   Colour Code=0   SIM=896101 */
   { 505,  1, NET_GSM_900, "AUS", "M-NET", "MOBILENET-AUS" },
   /* Optus Communications   Colour Code=45   SIM=896102 */
   { 505,  2, NET_GSM_900, "AUS", "Optus", "YES OPTUS AUS" },
   /* Vodafone Pty Limited   Colour Code=2   SIM=896103 */
   { 505,  3, NET_GSM_900, "AUS", "VFONE", "VODAFONE AUS" },

   /*******************
   **** Azerbaijan ****
   *******************/
   /* Azercell Telecom BM   Colour Code=1   SIM=8999401 */
   { 400,  1, NET_GSM_900, "AZE", "ACELL", "AZE - AZERCELL GSM" },

   /****************
   **** Belgium ****
   ****************/
   /* Mobistar S.A.   Colour Code=5   SIM=893202 */
   { 206, 10, NET_GSM_900, "B", "mobi*", "B mobistar" },
   /* Belgacom Mobile   Colour Code=1   SIM=893200 */
   { 206,  1, NET_GSM_900, "BEL", "PROXI", "BEL PROXIMUS" },

   /*****************
   **** Bulgaria ****
   *****************/
   /* MobilTel AD   Colour Code=1   SIM=8935901 */
   { 284,  1, NET_GSM_900, "BG", "M-TEL", "M-TEL GSM BG" },

   /*******************
   **** Bangladesh ****
   *******************/
   /* GrameenPhone Ltd   Colour Code=4   SIM=8988017 */
   { 470,  1, NET_GSM_900, "BGD", "GP", "BGD-GP" },

   /****************
   **** Bahrain ****
   ****************/
   /* BATELCO   Colour Code=3   SIM=8997301 */
   { 426,  1, NET_GSM_900, "BHR", "M.PLUS", "BHR MOBILE PLUS" },

   /***************************
   **** Bosnia Herzegovina ****
   ***************************/
   /* Public Enterprise PTT BiH   Colour Code=?   SIM=8938790 */
   { 218, 90, NET_GSM_900, "BIH", "BHGSM", "PTT-GSMBIH" },

   /**************************
   **** Brunei Darussalam ****
   **************************/
   /* DataStream Technology   Colour Code=4   SIM=8967311 */
   { 528, 11, NET_GSM_900, "BRU", "DSTCom", "BRU-DSTCom" },

   /*****************
   **** Cameroon ****
   *****************/
   /* Ministry of P & T Cameroon   Colour Code=- - -   SIM=8923701 */
   { 624,  1, NET_GSM_900, "CAM", "CAMNET", "CAM CELLNET" },

   /***************
   **** Canada ****
   ***************/
   /* Microcell Telecommunications Inc   Colour Code=0   SIM=8930237 */
   { 302, 37, NET_PCS_1900, "CAN", "MCELL", "CAN-MCELL" },

   /******************************************************
   **** Canada again, alternative 2/3 digit net codes ****
   ******************************************************/
   /* Microcell Telecommunications Inc   Colour Code=0   SIM=8930237 */
   { 302, 370, NET_PCS_1900, "CAN", "MCELL", "CAN-MCELL" },

   /********************
   **** Switzerland ****
   ********************/
   /* Swiss Telecom PTT   Colour Code=1   SIM=894101 */
   { 228,  1, NET_GSM_900, "CH", "SWISS", "SWISS GSM" },
   /* Swiss Telecom PTT   Colour Code=1   SIM=894101 */
   { 228,  1, NET_DCS_1800, "CH", "SWISS", "SWISS GSM" },
   /* diAx Mobile AG   Colour Code=5   SIM=894102 */
   { 228,  2, NET_GSM_900, "CH", "diAx", "diAx Swiss" },

   /*******************
   **** China (PR) ****
   *******************/
   /* China Telecom   Colour Code=4; 6; 7   SIM=898600 */
   { 460,  0, NET_GSM_900, "CHN", "CT-GSM", "CHINA TELECOM" },
   /* China Unicom   Colour Code=5   SIM=898601 */
   { 460,  1, NET_GSM_900, "CHN", "CU-GSM", "CHN-CUGSM" },

   /**********************
   **** Cote d'Ivoire ****
   **********************/
   /* Loteny Telecom   Colour Code=5   SIM=8922505 */
   { 612,  5, NET_GSM_900, "CI", "TELCEL", "TELECEL-CI" },
   /* Societe Ivorienne De Mobiles (SIM)   Colour Code=1   SIM=8922503 */
   { 612,  3, NET_GSM_900, "CI", "Ivoir", "CI Ivoiris" },

   /**************
   **** Chile ****
   **************/
   /* ENTEL PCS   Colour Code=1   SIM=8956100 */
   { 730, 10, NET_PCS_1900, "CL", "ENTEL", "CL ENTEL PCS" },
   /* ENTEL TELEFONIA MOVIL   Colour Code=0   SIM=8956010 */
   { 730,  1, NET_PCS_1900, "CL", "ENTEL", "CL ENTEL PCS" },

   /**************
   ****       ****
   **************/
   /* Cabo Verde Telecom   Colour Code=0   SIM=8923801 */
   { 625,  1, NET_GSM_900, "CPV", "CMOVEL", "CPV MOVEL" },

   /***************
   **** Cyprus ****
   ***************/
   /* Cyprus Telecommunications Auth   Colour Code=3   SIM=8935701 */
   { 280,  1, NET_GSM_900, "CY", "CY-GSM", "CY CYTAGSM" },

   /*************************
   **** Czech (Republic) ****
   *************************/
   /* RadioMobil   Colour Code=5   SIM=894201 */
   { 230,  1, NET_GSM_900, "CZ", "PAEGAS", "PAEGAS- CZ" },
   /* Eurotel Praha Ltd   Colour Code=1   SIM=894202 */
   { 230,  2, NET_GSM_900, "CZ", "ET - CZ", "EUROTEL - CZ" },

   /****************
   **** Germany ****
   ****************/
   /* DeTeMobil (DBP Telekom)   Colour Code=3   SIM=894902 */
   { 262,  1, NET_GSM_900, "D", "D1", "D1-TELEKOM" },
   /* Mannesmann Mobilfunk GmbH   Colour Code=7   SIM=894920 */
   { 262,  2, NET_GSM_900, "D", "D2", "D2 PRIVAT" },
   /* E-Plus Mobilfunk   Colour Code=3   SIM=894921 */
   { 262,  3, NET_DCS_1800, "D", "E-Plus", "E-Plus" },

   /****************
   **** Denmark ****
   ****************/
   /* Sonofon   Colour Code=5   SIM=894502 */
   { 238,  2, NET_GSM_900, "DK", "SONO", "DK SONOFON" },
   /* Mobilix   Colour Code=1   SIM=894505 */
   { 238, 30, NET_DCS_1800, "DK", "#mbix", "DK mobilix" },
   /* Telia A/S Denmark   Colour Code=1   SIM=894504 */
   { 238, 20, NET_DCS_1800, "DK", "TELIA", "TELIA DK" },
   /* Tele Danmark A/S   Colour Code=1   SIM=894501 */
   { 238,  1, NET_GSM_900, "DK", "TD MOB", "DK TDK-MOBIL" },

   /**************
   **** Spain ****
   **************/
   /* Airtel   Colour Code=5   SIM=893456 */
   { 214,  1, NET_GSM_900, "E", "AIRTL", "E AIRTEL" },
   /* Telefonica Moviles   Colour Code=1   SIM=893407 */
   { 214,  7, NET_GSM_900, "E", "MSTAR", "MOVISTAR" },

   /****************
   **** Estonia ****
   ****************/
   /* Estonian Mobil Telephone Co   Colour Code=3   SIM=8937201 */
   { 248,  1, NET_GSM_900, "EE", "EMT", "EE EMT GSM" },
   /* Ritabell   Colour Code=7   SIM=8937203 */
   { 248,  3, NET_GSM_900, "EE", "Q GSM", "EE Q GSM" },
   /* Radiolinja Estonia Ltd   Colour Code=1   SIM=8937202 */
   { 248,  2, NET_GSM_900, "EE", "RLE", "EE RLE" },

   /*****************
   **** Ethiopia ****
   *****************/
   /* Ethiopian Telecommunications Authority   Colour Code=1   SIM=? */
   { 636,  1, NET_GSM_900, "ETH", "ET-MTN", "ETH-MTN" },

   /***************
   **** France ****
   ***************/
   /* France Telecom Mobile   Colour Code=0   SIM=893301 */
   { 208,  1, NET_GSM_900, "F", "Itine", "F Itineris" },
   /* Tikiphone SA   Colour Code=1   SIM=8968920 */
   { 547, 20, NET_GSM_900, "F", "VINI", "F-VINI" },
   /* Societe Reunionnaise de Radiotelephone   Colour Code=?   SIM=893310 */
   { 647, 10, NET_GSM_900, "F", "SFR RU", "SFR REUNION" },
   /* Bouygues Telecom   Colour Code=4   SIM=893320 */
   { 208, 20, NET_DCS_1800, "F", "BYTEL", "F - BOUYGUES TELECOM" },
   /* SFR / COFIRA   Colour Code=4   SIM=893310 */
   { 208, 10, NET_GSM_900, "F", "SFR", "F SFR" },
   /* France Caraibe Mobiles   Colour Code=0   SIM=8959001 */
   { 340,  1, NET_GSM_900, "F", "AMERIS", "F AMERIS" },

   /****************
   **** Finland ****
   ****************/
   /* Sonera Corporation   Colour Code=0   SIM=8935801 */
   { 244, 91, NET_GSM_900, "FI", "SONERA", "FI SONERA" },
   /* Oy Radiolinja Ab   Colour Code=4   SIM=8935802 */
   { 244,  5, NET_GSM_900, "FI", "RL", "FI RADIOLINJA" },
   /* Finnet Group   Colour Code=4   SIM=8935805 */
   { 244,  9, NET_DCS_1800, "FI", "FINNET", "FI FINNET" },
   /* Telia Finland Oy   Colour Code=0   SIM=8935804 */
   { 244,  3, NET_DCS_1800, "FI", "TELIA", "FI TELIA" },

   /*************
   **** Fiji ****
   *************/
   /* Vodafone Fiji Ltd   Colour Code=2   SIM=8967901 */
   { 542,  1, NET_GSM_900, "FIJ", "VODAFJ", "VODAFONE FIJ" },

   /****************
   **** Georgia ****
   ****************/
   /* Geocell Ltd   Colour Code=5   SIM=8999501 */
   { 282,  1, NET_GSM_900, "GEO", "GCELL", "GEO-GEOCELL" },
   /* Magti GSM   Colour Code=2   SIM=8999597 */
   { 282,  2, NET_GSM_900, "GEO", "MAGTI", "MAGTI-GSM-GEO" },


   /**************
   **** Ghana ****
   **************/
   /* ScanCom Ltd   Colour Code=- - -   SIM=8923301 */
   { 620,  1, NET_GSM_900, "GH", "SPACE", "GH SPACEFON" },

   /******************
   **** Gibraltar ****
   ******************/
   /* Gibraltar Telecommunications Int. Ltd.   Colour Code=3   SIM=8935001 */
   { 266,  1, NET_GSM_900, "GIB", "GIBTEL", "GIBTEL GSM" },

   /***************
   **** Guinea ****
   ***************/
   /* Sotelgui sa   Colour Code=0   SIM=8922402 */
   { 611,  2, NET_GSM_900, "GN", "LAGUI", "GN LAGUI" },

   /***************
   **** Greece ****
   ***************/
   /* STET Hellas Telecommunications   Colour Code=4   SIM=893010 */
   { 202, 10, NET_GSM_900, "GR", "TLSTET", "GR TELESTET" },
   /* Cellular Operating System Mobile Telecom   Colour Code=7   SIM=893001 */
   { 202,  1, NET_DCS_1800, "GR", "C-OTE", "GR COSMOTE" },
   /* Panafon SA   Colour Code=0   SIM=893005 */
   { 202,  5, NET_GSM_900, "GR", "PAN", "GR PANAFON" },

   /****************
   **** Hungary ****
   ****************/
   /* Westel900 Mobil Tavkozlesi Rt.   Colour Code=6   SIM=893630 */
   { 216, 30, NET_GSM_900, "H", "W-900", "H-WESTEL 900" },
   /* Pannon GSM Telecommunications   Colour Code=2   SIM=893620 */
   { 216,  1, NET_GSM_900, "H", "PANNON", "H PANNON GSM" },

   /******************
   **** Hong Kong ****
   ******************/
   /* Hongkong Telecom CSL   Colour Code=0;1;4   SIM=8985200 */
   { 454,  0, NET_GSM_900, "HK", "TELCO", "HK TELECOM" },
   /* P Plus Communications Ltd   Colour Code=4   SIM=8985222 */
   { 454, 22, NET_DCS_1800, "HK", "P PLUS", "HK P PLUS" },
   /* Peoples Telephone Company Ltd   Colour Code=0   SIM=8985212 */
   { 454, 12, NET_DCS_1800, "HK", "PTC", "HK PEOPLES" },
   /* Pacific Link   Colour Code=0   SIM=8985218 */
   { 454, 18, NET_DCS_1800, "HK", "P-LINK", "HK PACLINK" },
   /* New World PCS   Colour Code=1   SIM=8985210 */
   { 454, 10, NET_DCS_1800, "HK", "NWT", "HK NWT" },
   /* Mandarin Communications Ltd   Colour Code=4   SIM=8985216 */
   { 454, 16, NET_DCS_1800, "HK", "SUNDAY", "HK SUNDAY" },
   /* Hutchison Telecom (HK) Ltd   Colour Code=4;0;1   SIM=8985204 */
   { 454,  4, NET_GSM_900, "HK", "HTCL", "HK HTCLGSM" },
   /* SmarTone Mobile Comms. Ltd.   Colour Code=1;0;4   SIM=8985206 */
   { 454,  6, NET_GSM_900, "HK", "HKSMC", "HK SMARTONE" },

   /****************
   **** Croatia ****
   ****************/
   /* HPT Croatian P & T   Colour Code=1   SIM=899506 */
   { 219,  1, NET_GSM_900, "HR", "CRON", "HR - CRONET" },

   /**************
   **** Italy ****
   **************/
   /* Omnitel Pronto Italia   Colour Code=6   SIM=893910 */
   { 222, 10, NET_GSM_900, "I", "OMNI", "I -OMNITEL" },
   /* Telecom Italia Mobile   Colour Code=2   SIM=893901 */
   { 222,  1, NET_GSM_900, "I", "TIM", "I TELECOM" },

   /***************
   **** Israel ****
   ***************/
   /* Partner Communications Company Ltd   Colour Code=1;5   SIM=8997201 */
   { TBA ,  1, NET_GSM_900, "IL", "PARTNR", "IL PARTNER" },

   /**************
   **** India ****
   **************/
   /* BPL USWEST Cellular   Colour Code=2   SIM=899146 */
   { 404, 46, NET_GSM_900, "INA", "BPL MO", "BPL MOBILE" },
   /* BPL USWEST Cellular   Colour Code=6   SIM=899143 */
   { 404, 43, NET_GSM_900, "INA", "BPL MO", "BPL MOBILE" },
   /* Aircel Digilink India Ltd   Colour Code=2   SIM=899101 */
   { 404,  1, NET_GSM_900, "INA", "ESSARH", "INA ESSAR CELLPHONE" },
   /* Aircel Digilink India Ltd   Colour Code=1   SIM=899115 */
   { 404, 15, NET_GSM_900, "INA", "ESSARU", "INA ESSAR CELLPHONE" },
   /* Aircel Digilink India Ltd   Colour Code=4   SIM=899160 */
   { 404, 60, NET_GSM_900, "INA", "ESSARR", "INA ESSAR CELLPHONE" },
   /* Modi Telstra   Colour Code=?   SIM=899114 */
   { 404, 14, NET_GSM_900, "INA", "MODICO", "MODICOM INA" },
   /* RPG Cellular Services   Colour Code=1   SIM=899141 */
   { 404, 41, NET_GSM_900, "INA", "RPG", "INA RPG" },
   /* Sterling Cellular Ltd (SCL)   Colour Code=5;7   SIM=899111 */
   { 404, 11, NET_GSM_900, "INA", "ESSAR", "INA ESSAR CELLPHONE" },
   /* TATA Communications Ltd   Colour Code=5   SIM=899107 */
   { 404,  7, NET_GSM_900, "INA", "TATA", "INA - TATA CELLULAR" },
   /* Usha Martin Telekom Ltd   Colour Code=?   SIM=899130 */
   { 404, 30, NET_GSM_900, "INA", "COMMND", "INA COMMAND" },
   /* Bharti Cellular Ltd   Colour Code=1   SIM=899110 */
   { 404, 10, NET_GSM_900, "INA", "AIRTL", "INA-AIRTL" },
   /* Escotel Mobile Communications   Colour Code=?   SIM=899112 */
   { 404, 12, NET_GSM_900, "INA", "ESCOTL", "INA - ESCOTEL" },
   /* BPL USWEST Cellular   Colour Code=6   SIM=899127 */
   { 404, 27, NET_GSM_900, "INA", "BPL MOBILE", "BPL MOBILE" },
   /* Hutchison Max Telecom PVT Ltd   Colour Code=- - -   SIM=899120 */
   { 404, 20, NET_GSM_900, "INA", "MAXTCH", "INA MaxTouch" },
   /* BPL Mobile Communications Ltd   Colour Code=2   SIM=899121 */
   { 404, 21, NET_GSM_900, "INA", "BPL MOBILE", "BPL MOBILE" },

   /******************
   **** Indonesia ****
   ******************/
   /* Excelcom   Colour Code=4   SIM=896211 */
   { 510, 11, NET_GSM_900, "IND", "EXCEL", "IND GSM-XL" },
   /* PT Satelindo   Colour Code=3   SIM=896201 */
   { 510,  1, NET_GSM_900, "IND", "SAT-C", "IND SATELINDOCEL" },
   /* PT Telekomunikasi Selular   Colour Code=2   SIM=896210 */
   { 510, 10, NET_GSM_900, "IND", "T-SEL", "IND TELKOMSEL" },

   /****************
   **** Ireland ****
   ****************/
   /* Esat Digifone   Colour Code=7   SIM=8935302 */
   { 272,  2, NET_GSM_900, "IRL", "DIGI", "IRL DIGIFONE" },
   /* Eircell   Colour Code=3   SIM=8935301 */
   { 272,  1, NET_GSM_900, "IRL", "E-GSM", "IRL EIR-GSM" },

   /****************
   **** Iceland ****
   ****************/
   /* Iceland Telecom   Colour Code=0   SIM=8935401 */
   { 274,  1, NET_GSM_900, "IS", "SIMINN", "IS SIMINN" },
   /* TAL hf.   Colour Code=4   SIM=8935402 */
   { 274,  2, NET_GSM_900, "IS", "TAL", "IS TAL" },

   /***************
   **** Jordan ****
   ***************/
   /* JMTS   Colour Code=0   SIM=8996201 */
   { 416,  1, NET_GSM_900, "JOR", "FSTLNK", "Fastlink" },

   /************************
   **** Kyrgyz Republic ****
   ************************/
   /* Bitel Ltd   Colour Code=- - -   SIM=8999601 */
   { 437,  1, NET_GSM_900, "KGZ", "BITEL", "BITEL KGZ" },

   /***************************
   **** Cambodia (Kingdom) ****
   ***************************/
   /* CamGSM   Colour Code=5   SIM=8985501 */
   { 456,  1, NET_GSM_900, "KHM", "MT-KHM", "MOBITEL - KHM" },

   /*********************
   **** Saudi Arabia ****
   *********************/
   /* Electronic Application Est- AlJawwal   Colour Code=7   SIM=? */
   { 420,  7, NET_GSM_900, "KSA", "EAE", "EAE-ALJAWWAL" },
   /* Ministry of PTT - AlJawwal   Colour Code=3   SIM=8996601 */
   { 420,  1, NET_GSM_900, "KSA", "KSA", "ALJAWWAL" },

   /***************
   **** Kuwait ****
   ***************/
   /* Mobile Telecommunications Co.   Colour Code=6   SIM=899659 */
   { 419,  2, NET_GSM_900, "KT", "MTC", "KT MTCNet" },

   /*******************
   **** Luxembourg ****
   *******************/
   /* P & T Luxembourg   Colour Code=2   SIM=8935201 */
   { 270,  1, NET_GSM_900, "L", "LUXGSM", "L LUXGSM" },
   /* Millicom Luxembourg S.A.   Colour Code=- - -   SIM=8935277 */
   { 270, 77, NET_GSM_900, "L", "TANGO", "L TANGO" },
   /* Millicom Luxembourg S.A.   Colour Code=- - -   SIM=8935277 */
   { 270, 77, NET_DCS_1800, "L", "TANGO", "L TANGO" },

   /****************
   **** Lesotho ****
   ****************/
   /* Vodacom Lesotho (Pty) Ltd   Colour Code=7   SIM=? */
   { 651,  1, NET_GSM_900, "LSO", "VCLCOM", "VCL COMMS" },

   /******************
   **** Lithuania ****
   ******************/
   /* UAB Bite GSM   Colour Code=7   SIM=8937002 */
   { 246,  2, NET_GSM_900, "LT", "BITE", "LT BITE GSM" },
   /* OMNITEL   Colour Code=3   SIM=8937001 */
   { 246,  1, NET_GSM_900, "LTU", "OMT", "OMNITEL LT" },

   /***************
   **** Latvia ****
   ***************/
   /* Baltcom GSM   Colour Code=6   SIM=8937102 */
   { 247,  2, NET_GSM_900, "LV", "B-COM", "LV BALTCOM" },
   /* Latvian Mobile Telephone Co   Colour Code=2   SIM=8937101 */
   { 247,  1, NET_GSM_900, "LV", "LMT", "LV LMT GSM" },

   /**************
   **** Malta ****
   **************/
   /* Vodafone Malta Limited   Colour Code=0   SIM=8935601 */
   { 278,  1, NET_GSM_900, "M", "VODA M", "VODAFONE MLA" },

   /**************
   **** Macau ****
   **************/
   /* CTM   Colour Code=5   SIM=8985301 */
   { 455,  1, NET_GSM_900, "MAC", "CTMGSM", "MAC-CTMGSM" },

   /**************
   ****       ****
   **************/
   /* SMM - Societe Malgache de Mobiles   Colour Code=1   SIM=8926102 */
   { 642,  2, NET_GSM_900, "MDG", "ANTARI", "MDG ANTARIS" },

   /**************************
   **** Macedonia (FYROM) ****
   **************************/
   /* Macedonian Telecommunications   Colour Code=2   SIM=8938901 */
   { 294,  1, NET_GSM_900, "MKD", "MOBI-M", "MKD-MOBIMAK" },

   /****************
   **** Morocco ****
   ****************/
   /* Itissalat Al-Maghrib S.A (IAM)   Colour Code=1   SIM=8921201 */
   { 604,  1, NET_GSM_900, "MOR", "IAM", "MOR IAM" },

   /*******************
   **** Mozambique ****
   *******************/
   /* Telecomunicacoes de Mocambique   Colour Code=0   SIM=8925801 */
   { 643,  1, NET_GSM_900, "MOZ", "mCel", "MOZ - mCel" },

   /******************
   **** Mauritius ****
   ******************/
   /* Cellplus Mobile Communications Ltd.   Colour Code=0   SIM=8923001 */
   { 617,  1, NET_GSM_900, "MRU", "CELL +", "CELLPLUS-MRU" },

   /***************
   **** Malawi ****
   ***************/
   /* Telekom Network Ltd   Colour Code=0   SIM=265? */
   { 650,  1, NET_GSM_900, "MW", "CP 900", "MW CP 900" },

   /*****************
   **** Malaysia ****
   *****************/
   /* Celcom/Cellular Comms. Network   Colour Code=3;7   SIM=896019 */
   { 502, 19, NET_GSM_900, "MY", "CELCOM", "MY CELCOM GSM" },
   /* Binariang Communications Sdn   Colour Code=5   SIM=896001 */
   { 502, 12, NET_GSM_900, "MY", "maxis", "MY maxis mobile" },
   /* Time Wireless Sdn Bhd   Colour Code=7   SIM=896017 */
   { 502, 17, NET_DCS_1800, "MY", "ADAM", "MY - ADAM  17" },
   /* Telekom Cellular Sdn Bhd (TMTOUCH)   Colour Code=5   SIM=896013 */
   { 502, 13, NET_DCS_1800, "MY", "MRTEL", "MY EMARTEL" },
   /* Mutiara Telecommunications Sdn   Colour Code=3   SIM=896016 */
   { 502, 16, NET_DCS_1800, "MY", "MT18", "MY DIGI 1800" },

   /***************
   **** Norway ****
   ***************/
   /* Telenor Mobil   Colour Code=3   SIM=894701 */
   { 242,  1, NET_GSM_900, "N", "TELENOR", "N Telenor" },
   /* NetCom GSM AS   Colour Code=- - -   SIM=894702 */
   { 242,  2, NET_GSM_900, "N", "N COM", "N NetCom GSM" },

   /**********************
   **** New Caledonia ****
   **********************/
   /* OPT New Caledonia   Colour Code=3   SIM=8968701 */
   { 546,  1, NET_GSM_900, "NCL", "MOBNCL", "NCL MOBILIS" },

   /********************
   **** Netherlands ****
   ********************/
   /* Libertel BV   Colour Code=4   SIM=893144 */
   { 204,  4, NET_GSM_900, "NL", "LIBTEL", "NL LIBERTEL" },
   /* Telfort Holding N.V.   Colour Code=0   SIM=893126 */
   { 204, 12, NET_DCS_1800, "NL", "TELFRT", "NL TELFORT" },
   /* PTT Telecom   Colour Code=0   SIM=893108 */
   { 204,  8, NET_GSM_900, "NL", "NL PTT", "NL PTT TELECOM" },

   /********************
   **** New Zealand ****
   ********************/
   /* BellSouth New Zealand Ltd   Colour Code=1   SIM=896401 */
   { 530,  1, NET_GSM_900, "NZ", "BSNZ", "BELLSOUTH NZ" },

   /****************************
   **** Oman (Sultanate of) ****
   ****************************/
   /* Ministry PTT   Colour Code=4   SIM=8996800 */
   { 422,  2, NET_GSM_900, "OMN", "OMAN", "OMAN MOBILE" },

   /*****************
   **** Portugal ****
   *****************/
   /* Telecel   Colour Code=3   SIM=8935101 */
   { 268,  1, NET_GSM_900, "P", "TLCL", "P TELECEL" },
   /* Main Road Telecomunicacoes S.A.   Colour Code=4   SIM=8935103 */
   { 268,  3, NET_GSM_900, "P", "OPTIM", "P OPTIMUS" },
   /* TMN   Colour Code=7   SIM=8935106 */
   { 268,  6, NET_GSM_900, "P", "TMN", "P TMN" },

   /********************
   **** Philippines ****
   ********************/
   /* Isla Communications Co. Inc.   Colour Code=1   SIM=896301 */
   { 515,  1, NET_GSM_900, "PH", "ISLA", "ISLACOM" },
   /* Globe Telecom   Colour Code=2;3   SIM=896302 */
   { 515,  2, NET_GSM_900, "PH", "GLOBE", "Globe Telecom-PH" },

   /***************
   **** Poland ****
   ***************/
   /* Polska Telefonia Cyfrowa   Colour Code=6   SIM=8948-02 */
   { 260,  2, NET_GSM_900, "PL", "ERAGSM", "PL-ERA GSM" },
   /* PTK Centertel   Colour Code=4   SIM=894803 */
   { 260,  3, NET_DCS_1800, "PL", "IDEA", "PL IDEA" },
   /* Polkomtel S.A.   Colour Code=2   SIM=894801 */
   { 260,  1, NET_GSM_900, "PL", "PLUS", "PL-PLUS" },

   /**************
   **** Qatar ****
   **************/
   /* Q-Tel   Colour Code=?   SIM=8997401 */
   { 427,  1, NET_GSM_900, "QAT", "Q-NET", "QAT-QATARNET" },

   /***************
   **** Rwanda ****
   ***************/
   /* Rwandacell SARL   Colour Code=- - -   SIM=8925010 */
   { 635, 10, NET_GSM_900, "R", "RCELL", "R-CELL" },

   /***************************
   **** Armenia (Republic) ****
   ***************************/
   /* ArmenTel   Colour Code=0   SIM=8937401 */
   { 283,  1, NET_GSM_900, "RA", "ARMMO1", "RA-ARMGSM" },

   /****************
   **** Lebanon ****
   ****************/
   /* LibanCell   Colour Code=3   SIM=8996103 */
   { 415,  3, NET_GSM_900, "RL", "LibCL", "RL LibanCell" },
   /* FTML   Colour Code=7   SIM=8996101 */
   { 415,  1, NET_GSM_900, "RL", "CLLIS", "RL Cellis" },

   /****************
   **** Romania ****
   ****************/
   /* Mobil Rom   Colour Code=3   SIM=894010 */
   { 226, 10, NET_GSM_900, "RO", "dialog", "RO dialog" },
   /* Mobifon SA   Colour Code=4   SIM=894092 */
   { 226,  1, NET_GSM_900, "RO", "CONNEX", "RO CONNEX" },

   /***************
   **** Russia ****
   ***************/
   /* Uraltel   Colour Code=- - -   SIM=- - - */
   { 250, 39, NET_GSM_900, "RUS", "UTL", "Uraltel" },
   /* Siberian Cellular Systems 900   Colour Code=4   SIM=8970105 */
   { 250,  5, NET_GSM_900, "RUS", "SCS", "SCS RUS" },
   /* North-West GSM   Colour Code=1   SIM=8970102 */
   { 250,  2, NET_GSM_900, "RUS", "NWGSM", "North-West GSM RUS" },
   /* Mobile TeleSystems (MTS)   Colour Code=3   SIM=8970101 */
   { 250,  1, NET_GSM_900, "RUS", "MTS", "MTS-RUS" },
   /* Kuban-GSM   Colour Code=1   SIM=8970113 */
   { 250, 13, NET_GSM_900, "RUS", "KUGSM", "Kuban-GSM RUS" },
   /* Far Eastern Cellular Systems - 900   Colour Code=1   SIM=8970112 */
   { 250, 12, NET_GSM_900, "RUS", "FEast", "Far East RUS" },

   /***************
   **** Sweden ****
   ***************/
   /* Europolitan AB   Colour Code=2;6   SIM=894608 */
   { 240,  8, NET_GSM_900, "S", "EURO", "S EUROPOLITAN" },
   /* COMVIQ GSM AB   Colour Code=6   SIM=894620 */
   { 240,  7, NET_GSM_900, "S", "IQ", "S COMVIQ" },
   /* Telia Mobile   Colour Code=2   SIM=894601 */
   { 240,  1, NET_GSM_900, "S", "TELIA", "TELIA S" },

   /*********************
   **** South Africa ****
   *********************/
   /* Vodacom Group Pty Ltd.   Colour Code=0   SIM=? */
   { 655,  1, NET_GSM_900, "SA", "VODA", "VodaCom-SA" },
   /* MTN - Mobile Telephone Network Ltd.   Colour Code=4   SIM=892701 */
   { 655, 10, NET_GSM_900, "SA", "MTN", "MTN-SA" },

   /**************
   **** Sudan ****
   **************/
   /* Mobitel   Colour Code=0   SIM=8924901 */
   { 634,  1, NET_GSM_900, "SDN", "SD-MOB", "SDN MobiTel" },

   /*******************
   **** Seychelles ****
   *******************/
   /* Cable & Wireless (Seychelles)   Colour Code=0   SIM=8924801 */
   { 633,  1, NET_GSM_900, "SEZ", "SEYCEL", "SEZ CELLULAR SERVICES" },
   /* Telecom Seychelles Ltd   Colour Code=3   SIM=8924810 */
   { 633, 10, NET_GSM_900, "SEZ", "AIRTEL", "SEZ AIRTEL" },

   /******************
   **** Singapore ****
   ******************/
   /* MobileOne (Asia) Pte. Ltd   Colour Code=0;5   SIM=896503 */
   { 525,  3, NET_GSM_900, "SGP", "M1-GSM", "M1-GSM-SGP" },
   /* Singapore Telecom Mobile Pte Ltd   Colour Code=1   SIM=896502 */
   { 525,  2, NET_DCS_1800, "SGP", "GSM1800", "ST-GSM1800-SGP" },
   /* Singapore Telecom Mobile Pte Ltd   Colour Code=1;4   SIM=896501 */
   { 525,  1, NET_GSM_900, "SGP", "STGSM", "ST-GSM-SGP" },

   /*****************
   **** Slovenia ****
   *****************/
   /* Mobitel D.D.   Colour Code=3   SIM=8938641 */
   { 293, 41, NET_GSM_900, "SI", "SI-GSM", "SI MOBITEL GSM" },

   /************************
   **** Slovak Republic ****
   ************************/
   /* Globtel GSM as   Colour Code=3   SIM=8942101 */
   { 231,  1, NET_GSM_900, "SK", "SVK GT", "SVK GT" },
   /* EuroTel Bratislava as   Colour Code=7   SIM=8942102 */
   { 231,  2, NET_GSM_900, "SK", "ET-SK", "EUROTEL-SK" },

   /******************
   **** Sri Lanka ****
   ******************/
   /* MTN Networks (Private) Limited   Colour Code=7   SIM=89???? */
   { 413,  2, NET_GSM_900, "SRI", "DALOG", "SRI DIALOG" },

   /**************
   **** Syria ****
   **************/
   /* Syrian Telecommunication Est.   Colour Code=?   SIM=8996309 */
   { 417,  9, NET_GSM_900, "SYR", "MOBILE", "SYR MOBILE SYR" },

   /*****************
   **** Thailand ****
   *****************/
   /* Advanced Info Service PLC   Colour Code=1   SIM=896601 */
   { 520,  1, NET_GSM_900, "TH", "TH AIS", "TH AIS GSM" },
   /* Wireless Com. Services Co. (WCS)   Colour Code=6   SIM=896610 */
   { 520, 10, NET_DCS_1800, "TH", "TH WCS", "TH WCS" },
   /* Digital Phone Co Ltd   Colour Code=6   SIM=8952023 */
   { 520, 23, NET_DCS_1800, "TH", "HELLO", "TH-HELLO" },

   /***************
   **** Turkey ****
   ***************/
   /* Turk Telekom   Colour Code=2   SIM=899001 */
   { 286,  1, NET_GSM_900, "TR", "TCELL", "TR PTT/TURKCELL GSM" },
   /* Turk Telekom   Colour Code=6   SIM=899002 */
   { 286,  2, NET_GSM_900, "TR", "TLSIM", "TR TELSIM" },

   /***************
   **** Taiwan ****
   ***************/
   /* Chunghwa Telecom LDM   Colour Code=0   SIM=8988600 */
   { 466, 92, NET_GSM_900, "ROC", "LDGSM", "ROC LDTA GSM" },
   /* MobiTai   Colour Code=2   SIM=8988693 */
   { 466, 93, NET_GSM_900, "TWN", "TW MOB", "TWN MOBITAI" },
   /* Pacific Cellular Corporation Ltd   Colour Code=7   SIM=8988697 */
   { 466, 97, NET_DCS_1800, "TWN", "PCC", "TWN GSM 1800" },
   /* Far EasTone Telecommunications Co Ltd   Colour Code=2   SIM=8988601 */
   { 466,  1, NET_GSM_900, "TWN", "FET", "Far EasTone" },
   /* Tuntex Telecom   Colour Code=6   SIM=8988606 */
   { 466,  6, NET_DCS_1800, "TWN", "TUNTEX", "TWN Tuntex GSM 1800" },
   /* KG Telecom   Colour Code=1   SIM=8988688 */
   { 466, 88, NET_DCS_1800, "TWN", "KGT", "KGT-Online" },

   /*****************
   **** Tanzania ****
   *****************/
   /* Tri Telecommunication (T) Ltd.   Colour Code=0   SIM=8925501 */
   { 640,  1, NET_GSM_900, "TZ", "TRITEL", "Tritel - TZ" },

   /*****************
   **** Test Nets****
   *****************/
   /* Test RACAL PCS 1900            Colour Code=0            */
   {   1,  1, NET_PCS_1900, "TEST", "TEST 01", "TEST NET 01" },
   {   1,  2, NET_PCS_1900, "TEST", "TEST 02", "TEST NET 02" },
   {   1,  3, NET_PCS_1900, "TEST", "TEST 03", "TEST NET 03" },
   {   1,  4, NET_PCS_1900, "TEST", "TEST 04", "TEST NET 04" },
   {   1,  5, NET_PCS_1900, "TEST", "TEST 05", "TEST NET 05" },
   {   1,  6, NET_PCS_1900, "TEST", "TEST 06", "TEST NET 06" },
   {   1,  7, NET_PCS_1900, "TEST", "TEST 07", "TEST NET 07" },
   {   1,  8, NET_PCS_1900, "TEST", "TEST 08", "TEST NET 08" },
   {   1,  9, NET_PCS_1900, "TEST", "TEST 09", "TEST NET 09" },
   {   1, 10, NET_PCS_1900, "TEST", "TEST 10", "TEST NET 10" },
   {   1,100, NET_PCS_1900, "TEST", "TEST 100","TEST NET 100" },
   /* Test ANITE/ROHDE PCS 1900            Colour Code=0            */
   {   1,  11, NET_PCS_1900, "TEST", "TEST 11", "TEST NET 11" },
   {   1, 110, NET_PCS_1900, "TEST", "TEST 110","TEST NET 110" },

   /****************
   **** Ukraine ****
   ****************/
   /* Kyivstar GSM JSC   Colour Code=5   SIM=8938003 */
   { 255,  3, NET_GSM_900, "UA", "UA-KS", "UA-KYIVSTAR" },
   /* Ukrainian Mobile Communications   Colour Code=5   SIM=8938001 */
   { 255,  1, NET_GSM_900, "UA", "UMC", "UA UMC" },
   /* Bancomsvyaz   Colour Code=1   SIM=8938005 */
   { 255,  5, NET_DCS_1800, "UA", "GT-BCS", "UA - GOLDEN TELECOM" },

   /*****************************
   **** United Arab Emirates ****
   *****************************/
   /* Emirates Telecom Corp-ETISALAT   Colour Code=6   SIM=899711 */
   { 424,  2, NET_GSM_900, "UAE", "ETSLT", "UAE ETISALAT" },

   /***************
   **** Uganda ****
   ***************/
   /* MTN Uganda Ltd   Colour Code=- - -   SIM=8925610 */
   { 641, 10, NET_GSM_900, "UG", "MTN-UG", "MTN-UGANDA" },

   /***********************
   **** United Kingdom ****
   ***********************/
   /* Guernsey Telecoms   Colour Code=1   SIM=894433 */
   { 234, 55, NET_GSM_900, "UK", "GSY-TEL", "GUERNSEY TEL UK" },
   /* Cellnet   Colour Code=2   SIM=894411 */
   { 234, 10, NET_GSM_900, "UK", "CLNET", "UK CELLNET" },
   /* VODAFONE Ltd   Colour Code=6   SIM=894410 */
   { 234, 15, NET_GSM_900, "UK", "VODA", "UK VODAFONE" },
   /* Jersey Telecommunications   Colour Code=5   SIM=894450 */
   { 234, 50, NET_GSM_900, "UK", "JT GSM", "JERSEY TELECOMS GSM UK" },
   /* Manx Telecom   Colour Code=1   SIM=894453 */
   { 234, 58, NET_GSM_900, "UK", "MANX", "MANX TELECOM PRONTO GSM" },
   /* One-2-One   Colour Code=2   SIM=894430 */
   { 234, 30, NET_DCS_1800, "UK", "ONE2ONE", "ONE 2 ONE" },
   /* Orange PCS Ltd   Colour Code=5   SIM=894412 */
   { 234, 33, NET_DCS_1800, "UK", "ORANGE", "ORANGE" },

   /****************
   **** Ukraine ****
   ****************/
   /* Ukrainian Radio Systems   Colour Code=1   SIM=8938002 */
   { 255,  2, NET_GSM_900, "UKR", "FLASH", "FLASH-UKR" },

   /**************
   **** U.S.A ****
   **************/
   /* DigiPH   Colour Code=- - -   SIM=- - - */
   { 310, TBA,  NET_PCS_1900, "US", "", "" },
   /* Aerial Communications   Colour Code=1   SIM=8901310 */
   { 310, 31,  NET_PCS_1900, "USA", "AERIAL", "USA-AERIAL" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 200, NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Powertel PCS Partners   Colour Code=- - -   SIM=8931027 */
   { 310, 270, NET_PCS_1900, "USA", "USA27", "USA Powertel Inc." },
   /* BellSouth Mobility DCS   Colour Code=0   SIM=8901150 */
   { 310, 150, NET_PCS_1900, "USA", "BSMDCS", "USA 150" },
   /* Omnipoint Corporation   Colour Code=   SIM= */
   { 310, 160, NET_PCS_1900, "USA", "Omni", "USA Omnipoint" },
   /* Third Kentucky Cellular Corporation   Colour Code=3   SIM=98310011 */
   { 310, 11,  NET_PCS_1900, "USA", "WTTCKy", "USA Wireless 2000 Telepho" },
   /* Pacific Bell */
        { 310, 17,  NET_PCS_1900, "USA", "PACBEL", "PAC BELL" },
        /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 260, NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 250, NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 240, NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 230, NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 220, NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 210, NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },

   /*****************************************************
   **** U.S.A again, alternative 2/3 digit net codes ****
   *****************************************************/
   /* DigiPH   Colour Code=- - -   SIM=- - - */
   { 310, TBA,  NET_PCS_1900, "US", "", "" },
   /* Aerial Communications   Colour Code=1   SIM=8901310 */
   { 310, 310, NET_PCS_1900, "USA", "AERIAL", "USA-AERIAL" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 20,  NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Powertel PCS Partners   Colour Code=- - -   SIM=8931027 */
   { 310, 27,  NET_PCS_1900, "USA", "USA27", "USA Powertel Inc." },
   /* BellSouth Mobility DCS   Colour Code=0   SIM=8901150 */
   { 310, 15,  NET_PCS_1900, "USA", "BSMDCS", "USA 150" },
   /* Omnipoint Corporation   Colour Code=   SIM= */
   { 310, 16,  NET_PCS_1900, "USA", "Omni", "USA Omnipoint" },
   /* Pacific Bell */
        { 310, 170,  NET_PCS_1900, "USA", "PACBEL", "PAC BELL" },
   /* Third Kentucky Cellular Corporation   Colour Code=3   SIM=98310011 */
   { 310, 110, NET_PCS_1900, "USA", "WTTCKy", "USA Wireless 2000 Telepho" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 26,  NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 25,  NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 24,  NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 23,  NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 22,  NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },
   /* Western Wireless Corporation   Colour Code=0   SIM=8901 */
   { 310, 21,  NET_PCS_1900, "USA", "WWC", "VOICESTREAM USA" },

   /*******************
   **** Uzbekistan ****
   *******************/
   /* Coscom   Colour Code=5   SIM=895005 */
   { 434,  5, NET_GSM_900, "UZB", "COSCOM", "UZB CSOCOM GSM" },
   /* Daewoo Central Paging Co   Colour Code=4   SIM=8999804 */
   { 434,  4, NET_GSM_900, "UZB", "DW-GSM", "UZB DAEWOO-GSM" },

   /*******************
   ****            ****
   *******************/
   /* MobiFone Comvik VMS   Colour Code=0   SIM=(?) */
   { 452,  1, NET_GSM_900, "VN", "MOBIF", "VN Mobi Fone" },

   /******************
   **** Venezuela ****
   ******************/
   /* Infonet Redes de Inormacion CA   Colour Code=- - -   SIM=895801 */
   { 734,  1, NET_GSM_900, "VZ", "INFONT", "INFONET-VZ" },

   /*****************************
   **** Yugoslavia (Fed Rep) ****
   *****************************/
   /* Mobile Telecommunications   Colour Code=3   SIM=8938101 */
   { 220,  1, NET_GSM_900, "YU", "MOBTEL", "YU MOBTEL" },

   /*****************
   **** Zimbabwe ****
   *****************/
   /* Posts & Telecommunications   Colour Code=1   SIM=8926301 */
   { 648,  1, NET_GSM_900, "ZW", "NET*1", "ZW NET*ONE" },

   /********************
   **** END OF LIST ****
   ********************/
   { TBA, TBA, NET_UNKNOWN_TYPE, "end", "end", "end" },


};
#else
extern const network_name_T network_names[];
#endif

#endif





