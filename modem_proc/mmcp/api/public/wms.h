#ifndef WMS_H
#define WMS_H

/**
  @file wms.h
  @brief Wireless Messaging Services (WMS) Application Programming Interface 
  (API).

  The WMS API provides the client software with the ability to send and receive 
  point-to-point Short Message Service (SMS) or Enhanced Messaging Service (EMS) 
  messages, to receive broadcast in a multimode environment (cdmaOne, 
  cdma2000, 1xEV, GSM/GPRS, and WCDMA).
*/
/*=========================================================================== 
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The WMS_API_mainpage.dox file contains all file/group descriptions that 
      are in the output PDF generated using Doxygen and LaTeX. To edit or 
      update any of the file/group text in the PDF, edit the 
      WMS_API_mainpage.dox file orf contact Tech Pubs.

===========================================================================*/


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W i r e l e s s    M e s s a g i n g    S e r v i c e s

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/wms.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "sys.h"

/* <EJECT> */
/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/


/* ========================================================================= */
/* ========================== Client group data types ====================== */
/* ========================================================================= */

/** @addtogroup client_group_data_types
@{ */
/** Client types.
*/
typedef enum
{
  WMS_CLIENT_TYPE_JAVA = 0, /**< Java. */
  WMS_CLIENT_TYPE_BREW,     /**< Brew@tm telephone API. */
  WMS_CLIENT_TYPE_WAP_MMS,  
  /**< Wireless Application Protocol/more-messages_to_send browser. */
  WMS_CLIENT_TYPE_WMS_APP,  /**< Messaging application. */
  WMS_CLIENT_TYPE_UI,       /**< Softkey UI and Core applications. */
  WMS_CLIENT_TYPE_CAT,      /**< Card Application Toolkit module (e.g., GSTK). */
  WMS_CLIENT_TYPE_ATCOP,    /**< AT command processor. */
  WMS_CLIENT_TYPE_DIAG,     /**< Diagnostic task. */
  WMS_CLIENT_TYPE_GPS,      /**< GPS application. */
  WMS_CLIENT_TYPE_GSM1x,    /**< GSM1x application. */
  WMS_CLIENT_TYPE_WINCE,    /**< WinCE applications. */
  WMS_CLIENT_TYPE_PBM,      /**< Phonebook manager. */
  WMS_CLIENT_TYPE_IMS,      /**< IMS/MMD/AIMS. */
  WMS_CLIENT_TYPE_RIL,      /**< Radio interface layer. */
  WMS_CLIENT_TYPE_BREW_MP,  /**< Brew MP@tmns. */
  WMS_CLIENT_FLOATING1,     /**< Floating type 1. */
  WMS_CLIENT_FLOATING2,     /**< Floating type 2. */
  WMS_CLIENT_TYPE_INTERNAL, /**< Internal use only. */
  WMS_CLIENT_TYPE_QMI,      /**< Qualcomm Modem Interface. */
  WMS_CLIENT_TYPE_MFLO,     /**< MediaFlo. */
  WMS_CLIENT_TYPE_IMS_APP,  /**< IMSAPP */
/** @cond */
  WMS_CLIENT_TYPE_MAX,
  WMS_CLIENT_TYPE_MAX32 = 0x10000000   /* pad to 32 bit */
/** @endcond */
} wms_client_type_e_type;
/*~ SENTINEL wms_client_type_e_type.WMS_CLIENT_TYPE_MAX */

/** Client CDMA Channel Pref.
*/
typedef enum
{
  WMS_CLIENT_CH_NONE,                  /**< No channel enable. */
  WMS_CLIENT_CH_PREF_ACH_ONLY,         /**< Access Channel Only . */
  WMS_CLIENT_CH_PREF_TCH_ONLY,         /**< Traffic Channel Only. */
  WMS_CLIENT_CH_PREF_ACH_OR_TCH,       /**< Access OR Traffic Channel. */
/** @cond 
*/
  WMS_CLIENT_CH_PREF_MAX, 
  WMS_CLIENT_CH_PREF_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_client_ch_pref_e_type;
/*~ SENTINEL wms_client_ch_pref_e_type.WMS_CLIENT_CH_PREF_MAX */
/** @} */ /* end_addtogroup client_group */


/** @addtogroup client_group
@{ */
/** Client ID.
*/
typedef uint8 wms_client_id_type;


/** @cond */
enum{ WMS_DUMMY_CLIENT_ID = 0xFF };
/** @endcond */

/** @} */ /* end_addtogroup client_group_data_types */


/* <EJECT> */

/* ========================================================================= */
/* ========================== Common Data Types ============================ */
/* ========================================================================= */
/** @addtogroup common_type_defs_group
@{ */
/** API version. Not currently used.
*/
#define WMS_API_VERSION           0x0500

/** Maximum size of the SIM MWI data including the status mask of MWI type. */
#define WMS_SIM_MWI_DATA_REC_LEN_MAX     ((int)WMS_GW_MSG_WAITING_KIND_MAX + 1)
/** @} */ /* end_addtogroup common_type_defs_group */

/** Port which is not used for filtering 
*/
#define WMS_PORT_MATCH_IGNORED 0xFFFFFFFF

/* TIMER F VALUE FROM IMS */
#define TIMER_F_TIMEOUT             408

/** @addtogroup common_type_defs_group
@{ */
/** @anonenum{WMS_MAX_LEN} 
  Maximum length; typically, the number of bytes. Value: 255. \n
*/
enum { WMS_MAX_LEN                = 255 };

/** @anonenum{WMS_ADDRESS_MAX} 
  Maximum number of bytes for an address; value: 48. \n
*/
enum { WMS_ADDRESS_MAX            = 48  };

/** @anonenum{WMS_SUBADDRESS_MAX} 
  Maximum number of bytes for a subaddress; value: 48. \n
*/
enum { WMS_SUBADDRESS_MAX         = 48  };

/** @anonenum{WMS_GW_ADDRESS_MAX} 
  Maximum number of bytes for a GSM/WCDMA address; value: 20. \n
*/
enum { WMS_GW_ADDRESS_MAX         = 20  };

/** @anonenum{WMS_CDMA_USER_DATA_MAX} 
  Maximum number of bytes for CDMA user data; value: 229. \n
*/
enum { WMS_CDMA_USER_DATA_MAX     = 229 };

/** @anonenum{WMS_GW_USER_DATA_MAX} 
  Maximum number of bytes for GSM/WCDMA user data; value: 160. \n
*/
enum { WMS_GW_USER_DATA_MAX     = 160 };

/** @anonenum{WMS_MESSAGE_LIST_MAX} 
  Maximum number of messages; value: 255. \n
*/
enum { WMS_MESSAGE_LIST_MAX     = 255 };

/** @anonenum{WMS_MAX_TEMPLATES} 
  Maximum number of templates; value: 10. \n
*/
enum { WMS_MAX_TEMPLATES        = 10 };

/** @anonenum{WMS_GW_COMMAND_DATA_MAX} 
  Maximum number of bytes for GSM/WCDMA command data; value: 157. \n
*/
enum { WMS_GW_COMMAND_DATA_MAX  = 157 };

// enum { WMS_MAX_LONG_LEN         = 39015 /* 255*153 */ };

/** @anonenum{WMS_ALPHA_ID_MAX} 
  Maximum number of bytes for an Alphabetic ID; value: 255. \n
*/
enum { WMS_ALPHA_ID_MAX         = 255 };

/** @anonenum{WMS_MAX_UD_HEADERS} 
  Maximum number of user data headers; value: 7. \n
*/
enum { WMS_MAX_UD_HEADERS       = 7};

/* ----- UDH MAX sizes ----- */
/** @anonenum{WMS_UDH_MAX_8BIT} 
  Maximum size for UDH if the data encoding is an 8-bit alphabet; 
  value: 134. \n
*/
enum { WMS_UDH_MAX_8BIT              = 134};

/** @anonenum{WMS_UDH_MAX_7BIT} 
  Maximum size for UDH if the data encoding is the default 7-bit alphabet; 
  value: 153. \n
*/
enum { WMS_UDH_MAX_7BIT              = 153};

/** @anonenum{WMS_UDH_OTHER_SIZE} 
  Maximum size for UDH if the data encoding is other than a 7-bit or 8-bit 
  alphabet; value: 226. \n
*/
enum { WMS_UDH_OTHER_SIZE            = 226 };

/** @anonenum{WMS_UDH_MAX_SND_SIZE} 
  Maximum size of user data that can be sent in a part, excluding the UDH 
  information; value: 128. \n
*/
enum { WMS_UDH_MAX_SND_SIZE          = 128 };

/* 229 (CDMA MAX USER DATA LEN) - 1(UDHL) - 1(UDH ID) - 1(UDH Len) */

/* ----- Picture definitions ----- */
/** @anonenum{WMS_UDH_LARGE_PIC_SIZE} 
  Size of a large picture that can be sent in an EMS part; value: 128. \n
*/
enum { WMS_UDH_LARGE_PIC_SIZE    = 128 };

/** @anonenum{WMS_UDH_SMALL_PIC_SIZE} 
  Size of a small picture that can be sent in an EMS part; value: 32. \n
*/
enum { WMS_UDH_SMALL_PIC_SIZE    = 32  };

/** @anonenum{WMS_UDH_VAR_PIC_SIZE} 
  Maximum size of a variable picture that can be sent in an EMS part; 
  value: 134. \n
*/
enum { WMS_UDH_VAR_PIC_SIZE      = 134 };

/* 140 - 1(UDHL) - 5(UDH) */ 

/* ----- Animation definitions ----- */
/** @anonenum{WMS_UDH_ANIM_NUM_BITMAPS} 
  Number of animation bitmaps that can be sent in an EMS part; value: 4. \n
*/
enum { WMS_UDH_ANIM_NUM_BITMAPS  = 4 };

/** @anonenum{WMS_UDH_LARGE_BITMAP_SIZE} 
  Bitmap size of a large animation; value: 32. \n
*/
enum { WMS_UDH_LARGE_BITMAP_SIZE = 32 };

/** @anonenum{WMS_UDH_SMALL_BITMAP_SIZE} 
  Bitmap size of a small animation; value: 8. \n
*/
enum { WMS_UDH_SMALL_BITMAP_SIZE = 8 };

/** @anonenum{WMS_MAX_CMAE_RECORDS}
  Maximum number of records in the CDMA Commercial Mobile Alert E interface 
  message; value: 3. 
*/
enum { WMS_MAX_CMAE_RECORDS = 3 };

/** @anonenum{WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX}
  Maximum size of sender address in transport MWI information 
  value: 200. 
*/
enum { WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX = 200 };

/** @anonenum{WMS_TRANSPORT_MWI_DATE_LENGTH_MAX}
  Maximum size of timestampin transport MWI information; value: 50. 
*/
enum { WMS_TRANSPORT_MWI_DATE_LENGTH_MAX = 50 };
/** @} */ /* end_addtogroup common_type_defs_group */

/** @addtogroup common_type_defs_group
@{ */
/** Digit mode indicator for the address parameters.
*/
typedef enum
{
  WMS_DIGIT_MODE_4_BIT     = 0, /**< Four-bit DTMF digit codes. */
  WMS_DIGIT_MODE_8_BIT     = 1, /**< Eight-bit DTMF digit codes. */
/** @cond */
  WMS_DIGIT_MODE_MAX32     = 0x10000000
/** @endcond */
} wms_digit_mode_e_type;


/** Number type (originating or destination) for the SMS or EMS message. 
  The first seven types are not data network addresses. The last two types are 
  data network addresses. In the address data digits, each byte contains an ASCII 
  character, e.g., x\@y.com,a\@b.com (refer to TIA/EIA-637A 3.4.3.3). 

  @sa #wms_digit_mode_e_type, #wms_number_mode_e_type, #wms_address_s
*/
typedef enum
{
  WMS_NUMBER_UNKNOWN        = 0,  /**< Unknown. */
  WMS_NUMBER_INTERNATIONAL  = 1,  /**< International. */
  WMS_NUMBER_NATIONAL       = 2,  /**< National. */
  WMS_NUMBER_NETWORK        = 3,  /**< Network. */
  WMS_NUMBER_SUBSCRIBER     = 4,  /**< Subscriber. */
  WMS_NUMBER_ALPHANUMERIC   = 5,  /**< For GSM SMS, the address value is 
                                       GSM 7-bit characters. */
  WMS_NUMBER_ABBREVIATED    = 6,  /**< Abbreviated. */
  WMS_NUMBER_RESERVED_7     = 7,  /**< Reserved. */
  
  WMS_NUMBER_DATA_IP        = 1,  /**< Data IP. */
  WMS_NUMBER_INTERNET_EMAIL = 2,  /**< Internet email. @newpage */
/** @cond */ 
  WMS_NUMBER_MAX32        = 0x10000000
/** @endcond */
} wms_number_type_e_type;


/** Numbering plan used for the address.
  This enumeration is used only if digit_mode is 8-bit and number_mode is 
  WMS_NUMBER_MODE_NONE_DATA_NETWORK.
*/
typedef enum
{
  WMS_NUMBER_PLAN_UNKNOWN     = 0,  /**< Unknown. */
  WMS_NUMBER_PLAN_TELEPHONY   = 1,  /**< As defined in CCITT E.164 and E.163, 
                                         including the ISDN plan. */
  WMS_NUMBER_PLAN_RESERVED_2  = 2,  /**< Reserved. */
  WMS_NUMBER_PLAN_DATA        = 3,  /**< As defined in CCITT X.121. */
  WMS_NUMBER_PLAN_TELEX       = 4,  /**< As defined in CCITT F.69. */
  WMS_NUMBER_PLAN_RESERVED_5  = 5,  /**< Reserved. */
  WMS_NUMBER_PLAN_RESERVED_6  = 6,  /**< Reserved. */ 
  WMS_NUMBER_PLAN_RESERVED_7  = 7,  /**< Reserved. */
  WMS_NUMBER_PLAN_RESERVED_8  = 8,  /**< Reserved. */
  WMS_NUMBER_PLAN_PRIVATE     = 9,  /**< Private. */
  WMS_NUMBER_PLAN_RESERVED_10 = 10, /**< Reserved. */
  WMS_NUMBER_PLAN_RESERVED_11 = 11, /**< Reserved. */
  WMS_NUMBER_PLAN_RESERVED_12 = 12, /**< Reserved. */
  WMS_NUMBER_PLAN_RESERVED_13 = 13, /**< Reserved. */
  WMS_NUMBER_PLAN_RESERVED_14 = 14, /**< Reserved. */
  WMS_NUMBER_PLAN_RESERVED_15 = 15, /**< Reserved. */
/** @cond */
  WMS_NUMBER_PLAN_MAX32       = 0x10000000
/** @endcond */
} wms_number_plan_e_type;


/** Number mode that indicates whether the address type is a data network 
  address.
*/
typedef enum
{
  WMS_NUMBER_MODE_NONE_DATA_NETWORK      = 0, /**< Not a data network address. */
  WMS_NUMBER_MODE_DATA_NETWORK           = 1, /**< Data network address. */
/** @cond */
  WMS_NUMBER_MODE_DATA_NETWORK_MAX32     = 0x10000000
/** @endcond */
} wms_number_mode_e_type;
/** @} */ /* end_addtogroup common_type_defs_group */


/** @ingroup common_type_defs_group
  Holds the address parameters for sending an SMS or EMS message.
*/
typedef struct wms_address_s
{
  wms_digit_mode_e_type          digit_mode;
    /**< Indicates 4 bit or 8 bit. */
  wms_number_mode_e_type         number_mode;
    /**< Used in CDMA only. It is meaningful only when digit_mode is 8 bit. */
  wms_number_type_e_type         number_type;
    /**< For a CDMA address, this is used only when digit_mode is 8 bit. \n 
         To specify an international address for CDMA, use the following 
         settings:
          - Digit_mode = 8-bit
          - Number_mode = NONE_DATA_NETWORK
          - Number_type = INTERNATIONAL
          - Number_plan = TELEPHONY
          - Number_of_digits = Number of digits
          - Digits = ASCII digits, e.g., 1, 2, 3, 4, and 5 @tablebulletend
    */

  wms_number_plan_e_type         number_plan;
    /**< For a CDMA address, this is used only when digit_mode is 8 bit. */
  uint8                          number_of_digits;
    /**< Number of bytes in the digits array. */
  uint8                          digits[ WMS_ADDRESS_MAX ];
    /**< Each byte in this array represents a 4-bit or 8-bit digit of address data.
         Each digit must be a valid standardized DTMF digit code:
      @verbatim
      Hex 4/8-bit             Hex 4/8-bit
      DTMF digit     ASCII    DTMF digit     ASCII
      -----------   -------   -----------   -------
       0x0/0x00     Invalid    0x8/0x08        8
       0x1/0x01        1       0x9/0x09        9
       0x2/0x02        2       0xA/0x0A        0
       0x3/0x03        3       0xB/0x0B        *
       0x4/0x04        4       0xC/0x0C        #
       0x5/0x05        5       0xD/0x0D     Invalid
       0x6/0x06        6       0xE/0x0E     Invalid
       0x7/0x07        7       0xF/0x0F     Invalid
      @endverbatim
    */
} wms_address_s_type;


/** @ingroup common_type_defs_group
  Subaddress types.
*/
typedef enum
{
  WMS_SUBADDRESS_NSAP           = 0,  /**< Network Service Access Protocol 
                                           (CCITT X.213 or ISO 8348 AD2). */
  WMS_SUBADDRESS_USER_SPECIFIED = 1,  /**< For example, X.25. @newpage */
/** @cond */ 
  WMS_SUBADDRESS_MAX32          = 0x10000000
/** @endcond */
} wms_subaddress_type_e_type;


/** @ingroup common_type_defs_group
  Holds the subaddress parameters.
*/
typedef struct wms_subaddress_s
{
  wms_subaddress_type_e_type           type;
    /**< Type of subaddress. */
  boolean                              odd; 
    /**< TRUE means the last byte's lower four bits are to be ignored. */
  uint8                                number_of_digits;
    /**< Number of bytes in the digits array. */
  uint8                                digits[ WMS_SUBADDRESS_MAX ];
    /**< Each byte in this array represents an 8-bit digit of subaddress data. */
} wms_subaddress_s_type;


/** @ingroup common_type_defs_group
  Message status tags.
  The values assigned here match those values in the SIM or R-UIM card.
*/
typedef enum
{
  WMS_TAG_NONE        = 0x00,
  /**< No status. */
  WMS_TAG_MT_READ     = 0x01,
  /**< MT is read. */
  WMS_TAG_MT_NOT_READ = 0x03,
  /**< MT is not read. */
  WMS_TAG_MO_SENT     = 0x05,
  /**< MO is sent; no status report is requested. */
  WMS_TAG_MO_NOT_SENT = 0x07,
  /**< MO is not sent. */
  WMS_TAG_MO_SENT_ST_NOT_RECEIVED = 0x0D,
  /**< MO is sent and status is not received; GSM/WCDMA only. */
  WMS_TAG_MO_SENT_ST_NOT_STORED   = 0x15,
  /**< MO is sent and status is not stored; GSM/WCDMA only. */
  WMS_TAG_MO_SENT_ST_STORED       = 0x1D,
  /**< MO is sent and status is stored; GSM/WCDMA only. */
  WMS_TAG_MO_TEMPLATE = 0x100,
  /**< SMS parameter. */
  WMS_TAG_STATUS_RPT  = 0x200,
  /**< Short message status report parameter. */
/** @cond */
  WMS_TAG_MAX32       = 0x10000000
/** @endcond */
} wms_message_tag_e_type;


/** @addtogroup common_type_defs_group
@{ */
/** Macro that checks if the tag represents a mobile-originated message. */
#define WMS_IS_MO( tag )           \
  ( ( (wms_message_tag_e_type)((int)(tag) & 0x7) ) == WMS_TAG_MO_SENT ||  \
    (tag) == WMS_TAG_MO_NOT_SENT ||  \
    (tag) == WMS_TAG_MO_TEMPLATE || \
    (tag) == WMS_TAG_MO_SENT_ST_NOT_RECEIVED || \
    (tag) == WMS_TAG_MO_SENT_ST_NOT_STORED ||   \
    (tag) == WMS_TAG_MO_SENT_ST_STORED )

/** Macro that checks if the tag represents a mobile-terminated message. */
#define WMS_IS_MT( tag )           \
  ( (tag) == WMS_TAG_MT_READ ||      \
    (tag) == WMS_TAG_MT_NOT_READ )

/** Macro that checks if the tag is valid. */
#define WMS_IS_VALID_TAG( tag )    \
  ( (tag) == WMS_TAG_NONE ||         \
    (tag) == WMS_TAG_MT_READ ||      \
    (tag) == WMS_TAG_MT_NOT_READ ||  \
    (tag) == WMS_TAG_MO_SENT ||      \
    (tag) == WMS_TAG_MO_NOT_SENT ||  \
    (tag) == WMS_TAG_MO_SENT_ST_NOT_RECEIVED || \
    (tag) == WMS_TAG_MO_SENT_ST_NOT_STORED ||   \
    (tag) == WMS_TAG_MO_SENT_ST_STORED ||       \
    (tag) == WMS_TAG_MO_TEMPLATE ||  \
    (tag) == WMS_TAG_STATUS_RPT )
/** @} */ /* end_addtogroup common_type_defs_group */

/** Macro that checks for EFsms record empty. */
#define WMS_IS_EFSMS_REC_EMPTY( tag ) \
((wms_message_tag_e_type)(tag & 0x01)) == WMS_TAG_NONE
/** @} */ /* end_addtogroup common_data_types_group */

/** @ingroup common_type_defs_group
  Timestamp structure.

  If the year is between 96 and 99, the actual year is 1900 + year. If the year 
  is between 00 and 95, the actual year is 2000 + year.
  
  @note1 Each field has two BCD digits. The byte arrangement is <MSB...LSB>. 
*/
typedef struct
{
  uint8      year;     /**< 00 through 99. */
  uint8      month;    /**< 01 through 12. */
  uint8      day;      /**< 01 through 31. */
  uint8      hour;     /**< 00 through 23. */
  uint8      minute;   /**< 00 through 59. */
  uint8      second;   /**< 00 through 59. */
  sint7      timezone; /**< +/-, [-48,+48] number of 15 minutes; GSM/WCDMA only. */
} wms_timestamp_s_type;

/** @ingroup common_type_defs_group
  TP Validity Period (TP-VP) enhanced format validity format tag.
*/
typedef enum
{
  WMS_ENHANCED_VP_NONE         = 0x00,     /**< None. */
  WMS_ENHANCED_VP_RELATIVE     = 0x01,     /**< Relative. */
  WMS_ENHANCED_VP_RELATIVE_INT = 0x02,     /**< Relative integer. */
  WMS_ENHANCED_VP_SEMI_OCTET   = 0x03,     /**< Semi-octet. */
  /** @cond */
  WMS_ENHANCED_VP_MAX32        = 0x10000000 
  /** @endcond */
}wms_enhanced_vp_format_e_type;

/** @ingroup common_type_defs_group
  Enhanced validity period. 
*/
typedef struct
{
  boolean                       with_extension;
  /**< Whether octet2 is an extension of octet1. */
  boolean                       single_shot;
  /**< Whether the SC is required to make up to one delivery attempt.  */
  uint8                         extension;
  /**< Extended functionality indicator. */
  wms_enhanced_vp_format_e_type vp_format;
  /**< Validity period format. */
  wms_timestamp_s_type          time;
  /**< Timestamp. @newpagetable */ 
} wms_enhanced_vp_s_type;

/** @addtogroup common_type_defs_group
@{ */
/** Language types.
*/
typedef enum
{
  WMS_LANGUAGE_UNSPECIFIED = 0, /**< Unspecified. */
  WMS_LANGUAGE_ENGLISH,         /**< English. */
  WMS_LANGUAGE_FRENCH,          /**< French. */
  WMS_LANGUAGE_SPANISH,         /**< Spanish. */
  WMS_LANGUAGE_JAPANESE,        /**< Japanese. */
  WMS_LANGUAGE_KOREAN,          /**< Korean. */
  WMS_LANGUAGE_CHINESE,         /**< Chinese. */
  WMS_LANGUAGE_HEBREW,          /**< Hebrew. @newpage */
/** @cond */
  WMS_LANGUAGE_MAX32       = 0x10000000
/** @endcond */
} wms_language_e_type;


/** System mode (CDMA or GSM/WCDMA) to be used for a message.
*/
typedef enum
{
  WMS_MESSAGE_MODE_CDMA = 0, /**< CDMA. */
  WMS_MESSAGE_MODE_GW,       /**< GSM/WCDMA. */
/** @cond */
  WMS_MESSAGE_MODE_MAX,
  WMS_MESSAGE_MODE_MAX32 = 0x100000
/** @endcond */
} wms_message_mode_e_type;


/** Transport Service data formats.
*/ 
typedef enum
{
  WMS_FORMAT_CDMA         = 0,  /**< CDMA format, IS-95. */
  WMS_FORMAT_ANALOG_CLI,        /**< Analog calling line identification, IS-91. */
  WMS_FORMAT_ANALOG_VOICE_MAIL, /**< Analog voice mail, IS-91. */
  WMS_FORMAT_ANALOG_SMS,        /**< Analog SMS, IS-91. */
  WMS_FORMAT_ANALOG_AWISMS,     /**< Analog IS-95 alert with information SMS. */
  WMS_FORMAT_MWI,               /**< Message waiting indication as voice mail. */
  WMS_FORMAT_GW_PP,             /**< GSM/WCDMA point-to-point SMS. */
  WMS_FORMAT_GW_CB,             /**< GSM/WCDMA cell broadcast SMS. */
/** @cond */
  WMS_FORMAT_MAX,
  WMS_FORMAT_MAX32        = 0x10000000
/** @endcond */
} wms_format_e_type;


/** Specific requirements for sending a message.
*/
typedef enum
{
  WMS_SEND_MODE_CLIENT_MESSAGE = 0, /**< Client message. */
  WMS_SEND_MODE_MEMORY_STORE,       /**< Memory storage. */
/** @cond */
  WMS_SEND_MODE_MAX,
  WMS_SEND_MODE_MAX32 = 0x10000000  /* Pad to 32-bit int */
/** @endcond */
} wms_send_mode_e_type;


/** Specific requirements for storing a message.
*/
typedef enum
{
  WMS_WRITE_MODE_INSERT = 0, /**< Insert. */
  WMS_WRITE_MODE_REPLACE,    /**< Replace. @newpage */
/** @cond */
  WMS_WRITE_MODE_MAX,
  WMS_WRITE_MODE_MAX32  = 0x10000000
/** @endcond */
} wms_write_mode_e_type;


/** Message classes in the data coding scheme of a TPDU or cell broadcast 
  message.
*/
typedef enum
{
  WMS_MESSAGE_CLASS_0 = 0, /**< Class 0. */
  WMS_MESSAGE_CLASS_1,     /**< Class 1. */
  WMS_MESSAGE_CLASS_2,     /**< Class 2. */
  WMS_MESSAGE_CLASS_3,     /**< Class 3. */
  WMS_MESSAGE_CLASS_NONE,  /**< No class. */
  WMS_MESSAGE_CLASS_CDMA,  /**< CDMA class. */
/** @cond */
  WMS_MESSAGE_CLASS_MAX,
  WMS_MESSAGE_CLASS_MAX32 = 0x10000000
/** @endcond */
} wms_message_class_e_type;


/** Message routing types.
*/
typedef enum
{
  WMS_ROUTE_DISCARD = 0,      /**< Discard the message. */
  WMS_ROUTE_STORE_AND_NOTIFY, /**< Store the message and notify the clients. */
  WMS_ROUTE_TRANSFER_ONLY,    /**< Do not store the message, but send it to 
                                   the clients. */
  WMS_ROUTE_TRANSFER_AND_ACK, /**< Send the message to the clients and 
                                   acknowledge it. */
  WMS_ROUTE_NO_CHANGE,        /**< Do not change the current setting. */
/** @cond */
  WMS_ROUTE_INTERNAL_PROC,    /* INTERNAL USE ONLY */
  WMS_ROUTE_MAX,
  WMS_ROUTE_MAX32   = 0x10000000
/** @endcond */
  } wms_route_e_type;


/** Types of memory stores that are supported.
*/
typedef enum
{
  WMS_MEMORY_STORE_NONE = 0, /**< No memory storage. */

  WMS_MEMORY_STORE_RAM_GW, /**< GSM/WCDMA memory store in RAM. */
  WMS_MEMORY_STORE_SIM,    /**< GSM/WCDMA memory store in SIM. */
  WMS_MEMORY_STORE_NV_GW,  /**< GSM/WCDMA memory store in nonvolatile memory. */

  WMS_MEMORY_STORE_RAM_CDMA, /**< CDMA memory store in RAM. Not supported. */
  WMS_MEMORY_STORE_RUIM,     /**< CDMA memory store in R-UIM. */
  WMS_MEMORY_STORE_NV_CDMA,  /**< CDMA memory store in nonvolatile memory. */

  WMS_MEMORY_STORE_CSIM,     /**< CDMA memory store in CSIM. */
  WMS_MEMORY_STORE_USIM,     /**< GSM/WCDMA memory store in USIM. */

/** @cond */
  WMS_MEMORY_STORE_MAX,
  WMS_MEMORY_STORE_MAX32  = 0x10000000
/** @endcond */
} wms_memory_store_e_type;
/*~ SENTINEL wms_memory_store_e_type.WMS_MEMORY_STORE_MAX */


/** Preferred mobile-originated SMS domain settings for GSM/GPRS/UMTS.
*/
typedef enum
{
  WMS_GW_DOMAIN_PREF_CS_PREFERRED  = 0, /**< Circuit-switched preferred. */
  WMS_GW_DOMAIN_PREF_PS_PREFERRED,      /**< Packet-switched preferred. */
  WMS_GW_DOMAIN_PREF_CS_ONLY,           /**< Circuit-switched only. */
  WMS_GW_DOMAIN_PREF_PS_ONLY,           /**< Packet-switched only. */
/** @cond */
  WMS_GW_DOMAIN_PREF_MAX,
  WMS_GW_DOMAIN_PREF_MAX32  = 0x10000000
/** @endcond */
} wms_gw_domain_pref_e_type;
/** @} */ /* end_addtogroup common_type_defs_group */


/** @ingroup common_type_defs_group
  Primary client configuration parameters.
*/
typedef struct
{
  boolean      set_primary;        /**< Indicates whether to set the client as 
                                        the primary client. */
  boolean      use_client_memory;  /**< Indicates whether to use the client 
                                        memory. */
  sys_modem_as_id_e_type as_id;    /**< Active subscription ID. */
} wms_cfg_set_primary_client_s_type;


/** @addtogroup common_type_defs_group
@{ */
/** Mode preferences [0,1,2] for link control.
*/
typedef enum
{
  WMS_LINK_CONTROL_DISABLED = 0, /**< Disabled. */
  WMS_LINK_CONTROL_ENABLED_ONE,  /**< Enable one. */
  WMS_LINK_CONTROL_ENABLE_ALL,   /**< Enable all. */
/** @cond */
  WMS_LINK_CONTROL_MAX,
  WMS_LINK_CONTROL_MAX32 = 0x10000000
/** @endcond */
} wms_cfg_link_control_mode_e_type;


/** Status of the link control.
*/
typedef enum
{
  WMS_LINK_CONTROL_INACTIVE = 0,       /**< Inactive. */
  WMS_LINK_CONTROL_ACTIVE,             /**< Active. */
  WMS_LINK_CONTROL_TIMEOUT,            /**< Timeout. */
  WMS_LINK_CONTROL_CONNECTION_RELEASED /**< Connection is released. */
} wms_cfg_link_control_status_e_type;
/** @} */ /* end_addtogroup common_type_defs_group */


/** @ingroup common_type_defs_group
  Mode and status of the link control.
*/
typedef struct wms_cfg_link_control_s
{
  wms_client_type_e_type              client_id;  /**< Client that issued the original command to the WMS. */
  wms_cfg_link_control_mode_e_type    link_control_mode; 
    /**< Link control mode. */
  wms_cfg_link_control_status_e_type  link_control_status; 
    /**< Link control status. */
  sys_modem_as_id_e_type              as_id;
    /**< Active subscription ID. */
} wms_cfg_link_control_s_type;


/** @ingroup common_type_defs_group
  Types of SMS bearers.
*/
typedef enum
{
  WMS_BEARER_CDMA_1X = 0, /**< CDMA 1X. */
  WMS_BEARER_CDMA_EVDO,   /**< CDMA EV-DO. */
  WMS_BEARER_CDMA_WLAN,   /**< CDMA WLAN. */

  //GSM/UMTS bearers to be supported later:
  WMS_BEARER_GW_CS,     /**< To be supported at a later date. */
  WMS_BEARER_GW_PS,     /**< To be supported at a later date. */
  WMS_BEARER_GW_PS_SIP, /**< To be supported at a later date. */
  WMS_BEARER_GW_WLAN,   /**< To be supported at a later date. */
  WMS_BEARER_IMS_3GPP,  /**< IMS with 3GPP capability. */
  WMS_BEARER_IMS_3GPP2, /**< IMS with 3GPP2 capability. */
  WMS_BEARER_LTE,       /**< SMS on LTE */
/** @cond */
  WMS_BEARER_MAX,
  WMS_BEARER_MAX32   = 0x10000000
/** @endcond */
} wms_bearer_e_type; 


/** @ingroup common_type_defs_group
  Message number type.
  In CDMA mode, the message number is the Message ID of the bearer data. 
  In GSM/WCDMA mode, it is the Message Reference number in the TPDU.
*/
typedef uint32  wms_message_number_type;


/** @ingroup common_type_defs_group
  For internal use. 
*/
/* Invalid message number */
#define WMS_DUMMY_MESSAGE_NUMBER  0xFFFFFFFF


/** @ingroup common_type_defs_group
  Message index in a message list.
*/
typedef uint32  wms_message_index_type;


/** @ingroup common_type_defs_group
  For internal use. 
*/
/* Invalid message index number */
#define WMS_DUMMY_MESSAGE_INDEX  0xFFFFFFFF


/** @ingroup common_type_defs_group
  Transaction ID to help differentiate between multiple incoming messages.
*/
typedef uint32  wms_transaction_id_type;


/** @ingroup common_type_defs_group
  For internal use. 
*/
/* Invalid transaction ID */
#define WMS_DUMMY_TRANSACTION_ID  0xFFFFFFFF 


/** @addtogroup common_type_defs_group
@{ */
/** WMS command IDs for Configuration group, Message group, Dedicated Channel 
  group, Broadcast Multimode group, and Debug group commands. 
*/
typedef enum
{
  /* Configuration group commands */
  WMS_CMD_CFG_SET_ROUTES = 0x0000,         /**< Set the routes. */
  WMS_CMD_CFG_GET_ROUTES,                  /**< Get the routes. */
  WMS_CMD_CFG_GET_MEMORY_STATUS,           /**< Get the memory status. */
  WMS_CMD_CFG_GET_MESSAGE_LIST,            /**< Get the message list. */
  WMS_CMD_CFG_SET_GW_DOMAIN_PREF,          /**< Set the GW domain preference. */
  WMS_CMD_CFG_GET_GW_DOMAIN_PREF,          /**< Get the GW domain preference. */
  WMS_CMD_CFG_SET_PRIMARY_CLIENT,          /**< Set the primary client. */
  WMS_CMD_CFG_SET_MEMORY_FULL,             /**< Set the memory as full. */
  WMS_CMD_CFG_SET_LINK_CONTROL,            /**< Set the link control. */
  WMS_CMD_CFG_GET_LINK_CONTROL,            /**< Get the link control. */
  WMS_CMD_CFG_SET_MESSAGE_WAITING,         /**< Set the Message Waiting Information. */
  WMS_CMD_CFG_GET_MESSAGE_WAITING,         /**< Get the Message Waiting Information. */
  WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT,       /**< Send SIM File read complete Event to PBM. */
  WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB,
  WMS_CMD_CFG_SET_LTE_DOMAIN_PREF,         /**< Set LTE domain preference. */

  /* Message group commands  */
  WMS_CMD_MSG_SEND       = 100,            /**< Send. */
  WMS_CMD_MSG_ACK,                         /**< Acknowledge. */
  WMS_CMD_MSG_READ,                        /**< Read. */
  WMS_CMD_MSG_WRITE,                       /**< Write. */
  WMS_CMD_MSG_DELETE,                      /**< Delete. */
  WMS_CMD_MSG_DELETE_ALL,                  /**< Delete all. */
  WMS_CMD_MSG_MODIFY_TAG,                  /**< Modify a tag. */
  WMS_CMD_MSG_READ_TEMPLATE,               /**< Read a template. */
  WMS_CMD_MSG_WRITE_TEMPLATE,              /**< Write a template. */
  WMS_CMD_MSG_DELETE_TEMPLATE,             /**< Delete a template. */
  WMS_CMD_MSG_DELETE_TEMPLATE_ALL,         /**< Delete all templates. */

  WMS_CMD_MSG_READ_STS_REPORT,             /**< Read a status report. */
  WMS_CMD_MSG_WRITE_STS_REPORT,            /**< Write a status report. */
  WMS_CMD_MSG_DELETE_STS_REPORT,           /**< Delete a status report. */
  WMS_CMD_MSG_DELETE_STS_REPORT_ALL,       /**< Delete all status reports. */
  WMS_CMD_MSG_TRANSPORT_REG,               /**< Transport registration. */

  /* DC commands  */   
  WMS_CMD_DC_CONNECT     = 200,            /**< Connect. */
  WMS_CMD_DC_DISCONNECT,                   /**< Disconnect. */
  WMS_CMD_DC_ENABLE_AUTO_DISCONNECT,       /**< Enable auto-disconnect. */
  WMS_CMD_DC_DISABLE_AUTO_DISCONNECT,      /**< Disable auto-disconnect. */

  /* BC_MM commands  */
  WMS_CMD_BC_MM_GET_CONFIG   = 400,        /**< Get the configuration. */
  WMS_CMD_BC_MM_GET_PREF,                  /**< Get the preference. */
  WMS_CMD_BC_MM_SET_PREF,                  /**< Set the preference. */
  WMS_CMD_BC_MM_GET_TABLE,                 /**< Get the table. */
  WMS_CMD_BC_MM_SELECT_SRV,                /**< Select the service. */
  WMS_CMD_BC_MM_GET_ALL_SRV_IDS,           /**< Get all service IDs. */
  WMS_CMD_BC_MM_GET_SRV_INFO,              /**< Get the service information. */
  WMS_CMD_BC_MM_ADD_SRV,                   /**< Add a service. */
  WMS_CMD_BC_MM_DELETE_SRV,                /**< Delete a service. */
  WMS_CMD_BC_MM_CHANGE_SRV_INFO,           /**< Change the service information. */
  WMS_CMD_BC_MM_DELETE_ALL_SERVICES,       /**< Delete all services. */
  WMS_CMD_BC_MM_SELECT_ALL_SERVICES,       /**< Select all services. */
  WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES, /**< Set the priority for all services. */
  WMS_CMD_BC_MM_MSG_DELETE_INDICATION,     /**< Delete an indication. */
  WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION, /**< Delete all indications. */
  WMS_CMD_BC_MM_GET_READING_PREF,          /**< Get the reading preference. */
  WMS_CMD_BC_MM_SET_READING_PREF,          /**< Set the reading preference. */

  /* DBG commands  */
  WMS_CMD_DBG_RESET_TL_SEQ_NUM  = 500,     /**< Reset the TL sequence number. */
  WMS_CMD_DBG_SET_MSG_REF_NUM,             /**< Set the message reference number. */
  WMS_CMD_DBG_CLEAR_SMMA,                  /**< Clear SMMA. */
  WMS_CMD_DBG_GET_RETRY_INTERVAL,          /**< Get the retry interval. */
  WMS_CMD_DBG_SET_RETRY_INTERVAL,          /**< Set the retry interval. */
  WMS_CMD_DBG_GET_RETRY_PERIOD,            /**< Get the retry period. */
  WMS_CMD_DBG_SET_RETRY_PERIOD,            /**< Set the retry period. */

  /* -- NOTE: the following are for internal use only --- */

/** @cond */
  /* Internal: events from MC  */
  WMS_CMD_MSG_MC_MT_MSG_E     = 600,
  WMS_CMD_MSG_MC_MO_STATUS_E,
  WMS_CMD_MSG_MC_STATE_CHANGED_E,
  WMS_CMD_MSG_MC_MWI_E,
  WMS_CMD_MSG_GSTK_EVT,

  /* Internal: events from CM  */
  WMS_CMD_DC_CALL_INCOM_E,
  WMS_CMD_DC_CALL_CONNECT_E,
  WMS_CMD_DC_CALL_END_E,
  WMS_CMD_DC_CALL_ERROR_E,  /* in case of CM cmd errors */

  WMS_CMD_CM_BC_ENABLE_FAILURE_E,
  WMS_CMD_CM_START_FULL_SERVICE_E,
  WMS_CMD_CM_ONLINE_E,
  WMS_CMD_CM_LPM_E,

  WMS_CMD_GSDI_ASYNC_CB,
  WMS_CMD_GSDI_CARD_STATUS_CB,
  WMS_CMD_GSDI_REFRESH_FCN_IND,
  WMS_CMD_BC_SCPT_DATA,

  WMS_CMD_SS_CHANGE_INFO,
  WMS_CMD_IPAPP_CHANGE_INFO,

  /* Internal events from DEM  */
  WMS_CMD_DEM_APPS_POWERDOWN,
  WMS_CMD_DEM_APPS_SUSPEND,
  WMS_CMD_DEM_APPS_POWER_RESTORED,
  WMS_CMD_DEM_APPS_RUN_QUIET,
  WMS_CMD_DEM_APPS_RUN_NORMAL,

  WMS_CMD_DUMMY,
  
  WMS_CMD_MMGSDI_EVENT_CB,
  WMS_CMD_MMGSDI_RESPONSE_CB,
  WMS_CMD_MMGSDI_SEEK_CB,

  WMS_CMD_CM_PH_EVENT_CB,
  WMS_CMD_CM_SUBS_EVENT_CB,
  WMS_CMD_CARD_EF_INFO_UPDATE,

  WMS_CMD_CLIENT_ACTIVATE,

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  WMS_CMD_NV_REFRESH_CB,
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  WMS_CMD_MAX,
  WMS_CMD_MAX32 = 0x10000000  /* pad to 32 bit int */
/** @endcond */
} wms_cmd_id_e_type;
/*~ SENTINEL wms_cmd_id_e_type.WMS_CMD_MAX */


/** Command status errors reported in the command callbacks.
*/
typedef enum
{
  WMS_CMD_ERR_NONE       = 0,          /**< No error. */

  WMS_CMD_ERR_CLIENT_ID,               /**< Client ID error. */
  WMS_CMD_ERR_NO_RESOURCE,             /**< No resource. */
  WMS_CMD_ERR_UNSUPPORTED,             /**< Unsupported. */
  WMS_CMD_ERR_BUSY,                    /**< Busy. */
  WMS_CMD_ERR_NULL_PTR,                /**< NULL pointer. */
  WMS_CMD_ERR_NO_NETWORK,              /**< No network. */
  WMS_CMD_ERR_BAD_PARM,                /**< Bad parameter. */
  WMS_CMD_ERR_CS_ERROR,                /**< Circuit-switched error. */

  WMS_CMD_ERR_CFG_NV_WRITE  = 100,     /**< Configuration error: NV write. */
  WMS_CMD_ERR_CFG_NV_READ,             /**< Configuration error: NV read. */
  WMS_CMD_ERR_CFG_ROUTE,               /**< Configuration error: route. */
  WMS_CMD_ERR_CFG_MEM_STORE,           /**< Configuration error: memory store. */
  WMS_CMD_ERR_CFG_MSG_CLASS,           /**< Configuration error: message class. */
  WMS_CMD_ERR_CFG_DOMAIN_PREF,         /**< Configuration error: domain preference. */
  WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT, /**< Configuration error: unprivileged client. */
  WMS_CMD_ERR_CFG_LINK_CONTROL,        /**< Configuration error: link control. */
  WMS_CMD_ERR_CFG_MESSAGE_WAITING,     /**< Configuration error: Message waiting information. */
  
  WMS_CMD_ERR_MSG_MEMORY_STORE = 200,  /**< Messaging error: memory store. */
  WMS_CMD_ERR_MSG_INDEX,               /**< Messaging error: index. */
  WMS_CMD_ERR_MSG_EMPTY_MESSAGE,       /**< Messaging error: empty message. */
  WMS_CMD_ERR_MSG_TAG,                 /**< Messaging error: tag. */
  WMS_CMD_ERR_MSG_ENCODE,              /**< Messaging error: encode. */
  WMS_CMD_ERR_MSG_DECODE,              /**< Messaging error: decode. */
  WMS_CMD_ERR_MSG_SEND_MODE,           /**< Messaging error: send mode. */
  WMS_CMD_ERR_MSG_WRITE_MODE,          /**< Messaging error: write mode. */
  WMS_CMD_ERR_MSG_MEMORY_FULL,         /**< Messaging error: memory is full. */
  WMS_CMD_ERR_MSG_RAM_FULL,            /**< Messaging error: RAM is full. */
  WMS_CMD_ERR_MSG_SIM_FULL,            /**< Messaging error: SIM is full. */
  WMS_CMD_ERR_MSG_NV_FULL,             /**< Messaging error: NV is full. */
  WMS_CMD_ERR_MSG_TRANSACTION_ID,      /**< Messaging error: transaction ID. */
  WMS_CMD_ERR_MSG_SIM_WRITE,           /**< Messaging error: SIM write. */
  WMS_CMD_ERR_MSG_SIM_READ,            /**< Messaging error: SIM read. */
  WMS_CMD_ERR_MSG_SIM_DELETE,          /**< Messaging error: SIM delete. */
  WMS_CMD_ERR_MSG_RUIM_WRITE,          /**< Messaging error: R-UIM write. */
  WMS_CMD_ERR_MSG_RUIM_READ,           /**< Messaging error: R-UIM read. */
  WMS_CMD_ERR_MSG_RUIM_DELETE,         /**< Messaging error: R-UIM delete. */
  WMS_CMD_ERR_MSG_NV_WRITE,            /**< Messaging error: NV write. */
  WMS_CMD_ERR_MSG_NV_READ,             /**< Messaging error: NV read. */
  WMS_CMD_ERR_MSG_NV_DELETE,           /**< Messaging error: NV delete. */
  WMS_CMD_ERR_MSG_TPDU_TYPE,           /**< Messaging error: TPDU type. */
  WMS_CMD_ERR_MSG_FORMAT,              /**< Messaging error: format. */
  WMS_CMD_ERR_MSG_NO_MO_MSG,           /**< Messaging error: no message. */
  WMS_CMD_ERR_MSG_NO_SC_ADDRESS,       /**< Messaging error: no Service Center address. */
  WMS_CMD_ERR_MSG_LPM,                 /**< Messaging error: Low Power mode. */
  WMS_CMD_ERR_MSG_BLOCKED,             /**< Messaging error: blocked. */ 
  WMS_CMD_ERR_PREF_CHN_UNSUPPORTED_S,  /**< Messaging error: MO channel. */ 
  WMS_CMD_ERR_MSG_GSTK_ACCESS_RAT,     /**< Messaging error: bad gstk access rat. */ 

  WMS_CMD_ERR_BC_BAD_PREF    = 300,    /**< Broadcast error: bad preference. */
  WMS_CMD_ERR_BC_CANNOT_ACTIVATE,      /**< Broadcast error: cannot activate. */
  WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL,  /**< Broadcast error: cannot activate any. */
  WMS_CMD_ERR_BC_NV_WRITE,             /**< Broadcast error: NV write. */
  WMS_CMD_ERR_BC_BAD_SRV_ID,           /**< Broadcast error: bad service ID. */
  WMS_CMD_ERR_BC_TABLE_FULL,           /**< Broadcast error: table is full. */
  WMS_CMD_ERR_BC_DUPLICATE_SRV,        /**< Broadcast error: duplicate service. */
  WMS_CMD_ERR_BC_SIM_WRITE,            /**< Broadcast error: SIM write. */
  WMS_CMD_ERR_BC_SIM_READ,             /**< Broadcast error: SIM read. */
  WMS_CMD_ERR_BC_DEACTIVATE_NOT_ALLOWED,  /**< Broadcast error: Cannot delete Presidental alert. */

  WMS_CMD_ERR_DC_BAD_STATE   = 400,    /**< Dedicated channel error: bad state. */
  WMS_CMD_ERR_DC_BAD_CALL_ID,          /**< Dedicated channel error: bad call ID. */
  WMS_CMD_ERR_DC_BAD_PARM,             /**< Dedicated channel error: bad parameter. */
  WMS_CMD_ERR_DC_LPM,                  /**< Dedicated channel error: Low Power mode. */

  WMS_CMD_ERR_DBG_BAD_PARAM  = 500,    /**< Debug error: bad parameter. */

  WMS_CMD_ERR_TRANSPORT_NOT_READY  = 600, /**< Transport is not ready to send 
                                               MO SMS. */
  WMS_CMD_ERR_TRANSPORT_NOT_ALLOWED,      /**< Transport does not allow MO SMS
                                               (e.g., limited service). */

  WMS_CMD_ERR_1X_AND_VOICE_NOT_ALLOWED,   /**< Simultaneous 1X and voice 
                                               does not allow MO SMS. */
  WMS_CMD_ERR_IMS_NOT_ALLOWED_IN_SRLTE,  /**< specified SMS trasport is not allowed while  
                                                  device operates in SRLTE mode. */

  WMS_CMD_ERR_AS_ID  = 700,               /**< Invalid active subscription ID. */
  
/** @cond */
  WMS_CMD_ERR_MAX, 
  WMS_CMD_ERR_MAX32 = 0x10000000     /* pad to 32 bit int */
/** @endcond */
} wms_cmd_err_e_type;
/*~ SENTINEL wms_cmd_err_e_type.WMS_CMD_ERR_MAX */
/** @} */ /* end_addtogroup common_type_defs_group */


/** @ingroup common_cb_group_wms_cmd_cb_type
  Command callback function for reporting the command execution status of 
  the API.

  @datatypes
  #wms_cmd_id_e_type \n
  #wms_cmd_err_e_type

  @param[in]  cmd        ID of the command whose status is being reported.
  @param[in]  user_data  Pointer to the user data.
  @param[in]  cmd_err    Status of the command.

  @return
  None.

  @dependencies
  None.
*/
typedef void (* wms_cmd_cb_type )
(
  wms_cmd_id_e_type        cmd,
  void                     *user_data,
  wms_cmd_err_e_type       cmd_err 
);
/*~ CALLBACK wms_cmd_cb_type
    ONERROR return  */


/** @addtogroup common_type_defs_group
@{ */
/** Client request errors.
*/
typedef enum
{
  WMS_CLIENT_ERR_NONE,                /**< No error. */
  WMS_CLIENT_ERR_CLIENT_ID_PTR,       /**< Client ID pointer. */
  WMS_CLIENT_ERR_CH_PREF_PTR,         /**< NULL channel pref pointer. */
  WMS_CLIENT_ERR_CLIENT_TYPE,         /**< Client type. */
  WMS_CLIENT_ERR_CLIENT_ID,           /**< Client ID. */
  WMS_CLIENT_ERR_TASK_NOT_READY,      /**< Task is not ready. */
  WMS_CLIENT_ERR_INVALID_PROCESSOR,   /**< Invalid processor. */
  WMS_CLIENT_ERR_INVALID_NOTIFY_TYPE, /**< Invalid notify type. */
  WMS_CLIENT_ERR_UNSUPPORTED,         /**< Unsupported. */
  WMS_CLIENT_ERR_GENERAL,             /**< General error. */
  WMS_CLIENT_ERR_RPC,                 /**< Remote procedure call error. */  
  WMS_CLIENT_ERR_CH_PREF,             /**< Unsupported MO channel. */
  WMS_CLIENT_ERR_SO_PREF_PTR,         /**< NULL service option pref pointer. */
  WMS_CLIENT_ERR_SO_PREF,             /**< Unsupported service option for MO channel. */
  WMS_CLIENT_ERR_FDC_PREF_PTR,        /**< NULL follow on DC pointer. */

/** @cond */
  WMS_CLIENT_ERR_MAX,                 /* FOR INTERNAL USE OF CM ONLY! */
  WMS_CLIENT_ERR_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_client_err_e_type;


/** Command or request status messages.
*/
typedef enum
{
  WMS_OK_S                 = 0,    /**< Status is OK. */
  WMS_OUT_OF_RESOURCES_S,          /**< Out of resources. */
  WMS_TERMINAL_BLOCKED_S,          /**< Terminal is blocked. */
  WMS_TERMINAL_BUSY_S,             /**< Terminal is busy. */
  WMS_INVALID_TRANSACTION_ID_S,    /**< Invalid transaction ID. */
  WMS_INVALID_FORMAT_S,            /**< Invalid format. */
  WMS_GENERAL_ERROR_S,             /**< General error. */
  WMS_UNSUPPORTED_S,               /**< Unsupported. */ 
  WMS_NULL_PTR_S,                  /**< NULL pointer. */
  WMS_INVALID_CLIENT_ID_S,         /**< Invalid client ID. */   
  WMS_ACK_OK_S,                    /**< ACK recived from NAS */

  WMS_INVALID_PARM_SIZE_S   = 100, /**< CDMA only. */
  WMS_INVALID_USER_DATA_SIZE_S,    /**< CDMA only. */
  WMS_INVALID_PARM_VALUE_S,        /**< CDMA only. */
  WMS_MISSING_PARM_S,              /**< CDMA only. */
  WMS_NETWORK_NOT_READY_S,         /**< CDMA only. */
  WMS_PHONE_NOT_READY_S,           /**< CDMA only. */
  WMS_NOT_ALLOWED_IN_AMPS_S,       /**< CDMA only. */
  WMS_NETWORK_FAILURE_S,           /**< CDMA only. */
  WMS_ACCESS_TOO_LARGE_S,          /**< CDMA only. */
  WMS_DTC_TOO_LARGE_S,             /**< CDMA only. */
  WMS_ACCESS_BLOCK_S,              /**< CDMA only. */
  WMS_ESN_MISMATCH_S,              /**< JCDMA2 feature only. */
  WMS_MO_PREF_CHN_NOT_AVAIL_S,     /**< CDMA only. */
  WMS_MO_PREF_CHN_UNSUPPORTED_S,   /**< CDMA only. */

  WMS_INVALID_TPDU_TYPE_S  = 200,  /**< GSM/WCDMA only. */
  WMS_INVALID_VALIDITY_FORMAT_S,   /**< GSM/WCDMA only. */
  WMS_INVALID_CB_DATA_S,           /**< GSM/WCDMA only. */

/** @cond */
  WMS_MT_MSG_FAILED_S,     /* internal use */
/** @endcond */

  WMS_SIP_PERM_ERROR_S     = 300,  /**< SIP permanent error. */
  WMS_SIP_TEMP_ERROR_S,            /**< SIP temporary error. */

  /* WMSC, CS and RPC errors  */
  WMS_WMSC_ERROR_S         = 400,  /**< WMSC error. */
  WMS_CS_ERROR_S,                  /**< CS error. */
  WMS_RPC_ERROR_S,                 /**< RPC error. */

  /* Transport layer errors */
  WMS_TRANSPORT_NOT_READY_S = 500, /**< Transport is not ready. */
  WMS_TRANSPORT_NOT_ALLOWED_S,     /**< Transport is not allowed. */
  WMS_1X_AND_VOICE_NOT_ALLOWED_S,  /**< 1X and voice is not allowed. */
  WMS_IMS_NOT_ALLOWED_IN_SRLTE_S,  /**< 1X not allowed while in 1xSRLTE mode. */ 

/** @cond */
  WMS_STATUS_MAX,
  WMS_STATUS_MAX32 = 0x10000000  /* pad to 32 bit int */
/** @endcond */
} wms_status_e_type;
/*~ SENTINEL wms_status_e_type.WMS_STATUS_MAX */


/** Information about a message report (SUBMIT-REPORT) received from the 
  network.
*/
typedef enum
{
  WMS_RPT_OK        = 0, /**< Success; submit_report_acknowledgment information 
                              is available for GSM/WCDMA. */
  WMS_RPT_LL_ERROR  = 1, /**< Lower layer errors. For GSM/WCDMA, 
                              cause_value = LL Cause. This information is not 
                              available to clients. */
  WMS_RPT_GENERAL_ERROR = 2, /**< Any other error. */

  /* ------- CDMA specific failures -------- */
  WMS_RPT_OUT_OF_RESOURCES = 3,  /**< CDMA-specific failure. */
  WMS_RPT_NETWORK_NOT_READY = 4, /**< CDMA-specific failure. */
  WMS_RPT_PHONE_NOT_READY   = 5, /**< CDMA-specific failure. */
  WMS_RPT_NO_ACK            = 6, /**< CDMA Transport Layer acknowledgment was 
                                         not received. */
  WMS_RPT_CDMA_TL_ERROR     = 7, /**< CDMA Transport Layer error from the 
                                         network. Error_class and status in 
                                         cause_info are available. */
  WMS_RPT_ACCESS_TOO_LARGE  = 8, /**< CDMA message is too large to be sent 
                                         over the access channel. */
  WMS_RPT_DC_TOO_LARGE      = 9, /**< CDMA message is too large to be sent 
                                         over the dedicated channel. */
  WMS_RPT_NOT_ALLOWED_IN_AMPS = 10,   /**< CDMA-specific failure. */
  WMS_RPT_CANNOT_SEND_BROADCAST = 11, /**< CDMA-specific failure. */
  WMS_RPT_ACCESS_BLOCK       = 12,    /**< CDMA-specific failure. */
  WMS_RPT_1X_AND_VOICE_NOT_ALLOWED = 13, /**< CDMA-specific failure. */
  WMS_RPT_PREF_CHN_NOT_AVAIL_ERROR = 14, /**< CDMA-specific failure. */

  /* ------- GSM/WCDMA specific failures -------- */
  WMS_RPT_RP_ERROR         = 100, 
   /**< GSM/WCDMA-specific failure: submit_report_error TPDU is available. 
           cause_value = RP Cause as defined in Section 8.2.5.4 TS 24.011; 
           diagnostics might be available as defined in Section 8.2.5.4 TS 24.011. */
  WMS_RPT_CP_ERROR         = 101, 
   /**< GSM/WCDMA-specific failure: no submit_report TPDU is available. 
           cause_value = CP Cause as defined in Section 8.1.4.2 TS 24.011. */
  WMS_RPT_SMR_TR1M_TIMEOUT = 102, 
   /**< GSM/WCDMA-specific failure: the SMS entity's Timer 1M timed out in the 
           attempt to relay an MO message or notification to the network. */
  WMS_RPT_SMR_TR2M_TIMEOUT = 103, 
   /**< GSM/WCDMA-specific failure: the SMR entity's Timer 2M timed out in the 
           attempt to transfer an MT message. */
  WMS_RPT_SMR_NO_RESOURCES = 104, 
   /**< GSM/WCDMA-specific failure: SMR entity has no resources available.  */
  WMS_RPT_MO_CONTROL_DISALLOW = 105,
   /**< GSM/WCDMA-specific failure: MO SMS control disallows the MO message. */
  WMS_RPT_MO_CONTROL_ERROR = 106,    
   /**< GSM/WCDMA-specific failure: MO SMS control problem. */
  WMS_RPT_MO_RETRY_PERIOD_EXPIRED = 107,  
   /**< GSM/WCDMA-specific failure: MO retry period expired. */ 
  WMS_RPT_NO_RESPONSE_FROM_NETWORK = 108, 
   /**< No response from the GSM/WCDMA network. */
  WMS_RPT_RADIO_LINK_ENDED_FOR_SUBMIT_REPORT = 109,
   /**< NAS radio link failure when the UE waits for a final CP_ACK from 
        the network in MT-SMS. */
  WMS_RPT_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT = 110,
   /**< NAS TC1M_MT timer expires for the second time. */

  /* ------ SIP specific errors ----- */
  WMS_RPT_SIP_PERM_ERROR   = 200, /**< SIP permanent error. */
  WMS_RPT_SIP_TEMP_ERROR   = 201, /**< SIP temporary error. */

  WMS_RPT_INVALID_AS_ID    = 300, /**< Invalid active subscription ID. */
  WMS_RPT_RADIO_UNAVAILABLE,      /**< Radio is unavailable because a connection 
                                       is active on the other subscription. */

  WMS_RPT_TRANSPORT_NOT_READY    = 400, /**< Transport is not ready to send MO SMS. */ 
  WMS_RPT_TRANSPORT_NOT_ALLOWED,        /**< MO SMS is not allowed to be sent over 
                                             the transport layer. */
  WMS_RPT_IMS_NOT_ALLOWED_IN_SRLTE, 
  
                                       
/** @cond */
  WMS_RPT_MAX,
  WMS_RPT_MAX32 = 0x10000000
/** @endcond */
} wms_report_status_e_type;
/*~ SENTINEL wms_report_status_e_type.WMS_RPT_MAX */
/** @} */ /* end_addtogroup common_type_defs_group */


/** @ingroup common_type_defs_group
  Includes all the information in an MO/MT message.
*/
typedef struct 
{
  wms_format_e_type               format;
  /**< Format of the message type. */
  uint16                          data_len;
  /**< Length of the message. */
  uint8                           data[WMS_MAX_LEN];
  /**< Raw Protocol Data Unit (PDU) SMS data. */
  wms_address_s_type              address;
  /**< SMS destination address; see #wms_address_s for details. @newpagetable */
} wms_ota_msg_s_type;


/** @addtogroup common_type_defs_group
@{ */
/** Supported SMS transport types.
*/
typedef enum 
{
  WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS,  /**< SMS over IMS. */   
  WMS_MSG_TRANSPORT_TYPE_SMS_OVER_SGS,  /**< SMS over SGS. */
  WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102, /**< SMS over S102. */   
  WMS_MSG_TRANSPORT_TYPE_FLOATING1,     /**< Floating type 1. */
  WMS_MSG_TRANSPORT_TYPE_FLOATING2,     /**< Floating type 2. */
  WMS_MSG_TRANSPORT_TYPE_INTERNAL,      /**< Internal use only. */
  WMS_MSG_TRANSPORT_TYPE_CS,            /**< Circuit-switched. @newpage */
/** @cond */
  WMS_MSG_TRANSPORT_TYPE_MAX,
  WMS_MSG_TRANSPORT_TYPE_MAX32 = 0x10000000
/** @endcond */
} wms_msg_transport_type_e_type;


/** SMS Transport Layer network registration status.
*/
typedef enum {
   WMS_MSG_TRANSPORT_NW_REG_STATUS_NO_SRV,       /**< No service. */
   WMS_MSG_TRANSPORT_NW_REG_STATUS_IN_PROGRESS,  /**< In progress. */
   WMS_MSG_TRANSPORT_NW_REG_STATUS_FAILED,       /**< Failed. */
   WMS_MSG_TRANSPORT_NW_REG_STATUS_LIMITED_SRV,  /**< Limited service. */
   WMS_MSG_TRANSPORT_NW_REG_STATUS_FULL_SRV,     /**< Full service. */

/** @cond */
   WMS_MSG_TRANSPORT_NW_REG_MAX32 = 0x10000000
/** @endcond */
} wms_msg_transport_nw_reg_status_e_type;

/** Transport-specific message waiting types.
*/
typedef enum
{
  WMS_TRANSPORT_MWI_MSG_NONE        = -1,
  WMS_TRANSPORT_MWI_MSG_VOICE       = 0,
  WMS_TRANSPORT_MWI_MSG_VIDEO       = 1,
  WMS_TRANSPORT_MWI_MSG_FAX         = 2,
  WMS_TRANSPORT_MWI_MSG_PAGER       = 3,
  WMS_TRANSPORT_MWI_MSG_MULTIMEDIA  = 4,
  WMS_TRANSPORT_MWI_MSG_TEXT        = 5,
  WMS_TRANSPORT_MWI_MAX
} wms_msg_transport_mwi_kind_e_type;

/** Transport-specific message waiting priority types.
*/
typedef enum
{
  WMS_TRANSPORT_MWI_MSG_PRIORITY_UNKNOWN   = -1,
  WMS_TRANSPORT_MWI_MSG_PRIORITY_LOW       = 0,
  WMS_TRANSPORT_MWI_MSG_PRIORITY_NORMAL    = 1,
  WMS_TRANSPORT_MWI_MSG_PRIORITY_URGENT    = 2,
  WMS_TRANSPORT_MWI_MSG_PRIORITY_MAX
} wms_msg_transport_mwi_priority_e_type;

/** @} */ /* end_addtogroup common_type_defs_group */

/** @ingroup common_type_defs_group
  SMS transport identifier assigned by the WMS when a transport is registered 
  with WMS.
*/
typedef uint8 wms_msg_transport_id_type;

/** @addtogroup common_type_defs_group
@{ */
/** Invalid Transport ID.
*/  
#define WMS_MSG_TRANSPORT_ID_INVALID          0xFF


/* SMS transport capability. */
/** 3GPP2 1X. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP2_1X       0x000000100 
/** 3GPP2 EV-DO. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP2_EVDO     0x000000200 
/** 3GPP2 IP multimedia subsystem. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP2_IMS      0x000000400 
/** 3GPP2 IP multimedia subsystem supporting administrative SMS only. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP2_IMS_LIMITED      0x000000800 

/** 3GPP circuit-switched. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP_CS        0x000010000 
/** 3GPP packet-switched. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP_PS        0x000020000 
/** 3GPP IP multimedia subsystem. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP_IMS       0x000040000 
/** 3GPP long term evolution. */
#define  WMS_MSG_TRANSPORT_CAP_3GPP_LTE       0x000080000 
/** 3GPP IP multimedia subsystem supporting administrative SMS only. @newpage */
#define  WMS_MSG_TRANSPORT_CAP_3GPP_IMS_LIMITED      0x000100000 
/** @} */ /* end_addtogroup common_type_defs_group */

/* SMS transport update command type. */
#define WMS_MSG_TRANSPORT_UPDATE_NONE             0
#define WMS_MSG_TRANSPORT_RPT_IND                 1
#define WMS_MSG_TRANSPORT_RPT_MWI_IND             2
#define WMS_MSG_TRANSPORT_UPDATE_NW_REG_STATUS    3
#define WMS_MSG_TRANSPORT_UPDATE_MO_SMS_CB        4
#define WMS_MSG_TRANSPORT_UPDATE_CAP              5
#define WMS_MSG_TRANSPORT_UPDATE_F_TIMER          6



/** @ingroup common_type_defs_group
  SMS transport capability type.
*/
typedef uint32 wms_msg_transport_cap_type;  

typedef uint32 wms_msg_transport_f_timer_type;

/** @addtogroup common_type_defs_group
@{ */
/** Transport Layer status.
*/
typedef struct 
{
  wms_msg_transport_type_e_type            transport_type;
  /**< Transport type. */
  wms_msg_transport_nw_reg_status_e_type   transport_status;
  /**< Transport network registration status. */
  wms_msg_transport_cap_type               transport_cap;
  /**< Transport capability. */
  sys_modem_as_id_e_type                   as_id;
  /**< Active subscription ID. */
} wms_msg_transport_status_s_type;

/** Event data for the WMS_MSG_EVENT_TRANSPORT_REG event to notify WMS 
  clients about the registration/deregistration of a particular Transport Layer.
*/
typedef struct
{
   boolean                        is_registered;
     /**< Indicates whether the Transport Layer is registered. */
   wms_msg_transport_type_e_type  transport_type; /**< Transport type. */   
   wms_msg_transport_cap_type     transport_cap; /**< Transport capability. @newpagetable */   
   sys_modem_as_id_e_type         as_id; /**< Active subscription ID. */   
} wms_msg_transport_reg_info_s_type;


/** @addtogroup common_type_defs_group
@{ */
/** Transport Layer MWI summary.
*/

typedef struct
{
  wms_msg_transport_mwi_kind_e_type  msg_type;     /**< Transport MWI type. */
  uint16                             new_msg;      /**< Number of new messages. */
  uint16                             old_msg;      /**< Number of old messages. */
  uint16                             new_urgent;   /**< Number of new messages which are of priority type urgent. */
  uint16                             old_urgent;   /**< Number of old messages which are of priority type urgent. */
} wms_msg_mwi_summary_s_type;

/** @addtogroup common_type_defs_group
@{ */
/** Transport Layer MWI details.
*/
typedef struct
{
  uint8 to_addr_len;                                       /**< Destination address length. */
  uint8 receiver_addr[WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX];  /**< Destination address. */
  uint8 from_addr_len;                                     /**< Sender's address length. */
  uint8 sender_addr[WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX];    /**< Sender's address. */
  uint8 len;                                               /**< Length of subject line. */
  uint8 subject[WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX];        /**< Subject line. */
  uint8 timestamp_len;                                     /**< Timestamp length. */
  uint8 timestamp[WMS_TRANSPORT_MWI_DATE_LENGTH_MAX];      /**< Timestamp. */
  wms_msg_transport_mwi_priority_e_type priority;          /**< Transport MWI priority type. */
  uint8 identifier_len;                                    /**< Message ID length. */
  uint8 message_id[WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX];     /**< Message ID. */
  wms_msg_transport_mwi_kind_e_type message_type;
} wms_msg_mwi_detail_s_type;

/** Event data for the WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION
    event to notify WMS clients about the MWI information
    received from Transport Layer.
*/
typedef struct
{
   uint8                      address_len;
   /**< Length of the message account address field. */   
   uint8                      msg_account_addr[WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX];
   /**< Message Account address. */
   uint8                      summary_tuple_count;
   /**< Number of instances of the MWI summary information. */
   wms_msg_mwi_summary_s_type *mwi_msg_summary;
   /**< Pointer to the MWI summary information. */
   uint8                      details_tuple_count;
   /**< Number of instances of the MWI details information. */
   wms_msg_mwi_detail_s_type  *mwi_msg_details;
   /**< Pointer to the MWI details information. */
   sys_modem_as_id_e_type      as_id;
   /**< Active subscription ID. */
} wms_msg_transport_mwi_info_s_type;                                                                                                                                                   
/** @} */ /* end_addtogroup common_type_defs_group */


/** @ingroup common_cb_group_wms_msg_transport_mo_sms_cb_type
  Callback function invoked by the WMS to send an SMS over a selected 
  registered Transport Layer.

  @datatypes
  #wms_ota_msg_s_type

  @param[in]  msg_ptr  Pointer to the MO SMS message.

  @return
  None.

  @dependencies
  None.
*/
typedef boolean (*wms_msg_transport_mo_sms_cb_type) (
  sys_modem_as_id_e_type  as_id,
  wms_ota_msg_s_type     *msg_ptr
);
/*~ PARAM IN msg_ptr         POINTER */
/*~ CALLBACK wms_msg_transport_mo_sms_cb_type 
    ONERROR return FALSE */


/** @ingroup common_type_defs_group
  SMS Transport Layer report indicator type.
*/
typedef enum
{
  WMS_MSG_TRANSPORT_MT_SMS_IND,
    /**< Mobile-terminated indication. */
  WMS_MSG_TRANSPORT_3GPP2_MO_SMS_STATUS_IND,
    /**< 3GPP2 mobile-originated indication. */
  WMS_MSG_TRANSPORT_3GPP_MO_SMS_STATUS_IND,
    /**< 3GPP mobile-originated indication. */ 

/** @cond */
  WMS_MSG_TRANSPORT_RPT_IND_MAX32 = 0x10000000
/** @endcond */
} wms_msg_transport_rpt_ind_e_type;


/** @ingroup common_type_defs_group
  Data structure for the SMS Transport Layer to send a report 
  indicator to the WMS.
*/
typedef struct 
{ 
   wms_msg_transport_rpt_ind_e_type  ind_type;     /**< Transport report type. */
   wms_msg_transport_id_type         transport_id; /**< Transport ID. */ 
   wms_status_e_type                 mo_status;    /**< MO status type. */
   wms_ota_msg_s_type                mt_sms;       /**< Over-the-air message. */
   uint8                             retry_after_time; /**< Retry after header from IMS. */
   uint16                            ims_cause_code;/* <IMS cause code for temp and permanent>*/
} wms_msg_transport_rpt_ind_s_type;

/** @ingroup common_type_defs_group
  Data structure for the SMS Transport Layer to send a MWI 
  report indicator to the WMS. 
*/
typedef struct 
{ 
   uint8                      addr_len;  /**< Length of the sender address field. */
   uint8                      ue_addr[WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX]; /**< To or Message Account address. */
   uint8                      num_instances_summary;   /**< Number of instances of MWI summary information. */
   wms_msg_mwi_summary_s_type *transport_mwi_summary;  /**< Pointer to the MWI summary information. */
   uint8                      num_instances_details;   /**< Number of instances of MWI details information. */
   wms_msg_mwi_detail_s_type  *transport_mwi_details;  /**< Pointer to the MWI details information. */
} wms_msg_transport_rpt_mwi_ind_s_type;

/* <EJECT> */
/* ========================================================================= */
/* ===================== CDMA specific type definitions ==================== */
/* ========================================================================= */

/** @addtogroup cdma_specific_type_defs_group 
@{ */
/** CDMA-specific teleservice IDs. 
  The values 0 through 0xFFFF are used in CDMA mode and Analog AWI SMS as defined 
  in IS-637/IS-41.

  @note1hang For teleservice CM_91, the encoding type of the user data indicates 
             whether the teleservice is actually CPT, VMN, or CMT. The user 
             data is extracted into the corresponding bearer data fields.
*/
typedef enum
{
  WMS_TELESERVICE_CMT_91             = 4096, /**< Embedded IS91 SMS. */
  WMS_TELESERVICE_CPT_95             = 4097, /**< Pager. */
  WMS_TELESERVICE_CMT_95             = 4098, /**< Short message. */
  WMS_TELESERVICE_VMN_95             = 4099, /**< Voice mail notification. */
  WMS_TELESERVICE_WAP                = 4100, /**< Wireless Application Protocol. */
  WMS_TELESERVICE_WEMT               = 4101, /**< Enhanced messaging/EMS. */
  WMS_TELESERVICE_SCPT               = 4102, /**< Service category programming. */
  WMS_TELESERVICE_CATPT              = 4103, /**< Card Application Toolkit 
                                                  Protocol. */

  WMS_TELESERVICE_GSM1x_01           = 4104, /**< GSM1x signaling message. */
  WMS_TELESERVICE_GSM1x_02           = 4105, /**< GSM1x short message. */

  /* GSM1x_03 thru _10 are reserved for now. */

  WMS_TELESERVICE_GSM1x_03           = 4106, /**< Reserved. */
  WMS_TELESERVICE_GSM1x_04           = 4107, /**< Reserved. */
  WMS_TELESERVICE_GSM1x_05           = 4108, /**< Reserved. */
  WMS_TELESERVICE_GSM1x_06           = 4109, /**< Reserved. */
  WMS_TELESERVICE_GSM1x_07           = 4110, /**< Reserved. */
  WMS_TELESERVICE_GSM1x_08           = 4111, /**< Reserved. */
  WMS_TELESERVICE_GSM1x_09           = 4112, /**< Reserved. */
  WMS_TELESERVICE_GSM1x_10           = 4113, /**< Reserved. */

  WMS_TELESERVICE_IMSST              = 4242, /**< IMS Services Teleservice. */

  WMS_TELESERVICE_CT_CDMA_WAP 	     = 65002, /**< Wireless Application Protocol. */

  WMS_TELESERVICE_IS91_PAGE          = 0x00010000,
    /**< Extension to standard teleservices: IS91 Paging in Analog mode. */
  WMS_TELESERVICE_IS91_VOICE_MAIL    = 0x00020000, 
    /**< Extension to standard teleservices: IS91 Voice Mail in Analog mode. */
  WMS_TELESERVICE_IS91_SHORT_MESSAGE = 0x00030000, 
    /**< Extension to standard teleservices: IS91 Short Message Service in 
         Analog mode. */
  WMS_TELESERVICE_MWI                = 0x00040000,
    /**< Extension to standard teleservices: Voice mail notification through 
         message waiting indication in CDMA or Analog mode. */
  WMS_TELESERVICE_BROADCAST          = 0x00050000,
    /**< Extension to standard teleservices: Broadcast SMS message. */
  WMS_TELESERVICE_UNKNOWN            = 0x0FFFFFFF   
    /**< Extension to standard teleservices: Invalid teleservice ID. */
} wms_teleservice_e_type;


/** CDMA-specific service categories.
*/
typedef enum
{
  WMS_SRV_UNKOWN  = 0,    /**< Unknown. */
  WMS_SRV_EMERGENCY,      /**< Emergency. */ 
  WMS_SRV_ADMIN,          /**< Administrative. */
  WMS_SRV_MAINTENANCE,    /**< Maintenance. */
  WMS_SRV_GEN_NEWS_LOC,   /**< General news: local. */
  WMS_SRV_GEN_NEWS_REG,   /**< General news: regional. */
  WMS_SRV_GEN_NEWS_NAT,   /**< General news: national. */
  WMS_SRV_GEN_NEWS_INT,   /**< General news: international. */
  WMS_SRV_FIN_NEWS_LOC,   /**< Business/financial news: local. */
  WMS_SRV_FIN_NEWS_REG,   /**< Business/financial news: regional. */
  WMS_SRV_FIN_NEWS_NAT,   /**< Business/financial news: national. */
  WMS_SRV_FIN_NEWS_INT,   /**< Business/financial news: international. */
  WMS_SRV_SPT_NEWS_LOC,   /**< Sports news: local. */
  WMS_SRV_SPT_NEWS_REG,   /**< Sports news: regional. */
  WMS_SRV_SPT_NEWS_NAT,   /**< Sports news: national. */
  WMS_SRV_SPT_NEWS_INT,   /**< Sports news: international. */
  WMS_SRV_ENT_NEWS_LOC,   /**< Entertainment news: local. */
  WMS_SRV_ENT_NEWS_REG,   /**< Entertainment news: regional. */
  WMS_SRV_ENT_NEWS_NAT,   /**< Entertainment news: national. */
  WMS_SRV_ENT_NEWS_INT,   /**< Entertainment news: international. */
  WMS_SRV_LOC_WEATHER,    /**< Local weather. */
  WMS_SRV_AREA_TRAFFIC,   /**< Area traffic reports. */
  WMS_SRV_AIRPORT_SCHED,  /**< Local airport flight schedules. */
  WMS_SRV_RESTAURANTS,    /**< Restaurants. */
  WMS_SRV_LODGINGS,       /**< Lodgings. */
  WMS_SRV_RETAILS,        /**< Retails. */
  WMS_SRV_ADS,            /**< Advertisements. */
  WMS_SRV_STOCK_QUOTES,   /**< Stock quotes. */
  WMS_SRV_JOBS,           /**< Employment opportunities. */
  WMS_SRV_MEDICAL,        /**< Medical. */
  WMS_SRV_TECH_NEWS,      /**< Technology news. */
  WMS_SRV_MULTI,          /**< Multi-category. */

  WMS_SRV_CATPT,          /**< 0x20 Card Application Toolkit Protocol teleservice. */ 
  WMS_SRV_CMAS_PRESIDENTIAL_ALERT  = 0x1000, /**< Presidential-level alert. */
  WMS_SRV_CMAS_EXTREME_ALERT       = 0x1001, /**< Extreme threat to life and 
                                                  property. */
  WMS_SRV_CMAS_SEVERE_ALERT        = 0x1002, /**< Severe threat to life and 
                                                  property. */
  WMS_SRV_CMAS_AMBER_ALERT         = 0x1003, /**< AMBER (child abduction 
                                                  emergency). */
  WMS_SRV_CMAS_TEST_MSG            = 0x1004, /**< Commercial Mobile Alert System 
                                                  test message. */
/** @cond */
  WMS_SRV_MAX32           = 0x10000000
/** @endcond */
} wms_service_e_type;


/** CDMA-specific error classes.
*/
typedef enum
{
  WMS_ERROR_NONE        = 0, /**< No error. */
  WMS_ERROR_RESERVED_1  = 1, /**< Reserved. */
  WMS_ERROR_TEMP        = 2, /**< Temporary error. */
  WMS_ERROR_PERM        = 3, /**< Permanent error. */
/** @cond */
  WMS_ERROR_MAX32       = 0x10000000
/** @endcond */
} wms_error_class_e_type;


/** CDMA-specific status codes.
  The first half of the enumerators are from IS-41D SMS cause codes with the 
  exact binary values as defined in IS-41D. They are in the range of 0x00 to 
  0xFF.  

  The status codes can be grouped as follows:
  - All values within the range of 6 to 31 are treated as 
    WMS_TL_OTHER_NETWORK_PROBLEM_S.
  - All values within the range of 40 to 47 are treated as 
    WMS_TL_OTHER_TERMINAL_PROBLEM_S.
  - All values within the range of 67 to 95 are treated as 
    WMS_TL_OTHER_RADIO_IF_PROBLEM_S.
  - All values within the range of 108 to 255 are treated as 
    WMS_TL_OTHER_GENERAL_PROBLEMS_S.
*/
typedef enum
{
  /* A. Network Problems:  */
  WMS_TL_ADDRESS_VACANT_S                     = 0, /**< Network problem. */
  WMS_TL_ADDRESS_TRANSLATION_FAILURE_S,            /**< Network problem. */
  WMS_TL_NETWORK_RESOURCE_SHORTAGE_S,              /**< Network problem. */
  WMS_TL_NETWORK_FAILURE_S,                        /**< Network problem. */
  WMS_TL_INVALID_TELESERVICE_ID_S,                 /**< Network problem. */
  WMS_TL_OTHER_NETWORK_PROBLEM_S,                  /**< Other network problem. */
  WMS_TL_OTHER_NETWORK_PROBLEM_MORE_FIRST_S   = 6, /**< First of other network 
												        problems. */

  /* All values within the range of 6 to 31 are treated as 
     WMS_TL_OTHER_NETWORK_PROBLEM_S. */

  WMS_TL_OTHER_NETWORK_PROBLEM_MORE_LAST_S    = 31, /**< Last of other network 
													     problems. */

  /* B. Terminal Problems:  */
  WMS_TL_NO_PAGE_RESPONSE_S                   = 32, /**< Terminal problem. */
  WMS_TL_DESTINATION_BUSY_S,                        /**< Terminal problem. */
  WMS_TL_NO_ACK_S,                                  /**< Terminal problem. */
  WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S,           /**< Terminal problem. */
  WMS_TL_SMS_DELIVERY_POSTPONED_S,                  /**< Terminal problem. */
  WMS_TL_DESTINATION_OUT_OF_SERVICE_S,              /**< Terminal problem. */
  WMS_TL_DESTINATION_NO_LONGER_AT_THIS_ADDRESS_S,   /**< Terminal problem. */
  WMS_TL_OTHER_TERMINAL_PROBLEM_S,                  /**< Other terminal problem. */
  WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_FIRST_S  = 40, /**< First of other terminal 
                                                         problems. */

  /* All values within the range of 40 to 47 are treated as 
     WMS_TL_OTHER_TERMINAL_PROBLEM_S. */

  WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_LAST_S   = 47, /**< Last of other terminal 
                                                         problems. */
  WMS_TL_SMS_DELIVERY_POSTPONED_MORE_FIRST_S  = 48, /**< First of SMS delivery 
                                                         postponed problems. */
  WMS_TL_SMS_DELIVERY_POSTPONED_MORE_LAST_S   = 63, /**< Last of SMS delivery 
                                                         postponed problems. */

  /* C. Radio Interface Problems:  */
  WMS_TL_RADIO_IF_RESOURCE_SHORTAGE_S         = 64, /**< Radio interface problem. */
  WMS_TL_RADIO_IF_INCOMPATIBLE_S,                   /**< Radio interface problem. */
  WMS_TL_OTHER_RADIO_IF_PROBLEM_S,                  /**< Other radio interface 
													     problem. */
  WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_FIRST_S  = 67, /**< First of other radio 
                                                         interface problems. */

  /* All values within the range of 67 to 95 are treated as 
     WMS_TL_OTHER_RADIO_IF_PROBLEM_S. */

  WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_LAST_S   = 95, /**< Last of other radio 
                                                         interface problems. */

  /* D. General Problems:  */
  WMS_TL_UNEXPECTED_PARM_SIZE_S               = 96,  /**< General problem. */
  WMS_TL_SMS_ORIGINATION_DENIED_S,                   /**< General problem. */         
  WMS_TL_SMS_TERMINATION_DENIED_S,                   /**< General problem. */
  WMS_TL_SUPPL_SERVICE_NOT_SUPPORTED,                /**< General problem. */
  WMS_TL_SMS_NOT_SUPPORTED_S,                        /**< General problem. */
  WMS_TL_RESERVED_101_S,                             /**< General problem. */
  WMS_TL_MISSING_EXPECTED_PARM_S,                    /**< General problem. */
  WMS_TL_MISSING_MANDATORY_PARM_S,                   /**< General problem. */
  WMS_TL_UNRECOGNIZED_PARM_VALUE_S,                  /**< General problem. */
  WMS_TL_UNEXPECTED_PARM_VALUE_S,                    /**< General problem. */
  WMS_TL_USER_DATA_SIZE_ERROR_S,                     /**< General problem. */
  WMS_TL_OTHER_GENERAL_PROBLEMS_S,                   /**< Other general problem. */
  WMS_TL_OTHER_GENERAL_PROBLEMS_MORE_FIRST_S  = 108, /**< First of other general 
                                                          problems. */

  /* All values within the range of 108 to 255 are treated as 
     WMS_TL_OTHER_GENERAL_PROBLEMS_S. */

  WMS_TL_OTHER_GENERAL_PROBLEMS_MORE_LAST_S   = 255, /**< Last of other general 
                                                          problems. */
/** @cond */
  WMS_TL_MAX32                                = 0x10000000
/** @endcond */
} wms_cdma_tl_status_e_type;


/** CDMA-specific message types.
*/
typedef enum
{
  WMS_BD_TYPE_RESERVED_0 = 0, /**< Reserved. */
  WMS_BD_TYPE_DELIVER,        /**< MT only. */
  WMS_BD_TYPE_SUBMIT,         /**< MO only. */
  WMS_BD_TYPE_CANCELLATION,   /**< MO only. */
  WMS_BD_TYPE_DELIVERY_ACK,   /**< MT only. */
  WMS_BD_TYPE_USER_ACK,       /**< MT and MO. */
  WMS_BD_TYPE_READ_ACK,       /**< MT and MO. */
/** @cond */
  WMS_BD_TYPE_MAX32          = 0x10000000
/** @endcond */
} wms_bd_message_type_e_type;
/** @} */ /* end_addtogroup cdma_specific_type_defs_group */


/** @ingroup cdma_specific_type_defs_group 
  CDMA-specific message types.
*/
#define WMS_IS_MO_CDMA_MSG_TYPE( type )         \
          ( (type) == WMS_BD_TYPE_SUBMIT ||       \
            (type) == WMS_BD_TYPE_CANCELLATION || \
            (type) == WMS_BD_TYPE_USER_ACK ||     \
            (type) == WMS_BD_TYPE_READ_ACK )


/** @ingroup cdma_specific_type_defs_group 
  CDMA-specific message ID.
*/
typedef struct wms_message_id_s
{
  wms_bd_message_type_e_type   type;        /**< CDMA-specific message type. */
  wms_message_number_type      id_number;   /**< Message ID of the bearer data. */
  boolean                      udh_present;
    /**< Indicates whether UDH is present. If FEATURE_SMS_UDH is not defined, 
         ignore udh_present. */
} wms_message_id_s_type;


/** @addtogroup cdma_specific_type_defs_group 
@{ */
/** CDMA-specific user data encoding types.
*/
typedef enum
{
  WMS_ENCODING_OCTET        = 0,         /**< 8 bit. */
  WMS_ENCODING_IS91EP,                   /**< Varies. */
  WMS_ENCODING_ASCII,                    /**< 7 bit. */
  WMS_ENCODING_IA5,                      /**< 7 bit. */
  WMS_ENCODING_UNICODE,                  /**< 16 bit. */
  WMS_ENCODING_SHIFT_JIS,                /**< 8 or 16 bit. */
  WMS_ENCODING_KOREAN,                   /**< 8 or 16 bit. */
  WMS_ENCODING_LATIN_HEBREW,             /**< 8 bit. */
  WMS_ENCODING_LATIN,                    /**< 8 bit. */
  WMS_ENCODING_GSM_7_BIT_DEFAULT,        /**< 7 bit. */
  WMS_ENCODING_GSM_DATA_CODING_SCHEME,   /**< Varies. */
/** @cond */
  WMS_ENCODING_MAX32        = 0x10000000
/** @endcond */
} wms_user_data_encoding_e_type;


/** CDMA-specific IS-91 EP data types.
*/
typedef enum
{
  WMS_IS91EP_VOICE_MAIL         = 0x82,      /**< Voice mail. */
  WMS_IS91EP_SHORT_MESSAGE_FULL = 0x83,      /**< Short message is full. */ 
  WMS_IS91EP_CLI_ORDER          = 0x84,      /**< Calling line identification. */ 
  WMS_IS91EP_SHORT_MESSAGE      = 0x85,      /**< Short message. */ 
/** @cond */
  WMS_IS91EP_MAX32              = 0x10000000
/** @endcond */
} wms_IS91EP_type_e_type;


/** CDMA-specific priorities for sending a message.
*/
typedef enum
{
  WMS_PRIORITY_NORMAL      = 0,         /**< Normal. */
  WMS_PRIORITY_INTERACTIVE,             /**< Interactive. */
  WMS_PRIORITY_URGENT,                  /**< Urgent. */
  WMS_PRIORITY_EMERGENCY,               /**< Emergency. */
/** @cond */
  WMS_PRIORITY_MAX32       = 0x10000000
/** @endcond */
} wms_priority_e_type;


/** CDMA-specific privacy types for sending a message.
*/
typedef enum
{
  WMS_PRIVACY_NORMAL      = 0,         /**< Normal. */
  WMS_PRIVACY_RESTRICTED,              /**< Restricted. */
  WMS_PRIVACY_CONFIDENTIAL,            /**< Confidential. */
  WMS_PRIVACY_SECRET,                  /**< Secret. */
/** @cond */
  WMS_PRIVACY_MAX32       = 0x10000000
/** @endcond */
} wms_privacy_e_type;


/** CDMA-specific display modes for when to display a message.
*/
typedef enum
{
  WMS_DISPLAY_MODE_IMMEDIATE   = 0, /**< Immediately. */
  WMS_DISPLAY_MODE_DEFAULT     = 1, /**< Default display mode. */
  WMS_DISPLAY_MODE_USER_INVOKE = 2, /**< User-invoked. */
  WMS_DISPLAY_MODE_RESERVED    = 3  /**< Reserved. */
} wms_display_mode_e_type;


/** CDMA-specific alert modes. For pre-IS-637A implementations, alert_mode has 
  values of TRUE or FALSE only. 
*/
typedef enum
{
  WMS_ALERT_MODE_DEFAULT         = 0, /**< Default alert mode. */
  WMS_ALERT_MODE_LOW_PRIORITY    = 1, /**< Low priority. */
  WMS_ALERT_MODE_MEDIUM_PRIORITY = 2, /**< Medium priority. */
  WMS_ALERT_MODE_HIGH_PRIORITY   = 3, /**< High priority. */

  WMS_ALERT_MODE_OFF   = 0,           /**< Off. */
  WMS_ALERT_MODE_ON    = 1            /**< On. @newpage */
} wms_alert_mode_e_type;


/** CDMA-specific download modes.
*/
typedef enum
{
  WMS_DOWNLOAD_MODE_NONE         = 0,    /**< No download mode. */
  WMS_DOWNLOAD_MODE_PP_VAS       = 0x20, /**< Download value added services 
                                              through SMS point-to-point. */
  WMS_DOWNLOAD_MODE_PP_PRL       = 0x30, /**< Download the preferred roaming 
                                              list through SMS point-to-point. */
  WMS_DOWNLOAD_MODE_UPDATE_PRL   = 0x10  /**< Update the preferred roaming list. */
} wms_download_mode_e_type;


/** CDMA-specific delivery status types (IS-637B parameters/fields). 
*/
typedef enum
{
  /* ERROR_CLASS_NONE */
  WMS_DELIVERY_STATUS_ACCEPTED              = 0, /**< No error class. */ 
  WMS_DELIVERY_STATUS_DEPOSITED_TO_INTERNET = 1, /**< No error class. */ 
  WMS_DELIVERY_STATUS_DELIVERED             = 2, /**< No error class. */ 
  WMS_DELIVERY_STATUS_CANCELLED             = 3, /**< No error class. */

  /* ERROR_CLASS_TEMP & PERM */ 
  WMS_DELIVERY_STATUS_NETWORK_CONGESTION  = 4,   /**< Temporary and permanent 
                                                      error classes. */ 
  WMS_DELIVERY_STATUS_NETWORK_ERROR       = 5,   /**< Temporary and permanent 
                                                      error classes. */ 

  /* ERROR_CLASS_PERM */
  WMS_DELIVERY_STATUS_CANCEL_FAILED       = 6,   /**< Permanent error class. */ 
  WMS_DELIVERY_STATUS_BLOCKED_DESTINATION = 7,   /**< Permanent error class. */ 
  WMS_DELIVERY_STATUS_TEXT_TOO_LONG       = 8,   /**< Permanent error class. */ 
  WMS_DELIVERY_STATUS_DUPLICATE_MESSAGE   = 9,   /**< Permanent error class. */ 
  WMS_DELIVERY_STATUS_INVALID_DESTINATION = 10,  /**< Permanent error class. */ 
  WMS_DELIVERY_STATUS_MESSAGE_EXPIRED     = 13,  /**< Permanent error class. */ 
  WMS_DELIVERY_STATUS_UNKNOWN_ERROR       = 0x1F /**< Permanent error class. */ 

/* All the other values are reserved */

} wms_delivery_status_e_type;
/** @} */ /* end_addtogroup cdma_specific_type_defs_group */


/** @ingroup cdma_specific_type_defs_group 
  CDMA-specific type definition.
*/
typedef struct wms_delivery_status_s
{
  wms_error_class_e_type       error_class; /**< Type of error (permanent 
                                                 or temporary). */
  wms_delivery_status_e_type   status;      /**< Status returned for message 
                                                 delivery. @newpagetable */
} wms_delivery_status_s_type;


/** @addtogroup cdma_specific_type_defs_group 
@{ */
/** CDMA-specific dedicated channel service options.
*/
typedef enum
{
  WMS_DC_SO_AUTO = 0,  /**< Automatic. */
  WMS_DC_SO_6    = 6,  /**< SO6. */
  WMS_DC_SO_14   = 14, /**< SO14. @newpage */
/** @cond */
  WMS_DC_SO_MAX,
  WMS_DC_SO_MAX32   = 0x10000000      /* pad to 32 bit int*/
/** @endcond */
} wms_dc_so_e_type;
/** @} */ /* end_addtogroup cdma_specific_type_defs_group */


/** @addtogroup mmbroadcast_group_data_types
@{ */
/** CDMA Commercial Mobile Alert E interface information record type.
*/
typedef enum
{
  WMS_CMAE_RECORD_TYPE_0       = 0x00,  /**< Type 0 elements (alert text). */
  WMS_CMAE_RECORD_TYPE_1       = 0x01,  /**< Type 1 elements. */
  WMS_CMAE_RECORD_TYPE_2       = 0x02,  /**< Type 2 elements. */
/** @cond */
  WMS_CMAE_RECORD_TYPE_MAX32   = 0x10000000
/** @endcond */
} wms_cmae_record_type_e_type;


/** CMAE category.
*/
typedef enum
{
  WMS_CMAE_CATEGORY_GEO       = 0x00,  /**< Geophysical, including landslide. */
  WMS_CMAE_CATEGORY_MET       = 0x01,  /**< Meteorological, including flood. */
  WMS_CMAE_CATEGORY_SAFETY    = 0x02,  /**< General emergency and public safety. */
  WMS_CMAE_CATEGORY_SECURITY  = 0x03,  /**< Law enforcement, military, homeland 
                                            and local/private security. */
  WMS_CMAE_CATEGORY_RESCUE    = 0x04,  /**< Rescue and recovery. */
  WMS_CMAE_CATEGORY_FIRE      = 0x05,  /**< Fire suppression and rescue. */   
  WMS_CMAE_CATEGORY_HEALTH    = 0x06,  /**< Medical and public health. */
  WMS_CMAE_CATEGORY_ENV       = 0x07,  /**< Pollution and other environmental. */
  WMS_CMAE_CATEGORY_TRANSPORT = 0x08,  /**< Public and private transportation. */
  WMS_CMAE_CATEGORY_INFRA     = 0x09,  /**< Utility, telecommunication, other 
                                            nontransport infrastructure. */
  WMS_CMAE_CATEGORY_CBRNE     = 0x0a,  /**< Chemical, biological, radiological, 
                                            nuclear, or high-yield explosive 
                                            threat or attack. */  
  WMS_CMAE_CATEGORY_OTHER     = 0x0b,  /**< Other events. */
/** @cond */
  WMS_CMAE_CATEGORY_MAX32     = 0x10000000
/** @endcond */
} wms_cmae_category_e_type;


/** CMAE response type.
*/
typedef enum
{
  WMS_CMAE_RESPONSE_SHELTER   = 0x00,  /**< Take shelter in place. */
  WMS_CMAE_RESPONSE_EVACUATE  = 0x01,  /**< Relocate. */
  WMS_CMAE_RESPONSE_PREPARE   = 0x02,  /**< Make preparations. */
  WMS_CMAE_RESPONSE_EXECUTE   = 0x03,  /**< Execute a pre-planned activity. */
  WMS_CMAE_RESPONSE_MONITOR   = 0x04,  /**< Pay attention to information 
                                            sources. */
  WMS_CMAE_RESPONSE_AVOID     = 0x05,  /**< Avoid hazard. */
  WMS_CMAE_RESPONSE_ACCESS    = 0x06,  /**< Evaluate the information in this 
                                            message. */
  WMS_CMAE_RESPONSE_NONE      = 0x07,  /**< No action recommended. */
/** @cond */
  WMS_CMAE_RESPONSE_MAX32     = 0x10000000
/** @endcond */
} wms_cmae_response_e_type;


/** CMAE severity.
*/
typedef enum
{
  WMS_CMAE_SEVERITY_EXTREME   = 0x00,   /**< Extraordinary threat to life or 
                                             property. */
  WMS_CMAE_SEVERITY_SEVERE    = 0x01,   /**< Significant threat to life or 
                                             property. */
/** @cond */
  WMS_CMAE_SEVERITY_MAX32     = 0x10000000
/** @endcond */
} wms_cmae_severity_e_type;


/** CMAE urgency.
*/
typedef enum
{
  WMS_CMAE_URGENCY_IMMEDIATE  = 0x00,  /**< Responsive action is to be taken 
                                            immediately. */
  WMS_CMAE_URGENCY_EXPECTED   = 0x01,  /**< Responsive action is to be taken 
                                            soon; within the next hour. */ 
/** @cond */
  WMS_CMAE_URGENCY_MAX32      = 0x10000000
/** @endcond */
} wms_cmae_urgency_e_type;


/** CMAE certainty.
*/
typedef enum
{
  WMS_CMAE_CERTAINTY_OBSERVED  = 0x00,  /**< Determined to have occurred or to 
                                             be ongoing. */
  WMS_CMAE_CERTAINTY_LIKELY    = 0x01,  /**< Likely; probability > ~50%). */
/** @cond */
  WMS_CMAE_CERTAINTY_MAX32     = 0x10000000
/** @endcond */
} wms_cmae_certainty_e_type;


/** Type-specific fields for E_RECORD_TYPE_0.
*/
typedef struct
{
   wms_user_data_encoding_e_type cmae_char_set;
     /**< Identifies the character set for the alert text in the 
          CMAE_Alert_Text element. */
   uint8                         number_of_chars;
     /**< Number of characters in the data array. */
   uint8                         data_len;
     /**< Message data length. */
   uint8                         data[ WMS_CDMA_USER_DATA_MAX ];
     /**< Message user data. */
   uint8                         padding_bits;
     /**< Message padding bits. */
} wms_cmae_record_type_0_s_type;


/** Type-specific fields for E_RECORD_TYPE_1.
*/
typedef struct
{
   wms_cmae_category_e_type  cmae_category;
     /**< Identifies the category of the CMAE alert. */
   wms_cmae_response_e_type  cmae_response_type;
     /**< Identifies the response indicated for the CMAE alert. */
   wms_cmae_severity_e_type  cmae_severity;
     /**< Identifies the severity of the CMAE alert. */
   wms_cmae_urgency_e_type   cmae_urgency;
     /**< Identifies the urgency of the CMAE alert. */
   wms_cmae_certainty_e_type cmae_certainty;
     /**< Identifies the certainty of the CMAE alert. @newpagetable */
} wms_cmae_record_type_1_s_type;

/** Type-specific fields for E_RECORD_TYPE_2
*/
typedef struct
{
   wms_message_number_type cmae_id; 
     /**< A number uniquely identifying this message. */
   uint8                   cmae_alert_handling;
     /**< Specifies if this alert message requires special handling. */
   wms_timestamp_s_type    cmae_expires;
     /**< Identifies the expiration date and time of the CMAE alert. */
   wms_language_e_type     cmae_language;
     /**< Identifies the language of the alert message in the CMAE_Alert_Text 
          element. */
} wms_cmae_record_type_2_s_type;

/** CDMA Commercial Mobile Alert E interface (CMAE) record type. 
*/
typedef struct wms_cmae_record_s
{
  wms_cmae_record_type_e_type        cmae_record_type;   /**< CMAE record type. */

  /** Used by the #wms_cmae_record_s structure. */
  union wms_cmas_record_u
  {
    wms_cmae_record_type_0_s_type    cmae_record_type_0; /**< CMAE record type 0. */
    wms_cmae_record_type_1_s_type    cmae_record_type_1; /**< CMAE record type 1. */
    wms_cmae_record_type_2_s_type    cmae_record_type_2; /**< CMAE record type 2. */
  }u; /**< Used by the #wms_cmae_record_s structure. */
} wms_cmae_record_s_type;


/** CMAS message.
*/
typedef struct 
{
  uint8 cmae_protocol_version;                     /**< CMAE protocol version. */
  uint8 cmae_record_count;                         /**< Number of records. */
  wms_cmae_record_s_type came_records[WMS_MAX_CMAE_RECORDS];
                                                   /**< Records content. */
} wms_cmas_msg_s_type;

/** @} */ /* end_addtogroup mmbroadcast_group_data_types */


/* <EJECT> */
/* ========================================================================= */
/* ===================== GSM/WCDMA specific type definitions =============== */
/* ========================================================================= */

/** @addtogroup gw_specific_type_defs_group 
@{ */
/** GSM/WCDMA-specific TPDU types.
*/
typedef enum
{
  WMS_TPDU_DELIVER = 0,          /**< Deliver message TPDU type; from SC to MS. */
  WMS_TPDU_DELIVER_REPORT_ACK,   /**< Report acknowledgment of delivery TPDU 
                                      type; from MS to SC. */
  WMS_TPDU_DELIVER_REPORT_ERROR, /**< Report error in delivery TPDU type; from 
                                      MS to SC. */
  WMS_TPDU_SUBMIT,               /**< From MS to SC. */
  WMS_TPDU_SUBMIT_REPORT_ACK,    /**< Report acknowledgment of submission TPDU 
                                      type; from SC to MS. */
  WMS_TPDU_SUBMIT_REPORT_ERROR,  /**< Report error in submission TPDU type; from 
                                      SC to MS. */
  WMS_TPDU_STATUS_REPORT,        /**< From SC to MS. */
  WMS_TPDU_COMMAND,              /**< From MS to SC. */
  WMS_TPDU_RESERVED,             /**< From SC to MS. */

/** @cond */
  WMS_TPDU_MAX,
  WMS_TPDU_NONE,
  WMS_TPDU_MAX32 = 0x10000000
/** @endcond */
} wms_gw_tpdu_type_e_type;
/*~ SENTINEL wms_gw_tpdu_type_e_type.WMS_TPDU_MAX */
/** @} */ /* end_addtogroup gw_specific_type_defs_group */

/** @ingroup gw_specific_type_defs_group 
  GSM/WCDMA-specific TPDU types.
*/
#define WMS_IS_MO_TPDU_TYPE( type )      \
          ( (type) == WMS_TPDU_SUBMIT ||   \
            (type) == WMS_TPDU_COMMAND )


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** GSM/WCDMA-specific GWC command types. Refer to 3GPP TS 23.040, Section 9.2.3.19.
*/
typedef enum
{
  WMS_GW_COMMAND_ENQUIRY                = 0x00, /**< Enquiry. */
  WMS_GW_COMMAND_CANCEL_STATUS_REPORT   = 0x01, /**< Cancel status report. */
  WMS_GW_COMMAND_DELETE_SM              = 0x02, /**< Delete short message. */
  WMS_GW_COMMAND_ENABLE_STATUS_REPORT   = 0x03, /**< Enable status report. @newpage */
/** @cond */
  WMS_GW_COMMAND_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */

  /* reserved: 0x04 - 0x1f */
  /* specific to each SC: 0xe0 - 0xff */
} wms_gw_command_e_type;


/** GSM/WCDMA-specific Transport Protocol (TP) failure-cause types.  
  Refer to 3GPP TS 23.040, Section 9.2.3.22.

  Reserved address blocks: 
  - 0x00 through 0x7f
  - 0x83 through 0x8e
  - 0x92 through 0x9e
  - 0xa2 through 0xae
  - 0xb1 through 0xbf
  - 0xc8 through 0xcf
  - 0xd6 through 0xdf
  
  Values from 0xe0 through 0xfe are for errors specific to an application.
*/
typedef enum
{
  /* values from 0x00 to 0x7f are reserved */

  WMS_TP_CAUSE_TELEMATIC_NOT_SUPPORTED    = 0x80,
     /**< TP-PID: mobile-originated Protocol Identifier Data (PID) error. */
  WMS_TP_CAUSE_SM_TYPE0_NOT_SUPPORTED     = 0x81, /**< TP-PID: MO and MT PID 
                                                       error. */
  WMS_TP_CAUSE_CANNOT_REPLACE_SM          = 0x82, /**< TP-PID: MO and MT PID 
                                                       error. */
  /* ... values from 0x83 to 0x8e are reserved .. */

  WMS_TP_CAUSE_UNSPECIFIED_PID_ERROR      = 0x8f, /**< TP-PID: MO and MT PID 
                                                       error. */
  WMS_TP_CAUSE_ALPHABET_NOT_SUPPORTED     = 0x90, /**< TP-DCS: MO DCS error. */
  WMS_TP_CAUSE_MSG_CLASS_NOT_SUPPORTED    = 0x91, /**< TP-DCS: MT DCS error. */

  /* ... values from 0x92 to 0x9e are reserved .. */

  WMS_TP_CAUSE_UNSPECIFIED_DCS_ERROR      = 0x9f, /**< TP-DCS: MO and MT DCS 
                                                       error. */
  WMS_TP_CAUSE_CANNOT_PERFORM_COMMAND     = 0xa0, /**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_COMMAND_NOT_SUPPORTED      = 0xa1, /**< TP-Command: MO command 
                                                       error. */
  /* ... values from 0xa2 to 0xae are reserved .. */

  WMS_TP_CAUSE_UNSPECIFIED_CMD_ERROR      = 0xaf, /**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_TPDU_NOT_SUPPORTED         = 0xb0, /**< TP-Command: MO and MT 
                                                       command error. */
  /* ... values from 0xb1 to 0xbf are reserved .. */

  WMS_TP_CAUSE_SC_BUSY                    = 0xc0, /**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_NO_SC_SUBSCRIPTION         = 0xc1, /**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_SC_FAILURE                 = 0xc2, /**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_INVALID_SME_ADDRESS        = 0xc3,/**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_DESTINATION_SME_BARRED     = 0xc4, /**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_REJECTED_DUPLICATE_SM      = 0xc5, /**< TP-Command: MO command 
                                                       error. */
  WMS_TP_CAUSE_VPF_NOT_SUPPORTED          = 0xc6, /**< TP-Command: MO command 
                                                       error, Validity Period 
                                                       Format. */
  WMS_TP_CAUSE_VP_NOT_SUPPORTED           = 0xc7, /**< TP-Command: MO command 
                                                       error, Validity Period. */
  /* values from 0xc8 to 0xcf are reserved */

  WMS_TP_CAUSE_SIM_FULL                   = 0xd0, /**< TP-Command: MT command 
                                                       error. */
  WMS_TP_CAUSE_NO_SM_STORAGE_IN_SIM       = 0xd1, /**< TP-Command: MT command 
                                                       error. */
  WMS_TP_CAUSE_ERROR_IN_MS                = 0xd2, /**< TP-Command: MT command 
                                                       error. */
  WMS_TP_CAUSE_MEMORY_FULL                = 0xd3, /**< TP-Command: MT command 
                                                       error. */
  WMS_TP_CAUSE_SAT_BUSY                   = 0xd4, /**< TP-Command: MT command 
                                                       error. */
  WMS_TP_CAUSE_SIM_DATA_DOWNLOAD_ERROR    = 0xd5, /**< TP-Command: MT command 
                                                       error. */
  /* values from 0xd6 to 0xdf are reserved */
  /* values from 0xe0 to 0xfe are for errors specific to an application */

  WMS_TP_CAUSE_UNSPECIFIED_ERROR          = 0xff /**< TP-Command: MO and MT 
                                                      command error. */
} wms_tp_cause_e_type;


/** GSM/WCDMA-specific Relay Protocol (RP) cause types. Refer to TS 24.011, Section 
  8.2.5.4.

  Error classifications:
  - All MO errors other than those listed below shall be treated as 
    WMS_RP_CAUSE_TEMPORARY_FAILURE.
  - All MT errors other than those listed below shall be treated as 
    WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED.
  - All SMMA errors other than those listed below shall be treated as 
    WMS_RP_CAUSE_TEMPORARY_FAILURE. 
  - All CP errors other than those listed below shall be treated as 
    WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED.
*/
typedef enum
{
  WMS_RP_CAUSE_UNASSIGNED_NUMBER                  = 1,  /**< MO error. */
  WMS_RP_CAUSE_OPERATOR_DETERMINED_BARRING        = 8,  /**< MO error. */
  WMS_RP_CAUSE_CALL_BARRED                        = 10, /**< MO error. */
  WMS_RP_CAUSE_RESERVED_11                        = 11, /**< MO error. */
  WMS_RP_CAUSE_NETWORK_FAILURE                    = 17, /**< CP error. */
  WMS_RP_CAUSE_SM_REJECTED                        = 21, /**< MO error. */
  WMS_RP_CAUSE_MEMORY_EXCEEDED                    = 22, /**< MT and CP error. */
  WMS_RP_CAUSE_DESTINATION_OUT_OF_ORDER           = 27, /**< MO error. */
  WMS_RP_CAUSE_UNIDENTIFIED_SUBSCRIBER            = 28, /**< MO error. */
  WMS_RP_CAUSE_FACILITY_REJECTED                  = 29, /**< MO error. */
  WMS_RP_CAUSE_UNKNOWN_SUBSCRIBER                 = 30, /**< MO and SMMA error. */
  WMS_RP_CAUSE_NETWORK_OUT_OF_ORDER               = 38, /**< MO and SMMA error. */
  WMS_RP_CAUSE_TEMPORARY_FAILURE                  = 41, /**< MO and SMMA error. */
  WMS_RP_CAUSE_CONGESTION                         = 42, /**< MO and SMMA error. */
  WMS_RP_CAUSE_RESOURCES_UNAVAILABLE_UNSPECIFIED  = 47, /**< MO and SMMA error. */
  WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED  = 50, /**< MO error. */
  WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED = 69, /**< MO and SMMA error. */
  WMS_RP_CAUSE_INVALID_SM_TRANSFER_REF_VALUE      = 81, /**< MO/MT/CP error. */
  WMS_RP_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE     = 95, /**< MO/MT/SMMA/CP error. */
  WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION      = 96, /**< MO/MT/SMMA/CP error. */
  WMS_RP_CAUSE_MESSAGE_TYPE_NON_EXISTENT          = 97, /**< MO/MT/SMMA/CP error. */
  WMS_RP_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE        = 98, /**< MO/MT/SMMA/CP error. */
  WMS_RP_CAUSE_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 99, /**< MO/MT/SMMA/CP error. */
  WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED         = 111,/**< MO/MT/SMMA/CP error. */
  WMS_RP_CAUSE_INTERWORKING_UNSPECIFIED           = 127, /**< MO and SMMA error. @newpage */
  WMS_RP_CAUSE_TEMP_REJ                           = 216 /**< MO temporary reject with expectation to retry in 1sec. */
} wms_rp_cause_e_type;


/** GSM/WCDMA-specific Transport Protocol status types. 
  Refer to 3GPP TS 23.040, Section 9.2.3.15.

  Reserved address blocks: 
  - 0x03 through 0x0f
  - 0x10 through 0x1f (specific to each SC)
  - 0x26 through 0x2f
  - 0x30 through 0x3f (specific to each SC)
  - 0x4a through 0x4f
  - 0x50 through 0x5f (specific to each SC)
  - 0x66 through 0x6f
  - 0x70 through 0x7f (specific to each SC)
  - 0x80 through 0xff
*/
typedef enum
{
  WMS_TP_STATUS_RECEIVED_OK                   = 0x00, 
    /**< Short message transaction completed */
  WMS_TP_STATUS_UNABLE_TO_CONFIRM_DELIVERY    = 0x01, 
    /**< Short message forwarded by the SC to the SME, but the SC is unable to 
        confirm delivery. */
  WMS_TP_STATUS_REPLACED                      = 0x02, 
    /**< Short message replaced by the SC. */

  /* reserved: 0x03 - 0x0f */
  /* specific to each SC: 0x10 - 0x1f */

  WMS_TP_STATUS_TRYING_CONGESTION             = 0x20,
    /**< Temporary error; the SC is still trying to transfer the SM. */
  WMS_TP_STATUS_TRYING_SME_BUSY               = 0x21,
    /**< Temporary error; the SC is still trying to transfer the SM. */
  WMS_TP_STATUS_TRYING_NO_RESPONSE_FROM_SME   = 0x22,
    /**< Temporary error; the SC is still trying to transfer the SM. */
  WMS_TP_STATUS_TRYING_SERVICE_REJECTED       = 0x23,
    /**< Temporary error; the SC is still trying to transfer the SM. */
  WMS_TP_STATUS_TRYING_QOS_NOT_AVAILABLE      = 0x24,
    /**< Temporary error; the SC is still trying to transfer the SM. */
  WMS_TP_STATUS_TRYING_SME_ERROR              = 0x25,
    /**< Temporary error; the SC is still trying to transfer the SM. */

  /* reserved: 0x26 - 0x2f */
  /* specific to each SC: 0x30 - 0x3f */

  WMS_TP_STATUS_PERM_REMOTE_PROCEDURE_ERROR   = 0x40,
    /**< Permanent error; the SC is not making any more attempts. */
  WMS_TP_STATUS_PERM_INCOMPATIBLE_DEST        = 0x41,
    /**< Permanent error; the SC is not making any more attempts. */
  WMS_TP_STATUS_PERM_REJECTED_BY_SME          = 0x42,
    /**< Permanent error; the SC is not making any more attempts. */
  WMS_TP_STATUS_PERM_NOT_OBTAINABLE           = 0x43,
    /**< Permanent error; the SC is not making any more attempts. */
  WMS_TP_STATUS_PERM_QOS_NOT_AVAILABLE        = 0x44,
    /**< Permanent error; the SC is not making any more attempts. */
  WMS_TP_STATUS_PERM_NO_INTERWORKING          = 0x45,
    /**< Permanent error; the SC is not making any more attempts. */
  WMS_TP_STATUS_PERM_VP_EXPIRED               = 0x46,
    /**< Permanent validity period error; the SC is not making any more 
         attempts. */
  WMS_TP_STATUS_PERM_DELETED_BY_ORIG_SME      = 0x47,
    /**< Permanent error; the SC is not making any more attempts. @newpage */
  WMS_TP_STATUS_PERM_DELETED_BY_SC_ADMIN      = 0x48,
    /**< Permanent error; the SC is not making any more attempts. */
  WMS_TP_STATUS_PERM_SM_NO_EXISTING           = 0x49,
    /**< Permanent error; the SC is not making any more attempts. */

  /* reserved: 0x4a - 0x4f */
  /* specific to each SC: 0x50 - 0x5f */

  WMS_TP_STATUS_TEMP_CONGESTION               = 0x60,
    /**< Temporary error; the SC is not making any more attempts. */
  WMS_TP_STATUS_TEMP_SME_BUSY                 = 0x61,
    /**< Temporary error; the SC is not making any more attempts. */
  WMS_TP_STATUS_TEMP_NO_RESPONSE_FROM_SME     = 0x62,
    /**< Temporary error; the SC is not making any more attempts. */
  WMS_TP_STATUS_TEMP_SERVICE_REJECTED         = 0x63,
    /**< Temporary error; the SC is not making any more attempts. */
  WMS_TP_STATUS_TEMP_QOS_NOT_AVAILABLE        = 0x64,
    /**< Temporary error; the SC is not making any more attempts. */
  WMS_TP_STATUS_TEMP_SME_ERROR                = 0x65,
    /**< Temporary error; the SC is not making any more attempts. */

  /* reserved: 0x66 - 0x6f */
  /* specific to each SC: 0x70 - 0x7f */
  /* reserved: 0x80 - 0xff */

/** @cond */
  WMS_TP_STATUS_LAST = 0xFF,
  WMS_TP_STATUS_MAX32 = 0x10000000 /* pad to 32 bit int */
/** @endcond */
} wms_tp_status_e_type;


/** GSM/WCDMA-specific PID. Refer to 3GPP TS 23.040, Section 9.2.3.9. 

  WMS_PID_SC_SPECIFIC_1 through WMS_PID_SC_SPECIFIC_7 can hold values specific to 
  each SC. The usage is based on mutual agreement between the SME and the SC. 
  Seven combinations are available for each SC.
*/
typedef enum
{
  /* values from 0x00 to 0x1f are for SM-AL protocols */
  WMS_PID_DEFAULT           = 0x00, /**< SM-AL protocols: default value. */

  /* values from 0x20 to 0x3f are for telematic interworking */
  WMS_PID_IMPLICIT          = 0x20,
    /**< Telematic interworking: device type is specific to this SC, or it can 
         be concluded on the basis of the address. */
  WMS_PID_TELEX             = 0x21, /**< Telematic interworking. */
  WMS_PID_G3_FAX            = 0x22, /**< Telematic interworking: group 3 telefax. */
  WMS_PID_G4_FAX            = 0x23, /**< Telematic interworking: group 4 telefax. */
  WMS_PID_VOICE_PHONE       = 0x24, /**< Telematic interworking. */
  WMS_PID_ERMES             = 0x25, /**< Telematic interworking: European radio 
                                         messaging system. */
  WMS_PID_NAT_PAGING        = 0x26, /**< Telematic interworking: national paging 
                                         system (known to the SC). */
  WMS_PID_VIDEOTEX          = 0x27, /**< Telematic interworking: videotex (T.100 
                                         [20] /T.101 [21]). */
  WMS_PID_TELETEX_UNSPEC    = 0x28, /**< Telematic interworking. */
  WMS_PID_TELETEX_PSPDN     = 0x29, /**< Telematic interworking: packet-switched 
                                         packet data network. */
  WMS_PID_TELETEX_CSPDN     = 0x2a, /**< Telematic interworking: circuit-switched 
                                         packet data network.*/
  WMS_PID_TELETEX_PSTN      = 0x2b, /**< Telematic interworking: analog public 
                                         switched telephone network. */
  WMS_PID_TELETEX_ISDN      = 0x2c, /**< Telematic interworking: digital 
                                         integrated services digital network. */
  WMS_PID_UCI               = 0x2d, /**< Telematic interworking: Universal 
                                         computer interface, ETSI DE/PS 3 01 3.*/
  WMS_PID_RESERVED_0x2e     = 0x2e, /**< Telematic interworking. */
  WMS_PID_RESERVED_0x2f     = 0x2f, /**< Telematic interworking.*/
  WMS_PID_MSG_HANDLING      = 0x30, /**< Telematic interworking. */
  WMS_PID_X400              = 0x31, /**< Telematic interworking: any public X.400-
                                         based message handling system. */
  WMS_PID_INTERNET_EMAIL    = 0x32, /**< Telematic interworking. */
  WMS_PID_RESERVED_0x33     = 0x33, /**< Telematic interworking. */
  WMS_PID_RESERVED_0x34     = 0x34, /**< Telematic interworking. */
  WMS_PID_RESERVED_0x35     = 0x35, /**< Telematic interworking. */
  WMS_PID_RESERVED_0x36     = 0x36, /**< Telematic interworking. */
  WMS_PID_RESERVED_0x37     = 0x37, /**< Telematic interworking. */
  WMS_PID_SC_SPECIFIC_1     = 0x38, /**< Telematic interworking. */
  WMS_PID_SC_SPECIFIC_2     = 0x39, /**< Telematic interworking. */
  WMS_PID_SC_SPECIFIC_3     = 0x3a, /**< Telematic interworking. */
  WMS_PID_SC_SPECIFIC_4     = 0x3b, /**< Telematic interworking. */
  WMS_PID_SC_SPECIFIC_5     = 0x3c, /**< Telematic interworking. */
  WMS_PID_SC_SPECIFIC_6     = 0x3d, /**< Telematic interworking. */
  WMS_PID_SC_SPECIFIC_7     = 0x3e, /**< Telematic interworking. */
  WMS_PID_GSM_UMTS          = 0x3f, /**< Telematic interworking. */

  /* values from 0x40 to 0x7f: */
  WMS_PID_SM_TYPE_0         = 0x40, /**< Short message type. */
  WMS_PID_REPLACE_SM_1      = 0x41, /**< Replace short message type. */
  WMS_PID_REPLACE_SM_2      = 0x42, /**< Replace short message type. */
  WMS_PID_REPLACE_SM_3      = 0x43, /**< Replace short message type. */
  WMS_PID_REPLACE_SM_4      = 0x44, /**< Replace short message type. */
  WMS_PID_REPLACE_SM_5      = 0x45, /**< Replace short message type. */
  WMS_PID_REPLACE_SM_6      = 0x46, /**< Replace short message type. */
  WMS_PID_REPLACE_SM_7      = 0x47, /**< Replace short message type. */
  /* ... values reserved not listed ... */
  WMS_PID_EMS               = 0x5e, /**< Enhanced message service (Obsolete).*/
  WMS_PID_RETURN_CALL       = 0x5f, /**< Return call. */
  /* ... values reserved not listed ... */
  WMS_PID_ANSI136_R_DATA    = 0x7c, /**< ANSI-136 R-data. */
  WMS_PID_ME_DATA_DOWNLOAD  = 0x7d, /**< ME data download. */
  WMS_PID_ME_DEPERSONALIZE  = 0x7e, /**< ME depersonalize. */
  WMS_PID_SIM_DATA_DOWNLOAD = 0x7f, /**< SIM data download. */

/** @cond */
  WMS_PID_E_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */

  /* values from 0x80 to 0xbf are reserved */
  /* values from 0xc0 to 0xff are for SC specific use */
} wms_pid_e_type;


/** GSM/WCDMA-specific alphabet sets.
*/
typedef enum
{
  WMS_GW_ALPHABET_7_BIT_DEFAULT, /**< GSM/WCDMA default. */
  WMS_GW_ALPHABET_8_BIT,         /**< 8-bit. */
  WMS_GW_ALPHABET_UCS2,          /**< Universal character set 16-bit variant. */
/** @cond */
  WMS_GW_ALPHABET_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_gw_alphabet_e_type;

/**MWI STATES
*/
typedef enum
{
  WMS_MWI_STATE_INIT    = 0x0001,  /*MWI cache is initialized and has a valid content*/
  WMS_MWI_STATE_ERROR   = 0x0002,  /*MWI cache is invalid*/
  WMS_MWI_STATE_MAX,
  WMS_MWI_STATE_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} wms_mwi_state_e_type;

/** GSM/WCDMA-specific message waiting actions.
*/
typedef enum
{
  WMS_GW_MSG_WAITING_NONE,      /**< No messages waiting. */
  WMS_GW_MSG_WAITING_DISCARD,   /**< Discard message after updating indication. */
  WMS_GW_MSG_WAITING_STORE,     /**< Store message after updating indication. */
  WMS_GW_MSG_WAITING_NONE_1111, /**< Number of messages waiting. @newpage */
/** @cond */
  WMS_GW_MSG_WAITING_MAX32 = 0x10000000 /* pad to 32 bit int */
/** @endcond */
} wms_gw_msg_waiting_e_type;

/** GSM/WCDMA-specific message waiting types.
*/
typedef enum
{
  WMS_GW_MSG_WAITING_VOICEMAIL, /**< Voice mail. */
  WMS_GW_MSG_WAITING_FAX,       /**< FAX. */
  WMS_GW_MSG_WAITING_EMAIL,     /**< Email. */
  WMS_GW_MSG_WAITING_OTHER,     /**< Extended Message Type Waiting (equivalent to 
                                     Other in 3GPP TS 23.038). */
  WMS_GW_MSG_WAITING_VIDEOMAIL, /**< Video mail. */
  WMS_GW_MSG_WAITING_KIND_MAX,
/** @cond */
  WMS_GW_MSG_WAITING_KIND_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_gw_msg_waiting_kind_e_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @ingroup gw_specific_type_defs_group 
  Message waiting indication information.
*/
typedef struct 
{
  wms_gw_msg_waiting_kind_e_type  type;      /**< Message indication type. */
  boolean                         is_active; /**< Set indication active/inactive. */
  uint32                          count;     /**< Message count. */
} wms_gw_mwi_info_s_type ;


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** GSM/WCDMA-specific validity formats.
*/
typedef enum
{
  WMS_GW_VALIDITY_NONE = 0,     /**< No validity format. */
  WMS_GW_VALIDITY_RELATIVE = 2, /**< Relative. */
  WMS_GW_VALIDITY_ABSOLUTE = 3, /**< Absolute. */
  WMS_GW_VALIDITY_ENHANCED = 1, /**< Enhanced. */
/** @cond */
  WMS_GW_VALIDITY_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_gw_validity_format_e_type;


/** SSM/WCDMA-specific MS User Data Header (UDH) IDs.

  Reserved address blocks:
  - 15 through 1F -- Reserved for future EMS.
  - 21 through 23 -- Reserved for future use.
  - 26 through 6F -- Reserved for future use.
  - 70 through 7f -- Reserved for (U)SIM toolkit security headers.
  - 80 through 9F -- SME to SME specific use.
  - A0 through BF -- Reserved for future use.
  - C0 through DF -- SC-specific use.
  - E0 through FF -- Reserved for future use.
*/
typedef enum
{
  WMS_UDH_CONCAT_8         = 0x00, /**< Concatenated short messages, 8-bit 
                                        reference number. */
  WMS_UDH_SPECIAL_SM,              /**< Special SMS message indication. */

  /* 02 - 03    Reserved */

  WMS_UDH_PORT_8           = 0x04, /**< Application port addressing scheme, 
                                        8-bit address. */
  WMS_UDH_PORT_16,                 /**< Application port addressing scheme, 
                                        16-bit address. */
  WMS_UDH_SMSC_CONTROL,            /**< SMSC control. */
  WMS_UDH_SOURCE,                  /**< Source. */
  WMS_UDH_CONCAT_16,               /**< Concatenated short message, 16-bit 
                                        reference number. */
  WMS_UDH_WCMP,                    /**< Wireless Control Message Protocol. */
  WMS_UDH_TEXT_FORMATING,          /**< Text formatting. */
  WMS_UDH_PRE_DEF_SOUND,           /**< Predefined sound. */
  WMS_UDH_USER_DEF_SOUND,          /**< User-defined sound. */
  WMS_UDH_PRE_DEF_ANIM,            /**< Predefined animation. */
  WMS_UDH_LARGE_ANIM,              /**< Large animation. */
  WMS_UDH_SMALL_ANIM,              /**< Small animation. */
  WMS_UDH_LARGE_PICTURE,           /**< Large picture. */
  WMS_UDH_SMALL_PICTURE,           /**< Small picture. */
  WMS_UDH_VAR_PICTURE,             /**< Variable-length picture. */

  WMS_UDH_USER_PROMPT      = 0x13, /**< User prompt. */
  WMS_UDH_EXTENDED_OBJECT  = 0x14, /**< Extended Object. */

  /* 15 - 1F    Reserved for future EMS */

  WMS_UDH_RFC822           = 0x20, /**< RFC 822 E-Mail Header. */

  WMS_UDH_NAT_LANG_SS      = 0x24, /**< National language single shift. */
  WMS_UDH_NAT_LANG_LS      = 0x25, /**< National language locking shift. */

  /*  21 - 23, 26 - 6F    Reserved for future use */
  /*  70 - 7f    Reserved for (U)SIM Toolkit Security Headers */
  /*  80 - 9F    SME to SME specific use */
  /*  A0 - BF    Reserved for future use */
  /*  C0 - DF    SC specific use */
  /*  E0 - FF    Reserved for future use */

  WMS_UDH_OTHER            = 0xFFFF, /**< For unsupported or proprietary headers. */

/** @cond */
  WMS_UDH_ID_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_udh_id_e_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** UDH short message information element.
*/
typedef struct wms_udh_concat_8_s
{
  uint8       msg_ref;
    /**< Reference number for a particular concatenated short message. It is 
         constant for every short message that makes up a particular concatenated 
         short message. */
  uint8       total_sm;
    /**< Total number of short messages within the concatenated short message. 
         The value shall start at 1 and remain constant for every short message 
         that makes up the concatenated short message. If the value is zero, the 
         receiving entity shall ignore the entire Information Element. */
  uint8      seq_num;
    /**< Sequence number of a particular short message within the concatenated 
         short message. The value shall start at 1 and increment by one for every 
         short message sent within the concatenated short message. If the value 
         is zero or the value is greater than the value in octet 2, the receiving 
         entity shall ignore the entire Information Element. */
} wms_udh_concat_8_s_type;


/** UDH special short message information element.
*/
typedef struct wms_udh_special_sm_s
{
  wms_gw_msg_waiting_e_type                  msg_waiting;
    /**< Waiting action. */
  wms_gw_msg_waiting_kind_e_type             msg_waiting_kind;
    /**< Type of message waiting. */
  uint8                                      message_count;
    /**< Number of messages waiting that are of the type specified in octet 1.*/

} wms_udh_special_sm_s_type;


/** UDH Wireless Application Protocol 8-bit port information element.
*/
typedef struct wms_udh_wap_8_s
{
  uint8  dest_port; /**< Receiving port (i.e., application) in the receiving 
                         device. */
  uint8  orig_port; /**< Sending port (i.e., application) in the sending 
                         device. */
} wms_udh_wap_8_s_type;


/** UDH Wireless Application Protocol 16-bit port information element.
*/
typedef struct wms_udh_wap_16_s
{
  uint16  dest_port; /**< Receiving port (i.e., application) in the receiving 
                          device. */
  uint16  orig_port; /**< Sending port (i.e., application) in the sending 
                          device. */
} wms_udh_wap_16_s_type;


/** UDH 16-bit concatenated information element.
*/
typedef struct wms_udh_concat_16_s
{
  uint16      msg_ref;  /**< Concatenated short message reference number. */
  uint8       total_sm; /**< Maximum number of short messages in the 
                             concatenated short message. */
  uint8       seq_num;  /**< Sequence number of the current short message. */
} wms_udh_concat_16_s_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @addtogroup gw_specific_type_defs_group 
@{ */
/* EMS Headers */
/* --------------------------- */
/** GSM/WCDMA-specific alignment definitions for text formatting.
*/
typedef enum
{
  WMS_UDH_LEFT_ALIGNMENT = 0, /**< Left alignment. */
  WMS_UDH_CENTER_ALIGNMENT,   /**< Center alignment. */
  WMS_UDH_RIGHT_ALIGNMENT,    /**< Right alignment. */
  WMS_UDH_DEFAULT_ALIGNMENT,  /**< Language-dependent (default) alignment. */
  WMS_UDH_MAX_ALIGNMENT,      /**< Maximum alignment. */
/** @cond */
  WMS_UDH_ALIGNMENT_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_udh_alignment_e_type;


/** GSM/WCDMA-specific font size definitions for text formatting.
*/
typedef enum
{
  WMS_UDH_FONT_NORMAL = 0, /**< Default font size. */
  WMS_UDH_FONT_LARGE,      /**< Large. */
  WMS_UDH_FONT_SMALL,      /**< Small. */
  WMS_UDH_FONT_RESERVED,   /**< Reserved. */
/** @cond */
  WMS_UDH_FONT_MAX,
  WMS_UDH_FONT_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_udh_font_size_e_type;


/** GSM/WCDMA-specific color definitions for text formatting. The color values 
  defined are simple primary and secondary colors plus four levels of grey 
  (black to white). Bright colors have a higher intensity than dark colors.
*/
typedef enum
{
  WMS_UDH_TEXT_COLOR_BLACK          = 0x0, /**< Black. */
  WMS_UDH_TEXT_COLOR_DARK_GREY      = 0x1, /**< Dark grey. */
  WMS_UDH_TEXT_COLOR_DARK_RED       = 0x2, /**< Dark red. */
  WMS_UDH_TEXT_COLOR_DARK_YELLOW    = 0x3, /**< Dark yellow. */
  WMS_UDH_TEXT_COLOR_DARK_GREEN     = 0x4, /**< Dark green. */
  WMS_UDH_TEXT_COLOR_DARK_CYAN      = 0x5, /**< Dark cyan. */
  WMS_UDH_TEXT_COLOR_DARK_BLUE      = 0x6, /**< Dark blue. */
  WMS_UDH_TEXT_COLOR_DARK_MAGENTA   = 0x7, /**< Dark magenta. */
  WMS_UDH_TEXT_COLOR_GREY           = 0x8, /**< Grey. */
  WMS_UDH_TEXT_COLOR_WHITE          = 0x9, /**< White. */
  WMS_UDH_TEXT_COLOR_BRIGHT_RED     = 0xA, /**< Bright red. */
  WMS_UDH_TEXT_COLOR_BRIGHT_YELLOW  = 0xB, /**< Bright yellow. */
  WMS_UDH_TEXT_COLOR_BRIGHT_GREEN   = 0xC, /**< Bright green. */
  WMS_UDH_TEXT_COLOR_BRIGHT_CYAN    = 0xD, /**< Bright cyan. */
  WMS_UDH_TEXT_COLOR_BRIGHT_BLUE    = 0xE, /**< Bright blue. */
  WMS_UDH_TEXT_COLOR_BRIGHT_MAGENTA = 0xF, /**< Bright magenta. */
/** @cond */
  WMS_UDH_TEXT_COLOR_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_udh_text_color_e_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** UDH text formatting information element.
*/
typedef struct wms_udh_text_formating_s
{
  uint8                     start_position;
    /**< Starting position of the text formatting. */
  uint8                     text_formatting_length; 
    /**< Gives the number of formatted characters or sets a default text 
         formatting. */
  wms_udh_alignment_e_type  alignment_type;
    /**< Indicated by bit 0 and bit 1 of the formatting mode octet. */
  wms_udh_font_size_e_type  font_size;
    /**< Indicated by bit 3 and bit 2 of the formatting mode octet. */
  boolean                   style_bold;
    /**< Indicated by bit 4 of the formatting mode octet. */
  boolean                   style_italic;
    /**< Indicated by bit 5 of the formatting mode octet. */
  boolean                   style_underlined;
    /**< Indicated by bit 6 of the formatting mode octet. */
  boolean                   style_strikethrough;
    /**< Indicated by bit 7 of the formatting mode octet. */
  boolean                   is_color_present;
    /**< If FALSE, ignores the following color information. */
  wms_udh_text_color_e_type text_color_foreground;
    /**< Defines the text foreground color. */
  wms_udh_text_color_e_type text_color_background;
    /**< Defines the text background color. @newpagetable */
} wms_udh_text_formating_s_type;


/* --------------------------- */
/* Sound Related Definitions   */
/* --------------------------- */
/** UDH predefined sound information element.
*/
typedef struct wms_udh_pre_def_sound_s
{
  uint8       position;   /**< Number of characters from the beginning of the 
                               SM data after which the sound shall be played. */
  uint8       snd_number; /**< Sound number encoded as an integer value. */
} wms_udh_pre_def_sound_s_type;


/** UDH user-defined sound information element.
*/
typedef struct wms_udh_user_def_sound_s
{
  uint8       data_length;
    /**< Length of the user-defined sound. */
  uint8       position;
    /**< Indicates in the SM data the instant after which the sound shall be 
         played. */
  uint8       user_def_sound[WMS_UDH_MAX_SND_SIZE];
    /**< Number of the user-defined sound. */
} wms_udh_user_def_sound_s_type;


/* --------------------------- */
/* Picture Related Definitions */
/* --------------------------- */
/** UDH large picture information element.
*/
typedef struct wms_udh_large_picture_data_s
{
  uint8             position;
    /**< Number of characters from the beginning of the SM data after which the 
         picture shall be displayed. */
  uint8             data[WMS_UDH_LARGE_PIC_SIZE];
    /**< Data for the large picture. */
} wms_udh_large_picture_data_s_type;


/** UDH small picture information element.
*/
typedef struct wms_udh_small_picture_data_s
{
  uint8               position;
    /**< Number of characters from the beginning of the SM data after which the 
         picture shall be displayed. */
  uint8               data[WMS_UDH_SMALL_PIC_SIZE];
    /**< Data for the small picture. */
} wms_udh_small_picture_data_s_type;


/** UDH variable-length picture information element.
*/
typedef struct wms_udh_var_picture_s
{
  uint8       position;
    /**< Number of characters from the beginning of the SM data after which the 
         picture shall be displayed. */
  uint8       width;
    /**< Horizontal dimension of the picture. Number of pixels in multiples of 8. */
  uint8       height;
    /**< Vertical dimension of the picture. Number of pixels in multiples of 8. */ 
  uint8       data[WMS_UDH_VAR_PIC_SIZE];
    /**< Data for the variable picture, line by line from top left to bottom right. */
} wms_udh_var_picture_s_type;


/* ----------------------------- */
/* Animation Related Definitions */
/* ----------------------------- */
/** UDH predefined animation information element.
*/
typedef struct wms_udh_pre_def_anim_s
{
  uint8       position;
    /**< Number of characters from the beginning of the SM data after which the 
         animation shall be displayed. */
  uint8       animation_number;
    /**< Animation number encoded as an integer. */
} wms_udh_pre_def_anim_s_type;


/** UDH large animation information element.
*/
typedef struct wms_udh_large_anim_s
{
  uint8        position;
    /**< Number of characters from the beginning of the SM data after which the 
         animation shall be displayed. */
  uint8        data[WMS_UDH_ANIM_NUM_BITMAPS][WMS_UDH_LARGE_BITMAP_SIZE]; 
    /**< Data for the large animation. */
} wms_udh_large_anim_s_type;


/** UDH small animation information element.
*/
typedef struct wms_udh_small_anim_s
{
  uint8         position;
    /**< Number of characters from the beginning of the SM data after which the 
         animation shall be displayed. */
  uint8         data[WMS_UDH_ANIM_NUM_BITMAPS][WMS_UDH_SMALL_BITMAP_SIZE];
    /**< Data for the small animation. */
} wms_udh_small_anim_s_type;


/** UDH email information element.
*/
typedef struct wms_udh_rfc822_s
{
  uint8        header_length; 
    /**< Using 8-bit data, an integer representation of the number of octets 
         within (that fraction of) the RFC 822 E-Mail Header that is located at 
         the beginning of the data part of the current (segment of the 
         concatenated) SM. */
} wms_udh_rfc822_s_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @addtogroup gw_specific_type_defs_group 
@{ */
/* ------------------------------------- */
/* National Language Related Definitions */
/* ------------------------------------- */
/** GSM/WCDMA-specific national language type IDs.
*/
typedef enum
{
  WMS_UDH_NAT_LANG_TURKISH          = 0x1, /**< Turkish. */
  WMS_UDH_NAT_LANG_SPANISH          = 0x2, /**< Spanish. */
  WMS_UDH_NAT_LANG_PORTUGUESE       = 0x3, /**< Portuguese. */
  /* 0x0    Reserved */
  /* 0x4 - 0xFF Reserved */
/** @cond */
  WMS_UDH_NAT_LANG_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_udh_nat_lang_id_e_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** UDH language single shift information element for a national language.

  This structure indicates which National Language Single Shift Table is used 
  instead of the GSM 7-bit default alphabet extension table specified in 
  3GPP TS 23.038.
*/
typedef struct wms_udh_nat_lang_ss_s
{
  wms_udh_nat_lang_id_e_type nat_lang_id; /**< National language identifier. */
} wms_udh_nat_lang_ss_s_type;


/** UDH language locking shift information element for a national language.

  This structure indicates which National Language Locking Shift Table is used 
  instead of the GSM 7-bit default alphabet extension table specified in 
  3GPP TS 23.038.
*/
typedef struct wms_udh_nat_lang_ls_s
{
  wms_udh_nat_lang_id_e_type nat_lang_id; /**< National language identifier. @newpagetable */
} wms_udh_nat_lang_ls_s_type;


/** UDH user prompt indicator information element.

  This structure indicates to the receiving entity that the following object is 
  intended to be handled at the time of reception (e.g., by means of user 
  interaction).
*/
typedef struct wms_udh_user_prompt_s
{      
  uint8       number_of_objects;
    /**< Number of objects (all of the same kind) that follow this header and that 
         will be stitched together by the applications. Objects that can be 
         stitched are images (small, large, variable) and user-defined sounds. 
         Following are two examples.
         - Five small pictures are to be stitched together horizontally.
         - Six iMelody tones are to be connected with the intermediate iMelody 
           header and footer ignored. @tablebulletend 
    */
   } wms_udh_user_prompt_s_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** Extended Object control data; an object can be forwarded.
*/
#define WMS_UDH_EO_CONTROL_FORWARD        0x01


/** Extended Object control data; an object can be handled as a User Prompt.
*/
#define WMS_UDH_EO_CONTROL_USER_PROMPT    0x02
/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** GSM/WCDMA-specific Extended Object IDs/types.
*/
typedef enum
{
  WMS_UDH_EO_VCARD                   = 0x09, /**< vCard. */
  WMS_UDH_EO_VCALENDAR               = 0x0A, /**< vCalendar. */
/** @cond */
  WMS_UDH_EO_MAX32 = 0x10000000   /* pad to 32 bit int */
/** @endcond */
} wms_udh_eo_id_e_type;


/** GSM/WCDMA CB Error Indication.
*/
typedef enum 
{
  WMS_GW_CB_ERROR_MEMORY_FULL,   
  /**< Indicates that memory is full for GW cell broadcast SMS messages. 
       Users must delete existing CB SMS messages to receive new messages. 
       @newpage */

/** @cond */
  WMS_GW_CB_ERROR_MAX32 = 0x10000000
/** @endcond */
} wms_gw_cb_error_e_type;
/** @} */ /* end_addtogroup gw_specific_type_defs_group */

/** @ingroup gw_specific_type_defs_group 
  Maximum amount of GSM/WCDMA-specific data allowed in an Extended Object 
  segment. */
/* 3 bytes for UDHL, EO UDH_ID, EO UDH_LEN; 6 bytes for the whole CONCAT16 */
#define WMS_UDH_EO_DATA_SEGMENT_MAX    131  
/* 140 - 3 - 6 */ 


/** @addtogroup gw_specific_type_defs_group 
@{ */
/** UDH Extended Object content information element.
*/
typedef struct wms_udh_eo_content_s
{
  uint8         length;
    /**< Extended Object length in number of octets (integer representation). */
  uint8         data[WMS_UDH_EO_DATA_SEGMENT_MAX];
    /**< Buffer to store data for vCard or vCalendar contents. */

    /* WMS_UDH_EO_VCARD: See http://www.imc.org/pdi/vcard-21.doc for payload.
     WMS_UDH_EO_VCALENDAR: See http://www.imc.org/pdi/vcal-10.doc.
     Or, Unsupported/proprietary extended objects. */

} wms_udh_eo_content_s_type;


/** UDH Extended Object information element.

  Extended Objects are to be used together with 16-bit concatenation UDH. The 
  maximum number of segments supported for extended objects is a minumum of 8. 
  The reference, length, control, type, and position fields are only present in 
  the first segment of a concatenated SMS message. 
*/
typedef struct wms_udh_eo_s
{
  wms_udh_eo_content_s_type         content;
    /**< UDH Extended Object content information element; see 
         #wms_udh_eo_content_s for details. */
  boolean                           first_segment;
    /**< Indicates whether it is the first segment. */
  uint8                             reference;
    /**< Identify those Extended Object segments that are to be linked together. */
  uint16                            length;
    /**< Length of the entire Extended Object data. */                                uint8                             control;
    /**< Control data. */
  wms_udh_eo_id_e_type              type;
    /**< ID of the Extended Object. */
  uint16                            position;
    /**< Absolute position of the Extended Object in the entire text after 
         concatenation, starting from 1. */
} wms_udh_eo_s_type;


/** UDH for other information elements that may be supported.
*/
typedef struct wms_udh_other_s
{
   wms_udh_id_e_type  header_id;
     /**< User Data Header ID, indicating the type of user data. */
   uint8              header_length;
     /**< Length of the user data header. */
   uint8              data[WMS_UDH_OTHER_SIZE];
     /**< User data. */
} wms_udh_other_s_type;


/** UDH information element definition. 
*/
typedef struct wms_udh_s
{
  wms_udh_id_e_type               header_id;
    /**< User Data Header ID, indicating the type of user data. */

  /** Used by the #wms_udh_s structure.
  */
  union wms_udh_u 
  {
    wms_udh_concat_8_s_type             concat_8;
      /**< Concatenated short messages, 8-bit reference number; see 
           #wms_udh_concat_8_s for details. */
 /*~ CASE WMS_UDH_CONCAT_8 wms_udh_u.concat_8 */

    wms_udh_special_sm_s_type           special_sm;
      /**< Special SMS Message Indication; see #wms_udh_special_sm_s for 
           details. */
 /*~ CASE WMS_UDH_SPECIAL_SM wms_udh_u.special_sm */

    wms_udh_wap_8_s_type                wap_8;
      /**< Wireless application port addressing scheme, 8-bit address; 
           see #wms_udh_wap_8_s for details. */ 
 /*~ CASE WMS_UDH_PORT_8 wms_udh_u.wap_8 */

    wms_udh_wap_16_s_type               wap_16;
      /**< Wireless application port addressing scheme, 16-bit address; 
           see #wms_udh_wap_16_s for details. */
 /*~ CASE WMS_UDH_PORT_16 wms_udh_u.wap_16 */

    wms_udh_concat_16_s_type            concat_16;
      /**< Concatenated short message, 16-bit reference number; see 
           #wms_udh_concat_16_s for details. */
 /*~ CASE WMS_UDH_CONCAT_16 wms_udh_u.concat_16 */

    wms_udh_text_formating_s_type       text_formating;
      /**< Text formatting; see #wms_udh_text_formating_s for details. */
 /*~ CASE WMS_UDH_TEXT_FORMATING wms_udh_u.text_formating */

    wms_udh_pre_def_sound_s_type        pre_def_sound;
      /**< Predefined sound; see #wms_udh_pre_def_sound_s for details. */
 /*~ CASE WMS_UDH_PRE_DEF_SOUND wms_udh_u.pre_def_sound */

    wms_udh_user_def_sound_s_type       user_def_sound;
      /**< User-defined sound; see #wms_udh_user_def_sound_s for details. */
 /*~ CASE WMS_UDH_USER_DEF_SOUND wms_udh_u.user_def_sound */

    wms_udh_pre_def_anim_s_type         pre_def_anim;
      /**< Predefined animation; see #wms_udh_pre_def_anim_s for details. */
 /*~ CASE WMS_UDH_PRE_DEF_ANIM wms_udh_u.pre_def_anim */

    wms_udh_large_anim_s_type           large_anim;
      /**< Large animation; see #wms_udh_large_anim_s for details. */
 /*~ CASE WMS_UDH_LARGE_ANIM wms_udh_u.large_anim */

    wms_udh_small_anim_s_type           small_anim;
      /**< Small animation; see #wms_udh_small_anim_s for details. */
 /*~ CASE WMS_UDH_SMALL_ANIM wms_udh_u.small_anim */ 

    wms_udh_large_picture_data_s_type   large_picture;
      /**< Large picture; see #wms_udh_large_picture_data_s for details. */
 /*~ CASE WMS_UDH_LARGE_PICTURE wms_udh_u.large_picture */

    wms_udh_small_picture_data_s_type   small_picture;
      /**< Small picture; see #wms_udh_small_picture_data_s for details. */
 /*~ CASE WMS_UDH_SMALL_PICTURE wms_udh_u.small_picture */

    wms_udh_var_picture_s_type          var_picture;
      /**< Variable picture; see #wms_udh_var_picture_s for details. */
 /*~ CASE WMS_UDH_VAR_PICTURE wms_udh_u.var_picture */

    wms_udh_user_prompt_s_type          user_prompt;
      /**< User prompt indicator; see #wms_udh_user_prompt_s for details. */
 /*~ CASE WMS_UDH_USER_PROMPT wms_udh_u.user_prompt */

    wms_udh_eo_s_type                   eo;
      /**< Extended Object; see #wms_udh_eo_s for details. */
 /*~ CASE WMS_UDH_EXTENDED_OBJECT wms_udh_u.eo */

    wms_udh_rfc822_s_type               rfc822;
      /**< RFC 822 E-Mail Header; see #wms_udh_rfc822_s for details. */
 /*~ CASE WMS_UDH_RFC822 wms_udh_u.rfc822 */

    wms_udh_nat_lang_ss_s_type          nat_lang_ss;
      /**< National language single shift; see #wms_udh_nat_lang_ss_s for 
           details. */
 /*~ CASE WMS_UDH_NAT_LANG_SS wms_udh_u.nat_lang_ss */

    wms_udh_nat_lang_ls_s_type          nat_lang_ls;
      /**< National language locking shift; see #wms_udh_nat_lang_ls_s for 
           details. */ 
 /*~ CASE WMS_UDH_NAT_LANG_LS wms_udh_u.nat_lang_ls */

    wms_udh_other_s_type                other;
      /**< Other type of information element supported; see wms_udh_other_s 
           for details. @newpagetable */
 /*~ DEFAULT wms_udh_u.other */

  }u; /**< Used by the #wms_udh_s structure. */
  /*~ FIELD wms_udh_s.u DISC _OBJ_.header_id */
} wms_udh_s_type;

/** @} */ /* end_addtogroup gw_specific_type_defs_group */


/* <EJECT> */
/* ========================================================================= */
/* ======================= Configuration group data types ================== */
/* ========================================================================= */

/** @addtogroup config_group_data_types
@{ */
/** Routing information.
*/
typedef struct wms_routing_s
{
  wms_route_e_type            route;     /**< Type of message routing action. */
  wms_memory_store_e_type     mem_store; /**< Type of memory storage for the 
                                              message. */
} wms_routing_s_type;
/** @} */ /* end_addtogroup config_group_data_types */


/** @addtogroup config_group_data_types
@{ */
/** Message route configuration.
*/
typedef struct wms_routes_s
{
  wms_client_type_e_type   client_id;  /**< Client that issued the original command to the WMS. */
  wms_routing_s_type       pp_routes[WMS_MESSAGE_CLASS_MAX];
    /**< Type of Point-to-Point message routing and storing actions; see 
         #wms_routing_s for details. */
  wms_routing_s_type       bc_routes[WMS_MESSAGE_CLASS_MAX];
    /**< Type of broadcast message routing and storing actions; see 
         #wms_routing_s for details. Meaningful only if FEATURE_CDSMS_BROADCAST 
         is defined. */
  boolean                  transfer_status_report; 
    /**< Indicate whether to transfer a status report. For GSM/WCDMA only: if TRUE, 
         do not store status reports but send them to clients. */
  sys_modem_as_id_e_type   as_id;  /**< Active subscription ID. @newpagetable */

} wms_routes_s_type;
/** @} */ /* end_addtogroup config_group_data_types */


/** @addtogroup config_group_data_types
@{ */
/** Memory store usage status.
*/
typedef struct wms_memory_status_s
{
  wms_client_type_e_type      client_id;      /**< Client that issued the original command to the WMS. */
  wms_memory_store_e_type     mem_store;      /**< Memory store. */
  uint32                      max_slots;      /**< Maximum number of slots. */
  uint32                      free_slots;     /**< Number of free slots. */
  uint32                      used_tag_slots;
    /**< Number of slots used by messages of the following tag. If the tag is 
         WMS_TAG_NONE, this is the number of all used/occupied slots. */
  wms_message_tag_e_type      tag;            /**< Message tag type. */
  sys_modem_as_id_e_type      as_id;          /**< Active subscription ID. */
} wms_memory_status_s_type;
/** @} */ /* end_addtogroup config_group_data_types */


/** @addtogroup config_group_data_types
@{ */
/** Message list for messages with a specific tag in a memory store.
*/
typedef struct wms_message_list_s
{
  wms_client_type_e_type      client_id;  /**< Client that issued the original command to the WMS. */
  wms_memory_store_e_type     mem_store;  /**< Memory store. */
  wms_message_tag_e_type      tag;        /**< Messages to be included in the list. */
  uint32                      len;        /**< Number of indices returned. */
  wms_message_index_type      voice_mail_index;  /**< Voice mail index. */
  boolean                     voice_mail_active;
    /**< Indicates whether voice mail is active. */
  wms_message_index_type      indices[WMS_MESSAGE_LIST_MAX]; 
    /**< Indices to message list. */
  wms_message_tag_e_type      tags[WMS_MESSAGE_LIST_MAX]; 
    /**< Message tags in message list. */
  sys_modem_as_id_e_type      as_id;   /**< Active subscription ID. @newpagetable */
} wms_message_list_s_type;
/** @} */ /* end_addtogroup config_group_data_types */


/** @addtogroup config_group_data_types
@{ */
/** Cell change causes.
*/
typedef enum
{
  WMS_GW_CB_NO_SERVICE,  /**< No service. */
  WMS_GW_CB_SAME_CELL,   /**< Same cell. */
  WMS_GW_CB_CELL_CHANGE, /**< Change in the cell. */
  WMS_GW_CB_LAC_CHANGE,  /**< Change in the location area code. */
  WMS_GW_CB_PLMN_CHANGE, /**< Change in the public land mobile network.*/
/** @cond */
  WMS_GW_CB_MAX32
/** @endcond */
}wms_gw_cb_cell_change_e_type;
/** @} */ /* end_addtogroup config_group_data_types */


/** @addtogroup config_group_data_types
@{ */

/** Type of system.
*/
typedef enum 
{
   WMS_CFG_SYS_GW,  /**< GSM/WCDMA. */
   WMS_CFG_SYS_CDMA /**< CDMA. */
} wms_cfg_sys_e_type; 
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** Event data for Ready event.
*/
typedef struct 
{
   wms_cfg_sys_e_type         sys_type; /**< System type (GSM/WCDMA or CDMA). */
   sys_modem_as_id_e_type     as_id;    /**< Active subscription ID. */
} wms_cfg_ready_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** Preferred domain.
*/
typedef struct 
{
   wms_client_type_e_type     client_id;    /**< Client that issued the original command to the WMS. */
   wms_gw_domain_pref_e_type  domain_pref;  /**< Domain preferred. */
   sys_modem_as_id_e_type     as_id;        /**< Active subscription ID. */
} wms_gw_domain_pref_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

typedef struct
{  
  wms_gw_domain_pref_s_type   gw;
  sys_sms_domain_pref_e_type  lte_domain_pref;
} wms_gwl_domain_pref_s_type;

/** @addtogroup config_group_data_types
@{ */
/** Event data for memory full event.
*/
typedef struct 
{
   wms_memory_store_e_type    mem_store; /**< Type of memory storage to use. */
   sys_modem_as_id_e_type     as_id;     /**< Active subscription ID. */
} wms_mem_full_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** Event data for memory status.
*/
typedef struct 
{
   boolean                   memory_full; /**< Indicates if memory is full. */
   sys_modem_as_id_e_type    as_id;       /**< Active subscription ID. */
} wms_cfg_memory_status_set_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** Event data for the cell type.
*/
typedef struct 
{
   wms_gw_cb_cell_change_e_type  cell_change_type; /**< Type of cell change for 
                                                        a cell broadcast. */
   sys_modem_as_id_e_type        as_id;            /**< Active subscription ID. */
} wms_gw_cb_cell_change_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** @brief Event data for the MWI information.
*/
typedef struct 
{
   sys_modem_as_id_e_type        as_id;                  /**< Active subscription ID. */
   byte sim_mwi_data[WMS_SIM_MWI_DATA_REC_LEN_MAX];      /**< Updated MWI information. */
   wms_mwi_state_e_type state;   
} wms_cfg_msg_waiting_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** Event data for SIM INIT event.
*/
typedef struct 
{
   wms_cfg_sys_e_type         sys_type;     /**< System type (GSM/WCDMA or CDMA). */
   sys_modem_as_id_e_type     as_id;        /**< Active subscription ID. */
   boolean                    is_inited;    /**< Indicates if SIM initialization is completed. */
   uint32                     session_type; /** Uint32 version of mmgsdi_session_type_enum_type */
} wms_cfg_sim_init_complete_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** Event data for Refresh status event.
*/
typedef struct 
{
   wms_cfg_sys_e_type               sys_type;      /**< System type (GSM/WCDMA or CDMA). */
   sys_modem_as_id_e_type           as_id;         /**< Active subscription ID. */
   boolean                          is_success;    /**< Indicates if refresh ended successfully. */
   uint32                           session_type;  /** Uint32 version of mmgsdi_session_type_enum_type */
} wms_cfg_refresh_process_end_s_type;
/** @} */ /* end_addtogroup config_group_data_types */


/** @addtogroup config_group_data_types
@{ */
/** Event data for OTA Service event.
*/
typedef struct 
{
  wms_cfg_sys_e_type                sys_type;      /**< System type (GSM/WCDMA or CDMA). */
  sys_sys_mode_e_type               sys_mode;  
  sys_modem_as_id_e_type            as_id;         /**< Active subscription ID. */
  boolean                           is_serv_avail; /**< Indicates if SMS OTA service is available. */
} wms_cfg_serv_avail_s_type;
/** @} */ /* end_addtogroup config_group_data_types */

/** @addtogroup config_group_data_types
@{ */
/** Configuration event types.
*/
typedef enum
{
  WMS_CFG_EVENT_GW_READY = 0,          /**< GSM/WCDMA is ready. */
  WMS_CFG_EVENT_CDMA_READY,            /**< CDMA is ready. */
  WMS_CFG_EVENT_ROUTES,                /**< Reports the routing configuration. */
  WMS_CFG_EVENT_MEMORY_STATUS,         /**< Reports the current memory status. */
  WMS_CFG_EVENT_MESSAGE_LIST,          /**< Message list is updated. */
  WMS_CFG_EVENT_MEMORY_FULL,           /**< Memory is full event. */
  WMS_CFG_EVENT_GW_DOMAIN_PREF,        /**< GSM/WCDMA domain preference event. */
  WMS_CFG_EVENT_CELL_CHANGE,           /**< Cell change event.*/
  WMS_CFG_EVENT_PRIMARY_CLIENT_SET,    /**< Primary client sets the event. */
  WMS_CFG_EVENT_MEMORY_STATUS_SET,     /**< Sets the memory status. */
  WMS_CFG_EVENT_LINK_CONTROL,          /**< Reports the current link control 
                                            information. */
  WMS_CFG_EVENT_CB_ERROR,              /**< Event from the NAS layer to indicate 
                                            a CB error. */
  WMS_CFG_EVENT_MS_READY,              /**< Ready for client services event in 
                                            Dual SIM Dual Standby (DSDS) mode. */
  WMS_CFG_EVENT_MS_MEMORY_FULL,        /**< Memory is full in DSDS mode. */
  WMS_CFG_EVENT_MS_GW_DOMAIN_PREF,     /**< GSM/WCDMA domain preference event in 
                                            DSDS mode. */
  WMS_CFG_EVENT_MS_CELL_CHANGE,        /**< Cell change event in DSDS mode. */
  WMS_CFG_EVENT_MS_MEMORY_STATUS_SET,  /**< Sets primary client in DSDS mode. */
  WMS_CFG_EVENT_MSG_WAITING,           /**< Reports the MWI update. */
  WMS_CFG_EVENT_MS_NOT_READY,          /**< WMS not initialized to process client
                                            services in dual sim mode. */
  WMS_CFG_EVENT_MS_SIM_INIT_START,     /**< Indicates that initialization of SIM files has 
                                            started.*/
  WMS_CFG_EVENT_MS_SIM_INIT_END,       /**< Indicates that reading SIM files is completed. */
  WMS_CFG_EVENT_MS_REFRESH_START,      /**< Refresh processing has started. */
  WMS_CFG_EVENT_MS_REFRESH_END,        /**< Refresh processing has ended. */
  WMS_CFG_EVENT_MS_SERV_AVAIL,         /**< WMS OTA service is available. */
  WMS_CFG_EVENT_MS_SERV_NOT_AVAIL,     /**< WMS OTA service is not available. */
  WMS_CFG_EVENT_MS_GWL_DOMAIN_PREF,    /**< GW + LTE domain preference event */ 

/** @cond */
  WMS_CFG_EVENT_MAX,
  WMS_CFG_EVENT_MAX32 = 0x10000000
/** @endcond */
} wms_cfg_event_e_type;
/*~ SENTINEL wms_cfg_event_e_type.WMS_CFG_EVENT_MAX */
/** @} */ /* end_addtogroup config_group_data_types */


/** @ingroup config_group_constants
  Card Ready event.
*/
#define WMS_CFG_EVENT_SIM_READY WMS_CFG_EVENT_GW_READY


/** @addtogroup config_group_data_types
@{ */
/** Configuration event information for each event.
*/
typedef union wms_cfg_event_info_u
{
  wms_routes_s_type           routes;
    /**< Event information associated with WMS_CFG_EVENT_ROUTES; see 
         #wms_routes_s for details. */
 /*~ CASE WMS_CFG_EVENT_ROUTES wms_cfg_event_info_u.routes */

  wms_memory_status_s_type    memory_status;
    /**< Event information associated with WMS_CFG_EVENT_MEMORY_STATUS; see 
         #wms_memory_status_s for details. */
 /*~ CASE WMS_CFG_EVENT_MEMORY_STATUS wms_cfg_event_info_u.memory_status */

  wms_message_list_s_type     message_list;
    /**< Event information associated with WMS_CFG_EVENT_MESSAGE_LIST; see 
         #wms_message_list_s for details. */
 /*~ CASE WMS_CFG_EVENT_MESSAGE_LIST wms_cfg_event_info_u.message_list */

  wms_memory_store_e_type      mem_store; 
    /**< Event information associated with WMS_CFG_EVENT_MEMORY_FULL. */
 /*~ CASE WMS_CFG_EVENT_MEMORY_FULL wms_cfg_event_info_u.mem_store */
 
  wms_gw_domain_pref_e_type    gw_domain_pref; 
    /**< Event information associated with WMS_CFG_EVENT_GW_DOMAIN_PREF. */
  /*~ CASE WMS_CFG_EVENT_GW_DOMAIN_PREF wms_cfg_event_info_u.gw_domain_pref */

  wms_gw_cb_cell_change_e_type cell_change_type; 
    /**< Event information associated with WMS_CFG_EVENT_CELL_CHANGE. */
/*~ CASE WMS_CFG_EVENT_CELL_CHANGE wms_cfg_event_info_u.cell_change_type */

   boolean                      memory_full; 
    /**< Event information associated with WMS_CFG_EVENT_MEMORY_STATUS_SET. */
 /*~ CASE WMS_CFG_EVENT_MEMORY_STATUS_SET wms_cfg_event_info_u.memory_full */

  wms_cfg_set_primary_client_s_type  set_primary;
    /**< Event information associated with WMS_CFG_PRIMARY_CLIENT_SET. */
 /*~ CASE WMS_CFG_EVENT_PRIMARY_CLIENT_SET wms_cfg_event_info_u.set_primary */

  wms_cfg_link_control_s_type  link_control;
    /**< Event information associated with WMS_CFG_EVENT_LINK_CONTROL; see 
         #wms_cfg_link_control_s for details. */
 /*~ CASE WMS_CFG_EVENT_LINK_CONTROL wms_cfg_event_info_u.link_control */

  wms_gw_cb_error_e_type       cb_error_info;
    /**< Event information associated with WMS_CFG_EVENT_CB_ERROR. */
 /*~ CASE WMS_CFG_EVENT_CB_ERROR wms_cfg_event_info_u.cb_error_info */

   wms_cfg_ready_s_type        ms_ready;
    /**< Event information associated with WMS_CFG_EVENT_MS_READY. */
 /*~ CASE WMS_CFG_EVENT_MS_READY wms_cfg_event_info_u.ms_ready */

   wms_mem_full_s_type         ms_memory_full;
    /**< Event information associated with WMS_CFG_EVENT_MS_MEMORY_FULL. */
 /*~ CASE WMS_CFG_EVENT_MS_MEMORY_FULL wms_cfg_event_info_u.ms_memory_full */
 
   wms_gw_domain_pref_s_type   ms_gw_domain_pref;
    /**< Event information associated with WMS_CFG_EVENT_MS_GW_DOMAIN_PREF. */
 /*~ CASE WMS_CFG_EVENT_MS_GW_DOMAIN_PREF wms_cfg_event_info_u.ms_gw_domain_pref */

   wms_gwl_domain_pref_s_type  ms_gwl_domain_pref;
    /**< Event information associated with WMS_CFG_EVENT_MS_GW_DOMAIN_PREF. */
 /*~ CASE WMS_CFG_EVENT_MS_GW_DOMAIN_PREF wms_cfg_event_info_u.ms_gwl_domain_pref */

   wms_gw_cb_cell_change_s_type  ms_cell_change;
    /**< Event information associated with WMS_CFG_EVENT_MS_CELL_CHANGE. */
 /*~ CASE WMS_CFG_EVENT_MS_CELL_CHANGE wms_cfg_event_info_u.ms_cell_change */

   wms_cfg_memory_status_set_s_type  ms_memory_status_set;
    /**< Event information associated with WMS_CFG_EVENT_MS_MEMORY_STATUS_SET. */
 /*~ CASE WMS_CFG_EVENT_MS_MEMORY_STATUS_SET wms_cfg_event_info_u.ms_memory_status_set */

 /*~ DEFAULT wms_cfg_event_info_u.void */
 /*  used as dummy data */

   wms_cfg_msg_waiting_s_type    msg_wait_info;
    /**< Event information associated with WMS_CFG_EVENT_MSG_WAITING. */
 /*~ CASE WMS_CFG_EVENT_MSG_WAITING wms_cfg_event_info_u.msg_wait_info */

   wms_cfg_ready_s_type          ms_not_ready;
    /**< Event information associated with WMS_CFG_EVENT_MS_NOT_READY. */
 /*~ CASE WMS_CFG_EVENT_MS_NOT_READY wms_cfg_event_info_u.ms_not_ready */

   wms_cfg_sim_init_complete_s_type  begin_sim_init;
    /**< Event information associated with WMS_CFG_EVENT_MS_SIM_INIT_START. */
 /*~ CASE WMS_CFG_EVENT_MS_SIM_INIT_START wms_cfg_event_info_u.begin_sim_init */

   wms_cfg_sim_init_complete_s_type  end_sim_init;
    /**< Event information associated with WMS_CFG_EVENT_MS_SIM_INIT_END. */
 /*~ CASE WMS_CFG_EVENT_MS_SIM_INIT_END wms_cfg_event_info_u.end_sim_init */

   wms_cfg_ready_s_type              begin_refresh;
    /**< Event information associated with WMS_CFG_EVENT_MS_REFRESH_START. */
 /*~ CASE WMS_CFG_EVENT_MS_REFRESH_START wms_cfg_event_info_u.begin_refresh */
   
   wms_cfg_refresh_process_end_s_type      end_refresh;
    /**< Event information associated with WMS_CFG_EVENT_MS_REFRESH_END. */
 /*~ CASE WMS_CFG_EVENT_MS_REFRESH_END wms_cfg_event_info_u.end_refresh */

   wms_cfg_serv_avail_s_type         svc_avail;
    /**< Event information associated with WMS_CFG_EVENT_MS_SERV_AVAIL. */
 /*~ CASE WMS_CFG_EVENT_MS_SERV_AVAIL wms_cfg_event_info_u.svc_avail */

   wms_cfg_serv_avail_s_type         svc_not_avail;
    /**< Event information associated with WMS_CFG_EVENT_MS_SERV_NOT_AVAIL. */
 /*~ CASE WMS_CFG_EVENT_MS_SERV_NOT_AVAIL wms_cfg_event_info_u.svc_avail */
} wms_cfg_event_info_s_type;
/** @} */ /* end_addtogroup config_group_data_types */


/** @ingroup config_group_wms_cfg_event_cb_type
  Configuration event callback function.

  @datatypes
  #wms_cfg_event_e_type \n
  #wms_cfg_event_info_u

  @param[in]  event     Configuration event type.
  @param[in]  info_ptr  Configuration event information.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_cfg_event_cb_type)
(
  wms_cfg_event_e_type         event,
  wms_cfg_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK wms_cfg_event_cb_type
    ONERROR return */

/** @brief WMS SIM EFS read complete input/ output struct.
*/
typedef struct 
{
  uint32                           session_type;
  uint32                           t_id;
  void                            *cfg_event_cb;
  const void*                      user_data;
} wms_cfg_efs_read_evt_s_type;

/** WMS SIM EFS read complete callback.
*/
typedef void (*wms_cfg_send_efs_read_event)
(
  wms_cfg_efs_read_evt_s_type    *info_ptr /**< Pointer to event information. */
);
/* <EJECT> */
/* ========================================================================= */
/* ====================== Message group data types ========================= */
/* ========================================================================= */

/** @addtogroup messaging_group_data_types
@{ */
/** Over-the-air raw Transport Service data structure.
*/
typedef struct wms_raw_ts_data_s
{
  wms_format_e_type           format;    /**< Format of the message. */
  wms_gw_tpdu_type_e_type     tpdu_type;
    /**< Type of TPDU. Meaningful only if the format is a GSM/WCDMA message. */
  uint32                      len;
    /**< Length of the message. Meaningful only if the format is a GSM/WCDMA 
         message. */
  uint8                       data[ WMS_MAX_LEN ]; 
    /**< Data in the message. Meaningful only if the format is a GSM/WCDMA 
         message. */
} wms_raw_ts_data_s_type;

/** Data coding scheme for GSM/WCDMA encoded messages.
*/
typedef struct wms_gw_dcs_s
{
  wms_message_class_e_type          msg_class;
    /**< Message class. */
  boolean                           is_compressed;  
    /**< Indicates whether the message is compressed. */
  wms_gw_alphabet_e_type            alphabet;
    /**< GSM/WCDMA-specific alphabet set. */  
  wms_gw_msg_waiting_e_type         msg_waiting;
    /**< Type of message waiting action. */
  boolean                           msg_waiting_active;
    /**< Indicates whether the message waiting is active. */
  wms_gw_msg_waiting_kind_e_type    msg_waiting_kind;
    /**< Type of message waiting (e.g., voice mail, email). */ 
  uint8                             raw_dcs_data;
    /**< Raw DCS byte. */
} wms_gw_dcs_s_type;

/* ====================== CDMA message definitions ========================= */

/* ------------------- */
/* ---- User Data ---- */
/* ------------------- */
/** User data for CDMA messages.

  If message_id.udh_present == TRUE, num_headers is the number of UDHs, and 
  headers includes all those headers.
*/
typedef struct wms_cdma_user_data_s
{
  uint8                              num_headers;
    /**< Number of UDHs. */
  wms_udh_s_type                     headers[WMS_MAX_UD_HEADERS];
    /**< All the UDHs; see #wms_udh_s for details. */
  wms_user_data_encoding_e_type      encoding;
    /**< Encoding type. */
  wms_IS91EP_type_e_type             is91ep_type;
    /**< IP-91 type. */
  uint8                              data_len;
    /**< Valid number of bytes in the data array. */
  uint8                              padding_bits;
    /**< Number of invalid bits (0 to 7) in the last byte of data. This parameter is 
         used for MO messages only. 
		 
         There is no way for the API to tell how many padding bits exist in the 
         received message. Instead, the application can find out how many padding 
         bits exist in the user data when decoding the user data. */
  uint8                              data[ WMS_CDMA_USER_DATA_MAX ];
    /**< Raw bits of the user data field of the SMS message. The client software 
         decodes the raw user data according to its supported encoding types and 
         languages. Following are exceptions:
           - CMT-91 user data raw bits are first translated into BD fields 
             (e.g., num_messages and callback). The translated user data field in 
             VMN and Short Message is in ASCII characters, each occupying a byte in 
             the resulted data.
           - GSM 7-bit Default characters are decoded such that each byte has one 
             7-bit GSM character. @tablebulletend
    */
  uint8                              number_of_digits;
    /**< Number of digits/characters (7, 8, 16, etc. bits) in the raw user data. 
         This data can be used by the client when decoding the user data according 
         to the encoding type and language. */
  wms_gw_dcs_s_type                  gsm_dcs;
    /**< The decoded Data Coding Scheme.  Only valid when the encoding field is 
         set to WMS_ENCODING_GSM_DATA_CODING_SCHEME.  When valid, the structure
         contents should be used to determine the encoding of the user data. */
} wms_cdma_user_data_s_type;
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @ingroup messaging_group_data_types
  User responses for CDMA messages.
*/
typedef uint8           wms_user_response_type;
  /**< This type is used in the SMS User Acknowledgment Message to respond to 
       previously received short messages. This element carries the identifier of 
       a predefined response specific to the Message Center. */


/** @addtogroup messaging_group_constants
@{ */
/** Cause for an application toolkit busy response is that the Transport Layer 
  destination is busy.
  */
#define WMS_RSP_APPLICATION_TOOLKIT_BUSY   WMS_TL_DESTINATION_BUSY_S


/** Cause for a data download error response is that the Transport Layer has a 
  problem with the other terminal.
*/
#define WMS_RSP_DATA_DOWNLOAD_ERROR        WMS_TL_OTHER_TERMINAL_PROBLEM_S 
/** @} */ /* end_addtogroup messaging_group_constants */

/** @ingroup messaging_group_data_types
  Reply options for CDMA messages.
*/
typedef struct wms_reply_option_s
{
    boolean          user_ack_requested;     
      /**< User acknowledgment is requested. */
    boolean          delivery_ack_requested; 
      /**< Delivery acknowledgment is requested. Set to FALSE for incoming 
           messages. */
    boolean          read_ack_requested;     
      /**< Message originator requests the receiving phone to return a READ_ACK 
           message automatically when the user reads the received message. */
} wms_reply_option_s_type;


/** @ingroup messaging_group_constants
  Maximum number of entries in the broadcast service table.
*/
#define WMS_BC_TABLE_MAX         ( 32 * 4 )


/** @ingroup messaging_group_data_types
  Service ID for CDMA messages. The ID is a service/language pair.
*/
typedef struct wms_bc_service_id_s
{
  wms_service_e_type   service;  /**< Specific service. */
  wms_language_e_type  language; /**< Specific language. @newpagetable */
} wms_bc_service_id_s_type;


/** @ingroup messaging_group_data_types
  Alert options for receiving broadcast SMS; for CDMA messages.
*/
typedef enum
{
  WMS_BC_ALERT_NONE  = 0,              /**< No alert. */
  WMS_BC_ALERT_DEFAULT,                /**< Default alert option. */
  WMS_BC_ALERT_VIBRATE_ONCE,           /**< Vibrate once. */
  WMS_BC_ALERT_VIBRATE_REPEAT,         /**< Vibrate repeatedly. */
  WMS_BC_ALERT_VISUAL_ONCE,            /**< Visual alert once. */
  WMS_BC_ALERT_VISUAL_REPEAT,          /**< Visual alert repeatedly. */
  WMS_BC_ALERT_LOW_PRIORITY_ONCE,      /**< Low-priority alert once. */
  WMS_BC_ALERT_LOW_PRIORITY_REPEAT,    /**< Low-priority alert repeatedly. @newpage */
  WMS_BC_ALERT_MEDIUM_PRIORITY_ONCE,   /**< Medium-priority alert once. */
  WMS_BC_ALERT_MEDIUM_PRIORITY_REPEAT, /**< Medium-priority alert repeatedly. */
  WMS_BC_ALERT_HIGH_PRIORITY_ONCE,     /**< High-priority alert once. */
  WMS_BC_ALERT_HIGH_PRIORITY_REPEAT    /**< High-priority alert repeatedly. */
  /* Other values are reserved. */
} wms_bc_alert_e_type;


/* NOTE: All SCPT related definitions are for internal use only! */

/** @cond */
/* Max number of services that can be included in a SCPT message */
#define WMS_BC_MM_SRV_LABEL_SIZE   30
#define WMS_BC_SCPT_MAX_SERVICES   5

/* Operation code for SCPT */
typedef enum
{
  WMS_BC_SCPT_OP_DELETE      = 0,
  WMS_BC_SCPT_OP_ADD         = 1,
  WMS_BC_SCPT_OP_DELETE_ALL  = 2,
  WMS_BC_SCPT_OP_MAX32       = 0x10000000
  /* Other values are reserved. */
} wms_bc_scpt_op_code_e_type;

/* Information for an entry in the SCPT Program Data. */
typedef struct wms_bc_scpt_data_entry_s
{
  wms_bc_scpt_op_code_e_type         op_code;

  wms_bc_service_id_s_type           srv_id;

  uint8                              max_messages;
    /* Maximum number of messages to be stored for this service. */
  wms_bc_alert_e_type                bc_alert;
    /* Alert options for this service. */

  uint8                              label_len;
    /* Number of characters in the label.
     ** For Unicode, each character is 2 bytes.
    */
  uint8                              label[WMS_BC_MM_SRV_LABEL_SIZE];
    /*< Name/label for this service */

} wms_bc_scpt_data_entry_s_type;

/* The SCPT Program Data from the network. */
typedef struct wms_bc_scpt_data_s
{
  wms_user_data_encoding_e_type      encoding; /* encoding of service name */
  uint8                              num_entries;
  wms_bc_scpt_data_entry_s_type      entry[WMS_BC_SCPT_MAX_SERVICES];

} wms_bc_scpt_data_s_type;

/* SCPT program result status */
typedef enum
{
  WMS_BC_SCPT_STATUS_OK   = 0,
  WMS_BC_SCPT_STATUS_SERVICE_MEMORY_FULL,
  WMS_BC_SCPT_STATUS_SERVICE_LIMIT_REACHED,
  WMS_BC_SCPT_STATUS_ALREADY_PROGRAMMED,
  WMS_BC_SCPT_STATUS_NOT_YET_PROGRAMMED,
  WMS_BC_SCPT_STATUS_INVALID_MAX_MESSAGES,
  WMS_BC_SCPT_STATUS_INVALID_ALERT_OPTION,
  WMS_BC_SCPT_STATUS_INVALID_SERVICE_NAME,
  WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR
  /* other values are reserved */
} wms_bc_scpt_status_e_type;

/* Information for an entry in the SCPT program result. */
typedef struct wms_bc_scpt_result_entry_s
{
  wms_bc_service_id_s_type            srv_id;
  wms_bc_scpt_status_e_type           status;

} wms_bc_scpt_result_entry_s_type;

/* SCPT program result to the network. */
typedef struct wms_bc_scpt_result_s
{
  uint8                                    num_entries;
  wms_bc_scpt_result_entry_s_type          entry[WMS_BC_SCPT_MAX_SERVICES];
} wms_bc_scpt_result_s_type;

/** @endcond */ /* SCPT - Internal use only */


/* ===== Multimode Broadcast Definitions ===== */
/** @addtogroup mmbroadcast_group_constants
@{ */
/** Commercial Mobile Alert System (CMAS) CBS message identifier for 
  Presidential-level alerts. The MMI cannot set this.  
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID1     4370

/** CMAS CBS message identifier for extreme alerts with Severity = Extreme, 
  Urgency = Immediate, and Certainty = Observed. 
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID2     4371

/** CMAS CBS message identifier for extreme alerts with Severity = Extreme, 
  Urgency = Immediate, and Certainty = Likely. 
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID3     4372

/** CMAS CBS message identifier for extreme alerts with Severity = Extreme, 
  Urgency = Expected, and Certainty = Observed. 
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID4     4373

/** CMAS CBS message identifier for extreme alerts with Severity = Extreme, 
  Urgency = Expected, and Certainty = Likely.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID5     4374

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe, 
  Urgency = Immediate, and Certainty = Observed. 
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID6     4375

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe, 
  Urgency = Immediate, and Certainty = Likely
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID7     4376

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe, 
  Urgency = Expected, and Certainty = Observed.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID8     4377

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe, 
  Urgency = Expected, and Certainty = Likely.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID9     4378

/** CMAS CBS message identifier for a child abduction emergency (or AMBER alert).
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID10    4379

/** CMAS CBS message identifier for the Required Monthly Test.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID11    4380

/** CMAS CBS message identifier for the CMAS Exercise.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID12    4381

/** CMAS CBS message identifier for operator-defined use.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID13    4382

/** CMAS CBS Message Identifier for CMAS Presidential Level
    Alerts for additional languages
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID14    4383

/** CMAS CBS message identifier for extreme alerts with Severity = Extreme,
    Urgency = Immediate, and Certainty = Observed for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID15    4384

/** CMAS CBS message identifier for extreme alerts with Severity = Extreme,
    Urgency = Immediate, and Certainty = Likely for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID16    4385

/** CMAS CBS message identifier for Severe Alerts with Severity = Extreme,
    Urgency = Expected, and Certainty = Observed for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID17    4386

/** CMAS CBS message identifier for Severe Alerts with Severity = Extreme,
    Urgency = Expected, and Certainty = Likely for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID18    4387

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe,
    Urgency = Immediate, and Certainty = Observed for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID19    4388

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe,
    Urgency = Immediate, and Certainty = Likely for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID20    4389

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe,
    Urgency = Expected, and Certainty = Observed for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID21    4390

/** CMAS CBS message identifier for Severe Alerts with Severity = Severe,
    Urgency = Expected, and Certainty = Likely for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID22    4391

/** CMAS CBS message identifier for a child abduction emergency (or AMBER alert)
    for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID23    4392

/** CMAS CBS message identifier for the Required Monthly Test for additional
    languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID24    4393

/** CMAS CBS message identifier for the CMAS Exercise for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID25    4394

/** CMAS CBS message identifier for operator defined use for additional languages.
*/
#define WMS_GW_CB_SRV_ID_CMAS_MSGID26    4395
/** @} */ /* end_addtogroup mmbroadcast_group_constants */


/** @ingroup mmbroadcast_group_data_types
  Multimode broadcast definitions.
*/
typedef uint16     wms_gw_cb_srv_id_type; /**< Service IDs. */


/** @addtogroup mmbroadcast_group_data_types
@{ */
/** GSM/WCDMA cell broadcast geographic scope.
*/
typedef enum
{
  WMS_GW_CB_GEO_SCOPE_CELL_IMMEDIATE = 0, /**< Immediate cell. */
  WMS_GW_CB_GEO_SCOPE_PLMN           = 1, /**< Public land mobile network. */
  WMS_GW_CB_GEO_SCOPE_LOCATION_AREA  = 2, /**< Location area defined by the
                                               location area code. */
  WMS_GW_CB_GEO_SCOPE_CELL_NORMAL    = 3  /**< Normal cell. */
} wms_gw_cb_geo_scope_e_type;

/** ETWS warning type.
*/
typedef enum
{
  WMS_ETWS_WARNING_EARTHQUAKE             = 0, /**< Earthquake. */
  WMS_ETWS_WARNING_TSUNAMI                = 1, /**< Tsunami. */
  WMS_ETWS_WARNING_EARTHQUAKE_AND_TSUNAMI = 2, /**< Earthquake and tsunami. */
  WMS_ETWS_WARNING_TEST                   = 3, /**< Test. */
  WMS_ETWS_WARNING_OTHER                  = 4, /**< Other. @newpage */
/** @cond */  
  WMS_ETWS_WARNING_MAX32                  = 0x10000000
/** @endcond */  
} wms_etws_warning_e_type;

/** ETWS notification type.
*/
typedef enum
{
  WMS_ETWS_NOTIFICATION_PRIMARY               = 0, /**< Primary notification. */
  WMS_ETWS_NOTIFICATION_SECONDARY_GSM_FORMAT  = 1, /**< Secondary notification 
                                                        in GSM format. */
  WMS_ETWS_NOTIFICATION_SECONDARY_UMTS_FORMAT = 2, /**< Secondary notification 
                                                        in UMTS format. */
/** @cond */  
  WMS_ETWS_NOTIFICATION_MAX32                 = 0x10000000
/** @endcond */  
} wms_etws_notification_e_type;

/** ETWS notification format type.
*/
typedef enum
{
  WMS_ETWS_NOTIFICATION_FORMAT_GSM    = 0, /**< Notification is in GSM format.  */
  WMS_ETWS_NOTIFICATION_FORMAT_UMTS   = 1, /**< Notification is in UMTS format. */
/** @cond */  
  WMS_ETWS_NOTIFICATION_FORMAT_MAX32  = 0x10000000
/** @endcond */  
} wms_etws_notification_format_e_type;

/** GSM/WCDMA cell broadcast languages, as defined in 3GPP TS 23.038.
*/
typedef enum
{
  WMS_GW_CB_LANGUAGE_GERMAN      = 0x00, /**< German. */
  WMS_GW_CB_LANGUAGE_ENGLISH     = 0x01, /**< English. */
  WMS_GW_CB_LANGUAGE_ITALIAN     = 0x02, /**< Italian. */
  WMS_GW_CB_LANGUAGE_FRENCH      = 0x03, /**< French. */
  WMS_GW_CB_LANGUAGE_SPANISH     = 0x04, /**< Spanish. */
  WMS_GW_CB_LANGUAGE_DUTCH       = 0x05, /**< Dutch. */
  WMS_GW_CB_LANGUAGE_SWEDISH     = 0x06, /**< Swedish. */
  WMS_GW_CB_LANGUAGE_DANISH      = 0x07, /**< Danish. */
  WMS_GW_CB_LANGUAGE_PORTUGUESE  = 0x08, /**< Portuguese. */
  WMS_GW_CB_LANGUAGE_FINNISH     = 0x09, /**< Finnish. */
  WMS_GW_CB_LANGUAGE_NORWEGIAN   = 0x0a, /**< Norwegian. */
  WMS_GW_CB_LANGUAGE_GREEK       = 0x0b, /**< Greek. */
  WMS_GW_CB_LANGUAGE_TURKISH     = 0x0c, /**< Turkish. */
  WMS_GW_CB_LANGUAGE_HUNGARIAN   = 0x0d, /**< Hungarian. */
  WMS_GW_CB_LANGUAGE_POLISH      = 0x0e, /**< Polish. */
  WMS_GW_CB_LANGUAGE_UNSPECIFIED = 0x0f,
    /**< If a language indication is not in the CB message, this enumerator is 
         used. */
  WMS_GW_CB_LANGUAGE_CZECH       = 0x20, /**< Czech. */
  WMS_GW_CB_LANGUAGE_HEBREW      = 0x21, /**< Hebrew. */
  WMS_GW_CB_LANGUAGE_ARABIC      = 0x22, /**< Arabic. */
  WMS_GW_CB_LANGUAGE_RUSSIAN     = 0x23, /**< Russian. */
  WMS_GW_CB_LANGUAGE_ICELANDIC   = 0x24, /**< Icelandic. */
  WMS_GW_CB_LANGUAGE_RESERVED_25 = 0x25, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_26 = 0x26, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_27 = 0x27, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_28 = 0x28, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_29 = 0x29, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_2A = 0x2a, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_2B = 0x2b, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_2C = 0x2c, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_2D = 0x2d, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_2E = 0x2e, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_RESERVED_2F = 0x2f, /**< Reserved. */
  WMS_GW_CB_LANGUAGE_ISO_639     = 0x10,
    /**< ISO639 2-letter language code is in the first two GSM 7-bit characters 
         of the CB user data. */
  /** @cond */
  WMS_GW_CB_LANGUAGE_DUMMY       = 255
  /** @endcond */
} wms_gw_cb_language_e_type;


/** GSM/WCDMA CB data coding scheme groups. 
*/
typedef enum
{
  WMS_GW_CB_DCS_GROUP_DEFINED   = 0, /**< Defined. */
  WMS_GW_CB_DCS_GROUP_WAP,           /**< Wireless Access Protocol. */
  WMS_GW_CB_DCS_GROUP_RESERVED       /**< Reserved. */
} wms_gw_cb_dcs_group_e_type;
/** @} */ /* end_addtogroup mmbroadcast_group_data_types */


/** @ingroup mmbroadcast_group_data_types
  GSM/WCDMA CB data coding schemes.
*/
typedef struct wms_gw_cb_dcs_s
{
  wms_gw_cb_dcs_group_e_type  group;          /**< Type of DCS group. */

  /* If group==DEFINED:  */
  wms_message_class_e_type    msg_class;       /**< If the group is defined. */
  boolean                     is_compressed;   /**< If the group is defined. */
  wms_gw_alphabet_e_type      alphabet;        /**< If the group is defined. */
  wms_gw_cb_language_e_type   language;        /**< If the group is defined. */
  uint8                       iso_639_lang[4];
    /**< If the group is defined: two GSM characters and one CR character. last bit is to indicate end of string */
  boolean                     is_udh_present;  /**< If the group is defined. */

  /* If group==WAP or RESERVED: (This is available for other DCS groups too) */
  uint8                       raw_dcs_data;
    /**< If the group is Wireless Application Protocol or Reserved. This can 
         also be used if the group is defined. */
} wms_gw_cb_dcs_s_type;



/* ===== Messaging Group ===== */
/** @ingroup messaging_group_data_types
  GSM/WCDMA cell broadcast decoded page header.
*/
typedef struct wms_gw_cb_page_header_s
{
  wms_gw_cb_srv_id_type        cb_srv_id;
    /**< Service ID. */
  wms_gw_cb_dcs_s_type         cb_dcs;
    /**< CB data coding scheme; see #wms_gw_cb_dcs_s for details. */
  uint8                        total_pages;
    /**< Total pages defined. */
    /* [1, 15] */ 
  uint8                        page_number;
    /**< Page number. */
    /* [1, 15] */ 

  /* The following are fields in Serial Number as defined in 3GPP 23.041 */
  wms_gw_cb_geo_scope_e_type   geo_scope;
    /**< Geographic scope for a cell broadcast. In the Serial Number as defined 
         in 3GPP 23.041. */
  uint16                       message_code;
    /**< Message code. In the Serial Number as defined in 3GPP 23.041. */
    /* [0, 1023] */ 
  uint8                        update_number;
    /**< Update number. In the Serial Number as defined in 3GPP 23.041. */
    /* [0, 15] */ 
  uint16                       raw_serial_number;
    /**< Original serial number raw data. */
  sys_modem_as_id_e_type       as_id; /**< Active subscription ID. */
} wms_gw_cb_page_header_s_type;


/** @addtogroup mmbroadcast_group_constants
@{ */
/** Maximum number of digits in mobile country code.
*/
#define WMS_MAX_MCC_DIGITS 3 

/** Maximum number of digits in mobile network code.
*/   
#define WMS_MAX_MNC_DIGITS 3 

/** Maximum number of bytes in the ETWS digital signature.
*/  
#define WMS_ETWS_DIGITAL_SIGNATURE_LEN 43  

/** Maximum length (in bytes) of the raw secondary ETWS notification.
*/  
#define WMS_ETWS_MAX_MSG_LEN 1252

/** Maximum length (in bytes) of unpacked user data in the secondary ETWS 
    notification.
*/
#define WMS_ETWS_MAX_USER_DATA_LEN 1406

/** Maximum number of information pages in the secondary ETWS notification.
*/  
#define WMS_ETWS_MAX_INFO_PAGE_NUMBER 15
/** @} */ /* end_addtogroup mmbroadcast_group_constants */

/** @addtogroup mmbroadcast_group_data_types
@{ */
/** PLMN ID information received from the lower layer.
*/
typedef struct wms_plmn_id_s
{
  uint8 mcc[WMS_MAX_MCC_DIGITS];  /**< Mobile country code. */
  uint8 num_mnc_digits;           /**< Number of digits in mobile network code. */
  uint8 mnc[WMS_MAX_MNC_DIGITS];  /**< Mobile network code. */
} wms_plmn_id_s_type;


/** Earthquake and Tsunami Warning System (ETWS) 
  warning security information.
*/
typedef struct wms_etws_security_info_s
{
  wms_timestamp_s_type     timestamp;
  /**< Official time the message was sent. */
  uint8                    digital_signature[WMS_ETWS_DIGITAL_SIGNATURE_LEN];
  /**< Signature of the authority sending the message (for verification). */
} wms_etws_security_info_s_type; 



/** ETWS primary notification structure.
*/
typedef struct wms_etws_primary_notification_s
{
  wms_gw_cb_page_header_s_type      cb_header_info;
    /**< CB header information included in the message ID and serial number; 
         see #wms_gw_cb_page_header_s for details. */
  boolean                           activate_emergency_user_alert;
    /**< Whether to activate the emergency user alert.  */
  boolean                           activate_popup;
    /**< Whether to activate the display pop-up. */  
  wms_etws_warning_e_type           warning_type;
    /**< ETWS warning type   */
  boolean                           security_info_present;
    /**< Whether the ETWS primary notification contains warning
         security information. */
  wms_etws_security_info_s_type     security_info;
    /**< ETWS warning security information; see #wms_etws_security_info_s for 
         details. */

} wms_etws_primary_notification_s_type;


/** ETWS secondary notification structure.
*/
typedef struct wms_etws_secondary_notification_s
{
  wms_gw_cb_page_header_s_type         cb_header_info;
    /**< CB header information included in the message ID and serial number; 
         see #wms_gw_cb_page_header_s for details. */
  boolean                              activate_emergency_user_alert;
    /**< Whether to activate the emergency user alert.  */
  boolean                              activate_popup;
    /**< Whether to activate the display pop-up. */ 
  wms_etws_notification_format_e_type  format;
    /**<  Format (GSM or UMTS) used to deliver the notification. */
  uint32                               cb_data_len;   
    /**< Secondary warning message length. */
  uint8                                cb_data[WMS_ETWS_MAX_USER_DATA_LEN];
    /**< Secondary warning message. */
  uint8                                page_lengths[WMS_ETWS_MAX_INFO_PAGE_NUMBER];
    /**< Length of each information page. */
} wms_etws_secondary_notification_s_type;


/** ETWS message structure.
*/
typedef struct wms_etws_msg_s
{
  wms_etws_notification_e_type      type;    
    /**< Notification type: primary or secondary. */
  boolean                           plmn_info_present; 
    /**< Whether PLMN information is present. */
  wms_plmn_id_s_type                plmn_info;
    /**< PLMN ID information; see #wms_plmn_id_s for details. */
  uint32                            data_len;
    /**< ETWS notification length. */
  uint8                             data[WMS_ETWS_MAX_MSG_LEN];
    /**< ETWS notification raw data. */
  sys_modem_as_id_e_type            as_id;
    /**< Active subscription ID. */ 
} wms_etws_msg_s_type;

/** @} */ /* end_addtogroup mmbroadcast_group_data_types */


/** @ingroup messaging_group_constants
  Size of the IP address.
*/
#define WMS_IP_ADDRESS_SIZE     4 


/** @addtogroup messaging_group_data_types
@{ */
/** IP address structure.
*/
typedef struct wms_ip_address_s
{
  uint8               address[WMS_IP_ADDRESS_SIZE]; /**< IP address. */
  boolean             is_valid;     /**< Indicates whether the IP address entry 
                                         is valid. */
} wms_ip_address_s_type;


/** Special structure that captures any unrecognized and/or proprietary 
  parameters.
*/
typedef struct wms_other_parm_s
{
  uint8                         input_other_len;
    /**< Length of other parameters. */
  uint8                         desired_other_len;
    /**< Number of bytes needed to properly decode the other parameters. */
  uint8                         * other_data;
  /**< Pointer to the other data. @newpagetable */
  /*~ FIELD wms_other_parm_s.other_data VARRAY LENGTH wms_other_parm_s.input_other_len*/
} wms_other_parm_s_type;


/** CDMA client message structure.
*/
typedef struct wms_cdma_message_s
{
  boolean                      is_mo;
    /**< TRUE -- MO message; FALSE -- MT message. */
  wms_teleservice_e_type       teleservice;
    /**< Type of teleservice. If the type is not available, it is set to Unknown. */
  wms_address_s_type           address;
    /**< For incoming messages, this is the origination address. Otherwise, this 
         is the destination address. See #wms_address_s for details. This parameter 
         is mandatory in both incoming and outgoing messages. */
  wms_subaddress_s_type        subaddress;
    /**< (Optional) See #wms_subaddress_s for details. 
         If subaddress.number_of_digits == 0, this field is not present. */
  boolean                      is_tl_ack_requested;
    /**< Indicates whether the client is to confirm whether the message is 
         received successfully. */
  boolean                      is_service_present;
    /**< Indicates whether the service is present. */
  wms_service_e_type           service;
    /**< Type of service. */
  wms_raw_ts_data_s_type       raw_ts;
    /**< Raw (un-decoded) bearer data; see #wms_raw_ts_data_s for details. */
} wms_cdma_message_s_type;
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @addtogroup messaging_group_data_types
@{ */
/** @name Bearer Data Subparameter Mask Values
  These values are used by the mask parameter of the CDMA bearer data structure, 
  #wms_client_bd_s. 
*/
/** @{ */
enum{ WMS_MASK_BD_NULL             =   0x00000000 }; 
enum{ WMS_MASK_BD_MSG_ID           =   0x00000001 };
enum{ WMS_MASK_BD_USER_DATA        =   0x00000002 };
enum{ WMS_MASK_BD_USER_RESP        =   0x00000004 };
enum{ WMS_MASK_BD_MC_TIME          =   0x00000008 };
enum{ WMS_MASK_BD_VALID_ABS        =   0x00000010 };
enum{ WMS_MASK_BD_VALID_REL        =   0x00000020 };
enum{ WMS_MASK_BD_DEFER_ABS        =   0x00000040 };
enum{ WMS_MASK_BD_DEFER_REL        =   0x00000080 };
enum{ WMS_MASK_BD_PRIORITY         =   0x00000100 };
enum{ WMS_MASK_BD_PRIVACY          =   0x00000200 };
enum{ WMS_MASK_BD_REPLY_OPTION     =   0x00000400 };
enum{ WMS_MASK_BD_NUM_OF_MSGS      =   0x00000800 };
enum{ WMS_MASK_BD_ALERT            =   0x00001000 };
enum{ WMS_MASK_BD_LANGUAGE         =   0x00002000 };
enum{ WMS_MASK_BD_CALLBACK         =   0x00004000 };
enum{ WMS_MASK_BD_DISPLAY_MODE     =   0x00008000 };
enum{ WMS_MASK_BD_SCPT_DATA        =   0x00010000 };
enum{ WMS_MASK_BD_SCPT_RESULT      =   0x00020000 };
enum{ WMS_MASK_BD_DEPOSIT_INDEX    =   0x00040000 };
enum{ WMS_MASK_BD_DELIVERY_STATUS  =   0x00080000 };
enum{ WMS_MASK_BD_IP_ADDRESS       =   0x10000000 };
enum{ WMS_MASK_BD_RSN_NO_NOTIFY    =   0x20000000 };
enum{ WMS_MASK_BD_OTHER            =   0x40000000 };
/** @} */ /* end_name_group Bearer Data Subparameter Mask Values */
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @ingroup messaging_group_data_types
  CDMA bearer data structure.
*/
typedef struct wms_client_bd_s
{
  uint32                        mask;
    /**< Indicates which of the following fields are present. */
  wms_message_id_s_type         message_id;
    /**< Type of message; see #wms_message_id_s for details. */
  wms_cdma_user_data_s_type     user_data;
    /**< User data; see #wms_cdma_user_data_s for details. */
  wms_user_response_type        user_response; 
    /**< Identifies a predefined response from the Message Center sent in the 
         SMS User Acknowledgment Message in response to previously received short 
         messages. */
  wms_timestamp_s_type          mc_time;
    /**< Message Center timestamp, which can be included with SMS messages sent 
         from the Message Center. */
  wms_timestamp_s_type          validity_absolute;
    /**< Tells the Message Center the absolute time of the validity period, after 
         which the message should be discarded if not delivered to the destination. */
  wms_timestamp_s_type          validity_relative;
    /**< Tells the Message Center the time period, starting from the time the 
         message is received by the Message Center, after which the message 
         should be discarded if not delivered to the destination.
         It can also be used to indicate the time period to retain a message 
         sent to a mobile station. */
  wms_timestamp_s_type          deferred_absolute;
    /**< Absolute time of delivery desired by the originator.  */
  wms_timestamp_s_type          deferred_relative;  
    /**< Relative time of delivery desired by the sender.
         It indicates the time period, starting from the time the message is 
         received by the Message Center, after which the message should be 
         delivered. */
  wms_priority_e_type           priority;  /**< Priority level of the message. */
  wms_privacy_e_type            privacy;   /**< Privacy level of the message. */
  wms_reply_option_s_type       reply_option; 
    /**< Indicates whether SMS acknowledgment is requested or not requested; 
         see #wms_reply_option_s for details. */
  uint8                         num_messages; 
    /**< Number of messages stored at the Voice Mail System. This represents 
         the actual value, not broadcast data. */
  wms_alert_mode_e_type         alert_mode;      
    /**< For pre-IS-637A implementations, alert_mode is either OFF or ON. The 
         type of alert used can distinguish different priorities of the message. */
  wms_language_e_type           language;
    /**< Language of the message. */
  wms_address_s_type            callback;
    /**< Call-back number to be dialed in reply to a received SMS message; 
         see #wms_address_s for details. */
  wms_display_mode_e_type       display_mode;
    /**< Indicates to the mobile station when to display the received message. */
  wms_download_mode_e_type      download_mode;
    /**< Type of download supported. */
  wms_delivery_status_s_type    delivery_status; 
    /**< Indicates the status of a message, or if an SMS error has occurred, the 
         nature of the error and whether the condition is considered temporary or 
         permanent; see #wms_delivery_status_s for details. */
  uint32                        deposit_index;   
    /**< Assigned by the Message Center as a unique index to the contents of the 
         User Data subparameter in each message sent to a particular mobile station.
         When replying to a previously received short message that included a 
         Message Deposit Index subparameter, the mobile station may include the 
         Message Deposit Index of the received message to indicate to the Message 
         Center that the original contents of the message are to be included in 
         the reply. */

    /* NOTE: All SCPT related definitions are for internal use only!  */

/** @cond */
  wms_bc_scpt_data_s_type       * scpt_data_ptr;
  /*~ FIELD wms_client_bd_s.scpt_data_ptr VARRAY LENGTH 0 */

  wms_bc_scpt_result_s_type     * scpt_result_ptr;
  /*~ FIELD wms_client_bd_s.scpt_result_ptr VARRAY LENGTH 0 */
/** @endcond */ /* SCPT - Internal use only */

  wms_ip_address_s_type         ip_address;
    /**< IP address; see #wms_ip_address_s for details. */
  uint8                         rsn_no_notify;
    /**< Reason of no notify. */

  wms_other_parm_s_type         other; 
    /**< Unrecognized or proprietary parameters included; see #wms_other_parm_s 
         for details. See the function comments for wms_ts_decode() and 
         wms_ts_decode_cdma_bd_with_other() for details about other parameters. */
} wms_client_bd_s_type;


/** @addtogroup messaging_group_data_types
@{ */
/** @name CDMA Template Masks
  CDMA template masks (Parameter Indicators byte 5 and 6), as defined in 
  3GPP2 C.S0023-D. 
  
  These values are used by the mask parameter of the template structure for 
  CDMA messages, #wms_cdma_template_s, to indicate which fields are present 
  in the message. 
*/
/** @{ */
enum
{
  /** @cond */
  WMS_CDMA_TEMPLATE_MASK_ORIG_ADDR             = 0x0100/** @endcond */, /* Not used. */
  WMS_CDMA_TEMPLATE_MASK_DEST_ADDR             = 0x0200, 
  /** @cond */
  WMS_CDMA_TEMPLATE_MASK_RESERVED_4            = 0x0400/** @endcond */, /* Not used. */
  WMS_CDMA_TEMPLATE_MASK_DCS                   = 0x0800, 
  WMS_CDMA_TEMPLATE_MASK_VALIDITY              = 0x1000, 
  WMS_CDMA_TEMPLATE_MASK_SERVICE               = 0x2000, 
  /** @cond */
  WMS_CDMA_TEMPLATE_MASK_ORIG_SUBADDR          = 0x4000/** @endcond */, /* Not used. */
  WMS_CDMA_TEMPLATE_MASK_DEST_SUBADDR          = 0x8000, 
  WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION   = 0x0001, 
    /* Indicates whether an SMS acknowledgment is requested. */
  WMS_CDMA_TEMPLATE_MASK_BEARER_DATA           = 0x0002 
};
/** @} */ /* end_name_group CDMA Template Masks */
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @addtogroup messaging_group_data_types
@{ */
/** Template structure for CDMA messages. 

  If WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION is set in the mask, there is no 
  corresponding field for it in this structure.
*/
typedef struct wms_cdma_template_s
{
  wms_teleservice_e_type          teleservice;
    /**< Type of teleservice. */
  uint16                          mask; 
    /**< Indicates which of the following fields are present. */
  wms_address_s_type              address;
    /**< Address parameters for the message; see #wms_address_s for details. */
  wms_subaddress_s_type           subaddress;
    /**< Subaddress parameter; see #wms_subaddress_s for details. */
  wms_user_data_encoding_e_type   dcs;
    /**< Data coding scheme for user data. */
  wms_timestamp_s_type            relative_validity;
    /**< Validity Period, relative format. */
  wms_service_e_type              service;
    /**< Type of service to use. */
  wms_client_bd_s_type            client_bd;
    /**< Client bearer data type; see #wms_client_bd_s for details. */
} wms_cdma_template_s_type;


/** Acknowledgment information for CDMA messages.
*/
typedef struct wms_cdma_ack_info_s
{
  wms_error_class_e_type      error_class;
    /**< Type of error (temporary or permanent). */
  wms_cdma_tl_status_e_type   tl_status;
    /**< CDMA-specific status code for a Transport Layer error. @newpagetable */
} wms_cdma_ack_info_s_type;



/* ====================== GSM/WCDMA message definitions ==================== */


/** Validity information for GSM/WCDMA messages.
*/
typedef struct wms_gw_validity_s
{
  wms_gw_validity_format_e_type  format; /**< Validity Period Format. */ 

  /** Used by the #wms_gw_validity_s structure.
  */
  union wms_gw_validity_u 
  {
    wms_timestamp_s_type    time;
      /**< Used by both absolute and relative formats. */
    /*~ IF (_DISC_ == WMS_GW_VALIDITY_RELATIVE || _DISC_ == WMS_GW_VALIDITY_ABSOLUTE) 
     wms_gw_validity_u.time */
      
    wms_enhanced_vp_s_type  enhanced_vp;
      /**< Enhanced validity period. */ 
    /*~ IF (_DISC_ == WMS_GW_VALIDITY_ENHANCED) wms_gw_validity_u.enhanced_vp */

    /*~ DEFAULT wms_gw_validity_u.void */
  } u; /**< Used by the #wms_gw_validity_s structure. */
  /*~ FIELD wms_gw_validity_s.u DISC _OBJ_.format */
} wms_gw_validity_s_type;


/** User data types for GSM/WCDMA messages. 

  If DCS indicates the default 7-bit alphabet, each byte holds one character 
  with bit 7 equal to 0, and sm_len indicates the number of characters. If DCS 
  indicates otherwise, each character can occupy multiple bytes, and sm_len 
  indicates the total number of bytes.
*/
typedef struct wms_gw_user_data_s
{
  uint8                          num_headers;
    /**< Number of user data headers. */
  wms_udh_s_type                 headers[WMS_MAX_UD_HEADERS];
    /**< Types of user data headers; see #wms_udh_s for details. */
  uint16                         sm_len;
    /**< Length of short message. */
  uint8                          sm_data[WMS_MAX_LEN];
    /**< Short message data. */
} wms_gw_user_data_s_type;


/** Deliver TPDU for GSM/WCDMA messages.
*/
typedef struct wms_gw_deliver_s
{
  boolean                          more;
    /**< TP More Messages to Send (TP-MMS). */
  boolean                          reply_path_present;
    /**< TP Reply Path (TP-RP). */
  boolean                          user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  boolean                          status_report_enabled;
    /**< TP Status Report Indication (TP-SRI). */
  wms_address_s_type               address;
    /**< TP Originating Address (TP-OA); see #wms_address_s for details. */
  wms_pid_e_type                   pid;
    /**< TP Protocol Identifier (TP-PID). */
  wms_gw_dcs_s_type                dcs;
    /**< TP Data Coding Scheme (TP-DCS); see #wms_gw_dcs_s for details. */
  wms_timestamp_s_type             timestamp;
    /**< TP Service Centre Time Stamp (TP-SCTS). */
  wms_gw_user_data_s_type          user_data;
    /**< TP User Data (TP-UD); see #wms_gw_user_data_s for details. @newpagetable */
} wms_gw_deliver_s_type;


/** Deliver report with acknowledgment TPDU; for GSM/WCDMA messages.
*/
typedef struct wms_gw_deliver_report_ack_s
{
  boolean                         user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  uint32                          mask;
    /**< Indicates which of the following optional fields are present. */
  wms_pid_e_type                  pid;
    /**< TP Protocol Identifier (TP-PID). */
  wms_gw_dcs_s_type               dcs;
    /**< TP Data Coding Scheme (TP-DCS); see #wms_gw_dcs_s for details. */
  wms_gw_user_data_s_type         user_data;
    /**< TP User Data (TP-UD); see #wms_gw_user_data_s for details. */
} wms_gw_deliver_report_ack_s_type;


/** Deliver report with error TPDU; for GSM/WCDMA messages.
*/
typedef struct wms_gw_deliver_report_error_s
{
  boolean                         user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  wms_tp_cause_e_type             tp_cause;
    /**< TP Failure Cause (TP-FCS). */
  uint32                          mask;
    /**< Indicates which of the following optional fields are present. */
  wms_pid_e_type                  pid;
    /**< TP Protocol Identifier (TP-PID). */
  wms_gw_dcs_s_type               dcs;
    /**< TP Data Coding Scheme (TP-DCS); see #wms_gw_dcs_s for details. */
  wms_gw_user_data_s_type         user_data;
    /**< TP User Data (TP-UD); see #wms_gw_user_data_s for details. */
} wms_gw_deliver_report_error_s_type;


/** Submit TPDU definitions for GSM/WCDMA messages.
*/
typedef struct wms_gw_submit_s
{
  boolean                           reject_duplicates;
    /**< TP Reject Duplicates (TP-RD). */
  boolean                           reply_path_present;
    /**< TP Reply Path (TP-RP). */
  boolean                           user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  boolean                           status_report_enabled;
    /**< TP Status Report Request (TP-SRR). */
  wms_message_number_type           message_reference;
    /**< TP Message Reference (TP-MR). */
  wms_address_s_type                address;
    /**< TP Destination Address (TP-DA); see #wms_address_s for details. */
  wms_pid_e_type                    pid;
    /**< TP Protocol Identifier (TP-PID). */
  wms_gw_dcs_s_type                 dcs;
    /**< TP Data Coding Scheme (TP-DCS); see #wms_gw_dcs_s for details. */
  wms_gw_validity_s_type            validity;
    /**< TP Validity Period Format (TP-VPF) and TP Validity Period (TP-VP);
         see #wms_gw_validity_s for details. */
  wms_gw_user_data_s_type           user_data;
    /**< TP User Data (TP-UD); see #wms_gw_user_data_s for details. */
} wms_gw_submit_s_type;
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @addtogroup messaging_group_data_types
@{ */
/** @name TPDU Parameter Bitmasks for GSM/WCDMA Messages 
  These values are used by the mask parameters of structures to indicate which 
  optional fields are present in GSM/WCDMA messages for the following types of 
  reports:
  - Submit report with acknowledgment TPDU (#wms_gw_submit_report_ack_s)
  - Submit report with error TPDU (#wms_gw_submit_report_error_s)
  - Status report TPDU (#wms_gw_status_report_s) 

  The values are:
*/
/** @{ */
enum
{
  WMS_TPDU_MASK_PID         = 0x0001,
  WMS_TPDU_MASK_DCS         = 0x0002,
  WMS_TPDU_MASK_USER_DATA   = 0x0004
};
/** @} */ /* end_name_group TPDU Parameter Bitmasks for GSM/WCDMA Messages */
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @addtogroup messaging_group_data_types
@{ */
/** Submit report with acknowledgment TPDU; for GSM/WCDMA messages.
*/
typedef struct wms_gw_submit_report_ack_s
{
  boolean                        user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  wms_timestamp_s_type           timestamp;
    /**< TP Service Centre Time Stamp (TP-SCTS). */
  uint32                         mask;
    /**< Indicates which of the following optional fields are present. */         
  wms_pid_e_type                 pid;
    /**< TP Protocol Identifier (TP-PID). */
  wms_gw_dcs_s_type              dcs;
    /**< TP Data Coding Scheme (TP-DCS); see #wms_gw_dcs_s for details. */
  wms_gw_user_data_s_type        user_data;
    /**< TP User Data (TP-UD); see #wms_gw_user_data_s for details. */
} wms_gw_submit_report_ack_s_type;


/** Submit report with error TPDU; for GSM/WCDMA messages.
*/
typedef struct wms_gw_submit_report_error_s
{
  boolean                       user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  wms_tp_cause_e_type           tp_cause;
    /**< TP Failure Cause (TP-FCS). */
  wms_timestamp_s_type          timestamp;
    /**< TP Service Centre Time Stamp (TP-SCTS). */
  uint32                        mask;
    /**< Indicates which of the following optional fields are present. */
  wms_pid_e_type                pid;
    /**< TP Protocol Identifier (TP-PID). */
  wms_gw_dcs_s_type             dcs;
    /**< TP Data Coding Scheme (TP-DCS); see #wms_gw_dcs_s for details. */
  wms_gw_user_data_s_type       user_data;
    /**< TP User Data (TP-UD); see #wms_gw_user_data_s for details. */
} wms_gw_submit_report_error_s_type;


/** Status report TPDU for GSM/WCDMA messages.
*/
typedef struct wms_gw_status_report_s
{
  boolean                       user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  boolean                       more;
    /**< TP More Messages to Send (TP-MMS). */
  boolean                       status_report_qualifier;
    /**< TP Status Report Qualifier (TP-SRQ). */
  wms_message_number_type       message_reference;
    /**< TP Message Reference (TP-MR). */
  wms_address_s_type            address;
    /**< TP Recipient Address (TP-RA); see #wms_address_s for details. */
  wms_timestamp_s_type          timestamp;
    /**< TP Service Centre Time Stamp (TP-SCTS). */
  wms_timestamp_s_type          discharge_time;
    /**< TP Discharge Time (TP-DT). */
  wms_tp_status_e_type          tp_status;
    /**< TP Status (TP-ST). */
  uint32                        mask;
    /**< Indicates which of the following optional fields are present. */
  wms_pid_e_type                pid;
    /**< TP Protocol Identifier (TP-PID). */
  wms_gw_dcs_s_type             dcs;
    /**< TP Data Coding Scheme (TP-DCS); see #wms_gw_dcs_s for details. */
  wms_gw_user_data_s_type       user_data;
    /**< TP User Data (TP-UD); see #wms_gw_user_data_s for details. @newpagetable */
} wms_gw_status_report_s_type;


/** Command TPDU for GSM/WCDMA messages.
*/
typedef struct wms_gw_command_s
{
  boolean                      user_data_header_present;
    /**< TP User Data Header Indicator (TP-UDHI). */
  boolean                      status_report_enabled;
    /**< TP Status Report Request (TP-SRR). */
  wms_message_number_type      message_reference;
    /**< TP Message Reference (TP-MR). */
  wms_pid_e_type               pid;       /**< TP Protocol Identifier (TP-PID). */
  wms_gw_command_e_type        command;   /**< TP Command Type (TP-CT). */
  wms_message_number_type      message_number; /**< TP Message Number (TP-MN). */
  wms_address_s_type           address;
    /**< TP Destination Address (TP-DA); see #wms_address_s for details. */
  uint8                        command_data_len;
    /**< TP Command Data Length (TP-CDL). */
  uint8                        command_data[WMS_GW_COMMAND_DATA_MAX]; 
    /**< TP Command Data (TP-CD). */
} wms_gw_command_s_type;
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @addtogroup messaging_group_data_types
@{ */
/** @name GSM/WCDMA Template Masks 
  GSM/WCDMA template masks; refer to 3GPP TS 31.102 (SMS parameters).

  These values are used by the mask parameter of the template structure for 
  GSM/WCDMA messages, #wms_gw_template_s, to indicate which fields are present 
  in the message. 
*/
/** @{ */
enum
{
  WMS_GW_TEMPLATE_MASK_DEST_ADDR = 0x01,
  WMS_GW_TEMPLATE_MASK_SC_ADDR   = 0x02,
  WMS_GW_TEMPLATE_MASK_PID       = 0x04,
  WMS_GW_TEMPLATE_MASK_DCS       = 0x08,
  WMS_GW_TEMPLATE_MASK_VALIDITY  = 0x10
};
/** @} */ /* end_name_group Template Masks for GSM/WCDMA Messages */
/** @} */ /* end_addtogroup messaging_group_data_types */
/** @addtogroup messaging_group_data_types
@{ */
/** Alphabetic ID for GSM/WCDMA messages.
*/
typedef struct wms_alpha_id_s
{
  uint8   len;     /**< Length of a 7-bit GSM default alphabet string. */
  uint8   *data;   /**< Pointer to a 7-bit GSM default alphabet string. */
} wms_alpha_id_s_type;
/*~ FIELD wms_alpha_id_s.data VARRAY LENGTH wms_alpha_id_s.len */


/** Template structure for GSM/WCDMA messages.
*/
typedef struct wms_gw_template_s
{
  wms_alpha_id_s_type          alpha_id;
    /**< Identifier for a 7-bit default alphabet string; see 
         #wms_alpha_id_s for details. */
  uint8                        mask;
    /**< Indicates which of the following fields are present. */
  wms_address_s_type           dest_addr;
    /**< Destination address; see #wms_address_s for details. */
  wms_address_s_type           sc_addr;
    /**< Service center address; see #wms_address_s for details. */
  wms_pid_e_type               pid;
    /**< Protocol identifier data. */
  wms_gw_dcs_s_type            dcs;
    /**< Data coding scheme; see #wms_gw_dcs_s for details. */
  wms_timestamp_s_type         relative_validity;
  /**< Validity Period relative format. */
} wms_gw_template_s_type;


/** Message definition for GSM/WCDMA messages.
*/
typedef struct wms_gw_message_s
{
  boolean                       is_broadcast;
    /**< Indicates whether this a cell broadcast message. */
  wms_address_s_type            sc_address;
    /**< Address of the Switching Center; see #wms_address_s for details. */
  wms_raw_ts_data_s_type        raw_ts_data;
    /**< Raw Transport Service data; see #wms_raw_ts_data_s for details. */
} wms_gw_message_s_type;


/** Acknowledgment information for GSM/WCDMA messages.
*/
typedef struct wms_gw_ack_info_s
{
  boolean                              success; 
    /**< Indicates whether this is an acknowledgment or an error report. */
  wms_rp_cause_e_type                  rp_cause;
    /**< Report the cause of failure. This is used when success == FALSE. */
  wms_gw_tpdu_type_e_type              tpdu_type;
    /**< Use WMS_TPDU_NONE if deliver_report TPDUs are not included. @newpagetable */

  /** Used by the #wms_gw_ack_info_s structure.
  */
  union wms_gw_ack_info_u 
  {
    wms_gw_deliver_report_ack_s_type    ack;
    /**< Acknowledgment information; see #wms_gw_deliver_report_ack_s for 
         details. */
     /* if success==TRUE  */
    /*~ IF (_DISC_ == TRUE) wms_gw_ack_info_u.ack */
    wms_gw_deliver_report_error_s_type  error;
    /**< Error information; see #wms_gw_deliver_report_error_s for details. */
     /* if success==FALSE */ 
    /*~ IF (_DISC_ == FALSE) wms_gw_ack_info_u.error */

    /*~ DEFAULT wms_gw_ack_info_u.void */
  } u; /**< Used by the #wms_gw_ack_info_s structure. */
  /*~ FIELD wms_gw_ack_info_s.u DISC _OBJ_.success */
} wms_gw_ack_info_s_type;


/* ============ Client message definitions ========== */
/** Header definition for CDMA messages.
*/
typedef struct wms_client_msg_hdr_s
{
  wms_message_mode_e_type   message_mode;
    /**< Message mode (e.g., CDMA or GSM/WDCMA). */
  wms_message_tag_e_type    tag;
    /**< Set to WMS_TAG_NONE if not applicable. */
  wms_memory_store_e_type   mem_store;
    /**< Memory store used for storing this message. Set to 
         WMS_MEMORY_STORE_NONE if this message is not to be stored. */
  wms_message_index_type    index;
    /**< Index for a stored message. Set to WMS_DUMMY_MESSAGE_INDEX when 
         mem_store == WMS_MEMORY_STORE_NONE. */
} wms_client_msg_hdr_s_type;          


/** Information about a client message of all types.

  Message types include MT, MO, regular message, report, template, CB message, 
  etc. 
*/
typedef struct wms_client_message_s
{
  wms_client_msg_hdr_s_type     msg_hdr;
  /**< Header definition; see #wms_client_msg_hdr_s for details. */

  /** Used by the #wms_client_message_s structure. The message_mode 
      and tag together decide which structure to use in this union. */
  union wms_client_message_u
  {
    wms_cdma_message_s_type      cdma_message;
    /**< CDMA message type; see #wms_cdma_message_s for details. */
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_CDMA &&
          _DISC_.tag != WMS_TAG_MO_TEMPLATE)
        wms_client_message_u.cdma_message*/

    wms_cdma_template_s_type     cdma_template;
    /**< CDMA message template; see #wms_cdma_template_s for details. */
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_CDMA &&
          _DISC_.tag == WMS_TAG_MO_TEMPLATE)
       wms_client_message_u.cdma_template*/

    wms_gw_message_s_type        gw_message;
    /**< GSM/WCDMA message type; see #wms_gw_message_s for details. */
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_GW &&
          _DISC_.tag != WMS_TAG_MO_TEMPLATE)
        wms_client_message_u.gw_message*/

    wms_gw_template_s_type       gw_template;
    /**< GSM/WCDMA message template; see #wms_gw_template_s for details. */
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_GW &&
          _DISC_.tag == WMS_TAG_MO_TEMPLATE)
        wms_client_message_u.gw_template*/

    /*~ DEFAULT wms_client_message_u.void */

   } u; /**< Used by the #wms_client_message_s structure. */
   /*~ FIELD wms_client_message_s.u DISC _OBJ_.msg_hdr */
} wms_client_message_s_type;
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @ingroup common_type_defs_group
  MO SMS call control status from GSTK.
*/
typedef enum
{
  WMS_MO_CTRL_STATUS_NO_MOD = 0, /**< No modifications. */
  WMS_MO_CTRL_STATUS_MOD,        /**< Modifications. */
  WMS_MO_CTRL_STATUS_DISALLOWED, /**< Not allowed. */
/** @cond */
  WMS_MO_CTRL_STATUS_MAX,
  WMS_MO_CTRL_STATUS_MAX32 = 0x10000000  /* Pad to 32-bit int */
/** @endcond */
} wms_mo_ctrl_status_e_type;


/** @addtogroup messaging_group_data_types
@{ */
/* ============ Message event definitions =========== */
/** Status information of a message that was sent/stored.
*/
typedef struct wms_message_status_info_s
{
  void                        *user_data;
  /**< User data pointer passed in the original command to the WMS. */
  wms_client_type_e_type      client_id;
  /**< Client that issued the original command to the WMS. */
  wms_send_mode_e_type        send_mode;
  /**< Valid if the event is WMS_MSG_EVENT_SEND. */
  wms_write_mode_e_type       write_mode;
  /**< Valid if the event is WMS_MSG_EVENT_WRITE. */
  wms_client_message_s_type   message;
  /**< Message/template/status report that is operated on; see 
       #wms_client_message_s for details. */
  wms_alpha_id_s_type         alpha_id;
  /**< Valid if the event is WMS_MSG_EVENT_SEND; see #wms_alpha_id_s for 
       details. */
  sys_modem_as_id_e_type      as_id;
  /**< Active subscription ID. */
  wms_mo_ctrl_status_e_type   mo_ctrl_status;
  /**< MO call control status from GSTK. @newpagetable */
} wms_message_status_info_s_type;


/** Information about a message received from the network.
*/
typedef struct wms_mt_message_info_s
{
  wms_client_type_e_type         client_id; 
     /**< If the client ID is set to WMS_CLIENT_TYPE_MAX, this message was 
          broadcast to all clients. Otherwise, the message was delivered to a 
          single client of this particular client_id. */
  wms_route_e_type               route; 
     /**< For TRANSFER_ONLY, the client needs to acknowledge the message. */
  wms_transaction_id_type        transaction_id; 
     /**< Differentiate between multiple incoming messages. */
  wms_client_message_s_type      message; 
     /**< Information about the client message type; see 
          #wms_client_message_s for details. */
  sys_modem_as_id_e_type         as_id;
     /**< Active subscription ID. */
  wms_msg_transport_type_e_type  transport_type;
     /**< Transport layer over which the MT SMS is received. */
} wms_mt_message_info_s_type;


/** Information about an error regarding a message received from the 
  network.
*/
typedef struct wms_mt_message_error_s
{
  wms_transaction_id_type        transaction_id; /**< Differentiate between 
                                                      multiple incoming messages. */
  wms_report_status_e_type       report_status;  /**< Success or failure reason. */
  sys_modem_as_id_e_type         as_id;          /**< Active subscription ID. */
} wms_mt_message_error_s_type;


/** Cause information in the submit report; for message events.
*/
typedef struct wms_cause_info_s
{
  wms_error_class_e_type        error_class; 
     /**< Type of error (permanent or temporary). Applicable to CDMA only. */
  wms_cdma_tl_status_e_type     tl_status;
     /**< CDMA-specific status code for Transport Layer error. */
  wms_rp_cause_e_type           cause_value; 
     /**< GSM/WCDMA-specific Relay Protocol cause types. */                                    
  boolean                       diagnostics_present; 
     /**< Indicates whether diagnostics are present. If not, ignore the next 
          member. Applicable to GSM/WCDMA only. */
  uint8                         diagnostics; 
     /**< Diagnostic data. Applicable to GSM/WCDMA only. */
} wms_cause_info_s_type;


/** Submit report information for message events.
*/
typedef struct wms_submit_report_info_s
{
  void                        *user_data;
    /**< Pointer to user data. */
  wms_client_type_e_type      client_id;
    /**< Client that issued the original command to the WMS. */
  wms_report_status_e_type    report_status;
    /**< Success or failure reason. */
  wms_message_mode_e_type     message_mode;
    /**< System mode to be used for a message. */
  wms_cause_info_s_type       cause_info;
    /**< Cause information in the submit report; see 
         #wms_cause_info_s for details. */
  wms_gw_tpdu_type_e_type     tpdu_type;
    /**< GSM/WCDMA-specific TPDU type. */
  wms_alpha_id_s_type         alpha_id;
    /**< Alphabetic ID for GSM/WCDMA messages; see #wms_alpha_id_s for 
         details. */
  wms_mo_ctrl_status_e_type   mo_ctrl_status; 
    /**< MO call control status from GSTK. */

  /** Used by the #wms_submit_report_info_s structure.
  */
  union wms_submit_report_info_u
  {
    wms_gw_submit_report_ack_s_type    ack;
      /**< Acknowledge success; see #wms_gw_submit_report_ack_s for details.  
           Applicable to GSM/WCDMA only. */
    /*~ CASE WMS_RPT_OK wms_submit_report_info_u.ack */
    wms_gw_submit_report_error_s_type  error;
      /**< Report error; see #wms_gw_submit_report_error_s for details. 
           Applicable to GSM/WCDMA only. */
    /*~ DEFAULT wms_submit_report_info_u.error */

  } u; /**< Used by the #wms_submit_report_info_s structure. */
  /*~ FIELD wms_submit_report_info_s.u DISC _OBJ_.report_status */

  sys_modem_as_id_e_type      as_id;
    /**< Active subscription ID. */
  boolean                     delivery_failure_permanent;
    /**< Indicates whether the delivery failure in the lower layer is 
         permanent. */
} wms_submit_report_info_s_type;


/** Acknowledgment request information for message events.
*/
typedef struct wms_ack_info_s
{
  wms_transaction_id_type          transaction_id;
    /**< Differentiate between multiple incoming messages. */
  wms_message_mode_e_type          message_mode;
    /**< System mode to be used for a message. */

  /** Used by the #wms_ack_info_s structure.
  */
  union wms_ack_info_u
  {
    wms_cdma_ack_info_s_type       cdma;
      /**< Acknowledgment information for CDMA messages; see 
           #wms_cdma_ack_info_s for details. */
    /*~ CASE WMS_MESSAGE_MODE_CDMA wms_ack_info_u.cdma */
    wms_gw_ack_info_s_type         gw;
      /**< Acknowledgment information for GSM/WCDMA messages; see 
           #wms_gw_ack_info_s for details. */
    /*~ CASE WMS_MESSAGE_MODE_GW wms_ack_info_u.gw */
    /* TBD: check default mode */

    /*~ DEFAULT wms_ack_info_u.void */

  } u; /**< Used by the #wms_ack_info_s structure. */
  /*~ FIELD wms_ack_info_s.u DISC _OBJ_.message_mode */

  sys_modem_as_id_e_type           as_id; /**< Active subscription ID. */
} wms_ack_info_s_type;


/** Acknowledgment report information for message events.
*/
typedef struct
{
  wms_client_type_e_type           client_id;
    /**< Type of client application. */
  wms_transaction_id_type          transaction_id;
    /**< Differentiate between multiple incoming messages. */
  wms_message_mode_e_type          message_mode;
    /**< System mode to be used for a message. */
  void                             *user_data;
    /**< Pointer to the user data. */
  boolean                          is_success;
    /**< Indicates whether the acknowledgment was sent successfully. */
  wms_report_status_e_type         error_status;
    /**< Reason for the failure. */
  sys_modem_as_id_e_type           as_id; /**< Active subscription ID. */
} wms_ack_report_info_s_type;
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @ingroup messaging_group_data_types
  Message event types for message events.
*/
typedef enum
{
  /* Command related events */
  WMS_MSG_EVENT_SEND = 0,
    /**< Command-related event: reports the local status of the send request. */
  WMS_MSG_EVENT_ACK,
    /**< Command-related event: reports the local status of the acknowledgment 
         request. */
  WMS_MSG_EVENT_READ,
    /**< Command-related event: reports the result of the read request. */
  WMS_MSG_EVENT_WRITE,
    /**< Command-related event: reports the status of the send request.
         In insert mode, the new message index is returned; otherwise, the 
         original index is returned. */
  WMS_MSG_EVENT_DELETE,
    /**< Command-related event: reports the result of the delete message request. */
  WMS_MSG_EVENT_DELETE_ALL,
    /**< Command-related event: reports the result of the delete all messages 
         request. */
  WMS_MSG_EVENT_MODIFY_TAG,
    /**< Command-related event: reports the result of the modification request. */
  WMS_MSG_EVENT_READ_TEMPLATE,
    /**< Command-related event: reports the result of the read request. */
  WMS_MSG_EVENT_WRITE_TEMPLATE,
    /**< Command-related event: reports the status of the send request.
         In insert mode, the new message index is returned; otherwise, the 
         original index is returned. */
  WMS_MSG_EVENT_DELETE_TEMPLATE,
    /**< Command-related event: reports the result of the delete template 
         request. */
  WMS_MSG_EVENT_DELETE_TEMPLATE_ALL,
    /**< Command-related event: reports the result of the delete all templates 
         request. */
  WMS_MSG_EVENT_READ_STS_REPORT,
    /**< Command-related event: reads the status report event. */
  WMS_MSG_EVENT_WRITE_STS_REPORT,
    /**< Command-related event: reports the status of the send request. 
         In insert mode, the new message index is returned; otherwise, the 
         original index is returned. */
  WMS_MSG_EVENT_DELETE_STS_REPORT,
    /**< Command-related event: reports the result of the Delete Status report 
       request. */
  WMS_MSG_EVENT_DELETE_STS_REPORT_ALL,
    /**< Command-related event: reports the result of the Delete All Status 
         Reports request. */

  /* Events from lower layer  */
  WMS_MSG_EVENT_RECEIVED_MESSAGE,
    /**< Event from the lower layer: message was received at the lower level. */
  WMS_MSG_EVENT_SUBMIT_REPORT,
    /**< Event from the lower layer: submit or status report from the network 
         if there was no local error. The status report can be saved to the SIM. */
  WMS_MSG_EVENT_STATUS_REPORT,
    /**< Event from the lower layer: status report from the lower level. */
  WMS_MSG_EVENT_MT_MESSAGE_ERROR,
    /**< Event from the lower layer: message error. An example is an MT message 
         acknowledgment timeout. */
  WMS_MSG_EVENT_ACK_REPORT,
    /**< Event from the lower layer: status of the client's acknowledgment to 
         an MT message. */
  WMS_MSG_EVENT_DUPLICATE_CB_PAGE,
    /**< Event from the lower layer: duplicate cell broadcast page. */
  WMS_MSG_EVENT_TRANSPORT_REG,
    /**< Event to notify clients about the registration/deregistration of an 
         SMS Transport Layer */ 
         
  WMS_MSG_EVENT_ETWS_NOTIFICATION,
   /**< Event to notify clients upon receiving an ETWS primary or secondary 
        notification. */
  
  WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION,
   /**< Event to notify clients upon receiving MWI notification from 
        trasnport layer. */   
        
  WMS_MSG_EVENT_SMSC_ADDR_CHANGE,
  /**< Event to notify clients that the default SMSC address template has 
       changed. */       
/** @cond */
  WMS_MSG_EVENT_MAX,
  WMS_MSG_EVENT_MAX32 = 0x10000000
/** @endcond */
} wms_msg_event_e_type;
/*~ SENTINEL wms_msg_event_e_type.WMS_MSG_EVENT_MAX */

/** @addtogroup messaging_group_data_types
@{ */
/** Message event information for each event.
*/
typedef union wms_msg_event_info_u
{
  wms_message_status_info_s_type     status_info;
    /**< Status information of a message that was sent/stored; see
         #wms_message_status_info_s for details. */

  /*~ DEFAULT wms_msg_event_info_u.status_info */
    /* WMS_MSG_EVENT_SEND
    ** WMS_MSG_EVENT_READ
    ** WMS_MSG_EVENT_WRITE
    ** WMS_MSG_EVENT_DELETE
    ** WMS_MSG_EVENT_DELETE_ALL
    ** WMS_MSG_EVENT_MODIFY_TAG
    ** WMS_MSG_EVENT_READ_TEMPLATE
    ** WMS_MSG_EVENT_WRITE_TEMPLATE
    ** WMS_MSG_EVENT_DELETE_TEMPLATE
    ** WMS_MSG_EVENT_DELETE_TEMPLATE_ALL
    ** WMS_MSG_EVENT_READ_STS_REPORT
    ** WMS_MSG_EVENT_WRITE_STS_REPORT
    ** WMS_MSG_EVENT_DELETE_STS_REPORT
    ** WMS_MSG_EVENT_DELETE_STS_REPORT_ALL 
    ** WMS_MSG_EVENT_SMSC_ADDR_CHANGE */ 

  wms_ack_info_s_type                ack_info; 
    /**< Acknowledgment request information for a message event; see 
         #wms_ack_info_s for details. */
    /* WMS_MSG_EVENT_ACK    */
 /*~ CASE WMS_MSG_EVENT_ACK wms_msg_event_info_u.ack_info */

  wms_mt_message_info_s_type         mt_message_info; 
    /**< Information about an MT message received from the network; see 
         #wms_mt_message_info_s for details. */
    /* WMS_MSG_EVENT_RECEIVED_MESSAGE
    ** WMS_MSG_EVENT_STATUS_REPORT    */
 /*~ IF (_DISC_ == WMS_MSG_EVENT_RECEIVED_MESSAGE || _DISC_ == WMS_MSG_EVENT_STATUS_REPORT) 
     wms_msg_event_info_u.mt_message_info */
 
  wms_submit_report_info_s_type      submit_report_info; 
    /**< Submit report information for a message event; see 
         #wms_submit_report_info_s for details. */
    /* WMS_MSG_EVENT_SUBMIT_REPORT    */
 /*~ CASE WMS_MSG_EVENT_SUBMIT_REPORT wms_msg_event_info_u.submit_report_info */

  wms_mt_message_error_s_type        mt_message_error; 
    /**< Information about an error regarding an MT message received from the 
         network; see #wms_mt_message_error_s for details. */
    /* WMS_MSG_EVENT_MT_MESSAGE_ERROR - Applicable to GW only    */
 /*~ CASE WMS_MSG_EVENT_MT_MESSAGE_ERROR wms_msg_event_info_u.mt_message_error */

  wms_ack_report_info_s_type         ack_report_info; 
    /**< Acknowledgment report information for a message event. */
    /* WMS_MSG_EVENT_ACK_REPORT    */
 /*~ CASE WMS_MSG_EVENT_ACK_REPORT wms_msg_event_info_u.ack_report_info */

  wms_gw_cb_page_header_s_type       cb_page_header; 
    /**< GSM/WCDMA cell broadcast decoded page header; see 
         #wms_gw_cb_page_header_s for details. */
    /* WMS_MSG_EVENT_DUPLICATE_CB_PAGE    */
 /*~ CASE WMS_MSG_EVENT_DUPLICATE_CB_PAGE wms_msg_event_info_u.cb_page_header */

  wms_msg_transport_reg_info_s_type  transport_reg_info;
    /**< Transport registration information for a message. */
    /* WMS_MSG_EVENT_TRANSPORT_REG    */
 /*~ CASE WMS_MSG_EVENT_TRANSPORT_REG wms_msg_event_info_u.transport_reg_info */

  wms_etws_msg_s_type                etws_msg_info;
    /**< ETWS notification information; see #wms_etws_msg_s for details. */
    /* WMS_MSG_EVENT_ETWS_NOTIFICATION    */
 /*~ CASE WMS_MSG_EVENT_ETWS_NOTIFICATION wms_msg_event_info_u.etws_msg_info */

  wms_msg_transport_mwi_info_s_type  transport_mwi_info;
    /**< Transport MWI notification information. */
    /* WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION    */
 /*~ CASE WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION wms_msg_event_info_u.transport_mwi_info */
  
} wms_msg_event_info_s_type/** @newpage */;
/** @} */ /* end_addtogroup messaging_group_data_types */


/** @ingroup messaging_group_wms_msg_event_cb_type
  Message event callback function.

  @datatypes
  #wms_msg_event_e_type \n
  #wms_msg_event_info_u

  @param[in]  event     Message event type.
  @param[in]  info_ptr  Message event information.
  @param[in]  shared    Indicates whether the message can be passed to other 
                        clients.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_msg_event_cb_type)
(
  wms_msg_event_e_type         event,
  wms_msg_event_info_s_type    *info_ptr,
  boolean                      *shared
);
/*~ PARAM IN info_ptr POINTER DISC event*/
/*~ PARAM OUT shared   POINTER */
/*~ CALLBACK wms_msg_event_cb_type
    ONERROR return  */


/** @ingroup messaging_group_wms_msg_transport_status_cb_type
  SMS/EMS Transport Layer network registration status callback function.

  @datatypes
  #wms_msg_transport_status_s_type

  @param[in]  status_ptr  Transport Layer status. 

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_msg_transport_status_cb_type)
(
   wms_msg_transport_status_s_type   *status_ptr
);
/*~ PARAM IN status_ptr POINTER */
/*~ CALLBACK wms_msg_transport_status_cb_type
    ONERROR return  */

/** @ingroup messaging_group_constants
Information about the message cache size.
*/
#define WMS_MSG_INFO_CACHE_SIZE    30


/** @ingroup messaging_group_wms_msg_info_cache_cb_type
  Message information cache callback function.

  @datatypes
  #wms_client_message_s

  @param[in]  msg_ptr    Pointer to the message input from the WMS.
  @param[in]  cache_ptr  Output from the client.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_msg_info_cache_cb_type)
(
  const wms_client_message_s_type         * msg_ptr,
  uint8                                   * cache_ptr
);
/*~ PARAM IN msg_ptr     POINTER */
/*~ PARAM OUT cache_ptr ARRAY WMS_MSG_INFO_CACHE_SIZE */ 
/*~ CALLBACK wms_msg_info_cache_cb_type 
    ONERROR return  */


/** @ingroup messaging_group_wms_cl_parse_msg_cb_type
  Specific parsing criteria provided by the client for message events. 
  If a client registers this callback to WMS, this callback is called each time 
  there is a mobile-terminated message. The client is to examine the message 
  pointed to by msg_ptr and return TRUE or FALSE in the shared parameter: \n
   - TRUE -- The client does not want to consume the message. The WMS 
     continues with normal processing.
   - FALSE -- The client wants to consume the message. The WMS delivers 
     the message only to this client using route TRANSFER_ONLY. The client is 
     responsible for an SMS acknowledgment.

  @datatypes
  #wms_client_message_s

  @param[in]  msg_ptr  Pointer to the message input from the WMS.
  @param[in]  shared   Indicates whether the message can be passed to other 
                       clients.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_cl_parse_msg_cb_type)
(
  const wms_client_message_s_type         *msg_ptr,
  boolean                                 *shared
);
/*~ PARAM IN msg_ptr  POINTER */
/*~ PARAM OUT shared   POINTER */
/*~ CALLBACK wms_cl_parse_msg_cb_type 
    ONERROR return  */



/* <EJECT> */
/* ========================================================================= */
/* =========================== DC group data types ========================= */
/* ========================================================================= */

/** @ingroup dc_group_data_types 
  Types of dedicated channel events.
*/
typedef enum
{
  WMS_DC_EVENT_INCOMING   = 0,  /**< Incoming call. */
  WMS_DC_EVENT_CONNECTED,       /**< Dedicated channel has been connected. */
  WMS_DC_EVENT_ABORTED,         /**< Transmission was aborted. */
  WMS_DC_EVENT_DISCONNECTED,    /**< Dedicated channel has been disconnected. */
  WMS_DC_EVENT_CONNECTING,      /**< Dedicated channel is connecting. */
  WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT,  /**< Auto-disconnect timer has been set. */
  WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT, /**< Auto-disconnect timer has been 
                                             disabled. */

/** @cond */
  WMS_DC_EVENT_MAX,
  WMS_DC_EVENT_MAX32 = 0x10000000
/** @endcond */
} wms_dc_event_e_type;


/** @ingroup dc_group_data_types 
  Dedicated channel event information.
*/
typedef struct wms_dc_event_info_s
{
  wms_dc_so_e_type    so;               /**< Service option. */
  uint32              idle_timeout;     /**< Idle timeout value. */
} wms_dc_event_info_s_type;


/** @ingroup dc_group_wms_dc_event_cb_type 
  Dedicated channel event callback function.

  @datatypes
  #wms_dc_event_e_type \n
  #wms_dc_event_info_s

  @param[in]  event     Dedicated channel event type.
  @param[in]  info_ptr  Dedicated channel event information.

  @return
  None.

  @dependencies
  None.
  @newpage
*/
typedef void (*wms_dc_event_cb_type)
(
  wms_dc_event_e_type         event,
  wms_dc_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER */
/*~ CALLBACK wms_dc_event_cb_type 
    ONERROR return  */


/** @ingroup dc_group_data_types 
  Dedicated channel disconnect cause type.
*/
typedef enum
{
   WMS_DC_DISCONNECT_CAUSE_NORMAL,           /**< Normal. */
   WMS_DC_DISCONNECT_CAUSE_CALL_IN_PROGRESS, /**< Other call is in progress. */
/** @cond */
   WMS_DC_DISCONNECT_CAUSE_MAX,
   WMS_DC_DISCONNECT_CAUSE_MAX32 = 0x10000000
/** @endcond */
} wms_dc_disconnect_cause_e_type;


/** @ingroup dc_group_data_types 
  Dedicated channel disconnect information type.
*/
typedef struct 
{
  wms_dc_so_e_type                so;           /**< Service option type. */
  uint32                          idle_timeout; /**< Idle timout in milliseconds. */
  wms_dc_disconnect_cause_e_type  disconnect_cause; /**< Disconnect cause. */
} wms_dc_disconnect_info_s_type;   


/** @ingroup dc_group_wms_dc_disconnect_cb_type 
  Dedicated channel disconnect callback function.

  @datatypes
  #wms_dc_disconnect_info_s_type

  @param[in]  disconnect_info_ptr  Disconnect information.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_dc_disconnect_cb_type)
(
   wms_dc_disconnect_info_s_type    *disconnect_info_ptr
);
/*~ PARAM IN disconnect_info_ptr POINTER */
/*~ CALLBACK wms_dc_disconnect_cb_type
    ONERROR return  */


/* ====================================================================== */
/* ----------------- Multimode Broadcast definitions: ------------------- */
/* ====================================================================== */

/** @ingroup mmbroadcast_group_constants 
  Maximum size of the Multimode Broadcast configuration table. */
#define WMS_BC_MM_TABLE_MAX        190


/* ---------- Data definitions ------------ */
/** @addtogroup mmbroadcast_group_data_types 
@{ */
/** Carrier's multimode broadcast configuration.
*/
typedef enum
{
  WMS_BC_CONFIG_DISALLOW,    /**< Disallow the carrier's configuration table. */
  WMS_BC_CONFIG_ALLOW_TABLE, /**< Allows the carrier's configuration table. */
  WMS_BC_CONFIG_ALLOW_ALL,   /**< Allows all configurations. CDMA only. */
/** @cond */
  WMS_BC_CONFIG_MAX32 = 0x10000000
/** @endcond */
} wms_bc_config_e_type;


/** User preferences for multimode broadcast.
*/
typedef enum
{
  WMS_BC_PREF_DEACTIVATE,     /**< Deactivate user language preferences. */
  WMS_BC_PREF_ACTIVATE_TABLE, /**< For GSM/WCDMA CB, use language preferences. */
  WMS_BC_PREF_ACTIVATE_ALL,   /**< For GSM/WCDMA CB, ignore language preferences. */
/** @cond */
  WMS_BC_PREF_MAX,
  WMS_BC_PREF_MAX32 = 0x10000000
/** @endcond */
} wms_bc_pref_e_type;
/** @} */ /* end_addtogroup mmbroadcast_group_data_types */


/** @addtogroup mmbroadcast_group_data_types 
@{ */
/** Range of services for multimode broadcast. 
*/
typedef struct wms_gw_cb_srv_range_s
{
  /* 'from' is <= 'to' */
  wms_gw_cb_srv_id_type    from; /**< Lower end of range of services. */
  wms_gw_cb_srv_id_type    to;   /**< Upper end of range of services. @newpagetable */
} wms_gw_cb_srv_range_s_type;


/** Service ID for multimode broadcast.
*/
typedef struct wms_bc_mm_srv_id_u
{
  wms_bc_service_id_s_type       bc_srv_id;
    /**< Multimode broadcast service ID; see #wms_bc_service_id_s for 
         details. */
  wms_gw_cb_srv_range_s_type     gw_cb_range;
    /**< Range of services for GSM/WCDMA; see #wms_gw_cb_srv_range_s for 
         details. */
} wms_bc_mm_srv_id_type;


/** GSM/WCDMA CB Decoded Page Data for multimode broadcast.
*/
typedef struct wms_gw_cb_ts_data_s
{
  wms_gw_cb_page_header_s_type     cb_page_hdr;
    /**< Page header; see #wms_gw_cb_page_header_s for details. */
  wms_gw_user_data_s_type          user_data;
    /**< User data; see #wms_gw_user_data_s for details. */
} wms_gw_cb_ts_data_s_type;


/** Multimode broadcast service table entry.
*/
typedef struct wms_bc_mm_service_info_s
{
  wms_bc_mm_srv_id_type           srv_id;   
    /**< Service ID for multimode broadcast. */
  boolean                         selected; 
    /**< Indicates whether this entry is selected. */
  wms_priority_e_type             priority; 
    /**< Priority for sending a broadcast message. For GSM/WCDMA CB, it is 
         always Normal. */
  char                            label[WMS_BC_MM_SRV_LABEL_SIZE];
    /**< Service label. */
  wms_user_data_encoding_e_type   label_encoding;
    /**< Encoding type for the service label. If it is ASCII or IA5, each byte 
         of the label has one character. Otherwise, the label has the raw bytes 
         to be decoded by the clients. */
  wms_bc_alert_e_type             alert;  
    /**< Alert options for this broadcast service. */
  uint8                            max_messages; 
    /**< Maximum number of messages that can be stored for this service. */
} wms_bc_mm_service_info_s_type;


/** Multimode broadcast service table.
*/
typedef struct wms_bc_mm_table_s
{
  uint16                           size;
    /**< Size of the table. */
  wms_bc_mm_service_info_s_type    * entries;
    /**< Table entries; see #wms_bc_mm_service_info_s for details. */
} wms_bc_mm_table_s_type;
/*~ FIELD wms_bc_mm_table_s.entries VARRAY LENGTH wms_bc_mm_table_s.size */


/** Service ID entry for multimode broadcast.
*/
typedef struct wms_bc_mm_srv_id_entry_s
{
  wms_bc_service_id_s_type       bc_srv_id;
  /**< Multimode broadcast service ID; see #wms_bc_service_id_s for details. */
  wms_gw_cb_srv_range_s_type     gw_cb_range;
  /**< Range of services for GSM/WCDMA; see #wms_gw_cb_srv_range_s for 
       details. */
  boolean                        selected;
  /**< Indicates whether this entry is selected. */
} wms_bc_mm_srv_id_s_type;


/** Array of multimode broadcast service IDs.
*/
typedef struct wms_bc_mm_service_ids_s
{
  uint16                       size;
  /**< Size of the array of service IDs. */
  wms_bc_mm_srv_id_s_type    * entries;
  /**< Array entries; see #wms_bc_mm_srv_id_entry_s for details. */
} wms_bc_mm_service_ids_s_type;
/*~ FIELD wms_bc_mm_service_ids_s.entries VARRAY LENGTH wms_bc_mm_service_ids_s.size */


/** Reading preferences for multimode broadcast.
*/
typedef struct wms_bc_mm_reading_pref_s
{
  boolean                      reading_advised;   /**< Indicates whether reading 
                                                       is advised. */
  boolean                      reading_optional;  /**< Indicates whether reading 
                                                       is optional. */
} wms_bc_mm_reading_pref_s_type;
/** @} */ /* end_addtogroup mmbroadcast_group_data_types */


/** @ingroup mmbroadcast_group_data_types 
  Multimode broadcast events.
*/
typedef enum
{
  WMS_BC_MM_EVENT_CONFIG,      /**< Sets the multimode broadcast configuration. */
  WMS_BC_MM_EVENT_PREF,        /**< Sets the user preferences. */
  WMS_BC_MM_EVENT_TABLE,       /**< Modifies the multimode broadcast service 
                                    table.*/
  WMS_BC_MM_EVENT_SRV_IDS,     /**< Modifies the array of multimode broadcast 
                                    service IDs. */
  WMS_BC_MM_EVENT_SRV_INFO,    /**< Gets the service ID information. */
  WMS_BC_MM_EVENT_SRV_UPDATED, /**< Updates the service ID. */
  WMS_BC_MM_EVENT_ADD_SRVS,    /**< Adds services to the service ID array. */
  WMS_BC_MM_EVENT_DELETE_SRVS, /**< Deletes services from the service ID array. */
  WMS_BC_MM_EVENT_SELECT_ALL_SRVS,       /**< Selects all service IDs. */
  WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS, /**< Sets the priority of all services. */
  WMS_BC_MM_EVENT_READING_PREF,    /**< Sets the reading preference. This 
                                        command executed successfully. */
  WMS_BC_MM_EVENT_DELETE_ALL_SRVS, /**< Deletes all services. */
  WMS_BC_MM_EVENT_ENABLE_FAILURE,  /**< Enables a lower layer (CM and CP) 
                                        failure. */
  WMS_BC_MM_EVENT_DISABLE_FAILURE, /**< Disables a lower layer (CM and CP) 
                                        failure. */
  WMS_BC_MM_EVENT_SCPT_ADD_SRVS,   /**< SCPT adds services to the service ID 
                                        array. */
  WMS_BC_MM_EVENT_SCPT_DELETE_SRVS,     /**< SCPT deletes services from the 
                                             service ID array. */
  WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS, /**< SCPT deletes all services. */   
                            
/** @cond */
  WMS_BC_MM_EVENT_MAX,
  WMS_BC_MM_EVENT_MAX32 = 0x10000000
/** @endcond */
} wms_bc_mm_event_e_type;


/** @ingroup mmbroadcast_group_data_types 
  Information about multimode broadcast events.
*/
typedef struct wms_bc_mm_event_info_s
{
 wms_message_mode_e_type         message_mode; 
   /**< System mode to be used for a message. */
 wms_bc_mm_event_e_type          event;        
   /**< Multimode broadcast event. */
 void                           *user_data;        
   /**< Pointer to user data. */


 /** Used by the #wms_bc_mm_event_info_s structure.
 */
 union wms_bc_mm_event_info_u
 {
  wms_bc_config_e_type            bc_config;   
    /**< Multimode broadcast configuration. */
    /* WMS_BC_MM_EVENT_CONFIG */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_CONFIG) wms_bc_mm_event_info_u.bc_config */

  wms_bc_pref_e_type              bc_pref;     
    /**< User preferences for multimode broadcast. */
    /* WMS_BC_MM_EVENT_PREF */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_PREF) wms_bc_mm_event_info_u.bc_pref */

  wms_bc_mm_table_s_type          bc_mm_table; 
    /**< Multimode broadcast service table; see #wms_bc_mm_table_s for 
         details. */
    /* WMS_BC_MM_EVENT_TABLE */
    /* WMS_BC_MM_EVENT_ADD_SRVS */
    /* WMS_BC_MM_EVENT_SELECT_ALL_SRVS */
    /* WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS */
  /*~ IF ( _DISC_ == WMS_BC_MM_EVENT_TABLE || _DISC_ == WMS_BC_MM_EVENT_ADD_SRVS 
          || _DISC_ == WMS_BC_MM_EVENT_SELECT_ALL_SRVS || _DISC_ == WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS ) 
      wms_bc_mm_event_info_u.bc_mm_table */

  wms_bc_mm_service_ids_s_type    bc_mm_srv_ids; 
    /**< Array of multimode broadcast service IDs; see #wms_bc_mm_service_ids_s 
         for details. */
    /* WMS_BC_MM_EVENT_SRV_IDS */
    /* WMS_BC_MM_EVENT_DELETE_SRVS */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_SRV_IDS || _DISC_ == WMS_BC_MM_EVENT_DELETE_SRVS)
      wms_bc_mm_event_info_u.bc_mm_srv_ids */

  wms_bc_mm_service_info_s_type   bc_mm_srv_info; 
    /**< Multimode broadcast service table entry; see #wms_bc_mm_service_info_s 
         for details. */
    /* WMS_BC_MM_EVENT_SRV_INFO */
    /* WMS_BC_MM_EVENT_SRV_UPDATED */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_SRV_INFO || _DISC_ == WMS_BC_MM_EVENT_SRV_UPDATED)
      wms_bc_mm_event_info_u.bc_mm_srv_info */

  /* no event data for WMS_BC_MM_EVENT_ALL_SRV_DELETED */

  wms_bc_mm_reading_pref_s_type   bc_mm_reading_pref; 
    /**< Reading preferences for multimode broadcast; see 
         #wms_bc_mm_reading_pref_s for details. */
    /* WMS_BC_MM_EVENT_READING_PREF */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_READING_PREF) wms_bc_mm_event_info_u.bc_mm_reading_pref */

  wms_status_e_type               bc_enable_error;  
    /**< Command or request status messages. @newpagetable */
    /* WMS_BC_MM_EVENT_ENABLE_FAILURE */
    /* WMS_BC_MM_EVENT_DISABLE_FAILURE */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_ENABLE_FAILURE || _DISC_ == WMS_BC_MM_EVENT_DISABLE_FAILURE)
      wms_bc_mm_event_info_u.bc_enable_error */

  /*~ DEFAULT wms_bc_mm_event_info_u.void */

 } u; /**< Used by the #wms_bc_mm_event_info_s structure. */
  /*~ FIELD wms_bc_mm_event_info_s.u DISC _OBJ_.event */

 sys_modem_as_id_e_type           as_id;
   /**< Active subscription ID. */
} wms_bc_mm_event_info_s_type;


/** @ingroup mmbroadcast_group_wms_bc_mm_event_cb_type 
  Multimode broadcast event callback function.

  @datatypes
  #wms_bc_mm_event_e_type \n
  #wms_bc_mm_event_info_s

  @param[in]  event     Multimode broadcast event type.
  @param[in]  info_ptr  Multimode broadcast event information.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_bc_mm_event_cb_type)
(
  wms_bc_mm_event_e_type         event,
  wms_bc_mm_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER */
/*~ CALLBACK wms_bc_mm_event_cb_type 
    ONERROR return  */


/* <EJECT> */
/* ========================================================================= */
/* ============================= Debug group data types ==================== */
/* ========================================================================= */

/** @ingroup debug_group_data_types
  Debug events.
*/
typedef enum
{
  WMS_DBG_EVENT_RESET_TL_SEQ_NUM,       /**< Resets the Transport Layer 
                                             sequence number. */
  WMS_DBG_EVENT_SET_SIM_MESSAGE_NUMBER, /**< Sets the SIM message number. */
  WMS_DBG_EVENT_CLEAR_SMMA_FLAG,        /**< Clears the short message memory 
                                             available flag. */
  WMS_DBG_EVENT_RETRY_INTERVAL,         /**< Retry interval has been retrieved 
                                             or changed. */
  WMS_DBG_EVENT_RETRY_PERIOD,           /**< Retry period has been retrieved 
                                             or changed. */
/** @cond */
  WMS_DBG_EVENT_MAX,
  WMS_DBG_EVENT_MAX32 = 0x10000000
/** @endcond */
} wms_dbg_event_e_type;


/** @ingroup debug_group_data_types
  Information about a debug event.
*/
typedef union wms_dbg_event_info_u
{
  uint32         retry_interval;   /**< Value is provided with 
                                        WMS_DBG_EVENT_RETRY_INTERVAL event. */

/*~ CASE WMS_DBG_EVENT_RETRY_INTERVAL wms_dbg_event_info_u.retry_interval */
  
  uint32         retry_period;     /**< Value is provided with 
                                        WMS_DBG_EVENT_RETRY_PERIOD event. */
                                        
/*~ CASE WMS_DBG_EVENT_RETRY_PERIOD   wms_dbg_event_info_u.retry_period   */

/*~ DEFAULT wms_dbg_event_info_u.void */
} wms_dbg_event_info_s_type/** @newpage */;


/** @ingroup debug_group_wms_dbg_event_cb_type
  Debug event callback function.

  @datatypes
  #wms_dbg_event_e_type \n
  #wms_dbg_event_info_u

  @param[in]  event     Debug event type.
  @param[in]  info_ptr  Debug event information.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*wms_dbg_event_cb_type)
(
  wms_dbg_event_e_type         event,
  wms_dbg_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK wms_dbg_event_cb_type 
    ONERROR return  */



/* <EJECT> */
/* ========================================================================= */
/* ======================= Translation group data types ==================== */
/* ========================================================================= */

/** @addtogroup translation_group_data_types 
@{ */
/** GSM/WCDMA point-to-point Transport Service (TS) data decoded from 
  the raw TS data.
*/
typedef struct wms_gw_pp_ts_data_s
{
  wms_gw_tpdu_type_e_type                tpdu_type; 
    /**< GSM/WCDMA-specific TPDU type. This is not equivalent to TP Message Type 
         Indicator (TP-MTI). */

  /** Used by the #wms_gw_pp_ts_data_s structure.
  */
  union wms_gw_pp_ts_data_u
  {
    wms_gw_deliver_s_type                deliver; 
      /**< Deliver TPDU for GSM/WCDMA messages; see #wms_gw_deliver_s for 
           details. */
    /*~ CASE WMS_TPDU_DELIVER wms_gw_pp_ts_data_u.deliver */

    wms_gw_deliver_report_ack_s_type     deliver_report_ack; 
      /**< Deliver report with acknowledgment TPDU; see 
           #wms_gw_deliver_report_ack_s for details. */
    /*~ CASE WMS_TPDU_DELIVER_REPORT_ACK wms_gw_pp_ts_data_u.deliver_report_ack */

    wms_gw_deliver_report_error_s_type   deliver_report_error;
      /**< Deliver report with error TPDU; see 
           #wms_gw_deliver_report_error_s for details. */
    /*~ CASE WMS_TPDU_DELIVER_REPORT_ERROR wms_gw_pp_ts_data_u.deliver_report_error */

    wms_gw_submit_s_type                 submit;     
      /**< Submit TPDU definition; see #wms_gw_submit_s for details. */
    /*~ CASE WMS_TPDU_SUBMIT wms_gw_pp_ts_data_u.submit */

    wms_gw_submit_report_ack_s_type      submit_report_ack;
      /**< Submit report with acknowledgment TPDU; see 
           #wms_gw_submit_report_ack_s for details. */
    /*~ CASE WMS_TPDU_SUBMIT_REPORT_ACK wms_gw_pp_ts_data_u.submit_report_ack */

    wms_gw_submit_report_error_s_type    submit_report_error; 
      /**< Submit report with error TPDU; see #wms_gw_submit_report_error_s 
           for details.*/
    /*~ CASE WMS_TPDU_SUBMIT_REPORT_ERROR wms_gw_pp_ts_data_u.submit_report_error */

    wms_gw_status_report_s_type          status_report; 
      /**< Status report TPDU; see #wms_gw_status_report_s for details. */
    /*~ CASE WMS_TPDU_STATUS_REPORT wms_gw_pp_ts_data_u.status_report*/

    wms_gw_command_s_type                command;
      /**< Command TPDU; see #wms_gw_command_s for details. */
    /*~ CASE WMS_TPDU_COMMAND wms_gw_pp_ts_data_u.command */

    /*~ DEFAULT wms_gw_pp_ts_data_u.void */

  } u; /**< Used by the #wms_gw_pp_ts_data_s structure. */
  /*~ FIELD wms_gw_pp_ts_data_s.u DISC _OBJ_.tpdu_type */
} wms_gw_pp_ts_data_s_type;


/** Client TS data decoded from the raw TS data.
*/
typedef struct wms_client_ts_data_s
{
  wms_format_e_type             format; /**< Transport Service data format. */

  /** Used by the #wms_client_ts_data_s structure.
  */
  union wms_client_ts_data_u
  {
    wms_client_bd_s_type        cdma;
      /**< CDMA for both point-to-point and broadcast; see #wms_client_bd_s for 
           details. */
    /*~ CASE WMS_FORMAT_CDMA wms_client_ts_data_u.cdma */

    wms_gw_pp_ts_data_s_type    gw_pp;
      /**< GSM/WCDMA point-to-point; see #wms_gw_pp_ts_data_s for details. */
    /*~ CASE WMS_FORMAT_GW_PP wms_client_ts_data_u.gw_pp */

    wms_gw_cb_ts_data_s_type    gw_cb;
      /**< GSM/WCDMA cell broadcast; see #wms_gw_cb_ts_data_s for details. */
    /*~ CASE WMS_FORMAT_GW_CB wms_client_ts_data_u.gw_cb */

    /*~ DEFAULT wms_client_ts_data_u.void */

  } u; /**< Used by the #wms_client_ts_data_s structure. */
  /*~ FIELD wms_client_ts_data_s.u DISC _OBJ_.format */
} wms_client_ts_data_s_type;

/** @} */ /* end_addtogroup translation_group_data_types */


/* <EJECT> */
/*===========================================================================

                          Client Group

                    API FUNCTION DEFINITIONS

===========================================================================*/
/*=========================================================================
FUNCTION wms_client_init
===========================================================================*/
/** @ingroup client_group_wms_client_init
  Initializes a client so the API can allocate resources for the client.

  @datatypes
  #wms_client_type_e_type

  @param[in] client_type      Client type.
  @param[out] client_id_ptr   Pointer to the returned client ID.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  None.

  @sideeffects
  Internal data is initialized for the new client.
*/
wms_client_err_e_type wms_client_init
(
  wms_client_type_e_type   client_type,
  wms_client_id_type       *client_id_ptr
);
/*~ FUNCTION wms_client_init
    RELEASE_FUNC wms_client_release(*client_id_ptr)
    ONERROR return WMS_CLIENT_ERR_RPC */
/*~ PARAM OUT client_id_ptr POINTER */

/*=========================================================================
FUNCTION  wms_client_release
===========================================================================*/
/** @ingroup client_group_wms_client_release
  Releases a client so the API can release the client's resources.

  @param[in] client_id        Client ID.

  @return
  Client error code.   
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  None.

  @sideeffects
  Internal data is reset for this client.
*/
wms_client_err_e_type wms_client_release
(
  wms_client_id_type       client_id
);
/*~ FUNCTION wms_client_release 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_activate
===========================================================================*/
/** @ingroup client_group_wms_client_activate
  Activates a client so the client can start using services from the API.

  @param[in] client_id        Client ID.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  None.

  @sideeffects
  Internal data is changed.
*/
wms_client_err_e_type wms_client_activate
(
  wms_client_id_type       client_id
);
/*~ FUNCTION wms_client_activate 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_deactivate
===========================================================================*/
/** @ingroup client_group_wms_client_deactivate
  Deactivates a client so the client can stop using services from the API.

  @param[in] client_id        Client ID.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  None.

  @sideeffects
  Internal data is changed.
*/
wms_client_err_e_type wms_client_deactivate
(
  wms_client_id_type       client_id
);
/*~ FUNCTION wms_client_deactivate 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_set_cdma_channel_pref
===========================================================================*/
/**
  Set client's channel preference for sending MO SMS.

  @param client_id    [IN]  Client ID.
  @param ch_pref      [IN]  Channel pref.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_CH_PREF

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is changed.
*/
wms_client_err_e_type wms_client_set_cdma_channel_pref
(
  wms_client_id_type           client_id,
  wms_client_ch_pref_e_type    ch_pref
);
/*~ FUNCTION wms_client_set_cdma_channel_pref
RELEASE FUNC wms_client_deactivate(client_id)
ERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_get_cdma_channel_pref
===========================================================================*/
/**
  Get client's MO SMS channel preference.

  @param client_id    [IN]  Client ID.
  @param * ch_pref    [OUT] Channel pref.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_CH_PREF_PTR

  @dependencies
  Client must have been initialized.

  @sideeffects
*/
wms_client_err_e_type wms_client_get_cdma_channel_pref
(
  wms_client_id_type           client_id,
  wms_client_ch_pref_e_type    *ch_pref
);
/*~ FUNCTION wms_client_get_cdma_channel_pref
RELEASE FUNC wms_client_deactivate(client_id)
ERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_set_cdma_service_option_pref
===========================================================================*/
/**
  Set client's service option for sending MO SMS.

  @param client_id    [IN]  Client ID.
  @param dc_so        [IN]  Service option.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_SO_PREF

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is changed.
*/
wms_client_err_e_type wms_client_set_cdma_service_option_pref
(
  wms_client_id_type           client_id,
  wms_dc_so_e_type             dc_so
);
/*~ FUNCTION wms_client_set_cdma_service_option_pref
RELEASE FUNC wms_client_deactivate(client_id)
ERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_get_cdma_service_option_pref
===========================================================================*/
/**
  Get client's MO SMS service option preference.

  @param client_id    [IN]  Client ID.
  @param * dc_so      [OUT] Service Option.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_SO_PREF_PTR

  @dependencies
  Client must have been initialized.

  @sideeffects
*/
wms_client_err_e_type wms_client_get_cdma_service_option_pref
(
  wms_client_id_type           client_id,
  wms_dc_so_e_type             *dc_so
);
/*~ FUNCTION wms_client_get_cdma_service_option_pref
RELEASE FUNC wms_client_deactivate(client_id)
ERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_set_cdma_service_follow_on_dc_pref
===========================================================================*/
/**
  Set client's service option for sending MO SMS.

  @param client_id    [IN]  Client ID.
  @param follow_on_dc [IN]  follow_on_dc option.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is changed.
*/
wms_client_err_e_type wms_client_set_cdma_service_follow_on_dc_pref(
  wms_client_id_type           client_id,
  boolean                      follow_on_dc
);
/*~ FUNCTION wms_client_set_cdma_service_follow_on_dc_pref
RELEASE FUNC wms_client_deactivate(client_id)
ERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_get_cdma_service_follow_on_dc_pref
===========================================================================*/
/**
  Get client's MO SMS service option preference.

  @param client_id       [IN]  Client ID.
  @param *follow_on_dc   [OUT] follow_on_dc Option.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID
   - WMS_CLIENT_ERR_FDC_PREF_PTR

  @dependencies
  Client must have been initialized.

  @sideeffects
*/
wms_client_err_e_type wms_client_get_cdma_service_follow_on_dc_pref(
  wms_client_id_type           client_id,
  boolean                     *follow_on_dc
);
/*~ FUNCTION wms_client_get_cdma_service_follow_on_dc_pref
RELEASE FUNC wms_client_deactivate(client_id)
ERROR return WMS_CLIENT_ERR_RPC */


/*=========================================================================
FUNCTION  wms_client_reg_cfg_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_cfg_cb
  Registers/deregisters a client's configuration event callback function.

  If a NULL pointer is passed, the callback is deregistered. Otherwise, it is 
  registered.

  @param[in] client_id        Client ID.
  @param[in] cfg_event_cb     Configuration event callback function pointer; 
                              see #wms_cfg_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_cfg_cb
(
  wms_client_id_type       client_id,
  wms_cfg_event_cb_type    cfg_event_cb
);
/*~ FUNCTION wms_client_reg_cfg_cb 
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_detail_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_detail_cb
  Registers/deregisters a client's configuration event to get WMS status
  information callback function.

  If a NULL pointer is passed, the callback is deregistered. Otherwise, it is 
  registered.

  @param[in] client_id        Client ID.
  @param[in] boolean          req_detail     Whether status details are needed
                                             or not.
                              

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_detail_cb
(
  wms_client_id_type       client_id,
  boolean                  req_detail
);
/*~ FUNCTION wms_client_reg_detail_cb 
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_msg_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_msg_cb
  Registers/deregisters the message event callback function.

  If a NULL pointer is passed, the callback is deregistered. Otherwise, it is 
  registered.

  @param[in] client_id        Client ID.
  @param[in] msg_event_cb     Message event callback function pointer; 
                              see #wms_msg_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_msg_cb
(
  wms_client_id_type       client_id,
  wms_msg_event_cb_type    msg_event_cb
);
/*~ FUNCTION wms_client_reg_msg_cb 
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_transport_status_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_transport_status_cb
  Registers/deregisters a callback function to get the Transport Layer
  status.

  If a NULL pointer is passed, the callback is deregistered. Otherwise, it is 
  registered.

  @param[in] client_id            Client ID.
  @param[in] transport_status_cb  Transport status callback function pointer; 
                                  see #wms_msg_transport_status_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_transport_status_cb
(
  wms_client_id_type                 client_id,
  wms_msg_transport_status_cb_type   transport_status_cb
);

/*~ FUNCTION wms_client_reg_transport_status_cb 
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_reg_msg_info_cache_cb
===========================================================================*/
/** @ingroup client_group_wms_reg_msg_info_cache_cb
  Registers the message information cache callback function for storing the 
  application's message list display information.

  The client can register its callback function so the client can tell the WMS 
  how to cache the message information for the messages from the memory. If a 
  NULL pointer is passed, the callback is deregistered. No message information 
  is cached.

  @note1 Only one cache callback is allowed. No client ID is required.

  @param[in] msg_info_cache_cb  Message information cache callback function 
                                pointer; see #wms_msg_info_cache_cb_type.

  @return
  None.

  @dependencies
  This callback is to be registered in the early stage of the power-up 
  initialization so that WMS can use this callback to cache the information 
  during initialization.

  @sideeffects
  Internal data is updated.
*/
void wms_reg_msg_info_cache_cb
(
  wms_msg_info_cache_cb_type    msg_info_cache_cb
);
/*~ FUNCTION wms_reg_msg_info_cache_cb 
    ONERROR return */

/*=========================================================================
FUNCTION  wms_client_reg_parse_msg_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_parse_msg_cb
  Registers the message parsing/filter callback function.

  The client can register its callback function so when an incoming message 
  arrives the WMS can query this client and determine whether this client is 
  to consume the message.
   - If TRUE, the incoming message is delivered only to this client.
   - If FALSE, the incoming message is delivered to all clients.

  The client order that is defined in wms_client_type_e_type is used for 
  querying all clients.

  @param[in] client_id        Client ID.
  @param[in] msg_parsing_cb   Message parsing callback function pointer; 
                              see #wms_cl_parse_msg_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Depending on the criteria of the parsing being administered to the message, 
  the message cannot be shared among other clients.
*/
wms_client_err_e_type wms_client_reg_parse_msg_cb
(
  wms_client_id_type               client_id,
  wms_cl_parse_msg_cb_type         msg_parsing_cb
);
/*~ FUNCTION wms_client_reg_parse_msg_cb 
    RELEASE_FUNC wms_client_deactivate(client_id)
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_dc_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_dc_cb
  Registers/deregisters the dedicated channel event callback function.
 
  If a NULL pointer is passed, the callback is deregistered. Otherwise, it is 
  registered.

  @param[in] client_id        Client ID.
  @param[in] dc_event_cb      Dedicated channel event callback function 
                              pointer; #wms_dc_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_dc_cb
(
  wms_client_id_type       client_id,
  wms_dc_event_cb_type     dc_event_cb
);
/*~ FUNCTION wms_client_reg_dc_cb 
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_dc_disconnect_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_dc_disconnect_cb
  Registers/deregisters the dedicated channel disconnect callback function. 

  If a NULL pointer is passed, the callback is deregistered.

  @param[in] client_id         Client ID.
  @param[in] dc_disconnect_cb  Dedicated channel disconnect callback function 
                               pointer; see #wms_dc_disconnect_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_dc_disconnect_cb
(
  wms_client_id_type            client_id,
  wms_dc_disconnect_cb_type     dc_disconnect_cb
);

/*~ FUNCTION wms_client_reg_dc_disconnect_cb
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_application_port
===========================================================================*/
/** @ingroup wms_client_reg_application_port
 Allow the client to register with WMS with specific application port number 


  @param[in] client_id          Client ID.
  @param[in] dest_port         corresponding dest_port for filtering
  @param[in] source_port      corresponding source_port for filtering

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/

wms_client_err_e_type wms_client_reg_application_port
(
  wms_client_id_type               client_id,
  uint32                           dest_port,
  uint32                           source_port
);

/*=========================================================================
FUNCTION  wms_client_reg_bc_mm_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_bc_mm_cb
  Registers/deregisters a multimode broadcast event callback function.

  If a NULL pointer is passed, the callback is deregistered. Otherwise, it is 
  registered.

  @param[in] client_id        Client ID.
  @param[in] bc_event_cb      Multimode broadcast event callback function 
                              pointer; see #wms_bc_mm_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_bc_mm_cb
(
  wms_client_id_type          client_id,
  wms_bc_mm_event_cb_type     bc_event_cb
);
/*~ FUNCTION wms_client_reg_bc_mm_cb 
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_dbg_cb
===========================================================================*/
/** @ingroup client_group_wms_client_reg_dbg_cb
  Registers/deregisters the debug (DBG) event callback function.

  If a NULL pointer is passed, the callback is deregistered. Otherwise, it is 
  registered.

  @param[in] client_id        Client ID.
  @param[in] dbg_event_cb     Debug event callback function pointer; 
                              see #wms_dbg_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  Client must have been initialized.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_reg_dbg_cb
(
  wms_client_id_type       client_id,
  wms_dbg_event_cb_type    dbg_event_cb
);
/*~ FUNCTION wms_client_reg_dbg_cb 
    RELEASE_FUNC wms_client_deactivate(client_id) 
    ONERROR return WMS_CLIENT_ERR_RPC */

/*===========================================================================
FUNCTION_new wms_client_activate_bearer
===========================================================================*/
/** @ingroup client_group_wms_client_activate_bearer
  Activates a specified bearer.

  @datatypes
  #wms_bearer_e_type

  @param[in] client_id        Client ID.
  @param[in] bearer           Specified bearer.

  @return
  Client error code. 
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  None.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_activate_bearer
(
  wms_client_id_type                client_id,
  wms_bearer_e_type                 bearer
);
/*~ FUNCTION wms_client_activate_bearer
    ONERROR return WMS_CLIENT_ERR_RPC */

/*===========================================================================
FUNCTION_new wms_client_deactivate_bearer
===========================================================================*/
/** @ingroup client_group_wms_client_deactivate_bearer
  Deactivates a specified bearer. 

  @datatypes
  #wms_bearer_e_type

  @param[in] client_id        Client ID.
  @param[in] bearer           Specified bearer.

  @return
  Client error code.
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_CLIENT_ID

  @dependencies
  None.

  @sideeffects
  Internal data is updated.
*/
wms_client_err_e_type wms_client_deactivate_bearer
(
  wms_client_id_type                client_id,
  wms_bearer_e_type                 bearer
);
/*~ FUNCTION wms_client_deactivate_bearer 
    ONERROR return WMS_CLIENT_ERR_RPC */


/* <EJECT> */
/*===========================================================================

                          Configuration Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION wms_cfg_set_lte_domain_pref

DESCRIPTION
  Allow the client to set LTE Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_set_lte_domain_pref(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  uint8                            lte_domain_pref
);
/*~ FUNCTION wms_cfg_set_lte_domain_pref 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_cfg_locate_mo_msg_for_sts_report
===========================================================================*/
/** @ingroup config_group_wms_cfg_locate_mo_msg_for_sts_report
  Returns the index for an MO SMS message based on the message 
  reference number in the received status report message.

  The function locates the MO message index given a message reference number 
  for that message on the SIM.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_message_index_type \n
  #wms_message_number_type

  @param[out] rec_id          Pointer to the return result.
  @param[in] message_ref      Reference number for the MO message.

  @return
  TRUE -- Message index was found. \n 
  FALSE -- Message index was not found.

  @dependencies
  Client must have been initialized.
*/
boolean wms_cfg_locate_mo_msg_for_sts_report
(
  wms_message_index_type  *rec_id,      /* OUTPUT */
  wms_message_number_type  message_ref  /* INPUT */
);
/*~ FUNCTION wms_cfg_locate_mo_msg_for_sts_report 
    ONERROR return FALSE */
/*~ PARAM OUT rec_id POINTER */

/*=========================================================================
FUNCTION  wms_cfg_get_sts_rpt_index
===========================================================================*/
/** @ingroup config_group_wms_cfg_get_sts_rpt_index
  Returns the status report's index for an MO SMS message given 
  its index on the SIM.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_message_index_type

  @param[in] sms_index        MO SMS message index.
  @param[out] smsr_index      Pointer to the result.

  @return
  TRUE -- Message index was found. \n 
  FALSE -- Message index was not found.

  @dependencies
  Client must have been initialized.
*/
boolean wms_cfg_get_sts_rpt_index
(
  wms_message_index_type sms_index,   /* INPUT */
  wms_message_index_type *smsr_index  /* OUTPUT */
);
/*~ FUNCTION wms_cfg_get_sts_rpt_index 
    ONERROR return FALSE */
/*~ PARAM OUT smsr_index POINTER */ 

/*=========================================================================
FUNCTION  wms_cfg_check_voicemail_contents
===========================================================================*/
/** @ingroup config_group_wms_cfg_check_voicemail_contents
  Examines the content of a CDMA or GSM SMS message and determines whether the 
  message has voice mail information.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_raw_ts_data_s

  @param[in] msg_mode         CDMA or GSM/WCDMA message mode.
  @param[in] message          Pointer to the raw bearer data or TPDU.
  @param[out] is_voicemail_active  Indicates whether to voice mail status is 
                                   active.
  @param[out] count                How many voice mails are waiting.

  @return
  TRUE -- Message is a voice mail message. \n 
  FALSE -- Message is not a voice mail message.

  @dependencies
  None.

  @sideeffects
  Updates is_voicemail_active and count.
*/
boolean wms_cfg_check_voicemail_contents
(
  wms_message_mode_e_type         msg_mode,             /* INPUT */
  const wms_raw_ts_data_s_type    *message,             /* INPUT */
  boolean                         *is_voicemail_active, /* OUTPUT */
  uint8                           *count  /* OUTPUT: how many voice mails */
);
/*~ FUNCTION wms_cfg_check_voicemail_contents 
    ONERROR return FALSE */
/*~ PARAM IN message POINTER */
/*~ PARAM OUT is_voicemail_active POINTER */
/*~ PARAM OUT count POINTER */ 

/*=========================================================================
FUNCTION  wms_cfg_check_mwi_contents
===========================================================================*/
/** @ingroup config_group_wms_cfg_check_mwi_contents
  Checks an SMS message for message waiting indication contents.
  
  The last parameter indicates the number of message waiting types in the
  SMS message. The caller can find out if the array is big enough to hold
  all the message waiting indication information by comparing the size of
  the passed-in array and the required array size.
 
  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_raw_ts_data_s \n
  #wms_gw_mwi_info_s_type

  @param[in] message         Pointer to the raw bearer data or TPDU.
  @param[out] mwi_info_arr   Pointer to the Message Waiting Indicator 
                             (MWI) information array.
  @param[in] mwi_info_arr_size       MWI information array size.
  @param[out] mwi_info_arr_size_req  Pointer to the MWI information array size.

  @return
  TRUE -- Message contains MWI information. \n 
  FALSE -- Message does not contain MWI information, and mwi_info_arr_size_req 
           is set to 0.

  @dependencies
  Undetermined.

  @sideeffects
  Updates *mwi_info_arr and *mwi_info_arr_size_req if they are not NULL.
*/
boolean wms_cfg_check_mwi_contents 
( 
  const wms_raw_ts_data_s_type  *message,              
  wms_gw_mwi_info_s_type        *mwi_info_arr,         
  int                            mwi_info_arr_size,        
  int                           *mwi_info_arr_size_req 
);
/*~ FUNCTION wms_cfg_check_mwi_contents 
    ONERROR return FALSE */
/*~ PARAM IN message POINTER */
/*~ PARAM OUT mwi_info_arr POINTER */
/*~ PARAM IN mwi_info_arr_size */
/*~ PARAM OUT mwi_info_arr_size_req POINTER */ 

/*===========================================================================
FUNCTION wms_cfg_check_cphs_msg
===========================================================================*/
/** @ingroup config_group_wms_cfg_check_cphs_msg
  Examines the address field of a GSM SMS message and determines whether it is 
  a CPHS Message Waiting message. The function uses TP-OA for a voice message 
  waiting indicator.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_address_s_type

  @param[in] addr             Pointer to the address structure.

  @return
  TRUE -- Message is a CPHS Message Waiting message. \n 
  FALSE -- Message is not a CPHS Message Waiting message.

  @dependencies
  The CPHS feature must be enabled.
*/
boolean wms_cfg_check_cphs_msg
(
   const wms_address_s_type     *addr
);
/*~ FUNCTION wms_cfg_check_cphs_msg 
    ONERROR return FALSE */
/*~ PARAM IN addr POINTER */ 

/*=========================================================================
FUNCTION  wms_cfg_check_cdma_duplicate
===========================================================================*/
/** @ingroup config_group_wms_cfg_check_cdma_duplicate
  Examines the content of a CDMA SMS message and determines whether it is a 
  duplicate by comparing messages in R-UIM and NV.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_client_message_s

  @param[in] msg_ptr          Pointer to the client message structure.

  @return
  TRUE -- Message is a duplicate. \n 
  FALSE -- Message is not a duplicate.

  @dependencies
  None.
*/
boolean wms_cfg_check_cdma_duplicate
(
  const wms_client_message_s_type     *msg_ptr
);
/*~ FUNCTION wms_cfg_check_cdma_duplicate 
    ONERROR return FALSE */
/*~ PARAM IN msg_ptr POINTER */

/*===========================================================================
FUNCTION  wms_cfg_locate_voicemail_line2
===========================================================================*/
/** @ingroup config_group_wms_cfg_locate_voicemail_line2
  Returns the index of the voice mail message in the SIM for subscription 
  line 2 for the CPHS Alternative Line feature.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_message_index_type
 
  @param[out] sms_index       Pointer to the returned index.

  @return
  TRUE -- Voice mail is active on line 2. \n 
  FALSE -- Voice mail is not active on line 2.

  @dependencies
  The CPHS feature must be enabled.
*/
boolean wms_cfg_locate_voicemail_line2
(
  wms_message_index_type        *sms_index   /* OUTPUT */
);
/*~ FUNCTION wms_cfg_locate_voicemail_line2 
    ONERROR return FALSE */
/*~ PARAM OUT sms_index POINTER */ 

/*=========================================================================
FUNCTION  wms_cfg_check_wap_push_message
===========================================================================*/
/** @ingroup config_group_wms_cfg_check_wap_push_message
  Examines the content of an CDMA/GSM/WCDMA SMS message and determines whether 
  it is a Wireless Application Protocol Push message.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_client_message_s

  @param[in] msg_ptr          Pointer to the message structure.

  @return
  TRUE -- Message is a WAP Push message. \n 
  FALSE -- Message is not a WAP Push message.

  @dependencies
  None.
*/
boolean wms_cfg_check_wap_push_message
(
  const wms_client_message_s_type     *msg_ptr
);
/*~ FUNCTION wms_cfg_check_wap_push_message 
    ONERROR return FALSE */
/*~ PARAM IN msg_ptr POINTER */ 

/*=========================================================================
FUNCTION  wms_cfg_send_sim_efs_read_event_to_pbm
===========================================================================*/
/**
  API exposed for PBM to post command to WMS. WMS returns the command response 
  asynchronously whenever WMS completes its SIM EFS file reading part
  Till then keep the last posted command (transaction_id) with itself.
  Transcation_id is sent in response for mapping 
  If WMS has already read its EFS part, it donot wait and send the response 
  immediately

  @note1 This is an asynchronous function call. No client ID is required.

  @param send_read_evt_param     [IN]  Structure containing 
                                       transaction id and session_type of request.

  @return
  Status of the request.
   - WMS_OK_S

  @dependencies
  None.
*/
wms_status_e_type wms_cfg_send_sim_efs_read_event_to_pbm
(
  wms_cfg_efs_read_evt_s_type   send_read_evt_param
);
/*~ FUNCTION wms_cfg_send_sim_efs_read_event_to_pbm 
    ONERROR return WMS_OUT_OF_RESOURCES_S */

/* <EJECT> */
/*===========================================================================

                              Message Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION  wms_msg_get_cache_info
===========================================================================*/
/** @ingroup messaging_group_wms_msg_get_cache_info
  Retrieves the message cache information for a message stored in a memory store.

  @note1 This is a synchronous function call. No client ID is required.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type \n
  #wms_message_tag_e_type

  @param[in] mem_store     Memory store from which to read the message.
  @param[in] index         Index of the message.
  @param[in] tag           Type of message, either NONE or TEMPLATE.
  @param[out] cache_ptr    Pointer to the cache buffer in the client space (this 
                           function copies data from the WMS to the client).

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
 
  @dependencies
  Client must have been initialized.
*/
wms_status_e_type wms_msg_get_cache_info
(
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag,
  uint8                           * cache_ptr
);
/*~ FUNCTION  wms_msg_get_cache_info 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM OUT cache_ptr ARRAY WMS_MSG_INFO_CACHE_SIZE */

/*=========================================================================
FUNCTION  wms_msg_set_retry_period
===========================================================================*/
/** @ingroup messaging_group_wms_msg_set_retry_period
  Specifies the SMS message retry period.

  If period = nonzero, the retry period is enabled. If period = 0 seconds, the 
  retry period is disabled.

  When the retry period is enabled and an error is received from the lower 
  layers, the WMS waits for five seconds and sends the message again. However, 
  the retry is not performed under the following conditions:
   - If there are errors from the network (CDMA SMS ACK with cause code, or 
     GSM SMS RP/CP errors)
   - If the message is too large
   - If the message fails the MO control by the SIM card

  @note1 This is a synchronous function call. 

  @param[in] client_id        Client ID.
  @param[in] period           Total retry period in seconds.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S
   - WMS_INVALID_PARM_VALUE_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_set_retry_period
(
  wms_client_id_type              client_id,
  uint32                          period     /* in seconds */
);
/*~ FUNCTION  wms_msg_set_retry_period 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_msg_get_retry_period
===========================================================================*/
/** @ingroup messaging_group_wms_msg_get_retry_period
  Retrieves the current SMS message retry period.

  @param[in] client_id        Client ID.
  @param[out] period          Pointer to the retry period result.
  
  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_get_retry_period
(
  wms_client_id_type              client_id,
  uint32                          * period     /* in seconds */
);
/*~ FUNCTION  wms_msg_get_retry_period 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM OUT period POINTER */

/*=========================================================================
FUNCTION  wms_msg_transport_release
===========================================================================*/
/** @ingroup messaging_group_wms_msg_transport_release
  Allows a client to release its messaging transport callback with WMS.

  @datatypes
  #wms_msg_transport_id_type

  @param[in] transport_id     Transport ID.

  @return
  Status of the request:
   - WMS_OK_S
   - WMS_UNSUPPORTED_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type  wms_msg_transport_release
(
  wms_msg_transport_id_type      transport_id
);

/*~ FUNCTION  wms_msg_transport_release 
    ONERROR return WMS_RPC_ERROR_S */
    
/*=========================================================================
FUNCTION  wms_msg_transport_init
===========================================================================*/
/** @ingroup messaging_group_wms_msg_transport_init
  Allows the SMS Transport Layer to register with WMS to handle MO and MT SMS 
  messages with specified capability.

  @datatypes
  #wms_msg_transport_type_e_type \n
  #wms_msg_transport_cap_type \n
  #wms_msg_transport_id_type
  
  @param[in] transport_type     Transport type.
  @param[in] transport_cap      Transport capability.
  @param[out] transport_id_ptr  Transport ID pointer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type  wms_msg_transport_init
(
  wms_msg_transport_type_e_type   transport_type,
  wms_msg_transport_cap_type      transport_cap,
  wms_msg_transport_id_type      *transport_id_ptr
);
/*~ FUNCTION  wms_msg_transport_init 
    RELEASE_FUNC wms_msg_transport_release(*transport_id_ptr)
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM OUT transport_id_ptr POINTER */

/*=========================================================================
FUNCTION  wms_msg_transport_reg_mo_sms_cb
===========================================================================*/
/** @ingroup messaging_group_wms_msg_transport_reg_mo_sms_cb
  Allows the SMS Transport Layer to register with WMS with a specified function 
  callback to be used to send SMS over the Transport Layer. If a NULL pointer 
  is passed, the callback is deregistered.

  @datatypes
  #wms_msg_transport_id_type  
  
  @param[in] transport_id     Transport ID.
  @param[in] mo_sms_cb        Transport MO SMS callback function pointer; 
                              see #wms_msg_transport_mo_sms_cb_type.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_UNSUPPORTED_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type  wms_msg_transport_reg_mo_sms_cb
(
  wms_msg_transport_id_type         transport_id,
  wms_msg_transport_mo_sms_cb_type  mo_sms_cb
);
/*~ FUNCTION  wms_msg_transport_reg_mo_sms_cb
    RELEASE_FUNC  wms_msg_transport_release(transport_id)
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_msg_transport_nw_reg_status_update
===========================================================================*/
/** @ingroup messaging_group_wms_msg_transport_nw_reg_status_update
  Allows the SMS Transport Layer to notify WMS when its network registration 
  status changes.

  @datatypes
  #wms_msg_transport_id_type \n
  #wms_msg_transport_nw_reg_status_e_type
  
  @param[in] transport_id      Transport ID.
  @param[in] transport_status  Transport status.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type  wms_msg_transport_nw_reg_status_update
(
  wms_msg_transport_id_type               transport_id,
  wms_msg_transport_nw_reg_status_e_type  transport_status
);
/*~ FUNCTION  wms_msg_transport_nw_reg_status_update
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_msg_transport_cap_update
===========================================================================*/
/** @ingroup messaging_group_wms_msg_transport_cap_update
  Allows the SMS Transport Layer to notify WMS when its registered 
  capability changes.

  @datatypes
  #wms_msg_transport_id_type \n
  #wms_msg_transport_cap_type

  @param[in] transport_id     Transport ID.
  @param[in] transport_cap    Transport Cap.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type  wms_msg_transport_cap_update
(
  wms_msg_transport_id_type         transport_id,
  wms_msg_transport_cap_type        transport_cap
);
/*~ FUNCTION  wms_msg_transport_cap_update
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_msg_transport_rpt_ind
===========================================================================*/
/** @ingroup messaging_group_wms_msg_transport_rpt_ind
  Notifies WMS when there is an MT SMS or MO status report.

  @datatypes
  #wms_msg_transport_rpt_ind_s_type

  @param[in] rpt_ind_ptr      Transport Report Indication pointer.

  @return
  Status of the request.  
   - WMS_OK_S
   - WMS_UNSUPPORTED_S
   - WMS_NULL_PTR_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_transport_rpt_ind
(
  wms_msg_transport_rpt_ind_s_type   *rpt_ind_ptr
);

/*~ FUNCTION  wms_msg_transport_rpt_ind 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN rpt_ind_ptr POINTER */

/*=========================================================================
FUNCTION  wms_msg_transport_rpt_mwi_ind
===========================================================================*/
/** @ingroup messaging_group_wms_msg_transport_rpt_mwi_ind
  Notifies WMS when there is MWI information update.

  @datatypes
  #wms_msg_transport_rpt_mwi_ind_s_type

  @param[in] rpt_mwi_ind_ptr      Transport Report MWI Indication pointer.

  @return
  Status of the request.  
   - WMS_OK_S
   - WMS_UNSUPPORTED_S
   - WMS_NULL_PTR_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_transport_rpt_mwi_ind
(
  wms_msg_transport_rpt_mwi_ind_s_type   *rpt_mwi_ind_ptr
);


/*=========================================================================
FUNCTION  wms_msg_put_transport_update_cmd
===========================================================================*/
/** @ingroup wms_msg_put_transport_update_cmd
  Allows the SMS Transport Layer to notify WMS about its 
  status changes and post Indications

  @datatypes
  #wms_msg_transport_id_type \n
  
  @param[in] transport_id     Transport ID.
  @param[in] update_item      Update item type.
  @param[in] item_len         Length of item in byte.
  @param[in] item             Item pointer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_put_transport_update_cmd(
  wms_msg_transport_id_type   transport_id,
  sys_modem_as_id_e_type      as_id,
  uint32                      update_item,
  uint32                      item_len,
  void                       *item
);


/*~ FUNCTION  wms_msg_transport_rpt_mwi_ind 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN rpt_ind_ptr POINTER */

/* <EJECT> */
/*===========================================================================

                         Dedicated Channel Group

                        API FUNCTION DEFINITIONS

===========================================================================*/
/*=========================================================================
FUNCTION  wms_dc_enable_auto_disconnect
===========================================================================*/
/** @ingroup dc_group_wms_dc_enable_auto_disconnect
  Submits a request to automatically disconnect a dedicated channel connection 
  if no SMS message is sent or received during the timeout period as specified 
  by the client.

  The client or the base station can still explicitly disconnect the connection 
  after this feature is enabled. The connection release result is delivered to 
  the client through the event callback function.

  The auto-disconnect feature is disabled by default.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] timeout        Timeout in seconds.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE

  @events
  WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT \n
  WMS_DC_EVENT_DISCONNECT

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_dc_enable_auto_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  uint32                          timeout    /* in seconds */
);
/*~ FUNCTION  wms_dc_enable_auto_disconnect 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dc_disable_auto_disconnect
==========================================================================*/
/** @ingroup dc_group_wms_dc_disable_auto_disconnect
  Submits a request to disable the auto-disconnect feature for a dedicated 
  channel connection.

  The auto-disconnect feature is disabled by default.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE

  @events
  WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT

  @sideeffects
  Request is added to the request queue.
*/

wms_status_e_type wms_dc_disable_auto_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION  wms_dc_disable_auto_disconnect 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dc_connect
==========================================================================*/
/** @ingroup dc_group_wms_dc_connect
  Originates a dedicated channel connection request (CDMA only) to the base 
  station and changes its internal state.

  The connection result is delivered to the client through the event callback 
  function. SMS messages can still be sent or received whether or not there is 
  a DC connection.

  @datatypes
  #wms_dc_so_e_type

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] so             Service option.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_DC_BAD_STATE

  @newpage
  @events
  WMS_DC_EVENT_CONNECT \n
  WMS_DC_EVENT_DISCONNECT

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_dc_connect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_dc_so_e_type                so
);
/*~ FUNCTION  wms_dc_connect 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dc_disconnect
==========================================================================*/
/** @ingroup dc_group_wms_dc_disconnect
  Submits a request from the client or base station to disconnect a dedicated 
  channel connection (CDMA only) and change its internal state.

  An event is issued through the event callback function indicating when the 
  connection is disconnected. SMS messages can still be sent or received 
  regardless of whether there is a DC connection.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE

  @events
  WMS_DC_EVENT_DISCONNECT

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_dc_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_dc_disconnect 
    ONERROR return WMS_RPC_ERROR_S */


/* <EJECT> */
/*===========================================================================

                            Translation Group

                        API FUNCTION DEFINITIONS

===========================================================================*/
/*=========================================================================
FUNCTION  wms_ts_encode
===========================================================================*/
/** @ingroup translation_group_wms_ts_encode
  Encodes a Transport Service message to over-the-air raw format so the 
  client can send it to a specific destination.
 
  The function blocks until the encoding is completed.

  @datatypes
  #wms_client_ts_data_s \n
  #wms_raw_ts_data_s

  @param[in] client_ts_data_ptr  Pointer to the client message TS data 
                                 structure in the client's memory space.
  @param[out] raw_ts_data_ptr    Pointer to the raw TS data buffer in the 
                                 client's memory space.

  @return
  Status of the request.
   - WMS_TL_UNEXPECTED_PARM_SIZE_S
   - WMS_TL_MISSING_EXPECTED_PARM_S
   - WMS_TL_MISSING_MANDATORY_PARM_S
   - WMS_TL_UNRECOGNIZED_PARM_VALUE_S
   - WMS_TL_UNEXPECTED_PARM_VALUE_S
   - WMS_TL_USER_DATA_SIZE_ERROR_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.
*/

/*
COMMENTS
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.input_other_len
                              to a Valid Value or 0.
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.other_data
                              to a Valid Value or NULL.
*/
wms_status_e_type wms_ts_encode
(
  const wms_client_ts_data_s_type         * client_ts_data_ptr,
  wms_raw_ts_data_s_type                  * raw_ts_data_ptr /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION  wms_ts_decode
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode
  Decodes a Transport Service message from the over-the-air raw message 
  structure to the client message structure.

  The function blocks until the decoding is done.

  @note1hang If the CDMA bearer data has other parameters: upon return of 
             this API function, the mask has WMS_MASK_BD_OTHER set, and 
             desired_other_len indicates how many bytes are required to 
             properly decode the other parameters by calling 
             wms_ts_decode_cdma_bd_with_other().

  @datatypes
  #wms_raw_ts_data_s \n
  #wms_client_ts_data_s

  @param[in] raw_ts_data_ptr      Pointer to the raw TS data in the client's 
                                  memory space.
  @param[out] client_ts_data_ptr  Pointer to the client TS data structure in 
                                  the client's memory space.

  @return
  Status of the request.
   - WMS_TL_UNEXPECTED_PARM_SIZE_S
   - WMS_TL_MISSING_EXPECTED_PARM_S
   - WMS_TL_MISSING_MANDATORY_PARM_S
   - WMS_TL_UNRECOGNIZED_PARM_VALUE_S
   - WMS_TL_UNEXPECTED_PARM_VALUE_S
   - WMS_TL_USER_DATA_SIZE_ERROR_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @sa
  wms_ts_decode_cdma_bd_with_other()
*/

/*
COMMENTS
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.input_other_len
                              to a Valid Value or 0.
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.other_data
                              to a Valid Value or NULL.
*/
wms_status_e_type wms_ts_decode
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr /* OUTPUT */
);

/*=========================================================================
FUNCTION  wms_ts_decode_cdma_bd_with_other
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode_cdma_bd_with_other
  Decodes the CDMA bearer data with other parameters. The function blocks until 
  the decoding is done.

  @note1hang The client shall ensure input_other_len reflects the buffer size 
             of other_data, which should have enough room for decoding the 
             other parameters. The size of the memory allocation in other_data 
             can be determined from the desired_other_len after calling 
             wms_ts_decode().

  @datatypes
  #wms_raw_ts_data_s \n
  #wms_client_ts_data_s

  @param[in] raw_ts_data_ptr      Pointer to the raw TS data in the client's 
                                  memory space.
  @param[out] client_ts_data_ptr  Pointer to the client TS data structure in 
                                  the client's memory space.

  @return
  Status of the request.
   - WMS_TL_UNEXPECTED_PARM_SIZE_S
   - WMS_TL_MISSING_EXPECTED_PARM_S
   - WMS_TL_MISSING_MANDATORY_PARM_S
   - WMS_TL_UNRECOGNIZED_PARM_VALUE_S
   - WMS_TL_UNEXPECTED_PARM_VALUE_S
   - WMS_TL_USER_DATA_SIZE_ERROR_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @sa
  wms_ts_decode()
*/
/*
COMMENTS
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.input_other_len
                              to a Valid Value or 0.
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.other_data
                              to a Valid Value or NULL.
*/
wms_status_e_type wms_ts_decode_cdma_bd_with_other
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr
);

/*=========================================================================
FUNCTION wms_ts_encode_bearer_data
=========================================================================*/
/** @ingroup translation_group_wms_ts_encode_bearer_data
  Encodes the SMS bearer data from the client structure to the raw format.

  @datatypes
  #wms_client_bd_s \n
  #wms_raw_ts_data_s

  @param[in] cl_bd_ptr            Pointer to the client bearer structure in the
                                  client's memory space.
  @param[out] raw_bd_ptr          Pointer to the raw bearer structure in the
                                  client's memory space.

  @return
  Encoding result.
  - WMS_OK_S indicating success
  - Error indicating failure

  @dependencies
  None.

  @sideeffects
  None.

  @sa
  wms_ts_decode_bearer_data()
*/
wms_status_e_type wms_ts_encode_bearer_data
(
  const wms_client_bd_s_type                * cl_bd_ptr,       /* IN */
  wms_raw_ts_data_s_type                    * raw_bd_ptr       /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_decode_bearer_data
=========================================================================*/
/** @ingroup translation_group_wms_ts_decode_bearer_data
  Decodes the SMS bearer data from the raw format to the client structure.

  @datatypes
  #wms_raw_ts_data_s \n
  #wms_client_bd_s

  @param[in] raw_bd_ptr           Pointer to the raw bearer structure in the
                                  client's memory space.
  @param[out] cl_bd_ptr           Pointer to the client bearer structure in the
                                  client's memory space.

  @return
  Encoding result.
  - WMS_OK_S indicating success
  - Error indicating failure

  @dependencies
  None.

  @sideeffects
  None.

  @sa
  wms_ts_encode_bearer_data()
*/
wms_status_e_type wms_ts_decode_bearer_data
(
  const wms_raw_ts_data_s_type            * raw_bd_ptr,   /* IN */
  wms_client_bd_s_type                    * cl_bd_ptr     /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_get_udh_length
===========================================================================*/
/** @ingroup translation_group_wms_ts_get_udh_length
  Retrieves the header length for a user data header.

  This function returns the raw data length of an SMS user data header as it is 
  transmitted over-the-air or stored in the SIM or R-UIM card. It blocks until 
  the conversion is completed.

  @datatypes
  #wms_udh_s

  @param[in] udh              Pointer to the user data header.

  @return
  Number of bytes.

  @dependencies
  None.
*/
uint8 wms_ts_get_udh_length
(
  const wms_udh_s_type                    *udh
);

/*=========================================================================
FUNCTION  wms_ts_bcd_to_ascii
===========================================================================*/
/** @ingroup translation_group_wms_ts_bcd_to_ascii
  Converts BCD digits to an ASCII number string.
 
  The function blocks until the conversion is completed.

  @param[in] addr             Pointer to the BCD string.
  @param[in] len              Length of the BCD string.
  @param[in,out] out          Pointer to the output ASCII buffer.

  @return
  None.

  @dependencies
  None.
*/
void wms_ts_bcd_to_ascii
(
  const uint8         * addr,
  uint8               len,
  uint8               * out  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION  wms_ts_ascii_to_bcd
===========================================================================*/
/** @ingroup translation_group_wms_ts_ascii_to_bcd
  Converts an ASCII number string to BCD digits. The function blocks until the 
  conversion is completed.

  @param[in] ascii            Pointer to the ASCII string.
  @param[in] len              Length of the ASCII string.
  @param[in,out] out          Pointer to the output BCD buffer.

  @return
  None.

  @dependencies
  None.
*/
void wms_ts_ascii_to_bcd
(
  const uint8     * ascii,
  uint8           len,
  uint8           * out  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION  wms_ts_ascii_to_default
===========================================================================*/
/** @ingroup translation_group_wms_ts_ascii_to_default
  Converts an ASCII number string to a GSM 7-bit default alphabet string. 
  The function blocks until the conversion is completed.

  @param[in] asc              Pointer to the ASCII string.
  @param[in] len              Length of the ASCII string.
  @param[in,out] def          Pointer to the output GSM 7-bit default 
                              alphabet string.

  @return
  None.

  @dependencies
  None.
*/
void wms_ts_ascii_to_default
(
  const uint8   * asc,
  uint32        len,
  uint8         * def  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION  wms_ts_default_to_ascii
===========================================================================*/
/** @ingroup translation_group_wms_ts_default_to_ascii
  Converts a 7-bit GSM default alphabet string to an ASCII number string. 
  The function blocks until the conversion is completed.

  @param[in] def              Pointer to the 7-bit GSM default alphabet string.
  @param[in] len              Length of the 7-bit GSM default alphabet string.
  @param[in,out] asc          Pointer to the output ASCII buffer.

  @return
  None.

  @dependencies
  None.
*/
void wms_ts_default_to_ascii
(
  const uint8   * def,
  uint32        len,
  uint8         * asc  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION  wms_ts_decode_relative_time
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode_relative_time
  Decodes a raw relative time byte into a relative time structure as it is 
  transmitted over-the-air. The function blocks until the decoding is completed.

  @datatypes
  #wms_timestamp_s_type

  @param[in] v                Pointer to the raw byte.
  @param[out] timestamp       Pointer to the output structure.

  @return
  None.

  @dependencies
  None.
*/
void wms_ts_decode_relative_time
(
  uint8                     v,
  wms_timestamp_s_type    * timestamp
);

/*=========================================================================
FUNCTION  wms_ts_encode_relative_time
===========================================================================*/
/** @ingroup translation_group_wms_ts_encode_relative_time
  Encodes a relative time structure into a raw byte as it is transmitted 
  over-the-air. The function blocks until the encoding is completed.

  @datatypes
  #wms_timestamp_s_type

  @param[in] timestamp        Pointer to the relative time structure.

  @return
  Raw byte for the relative time.

  @dependencies
  Undetermined.
*/
uint8 wms_ts_encode_relative_time
(
  const wms_timestamp_s_type  *timestamp
);

/*=========================================================================
FUNCTION  wms_ts_encode_dcs
===========================================================================*/
/** @ingroup translation_group_wms_ts_encode_dcs
  Encodes a Data Coding Scheme (DCS) structure into raw bytes as it is 
  transmitted over-the-air. The function blocks until the encoding is completed.

  @datatypes
  #wms_gw_dcs_s

  @param[in] dcs              Pointer to the DCS structure.
  @param[out] data            Pointer to the output.

  @return
  Number of bytes encoded.

  @dependencies
  None.
*/

uint8 wms_ts_encode_dcs
(
  const wms_gw_dcs_s_type   *dcs,   /* INPUT */
  uint8                     *data   /* OUTPUT */
);

/*=========================================================================
FUNCTION  wms_ts_decode_dcs
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode_dcs
  Decodes raw data coding scheme bytes into a DCS structure. The function 
  blocks until the decoding is completed.

  @datatypes
  #wms_gw_dcs_s

  @param[in] data             Pointer to the raw byte.
  @param[in,out] dcs          Pointer to the output structure.

  @return
  Number of bytes decoded.

  @dependencies
  None.
*/
uint8 wms_ts_decode_dcs
(
  const uint8           *data,     /* INPUT */
  wms_gw_dcs_s_type     *dcs       /* INPUT/OUTPUT */
);

/*=========================================================================
FUNCTION wms_ts_encode_address
=========================================================================*/
/** @ingroup translation_group_wms_ts_encode_address
  Encodes an address structure into a format for SIM storage and for 
  interfacing with lower layers.

  @datatypes
  #wms_address_s

  @param[in]     addr        Pointer to the address type.
  @param[in,out] data        Pointer to the output data stream.

  @return
  Number of bytes encoded.

  @dependencies
  None.
*/
uint8 wms_ts_encode_address
(
  const wms_address_s_type  *addr,
  uint8                     *data
);

/*=========================================================================
FUNCTION wms_ts_decode_address
=========================================================================*/
/** @ingroup translation_group_wms_ts_decode_address
  Decodes an address data stream, used by lower layers and for storage on a 
  SIM, into an address structure.

  @datatypes
  #wms_address_s

  @param[in]     data        Pointer to the input data stream.
  @param[in,out] addr        Pointer to the address type.

  @return
  Number of bytes decoded.

  @dependencies
  None.
*/
uint8 wms_ts_decode_address
(
  const uint8               *data,
  wms_address_s_type        *addr
);

/*===========================================================================
FUNCTION    wms_ts_pack_ascii
===========================================================================*/
/** @ingroup translation_group_wms_ts_pack_ascii
  Packs the ASCII characters into CDMA user data format. The function blocks 
  until the encoding is completed.

  @param[in] ascii_ptr         Pointer to a NULL-terminated ASCII string.
  @param[in,out] data          Pointer to the output result buffer.
  @param[in,out] data_len_ptr  Pointer to the length of the result data.
  @param[in,out] padding_bits_ptr  Pointer to the number of padding bits of 
                                   the result data.

  @return
  ASCII string length.

  @dependencies
  None.
*/
uint8 wms_ts_pack_ascii
(
  const char         * ascii_ptr,       /* IN */
  uint8              * data,            /* IN/OUT */
  uint8              * data_len_ptr,    /* IN/OUT */
  uint8              * padding_bits_ptr /* IN/OUT */
);

/*===========================================================================
FUNCTION    wms_ts_unpack_ascii
===========================================================================*/
/** @ingroup translation_group_wms_ts_unpack_ascii
  Unpacks ASCII characters from the CDMA user data parameter. The function 
  blocks until the encoding is completed.

  @datatypes
  #wms_cdma_user_data_s

  @param[in] ud               Pointer to the CDMA user data structure.
  @param[in] buf_len          Input buffer length.
  @param[in,out] buf          Output buffer pointer.

  @return
  ASCII string length.

  @dependencies
  None.
*/
uint8 wms_ts_unpack_ascii
(
  const wms_cdma_user_data_s_type    *ud,     /* IN */
  uint8                              buf_len, /* IN */
  uint8                              *buf     /* INOUT */
);

/*===========================================================================
FUNCTION    wms_ts_dtmf2ascii
===========================================================================*/
/** @ingroup translation_group_wms_ts_dtmf2ascii
  Converts DTMF digits into ASCII digits. The function blocks until the 
  encoding is completed.

  @param[in] len              Length of the input buffer.
  @param[in] dtmf             Input DTMF digits, each in a byte.
  @param[out] ascii           Pointer to the output buffer.

  @return
  Number of digits converted.

  @dependencies
  None.
*/
uint8 wms_ts_dtmf2ascii
(
  uint8            len,
  const uint8     *dtmf,
  uint8           *ascii
);

/*===========================================================================
FUNCTION    wms_ts_ascii2dtmf
===========================================================================*/
/** @ingroup translation_group_wms_ts_ascii2dtmf
  Converts ASCII digits into DTMF digits. The function blocks until the 
  encoding is completed.

  @param[in] ascii            Pointer to NULL-terminated ASCII string.
  @param[out] dtmf            Pointer to the output DTMF buffer.

  @return
  Number of digits converted.

  @dependencies
  None.
*/
uint8 wms_ts_ascii2dtmf
(
  const char     *ascii,
  uint8          *dtmf
);

/*=========================================================================
FUNCTION  wms_ts_pack_gw_user_data
===========================================================================*/
/** @ingroup translation_group_wms_ts_pack_gw_user_data
  Packs GSM 7-bit user data into 8-bit octets for supporting SAT/USAT. The 
  function blocks until the packing is completed.

  @datatypes
  #wms_raw_ts_data_s

  @param[in,out] raw_ts_data_ptr  Pointer to the input and output raw_ts data.

  @return
  WMS_OK_S

  @dependencies
  None.

  @sideeffects
  Modifies raw_ts_data_ptr directly.
*/
wms_status_e_type wms_ts_pack_gw_user_data
(
  wms_raw_ts_data_s_type            * raw_ts_data_ptr
);

/*=========================================================================
FUNCTION  wms_ts_pack_cdma_user_data
===========================================================================*/
/** @ingroup translation_group_wms_ts_pack_cdma_user_data
  Packs only the user data characters from a CDMA message into an over-the-air 
  format in the same bearer data buffer.

  This function accepts a CDMA SMS raw bearer data message with the 7-bit 
  ASCII/IA5 or GSM 7-bit user data characters unpacked. For example, the CDMA 
  Card Application Toolkit (CCAT) uses this function when the R-UIM card needs 
  to send an SMS message to the network with the user data characters unpacked. 
  After this packing is done, the CCAT application can call wms_msg_send() using 
  the packed bearer data as the input.

  @datatypes
  #wms_raw_ts_data_s

  @param[in,out] raw_ts_data_ptr  Pointer to the input and output message 
                                  buffer.

  @return
  Status of the request.

  @dependencies
  None.

  @sideeffects
  Modifies raw_ts_data_ptr directly.
*/
wms_status_e_type wms_ts_pack_cdma_user_data
(
  wms_raw_ts_data_s_type            * raw_ts_data_ptr
);

/*=========================================================================
FUNCTION  wms_ts_cdma_OTA2cl
===========================================================================*/
/** @ingroup translation_group_wms_ts_cdma_OTA2cl
  Converts CDMA SMS messages from raw over-the-air format to the WMS' client 
  message format.

  When the proactive card sends a CDMA message, it provides the over-the-air 
  format. 

  This function can be called by a client (such as CATApp) so that it can later 
  call wms_msg_send() to send the message for the card. The function blocks 
  until the conversion is completed.

  @datatypes
  #wms_client_message_s

  @param[in] data             Pointer to the over-the-air data.
  @param[in] length           Size of the input data.
  @param[out] cl_msg          Pointer to the result of the conversion.

  @return
  WMS_OK_S

  @dependencies
  None.

  @sideeffects
  Modifies cl_msg directly.
*/
wms_status_e_type wms_ts_cdma_OTA2cl
(
  const uint8                 *data,
  uint16                      length,
  wms_client_message_s_type   *cl_msg
);

/*=========================================================================
FUNCTION_new  wms_ts_cdma_cl2OTA
===========================================================================*/
/** @ingroup translation_group_wms_ts_cdma_cl2OTA
  Converts the client message format to over-the-air format.

  @datatypes
  #wms_client_message_s

  @param[in] cl_msg           Pointer to the result of the conversion.
  @param[out] data_buf        Pointer to the data buffer.
  @param[in] data_buf_len     Length of the data buffer.
  @param[out] ota_data_len    Pointer to the over-the-air message length.

  @return
  Status of the conversion.

  @dependencies
  None.
*/
wms_status_e_type wms_ts_cdma_cl2OTA
( 
  wms_client_message_s_type   *cl_msg, 
  uint8                       *data_buf,
  uint16                       data_buf_len,
  uint16                      *ota_data_len
);

/*=========================================================================
FUNCTION  wms_ts_pack_gw_7_bit_chars
===========================================================================*/
/** @ingroup translation_group_wms_ts_pack_gw_7_bit_chars
  Packs the GSM 7-bit default characters into GSM/WCDMA user data format. The 
  function blocks until the packing is completed.

  @param[in] in               Pointer to a GSM 7-bit unpacked string.
  @param[in] in_len           Number of GSM 7-bit unpacked characters.
  @param[in] shift            Number of bits to shift.
  @param[in] out_len_max      Maximum number of output bytes.
  @param[out] out             Pointer to the packed GSM 7-bit default string.

  @return
  Number of bytes encoded.

  @dependencies
  None.
*/
uint16 wms_ts_pack_gw_7_bit_chars
(
  const uint8     * in,
  uint16          in_len, //number of chars
  uint16          shift,
  uint16          out_len_max,
  uint8           * out
);

/*=========================================================================
FUNCTION  wms_ts_unpack_gw_7_bit_chars
===========================================================================*/
/** @ingroup translation_group_wms_ts_unpack_gw_7_bit_chars
  Unpacks the GSM 7-bit default characters from GSM/WCDMA user data format. 
  The function blocks until the packing is completed.

  @param[in] in               Pointer to a GSM 7-bit packed string.
  @param[in] in_len           Number of GSM 7-bit unpacked characters.
  @param[in] out_len_max      Maximum number of output bytes.
  @param[in] shift            Number of bits to shift.
  @param[out] out             Pointer to the unpacked GSM 7-bit default string.

  @return
  Number of bytes decoded.

  @dependencies
  None.
*/
uint8 wms_ts_unpack_gw_7_bit_chars
(
  const uint8       * in,
  uint8             in_len,
  uint8             out_len_max,
  uint16            shift,
  uint8             * out
);

/*=========================================================================
FUNCTION  wms_ts_encode_user_data_header
===========================================================================*/
/** @ingroup translation_group_wms_ts_encode_user_data_header
  Encodes a User Data Header (UDH) structure into raw bytes as it is 
  transmitted over-the-air. The function blocks until the encoding is completed.

  @datatypes
  #wms_udh_s

  @param[in] num_headers      Number of user data headers.
  @param[in] headers          Pointer to the UDH structure.
  @param[out] data            Pointer to the output.

  @return
  Number of bytes encoded.

  @dependencies
  None.
*/
uint8 wms_ts_encode_user_data_header
(
  uint8                           num_headers, /* IN */
  const wms_udh_s_type            * headers,   /* IN */
  uint8                           *data        /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_decode_user_data_header
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode_user_data_header
  Decodes raw user data header bytes into a user data header structure. The 
  function blocks until the decoding is completed.

  @datatypes
  #wms_udh_s

  @param[in] len              Length of the raw bytes.
  @param[in] data             Pointer to the raw bytes.
  @param[out] num_headers_ptr Pointer to the byte in which to store the 
                              number of headers.
  @param[out] udh_ptr         Pointer to the output structure.

  @return
  Number of bytes decoded.

  @dependencies
  None.
*/
uint8 wms_ts_decode_user_data_header
(
  const uint8               len, /* user_data_length*/
  const uint8               *data,
  uint8                     * num_headers_ptr, /* OUT */
  wms_udh_s_type            * udh_ptr          /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_bcd_to_int
===========================================================================*/
/** @ingroup translation_group_wms_ts_bcd_to_int
  Converts BCD digits to integer. Valid BCD values range from 0 to 99, and 
  the byte arrangement is <MSB...LSB>.

  @note1 The function blocks until the conversion is completed.

  @param[in] bcd              BCD digits.
  @param[out] result          Pointer to the output integer buffer.

  @return
   - Failure if either of the BCD digits is invalid (greater than 9, e.g., 
     11111010 = 15 and 10).
   - Success if both BCD digits are valid.

  @dependencies
  None.
*/
boolean wms_ts_bcd_to_int
(
  const uint8 bcd, /*IN*/
  uint8 *result    /*OUT*/
);

/*=========================================================================
FUNCTION  wms_ts_decode_cmas_msg
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode_cmas_msg
  Decodes a Commercial Mobile Alert Service (CMAS) message from raw bytes.

  @datatypes
  #wms_cmas_msg_s_type

  @param[in] data             Pointer to raw CMAS message.
  @param[in] data_len         Length of raw CMAS message.
  @param[out] cmas_msg_ptr    Pointer to the output CMAS message buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_INVALID_PARM_SIZE_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
*/
wms_status_e_type wms_ts_decode_cmas_msg
(
  uint8                     *data,             /* IN */
  uint8                      data_len,         /* IN */
  wms_cmas_msg_s_type       *cmas_msg_ptr      /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_encode_cmas_msg
===========================================================================*/
/** @ingroup translation_group_wms_ts_encode_cmas_msg
  Encodes a CMAS message structure into raw bytes.

  @datatypes
  #wms_cmas_msg_s_type

  @param[in] cmas_msg_ptr     Pointer to the CMAS message buffer.
  @param[in,out] data         Pointer to the data buffer.
  @param[in] data_len         Length of the CMAS message.
  @param[out] data_len_req    Pointer to the data length request.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
*/
wms_status_e_type wms_ts_encode_cmas_msg
(
  wms_cmas_msg_s_type   *cmas_msg_ptr,    /* IN */
  uint8                 *data,            /* IN-OUT */
  uint8                  data_len,        /* IN */
  uint8                 *data_len_req     /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_decode_etws_primary_notification
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode_etws_primary_notification
  Decodes an Earthquake and Tsunami Warninng System (ETWS) primary notification
  from raw bytes.

  @datatypes
  #wms_etws_primary_notification_s

  @param[in] data             Pointer to the raw ETWS message.
  @param[in] data_len         Length of the raw ETWS message.
  @param[out] etws_msg_ptr    Pointer to the output ETWS primary notification 
                              buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_INVALID_PARM_SIZE_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
*/
wms_status_e_type wms_ts_decode_etws_primary_notification
(
  uint8                                *data,             /* IN */
  uint32                                data_len,         /* IN */
  wms_etws_primary_notification_s_type *etws_msg_ptr      /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_encode_etws_primary_notification
===========================================================================*/
/** @ingroup translation_group_wms_ts_encode_etws_primary_notification
  Encodes an ETWS primary notification into raw bytes.

  @datatypes
  #wms_etws_primary_notification_s

  @param[in] etws_msg_ptr     Pointer to the ETWS primary notification buffer.
  @param[in,out] data         Pointer to the data buffer.
  @param[in] data_len         Length of the ETWS message.
  @param[out] data_len_req    Pointer to the required minimum length for the 
                              data buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
*/
wms_status_e_type wms_ts_encode_etws_primary_notification
(
  wms_etws_primary_notification_s_type   *etws_msg_ptr,    /* IN */
  uint8                                  *data,            /* IN-OUT */
  uint32                                  data_len,        /* IN */
  uint32                                 *data_len_req     /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_decode_etws_secondary_notification
===========================================================================*/
/** @ingroup translation_group_wms_ts_decode_etws_secondary_notification
  Decodes an ETWS secondary notification from raw bytes.

  @datatypes
  #wms_etws_notification_format_e_type \n
  #wms_etws_secondary_notification_s

  @param[in] data             Pointer to the raw ETWS secondary notification.
  @param[in] data_len         Length of the raw ETWS secondary notification.
  @param[in] format           Format used for the ETWS secondary notification.
  @param[out] etws_msg_ptr    Pointer to the output ETWS secondary notification 
                              buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_INVALID_PARM_SIZE_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
*/
wms_status_e_type wms_ts_decode_etws_secondary_notification
(
  uint8                                  *data,             /* IN */
  uint32                                  data_len,         /* IN */
  wms_etws_notification_format_e_type     format,           /* IN */
  wms_etws_secondary_notification_s_type *etws_msg_ptr      /* OUT */
);

/*=========================================================================
FUNCTION  wms_ts_encode_etws_secondary_notification
===========================================================================*/
/** @ingroup translation_group_wms_ts_encode_etws_secondary_notification
  Encodes an ETWS secondary notification into raw bytes.

  @datatypes
  #wms_etws_secondary_notification_s

  @param[in] etws_msg_ptr     Pointer to the ETWS secondary notification buffer.
  @param[in,out] data         Pointer to the data buffer.
  @param[in] data_len         Length of the data buffer.
  @param[out] data_len_req    Pointer to the required minimum length for the 
                              data buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
*/
wms_status_e_type wms_ts_encode_etws_secondary_notification
(
  wms_etws_secondary_notification_s_type  *etws_msg_ptr,    /* IN */
  uint8                                   *data,            /* IN-OUT */
  uint32                                   data_len,        /* IN */
  uint32                                  *data_len_req     /* OUT */
);


/* <EJECT> */
/*===========================================================================

                            Debugging Group

                        API FUNCTION DEFINITIONS

===========================================================================*/
/*=========================================================================
FUNCTION  wms_dbg_reset_tl_seq_num
===========================================================================*/
/** @ingroup debug_group_wms_dbg_reset_tl_seq_num
  Resets the Transport Layer's sequence number so the next message sent has a 
  sequence number 0 for the bearer reply option.

  This is a test interface. Typically, clients do not call this function for 
  normal operation.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_dbg_reset_tl_seq_num
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_dbg_reset_tl_seq_num 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dbg_set_msg_ref
===========================================================================*/
/** @ingroup debug_group_wms_dbg_set_msg_ref
  Resets the GSM SMS message reference number to 0.

  This is a test interface. Typically, clients do not call this function for 
  normal operation.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_dbg_set_msg_ref
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
);
/*~ FUNCTION wms_dbg_set_msg_ref 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dbg_clear_smma_flag
===========================================================================*/
/** @ingroup debug_group_wms_dbg_clear_smma_flag
  Resets the GSM SMS SMMA/memory full flag in the SIM's EF-SMSS.

  This is a test interface. Typically, clients do not call this function for 
  normal operation.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_dbg_clear_smma_flag
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
);
/*~ FUNCTION wms_dbg_clear_smma_flag 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dbg_get_retry_interval
===========================================================================*/
/** @ingroup debug_group_wms_dbg_get_retry_interval
  Retrieves the current retry interval value.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE

  @events
  WMS_DBG_EVENT_RETRY_INTERVAL

  @sideeffects
  A command is sent to the WMS task.
*/
/*
  Retrieve the SMS Retry Interval. i.e. the interval between the SMS Retry Attempts
  Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.
*/
wms_status_e_type wms_dbg_get_retry_interval
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
); 
/*~ FUNCTION wms_dbg_get_retry_interval 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dbg_set_retry_interval
===========================================================================*/
/** @ingroup debug_group_wms_dbg_set_retry_interval
  Specifies the retry interval for the retry feature. The retry interval is 
  the interval in seconds between the SMS retry attempts.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] retry_interval  New retry interval in seconds.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_DBG_BAD_PARAM

  @events
  WMS_DBG_EVENT_RETRY_INTERVAL

  @sideeffects
  A command is sent to the WMS task.
*/

/*
  Retrieve the SMS Retry Interval. i.e. the interval between the SMS Retry Attempts
  Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.
*/
wms_status_e_type wms_dbg_set_retry_interval
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  uint32                           retry_interval /* seconds */
);
/*~ FUNCTION wms_dbg_set_retry_interval 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dbg_get_retry_period
===========================================================================*/
/** @ingroup debug_group_wms_dbg_get_retry_period
  Retrieves the current SMS message retry period.

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE

  @events
  WMS_DBG_EVENT_RETRY_PERIOD

  @sideeffects
  A command is sent to the WMS task.
*/
/*
  Retrieves the current SMS message retry period. 
  A Debug Event will be posted by WMS to its clients with the Retry Period.
*/
wms_status_e_type wms_dbg_get_retry_period
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
); 
/*~ FUNCTION wms_dbg_get_retry_period 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_dbg_set_retry_period
===========================================================================*/
/** @ingroup debug_group_wms_dbg_set_retry_period
  Specifies the retry period for the retry feature. 
  
  If period = nonzero, the retry period is enabled. If period = 0 seconds, the 
  retry period is disabled.

  When the retry period is enabled and an error is received from the lower 
  layers, the WMS waits for five seconds by default and sends the message again. 
  However, the retry is not performed under the following conditions:
   - If there are errors from the network (CDMA SMS ACK with cause code, or 
     GSM SMS RP/CP errors)
   - If the message is too large
   - If the message fails the MO control by the SIM card

  @param[in] client_id      Client ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] retry_period   Total retry period in seconds.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_DBG_BAD_PARAM

  @newpage
  @events
  WMS_DBG_EVENT_RETRY_PERIOD

  @sideeffects
  A command is sent to the WMS task.
*/

/*
  Specifies the SMS message retry period. Command is sent to WMS Task.
  A Debug Event will be posted by WMS to its clients with the Retry Period.
*/
wms_status_e_type wms_dbg_set_retry_period
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  uint32                           retry_period /* seconds */
);
/*~ FUNCTION wms_dbg_set_retry_period 
    ONERROR return WMS_RPC_ERROR_S */


/*===========================================================================

                          Dual SIM/Dual Standby

                        API FUNCTION DEFINITIONS

===========================================================================*/
/* DSDS configuration */
/*=========================================================================
FUNCTION_new  wms_cfg_ms_get_routes
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_get_routes
  Retrieves the current message routing configuration in a specified 
  subscription account.

  @param[in] client_id    Client ID.
  @param[in] as_id        Active subscription ID.
  @param[in] cmd_cb       Command callback for reporting the command status; 
                          see #wms_cmd_cb_type.
  @param[in] user_data    Pointer provided by the client to uniquely identify 
                          this transaction of sending a message. The same 
                          pointer is passed to the client's callback function. 
                          A NULL pointer is acceptable.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_get_routes
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,     
  wms_cmd_cb_type                  cmd_cb,
  const void*                      user_data
);
/*~ FUNCTION wms_cfg_ms_get_routes 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_set_routes
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_set_routes
  Changes the message routing configuration in a specified subscription 
  account.

  @datatypes
  #wms_routes_s

  @param[in] client_id    Client ID.
  @param[in] as_id        Active subscription ID.
  @param[in] cmd_cb       Command callback for reporting the command status; 
                          see #wms_cmd_cb_type.
  @param[in] user_data    Pointer provided by the client to uniquely identify 
                          this transaction of sending a message. The same 
                          pointer is passed to the client's callback function. 
                          A NULL pointer is acceptable.
  @param[in] routes_ptr   Pointer to the routing configuration.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_set_routes
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void*                      user_data,
  const wms_routes_s_type*         routes_ptr
);
/*~ FUNCTION wms_cfg_ms_set_routes 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN routes_ptr POINTER */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_get_memory_status
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_get_memory_status
  Retrieves the status of a memory store on a specified subscription account.

  If tag == WMS_TAG_NONE, all used slots are calculated. Otherwise, only 
  messages having this tag value are counted as used_tag_slots.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_tag_e_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to retrieve the status.
  @param[in] tag         Type of message for which the status is to be returned.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_get_memory_status
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
);
/*~ FUNCTION wms_cfg_ms_get_memory_status
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_get_message_list
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_get_message_list
  Retrieves the message list for a memory store on a specified subscription 
  account.

  If tag == WMS_TAG_NONE, all types of messages are included in the list. 
  Otherwise, only messages having this tag value are included.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_tag_e_type

  @param[in] client_id    Client ID.
  @param[in] as_id        Active subscription ID.
  @param[in] cmd_cb       Command callback for reporting the command status; 
                          see #wms_cmd_cb_type.
  @param[in] user_data    Pointer provided by the client to uniquely identify 
                          this transaction of sending a message. The same 
                          pointer is passed to the client's callback function. 
                          A NULL pointer is acceptable.
  @param[in] mem_store    Memory store from which to retrieve the message list.
  @param[in] tag          Type of message list to be returned.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_get_message_list
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
);
/*~ FUNCTION wms_cfg_ms_get_message_list
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_cfg_ms_get_gw_domain_pref
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_get_gw_domain_pref
  Retrieves the GSM/WCDMA domain preference in a specified subscription 
  account.

  @param[in] client_id    Client ID.
  @param[in] as_id        Active subscription ID.
  @param[in] cmd_cb       Command callback for reporting the command status; 
                          see #wms_cmd_cb_type.
  @param[in] user_data    Pointer provided by the client to uniquely identify 
                          this transaction of sending a message. The same 
                          pointer is passed to the client's callback function. 
                          A NULL pointer is acceptable.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_get_gw_domain_pref
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
);
/*~ FUNCTION wms_cfg_ms_get_gw_domain_pref
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_set_gw_domain_pref
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_set_gw_domain_pref
  Sets the GSM/WCDMA domain preference in a specified subscription account.

  @datatypes
  #wms_gw_domain_pref_e_type

  @param[in] client_id       Client ID.
  @param[in] as_id           Active subscription ID.
  @param[in] cmd_cb          Command callback for reporting the command status; 
                             see #wms_cmd_cb_type.
  @param[in] user_data       Pointer provided by the client to uniquely identify 
                             this transaction of sending a message. The same 
                             pointer is passed to the client's callback 
                             function. A NULL pointer is acceptable.
  @param[in] gw_domain_pref  New domain preference.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_set_gw_domain_pref
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_gw_domain_pref_e_type         gw_domain_pref
);
/*~ FUNCTION wms_cfg_ms_set_gw_domain_pref
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new   wms_cfg_ms_locate_mo_msg_for_sts_report
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_locate_mo_msg_for_sts_report
  Locates the original MO message's index on the specified 
  SIM given the message reference number in the received status report message.

  @note1 This is a synchronous function call.

  @datatypes
  #wms_message_index_type \n
  #wms_message_number_type

  @param[in] as_id            Active subscription ID.
  @param[out] rec_id          Pointer to the return result.
  @param[in] message_ref      Reference number for the MO message.

  @return 
  TRUE -- Message index was found. \n
  FALSE -- Message index was not found.

  @dependencies 
  None.
*/
boolean wms_cfg_ms_locate_mo_msg_for_sts_report
(
  sys_modem_as_id_e_type   as_id,
  wms_message_index_type  *rec_id,      /* OUTPUT */
  wms_message_number_type  message_ref  /* INPUT */
);
/*~ FUNCTION wms_cfg_ms_locate_mo_msg_for_sts_report 
    ONERROR return FALSE */
/*~ PARAM OUT rec_id POINTER */ 

/*=========================================================================
FUNCTION_new   wms_cfg_ms_get_sts_rpt_index
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_get_sts_rpt_index
  Retrieves the status report's index on the specified SIM given the MO 
  SMS's index on the SIM.

  @note1 This is a synchronous function call.

  @datatypes
  #wms_message_index_type

  @param[in] as_id            Active subscription ID.
  @param[in] sms_index        Index of the MO SMS message.
  @param[out] smsr_index      Pointer to the result.

  @return 
  TRUE -- Status report index was found. \n
  FALSE -- Status report index was not found.

  @dependencies 
  None.
*/
boolean wms_cfg_ms_get_sts_rpt_index
(
  sys_modem_as_id_e_type  as_id,
  wms_message_index_type  sms_index,   /* INPUT */
  wms_message_index_type  *smsr_index  /* OUTPUT */
);
/*~ FUNCTION wms_cfg_ms_get_sts_rpt_index 
    ONERROR return FALSE */
/*~ PARAM OUT smsr_index POINTER */

/*===========================================================================
FUNCTION_new   wms_cfg_ms_get_link_control
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_get_link_control
  Retrieves the current link control setting in a specified subscription 
  account.
 
  The WMS sends a configuration event to all clients, telling them if the 
  link is up or down and what the mode is.

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.

  @return 
  None.

  @dependencies 
  None.
*/
wms_status_e_type wms_cfg_ms_get_link_control
(
  wms_client_id_type                client_id,
  sys_modem_as_id_e_type            as_id,
  wms_cmd_cb_type                   cmd_cb,
  const void                       *user_data
);
/*~ FUNCTION wms_cfg_ms_get_link_control 
    ONERROR return WMS_RPC_ERROR_S */

/*===========================================================================
FUNCTION_new   wms_cfg_ms_set_link_control
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_set_link_control
  Sends a request to the lower layers to disable/enable SMS link control for
  connection establishment and timer duration in a specified subscription 
  account.

  @datatypes
  #wms_cfg_link_control_mode_e_type

  @param[in] client_id       Client ID.
  @param[in] as_id           Active subscription ID.
  @param[in] cmd_cb          Command callback for reporting the command status; 
                             see #wms_cmd_cb_type.
  @param[in] user_data       Pointer provided by the client to uniquely identify 
                             this transaction of sending a message. The same 
                             pointer is passed to the client's callback 
                             function. A NULL pointer is acceptable.
  @param[in] control_option  WMS_LINK_CONTROL_DISABLED: disable link control 
                             so the lower layer can release the link.
  @param[in] idle_timer      Idle time period in seconds before the link is 
                             released.

  @return 
  None.

  @dependencies 
  None.
*/
wms_status_e_type wms_cfg_ms_set_link_control
(
  wms_client_id_type                client_id,
  sys_modem_as_id_e_type            as_id,
  wms_cmd_cb_type                   cmd_cb,
  const void                       *user_data,
  wms_cfg_link_control_mode_e_type  control_option,
  uint8                             idle_timer /* in seconds */
);
/*~ FUNCTION wms_cfg_ms_set_link_control
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_set_primary_client
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_set_primary_client
  Sets a client as the primary client in a specified subscription account.

  The advantage of being a primary client is that the memory status is 
  checked for memory full and an SMMA Request is sent to the network. 
  After a primary client is set, no other client can become the primary client.
  If set_primary == FALSE, the primary client is deregistered.

  @param[in] client_id       Client ID.
  @param[in] as_id           Active subscription ID.
  @param[in] cmd_cb          Command callback for reporting the command status; 
                             see #wms_cmd_cb_type.
  @param[in] user_data       Pointer provided by the client to uniquely 
                             identify this transaction of sending a message. 
                             The same pointer is passed to the client's 
                             callback function. A NULL pointer is acceptable.
  @param[in] set_primary     Boolean flag to set/reset the primary client.
  @param[in] use_client_memory  Boolean flag to use the client's memory store.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.

  @sa 
  wms_cfg_ms_set_memory_full()
*/
wms_status_e_type wms_cfg_ms_set_primary_client
(
  wms_client_id_type      client_id,
  sys_modem_as_id_e_type  as_id,
  wms_cmd_cb_type         cmd_cb,
  const void            * user_data,
  boolean                 set_primary,
  boolean                 use_client_memory
);
/*~ FUNCTION wms_cfg_ms_set_primary_client
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_set_memory_full
===========================================================================*/
/** @ingroup config_group_wms_cfg_ms_set_memory_full
  Sets a primary client's memory status in a specified subscription account.
 
  Only the primary client is allowed to set/reset the memory status.

  @param[in] client_id    Client ID.
  @param[in] as_id        Active subscription ID.
  @param[in] cmd_cb       Command callback for reporting the command status; 
                          see #wms_cmd_cb_type.
  @param[in] user_data    Pointer provided by the client to uniquely identify 
                          this transaction of sending a message. The same 
                          pointer is passed to the client's callback function. 
                          A NULL pointer is acceptable.
  @param[in] memory_full  Boolean flag to set/reset the memory full flag of 
                          the client memory.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.

  @sa 
  wms_cfg_ms_set_primary_client()
*/
wms_status_e_type wms_cfg_ms_set_memory_full
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                     * user_data,
  boolean                          memory_full
);
/*~ FUNCTION wms_cfg_ms_set_memory_full
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_set_message_waiting
===========================================================================*/
/**
Sets the MWI information in a specified subscription account.

@param client_id   [IN] Client ID.
@param as_id       [IN] Active subscription ID.
@param cmd_cb      [IN] Command callback for reporting the command status.
@param user_data   [IN] Pointer provided by the client to uniquely identify 
                        this transaction of sending a message. The same pointer 
                        is passed to the client's callback function. A NULL 
                        pointer is acceptable.
@param mwi_info    [IN] Pointer to the MWI information. 
@param num_mwi     [IN] Number of instances of MWI to update. 

@return Status of the request.
 - WMS_OK_S
 - WMS_OUT_OF_RESOURCES_S

@dependencies None.

@sideeffects A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_set_message_waiting
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void*                      user_data,
  wms_gw_mwi_info_s_type           *mwi_info,
  uint8                            num_mwi
);
/*~ FUNCTION wms_cfg_ms_set_message_waiting 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_cfg_ms_get_message_waiting
===========================================================================*/
/**
Gets the MWI information from a specified subscription account.

@param client_id   [IN] Client ID.
@param as_id       [IN] Active subscription ID.
@param cmd_cb      [IN] Command callback for reporting the command status.
@param user_data   [IN] Pointer provided by the client to uniquely identify 
                        this transaction of sending a message. The same pointer 
                        is passed to the client's callback function. A NULL 
                        pointer is acceptable.
@return Status of the request.
 - WMS_OK_S
 - WMS_OUT_OF_RESOURCES_S

@dependencies None.

@sideeffects A command is sent to the WMS task.
*/
wms_status_e_type wms_cfg_ms_get_message_waiting
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void*                      user_data
);
/*~ FUNCTION wms_cfg_ms_get_message_waiting 
    ONERROR return WMS_RPC_ERROR_S */

/* DSDS messaging */
/*=========================================================================
FUNCTION_new  wms_msg_ms_send
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_send
  Sends a message using a specified subscription account.

  @datatypes
  #wms_send_mode_e_type \n
  #wms_client_message_s

  @param[in] client_id     Client ID.
  @param[in] as_id         Active subscription ID.
  @param[in] cmd_cb        Command callback for reporting the command status; 
                           see #wms_cmd_cb_type.
  @param[in] user_data     Pointer provided by the client to uniquely identify 
                           this transaction of sending a message. The same 
                           pointer is passed to the client's callback function. 
                           A NULL pointer is acceptable.
  @param[in] send_mode     Mode for sending the message.
  @param[in] message_ptr   Pointer to the client's SMS message buffer. The API 
                           must copy the data in this buffer into its own 
                           buffer so the client can reuse it after the function 
                           returns.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_send
(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                       * user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type  * message_ptr
);
/*~ FUNCTION  wms_msg_ms_send 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN message_ptr POINTER */ 

/*=========================================================================
FUNCTION_new  wms_msg_ms_send_ext
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_send_ext
  Sends a message using a specified subscription account over a specified 
  transport layer.

  @datatypes
  #wms_send_mode_e_type \n
  #wms_client_message_s \n
  #wms_msg_transport_type_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] send_mode      Mode for sending the message.
  @param[in] message_ptr    Pointer to the client's SMS message buffer. The 
                            function must copy the data in this buffer into its 
                            own buffer so the client can reuse it after the 
                            function returns.
  @param[in] transport_type Transport layer, such as IMS or SGs, to deliver SMS.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S   @vertspace{-3}

  @dependencies
  None.   @vertspace{-3}

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_send_ext
(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                       * user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type  * message_ptr,
  wms_msg_transport_type_e_type      transport_type 
);
/*~ FUNCTION  wms_msg_ms_send_ext 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN message_ptr POINTER */ 

/*=========================================================================
FUNCTION  wms_msg_ms_resend
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_resend
  Sends a retry message using a specified subscription account with a 
  client-specified message ID. 

  @datatypes
  #wms_send_mode_e_type \n
  #wms_client_message_s \n
  #wms_message_number_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID; not used for non-DSDS 
                            targets.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] send_mode      Mode for sending the message.
  @param[in] message_ptr    Pointer to the client's SMS message buffer. The 
                            function must copy the data in this buffer into its 
                            own buffer so the client can reuse it after the 
                            function returns. 
  @param[in] client_msg_id  Message ID assigned by the client for this retry 
                            message. If \n 
                            the ID set to an invalid number, i.e., > 0xFF for GW 
                            and \n
                            > 0xFFFF for CDMA, no re-encoding is performed. 
                            Otherwise, the WMS re-encodes the raw TS data with 
                            this ID.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @newpage
  @dependencies
  None.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_resend
(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                       * user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type  * message_ptr,
  wms_message_number_type            client_msg_id
);
/*~ FUNCTION  wms_msg_ms_resend 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN message_ptr POINTER */  


/*=========================================================================
FUNCTION  wms_msg_ms_resend_ext
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_resend_ext
  Sends a retry message using a specified subscription account with a 
  client-specified message ID over a specified transport layer.

  @datatypes
  #wms_send_mode_e_type \n
  #wms_client_message_s \n
  #wms_message_number_type \n
  #wms_msg_transport_type_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID; not used for non-DSDS 
                            targets.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] send_mode      Mode for sending the message.
  @param[in] message_ptr    Pointer to the client's SMS message buffer. The 
                            function must copy the data in this buffer into its 
                            own buffer so the client can reuse it after the 
                            function returns. 
  @param[in] client_msg_id  Message ID assigned by the client for this retry 
                            message. If \n 
                            the ID set to an invalid number, i.e., > 0xFF for GW 
                            and \n
                            > 0xFFFF for CDMA, no re-encoding is performed. 
                            Otherwise, the WMS re-encodes the raw TS data with 
                            this ID.
  @param[in] transport_type Transport layer, such as IMS or SGs, to deliver SMS.
                          
  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_resend_ext
(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                       * user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type  * message_ptr,
  wms_message_number_type            client_msg_id,
  wms_msg_transport_type_e_type      transport_type                            
);
/*~ FUNCTION  wms_msg_ms_resend_ext 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN message_ptr POINTER */  

/*=========================================================================
FUNCTION_new  wms_msg_ms_ack_ext
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_ack_ext
  Acknowledges a message, if it has not been acknowledged, using a 
  specified subscription account over a specified transport layer.

  @datatypes
  #wms_ack_info_s \n
  #wms_msg_transport_type_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] ack_info_ptr   Pointer to acknowledgment information.
  @param[in] transport_type Transport layer, such as IMS or SGs, to deliver SMS.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @sideeffects
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_ack_ext
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  const wms_ack_info_s_type      *ack_info_ptr,
  wms_msg_transport_type_e_type   transport_type    
);
/*~ FUNCTION  wms_msg_ms_ack_ext 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN ack_info_ptr POINTER */

/*=========================================================================
FUNCTION_new  wms_msg_ms_ack
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_ack
  Acknowledges a message, if it has not been acknowledged, using a specified 
  subscription account.

  @datatypes
  #wms_ack_info_s

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] ack_info_ptr   Pointer to the acknowledgment information.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_ack
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  const wms_ack_info_s_type      *ack_info_ptr
);
/*~ FUNCTION  wms_msg_ms_ack 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN ack_info_ptr POINTER */

/*=========================================================================
FUNCTION_new  wms_msg_ms_read
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_read
  Reads a message from a memory store in a specified subscription account.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to read the message.
  @param[in] index       Index of the message.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_read
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION wms_msg_ms_read
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_msg_ms_read_sts_report
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_read_sts_report
  Reads a status report from a memory store in a specified subscription 
  account.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to read the status report.
  @param[in] index       Index of the status report.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_read_sts_report
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_ms_read_sts_report
    ONERROR return WMS_RPC_ERROR_S */


/*=========================================================================
FUNCTION  wms_msg_ms_write
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_write
  Writes a message to a memory store in a specified subscription account.

  @datatypes
  #wms_write_mode_e_type \n
  #wms_client_message_s

  @param[in] client_id     Client ID.
  @param[in] as_id         Active subscription ID.
  @param[in] cmd_cb        Command callback for reporting the command status; 
                           see #wms_cmd_cb_type.
  @param[in] user_data     Pointer provided by the client to uniquely identify 
                           this transaction of sending a message. The same 
                           pointer is passed to the client's callback function. 
                           A NULL pointer is acceptable.
  @param[in] write_mode    Mode for storing the message.
  @param[in] message_ptr   Pointer to the client's SMS message buffer. The API 
                           must copy the data in this buffer into its own 
                           buffer so the client can reuse it after the function 
                           returns.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_write
(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                       * user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type  * message_ptr
);
/*~ FUNCTION wms_msg_ms_write 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN message_ptr POINTER */

/*=========================================================================
FUNCTION_new  wms_msg_ms_delete
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_delete
  Deletes a message from a memory store in a specified subscription account.
 
  If the corresponding status report for the message is stored, the report is 
  also deleted.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type

  @param[in] client_id    Client ID.
  @param[in] as_id        Active subscription ID.
  @param[in] cmd_cb       Command callback for reporting the command status; 
                          see #wms_cmd_cb_type.
  @param[in] user_data    Pointer provided by the client to uniquely identify 
                          this transaction of sending a message. The same 
                          pointer is passed to the client's callback function. 
                          A NULL pointer is acceptable.
  @param[in] mem_store    Memory store from which to delete the message.
  @param[in] index        Index of the message.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_delete
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION wms_msg_ms_delete
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_msg_ms_delete_all
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_delete_all
  Deletes all messages from a memory store in a specified subscription account.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_tag_e_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to delete the messages.
  @param[in] tag         Type of messages to be deleted.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_delete_all
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_tag_e_type          tag
);
/*~ FUNCTION wms_msg_ms_delete_all
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_msg_ms_modify_tag
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_modify_tag
  Modifies the tag of a message stored in a specified subscription account.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type \n
  #wms_message_tag_e_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store containing the message.
  @param[in] index       Index of the message.
  @param[in] tag         Type of message for which the tag is to be modified.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_modify_tag
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag
);
/*~ FUNCTION  wms_msg_ms_modify_tag
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_msg_ms_read_template
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_read_template
  Reads a template from a memory store in a specified subscription account.

  If the memory store is NV, the message index is ignored because only one 
  template is in NV.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to read the template.
  @param[in] index       Index of the template.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_read_template
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_ms_read_template
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_msg_ms_write_template
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_write_template
  Writes a template from a memory store in a specified subscription account.
 
  If the memory store is NV, the message index is ignored because only one 
  template is in NV. Otherwise, only the SIM memory store is allowed.

  @datatypes
  #wms_write_mode_e_type \n
  #wms_client_message_s

  @param[in] client_id     Client ID.
  @param[in] as_id         Active subscription ID.
  @param[in] cmd_cb        Command callback for reporting the command status; 
                           see #wms_cmd_cb_type.
  @param[in] user_data     Pointer provided by the client to uniquely identify 
                           this transaction of sending a message. The same 
                           pointer is passed to the client's callback function. 
                           A NULL pointer is acceptable.
  @param[in] write_mode    Record is to be inserted or modified.
  @param[in] message_ptr   Pointer to the client's SMS message buffer. The API 
                           must copy the data in this buffer into its own 
                           buffer so the client can reuse it after the function 
                           returns.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/

/*
COMMENTS
  If referenced in the union, set msg_ptr->u.gw_template.alpha_id.data
                              to a Valid Value or NULL
  If referenced in the union, set msg_ptr->u.gw_template.alpha_id.len
                              to a Valid Value or 0
*/
wms_status_e_type wms_msg_ms_write_template
(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_write_mode_e_type            write_mode,
  const wms_client_message_s_type  * message_ptr
);
/*~ FUNCTION  wms_msg_ms_write_template 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN message_ptr POINTER */

/*=========================================================================
FUNCTION_new  wms_msg_ms_delete_template
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_delete_template
  Deletes a template from a memory store in a specified subscription account.
 
  Only an NV or SIM memory store is allowed. 

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to delete the template.
  @param[in] index       Index of the template.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_delete_template
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_ms_delete_template 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_msg_ms_delete_template_all
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_delete_template_all
  Deletes all templates from a memory store in a specified subscription 
  account.
 
  Only an NV or SIM memory store is allowed. 

  @datatypes
  #wms_memory_store_e_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to delete the templates.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_delete_template_all
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store
);
/*~ FUNCTION  wms_msg_ms_delete_template_all
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_msg_ms_write_sts_report
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_write_sts_report
  Writes a status report to a memory store in a specified subscription account.

  The status report is stored only if the corresponding SMS Message is also 
  stored in the same memory. If the SMS Message is not found, 
  WMS_CMD_ERR_MSG_NO_MO_MSG is returned in the command callback.

  @datatypes
  #wms_write_mode_e_type \n
  #wms_client_message_s

  @param[in] client_id     Client ID.
  @param[in] as_id         Active subscription ID.
  @param[in] cmd_cb        Command callback for reporting the command status; 
                           see #wms_cmd_cb_type.
  @param[in] user_data     Pointer provided by the client to uniquely identify 
                           this transaction of sending a message. The same 
                           pointer is passed to the client's callback function. 
                           A NULL pointer is acceptable.
  @param[in] write_mode    Mode for storing the report.
  @param[in] message_ptr   Pointer to the client's SMS message buffer. The API 
                           must copy the data in this buffer into its own buffer 
                           so the client can reuse it after the function 
                           returns.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  None.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_write_sts_report
(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                       * user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type  * message_ptr
);
/*~ FUNCTION  wms_msg_ms_write_sts_report 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN message_ptr POINTER */

/*=========================================================================
FUNCTION_new  wms_msg_ms_delete_sts_report
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_delete_sts_report
  Deletes a status report from a memory store in a specified subscription 
  account.

  @datatypes
  #wms_memory_store_e_type \n
  #wms_message_index_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to delete the status report.
  @param[in] index       Index of the status report.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  Client must have been initialized.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_delete_sts_report
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_ms_delete_sts_report
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION_new  wms_msg_ms_delete_sts_report_all
===========================================================================*/
/** @ingroup messaging_group_wms_msg_ms_delete_sts_report_all
  Deletes all status reports from a memory store in a specified subscription 
  account.

  @datatypes
  #wms_memory_store_e_type

  @param[in] client_id   Client ID.
  @param[in] as_id       Active subscription ID.
  @param[in] cmd_cb      Command callback for reporting the command status; 
                         see #wms_cmd_cb_type.
  @param[in] user_data   Pointer provided by the client to uniquely identify 
                         this transaction of sending a message. The same 
                         pointer is passed to the client's callback function. 
                         A NULL pointer is acceptable.
  @param[in] mem_store   Memory store from which to delete the status reports.

  @return 
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies 
  Client must have been initialized.

  @sideeffects 
  A command is sent to the WMS task.
*/
wms_status_e_type wms_msg_ms_delete_sts_report_all
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store
);
/*~ FUNCTION  wms_msg_ms_delete_sts_report_all
    ONERROR return WMS_RPC_ERROR_S */


/* DSDS multimode broadcast */
/*=========================================================================
FUNCTION   wms_bc_ms_get_config
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_get_config
  Retrieves the carrier's configuration for multimode broadcast SMS in a
  specified subscription account.

  The result is returned to the user through the event callback function. The 
  command status is returned through the command callback function.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @events
  WMS_BC_MM_EVENT_CONFIG

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_get_config
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_ms_get_config 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_get_pref
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_get_pref
  Retrieves the user's preference for multimode broadcast SMS in a
  specified subscription account.

  The result is returned to the user through the event callback function. The 
  command status is returned through the command callback function.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE

  @newpage
  @events
  WMS_BC_MM_EVENT_PREF

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_get_pref
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_ms_get_pref 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_set_pref
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_set_pref
  Changes the user's preference for multimode broadcast SMS in a
  specified subscription account.

  The result is returned to the user through the event callback function. The 
  command status is returned through the command callback function. 

  If the enabling status of the broadcast SMS is changed because of the new 
  preference, an enable/disable command is sent to the lower layers.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_bc_pref_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] pref           New user preference.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @newpage
  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_CANNOT_ACTIVATE \n
  WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL \n
  WMS_CMD_ERR_BC_BAD_PREF \n
  WMS_CMD_ERR_BC_NV_WRITE

  @events
  WMS_BC_MM_EVENT_PREF

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_set_pref
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  wms_bc_pref_e_type              pref
);
/*~ FUNCTION wms_bc_ms_set_pref 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_get_table
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_get_table
  Retrieves the whole broadcast SMS service table in a specified
  subscription account.

  The result is returned to the user through the event callback function. The 
  command status is returned through the command callback function.

  @note1hang To prevent retrieving a large service table, the 
             wms_bc_mm_get_all_service_ids() function can be called to retrieve 
             all service IDs, where each ID is much smaller than a complete 
             table entry. Then the wms_bc_mm_get_service_info() function can 
             be called for each service ID to retrieve the table entries 
             one by one.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE

  @newpage
  @events
  WMS_BC_MM_EVENT_TABLE

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_get_table
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_ms_get_table 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_select_service
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_select_service
  Selects/deselects a broadcast SMS service in the service table for filtering 
  purposes in a specified subscription account.

  The broadcast messages for this service can be received when the service is 
  selected or when the current user's preference is All Services.

  For GSM SMS CB, the service IDs of the selected entries are written to the 
  SIM's EF-CBMI and EF-CBMIR. If the enabling status of broadcast SMS is 
  changed due to the new selection, an enable/disable command is sent to the 
  lower layers.

  @note1hang If the current user preference is table only and no service 
             is selected in the table, broadcast SMS is disabled.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_bc_mm_srv_id_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] srv_id_ptr     Pointer to the service ID for the service.
  @param[in] selected       Indicates whether this service is selected.

  @return
  Status of the request.
   - WMS_OK_S 
   - WMS_OUT_OF_RESOURCES_S

  @newpage
  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_BAD_SRV_ID \n
  WMS_CMD_ERR_BC_NV_WRITE \n
  WMS_CMD_ERR_BC_SIM_WRITE

  @events
  WMS_BC_MM_EVENT_SRV_UPDATED

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_select_service
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  const wms_bc_mm_srv_id_type     *srv_id_ptr,
  boolean                         selected
);
/*~ FUNCTION wms_bc_ms_select_service 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN srv_id_ptr POINTER */

/*=========================================================================
FUNCTION  wms_bc_ms_get_all_service_ids
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_get_all_service_ids
  Retrieves all the service IDs from the broadcast SMS service table 
  in a specified subscription account.
 
  The result is returned to the user through the event callback function. The 
  command status is returned through the command callback function.

  The result does not contain the priorities and labels for each service. The 
  wms_bc_mm_get_service_info() function can be called for each service to 
  retrieve the service information one by one.

  @note1 The purpose of this function is to reduce the return buffer size.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_NO_RESOURCE

  @events
  WMS_BC_MM_EVENT_SRV_IDS

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_get_all_service_ids
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_ms_get_all_service_ids 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_get_service_info
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_get_service_info
  Retrieves a table entry from the broadcast SMS service table in a
  specified subscription account.

  The result is returned to the user through the event callback function. The 
  command status is returned through the command callback function.

  By using wms_bc_mm_get_all_service_ids() with this function, the table entries 
  can be retrieved one by one.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_bc_mm_srv_id_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] srv_id_ptr     Pointer to the service ID for the table entry to be 
                            retrieved.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @newpage
  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_BAD_SRV_ID

  @events
  WMS_BC_MM_EVENT_SRV_INFO

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_get_service_info
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  const wms_bc_mm_srv_id_type     *srv_id_ptr
);
/*~ FUNCTION wms_bc_ms_get_service_info 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN srv_id_ptr POINTER */ 

/*=========================================================================
FUNCTION  wms_bc_ms_add_services
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_add_services
  Adds a list of table entries to the broadcast SMS service table in a
  specified subscription account.
 
  If the enabling status of the broadcast SMS is changed due to the new 
  preference, an enable/disable command is sent to the lower layers.

  For GSM SMS CB, the service IDs of the selected entries are written to the 
  SIM's EF-CBMI and EF-CBMIR.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_bc_mm_service_info_s

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] num_entries    Number of entries to be added.
  @param[in] entries        Pointer to the list of table entries to be added.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_TABLE_FULL \n
  WMS_CMD_ERR_BC_DUPLICATE_SRV \n
  WMS_CMD_ERR_BC_NV_WRITE \n
  WMS_CMD_ERR_NULL_PTR \n
  WMS_CMD_ERR_BC_SIM_WRITE \n
  WMS_CMD_ERR_NO_RESOURCE 

  @events
  WMS_BC_MM_EVENT_ADD_SRVS 

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_add_services
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  uint8                           num_entries,
  const wms_bc_mm_service_info_s_type   *entries
);
/*~ FUNCTION wms_bc_ms_add_services 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN entries VARRAY LENGTH num_entries */ 

/*=========================================================================
FUNCTION  wms_bc_ms_delete_services
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_delete_services
  Deletes a list of table entries from the broadcast SMS service table in a
  specified subscription account.
 
  If the enabling status of broadcast SMS is changed due to the new preference, 
  an enable/disable command is sent to the lower layers.

  For GSM SMS CB, the service IDs are also removed from the SIM's EF-CBMI and 
  EF-CBMIR.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_bc_mm_srv_id_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] num_entries    Number of entries to be added.
  @param[in] srv_ids        Pointer to the list of service IDs to be removed 
                            from the table.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @newpage
  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_BAD_SRV_ID \n
  WMS_CMD_ERR_BC_NV_WRITE \n
  WMS_CMD_ERR_NULL_PTR \n
  WMS_CMD_ERR_BC_SIM_WRITE \n
  WMS_CMD_ERR_NO_RESOURCE 

  @events
  WMS_BC_MM_EVENT_DELETE_SRVS

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_delete_services
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  uint8                           num_entries,
  const wms_bc_mm_srv_id_type     *srv_ids
);
/*~ FUNCTION wms_bc_ms_delete_services 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN srv_ids VARRAY LENGTH num_entries */ 

/*=========================================================================
FUNCTION  wms_bc_ms_change_service_info
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_change_service_info
  Changes the information of an entry in the broadcast SMS service table, 
  including priority for CDMA, label, etc., in a specified subscription 
  account.

  For GSM SMS CB, the service IDs of the selected entries are written to the 
  SIM's EF-CBMI and EF-CBMIR.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_bc_mm_service_info_s

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] srv_info_ptr   Pointer to the new service table entry information.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @newpage
  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_BAD_SRV_ID \n
  WMS_CMD_ERR_BC_NV_WRITE \n
  WMS_CMD_ERR_BC_SIM_WRITE

  @events
  WMS_BC_MM_EVENT_SRV_UPDATED

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_change_service_info
(
  wms_client_id_type                  client_id,
  sys_modem_as_id_e_type              as_id,
  wms_cmd_cb_type                     cmd_cb,
  const void                          *user_data,
  wms_message_mode_e_type             message_mode,
  const wms_bc_mm_service_info_s_type *srv_info_ptr
);
/*~ FUNCTION wms_bc_ms_change_service_info 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN srv_info_ptr POINTER */ 

/*=========================================================================
FUNCTION  wms_bc_ms_delete_all_services
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_delete_all_services
  Deletes all entries from the broadcast SMS service table in a
  specified subscription account.

  For GSM SMS CB, all service IDs are removed from the SIM's EF-CBMI and 
  EF-CBMIR.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_NV_WRITE \n
  WMS_CMD_ERR_BC_SIM_WRITE 

  @newpage
  @events
  WMS_BC_MM_EVENT_DELETE_ALL_SRVS

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_delete_all_services
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_ms_delete_all_services 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_select_all_services
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_select_all_services
  Selects/deselects all entries from the broadcast SMS service table in a
  specified subscription account.

  For GSM SMS CB, the service IDs of the selected entries are written to the 
  SIM's EF-CBMI and EF-CBMIR.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] selected       Indicates whether all services are selected or 
                            deselected.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_NV_WRITE \n
  WMS_CMD_ERR_BC_SIM_WRITE \n 
  WMS_CMD_ERR_NO_RESOURCE

  @events
  WMS_BC_MM_EVENT_SELECT_ALL_SRVS

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_select_all_services
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         selected
);
/*~ FUNCTION wms_bc_ms_select_all_services 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_set_priority_for_all_services
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_set_priority_for_all_services
  Sets the priorities for all entries from the broadcast SMS service table
  in a specified subscription account. Priority levels are all changed to the 
  same level.

  @note1 This function is available for CDMA mode only, not for GSM/WCDMA mode.

  @datatypes
  #wms_message_mode_e_type \n
  #wms_priority_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Indicates CDMA Broadcast SMS or GSM SMS CB.
  @param[in] priority       Priority level for all services.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @newpage
  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_BC_NV_WRITE \n
  WMS_CMD_ERR_UNSUPPORTED \n
  WMS_CMD_ERR_NO_RESOURCE

  @events
  WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_set_priority_for_all_services
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  wms_priority_e_type             priority
);
/*~ FUNCTION wms_bc_ms_set_priority_for_all_services 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_msg_delete_indication
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_msg_delete_indication
  Notifies the WMS and lower layers that an SMS-CB Page message has been 
  deleted by the client in a specified subscription account.

  The WMS and lower layers then remove cached information for this message, 
  ensuring that the repeated message can be received again.

  @note1 This function is applicable to GSM/WCDMA mode only.

  @datatypes
  #wms_gw_cb_page_header_s

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] cb_page_header  Pointer to the CB page header buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S

  @dependencies
  None.

  @newpage
  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_UNSUPPORTED \n
  WMS_CMD_ERR_NO_RESOURCE

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_msg_delete_indication
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_gw_cb_page_header_s_type   *cb_page_header
);
/*~ FUNCTION wms_bc_ms_msg_delete_indication 
    ONERROR return WMS_RPC_ERROR_S */
/*~ PARAM IN cb_page_header POINTER */ 

/*=========================================================================
FUNCTION  wms_bc_ms_msg_delete_all_indication
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_msg_delete_all_indication
  Notifies the WMS and lower layers that all SMS-CB Page messages have been 
  deleted by the client in a specified subscription account.

  The WMS and lower layers then remove cached information for all CB messages, 
  ensuring that the repeated messages can be received again.

  @note1 This function is applicable to GSM/WCDMA mode only.

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_UNSUPPORTED \n
  WMS_CMD_ERR_NO_RESOURCE

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_msg_delete_all_indication
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data
);
/*~ FUNCTION wms_bc_ms_msg_delete_all_indication 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_get_reading_pref
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_get_reading_pref
  Retrieves the reading preferences for UMTS BMC (SMS cell broadcast in a 
  WCDMA system) in a specified subscription account.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Message mode (only WMS_MESSAGE_MODE_GW is supported).

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  FEATURE_UMTS_BMC must be turned on.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_UNSUPPORTED \n
  WMS_CMD_ERR_NO_RESOURCE

  @events
  WMS_BC_MM_EVENT_READING_PREF

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_get_reading_pref
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_ms_get_reading_pref 
    ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION  wms_bc_ms_set_reading_pref
===========================================================================*/
/** @ingroup mmbroadcast_group_wms_bc_ms_set_reading_pref
  Sets the reading preferences for UMTS BMC (SMS cell broadcast in a WCDMA 
  system) in a specified subscription account.

  The schedule message from the network sometimes indicates the reading 
  preferences for certain BMC messages that the network schedules to send at a 
  later time. The mobile can decide to wake up and receive those messages based 
  on these reading preferences.

  @datatypes
  #wms_message_mode_e_type

  @param[in] client_id      Client ID.
  @param[in] as_id          Active subscription ID.
  @param[in] cmd_cb         Command callback for reporting the command status; 
                            see #wms_cmd_cb_type.
  @param[in] user_data      Pointer provided by the client to uniquely identify 
                            this transaction of sending a message. The same 
                            pointer is passed to the client's callback function. 
                            A NULL pointer is acceptable.
  @param[in] message_mode   Message mode (only WMS_MESSAGE_MODE_GW is supported).
  @param[in] reading_advised   BMC reception status for Reading Advised messages.
  @param[in] reading_optional  BMC reception status for Reading Optional messages.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  FEATURE_UMTS_BMC must be turned on.

  @commandstatus
  WMS_CMD_ERR_NONE \n
  WMS_CMD_ERR_UNSUPPORTED \n
  WMS_CMD_ERR_NO_RESOURCE

  @events
  WMS_BC_MM_EVENT_READING_PREF

  @sideeffects
  Request is added to the request queue.
*/
wms_status_e_type wms_bc_ms_set_reading_pref
(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         reading_advised,
  boolean                         reading_optional
);
/*~ FUNCTION wms_bc_ms_set_reading_pref 
    ONERROR return WMS_RPC_ERROR_S */


/* ========================================================================= */
/* Merged from wmspm.h */
/* ========================================================================= */

/** @addtogroup client_pm_group_data_types
@{ */
/** Filter type.
*/
typedef enum
{
  WMS_PM_NOTIFY_NONE   = 0,     /**< Do not notify any events. */
  WMS_PM_NOTIFY_ALL,            /**< Notify events based on event masks. */
  WMS_PM_NOTIFY_DEFAULT_FILTER, /**< Notify events based on default filtering. */
  WMS_PM_NOTIFY_CUSTOM_FILTER,  /**< Notify events based on customized filtering. */
/** @cond */
  WMS_PM_NOTIFY_MAX
/** @endcond */
} wms_pm_notify_e_type;


/** Processor type.
*/
typedef enum
{
  WMS_CLIENT_PROCESSOR_NONE   = 0, /**< No processor. */
  WMS_CLIENT_PROCESSOR_MODEM,      /**< Modem processor. */
  WMS_CLIENT_PROCESSOR_APP1,       /**< Applications processor 1. */
  WMS_CLIENT_PROCESSOR_APP2,       /**< Applications processor 2. */
/** @cond */
  WMS_CLIENT_PROCESSOR_MAX
/** @endcond */
} wms_client_processor_e_type;
/** @} */ /* end_addtogroup client_pm_group_data_types */

/** @addtogroup client_pm_group_constants
@{ */
/* Configuration event masks. */
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_GW_READY          (1 << WMS_CFG_EVENT_GW_READY)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_CDMA_READY        (1 << WMS_CFG_EVENT_CDMA_READY)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_ROUTES            (1 << WMS_CFG_EVENT_ROUTES)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MEMORY_STATUS     (1 << WMS_CFG_EVENT_MEMORY_STATUS)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MESSAGE_LIST      (1 << WMS_CFG_EVENT_MESSAGE_LIST)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MEMORY_FULL       (1 << WMS_CFG_EVENT_MEMORY_FULL)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_GW_DOMAN_PREF     (1 << WMS_CFG_EVENT_GW_DOMAIN_PREF)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_CELL_CHANGE       (1 << WMS_CFG_EVENT_CELL_CHANGE)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_PRIMARY_CLIENT_SET (1 << WMS_CFG_EVENT_PRIMARY_CLIENT_SET)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MEMORY_STATUS_SET (1 << WMS_CFG_EVENT_MEMORY_STATUS_SET)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_LINK_CONTROL      (1 << WMS_CFG_EVENT_LINK_CONTROL)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_CB_ERROR          (1 << WMS_CFG_EVENT_CB_ERROR)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_READY          (1 << WMS_CFG_EVENT_MS_READY)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_MEMORY_FULL       (1 << WMS_CFG_EVENT_MS_MEMORY_FULL)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_GW_DOMAN_PREF     (1 << WMS_CFG_EVENT_MS_GW_DOMAIN_PREF)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_CELL_CHANGE       (1 << WMS_CFG_EVENT_MS_CELL_CHANGE)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_MEMORY_STATUS_SET (1 << WMS_CFG_EVENT_MS_MEMORY_STATUS_SET)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MSG_WAITING          (1 << WMS_CFG_EVENT_MSG_WAITING)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_NOT_READY         (1 << WMS_CFG_EVENT_MS_NOT_READY)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_SIM_INIT_START    (1 << WMS_CFG_EVENT_MS_SIM_INIT_START)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_SIM_INIT_END      (1 << WMS_CFG_EVENT_MS_SIM_INIT_END)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_REFRESH_START     (1 << WMS_CFG_EVENT_MS_REFRESH_START)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_REFRESH_END       (1 << WMS_CFG_EVENT_MS_REFRESH_END)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_SERV_AVAIL        (1 << WMS_CFG_EVENT_MS_SERV_AVAIL)
/** Configuration event mask.
*/
#define WMS_CFG_EVENT_MASK_MS_SERV_NOT_AVAIL    (1 << WMS_CFG_EVENT_MS_SERV_NOT_AVAIL)

/* MSG Event Masks */
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_SEND                 (1 << WMS_MSG_EVENT_SEND)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_ACK                  (1 << WMS_MSG_EVENT_ACK)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_READ                 (1 << WMS_MSG_EVENT_READ)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_WRITE                (1 << WMS_MSG_EVENT_WRITE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_DELETE               (1 << WMS_MSG_EVENT_DELETE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_DELETE_ALL           (1 << WMS_MSG_EVENT_DELETE_ALL)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_MODIFY_TAG           (1 << WMS_MSG_EVENT_MODIFY_TAG)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_READ_TEMPLATE        (1 << WMS_MSG_EVENT_READ_TEMPLATE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_WRITE_TEMPLATE       (1 << WMS_MSG_EVENT_WRITE_TEMPLATE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_DELETE_TEMPLATE      (1 << WMS_MSG_EVENT_DELETE_TEMPLATE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_DELETE_TEMPLATE_ALL  (1 << WMS_MSG_EVENT_DELETE_TEMPLATE_ALL)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_READ_STS_REPORT      (1 << WMS_MSG_EVENT_READ_STS_REPORT)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_WRITE_STS_REPORT     (1 << WMS_MSG_EVENT_WRITE_STS_REPORT)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_DELETE_STS_REPORT    (1 << WMS_MSG_EVENT_DELETE_STS_REPORT)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_DELETE_STS_REPORT_ALL (1 << WMS_MSG_EVENT_DELETE_STS_REPORT_ALL)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_SMSC_ADDR_CHANGE     (1 << WMS_MSG_EVENT_SMSC_ADDR_CHANGE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_RECEIVED_MESSAGE     (1 << WMS_MSG_EVENT_RECEIVED_MESSAGE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_SUBMIT_REPORT        (1 << WMS_MSG_EVENT_SUBMIT_REPORT)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_STATUS_REPORT        (1 << WMS_MSG_EVENT_STATUS_REPORT)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_MT_MESSAGE_ERROR     (1 << WMS_MSG_EVENT_MT_MESSAGE_ERROR)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_ACK_REPORT           (1 << WMS_MSG_EVENT_ACK_REPORT)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_DUPLICATE_CB_PAGE    (1 << WMS_MSG_EVENT_DUPLICATE_CB_PAGE)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_TRANSPORT_REG        (1 << WMS_MSG_EVENT_TRANSPORT_REG)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_ETWS_NOTIFICATION    (1 << WMS_MSG_EVENT_ETWS_NOTIFICATION)
/** Messaging event mask.
*/
#define WMS_MSG_EVENT_MASK_TRANSPORT_MWI_NOTIFICATION    (1 << WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION)

/* Dedicated Channel event masks. */
/** Dedicated channel event mask.
*/
#define WMS_DC_EVENT_MASK_INCOMING             (1 << WMS_DC_EVENT_INCOMING)
/** Dedicated channel event mask.
*/
#define WMS_DC_EVENT_MASK_CONNECTED            (1 << WMS_DC_EVENT_CONNECTED)
/** Dedicated channel event mask.
*/
#define WMS_DC_EVENT_MASK_ABORTED              (1 << WMS_DC_EVENT_ABORTED)
/** Dedicated channel event mask.
*/
#define WMS_DC_EVENT_MASK_DISCONNECTED         (1 << WMS_DC_EVENT_DISCONNECTED)
/** Dedicated channel event mask.
*/
#define WMS_DC_EVENT_MASK_CONNECTING           (1 << WMS_DC_EVENT_CONNECTING)
/** Dedicated channel event mask.
*/
#define WMS_DC_EVENT_MASK_ENABLE_AUTO_DISCONNECT  (1 << WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT)
/** Dedicated channel event mask.
*/
#define WMS_DC_EVENT_MASK_DISABLE_AUTO_DISCONNECT (1 << WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT)

/* Multimode Broadcast event masks. */
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_CONFIG          (1 << WMS_BC_MM_EVENT_CONFIG)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_PREF            (1 << WMS_BC_MM_EVENT_PREF)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_TABLE           (1 << WMS_BC_MM_EVENT_TABLE)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_SRV_IDS         (1 << WMS_BC_MM_EVENT_SRV_IDS)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_SRV_INFO        (1 << WMS_BC_MM_EVENT_SRV_INFO)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_SRV_UPDATED     (1 << WMS_BC_MM_EVENT_SRV_UPDATED)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_ADD_SRVS        (1 << WMS_BC_MM_EVENT_ADD_SRVS)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_DELETE_SRVS     (1 << WMS_BC_MM_EVENT_DELETE_SRVS)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_SELECT_ALL_SRVS (1 << WMS_BC_MM_EVENT_SELECT_ALL_SRVS)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_SET_PRIORITY_ALL_SRVS (1 << WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_READING_PREF    (1 << WMS_BC_MM_EVENT_READING_PREF)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_DELETE_ALL_SRVS (1 << WMS_BC_MM_EVENT_DELETE_ALL_SRVS)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_ENABLE_FAILURE  (1 << WMS_BC_MM_EVENT_ENABLE_FAILURE)
/** Multimode broadcast event mask. 
*/
#define WMS_BC_MM_EVENT_MASK_DISABLE_FAILURE (1 << WMS_BC_MM_EVENT_DISABLE_FAILURE)

/* Debug event masks. */
/** Debug event mask. 
*/
#define WMS_DBG_EVENT_MASK_RESET_TL_SEQ_NUM (1 << WMS_DBG_EVENT_RESET_TL_SEQ_NUM)
/** Debug event mask. 
*/
#define WMS_DBG_EVENT_MASK_SET_SIM_MESSAGE_NUMBER  (1 << WMS_DBG_EVENT_SET_SIM_MESSAGE_NUMBER)
/** Debug event mask. 
*/
#define WMS_DBG_EVENT_MASK_CLEAR_SMMA_FLAG  (1 << WMS_DBG_EVENT_CLEAR_SMMA_FLAG)
/** Debug event mask. 
*/
#define WMS_DBG_EVENT_MASK_RETRY_INTERVAL   (1 << WMS_DBG_EVENT_RETRY_INTERVAL)
/** Debug event mask. 
*/
#define WMS_DBG_EVENT_MASK_RETRY_PERIOD     (1 << WMS_DBG_EVENT_RETRY_PERIOD)
/** @} */ /* end_addtogroup client_pm_group_constants */


/*=========================================================================
FUNCTION  wms_client_init_for_pm
===========================================================================*/
/** @ingroup client_pm_group_wms_client_init_for_pm
  Allows a client to register itself with the API for the purpose of power 
  management.

  @datatypes
  #wms_client_type_e_type \n
  #wms_client_processor_e_type

  @param[in] client_type      Client type.
  @param[out] client_id_ptr   Pointer to the client ID.
  @param[in] processor        Processor type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_RPC

  @dependencies
  Undetermined.

  @sideeffects
  Internal data is initialized for the new client.
*/
wms_client_err_e_type wms_client_init_for_pm
(
  wms_client_type_e_type       client_type,
  wms_client_id_type           * client_id_ptr,
  wms_client_processor_e_type  processor
);
/*~ FUNCTION wms_client_init_for_pm 
    RELEASE_FUNC wms_client_release(*client_id_ptr)
    ONERROR return WMS_CLIENT_ERR_RPC */
/*~ PARAM INOUT client_id_ptr POINTER */

/*=========================================================================
FUNCTION  wms_client_reg_cfg_cb_for_pm
===========================================================================*/
/** @ingroup client_pm_group_wms_client_reg_cfg_cb_for_pm
  Allows a client to register its configuration event callback function with 
  event notification filtering preferences for the purpose of power management. 
  If a NULL pointer is passed, the callback is deregistered.

  @datatypes
  #wms_pm_notify_e_type 

  @param[in] client_id        Client ID.
  @param[in] cfg_notify_type  Filter type.
  @param[in] cfg_event_mask   Configuration event mask.
  @param[in] cfg_event_cb     Configuration event callback function pointer; 
                              see #wms_cfg_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_RPC

  @dependencies
  Undetermined.

  @sideeffects
  Internal data is updated.
*/

wms_client_err_e_type wms_client_reg_cfg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         cfg_notify_type,
  uint32                       cfg_event_mask,
  wms_cfg_event_cb_type        cfg_event_cb
);
/*~ FUNCTION wms_client_reg_cfg_cb_for_pm 
    RELEASE_FUNC wms_client_deactivate(client_id)
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_msg_cb_for_pm
===========================================================================*/
/** @ingroup client_pm_group_wms_client_reg_msg_cb_for_pm
  Allows a client to register its event callback function with event notification 
  filtering preferences for the purpose of power management. If a NULL pointer 
  is passed, the callback is deregistered.

  @datatypes
  #wms_pm_notify_e_type 

  @param[in] client_id        Client ID.
  @param[in] msg_notify_type  Filter type.
  @param[in] msg_event_mask   Message event mask.
  @param[in] msg_event_cb     Message event callback function pointer; 
                              see #wms_msg_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_RPC

  @dependencies
  Undetermined.

  @sideeffects
  Internal data is updated.
*/

wms_client_err_e_type wms_client_reg_msg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         msg_notify_type,
  uint32                       msg_event_mask,
  wms_msg_event_cb_type        msg_event_cb
);
/*~ FUNCTION wms_client_reg_msg_cb_for_pm 
    RELEASE_FUNC wms_client_deactivate(client_id)
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_dc_cb_for_pm
===========================================================================*/
/** @ingroup client_pm_group_wms_client_reg_dc_cb_for_pm
  Allows a client to register its event callback function with event notification 
  filtering preferences for the purpose of power management. If a NULL pointer 
  is passed, the callback is deregistered.

  @datatypes
  #wms_pm_notify_e_type 

  @param[in] client_id        Client ID.
  @param[in] dc_notify_type   Filter type.
  @param[in] dc_event_mask    Dedicated channel event mask.
  @param[in] dc_event_cb      Dedicated channel event callback function 
                              pointer; #wms_dc_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_RPC

  @dependencies
  Undetermined.

  @sideeffects
  Internal data is updated.
*/

wms_client_err_e_type wms_client_reg_dc_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dc_notify_type,
  uint32                       dc_event_mask,
  wms_dc_event_cb_type         dc_event_cb
);
/*~ FUNCTION wms_client_reg_dc_cb_for_pm 
    RELEASE_FUNC wms_client_deactivate(client_id)
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_bc_mm_cb_for_pm
===========================================================================*/
/** @ingroup client_pm_group_wms_client_reg_bc_mm_cb_for_pm
  Allows a client to register its event callback function with event notification 
  filtering preferences for the purpose of power management. If a NULL pointer 
  is passed, the callback is deregistered.

  @datatypes
  #wms_pm_notify_e_type 

  @param[in] client_id          Client ID.
  @param[in] bc_mm_notify_type  Filter type.
  @param[in] bc_mm_event_mask   Multimode broadcast event mask.
  @param[in] bc_mm_event_cb     Multimode broadcast event callback function 
                                pointer; see #wms_bc_mm_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_RPC

  @dependencies
  Undetermined.

  @sideeffects
  Internal data is updated.
*/

wms_client_err_e_type wms_client_reg_bc_mm_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         bc_mm_notify_type,
  uint32                       bc_mm_event_mask,
  wms_bc_mm_event_cb_type      bc_mm_event_cb
);
/*~ FUNCTION wms_client_reg_bc_mm_cb_for_pm 
    RELEASE_FUNC wms_client_deactivate(client_id)
    ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION  wms_client_reg_dbg_cb_for_pm
===========================================================================*/
/** @ingroup client_pm_group_wms_client_reg_dbg_cb_for_pm
  Allows a client to register its event callback function with event notification 
  filtering preferences for the purpose of power management. If a NULL pointer 
  is passed, the callback is deregistered.

  @datatypes
  #wms_pm_notify_e_type 

  @param[in] client_id        Client ID.
  @param[in] dbg_notify_type  Filter type.
  @param[in] dbg_event_mask   Debug event mask.
  @param[in] dbg_event_cb     Debug event callback function pointer; 
                              see #wms_dbg_event_cb_type.

  @return
  Client error code.
   - WMS_CLIENT_ERR_RPC

  @dependencies
  Undetermined.

  @sideeffects
  Internal data is updated.
*/

wms_client_err_e_type wms_client_reg_dbg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dbg_notify_type,
  uint32                       dbg_event_mask,
  wms_dbg_event_cb_type        dbg_event_cb
);
/*~ FUNCTION wms_client_reg_dbg_cb_for_pm 
    RELEASE_FUNC wms_client_deactivate(client_id)
    ONERROR return WMS_CLIENT_ERR_RPC */

boolean wms_msg_mo_sms_busy(
  sys_modem_as_id_e_type              as_id
);

sys_modem_as_id_e_type wms_get_default_as_id(
  void
);
/*~ FUNCTION wms_get_default_as_id 
    ONERROR return NOTHING */
/*~ PARAM IN NONE */ 


#define wms_cfg_set_routes( a, b, c, d ) wms_cfg_ms_set_routes( a, wms_get_default_as_id(), b, c, d )
#define wms_cfg_get_routes( a, b, c ) wms_cfg_ms_get_routes( a, wms_get_default_as_id(), b, c )
#define wms_cfg_get_memory_status( a, b, c, d, e ) wms_cfg_ms_get_memory_status( a, wms_get_default_as_id(), b, c, d, e )
#define wms_cfg_get_message_list( a, b, c, d, e ) wms_cfg_ms_get_message_list( a, wms_get_default_as_id(), b, c, d, e )
#define wms_cfg_set_gw_domain_pref( a, b, c, d ) wms_cfg_ms_set_gw_domain_pref( a, wms_get_default_as_id(), b, c, d )
#define wms_cfg_get_gw_domain_pref( a, b, c ) wms_cfg_ms_get_gw_domain_pref( a, wms_get_default_as_id(), b, c )
#define wms_cfg_set_link_control( a, b, c, d, e ) wms_cfg_ms_set_link_control( a, wms_get_default_as_id(), b, c, d, e )
#define wms_cfg_get_link_control( a, b, c ) wms_cfg_ms_get_link_control( a, wms_get_default_as_id(), b, c )
#define wms_cfg_set_primary_client( a, b, c, d, e ) wms_cfg_ms_set_primary_client( a, wms_get_default_as_id(), b, c, d, e )
#define wms_cfg_set_memory_full( a, b, c, d ) wms_cfg_ms_set_memory_full( a, wms_get_default_as_id(), b, c, d )
#define wms_msg_send( a, b, c, d, e ) wms_msg_ms_send( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_ack( a, b, c, d ) wms_msg_ms_ack( a, wms_get_default_as_id(), b, c, d )
#define wms_msg_read( a, b, c, d, e ) wms_msg_ms_read( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_write( a, b, c, d, e ) wms_msg_ms_write( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_delete( a, b, c, d, e ) wms_msg_ms_delete( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_delete_all( a, b, c, d, e ) wms_msg_ms_delete_all( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_modify_tag( a, b, c, d, e, f ) wms_msg_ms_modify_tag( a, wms_get_default_as_id(), b, c, d, e, f )
#define wms_msg_read_template( a, b, c, d, e ) wms_msg_ms_read_template( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_write_template( a, b, c, d, e ) wms_msg_ms_write_template( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_delete_template( a, b, c, d, e ) wms_msg_ms_delete_template( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_delete_template_all( a, b, c, d ) wms_msg_ms_delete_template_all( a, wms_get_default_as_id(), b, c, d )
#define wms_msg_read_sts_report( a, b, c, d, e ) wms_msg_ms_read_sts_report( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_write_sts_report( a, b, c, d, e ) wms_msg_ms_write_sts_report( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_delete_sts_report( a, b, c, d, e ) wms_msg_ms_delete_sts_report( a, wms_get_default_as_id(), b, c, d, e )
#define wms_msg_delete_sts_report_all( a, b, c, d ) wms_msg_ms_delete_sts_report_all( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_get_config( a, b, c, d ) wms_bc_ms_get_config( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_get_pref( a, b, c, d ) wms_bc_ms_get_pref( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_set_pref( a, b, c, d, e ) wms_bc_ms_set_pref( a, wms_get_default_as_id(), b, c, d, e )
#define wms_bc_mm_get_table( a, b, c, d ) wms_bc_ms_get_table( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_select_service( a, b, c, d, e, f ) wms_bc_ms_select_service( a, wms_get_default_as_id(), b, c, d, e, f )
#define wms_bc_mm_get_all_service_ids( a, b, c, d ) wms_bc_ms_get_all_service_ids( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_get_service_info( a, b, c, d, e ) wms_bc_ms_get_service_info( a, wms_get_default_as_id(), b, c, d, e )
#define wms_bc_mm_add_services( a, b, c, d, e, f ) wms_bc_ms_add_services( a, wms_get_default_as_id(), b, c, d, e, f )
#define wms_bc_mm_delete_services( a, b, c, d, e, f ) wms_bc_ms_delete_services( a, wms_get_default_as_id(), b, c, d, e, f )
#define wms_bc_mm_change_service_info( a, b, c, d, e ) wms_bc_ms_change_service_info( a, wms_get_default_as_id(), b, c, d, e )
#define wms_bc_mm_delete_all_services( a, b, c, d ) wms_bc_ms_delete_all_services( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_select_all_services( a, b, c, d , e) wms_bc_ms_select_all_services( a, wms_get_default_as_id(), b, c, d, e )
#define wms_bc_mm_set_priority_for_all_services( a, b, c, d, e ) wms_bc_ms_set_priority_for_all_services( a, wms_get_default_as_id(), b, c, d, e )
#define wms_bc_mm_msg_delete_indication( a, b, c, d ) wms_bc_ms_msg_delete_indication( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_msg_delete_all_indication( a, b, c ) wms_bc_ms_msg_delete_all_indication( a, wms_get_default_as_id(), b, c )
#define wms_bc_mm_get_reading_pref( a, b, c, d ) wms_bc_ms_get_reading_pref( a, wms_get_default_as_id(), b, c, d )
#define wms_bc_mm_set_reading_pref( a, b, c, d, e, f ) wms_bc_ms_set_reading_pref( a, wms_get_default_as_id(), b, c, d, e, f )


#ifdef __cplusplus
}
#endif

#endif /* WMS_H */
