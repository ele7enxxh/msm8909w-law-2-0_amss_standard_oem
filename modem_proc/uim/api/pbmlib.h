#ifndef PBMLIB_H
#define PBMLIB_H

/*===========================================================================
           INTERNAL HEADER FILE FOR PHONEBOOK MANAGER LIBRARY
===========================================================================*/

/**
@file pbmlib.h
@brief This is the header file for the Phone Book Manager
       library.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The pbm_MainPage.dox file contains all file/group descriptions that are
      in the output PDF generated using Doxygen and LaTeX. To edit or update
      any of the file/group text in the PDF, edit the pbm_MainPage.dox file or
      contact Tech Pubs.

      The above description for this file is part of the "pbmlib" group
      description in the pbm_MainPage.dox file.
===========================================================================*/
/*-------------------------------------------------------------------------------------
     Copyright (c) 2002 - 2014,2016 QUALCOMM Technologies, Inc(QTI).
     All rights reserved.
     QUALCOMM Technologies Confidential and Proprietary
-------------------------------------------------------------------------------------*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/pbmlib.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/16   nr      NAL China emergency requirement
08/27/14   stv     Return card error correctly when queried for by clients 
01/21/14   NR      PBM/QMI_PBM valid_pb_mask is not resetting after REFRESH EVENT
03/30/11   sw      (Tech Pubs) Updated to latest Doxygen version/standards; edited
                   all text.
01/19/11   pk      Changes for the Master Slave activity for Fusion targets. 
                   Making the 7K target in Fusion as Slave and 9K as Master.
12/01/10   pk      Added the new APIs for concurrent access to record read by clients.
12/02/10   mpa     Fixed compiler warning
10/25/10   pk      Added the comments for numbering the enum values.
08/31/10   lo      (Tech Pubs) Edited/added Doxygen comments and markup.
06/24/10   kcp     Added new PB for EF-GAS & Hiddenkey support
06/06/10   kcp     Added new session based ecc API - CR 225730
03/18/10   kcp     Resolved the compilation errors in WM7
10/08/09   kcp     Support for DSDS Feature
07/10/09   ak      Support for CSIM and Dual Sim Features
05/04/09   kcp     Changes for CR 167785
12/26/08   kcp     adding changes for ASUS CR 165600
03/14/08   clm     Add PBM_EVENT_SIM_INIT_DONE event.
11/07/07   clm     Add PBM_FIELD_TECH_TYPE and PBM_FIELD_FIELDID_PRIMARY_PHONE
03/16/07   cvs     Add support for error category
02/09/07   cvs     Support for 4 phonebook sets
01/16/07   cvs     Add event for phonebooks becoming ready
11/01/06   cvs     multi pb set fixes
10/03/06   cvs     Add support for MBN
04/05/06   cvs     Emergency category priority fixes
03/21/06   cvs     Add function to get extended phonebook info
03/06/06   cvs     Add new fields
02/28/06   cvs     Add support for MBDN
01/19/06   cvs     Add pbm_emergency_number_cat(), cleanup emergency nums
11/16/05   cvs     Lint fixes
11/10/05   cvs     Add network ECC numbers
10/18/05   cvs     Add new fields
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
08/12/05   AT      Added UCS2 support for strings.
08/01/05   cvs     rename field
07/08/05   cvs     Add new fields
06/29/05   AT      Added error codes, fixed typos, moved ECC field IDs to
                   upper numbers.
06/03/05   cvs     Multiproc cleanup, add metacomments
05/26/05   cvs     API changes to make APIs more multiprocessor friendly
04/08/05   cvs     Add fields
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements.
03/23/05   cvs     compilation fixes
03/08/05   PA      Support for PBM notifications
01/20/05   PA      Support for multiple lock types, greater than enumeration,
                   support for MSISDN.
01/24/05   PA      Changed NV_EMERGENCY_NUMBER_START to
                   PBM_NV_EMERGENCY_NUMBER_START
01/20/05   PA      Expose definitions required by ui_base
01/12/05   PA      PBM USIM Support
01/10/05   PA      Added distribution list field.
01/10/05   PA      Added definition for ui_emergency_number
12/16/04   SUN     Added a new PBM field
12/15/04   SUN     Added new Address Fields
12/08/04   PA      Added PBM_FIELD_DIAL_STRING field.
12/03/04   PA      PBM record validation APIs
11/18/04   PA      Support for complex fields and write lock
11/02/04   PA      Support for SDN
===========================================================================*/

#include "cm.h" /* For CM_TON_* and CM_NPI_* */
#include "nv.h" /* For NV_MAX_DIAL_DIGITS */

//#include "pbm_common_types.h"


/** @addtogroup pbmlib
    @{ */

#define PBM_HARDPAUSE_CHAR      'H'
  /**< Character in the dial string used to indicate a hard pause.

@note1hang On a UIM card, this is converted to a TIMEPAUSE character before saving. */

#define PBM_TIMEPAUSE_CHAR      'T'
  /**< Character in the dial string used to indicate a time pause. */

#define PBM_WILD_CHAR           '?'
  /**< Character in the dial string used to indicate a wildcard character. */

#define PBM_EXPANSION_CHAR      'e'
  /**< Character in the dial string used to indicate an expansion character. */

#define PBM_MAX_NUM_LENGTH                40
  /**< Maximum length of the phone number. */

#define PBM_MAX_NUM_LENGTH_NO_EXT_RECORD  20
  /**< Maximum number length if there is no extension record available. */

#define PBM_MAX_TEXT_LENGTH               255
  /**< Maximum text length of the name. */

#define PBM_GET_FIRST_AVAILABLE 0xFFFF
  /**< Get the first available location. */

#define PBM_SESSION_GET_FIRST_AVAILABLE 0xFFFFFFFF
  /**< Get the first available session-based location. */

#define PBM_INVALID_REC_ID      0x0000
  /**< Invalid record ID. */

/* One-to-one mapping with ADDRREC values. */

/** @name Search Mode Bitmasks
  Flag bitmasks used to support various search modes.
    @{ */

#define PBM_CASE_MASK                    0x1   /**< Indicates that the search
                                                    criteria is based on case. */
#define PBM_CASE_INSENSITIVE             0x0   /**< Search criteria is case
                                                    insensitive. */
#define PBM_CASE_SENSITIVE               0x1   /**< Search criteria is case
                                                    sensitive. */

#define PBM_SEARCHTYPE_MASK              0x6   /**< Indicates that the search
                                                    criteria is based on a text
                                                    match. */
#define PBM_SEARCHTYPE_STARTSWITH        0x0   /**< Search criteria starts with
                                                    a specific character. */
#define PBM_SEARCHTYPE_EXACT             0x2   /**< Search criteria matches the
                                                    exact text. */
#define PBM_SEARCHTYPE_CONTAINS          0x4   /**< Search criteria contains
                                                    certain characters. */

#define PBM_DATATYPE_MASK                0x8   /**< Indicates that the search
                                                    criteria is based on a data
                                                    type. */
#define PBM_DATATYPE_STR                 0x0   /**< Search for a string. */
#define PBM_DATATYPE_BYTE                0x8   /**< Search for a byte. */

#define PBM_FIELDID_MASK                 0x10  /**< Indicates that the search
                                                    criteria is based on a field
                                                    ID. */
#define PBM_FIELDID_NONE                 0x00  /**< Search for a field ID that
                                                    equals None. */
#define PBM_FIELDID_PHONE                0x10  /**< Search for a field ID of the
                                                    Number type. */

#define PBM_CALLHISTORY_MASK             0x20  /**< Indicates the search criteria
                                                    that is used to search in the
                                                    call history. */
#define PBM_CALLHISTORY_ONE              0x00  /**< Search for a single call history
                                                    entry. */
#define PBM_CALLHISTORY_ALL              0x20  /**< Search for all call history
                                                    entries. */

#define PBM_ENUMERATE_MASK               0x40  /**< Indicates whether the search
                                                    criteria is to enumerate all. */
#define PBM_ENUMERATE_ALL                0x00  /**< Search criteria is to enumerate
                                                    all. */
#define PBM_ENUMERATE_AVAILABLE          0x40  /**< Search criteria is to enumerate
                                                    only available records.  */

#define PBM_MATCHING_MASK                0x80  /**< Currently no significance.
                                                    Reserved for future use.*/
#define PBM_MATCHING_NORMAL              0x00  /**< Currently no significance.
                                                    Reserved for future use.*/
#define PBM_MATCHING_GREATER             0x80  /**< Currently no significance.
                                                    Reserved for future use.*/

#define PBM_SORT_ORDER_MASK              0x100 /**< Indicates whether search results
                                                    are to be sorted. */
#define PBM_SORT_LASTNAME_FIRST          0x000 /**< Search for a name with the
                                                    last name first in the string. */
#define PBM_SORT_LASTNAME_SECOND         0x100 /**< Search for a name with the
                                                    last name second in the
                                                    string. */

#define PBM_STRTYPE_MASK                 0x200 /**< Indicates the type of search
                                                    data. */
#define PBM_STRTYPE_UCS2                 0x200 /**< Search for a universal
                                                    character set (2-byte) string. */
#define PBM_STRTYPE_PBM8                 0x000 /**< Search for a phone book
                                                    manager string. */

/** @} */ /* end_name */

#define PBM_TEXT_SIZE_CHARS (PBM_MAX_TEXT_LENGTH+1)
  /**< NULL-terminated string. */

#define PBM_TEXT_SIZE_BYTES (PBM_TEXT_SIZE_CHARS*sizeof(uint16))
  /**< Text size in bytes. */

#define PBM_NUM_SIZE       (PBM_MAX_NUM_LENGTH+2)
  /**< Size in bytes; null for the string. */

#define PBM_WORD_SIZE      2
  /**< Word: 16 bits. */

#define PBM_DWORD_SIZE     4
  /**< Double word: 32 bits. */

#define PBM_DDWORD_SIZE    8
  /**< Double-double word: 64 bits. */

#define PBM_BYTE_SIZE      1
  /**< Byte size. */

/* Status values within NV_DIAL_I. */

#define NV_NON_SECRET_SD_NUM 0
  /**< Normal (non-secret) speed dial number. */

#define NV_SECRET_SD_NUM     1
  /**< Secret speed dial number. */

#define NV_PRIORITY_SD_NUM   2
  /**< Priority speed dial number. */

#define NV_EMERGENCY_SD_NUM  3
  /**< Emergency speed dial number. */

#define NV_OTASP_SD_NUM      4
  /**< OTASP speed dial number. */

#define NV_OTASP2_SD_NUM     6
  /**< OTASP-2 (customer-specific) speed dial number. */

#define PBM_NV_EMERGENCY_NUMBER_START 99
  /**< Start of emergency numbers within NV_DIAL_I (an NV element stored the NV
       file system). */

#define PBM_NV_EMERGENCY_NUMBERS 10
  /**< Maximum number of emergency numbers within NV_DIAL_I. */

/*=============================================================================
* Record ID data types used within PBM
=============================================================================*/

#define pbm_adn_record_id_type   uint16
  /**< Abbreviated dialing number record ID data type. */

#define pbm_record_id_type       uint32
  /**< Record ID data type. */

/*============================================================================
   STRUCTURE:      PBM_UIM_APP_ENUM_TYPE
=============================================================================*/

/**
  Specifies the phone book applications the PBM maintains on the phone in
  different active slots. These can be Local Phone Book (LPB) or Global Phone
  Book (GPB) applications.
*/
typedef enum
{
   PBM_UIM_APP_DEFAULT = 0,
     /**< Default is the same as PBM_UIM_APP_GPB_SLOT_1. */
   PBM_UIM_APP_GPB_SLOT_1 = PBM_UIM_APP_DEFAULT,
     /**< Represents GPB on slot 1. */
   PBM_UIM_APP_GPB_SLOT_2,
     /**< Represents GPB on slot 2. */
   PBM_UIM_APP_LPB_GW_PRIMARY,
     /**< Represents the active GSM/USIM application on slot 1. */
   PBM_UIM_APP_LPB_GW_PROV = PBM_UIM_APP_LPB_GW_PRIMARY,
     /**< Represents the active GSM/USIM application on slot 1. */
   PBM_UIM_APP_LPB_1X_PRIMARY,
     /**< Represents the active 1X/CSIM application on slot 1. */
   PBM_UIM_APP_LPB_1X_PROV = PBM_UIM_APP_LPB_1X_PRIMARY,
     /**< Represents the active 1X/CSIM application on slot 1. */
   PBM_UIM_APP_LPB_GW_SECONDARY,
     /**< Represents the active GSM/USIM application on slot 2. */
   PBM_UIM_APP_LPB_1X_SECONDARY,
     /**< Represents the active 1X/CSIM application on slot 2. */
   PBM_UIM_APP_GPB_SLOT_3,
     /**< Represents GPB on slot 3. */
   PBM_UIM_APP_LPB_GW_TERTIARY,
     /**< Represents the active GSM/USIM application on slot 3. */
   PBM_UIM_APP_LPB_1X_TERTIARY,
     /**< Represents the active 1X/CSIM application on slot 3. */
   PBM_UIM_APP_MAX,
     /**< Invalid application. */
   MAX_UIM_APPS = PBM_UIM_APP_MAX
     /**< Invalid application. */
} pbm_uim_app_enum_type;

/*============================================================================
   STRUCTURE:      PBM_SESSION_TYPE
=============================================================================*/
/**
  Specifies the PBM session type that represents all the MMGSDI sessions opened
  for maintaining various phone books (LPB and GPB) in different active slots.
*/
typedef enum
{
   PBM_SESSION_DEFAULT = PBM_UIM_APP_DEFAULT,
     /**< Default is the same as PBM_UIM_APP_GPB_SLOT_1. */
   PBM_SESSION_GPB_1 = PBM_UIM_APP_GPB_SLOT_1,
     /**< Represents the GPB on slot 1. */
   PBM_SESSION_GPB_2 = PBM_UIM_APP_GPB_SLOT_2,
     /**< Represents the GPB on slot 2. */
   PBM_SESSION_LPB_GW = PBM_UIM_APP_LPB_GW_PRIMARY,
     /**< Represents the active GSM/USIM application on slot 1. */
   PBM_SESSION_LPB_1X = PBM_UIM_APP_LPB_1X_PRIMARY,
     /**< Represents the active 1X/CSIM application on slot 1. */
   PBM_SESSION_LPB_GW_SECONDARY = PBM_UIM_APP_LPB_GW_SECONDARY,
     /**< Represents the active GSM/USIM application on slot 2. */
   PBM_SESSION_LPB_1X_SECONDARY = PBM_UIM_APP_LPB_1X_SECONDARY,
     /**< Represents the active 1X/CSIM application on slot 2. */
   PBM_SESSION_GPB_3,
     /**< Represents GPB on slot 3. */
   PBM_SESSION_LPB_GW_TERTIARY,
     /**< Represents the active GSM/USIM application on slot 3. */
   PBM_SESSION_LPB_1X_TERTIARY,
     /**< Represents the active 1X/CSIM application on slot 3. */
   PBM_SESSION_MAX = PBM_UIM_APP_MAX
     /**< Invalid session. */
} pbm_session_enum_type;

/*============================================================================
   STRUCTURE:      PBM_PROVISION_ENUM_TYPE
=============================================================================*/
/**
  Indicates which active provision type is in use. It points to a local
  phone book of the provider application (1X or GW).
*/
typedef enum
{
   PBM_PROVISION_NONE,                           /**< No provider. */
   PBM_PROVISION_GW_PRIMARY,                     /**< Primary provider application
                                                      is GSM WCDMA. */
   PBM_PROVISION_GW = PBM_PROVISION_GW_PRIMARY,  /**< Primary provider application
                                                      is the same as
                                                      PBM_PROVISION_GW_PRIMARY. */
   PBM_PROVISION_1X_PRIMARY,                     /**< Primary provider application
                                                      is 1X. */
   PBM_PROVISION_1X = PBM_PROVISION_1X_PRIMARY,  /**< Primary provider application
                                                      is the same as
                                                      PBM_PROVISION_1X_PRIMARY. */
   PBM_PROVISION_GW_SECONDARY,                   /**< Secondary provider application
                                                      is GSM WCDMA. */
   PBM_PROVISION_1X_SECONDARY,                   /**< Secondary provider application
                                                      is 1X. */
   PBM_PROVISION_GW_TERTIARY,                    /**< Secondary provider application
                                                      is GSM WCDMA. */
   PBM_PROVISION_1X_TERTIARY,                    /**< Secondary provider application
                                                      is 1X. */
   PBM_PROVISION_UNKNOWN                         /**< Provider is unknown. */
}pbm_provision_enum_type;

/*============================================================================
   STRUCTURE:      PBM_PB_CATEGORY_ENUM_TYPE
=============================================================================*/
/**
   Used to specify a phone book category (LPB or GPB).
*/
typedef enum
{
   PBM_GPB,                   /**< Category is Global Phone Book. */
   PBM_LPB,                   /**< Category is Local Phone Book. */
}pbm_pb_category_enum_type;

/*============================================================================
   STRUCTURE:      PBM_RETURN_TYPE
=============================================================================*/

/** Return values supported from PBM functions.
*/
typedef enum
{
   PBM_SUCCESS,            /**< Call to the PBM was successful. */
   PBM_EOF,                /**< When searching, the last entry (end of file)
                                was already returned. */
   PBM_ERROR_PH_SIM_REQ,   /**< Not used. Reserved for future use.*/
   PBM_ERROR_NO_SIM,       /**< Denotes that the SIM is not present. */
   PBM_ERROR_SIM_PIN_REQ,  /**< SIM PIN required. */

   /* 5 */
   PBM_ERROR_SIM_PUC_REQ,  /**< Universal PIN is required. */
   PBM_ERROR_SIM_FAIL,     /**< SIM is unusable. */
   PBM_ERROR_PIN2_REQ,     /**< SIM PIN2 is required. */
   PBM_ERROR_PUC2_REQ,     /**< SIM PUC2 is required. */

   PBM_ERROR_MEM_FULL,
    /**< An entry cannot be stored due to a limit being reached. The
         limit may be one of the following:
         - There is no free entry in the phone book.
         - The limit of reserved cache space has been reached.
         - The UIM record requires an extension record and one
           is not available.
         - The PBM heap is exhausted.  */

   /* 10 */
   PBM_ERROR_INDEX_INVALID,    /**< Index parameter is out of range for the
                                  specified phone book. */

   PBM_ERROR_NOT_FOUND,        /**< Search was performed, but no records match
                              the criteria. */

   PBM_ERROR_TEXT_TOO_LONG,    /**< Text is too long in the context. */
   PBM_ERROR_NUMBER_TOO_LONG,  /**< Number is too long in the context. */
   PBM_ERROR_INVALID_CHAR,     /**< Not used. Reserved for future use.*/

   /* 15 */
   PBM_ERROR_NOT_READY,
     /**< Specified phone book has not been initialized. This may
          be because:
          - There is an error with the EFS or UIM.
          - The associated UIM is not installed.
          - The UIM Pin1 (CHV1) was not entered.
          - The corresponding EF does not exist on the card. */


   PBM_ERROR_SYNC,                /**< Phonebook synchronization is in progress. */

   PBM_ERROR,                     /**< An error other than one of the above errors
                                       has occurred. */

   PBM_ERROR_NOT_AVAILABLE,       /**< Requested phonebook is not available. */
   PBM_ERROR_RESTRICTED,          /**< Requested phonebook is restricted. */

   /* 20 */
   PBM_ERROR_NOT_SUPPORTED,       /**< Requested operation is not supported. */
   PBM_ERROR_LOCKED,              /**< Record is locked. */
   PBM_ERROR_INVALID_REC,         /**< Record is invalid. */
   PBM_ERROR_UNIQUE_IDS_CHANGED,  /**< Unique ID changes for every update.
                                       Hence, the record is already updated. */
   PBM_ERROR_CAT_NOT_SUPPORTED,   /**< Requested category is not supported. */

   /* 25 */
   PBM_ERROR_INVALID_ENUM_FIELD,  /**< Not used. Reserved for future use.*/
   PBM_ERROR_SIM_MEMORY_PROBLEM,  /**< SIM write failed due to memory problem.*/
   PBM_ERROR_MAX                  /**< Sentinal. Last enumeration. */

   /* 26  Please update this number accordingly if new error type is added */
} pbm_return_type;

typedef enum
{
  NOT_EMERGENCY = -1,
  EMERGENCY_GW,
  EMERGENCY_1X,
  EMERGENCY_GW_1X
}pbm_ecc_mode_type;


/** Denotes the return values supported from the PBM API. */
typedef pbm_return_type pbm_return_T;  // QCT standard for "new" interface

/** Enum used to represent the PBM slot number.
*/
typedef enum
{
   // Earlier, the value of PBM_FIRST_SLOT was 0, but since MMGSDI_SLOT_1 was 1
   // changed in PBM also to 1 to be more sync

   PBM_FIRST_SLOT = 1,           /**< Represents slot 1. */
   PBM_SLOT_1 = PBM_FIRST_SLOT,  /**< First PBM slot is number 1. */
   PBM_SLOT_2,                   /**< Represents slot 2. */
   PBM_SLOT_3,                   /**< Represents slot 3. */
   PBM_NUM_SLOTS                 /**< Sentinal. Last enumeration. */
} pbm_slot_type;

/* From table 10.81 of 3GPP "Mobile radio interface layer 3 * specification" */

/*===========================================================================
STRUCTURE: PBM_RECORD_S_TYPE
===========================================================================*/
/**
  @brief Communicates the contents of a record entry to and from the API. This
  is one of the main PBM structures.
*/
typedef struct
{
   int     index;
    /**< Logical index across multiple phone book sets. */

   char    number[PBM_NUM_SIZE];
    /**< Phone number, which has an additional character if the number
         starts with a + and another for the string NULL termination. */

   uint8   type;
    /**< Type of address octet, in integer format. */

   char    text[PBM_TEXT_SIZE_CHARS];
    /**< Name associated with this record as a null-terminated string. */

    /* Following fields added to support USIM */

   uint32  time;
    /**< Date and time of the call in clock seconds. This is used for the call
         history. */

   uint32  duration;
    /**< Duration of the call in seconds. The USIM supports up to 0x00FFFFFF. */

   boolean is_answered;
    /**< For incoming call, distinguishes between answered and missed calls.
         - TRUE -- Answered
         - FALSE -- Not answered */

} pbm_record_s_type;
/*~ FIELD pbm_record_s_type.text STRING */

/** Denotes the available phone books.
*/
typedef enum
{
   PBM_DEFAULT = 0,         /**< Enumeration of ADN, FDN, and EFS. */
   PBM_ADN,                 /**< Abbreviated dialing number list
                                 (SIM/USIM phone book). */
   PBM_DS_SM = PBM_ADN,     /**< Data services SIM/USIM phone book. */
   PBM_DS_MT,               /**< Data services combined NV/SIM phone book. */
   PBM_FDN,                 /**< Fixed dialing number list. */
   PBM_DS_FD = PBM_FDN,     /**< Data services fixed dialing number list. */
   PBM_BDN,                 /**< Barred dialing number list. */

   /* 5 */
   PBM_ECC,                 /**< Emergency call code (emergency number list). */
   PBM_DS_EN = PBM_ECC,     /**< Data services emergency number list. */
   PBM_DIALED,              /**< Last number dialed list. */
   PBM_DS_LD = PBM_DIALED,  /**< Data services last dialed number list. */
   PBM_RCVD,                /**< Received call list from NV. */
   PBM_DS_RC = PBM_RCVD,    /**< Data services received call list. */
   PBM_MISS,                /**< Missed call list from NV.  */
   PBM_DS_MS = PBM_MISS,    /**< Data services missed call list. */
   PBM_EFS,                 /**< Embedded file system phone book. */
   PBM_DS_ME = PBM_EFS,     /**< Data services ME phone book. */

   /* 10 */
   PBM_SDL,                 /**< Speed dial list. */
   PBM_LND,                 /**< SIM/RUIM last number dialed list. */
   PBM_OCI,                 /**< USIM outgoing call information file.  */
   PBM_ICI,                 /**< USIM incoming call information file.  */
   PBM_SDN,                 /**< Service dialing number. */

   /* 15 */
   PBM_MSISDN,              /**< Mobile station integrated services digital network. */
   PBM_MBDN,                /**< Mailbox dialing number. */
   PBM_MBN,                 /**< Common PCN Handset Specification (CPHS)
                                 mailbox number. */

                            /* SIM 2 filenames start here. */
   /* Below enums are kept for backward compatabilty to avoid compilation errors.
        PBM_ADN2 PBM_FDN2 PBM_BDN2 PBM_LND2 PBM_OCI2 PBM_ICI2 PBM_SDN2 PBM_MSISDN2 PBM_MBDN2 PBM_MBN2 PBM_ECC2
        Hence please do not use them
        Instead please use the appropriate session based API to access the phonebooks those belong to Slot2*/

   PBM_ADN2,        /**< Not supported. */
   PBM_FDN2,        /**< Not supported. */

   /* 20 */
   PBM_BDN2,        /**< Not supported. */
   PBM_LND2,        /**< Not supported. */
   PBM_OCI2,        /**< Not supported. */
   PBM_ICI2,        /**< Not supported. */
   PBM_SDN2,        /**< Not supported. */

   /* 25 */
   PBM_MSISDN2,     /**< Not supported. */
   PBM_MBDN2,       /**< Not supported. */
   PBM_MBN2,        /**< Not supported. */
   PBM_ECC2,        /**< Not supported. */
   PBM_ECC_OTHER,   /**< Emergency call codes for NV, network, and hardcoded. */
   PBM_GRPNAME,     /**< Group name stored on the USIM card. */
   PBM_AAS,         /**< Additional Alpha name stored on the USIM card. */
   PBM_DEVICE_TYPE_ALL, /* Device type used to clear the global data in QMI-PBM when refresh or subscription read receieved from mmgsdi */
   PBM_RESERVED     /**< SENTINEL; it is not used. */
} pbm_device_type;
/*~ SENTINEL pbm_device_type.PBM_RESERVED */

/**
  @brief Represents a session-based phone book. Each member in this structure
  helps to identify the session-based phone book.
*/
typedef struct
{
   pbm_pb_category_enum_type pb_category; /**< Denotes whether the phone book is
                                               local or global. */
   pbm_provision_enum_type prov_type;     /**< Denotes the provision type. */
   uint16 slot_id;                        /**< Denotes the slot ID. */
   pbm_device_type device_type;           /**< Denotes the device type. */
} pbm_phonebook_type;

/** Denotes the category of the phone book entry.
*/
typedef enum
{

   PBM_CAT_NONE = 0, /**< Entry stored in the EFS phone book. The category is
                          saved and returned, but interpretation is up to the
                          application. */

   PBM_CAT_PERSONAL, /**< Entry stored in the EFS phone book. The category is
                          saved and returned, but interpretation is up to the
                          application. */

   PBM_CAT_BUSINESS, /**< Used only with the advanced API; indicates an entry
                          stored in the EFS phone book. The category is saved
                          and returned, but interpretation is up to the
                          application. */

   PBM_CAT_USER_DEFINED = 0xFF00,  /**< Not used.Reserved for future use.*/
   PBM_CAT_SIM_ADN,      /**< Entry is stored in the UIM ADN phone book in slot 1. */
   PBM_CAT_SIM_FDN,      /**< Entry is stored in the UIM FDN phone book in slot 1. */
   PBM_CAT_SIM_BDN,      /**< Entry is stored in the UIM BDN phone book in slot 1. */
   PBM_CAT_SDL,          /**< Entry is stored in the EFS speed dial phone book. */
   PBM_CAT_INCOMING,     /**< Entry is stored in the EFS incoming calls phone book. */
   PBM_CAT_OUTGOING,     /**< Entry is stored in the EFS outgoing calls phone book. */
   PBM_CAT_MISSED,       /**< Entry is stored in the EFS missed calls phone book. */
   PBM_CAT_SIM_ADN2,     /**< Entry is stored in the UIM ADN phone book in slot 2. */
   PBM_CAT_SIM_FDN2,     /**< Entry is stored in the UIM FDN phone book in slot 2. */
   PBM_CAT_SIM_BDN2,     /**< Entry is stored in the UIM BDN phone book in slot 2. */
   PBM_CAT_SIM_SDN,      /**< Entry is stored in the UIM SDN phone book in slot 1.  */
   PBM_CAT_SIM_SDN2,     /**< Entry is stored in the UIM SDN phone book in slot 2. */
   PBM_CAT_SIM_MSISDN,   /**< Entry is stored in the UIM MSISDN phone book in slot 1. */
   PBM_CAT_SIM_MSISDN2,  /**< Entry is stored in the UIM MSISDN phone book in slot 2. */
   PBM_CAT_SIM_MBDN,     /**< Entry is stored in the UIM MBDN phone book in slot 1. */
   PBM_CAT_SIM_MBDN2,    /**< Entry is stored in the UIM MBDN phone book in slot 2. */
   PBM_CAT_SIM_MBN,      /**< Entry is stored in the UIM MBN phone book in slot 1. */
   PBM_CAT_SIM_MBN2,     /**< Entry is stored in the UIM MBN phone book in slot 2. */
   PBM_CAT_ECC,          /**< Entry is stored in the UIM ECC phone book in slot 1. */
   PBM_CAT_COUNTER,      /**< Entry is stored in the UIM ECC phone book in slot 2. */
   PBM_CAT_GRPNAME,      /**< Entry is stored in the UIM GRPNAME phone book in slot 1. */
   PBM_CAT_AAS,          /**< Entry is stored in the UIM AAS phone book. */   
   PBM_CAT_MAX = 0xFFFE, /**< Sentinal. Last enumeration. */
   PBM_CAT_ERROR         /**< Sentinal. Last enumeration. */
} pbm_cat_e_type;

/**
  PBM field ID. When using the advanced API, the structure for each field item
  contains a field ID. The field ID indicates the meaning of the data stored in
  the specified field.
*/
typedef uint16 pbm_field_id_e_type;

/** @name Advanced API Fields
    These fields are for the advanced API only and correspond to the
    BREW<sup>&reg;</sup> fields.
    @{ */
#define PBM_FIELD_NONE                     ((pbm_field_id_e_type) 0)
  /**< For the advanced API, no particular field value.
  PBM_FIELD_NONE should not be changed to any other value than 0. */

#define PBM_FIELD_FIRSTNAME                ((pbm_field_id_e_type) 1)
  /**< For the advanced API, the field value is a first name. */
#define PBM_FIELD_MIDDLENAME               ((pbm_field_id_e_type) 2)
  /**< For the advanced API, the field value is a middle name. */
#define PBM_FIELD_LASTNAME                 ((pbm_field_id_e_type) 3)
  /**< For the advanced API, the field value is a last name. */
#define PBM_FIELD_NICKNAME                 ((pbm_field_id_e_type) 4)
  /**< For the advanced API, the field value is a nickname. */

#define PBM_FIELD_NAME                     ((pbm_field_id_e_type) 5)
  /**< For the advanced API, the field value is a name from the UIM, a text
       field from the legacy API, or the corresponding BREW field. */

#define PBM_FIELD_ORG                      ((pbm_field_id_e_type) 6)
  /**< For the advanced API, the field value is an organization. */
#define PBM_FIELD_JOBTITLE                 ((pbm_field_id_e_type) 7)
  /**< For the advanced API, the field value is a job title. */
#define PBM_FIELD_ADDRESS                  ((pbm_field_id_e_type) 8)
  /**< For the advanced API, the field value is a street address. */
#define PBM_FIELD_CITY                     ((pbm_field_id_e_type) 9)
  /**< For the advanced API, the field value is a city. */
#define PBM_FIELD_STATE                    ((pbm_field_id_e_type) 10)
  /**< For the advanced API, the field value is a state. */
#define PBM_FIELD_COUNTRY                  ((pbm_field_id_e_type) 11)
  /**< For the advanced API, the field value is a country. */
#define PBM_FIELD_ZIPCODE                  ((pbm_field_id_e_type) 12)
  /**< For the advanced API, the field value is a zipcode or postal code. */
#define PBM_FIELD_URL                      ((pbm_field_id_e_type) 13)
  /**< For the advanced API, the field value is a URL. */
#define PBM_FIELD_DEPARTMENT               ((pbm_field_id_e_type) 14)
  /**< For the advanced API, the field value is a department name. */
#define PBM_FIELD_EMAIL                    ((pbm_field_id_e_type) 15)
  /**< For the advanced API, the field value is an email address. */
#define PBM_FIELD_GENDER                   ((pbm_field_id_e_type) 16)
  /**< For the advanced API, the field value is a gender. */
#define PBM_FIELD_PHONE_WORK               ((pbm_field_id_e_type) 17)
  /**< For the advanced API, the field value is a work phone number. */
#define PBM_FIELD_PHONE_HOME               ((pbm_field_id_e_type) 18)
  /**< For the advanced API, the field value is a home phone number. */
#define PBM_FIELD_PHONE_FAX                ((pbm_field_id_e_type) 19)
  /**< For the advanced API, the field value is a fax number. */
#define PBM_FIELD_PHONE_VOICE              ((pbm_field_id_e_type) 20)
  /**< For the advanced API, the field value is a voice phone number. */
#define PBM_FIELD_PHONE_PREF               ((pbm_field_id_e_type) 21)
  /**< For the advanced API, the field value is a preferred phone number. */
#define PBM_FIELD_PHONE_CELL               ((pbm_field_id_e_type) 22)
  /**< For the advanced API, the field value is a cell phone number. */
#define PBM_FIELD_PHONE_PAGER              ((pbm_field_id_e_type) 23)
  /**< For the advanced API, the field value is a pager number. */

#define PBM_FIELD_PHONE_GENERIC            ((pbm_field_id_e_type) 24)
  /**< For the advanced API, the field value is a number from the UIM, a text
       field from the legacy API, or the corresponding BREW field. */

#define PBM_FIELD_PHONE_OTHER              ((pbm_field_id_e_type) 25)
  /**< For the advanced API, the field value is another phone number. */
#define PBM_FIELD_BDAY                     ((pbm_field_id_e_type) 26)
  /**< For the advanced API, the field value is a birthday. */
#define PBM_FIELD_SORTSTRING               ((pbm_field_id_e_type) 27)
  /**< For the advanced API, the field value is a string to be used for sorting. */
#define PBM_FIELD_NOTES                    ((pbm_field_id_e_type) 28)
  /**< For the advanced API, the field value is miscellaneous notes. */
#define PBM_FIELD_SIP_URL                  ((pbm_field_id_e_type) 29)
  /**< For the advanced API, the field value is a Session Initiation Protocol URL. */
#define PBM_FIELD_DIAL_STRING              ((pbm_field_id_e_type) 30)
  /**< For the advanced API, the field value is a dial string. */

// BREW took 31-33 for entries currently in 0xFFxx. Leaving a gap
// for maintainability.

#define PBM_FIELD_SPEED_DIAL               ((pbm_field_id_e_type) 34)
  /**< For the advanced API, the field value is a speed dial number. */
#define PBM_FIELD_INDEX                    ((pbm_field_id_e_type) 0xFF01)
  /**< For the advanced API, the field value is an index value. */
#define PBM_FIELD_VOICETAG                 ((pbm_field_id_e_type) 0xFF02)
  /**< For the advanced API, the field value is a voice tag. */

#define PBM_FIELD_NUMBER_TYPE              ((pbm_field_id_e_type) 0xFF03)
  /**< For the advanced API, the field value is a number type. Typically, this
       value is used only by call history entries. */

#define PBM_FIELD_NUMBER_PLAN              ((pbm_field_id_e_type) 0xFF04)
  /**< For the advanced API, the field value is a number plan. Typically, this
       value is used only by call history entries. */

#define PBM_FIELD_DATE                     ((pbm_field_id_e_type) 0xFF05)
  /**< For the advanced API, the field value is a date from the UIM, a date
       field from the legacy API, or the corresponding BREW field. Typically,
       this value is used when referring to call history entries. */

#define PBM_FIELD_DURATION                 ((pbm_field_id_e_type) 0xFF06)
  /**< Duration of a call in seconds. */

#define PBM_FIELD_CALL_TYPE                ((pbm_field_id_e_type) 0xFF07)
  /**< Field value is an advanced API that corresponds to the BREW field.
       Typically, this value is used only by call history entries. */

#define PBM_FIELD_NAMESUFFIX               ((pbm_field_id_e_type) 0xFF08)
  /**< For the advanced API, the field value is a name suffix. */
#define PBM_FIELD_NAMEPREFIX               ((pbm_field_id_e_type) 0xFF09)
  /**< For the advanced API, the field value is a name prefix. */
#define PBM_FIELD_OTHERNAME                ((pbm_field_id_e_type) 0xFF0A)
  /**< For the advanced API, the field value is another name. */
#define PBM_FIELD_PHOTO                    ((pbm_field_id_e_type) 0xFF0B)
  /**< For the advanced API, the field value is a photo. */
#define PBM_FIELD_PHOTOURL                 ((pbm_field_id_e_type) 0xFF0C)
  /**< For the advanced API, the field value is a URL of a photo. */
#define PBM_FIELD_POBOX                    ((pbm_field_id_e_type) 0xFF0D)
  /**< For the advanced API, the field value is a PO box. */
#define PBM_FIELD_ADDREXTRA                ((pbm_field_id_e_type) 0xFF0E)
  /**< For the advanced API, the field value is an extra address line. */
#define PBM_FIELD_STREET                   ((pbm_field_id_e_type) 0xFF0F)
  /**< For the advanced API, the field value is a street address. */
#define PBM_FIELD_UID                      ((pbm_field_id_e_type) 0xFF10)
  /**< For the advanced API, the field value is a unique ID of the record. */
#define PBM_FIELD_PUBLICKEY                ((pbm_field_id_e_type) 0xFF11)
  /**< For the advanced API, the field value is a public key. */
#define PBM_FIELD_PUBLICKEYSTRING          ((pbm_field_id_e_type) 0xFF12)
  /**< For the advanced API, the field value is a public key string. */
#define PBM_FIELD_GSM_TONNPI               ((pbm_field_id_e_type) 0xFF13)
  /**< For the advanced API, the field value is a type of numbering/numbering
       plan indicator. */
#define PBM_FIELD_COMPLEX                  ((pbm_field_id_e_type) 0xFF14)
  /**< For the advanced API, the field value denotes that it can enumerate
       multiple fields. */
#define PBM_FIELD_ADDITIONAL_NUMBER        ((pbm_field_id_e_type) 0xFF15)
  /**< For the advanced API, the field value is an additional phone number. */

/** @cond
*/
#define PBM_FIELD_ANNAME                   ((pbm_field_id_e_type) 0xFF16)
#define PBM_FIELD_ANNAME_ID                ((pbm_field_id_e_type) 0xFF17)
/** @endcond */

#define PBM_FIELD_GRPNAME                  ((pbm_field_id_e_type) 0xFF18)
  /**< For the advanced API, the field value is a group name. */
#define PBM_FIELD_GRPNAME_ID               ((pbm_field_id_e_type) 0xFF19)
  /**< For the advanced API, the field value is the ID of a group name. */
#define PBM_FIELD_HIDDEN                   ((pbm_field_id_e_type) 0xFF1A)
  /**< For the advanced API, the field value is hidden. */

#define  PBM_FIELD_PHONE_VIDEOCALLERID           ((pbm_field_id_e_type)0xFF1B)
  /**< For the advanced API, the field value is the caller ID of a video phone. */
#define  PBM_FIELD_PRIVATE_EMAIL1                ((pbm_field_id_e_type)0xFF1C)
  /**< For the advanced API, the field value is private email account 1. */
#define  PBM_FIELD_PRIVATE_EMAIL2                ((pbm_field_id_e_type)0xFF1D)
  /**< For the advanced API, the field value is private email account 2. */

/** @} */ /* end_name Advanced API Fields */

/*=============================================================================
Business tab fields
=============================================================================*/
/** @name Business Tab Fields
    @{ */

#define  PBM_FIELD_BUSINESS_EMAIL1               ((pbm_field_id_e_type)0xFF1E)
  /**< For the Business tab, the field value is the first email address. */
#define  PBM_FIELD_BUSINESS_EMAIL2               ((pbm_field_id_e_type)0xFF1F)
  /**< For the Business tab, the field value is the second email address. */
#define  PBM_FIELD_BUSINESS_URL                  ((pbm_field_id_e_type)0xFF20)
  /**< For the Business tab, the field value is the URL. */
#define  PBM_FIELD_PHONE_BUSINESS_FAX            ((pbm_field_id_e_type)0xFF21)
  /**< For the Business tab, the field value is the fax number. */
#define  PBM_FIELD_BUSINESS_STREET               ((pbm_field_id_e_type)0xFF22)
  /**< For the Business tab, the field value is the street address. */
#define  PBM_FIELD_BUSINESS_CITY                 ((pbm_field_id_e_type)0xFF23)
  /**< For the Business tab, the field value is the city in a business address. */
#define  PBM_FIELD_BUSINESS_STATE                ((pbm_field_id_e_type)0xFF24)
  /**< For the Business tab, the field value is the state in a business address. */
#define  PBM_FIELD_BUSINESS_COUNTRY              ((pbm_field_id_e_type)0xFF25)
  /**< For the Business tab, the field value is the country in a business address. */
#define  PBM_FIELD_BUSINESS_POSTALCODE           ((pbm_field_id_e_type)0xFF26)
  /**< For the Business tab, the field value is the zipcode or postal code in a
       business address. */
#define  PBM_FIELD_BUSINESS_NOTES                ((pbm_field_id_e_type)0xFF27)
  /**< For the Business tab, the field value is miscellaneous notes. */

/** @} */ /* end_name Business Tab Fields */

/*=============================================================================
Personal tab fields
=============================================================================*/
/** @name Personal Tab Fields
    @{ */
#define  PBM_FIELD_RINGTONE_AUDIO                ((pbm_field_id_e_type)0xFF28)
  /**< For the Personal tab, the field value is the ringtone for an audio call. */
#define  PBM_FIELD_RINGTONE_VIDEO                ((pbm_field_id_e_type)0xFF29)
  /**< For the Personal tab, the field value is the ringtone for a video call. */
#define  PBM_FIELD_BIRTHDAY                      ((pbm_field_id_e_type)0xFF2A)
  /**< For the Personal tab, the field value is the ringtone for a birthday. */
#define  PBM_FIELD_EVENT_UID                     ((pbm_field_id_e_type)0xFF2B)
  /**< For the Personal tab, the field value is the unique ID of an event. */

/** @} */ /* end_name Personal Tab Fields */

/*=============================================================================
Fields Used to store default information
=============================================================================*/
/** @name Fields Used to Store Default Information
    @{ */
#define  PBM_FIELD_DEFAULT_PHONE                 ((pbm_field_id_e_type)0xFF2C)
  /**< Field value is a default mobile phone number. */
#define  PBM_FIELD_DEFAULT_EMAIL                 ((pbm_field_id_e_type)0xFF2D)
  /**< Field value is a default email address. */
#define  PBM_FIELD_DEFAULT_ADDRESS               ((pbm_field_id_e_type)0xFF2E)
  /**< Field value is a default address. */
#define  PBM_FIELD_DEFAULT_URL                   ((pbm_field_id_e_type)0xFF2F)
  /**< Field value is a default URL. */
#define  PBM_FIELD_DEFAULT_FAX                   ((pbm_field_id_e_type)0xFF30)
  /**< Field value is a default fax number. */
#define  PBM_FIELD_DISTRIBUTION_LIST             ((pbm_field_id_e_type)0xFF31)
  /**< Field value is a default distribution list. */

/** @} */ /* end_name Fields Used to Store Default Information */

/*=============================================================================
Additional Fields
=============================================================================*/
/** @name Additional Fields
    @{ */
#define  PBM_FIELD_PRIVATEPHONE_VOICETAG         ((pbm_field_id_e_type)0xFF32)
  /**< Field value is a voice tag for a private phone number. */
#define  PBM_FIELD_BUSINESSPHONE_VOICETAG        ((pbm_field_id_e_type)0xFF33)
  /**< Field value is a voice tag for a business phone number. */
#define  PBM_FIELD_PRIVATEMOBILE_VOICETAG        ((pbm_field_id_e_type)0xFF34)
  /**< Field value is a voice tag for a private mobile phone. */
#define  PBM_FIELD_BUSINESSMOBILE_VOICETAG       ((pbm_field_id_e_type)0xFF35)
  /**< Field value is a voice tag for a business mobile phone. */
#define  PBM_FIELD_VIDEOTELEPHONY                ((pbm_field_id_e_type)0xFF36)
  /**< Field value is the video telephony. */
#define  PBM_FIELD_USEDCOUNT                     ((pbm_field_id_e_type)0xFF37)
  /**< Field value is a used account. */
#define  PBM_FIELD_BCARD                         ((pbm_field_id_e_type)0xFF38)
  /**< Field value is a business card. */
#define  PBM_FIELD_GROUP                         ((pbm_field_id_e_type)0xFF39)
  /**< Field value is a group, e.g., a family. */
#define  PBM_FIELD_DLISTS                        ((pbm_field_id_e_type)0xFF3A)
  /**< Field value is a distribution list. */
#define  PBM_FIELD_CALL_METHOD                   ((pbm_field_id_e_type)0xFF3B)
  /**< Field value is a call method. */
#define  PBM_FIELD_AOC                           ((pbm_field_id_e_type)0xFF3C)
  /**< Field value is an Advice of Charge. */

/** @} */ /* end_name Additional Fields */

/*=============================================================================
Field for phone unique ID
=============================================================================*/
/** @name Field for Phone Unique ID
    @{ */

#define  PBM_FIELD_PH_UID                        ((pbm_field_id_e_type)0xFF3D)
  /**< Field value is a phone unique ID. */

/** @} */ /* end_name */

/*=============================================================================
Field for different emergency numbers
=============================================================================*/
/** @name Fields for Emergency and Videotelephony Numbers
    @{ */
#define PBM_FIELD_HARDCODED_ECC                  ((pbm_field_id_e_type) 0xFF3E)
  /**< Field value is an emergency number that is hardcoded in the phone. */
#define PBM_FIELD_NV_ECC                         ((pbm_field_id_e_type) 0xFF3F)
  /**< Field value is an emergency number that is in NV memory. */
#define PBM_FIELD_SIM1_ECC                       ((pbm_field_id_e_type) 0xFF40)
  /**< Field value is an emergency number that is on the SIM1 card. */

/* Defined for a session-based phone book */
#define PBM_FIELD_SIM_ECC                        PBM_FIELD_SIM1_ECC
  /**< Field value is an emergency number that is on the SIM card. */
#define PBM_FIELD_SIM2_ECC                       ((pbm_field_id_e_type) 0xFF41)
  /**< Field value is an emergency number that is on the SIM2 card. */
#define PBM_FIELD_ECC_SERVICE_CATEGORY           ((pbm_field_id_e_type) 0xFF42)
  /**< Field value is the service category of an emergency number. */

/* More EFS fields */
#define  PBM_FIELD_PRIVATE_ADDITIONALINFO        ((pbm_field_id_e_type) 0xFF43)
  /**< Field value is additional information for a private number. */
#define  PBM_FIELD_BUSINESS_ADDITIONALINFO       ((pbm_field_id_e_type) 0xFF44)
  /**< Field value is additional information for a business number. */

/* Videotelephony stuff */
#define  PBM_FIELD_PHONE_VIDEOTELEPHONY          ((pbm_field_id_e_type) 0xFF45)
  /**< Field value is video telephony. */
#define  PBM_FIELD_PREFER_VT                     ((pbm_field_id_e_type) 0xFF46)
  /**< Field value is the preferred video telephony. */

#define  PBM_FIELD_NETWORK_ECC                   ((pbm_field_id_e_type) 0xFF47)
  /**< Field value is an emergency number from the network. */

#define  PBM_FIELD_TECH_TYPE                     ((pbm_field_id_e_type) 0xFF48)
  /**< Field value is a call technology type. */

#define  PBM_FIELD_FIELDID_PRIMARY_PHONE         ((pbm_field_id_e_type) 0xFF49)
  /**< Field value is the primary phone ID. */

/** @} */ /* end_name Fields for Various Emergency and Videotelephony Numbers */

/*=============================================================================
Fields for UIOneRefUI Contact App
=============================================================================*/
/** @name Fields for the UIOneRefUI Contact Application
    @{ */
#define  PBM_FIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE  ((pbm_field_id_e_type) 0xFF50)
  /**< For the UIOneRefUI Contact application, the value of the field attribute
       is a boolean value. */
#define  PBM_FIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE   ((pbm_field_id_e_type) 0xFF51)
  /**< For the UIOneRefUI Contact application, the value of the field attribute
       is a text string. */
#define  PBM_FIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE ((pbm_field_id_e_type) 0xFF52)
  /**< For the UIOneRefUI Contact application, the value of the record attribute
       is a boolean value. */
#define  PBM_FIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE  ((pbm_field_id_e_type) 0xFF53)
  /**< For the UIOneRefUI Contact application, the value of the record attribute
       is a text string. */

#define PBM_FIELD_ADDITIONAL_NUMBER_TYPE        ((pbm_field_id_e_type) 0xFF54)
  /**< To specify Additional Alpha String Identifier for 
       a particular Additional Number*/

#define  PBM_FIELD_EMERGENCY_MODE               ((pbm_field_id_e_type) 0xFF55)  
/** @} */ /* end_name Fields for UIOneRefUI Contact Application */

#define  PBM_FIELD_ADDITIONAL_NUMBER_EXT        ((pbm_field_id_e_type)0xFF56)

#define  PBM_FIELD_SRV_STATUS_FLAG              ((pbm_field_id_e_type) 0xFF57)

#define PBM_MAX_USIM_FIELDS_IN_PB_SET 10 
/**< Maximum number of USIM Fields that can be present in one set i.e. 8 (email, emaila
        sne, grp, anr, anra, anrb, anrc) +2 (RFU) */


/** @cond
*/
/* Call technology types */
enum
{
   PBM_TECH_TYPE_VOICE,  /**< Technology type is voice. */
   PBM_TECH_TYPE_VIDEO,  /**< Technology type is video. */
   PBM_TECH_TYPE_DATA    /**< Technology type is data. */
};
/** @endcond */


/** Technology to which the phonebook record belongs. */
typedef uint16 pbm_tech_type_e_type;


/** @cond
*/
/* apitrim -include */
enum
{
   PBM_FT_NONE,
   PBM_FT_BYTE,   /* Data type is a number, one byte in length. */
   PBM_FT_WORD,   /* Data type is a number, two bytes in length. */
   PBM_FT_DWORD,  /* Data type is a number, four bytes in length. */
   PBM_FT_STRING, /* Data type is a null terminated character string. */
   PBM_FT_BINARY, /* Use of this type is up to the application. */
   PBM_FT_PHONE,  /* A phone number is a null terminated string. The PBM
                     interprets this field type to be a null terminated character
                     string, but this entry can be used when searching for phone
                     numbers. */

   PBM_FT_BITMAP,
   PBM_FT_COMPLEX,
   PBM_FT_UCS2,
   PBM_FT_UCS2_COMPRESSED,
   PBM_FT_BCD,
   PBM_FT_BOOLEAN
};
/** @endcond */


/** Denotes the field type associated with a record. */
typedef uint8 pbm_field_type_e_type;

/*===========================================================================
STRUCTURE: PBM_FIELD_S_TYPE
===========================================================================*/
/**
  @brief For functions implementing the advanced API, this structure
  is used to communicate the contents of the entry to and from the API.
  The interface typically contains an array of these structures, one
  entry for each field being communicated.
*/
typedef struct
{
   pbm_field_id_e_type field_id;
      /**< Describes what the data represents (name, number, etc). */

   pbm_field_type_e_type field_type;
      /**< Indicates if the field is an 8-bit number, a 32-bit number,
           or a character string. */

   uint32 buffer_offset;
     /**< Number of bytes from the beginning of this field to the beginning
          of data. */

   uint16 data_len;
     /**< Length of the data allocated. A string of 7 characters requires
          8 bytes (7 for the string, 1 for the NULL termination). */

} pbm_field_s_type;

/* apitrim -end-include */

/** @brief Denotes the basic field structure.
*/
typedef struct
{
   pbm_field_id_e_type   field_id;         /**< Field ID. */
   int                   num_items;        /**< Maximum number of items within
                                                a record.*/
   int                   max_length;       /**< Maximum length of the data. */
   pbm_field_type_e_type field_type;       /**< Field type. */
   boolean               field_search;     /**< Allows a search for data. */
   boolean               field_cat_search; /**< Allows a search for data within a
                                                specified category. */
} pbm_field_info_s_type;

/** @brief Complex data type item. This structure maps directly to AEEDBComplex.
*/
typedef PACKED struct
{
   pbm_field_id_e_type   field_id;   /**< Field ID. */
   uint16                data_len;   /**< Number of bytes in buf[]. */
   pbm_field_type_e_type field_type; /**< Field type. */
   uint8                 buf[1];     /**< Variable number of bytes of data. */
} pbm_sub_field_s_type;

/** @brief Complex data type. This structure maps directly to AEEDBSubfield.
*/
typedef PACKED struct
{
   uint16   num_subfields; /**< Number of data type items. */
   uint8    version;       /**< Version of the complex data type. */
   uint8    byReserved;    /**< Currently not used. */
   uint8    buf[1];        /**< Variable number of bytes containing
                                the data type items. */
} pbm_complex_field_s_type;

/** Denotes the type of lock.
*/
typedef enum
{
   PBM_LOCK_NOT_INIT,        /**< Lock is not set. */
   PBM_W_LOCK_BLOCKING,      /**< Other tasks are blocked on the lock. */
   PBM_W_LOCK_NON_BLOCKING,  /**< Writes from other tasks return errors. */
   PBM_RW_LOCK_BLOCKING,     /**< No read/writes from other tasks (blocking). */
   PBM_RW_LOCK_NON_BLOCKING  /**< No read/writes from other tasks (nonblocking). */
} pbm_lock_type_e_type;

/** Denotes the type of event.
*/
typedef enum
{
   PBM_EVENT_REC_ADD,           /**< New record was added (data: rec_id). */
   PBM_EVENT_REC_UPDATE,        /**< Record was updated (data: rec_id). */
   PBM_EVENT_REC_DELETE,        /**< Record was deleted (data: rec_id). */
   PBM_EVENT_REC_FAILED,        /**< Record operation failed (data: rec_id). */
   PBM_EVENT_PB_REFRESH_START,  /**< Phone book refresh was started
                                     (data: pb_id). */
   /* 5 */
   PBM_EVENT_PB_REFRESH_DONE,   /**< Phone book refresh is done (data: pb_id). */
   PBM_EVENT_PB_READY,          /**< Phone book is now ready (data: pb_id). */
   PBM_EVENT_LOCKED,            /**< PBM is locked (data: lock_type). */
   PBM_EVENT_UNLOCKED,          /**< PBM is unlocked (data: lock_type). */
   PBM_EVENT_PH_UNIQUE_IDS_VALIDATED,  /**< Phone's unique IDs are validated
                                            (data: none). */
   /* 10 */
   PBM_EVENT_SIM_INIT_DONE,         /**< PBM SIM is done initializing (data: sim_id). */
   PBM_EVENT_SESSION_INIT_DONE,     /**< PBM session is done initializing (data:
                                         session_id). */
   PBM_EVENT_HIDDEN_CONTACTS_STATUS, /**< Indicates whether a hidden key is verified.
                                         The ADN cache is updated; only one event is
                                         sent across (data: hidden_contact_status). */
   /* 12 Please update this number if new event is added */
   PBM_EVENT_CACHE_STATUS_UPDATE  
   /* update the PB cache status to QMI PBM whem the cache status is RESTRICTED/NOT AVAILABLE*/
} pbm_notify_event_e_type;

/** Denotes whether hidden contacts are visible or invisible.
*/
typedef enum
{
    PBM_HIDDEN_CONTACTS_INVALID,     /**<Hidden contacts are in an invisible state. */
    PBM_HIDDEN_CONTACTS_VALID        /**<Hidden contacts are in a visible state. */
}pbm_hidden_contact_status_e_type;

/** Denotes the write status for particular phonebook.
*/
typedef enum
{
  PBM_WRITE_STATUS_NOT_ALLOWED,	  /**< PB is writable. */
  PBM_WRITE_STATUS_ALLOWED, /**< PB is not writable, check protection status */
  PBM_WRITE_STATUS_NOT_KNOWN   /**< PB write status is not known */
}pb_write_status_e_type;

/** Denotes the bit position of various pins in pin_protection_mask.
*/
typedef enum
{
  PBM_PIN_PIN1,  /**< PIN1 */
  PBM_PIN_PIN2,  /**< PIN2 */
  PBM_PIN_ADM,   /**< ADM */
  PBM_PIN_RESERVED   /**< Reserved */
}pbm_pin_id_e_type;


/** Denotes the number of PB sets.
*/

typedef enum
{
   PBM_USIM_PB_INVALID = -1,
   PBM_USIM_PB_ONE,   /**< set 1 */
   PBM_USIM_PB_TWO,   /**< set 2 */
   PBM_USIM_PB_THREE, /**< set 3 */
   PBM_USIM_PB_FOUR,  /**< set 4 */
   PBM_MAX_NUM_PB_SETS  /**< MAX set */
} pbm_pb_set_type;

/** Denotes the mapping type of file.
*/
typedef enum {
   PBM_MAPPING_UNKNOWN = 0x00000000,
   PBM_MAPPING_TYPE_1,  /**< type 1 file */
   PBM_MAPPING_TYPE_2,  /**< type 2 file */
   PBM_MAPPING_TYPE_3  = 0x00000003  /**< type 3 file */
}pbm_pbr_mapping_enum_type;



/** @brief Session-based event details of hidden contacts.
*/
typedef struct
{
    pbm_hidden_contact_status_e_type status; /**< State of the hidden contacts. */
    pbm_phonebook_type      pb_id;           /**< Session-based phone book to which
                                                  this event is coupled. */
}pbm_session_hidden_contacts_status_s_type;

/** @brief Legacy event details of hidden contacts.
*/
typedef struct
{
    pbm_hidden_contact_status_e_type status;  /**< State of the hidden contacts. */
    pbm_device_type      pb_id;               /**< SIM-based phonebook to which this
                                                   event is coupled. */
}pbm_hidden_contacts_status_s_type;

/** @brief Denotes the structure of PBM events.
*/
typedef union {
  uint16               rec_id;          /**< Record ID. */
  pbm_device_type      pb_id;           /**< Phone book ID. */
  pbm_lock_type_e_type lock_type;       /**< Lock type. */
  pbm_slot_type               sim_id;   /**< ID 1 for slot 1. Add 1 for each
                                             additional slot. */
  pbm_hidden_contacts_status_s_type hidden_contact_status;
    /**< State of the hidden contacts. */

} pbm_event_data_u_type;
/*~ IF ( _DISC_ == PBM_EVENT_REC_ADD ||
         _DISC_ == PBM_EVENT_REC_UPDATE ||
         _DISC_ == PBM_EVENT_REC_DELETE ||
         _DISC_ == PBM_EVENT_REC_FAILED )
    pbm_event_data_u_type.rec_id */
/*~ IF ( _DISC_ == PBM_EVENT_PB_REFRESH_START ||
         _DISC_ == PBM_EVENT_PB_REFRESH_DONE ||
         _DISC_ == PBM_EVENT_SESSION_INIT_DONE ||
         _DISC_ == PBM_EVENT_PH_UNIQUE_IDS_VALIDATED ||
         _DISC_ == PBM_EVENT_PB_READY )
    pbm_event_data_u_type.pb_id */
/*~ IF ( _DISC_ == PBM_EVENT_LOCKED ||
         _DISC_ == PBM_EVENT_UNLOCKED )
     pbm_event_data_u_type.lock_type */
/*~ IF ( _DISC_ == PBM_EVENT_SIM_INIT_DONE )
     pbm_event_data_u_type.sim_id */
/*~ IF ( _DISC_ == PBM_EVENT_HIDDEN_CONTACTS_STATUS )
     pbm_event_data_u_type.hidden_contact_status */

/** @brief Union for a session-based phone book.
*/
typedef union
{
   pbm_record_id_type    rec_id;    /**< Record ID. */
   pbm_phonebook_type pb_id;        /**< Phone book ID. */
   pbm_lock_type_e_type  lock_type; /**< Lock type. */
   pbm_slot_type         sim_id;    /**< ID 1 for slot 1. Add 1 for each
                                         additional slot. */
   pbm_session_hidden_contacts_status_s_type hidden_contact_status;
     /**< State of the hidden contacts. */

} pbm_session_event_data_u_type;
/*~ IF ( _DISC_ == PBM_EVENT_REC_ADD ||
         _DISC_ == PBM_EVENT_REC_UPDATE ||
         _DISC_ == PBM_EVENT_REC_DELETE ||
         _DISC_ == PBM_EVENT_REC_FAILED )
    pbm_session_event_data_u_type.rec_id */
/*~ IF ( _DISC_ == PBM_EVENT_PB_REFRESH_START ||
         _DISC_ == PBM_EVENT_PB_REFRESH_DONE ||
         _DISC_ == PBM_EVENT_SESSION_INIT_DONE ||
         _DISC_ == PBM_EVENT_PH_UNIQUE_IDS_VALIDATED ||
         _DISC_ == PBM_EVENT_PB_READY )
    pbm_session_event_data_u_type.pb_id */
/*~ IF ( _DISC_ == PBM_EVENT_LOCKED ||
         _DISC_ == PBM_EVENT_UNLOCKED )
     pbm_session_event_data_u_type.lock_type */
/*~ IF ( _DISC_ == PBM_EVENT_SIM_INIT_DONE )
     pbm_session_event_data_u_type.sim_id */
/*~ IF ( _DISC_ == PBM_EVENT_HIDDEN_CONTACTS_STATUS )
     pbm_session_event_data_u_type.hidden_contact_status */

/** @brief Denotes the notification structure.
*/
typedef struct
{
   pbm_notify_event_e_type  event;                 /**< Event. */
   pbm_event_data_u_type    data;                  /**< Event data. */
   pbm_session_event_data_u_type    session_data;  /**< Session event data. */

} pbm_notify_data_s_type;
/*~ FIELD pbm_notify_data_s_type.data DISC pbm_notify_data_s_type.event */
/*~ FIELD pbm_notify_data_s_type.session_data DISC pbm_notify_data_s_type.event */

/** @brief Denotes the write callback structure.
*/
typedef struct {
  pbm_return_type  ret;            /**< Return value of the operation. */
  uint16           rec_id;         /**< Legacy record ID to which this callback
                                        applies. */
  pbm_record_id_type sess_rec_id;  /**< 32-bit record ID for session-based
                                        phone book. */
  void            *user_data;      /**< User-specified data passed in by the
                                        caller, if supported. */
} pbm_writecb_data_s_type;

/** @brief Denotes the usim field capibility for one field in one pb set.
*/
typedef struct {
  pbm_field_id_e_type field_id ;   /**< field id for USIM field */
  pbm_pbr_mapping_enum_type mapping_type; /**< mapping type for the EF corresponding to this field */
  int   num_of_records;   /**< number of records present in EF for this field */
  int   records_in_use;         /**< records in use for this field */
  int   record_length;    /**< length of resord that can be used by clients to fill the field*/  
} pbm_usim_field_capability_s_type;

/** @brief Denotes the usim field capibilities for all USIM fields in all PB sets.
*/
typedef struct {
  pbm_usim_field_capability_s_type	pbm_all_usim_capibilities[PBM_MAX_NUM_PB_SETS][PBM_MAX_USIM_FIELDS_IN_PB_SET];	  /**< length of resord that can be used by clients to fill the field*/  
  uint16 total_pbsets;  
} pbm_all_usim_field_capabilities_s_type;

typedef struct {
  uint8 used_adn_records;
  uint8 max_adn_records;
  uint8 max_num_len;
  uint8 max_text_len;
}pbm_adn_caps_s_type;

typedef struct {
  uint8                 num_pb_sets;
  pbm_adn_caps_s_type   adn_caps[PBM_MAX_NUM_PB_SETS];
}pbm_adn_capabilites_pbset_s_type;

/** PBM Ready event for the pbm_notify_register() function. */
typedef void (*PBM_EVENT_FUNC)(boolean ready);

/** Translates the user data into a callback function. */
typedef void (*PBM_WRITE_COMPAT_CB_FUNC)(pbm_return_type ret);

/** Callback for a write operation. */
typedef void (*PBM_WRITE_CB_FUNC)(pbm_writecb_data_s_type *cb_data);
/*~ PARAM cb_data POINTER */

/** Used by the PBM to notify clients about different events. */
typedef void (*PBM_NOTIFY_FUNC)(void *user_data,  pbm_notify_data_s_type *notify_data);
/*~ PARAM notify_data POINTER */

/** Callback for file information. */
typedef void (*PBM_FILE_INFO_CB_FUNC) ( pbm_return_type ret,
                                        pbm_device_type pb_id,
                                        int records_used,
                                        int number_of_records,
                                        int text_len);

/**Denotes information regarding the write status of phone book.
Also tells the pin protection status of the phonebook
*/
typedef struct
{
   pb_write_status_e_type write_status;         /**< status whether PB can be written or not */
   uint32 pin_protection_mask;    /**< this bit mask tells what PINs needs to be 
                                                                    verified for writing into this particular phonebook.
                                                                    bit0 : PIN1
                                                                    bit1 : PIN2
                                                                    bit2 : ADM 
                                                                    rest of the bits are reserved for future use 
                                                                    */
   mmgsdi_file_security_enum_type protection_method; 
	/**< Protection method: 
     - MMGSDI_ALWAYS_ALLOWED -- num_protection_pin = 0 and protection_pin = NULL
     - MMGSDI_NEVER_ALLOWED -- num_protection_pin = 0 and protection_pin = NULL
     - MMGSDI_AND_ALLOWED -- More than one PIN is required
     - MMGSDI_OR_ALLOWED -- One of multiple PINs is required
     - MMGSDI_SINGLE_ALLOWED -- Only one PIN is required
     */
} pb_write_status_s_type;


/** @brief Denotes information regarding the corresponding phone book.
*/
typedef struct
{
   int records_used;         /**< Number of records used. */
   int number_of_records;    /**< Total number of records. */
   int text_len;             /**< Length of the text fields. */
   int number_len;           /**< Length of the number fields, not including +
                                  characters or NULL. */
   pb_write_status_s_type pb_write_status;  /**< status info to know
    whether the phonebook is read only or write only */ 

} pbm_extended_fileinfo_s_type;

/** @brief Denotes the initialization status of phone memory and session memory.
*/
typedef struct {
   boolean pbm_efs_init_done;                    /**< Initialization of phone
                                                      memory. */
   boolean pbm_session_init_done[MAX_UIM_APPS];  /**< Initialization of session
                                                      memory. */
} pbm_init_info_s_type;

/** Denotes the initialization status of different sessions. */
extern pbm_init_info_s_type pbm_init_info;

/** Callback for extended file information. */
typedef void (*PBM_EXTENDED_FILE_INFO_CB_FUNC) ( pbm_return_type ret,
                                                 pbm_device_type pb_id,
                                                 pbm_extended_fileinfo_s_type *info);
/*~ PARAM info POINTER */

/** Callback for session-based extended file information. */
typedef void (*PBM_SESSION_EXTENDED_FILE_INFO_CB_FUNC) ( pbm_return_type ret,
                                                 pbm_phonebook_type pb_id,
                                                 pbm_extended_fileinfo_s_type *info);

/*~ PARAM info POINTER */


/** Callback for a find operation. */
typedef void (*PBM_FIND_CB_FUNC) ( pbm_return_type ret, pbm_record_s_type *rec);
/*~ PARAM rec POINTER */

/** Callback for data when the PBM is ready. */
extern PBM_EVENT_FUNC ds_ready_fn;

/*===========================================================================
FUNCTION: PBM_CLEAR_CALL_HISTORY
===========================================================================*/
/**
  Clears all call history entries.

@return
  None.

@dependencies
  None.
*/
extern pbm_return_type pbm_clear_call_history(void);
/*~ FUNCTION pbm_clear_call_history */

/*===========================================================================
FUNCTION: PBM_IS_INIT_COMPLETED
===========================================================================*/
/**
  Notifies the client if PBM initialization is complete.

@return
  None.

@dependencies
  None.
*/
extern boolean pbm_is_init_completed(void);
/*~ FUNCTION pbm_is_init_completed */

/*===========================================================================
FUNCTION: PBM_ENUM_REC_INIT
===========================================================================*/
/**
  Performs a search to set up a list of records with the matching criteria.
  This list is used in subsequent calls to pbm_enum_next_rec(), which iterates
  through the records that match the given criteria. Search results are stored
  in static variables and are overwritten by the next call to pbm_enum_rec_init().

  Following are the searching rules:
  - Only records with the specified category are matched. If PBM_CAT_NONE is
    specified, all categories are matched.
  - Only records that contain at least one instance of a field with the
    specified field ID are matched. If PBM_FIELD_NONE is specified, this
    criterion is ignored.
  - Only records with data in the specified field that match the data
    specified by data_ptr are matched. If data_ptr is NULL, this
    criterion is ignored.

@newpage
  The type of matching depends on the setting of flags:
  - PBM_CASE_SENSITIVE_MATCH and PBM_CASE_INSENSITIVE_MATCH control
    case sensitivity.
  - PBM_EXACT_MATCH and PBM_STARTS_WITH_MATCH control how much of the
    data specified by data_ptr must be the same to be considered a match.

  Example: pbm_rec_init(PBM_EFS, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
            PBM_CASE_INSENSITIVE_MATCH | PBM_STARTS_WITH_MATCH)
            matches on the names John Smith and Johann Bach.

@vertspace
@param[in] pb_id       Phone book for which the information is requested.
@param[in] category    Category of the record.
@param[in] field_id    Field ID that the data pointer must match.
@param[in] data_ptr    Pointer to the contents of the field that must match
                       the data and record content. The data supplied must be the
                       correct type for  the field_id.
@param[in] data_size   Number of bytes pointed to by data_ptr.
@param[in] flags       Modifiers for the search. At most, one from each
                      category can be numerically ORed together to create
                      the result.

@return
  PBM_ERROR -- The parameters are invalid, or the PBM has encountered an
               internal error. \n
  PBM_SUCCESS -- The search has been performed. The results of the search are
                 returned via pbm_enum_next_rec.

@dependencies
  The pbm_init() function must be called first.\n
@vertspace
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based API, pbm_session_enum_rec_init.
*/
extern pbm_return_type pbm_enum_rec_init(pbm_device_type pb_id,
                                         uint16 category,
                                         pbm_field_id_e_type field_id,
                                         const uint8 *data_ptr,
                                         uint16 data_size,
                                         uint32 flags);
/*~ FUNCTION pbm_enum_rec_init */
/*~ PARAM data_ptr VARRAY LENGTH data_size */

/*===========================================================================
FUNCTION: PBM_ENUM_NEXT_REC_ID
===========================================================================*/
/**
  Retrieves the next record ID that matches the search criteria
  specified in the most recent call to pbm_enum_rec_init().

  Only one search at a time is supported. This function cannot allow situations
  where one application has called pbm_enum_rec_init() and is in the process of
  calling pbm_enum_next_rec_id(), and then another application calls
  pbm_enum_rec_init(), changes the search criteria, and resets the search
  before the first application is done. In this case, the first application may
  not get the expected records.

  When the end of the enumeration is reached, the index is not reset to the
  beginning of the enumeration. All subsequent calls to this function must
  continue to return PBM_EOF. The caller must call pbm_enum_rec_init() to
  reinitialize the search criteria.

@vertspace
@param[out] rec_id_ptr   Pointer to the location of the matching record ID.

@dependencies
  The pbm_enum_rec_init() function must be called first.\n
@vertspace
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_enum_next_rec_id().
*/
pbm_return_type pbm_enum_next_rec_id(uint16 *rec_id_ptr);
/*~ FUNCTION pbm_enum_next_rec_id */
/*~ PARAM OUT rec_id_ptr POINTER */

/*===========================================================================
FUNCTION: PBM_FILE_INFO
===========================================================================*/
/**
  Provides information about the specified phone book.

  The result of the command is returned via a callback. The callback function
  has the result of the command, the device that was requested, the number of
  records used, the total number of records, and the maximum length of the text
  field supported by this device.

@vertspace
@param[in] pb_id       Phone book for which the information is requested.
@param[in] proc_func   Callback function that returns the result of the command.

@return
  PBM_ERROR -- The parameters are invalid.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_SUCCESS -- The callback was called.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_extended_file_info().
*/
extern pbm_return_type pbm_file_info (pbm_device_type pb_id,
                                      PBM_FILE_INFO_CB_FUNC proc_func);
/*~ FUNCTION pbm_file_info */

/*===========================================================================
FUNCTION: PBM_EXTENDED_FILE_INFO
===========================================================================*/
/**
  Returns, through GSDI, the total number of records, the number of records
  in use, the text length, and the maximum phone number length for the specified
  phone book. The data is returned in the proc_func parameter.

@vertspace
@param[in] pb_id       Phone book for which the information is requested.
@param[in] proc_func   Callback function that returns the requested file information.

@return
  PBM_ERROR -- The parameters are invalid.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_SUCCESS -- The callback was called, or a record was found and its
                 results are returned in the data.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_extended_file_info().
*/
extern pbm_return_type pbm_extended_file_info (pbm_device_type pb_id,
                                               PBM_EXTENDED_FILE_INFO_CB_FUNC proc_func);
/*~ FUNCTION pbm_extended_file_info */

/*===========================================================================
FUNCTION: PBM_FIND_LOCATION
===========================================================================*/
/**
  Returns the contents of a record at the given index either via the data
  parameter or the callback mechanism.

  This function calls the function defined in the proc_func parameter for each matching
  occurrence of the string, <i>number</i>. If no match is found, PBM_EOF is returned.
  Otherwise, each matching record is returned until PBM_EOF is reached.

  The data is returned in proc_func. The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback. If clients want
  to use the data after the callback terminates, they must make a copy.

@vertspace
@param[in]     pb_id       Phone book for which the information is requested.
@param[in]     index       Location of the requested record.
@param[in,out] data        If this parameter is not NULL, the contents of the
                           record at this location are written to this pointer.
@param[in]     proc_func   Callback function that returns the contents of the
                           record at the requested location.

@return
  PBM_ERROR -- The parameters are invalid.\n
@vertspace
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
@vertspace
  PBM_ERROR_NOT_FOUND -- The location in the requested phone book is empty and
                         there is no callback specified.\n
                         If a callback is specified, this function returns
                         PBM_SUCCESS, and the fact that the record was not found
                         is supplied via the callback.\n
@vertspace
  PBM_SUCCESS -- The callback was called, or a record was found and its
                 results are returned in the data.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_find_location().
*/
extern pbm_return_type pbm_find_location (pbm_device_type pb_id,
                                          int index,
                                          pbm_record_s_type * data,
                                          PBM_FIND_CB_FUNC proc_func);
/*~ FUNCTION pbm_find_location */
/*~ PARAM OUT data POINTER */

/*===========================================================================
FUNCTION: PBM_FIND_NAME_NEXT
===========================================================================*/
/**
  Returns the next record that has matching name information.

  This function can be called only after pbm_find_name() has been called and a
  callback is provided via that function.
@newpage
  This function causes the previously-used callback to be called with the next
  result from the pbm_find_name() function. It also continues reading from the
  linked list to determine if additional calls to the SIM are needed. If so,
  it calls pbm_sim_read() again.

@return
  PBM_ERROR -- The parameters are invalid, or the PBM is unable to return a
               result because of an internal error.\n
@vertspace
  PBM_ERROR_NOT_FOUND -- The search did not find any record.\n
@vertspace
  PBM_EOF -- The last record has already been returned.\n
@vertspace
  PBM_SUCCESS -- The callback was called.

@dependencies
  None.
*/
extern pbm_return_type pbm_find_name_next( void );
/*~ FUNCTION pbm_find_name_next */

/*===========================================================================
FUNCTION: PBM_FIND_NAME
===========================================================================*/
/**
  Searches the requested phone books for any record containing a PBM_FIELD_NAME
  (when the record is from the UIM or when using a legacy API) with the
  contents starting with the specified string. The search is case insensitive.

  This function calls the function defined in the proc_func parameter for each matching
  occurrence of the string, <i>name</i>. If no match is found, PBM_ERROR_NOT_FOUND is
  returned. Otherwise, each matching record is returned until PBM_EOF is reached.

  The data is returned in proc_func. The record pointer passed as a parameter
  in proc_func is only valid for the duration of the callback. If clients want
  to use the data after the callback terminates, they must make a copy.

@vertspace
@param[in]     pb_id       Phone book for which the information is requested.
@param[in]     name        Pointer to the NULL-terminated string to search for
                           in the name.
@param[in,out] data        If this parameter is not NULL, the contents of the
                           record at this location are written to this pointer.
@param[in]     proc_func   Callback function that returns the contents of the
                           record at the requested location.

@return
  PBM_ERROR -- The parameters are invalid, the search cannot be performed, or
               an internal error prevents the PBM from providing a response.\n
@vertspace
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
@vertspace
  PBM_ERROR_NOT_FOUND -- No callback is specified, and either the name was not
                         found in the requested phone book or the phone book
                         is empty.\n
                         If a callback is specified, this function
                         returns PBM_SUCCESS, and the fact that the record was
                         not found is supplied via the callback.\n
@vertspace
  PBM_SUCCESS -- Indicates either the callback was called or a record
                 was found and its results are returned in the data.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_find_name().
*/
extern pbm_return_type pbm_find_name (pbm_device_type pb_id,
                                      char *name,
                                      pbm_record_s_type * data,
                                      PBM_FIND_CB_FUNC proc_func);
/*~ FUNCTION pbm_find_name */
/*~ PARAM name STRING */
/*~ PARAM OUT data POINTER */

/*===========================================================================
FUNCTION: PBM_FIND_NUMBER
===========================================================================*/
/**
  Searches the requested phone books for a specified number. This function
  can be used to display the call information based on the incoming number.

  This function returns the contents of the record matching the phone number
  according to the rules appropriate to the pb_id. For UIM FDN files, if the
  specified number begins with an entry in the FDN, the record matches. For all
  other files, the entire number must match.

  The data is returned in the proc_func parameter . The record pointer passed as
  a parameter in proc_func is only valid for the duration of the callback. If
  clients want to use the data after the callback terminates, they must make a copy.

@vertspace
@param[in]     pb_id           Phone book for which the information is requested.
@param[in]     number          Pointer to the phone number to search for.
@param[in]     loc_number_len  Length of the phone number to search for.
@param[in,out] record          If this parameter is not NULL, the contents of
                               the first record matching the phone number are
                               returned via this parameter.
@param[in]     proc_func       Callback function that returns the contents of the
                               first record matching the specified phone number.

@return
  PBM_ERROR -- The parameters are invalid.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_ERROR_NOT_FOUND -- The phone book is empty, or no matching record is
                         found.\n
  PBM_SUCCESS -- A record was found and its results are returned via either the
                 record or the callback.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_find_number().
*/
extern pbm_return_type pbm_find_number (pbm_device_type pb_id,
                                        const byte *number, int loc_number_len,
                                        pbm_record_s_type * record,
                                        PBM_FIND_CB_FUNC proc_func);
/*~ FUNCTION pbm_find_number */
/*~ PARAM number VARRAY LENGTH loc_number_len */
/*~ PARAM OUT record POINTER */

/*===========================================================================
FUNCTION: PBM_GET_NUM_RECS
===========================================================================*/
/**
  Returns the number of records in use for a specified phone book.

  If pb_id is PBM_DEFAULT, the function returns the total records in use for the
  EFS, speed dial, and ADN phone books.

@param[in] pb_id   Phone book for which the information is requested.

@return
  Number of records in use.\n
  Zero if the phone book is invalid or the files are not initialized.

@dependencies
  The pbm_init() function must be called first.\n
@vertspace
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_get_num_recs().
*/
extern uint16 pbm_get_num_recs(pbm_device_type pb_id);
/*~ FUNCTION pbm_get_num_recs */

/*===========================================================================
FUNCTION: PBM_CALCULATE_SIZE_FROM_ID
===========================================================================*/
/**
  Calculates how much memory is needed for a copy of the field's structure
  based on the record ID.

@param[in] rec_id   Record ID.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_calculate_fields_size_from_id().
*/
int pbm_calculate_fields_size_from_id( uint16 rec_id );
/*~ FUNCTION pbm_calculate_fields_size_from_id */

/*===========================================================================
FUNCTION: PBM_RECORD_READ
===========================================================================*/
/**
  Searches the cache appropriate to the specified phone book and retrieves
  information contained in a specified record.

  This function does not delete the record from the address book. If no
  record is found, the error, PBM_ERROR_NOT_FOUND, is returned.

  The layout of data_buf when it is returned is that it starts with a
  num_fields_ptr number of pbm_field_s_type structures. Following those
  structures is the data for those fields. pbm_get_field_data_ptr()
  can get pointers to the actual field data.

@vertspace
@param[in]  rec_id           ID of the record to be read.
@param[out] category_ptr     Pointer to the record category.
@param[out] num_fields_ptr   Pointer to the number of fields in the record.
@param[out] data_buf         Pointer to the buffer that holds the record data.
@param[out] data_buf_size    Size of the data buffer.

@return
  PBM_ERROR -- The parameters are invalid.\n
@vertspace
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
@vertspace
  PBM_ERROR_NOT_FOUND -- No record with the supplied record_id was found in
                         the specified phone book.\n
@vertspace
  PBM_SUCCESS -- The item was found and the output parameters have been filled.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_record_read().
*/
pbm_return_type pbm_record_read(uint16            rec_id,
                                uint16           *category_ptr,
                                int              *num_fields_ptr,
                                uint8            *data_buf,
                                uint32            data_buf_size);
//change the value of PARAM OUT appropriately whenever PBM_MAX_RECORD_SIZE is changed
/*~ FUNCTION pbm_record_read */
/*~ PARAM OUT category_ptr POINTER */
/*~ PARAM OUT num_fields_ptr POINTER */
/*~ PARAM OUT data_buf VARRAY 1500
    LENGTH data_buf_size */

/*===========================================================================
FUNCTION: PBM_RECORD_WRITE
===========================================================================*/
/**
  Adds, updates, or deletes a record in the address book (both EFS and
  SIM). This interface supports many different fields per contact.

  To add a record:
  -# Set rec_id = 0xFFFF (and rec_id_ptr=&rec_id). After a synchronous call has
     completed, look here for the assigned record ID for the new record.
  -# Set data_buf equal to an array of fields for the new record.
  -# Set num_fields equal to the length of above array.

  To update a record:
  - Use the same steps as adding a record, except set rec_id to the record ID
    of the record to be updated.
  - The fields specified in data_buf replace all fields in the current record,
    i.e., the current fields are deleted and replaced, not added to.

  To delete a record:
  -# Set rec_id to the record ID of the record to be deleted.
  -# Set data_buf = NULL.
  Other fields are not used.

  When making an asynchronous call, a success return value indicates that
  the command has been successfully queued. The actual result of the operation
  is given in the first argument to the callback function, proc_func. The
  resulting record ID is returned synchronously within rec_id_ptr along with
  the proc_func callback.

  The layout of data_buf when it is sent starts with a num_fields_ptr number of
  pbm_field_s_type structures. Following those structures is the data for those
  fields. pbm_get_field_data_ptr() can get pointers to the actual field data.

@vertspace
@param[in]     pb_id          Phone book for which the information is requested.
@param[in,out] rec_id_ptr     Pointer to the record ID being requested. If this
                           parameter contains PBM_GET_FIRST_AVAILABLE, the
                           resulting record ID is returned via this parameter.
@param[in]     cat            Set to the category of the record that was found.
@param[in]     num_fields     Number of fields in the record.
@param[in]     data_buf       Pointer to the data buffer to hold the record.
@param[in]     data_buf_size  Size of the data buffer.
@param[in]     proc_func      Callback function that returns the result of the
                              write command.
@param[in]     user_data      Pointer to the user data.

@return
  The callback function's first parameter contains the result, which is:\n
  - PBM_ERROR -- The PBM failed to write the record.
  - PBM_SUCCESS -- The PBM successfully wrote the record.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_record_write().
*/
extern pbm_return_type pbm_record_write( pbm_device_type         pb_id,
                                         uint16                 *rec_id_ptr,
                                         uint16                  cat,
                                         int                     num_fields,
                                         const uint8            *data_buf,
                                         int                     data_buf_size,
                                         PBM_WRITE_CB_FUNC       proc_func,
                                         void                   *user_data);
/*~ FUNCTION pbm_record_write */
/*~ PARAM INOUT rec_id_ptr POINTER */
/*~ PARAM data_buf VARRAY LENGTH data_buf_size */

/*===========================================================================
FUNCTION: PBM_REG_EVENT_CB
===========================================================================*/
/**
  Allows data services to register a callback to be used when the PBM finishes
  initializing the SIM.

  This function is called when the data services application initializes and
  requests the PBM to call the supplied user_fn when it completes SIM
  initialization. The callback is called after the PBM has initialized all
  available SIM phone books after the initial PIN verification.

@vertspace
@param[in] user_fn   If this parameter is not NULL, this function is called when
                    the PBM finishes initializing. The parameter indicates the
                    state of the ADN file.

@return
  PBM_ERROR -- The parameters are invalid.\n
  PBM_SUCCESS -- Proc_func is not null and will be called.

@dependencies
  None.
*/
extern pbm_return_type pbm_reg_event_cb ( PBM_EVENT_FUNC user_fn );
/*~ FUNCTION pbm_reg_event_cb */

/*===========================================================================
FUNCTION: PBM_WRITE
===========================================================================*/
/**
  Writes the data contents of the specified record to the specified phone book.

  If the phone book is ADN or FDN and the number is zero length, the record
  is simply deleted. For any other phone book, the record is deleted only if
  both the phone number and text are zero length.

  If the index supplied is -1, the PBM attempts to find the first available
  location in the specified phone book to store the data.

@note1hang Currently, this is just a wrapper for pbm_record_write() that
  preserves the legacy interface of one callback function with one argument.

@vertspace
@param[in]     pb_id       Phone book for which the information is requested.
@param[in,out] record      Pointer to the record. If record.index == -1, write
                           to the first empty record. If record.number is NULL,
                           delete entry record.index.
@param[in]     proc_func   Callback function that returns the results of the
                           write command.

@return
  PBM_ERROR -- The parameters are invalid, or the PBM encountered an internal
               error when attempting the write.\n
@vertspace
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
@vertspace
  PBM_ERROR_NOT_FOUND -- The location in the requested phone book is empty
                         and the data indicates the request is to delete
                         the record only.\n
@vertspace
  PBM_ERROR_INDEX_INVALID -- The index provided in the data is beyond the range
                             of entries for the specified file.\n
@vertspace
  PBM_ERROR_MEM_FULL -- There is no free location in the specified phone
                        book, or the number requires a SIM extension record
                        and there is no free location for the record.\n
@vertspace
  PBM_SUCCESS -- The write was performed to the specified device. The results
                 of the write are not known until the callback is called. If
                 proc_func is NULL, the results of the write are not returned.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_write().
*/
pbm_return_type pbm_write ( pbm_device_type pb_id,
                            pbm_record_s_type * record,
                            PBM_WRITE_COMPAT_CB_FUNC proc_func);
/*~ FUNCTION pbm_write */
/*~ PARAM INOUT record POINTER */

/*===========================================================================
FUNCTION: PBM_FILL_FIELD
===========================================================================*/
/**
  This function is a shortcut to fill in the four fields that make up the
  field structure.

@param[out] field        Pointer to the field structure.
@param[in]  field_id     Field ID.
@param[in]  field_type   Field type.
@param[in]  buffer       Pointer to the field data buffer.
@param[in]  data_len     Field length.

@return
  None.

@dependencies
  None.
*/
void pbm_fill_field(pbm_field_s_type *field,
                    pbm_field_id_e_type field_id, pbm_field_type_e_type field_type,
                    const byte *buffer, uint16 data_len);

/*===========================================================================
FUNCTION: PBM_GET_FIELD_DATA_PTR
===========================================================================*/
/**
  Provides a shortcut to return the pointer to the data buffer for
  a specified field number.

@param[in] field_buf   Pointer to the data buffer.
@param[in] field_num   Field number.

@return
  None.

@dependencies
  None.
*/
void * pbm_get_field_data_ptr(const void *field_buf,
                              int field_num);


/*===========================================================================
FUNCTION: PBM_GET_FIELDS_FROM_COMPLEX
===========================================================================*/
/**  
  This function converts a complex field into its constituent simple fields .
  

@param[out] fields_buf        Pointer to the field structure.
@param[out] num_fields      Number of fields in the fields_buf.
@param[in]  complex_field   pointer to the sub-fields.

@return
  None.

@dependencies
  None.
*/
pbm_return_type pbm_get_fields_from_complex(pbm_complex_field_s_type *complex_field,
                                            pbm_field_s_type *fields_buf,
                                            int *num_fields);


/*===========================================================================
FUNCTION: PBM_FILL_COMPLEX_FIELD
===========================================================================*/
/**
  This function is a shortcut to fill in the four fields that make up the
  complex field structure.

@param[out] field        Pointer to the field structure.
@param[in]  num_fields  Number of fields in the data_buf.
@param[in]  data_buf    Pointer to the field data buffer.
@param[in]  data_buf_size     Length of the data_buf.

@return
  None.

@dependencies
  None.
*/
pbm_return_type pbm_fill_complex_field(int         num_fields,
                                  const uint8            *data_buf,
                                  int                     data_buf_size,
                                  pbm_field_s_type       *field);
/*===========================================================================
FUNCTION: PBM_CAL_COMPLEX_FIELD_SIZE
===========================================================================*/
/**
  Calculates how much memory is needed for the fields of sizes.


@param[in]  num_fields       Number of fields in the data_buf.
@param[in]  data_sizes sizes of the fields.

@return
  The accumulation of the combined field sizes.

@dependencies
  None.
*/
uint16 pbm_cal_complex_field_size(int         num_fields,
                                  const uint32            *data_sizes);

/*===========================================================================
FUNCTION: PBM_COMPLEX_FILL_SIZES
===========================================================================*/
/**
  This function fills a sizes array, so that a normal fields structure can
  be created with pbm_allocate_fields().

@param[out] sizes        Pointer to the array of sizes.
@param[in]  max_num_sub_fields       Number of sub-fields in the data_buf.
@param[in]  complex_field  pointer to the sub-fields.

@return
  The number of total fields filled

@dependencies
  None.
*/
int pbm_complex_fill_sizes(uint32 *sizes,
                                  int max_num_sub_fields,
                                  pbm_complex_field_s_type *complex_field);
/*===========================================================================
FUNCTION: PBM_COMPLEX_NUM_SUB_FIELDS
===========================================================================*/
/**
  This API returns the number of sub fields present in the complex field.
  
@param[in]  complex_field  pointer to the sub-fields.

@return
  Number of sub-fields in the complex field.

@dependencies
  None.
*/

int pbm_complex_num_sub_fields(pbm_complex_field_s_type *complex_field);

/*===========================================================================
FUNCTION: PBM_FREE_FIELDS
===========================================================================*/
/**
  Frees the memory allocated by pbm_allocate_fields.

@param[in] fields       Fields that were allocated memory.
@param[in] num_fields   Number of fields.

@return
  None.

@dependencies
  None.
*/
void pbm_free_fields( uint8 **fields, int num_fields);

/*===========================================================================
FUNCTION: PBM_ALLOCATE_FIELDS
===========================================================================*/
/**
  Allocates the memory required for the field array in the cache structure.

@param[in]  fields_buf_ptr    Pointer to the field array.
@param[out] fields_buf_size   Size of the fields buffer. Zeroes values are not
                              to be passed as members in this array.
@param[in]  num_fields        Number of fields in the array.
@param[in]  sizes             Memory size to be allocated.

@return
  TRUE -- Memory allocation is successful.\n
  FALSE -- Memory allocation failed.

@dependencies
  None.
*/
boolean pbm_allocate_fields(uint8 **fields_buf_ptr,
                            uint32 *fields_buf_size,
                            uint32 num_fields,
                            const uint32 *sizes);

/*===========================================================================
FUNCTION: PBM_LOCATION_TO_RECORD_ID
===========================================================================*/
/**
  Converts a phone book ID plus a location to a record ID.

@param[in] pb_id      Phone book for which the information is requested.
@param[in] location   Location.

@return
  The record ID corresponding to the location being passed.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_record_id_to_location().
*/
uint16 pbm_location_to_record_id(pbm_device_type pb_id, int location);

/*===========================================================================
FUNCTION: PBM_RECORD_ID_TO_LOCATION
===========================================================================*/
/**
  Converts a record ID to a location (extracts the location information from
  the record ID).

@param[in] record_id   Record ID to be converted.

@return
  Location corresponding to the record ID being passed.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_record_id_to_location().
*/
uint16 pbm_record_id_to_location(uint16 record_id);

/*===========================================================================
FUNCTION: PBM_RECORD_ID_TO_PHONEBOOK
===========================================================================*/
/**
  Converts a record ID to a phone book ID (extracts the phone book ID
  information from the record ID).

@param[in] record_id   Record ID to be converted.

@return
  The phone book ID corresponding to the record ID being passed.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_record_id_to_phonebook_type().
*/
pbm_device_type pbm_record_id_to_phonebook(uint16 record_id);

/*===========================================================================
FUNCTION: PBM_WRITE_LOCK
===========================================================================*/
/**
  Locks further writes to the PBM.

  This function returns an error if it is unable to acquire the lock because,
  by default, the function is always nonblocking.

  The lock type specifies whether other tasks are blocked by this lock or not.
  If the lock type is blocking, other tasks are blocked until the task that owns
  the lock unlocks PBM write. If the lock type is unblocking, writes from other
  tasks will result in an error.

@vertspace
@param[in]  type        Lock type.
@param[in]  user_data   Pointer to the user data.
@param[out] func        Callback function to ensure the lock operation completed.

@return
  PBM_SUCCESS -- Acquired a nonblocking lock.
  PBM_ERROR -- Was unable to acquire a nonblocking lock.\n

@dependencies
  None.
*/
pbm_return_type pbm_write_lock(pbm_lock_type_e_type type,
                               void *user_data,
                               PBM_WRITE_CB_FUNC func);
/*~ FUNCTION pbm_write_lock */

/*===========================================================================
FUNCTION: PBM_WRITE_UNLOCK
===========================================================================*/
/**
  Unlocks PBM writes.

@return
  Returns an error if it is unable to release the nonblocking lock. Otherwise,
  it returns PBM_SUCCESS.

@dependencies
  None.
*/
pbm_return_type pbm_write_unlock(void);
/*~ FUNCTION pbm_write_unlock */

/*===========================================================================
FUNCTION: PBM_GET_FIELD_INFO_COUNT
===========================================================================*/
/**
  Returns the number of PBM fields supported by the specified PBM phone book
  or category.

@param[in]  pb_id   Phone book ID.
@param[in]  cat     Category for which information is requested.
@param[out] num     Pointer to the number of fields supported by this category.

@return
  PBM_SUCCESS -- The operation was successful.\n
  PBM_ERROR_NOT_READY -- The requested phone book is not ready.\n
  PBM_ERROR -- Other error.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_get_field_info_count().
*/
pbm_return_type pbm_get_field_info_count(pbm_device_type pb_id, pbm_cat_e_type cat,
                                         int *num);
/*~ FUNCTION pbm_get_field_info_count */
/*~ PARAM OUT num POINTER */

/*===========================================================================
FUNCTION: PBM_GET_FIELD_INFO
===========================================================================*/
/**
  Returns field information associated with each PBM field
  supported by the specified category.

@param[in]  pb_id   Phone book device for which information is requested.
@param[in]  cat     Category for which information is requested.
@param[out] pf      Pointer to the array of field information that is filled by
                    the PBM.
@param[in]  size    Size of the pf array in bytes.

@return
  PBM_SUCCESS -- The operation was successful.\n
  PBM_ERROR_NOT_READY -- The requested phone book is not ready.\n
  PBM_ERROR -- Other error.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_get_field_info().
*/
pbm_return_type pbm_get_field_info(pbm_device_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size);
//change the value of PARAM OUT appropriately whenever PBM_MAX_NUM_FIELDS is changed
/*~ FUNCTION pbm_get_field_info */
/*~ PARAM OUT pf VARRAY 80
    LENGTH (size/sizeof(pbm_field_info_s_type)) */

/*===========================================================================
FUNCTION: PBM_RECORD_VALIDATE
===========================================================================*/
/**
  Checks whether a specified record is valid. The validation includes the
  following:

  - Checks whether fields that are required are included. For ADN or FDN phone
    books, there should be a non-NULL number string. For other phone books,
    there should be a non-NULL number OR non-NULL name string.
  - Checks whether the maximum number of a particular field, the maximum size of
    a particular field, or the maximum size of the entire record is exceeded.
  - Checks whether all fields are supported for this phone book and whether the
    types of fields match the field ID.

@vertspace
@param[in] pb_id           Phone book for which the information is requested.
@param[in] rec_id          Record ID.
@param[in] cat             Category of the record.
@param[in] data_buff       Pointer to the data buffer fields.
@param[in] data_buf_size   Size of the data buffer.
@param[in] num_fields      Number of fields.

@return
  PBM_ERROR_INDEX_INVALID -- The record contains an invalid index field or
                             an invalid record ID.\n
  PBM_ERROR_TEXT_TOO_LONG -- The name is too long.\n
  PBM_ERROR_NUMBER_TOO_LONG -- The number is too long.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_ERROR_NO_SIM -- The corresponding UIM is not present.\n
  PBM_ERROR_NOT_AVAILABLE -- The corresponding UIM file is not available.\n
  PBM_ERROR_RESTRICTED -- The UIM file is not available due to a restriction.\n
  PBM_ERROR_INVALID_REC -- The record is not valid for other reasons.\n
  PBM_SUCCESS -- The record is valid.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_record_validate().
*/
pbm_return_type pbm_record_validate(pbm_device_type pb_id,
                                    uint16 rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *data_buf,
                                    int data_buf_size,
                                    int num_fields);
/*~ FUNCTION pbm_record_validate */
/*~ PARAM data_buf VARRAY LENGTH data_buf_size */

/*===========================================================================
FUNCTION: PBM_CLEAR_PHONEBOOK
===========================================================================*/
/**
  Clears all entries in a specified phone book.

@param[in] pb_id   Phone book for which the information is requested.

@return
  PBM_SUCCESS -- Cleared all entries in the specified phone book.\n
  PBM_ERROR -- Did not clear all entries in the specified phone book.

@dependencies
  If the software can support multiple applications (GW&GW, GW&1X, or 1X&1X) or
  multiple slots, do not use this function. Instead, use the equivalent
  session-based function, pbm_session_clear_phonebook().
*/
pbm_return_type pbm_clear_phonebook(pbm_device_type pb_id);
/*~ FUNCTION pbm_clear_phonebook */

boolean pbm_session_emergency_number_cat(pbm_session_enum_type sess_type,const byte *num, byte len, uint8 *ecc_category_ptr);
/*~ FUNCTION pbm_session_emergency_number_cat */
/*~ PARAM num VARRAY LENGTH len */
/*~ PARAM OUT ecc_category_ptr POINTER */

pbm_session_enum_type  pbmif_mmgsdi_session_to_pbm_session(mmgsdi_session_type_enum_type mmgsdi_session);


pbm_return_type pbm_session_emergency_number_cat_ext(pbm_session_enum_type sess_type,
                                                                  const byte *num, 
                                                                  byte len, 
                                                                  boolean *found_in_gw,
                                                                  boolean *found_in_1x,
                                                                  uint8 *ecc_category_ptr_gw,
                                                                  uint8 *ecc_category_ptr_1x);	


/*===========================================================================
FUNCTION: PBM_NUMBER_MATCH
===========================================================================*/
/**
  Performs an exact match of a phone number, ignoring any nondialed characters.

@param[in] sim_num          Pointer to the SIM number.
@param[in] sim_num_len      Number of digits in the SIM number.
@param[in] dialed_num       Pointer to the dialed number.
@param[in] dialed_num_len   Number of digits in the dialed number.

@return
  TRUE -- The numbers matched.\n
  FALSE -- The numbers did not match.

@dependencies
  None.
*/
boolean pbm_number_match(const byte *sim_num,    byte sim_num_len,
                         const byte *dialed_num, byte dialed_num_len);
/*~ FUNCTION pbm_number_match */
/*~ PARAM sim_num VARRAY LENGTH sim_num_len */
/*~ PARAM dialed_num VARRAY LENGTH dialed_num_len */

/*===========================================================================
FUNCTION: PBM_FDN_NUMBER_MATCH
===========================================================================*/
/**
  Performs a match of an FDN phone number.

  The rule for FDN is that the digits in the SIM must match, but digits are
  allowed at the end.

@vertspace
@param[in] sim_num          Pointer to the SIM number.
@param[in] sim_num_len      Number of digits in the SIM number.
@param[in] dialed_num       Pointer to the dialed number.
@param[in] dialed_num_len   Number of digits in the dialed number.

@return
  TRUE -- The numbers matched.\n
  FALSE -- The numbers did not match.

@dependencies
  None.
*/
boolean pbm_fdn_number_match(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len);
/*~ FUNCTION pbm_fdn_number_match */
/*~ PARAM sim_num VARRAY LENGTH sim_num_len */
/*~ PARAM dialed_num VARRAY LENGTH dialed_num_len */


/*===========================================================================
FUNCTION: PBM_NOTIFY_REGISTER
===========================================================================*/
/**
  Registers a callback function that is used to notify of PBM events. The user
  input data is passed on when the notification callback is called.

@param[in] func        Callback function.
@param[in] user_data   Pointer to the user data.

@return
  PBM_SUCCESS -- The specified client was registered to receive PBM events.\n
  PBM_ERROR -- The specified client was not registered to receive PBM events.

@dependencies
  None.

@sa
  pbm_notify_unregister()
*/
pbm_return_type pbm_notify_register(PBM_NOTIFY_FUNC func, void *user_data);
/*~ FUNCTION pbm_notify_register */

/*===========================================================================
FUNCTION: PBM_NOTIFY_UNREGISTER
===========================================================================*/
/**
  Cancels the notification function registered with the PBM. The function pointer
  and user data provided as input should match the registered function pointer
  and user data.

@vertspace
@param[in] func        Function pointer.
@param[in] user_data   Pointer to the user data.

@return
  PBM_SUCCESS -- The notification function was cancelled.\n
  PBM_ERROR -- The notification function was not cancelled.

@dependencies
  None.

@sa
  pbm_notify_register()
*/
pbm_return_type pbm_notify_unregister(PBM_NOTIFY_FUNC func, void *user_data);
/*~ FUNCTION pbm_notify_unregister */

/*===========================================================================
FUNCTION: PBM_HAVE_UIDS_CHANGED
===========================================================================*/
/**
  Checks whether the phone Unique IDs (UIDs) have changed.

@return
  TRUE -- The phone UIDs have changed.\n
  FALSE -- The phone UIDs have not changed.

@dependencies
  None.

@sa
  pbm_validate_uid_changes()
*/
boolean pbm_have_uids_changed(void);
/*~ FUNCTION pbm_have_uids_changed */

/*===========================================================================
FUNCTION: PBM_VALIDATE_UID_CHANGES
===========================================================================*/
/**
  Acknowledges that the phone UIDs have changed.

@return
  PBM_SUCCESS -- Successfully acknowledged that the UIDs have changed.\n
  PBM_ERROR -- Failed to acknowledge that the UIDs have changed.

@dependencies
  None.

@sa
  pbm_have_uids_changed()
*/
pbm_return_type pbm_validate_uid_changes(void);
/*~ FUNCTION pbm_validate_uid_changes */

#define PBM_UNDEFINED_CHAR 0xA0
  /**< Nonblocking space fills characters that cannot be converted. */

/*===========================================================================
FUNCTION: pbmutils_ucs2_to_pbm8
===========================================================================*/
/**
  Converts a UCS2 string to PBM8.

  This function tries to convert the entire string, but if there are
  unrecognized characters, it fills them with the UNDEFINED_CHAR define
  octet (nonblocking space). The function leaves the resulting string as
  NULL-terminated, truncating the source if necessary.

@vertspace
@param[in]  ucs2_str       Pointer to the array of UCS2 characters to convert
                           to PBM8.
@param[in]  ucs_str_len    Length of the array of UCS2 characters.
@param[out] pbm8_str       Pointer to the array where the conversion is saved.
@param[in]  pbm8_str_len   Length of the results string.

@return
  TRUE -- All characters in the ucs2_str are in the resulting pbm8_str.\n
  FALSE -- Some characters cannot be converted, or the buffer is full.

@dependencies
  None.
*/
boolean pbmutils_ucs2_to_pbm8(const uint16 *ucs2_str, uint16 ucs_str_len,
                              uint8 *pbm8_str, uint16 pbm8_str_len);

/*===========================================================================
FUNCTION: pbmutils_pbm8_to_ucs2
===========================================================================*/
/**
  Converts a PBM8 string to UCS2. This function leaves the resulting string as
  NULL terminated, truncating the source if necessary.

@vertspace
@param[in]  pbm8_str       Pointer to the PBM8 string to convert.
@param[in]  pbm8_str_len   Length of the string.
@param[out] ucs2_str       Pointer to the buffer containing the conversion.
@param[in]  ucs2_str_len   Size of the resulting buffers. The actual size of the
                           memory must be double this value because each item is
                         16 bits large.

@return
  TRUE -- All characters in the pbm8_string are in the resulting ucs2_str.\n
  FALSE -- The buffer ran out of space.

@dependencies
  None.
*/
boolean pbmutils_pbm8_to_ucs2( uint8 *pbm8_str,  uint16 pbm8_str_len,
                               uint16 *ucs2_str, uint16 ucs2_str_len);


/*===========================================================================
FUNCTION: pbmutils_wstrlen;
===========================================================================*/
/**
  Counts the number of UCS2 characters in a string. The
  string must be terminated by 0x0000.

@param[in] ucs2_str   Pointer to the UCS2 string in which to count the characters.

@return
  The number of UCS2 characters in the specified string.

@dependencies
  None.
*/
uint32 pbmutils_wstrlen(const uint16 *ucs2_str);


/*===========================================================================
FUNCTION: PBM_SESSION_ENUM_REC_INIT
===========================================================================*/
/**
  Performs a search for specific records in a phone book and sets up a list of
  records with the matching criteria. This list is used in subsequent calls to
  pbm_enum_next_rec(), which iterates through the records that match the given
  criteria. Search results are stored in static variables and are overwritten
  by the next call to pbm_rec_init().

  Following are the searching rules:
  - Only records with the given category are matched. If PBM_CAT_NONE is
    specified, all categories are matched.
  - Only records that contain at least one instance of a field with the
    specified field ID are matched. If PBM_FIELD_NONE is specified, this
    criterion is ignored.
  - Only records with data in the specified field that matches the data
    specified by data_ptr are matched. If data_ptr is NULL, this criterion
    is ignored.

  The type of matching depends on the flag settings:
  - PBM_CASE_SENSITIVE_MATCH and PBM_CASE_INSENSITIVE_MATCH control
    case sensitivity.
  - PBM_EXACT_MATCH and PBM_STARTS_WITH_MATCH control how much of the data
    specified by data_ptr must be the same to be considered a match.

  Example:  pbm_rec_init(PBM_EFS, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
            PBM_CASE_INSENSITIVE_MATCH | PBM_STARTS_WITH_MATCH)
            matches on the names John Smith and Johann Bach.

@vertspace
@param[in] pb_id       Phone book for which the information is requested.
@param[in] category    Category of the record.
@param[in] field_id    Field ID that the data pointer must match.
@param[in] data_ptr    Pointer to the contents of the field that must match.
                       The data supplied must be the correct type for the
                       field_id.
@param[in] data_size   Number of bytes pointed to by data_ptr.
@param[in] flags       Modifiers for the search. At most, one from each
                      category can be numerically ORed together to create
                      the result.

@return
  PBM_ERROR -- The parameters are invalid or the PBM has encountered an
               internal error. \n
  PBM_SUCCESS -- Indicates the search has been performed. The results
                 of the search itself are returned via pbm_enum_next_rec.

@dependencies
  The pbm_init() function must be called first.
*/
extern pbm_return_type pbm_session_enum_rec_init(pbm_phonebook_type pb_id,
                                         uint16 category,
                                         pbm_field_id_e_type field_id,
                                         const uint8 *data_ptr,
                                         uint16 data_size,
                                         uint32 flags);
/*~ FUNCTION pbm_session_enum_rec_init */
/*~ PARAM data_ptr VARRAY LENGTH data_size */

/*===========================================================================
FUNCTION: PBM_SESSION_ENUM_NEXT_REC_ID
===========================================================================*/
/**
  Retrieves the next record ID that matches the search criteria specified in
  the most recent call to pbm_enum_rec_init().

  Only one search at a time is supported. This function cannot allow situations
  where one application has called pbm_enum_rec_init() and is in the process of
  calling pbm_enum_next_rec_id(), and another application calls
  pbm_enum_rec_init(), changes the search criteria, and resets the search before
  the first application is done. If this happens, the first application may not
  get the expected records.

  When the end of the enumeration is reached, the index is not reset to the
  beginning of the enumeration. All subsequent calls to this function must
  continue to return PBM_EOF. The caller must call pbm_enum_rec_init() to
  reinitialize the search criteria.

@vertspace
@param[out] rec_id_ptr   Pointer to the record ID.

@return
  PBM_SUCCESS -- The operation was successful.\n
  PBM_ERROR -- The operation failed.

@dependencies
  The pbm_enum_rec_init() function must be called first.
*/
pbm_return_type pbm_session_enum_next_rec_id(pbm_record_id_type *rec_id_ptr);
/*~ FUNCTION pbm_session_enum_next_rec_id */
/*~ PARAM OUT rec_id_ptr POINTER */

/*===========================================================================
FUNCTION: PBM_SESSION_ENUM_REC_INIT_EXT
===========================================================================*/
/**
  Performs a search for specific records in a phone book and sets up a list of
  records with the matching criteria. This list is used in subsequent calls
  to pbm_session_enum_next_rec_ext(), which iterates through the records that
  match the given criteria.

  This function is extended from pbm_session_enum_rec_init() so that rec_init()
  can be called simultaneously by more than one client. The maximum number of
  clients that can use the pbm_session_enum_rec_init() service is
  PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT (i.e., 10 for initial usage).

  Following are the searching rules:
  - Only records with the given category are matched. If PBM_CAT_NONE is
    specified, all categories are matched.
  - Only records that contain at least one instance of a field with the
    specified field ID are matched. If PBM_FIELD_NONE is specified, this
    criterion is ignored.
  - Only records with data in the specified field that matches the data
    specified by data_ptr are matched. If data_ptr is NULL, this criterion
    is ignored.

  The type of matching depends on the flag settings:
  - PBM_CASE_SENSITIVE_MATCH and PBM_CASE_INSENSITIVE_MATCH control
    case sensitivity.
  - PBM_EXACT_MATCH and PBM_STARTS_WITH_MATCH control how much of the data
    specified by data_ptr must be the same to be considered a match.

  Example:  pbm_rec_init(PBM_EFS, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
            PBM_CASE_INSENSITIVE_MATCH | PBM_STARTS_WITH_MATCH)
            matches on the names John Smith and Johann Bach.

@vertspace
@param[in]  pb_id       Phone book for which the information is requested.
@param[in]  category    Category of the record.
@param[in]  field_id    Field ID that the data pointer must match.
@param[in]  data_ptr    Pointer to the contents of the field that must match.
                        The data supplied must be the correct type for the
                        field_id.
@param[in]  data_size   Number of bytes pointed to by data_ptr.
@param[in]  flags       Modifiers for the search. At most, one from each
                      category can be numerically ORed together to create
                      the result.
@param[out] handle      Pointer to the handle filled by the PBM for the
                        current client. This handle is used by the clients
                        in subsequent calls to pbm_session_enum_next_rec_id_ext().
                        The handle is filled with the value 0xff in case of
                        an error.

@return
  PBM_ERROR -- The parameters are invalid, or the PBM has encountered an
               internal error. \n
@vertspace
  PBM_SUCCESS -- The search has been performed. The results of the search are
                 returned via pbm_enum_next_rec().

@dependencies
  The pbm_init() function must be called first.
*/
extern pbm_return_type pbm_session_enum_rec_init_ext(pbm_phonebook_type pb_id,
                                         uint16 category,
                                         pbm_field_id_e_type field_id,
                                         const uint8 *data_ptr,
                                         uint16 data_size,
                                         uint32 flags,
                                         uint8 *handle);
/*~ FUNCTION pbm_session_enum_rec_init_ext */
/*~ PARAM OUT handle POINTER */
/*~ PARAM data_ptr VARRAY LENGTH data_size */

/*===========================================================================
FUNCTION: PBM_SESSION_ENUM_REC_INIT_EXT_FREE_HANDLE
===========================================================================*/
/**
  Called by clients to free the handle that has been allocated to
  the client via pbm_session_enum_rec_init_ext().

  This function is to be used in case all the records are not read via
  pbm_session_enum_next_rec_id_ext(). If all the records are read, the handle
  is automatically freed. The handle is marked as not in use, and memory allocated
  to the array containing the record IDS is deallocated.

@vertspace
@param[in] handle   Handle to be freed.

@return
  PBM_SUCCESS -- The operation was successful.\n
  PBM_ERROR -- The operation failed.

@dependencies
  The pbm_session_enum_rec_init_ext() function must be called first to retrieve
  the handle.\n
@vertspace
  If clients have read all the records and PBM_EOF has been returned to them 
  once, they are not to call this function to free the handle. Because the PBM_EOF
  handle might have been assigned to another client, calling this function will
  free the handle assigned to the other client.
*/
pbm_return_type pbm_session_enum_rec_init_ext_free_handle( uint8 handle);
/*~ FUNCTION pbm_session_enum_rec_init_ext_free_handle */

/*===========================================================================
FUNCTION: PBM_SESSION_ENUM_NEXT_REC_ID_EXT
===========================================================================*/
/**
  Retrieves the next record ID that matches the search criteria specified in
  the call to pbm_session_enum_rec_init_ext() for the passed handle.

  When the end of the enumeration is reached, the index is not reset to the
  beginning of the enumeration. Once all the records are read, this function
  returns PBM_EOF. A client is not to call this function after recieving PBM_EOF
  Because it may return stale data. The caller must call
  pbm_session_enum_rec_init_ext() to reinitialize the search criteria.

@vertspace
@param[out] rec_id_ptr   Pointer to the record ID.
@param[in]  handle       Passed handle.

@return
  PBM_SUCCESS -- The operation was successful.\n
  PBM_ERROR -- The operation failed.

@dependencies
  The pbm_session_enum_rec_init_ext() function must be called first.
*/
pbm_return_type pbm_session_enum_next_rec_id_ext(pbm_record_id_type *rec_id_ptr, 
                                                 uint8 handle);
/*~ FUNCTION pbm_session_enum_next_rec_id_ext */
/*~ PARAM OUT rec_id_ptr POINTER */


/*===========================================================================
FUNCTION: PBM_SESSION_FIND_LOCATION
===========================================================================*/
/**
  Returns the contents of the record at the specified index via the data
  parameter.

@param[in]     pb_id   Phone book for which the information is requested.
@param[in]     index   Location of the requested record.
@param[in,out] data    Pointer to the record contents. If this parameter is not
                       NULL, the contents of the record at this location are
                       written to this structure.

@return
  PBM_ERROR -- The parameters are invalid.\n
@vertspace
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
@vertspace
  PBM_ERROR_NOT_FOUND -- The location in the requested phone book is empty and
                         no callback is specified.\n
                         If a callback is specified, this function returns
                         PBM_SUCCESS, and the fact that the record was not
                         found is supplied via the callback.\n
@vertspace
  PBM_SUCCESS -- The record was found and its results are returned in the data.

@dependencies
  None.
*/
extern pbm_return_type pbm_session_find_location (pbm_phonebook_type pb_id,
                                          int index,
                                          pbm_record_s_type * data);
/*~ FUNCTION pbm_session_find_location */
/*~ PARAM OUT data POINTER */

/*===========================================================================
FUNCTION: PBM_SESSION_FIND_NAME
===========================================================================*/
/**
  Calls the function defined in the proc_func parameter for each matching
  occurrence of the string, <i>name</i>.

  If no match is found, PBM_ERROR_NOT_FOUND is returned. Otherwise, each
  matching record is returned until PBM_EOF is reached.

  The data is returned in proc_func. The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback. If clients want
  to use the data after the callback terminates, they must make a copy.

@vertspace
@param[in] pb_id       Phone book ID.
@param[in] name        Pointer to the name of the search string.
@param[in] data        Pointer to the returned data.
@param[in] proc_func   Callback function that returns the record pointer.

@return
  PBM_ERROR -- The parameters are invalid, the search cannot be performed, or
               an internal error prevents the PBM from providing a response.\n
@vertspace
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
@newpage
  PBM_ERROR_NOT_FOUND -- No callback was specified, and either the name was
                         not found in the requested phone book or the phone book
                         is empty.\n
                         If a callback is specified, this function returns
                         PBM_SUCCESS, and the fact that the record was not
                         found is supplied via the callback.\n
@vertspace
  PBM_SUCCESS -- The callback is called, or a record is found and its results
                 are returned in the data.

@dependencies
  None.
*/
extern pbm_return_type pbm_session_find_name (pbm_phonebook_type pb_id,
                                      char *name,
                                      pbm_record_s_type * data,
                                      PBM_FIND_CB_FUNC proc_func);
/*~ FUNCTION pbm_session_find_name */
/*~ PARAM name STRING */
/*~ PARAM OUT data POINTER */

/*===========================================================================
FUNCTION: PBM_SESSION_FIND_NUMBER
===========================================================================*/
/**
  Calls the function defined in the proc_func parameter for each matching
  occurrence of the string <i>number</i>.

  If no match is found, PBM_EOF is returned. Otherwise, each matching record
  is returned until PBM_EOF is reached.

  The data is returned in proc_func. The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback. If clients want
  to use the data after the callback terminates, they must make a copy.

@vertspace
@param[in]  pb_id            Phone book ID.
@param[in]  number           Pointer to the number string to match.
@param[in]  loc_number_len   Length of the number string.
@param[out] record           Pointer to the record.
@param[in]  proc_func        Callback function that returns the record pointer.

@return
  PBM_ERROR -- The parameters are invalid.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_ERROR_NOT_FOUND -- The phone book is empty or no matching record is found.\n
  PBM_SUCCESS -- A record was found and its results are returned via either the
                 record or the callback.

@dependencies
  None.
*/
extern pbm_return_type pbm_session_find_number (pbm_phonebook_type pb_id,
                                        const byte *number, int loc_number_len,
                                        pbm_record_s_type * record,
                                        PBM_FIND_CB_FUNC proc_func);
/*~ FUNCTION pbm_session_find_number */
/*~ PARAM number VARRAY LENGTH loc_number_len */
/*~ PARAM OUT record POINTER */

/*===========================================================================
FUNCTION: PBM_SESSION_GET_NUM_RECS
===========================================================================*/
/**
  Gets the total number of records in the specified phone book.

@param[in] pb_id   Phone book ID.
@newpage
@return
  Number of records in use.\n
  Zero if the phone book is invalid or the files are not initialized.

@dependencies
  The pbm_init() function must be called first.
*/
extern uint16 pbm_session_get_num_recs(pbm_phonebook_type pb_id);
/*~ FUNCTION pbm_session_get_num_recs */

/*===========================================================================
FUNCTION: PBM_SESSION_CALCULATE_FIELDS_SIZE_FROM_ID
===========================================================================*/
/**
  Calculates how much memory is needed for a copy of the fields structure
  based on the record ID.

@param[in] rec_id   Record ID.

@return
  The accumulation of the combined field sizes returns 0 if the
  corresponding record cannot be located.

@dependencies
  None.
*/
int pbm_session_calculate_fields_size_from_id( pbm_record_id_type rec_id );

/*===========================================================================
FUNCTION: PBM_SESSION_CALCULATE_UNDECODED_FIELDS_SIZE_FROM_ID
===========================================================================*/
/**
  Calculates how much memory is needed for a copy of the fields structure
  based on the record ID.

@param[in] rec_id   Record ID.

@return
  The accumulation of the combined field sizes returns 0 if the
  corresponding record cannot be located.

@dependencies
  None.
*/
int pbm_session_calculate_undecoded_fields_size_from_id( pbm_record_id_type rec_id );

/*~ FUNCTION pbm_session_calculate_fields_size_from_id */

/*===========================================================================
FUNCTION: PBM_SESSION_RECORD_READ
===========================================================================*/
/**
  Searches the cache appropriate to the specified phone book and retrieves
  information contained in a specified record, returning a success/failure
  indication.

  This function does not delete the record from the address book. If no
  record is found, PBM_ERROR_NOT_FOUND is returned.

  The layout of data_buf when it is returned starts with a num_fields_ptr
  number of pbm_field_s_type structures. Following those structures is the
  data for those fields. pbm_get_field_data_ptr() can get pointers to
  the actual field data.

@vertspace
@param[in]  rec_id           Record ID.
@param[out] category_ptr     Pointer to the category defined in pbm_cat_e_type.
@param[out] num_fields_ptr   Pointer to the number of fields in the returned
                             record.
@param[out] data_buf         Pointer to the data buffer fields.
@param[in]  data_buf_size    Size of the data buffer.

@return
  PBM_ERROR_INDEX_INVALID -- The record contains an invalid index field or
                             an invalid record ID.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_ERROR_NO_SIM -- The corresponding UIM is not present.\n
  PBM_ERROR_NOT_AVAILABLE -- the corresponding UIM file is not available.\n
  PBM_ERROR_RESTRICTED -- The UIM file is not available due to a restriction.\n
  PBM_ERROR_INVALID_REC -- The record is not valid for this phone book due
                           to other reasons.\n
  PBM_SUCCESS -- The record is valid.

@dependencies
  None.
*/
pbm_return_type pbm_session_record_read(pbm_record_id_type            rec_id,
                                uint16           *category_ptr,
                                int              *num_fields_ptr,
                                uint8            *data_buf,
                                uint32            data_buf_size);

/*===========================================================================
FUNCTION: PBM_SESSION_RECORD_READ_UNDECODED
===========================================================================*/
/**
  Searches the cache appropriate to the specified phone book and retrieves
  undecoded information contained in a specified record, returning a 
  success/failure indication.

  This function does not delete the record from the address book. If no
  record is found, PBM_ERROR_NOT_FOUND is returned.

  The layout of data_buf when it is returned starts with a num_fields_ptr
  number of pbm_field_s_type structures. Following those structures is the
  data for those fields. pbm_get_field_data_ptr() can get pointers to
  the actual field data.

@vertspace
@param[in]  rec_id           Record ID.
@param[out] category_ptr     Pointer to the category defined in pbm_cat_e_type.
@param[out] num_fields_ptr   Pointer to the number of fields in the returned
                             record.
@param[out] data_buf         Pointer to the data buffer fields.
@param[in]  data_buf_size    Size of the data buffer.

@return
  PBM_ERROR_INDEX_INVALID -- The record contains an invalid index field or
                             an invalid record ID.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_ERROR_NO_SIM -- The corresponding UIM is not present.\n
  PBM_ERROR_NOT_AVAILABLE -- the corresponding UIM file is not available.\n
  PBM_ERROR_RESTRICTED -- The UIM file is not available due to a restriction.\n
  PBM_ERROR_INVALID_REC -- The record is not valid for this phone book due
                           to other reasons.\n
  PBM_SUCCESS -- The record is valid.

@dependencies
  None.
*/
pbm_return_type pbm_session_record_read_undecoded(pbm_record_id_type  rec_id,
                                uint16           *category_ptr,
                                int              *num_fields_ptr,
                                uint8            *data_buf,
                                uint32            data_buf_size);

//change the value of PARAM OUT appropriately whenever PBM_MAX_RECORD_SIZE is changed
/*~ FUNCTION pbm_session_record_read */
/*~ PARAM OUT category_ptr POINTER */
/*~ PARAM OUT num_fields_ptr POINTER */
/*~ PARAM OUT data_buf VARRAY 1500 LENGTH data_buf_size */

/*===========================================================================
FUNCTION: PBM_SESSION_RECORD_WRITE
===========================================================================*/
/**
  Adds, updates, or deletes a record in the address book (both EFS and
  SIM). This interface supports many different fields per contact.

  To add a record:
  -# Set rec_id = 0xFFFF (and rec_id_ptr=&rec_id). After a synchronous call has
     completed, look here for the assigned record ID for the new record.
  -# Set items_ptr equal to an array of fields for the new record
  -# Set item_count equal to the length of above array

  To update a record:
  - Use the same steps as when adding a record, except set rec_id to the
    record ID of the record to be updated.
  - The fields specified in items_list will replace all fields in the current
    record, i.e., the current fields will be deleted and replaced, not added to.

  To delete a record:
  -# Set rec_id to the record ID of the record to be deleted.
  -# Set data_buf = NULL.
  -# Other fields are not used.

  When making an asynchronous call, a success return value indicates that
  the command has been successfully queued. The actual result of the operation
  is given in the first argument to the callback function, proc_func. The
  resulting record ID is returned synchronously within rec_id_ptr and along
  with the proc_func callback.

  The layout of data_buf when it is sent starts with a num_fields_ptr number of
  pbm_field_s_type structures. Following those structures is the data for those
  fields. pbm_get_field_data_ptr() can get pointers to the actual field data.

@vertspace
@param[in]     pb_id           Phone book ID.
@param[in,out] rec_id_ptr      Pointer to the resulting record ID. (See the
                               steps above for adding/updating/deleting a
                               record).
@param[in]     cat             Category of the record.
@param[in]     num_fields      Number of fields in the record.
@param[in]     data_buf        Pointer to the data buffer fields.
@param[in]     data_buf_size   Size of the data buffer.
@param[in]     proc_func       Callback function that returns the result of the
                               operation.
@param[in]     user_data       Pointer to the user data.

@return
  PBM_ERROR_INDEX_INVALID -- The record contains an invalid index field or
                             an invalid record ID.\n
  PBM_ERROR_TEXT_TOO_LONG -- The name is too long.\n
  PBM_ERROR_NUMBER_TOO_LONG -- The number is too long.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_ERROR_NO_SIM -- The corresponding UIM is not present.\n
  PBM_ERROR_NOT_AVAILABLE -- The corresponding UIM file is not available.\n
  PBM_ERROR_RESTRICTED -- The UIM file is not available due to a restriction.\n
  PBM_ERROR_INVALID_REC -- The record is not valid for this phone book due
                           to other reasons.\n
  PBM_SUCCESS -- The record is valid.

@dependencies
  None.
*/
extern pbm_return_type pbm_session_record_write( pbm_phonebook_type      pb_id,
                                         pbm_record_id_type     *rec_id_ptr,
                                         uint16                  cat,
                                         int                     num_fields,
                                         const uint8            *data_buf,
                                         int                     data_buf_size,
                                         PBM_WRITE_CB_FUNC       proc_func,
                                         void                   *user_data);
/*~ FUNCTION pbm_session_record_write */
/*~ PARAM INOUT rec_id_ptr POINTER */
/*~ PARAM data_buf VARRAY LENGTH data_buf_size */


/*===========================================================================
FUNCTION: PBM_SESSION_WRITE
===========================================================================*/
/**
  Writes a record to the specified phone book.

@note1hang Currently, this is just a wrapper for pbm_record_write() that
           preserves the legacy interface of one callback function with one
           argument.

@vertspace
@param[in]     pb_id       Phone book ID.
@param[in,out] record      Pointer to the record. If record.index == -1, write
                           to the first empty record. If record.number is NULL,
                           delete the entry record.index.
@param[in]     proc_func   Callback function that returns the result of the
                           write operation. If it is not NULL,
                           it is called when the write is complete.


@return
  PBM_ERROR -- The parameters are invalidm or the PBM encountered
               an internal error when attempting the write.\n
@vertspace
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
@vertspace
  PBM_ERROR_NOT_FOUND -- The location in the requested phone book is empty and
                         the data indicates the request is to delete the
                         record only.\n
@vertspace
  PBM_ERROR_INDEX_INVALID -- The index provided in the data is beyond the range of
                             entries for the specified file.\n
@vertspace
  PBM_ERROR_MEM_FULL -- There is no free location in the specified phone
                        book, or the number requires a SIM extension record
                        and there is no free location for the record.\n
@vertspace
  PBM_SUCCESS -- The write was performed to the specified device. The results of
                 the write are not known until the callback is called. If
                 proc_func is NULL, the results of the write are not returned.

@dependencies
  None.
*/
pbm_return_type pbm_session_write ( pbm_phonebook_type pb_id,
                            pbm_record_s_type * record,
                            PBM_WRITE_COMPAT_CB_FUNC proc_func);
/*~ FUNCTION pbm_session_write */
/*~ PARAM INOUT record POINTER */

/*===========================================================================
FUNCTION: PBM_SESSION_RECORD_ID_TO_LOCATION
===========================================================================*/
/**
  Converts a record ID to a location (extracts the location information from
  the record ID).

@param[in] record_id   Record ID to be converted.

@return
  Location corresponding to the record ID being passed.

@dependencies
  None.
*/
uint16 pbm_session_record_id_to_location(pbm_record_id_type record_id);

/*===========================================================================
FUNCTION: PBM_SESSION_RECORD_ID_TO_PHONEBOOK
===========================================================================*/
/**
  Converts a record ID to a phone book ID (extracts the phone book ID information
  from the record ID).

@param[in] record_id   Record ID to be converted.

@return
  Phone book corresponding to the record ID being passed.

@dependencies
  None.
*/
pbm_phonebook_type pbm_session_record_id_to_phonebook_type(pbm_record_id_type record_id);

/*===========================================================================
FUNCTION: PBM_SESSION_GET_FIELD_INFO_COUNT
===========================================================================*/
/**
  Returns the number of PBM fields supported by the specified PBM phone book or
  category.

@param[in]  pb_id   Phone book ID.
@param[in]  cat     Category for which information is requested.
@param[out] num     Pointer to the number of fields supported by this category.

@return
  PBM_SUCCESS -- The operation was successful.\n
  PBM_ERROR_NOT_READY -- The requested phone book is not ready.\n
  PBM_ERROR -- Other error.

@dependencies
  None.
*/
pbm_return_type pbm_session_get_field_info_count(pbm_phonebook_type pb_id, pbm_cat_e_type cat,
                                         int *num);
/*~ FUNCTION pbm_session_get_field_info_count */
/*~ PARAM OUT num POINTER */

/*===========================================================================
FUNCTION: PBM_SESSION_GET_FIELD_INFO
===========================================================================*/
/**
  Returns field information associated with each PBM field supported by the
  specified category

@param[in]  pb_id   Phone book ID.
@param[in]  cat     Category for which information is requested.
@param[out] pf      Pointer to the array of field information that is filled by
                    the PBM.
@param[in]  size    Size of the pf array in bytes.

@return
  PBM_SUCCESS -- The operation was successful.\n
  PBM_ERROR_NOT_READY -- The requested phone book is not ready.\n
  PBM_ERROR -- Other error.

@dependencies
  None.
*/
pbm_return_type pbm_session_get_field_info(pbm_phonebook_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size);
//change the value of PARAM OUT appropriately whenever PBM_MAX_NUM_FIELDS is changed
/*~ FUNCTION pbm_session_get_field_info */
/*~ PARAM OUT pf VARRAY 80
    LENGTH (size/sizeof(pbm_field_info_s_type)) */

/*===========================================================================
FUNCTION: PBM_SESSION_RECORD_VALIDATE
===========================================================================*/
/**
  Checks whether a specified record is valid. The validation includes the
  following:

  - Checks whether fields that are required are included. For ADN or FDN phone
    books, there should be a non-NULL number string. For other phone books,
    there should be a non-NULL number OR non-NULL name string.
  - Checks whether the maximum number of a particular field, the maximum size
    of a particular field, or the maximum size of the entire record is exceeded.
  - Checks whether all fields are supported for this phone book and whether the
    types of fields match the field ID.

@vertspace
@param[in] pb_id           Phone book ID.
@param[in] rec_id          Record ID.
@param[in] cat             Category of the record.
@param[in] data_buf        Pointer to the buffer with an array of fields containing
                           the data (same structure as pbm_record_read() and
                           pbm_record_write()).
@param[in] data_buf_size   Buffer size.
@param[in] num_fields      Number of fields.

@return
  PBM_ERROR_INDEX_INVALID -- The record contains an invalid index field or
                             an invalid record ID.\n
  PBM_ERROR_TEXT_TOO_LONG -- The name is too long.\n
  PBM_ERROR_NUMBER_TOO_LONG -- The number is too long.\n
  PBM_ERROR_NOT_READY -- The requested phone book has not been initialized.\n
  PBM_ERROR_NO_SIM -- The corresponding UIM is not present.\n
  PBM_ERROR_NOT_AVAILABLE -- The corresponding UIM file is not available.\n
  PBM_ERROR_RESTRICTED -- The UIM file is not available due to a restriction.\n
  PBM_ERROR_INVALID_REC -- The record is not valid for this phone book due
                           to other reasons.\n
  PBM_SUCCESS -- The record is valid.

@dependencies
  None.
*/
pbm_return_type pbm_session_record_validate(pbm_phonebook_type pb_id,
                                    pbm_record_id_type rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *data_buf,
                                    int data_buf_size,
                                    int num_fields);
/*~ FUNCTION pbm_session_record_validate */
/*~ PARAM data_buf VARRAY LENGTH data_buf_size */

/*===========================================================================
FUNCTION: PBM_SESSION_CLEAR_PHONEBOOK
===========================================================================*/
/**
  Clears all entries in a specified phone book.

@param[in] pb_id   Phone book ID.

@return
  PBM_SUCCESS -- All entries in the specified phone book were cleared.\n
  PBM_ERROR -- All entries in the specified phone book werenot  cleared.

@dependencies
  None.
*/
pbm_return_type pbm_session_clear_phonebook(pbm_phonebook_type pb_id);
/*~ FUNCTION pbm_session_clear_phonebook */

/*===========================================================================
FUNCTION: PBM_SESSION_EXTENDED_FILE_INFO
===========================================================================*/
/**
  Through a GSDI, returns the total number of records, the number of records in
  use, the text length, and the maximum phone number length for the specified
  phone book.

  The requested data is returned in the pointer information synchronously.

@vertspace
@param[in]  pb_id   Phone book ID.
@param[out] info    Pointer to the information about the specified phone book.

@return
  PBM_SUCCESS -- The file information was retrieved successfully.\n
  PBM_ERROR -- The file information was not retrieved successfully.

@dependencies
  None.
*/
extern pbm_return_type pbm_session_extended_file_info (pbm_phonebook_type pb_id,
                                               pbm_extended_fileinfo_s_type *info);
/*~ FUNCTION pbm_session_extended_file_info */
/*~ PARAM OUT info POINTER */


/*===========================================================================
FUNCTION: PBM_GET_PHONEBOOK_USIM_FIELDS_CAPIBILITIES
===========================================================================*/
/**
  It will return the USIM field capibilities like mapping type of file, number of records, 
  records_in_use  and record length.

  The requested data is returned in the pointer information synchronously.

@vertspace
@param[in]  pb_id   Phone book ID.
@param[out] info    Pointer to the information about the usim fields belonging to that phonebook.

@return
  PBM_SUCCESS -- fields information was retrieved successfully.\n
  PBM_ERROR -- fields information was not retrieved successfully.

@dependencies
  None.
*/
extern pbm_return_type pbm_get_phonebook_usim_fields_capibilities (pbm_phonebook_type pb_id,
                                               pbm_all_usim_field_capabilities_s_type *info);

/*~ FUNCTION pbm_get_phonebook_usim_fields_capibilities */
/*~ PARAM OUT info POINTER */

/*===========================================================================
FUNCTION: pbm_get_adn_pbset_capibilities
===========================================================================*/
/**
It will return the pbset wise ADN capabilities 
  The requested data is returned in the pointer information synchronously.

@vertspace
@param[in]  pb_id   Phone book ID.
@param[out] info    Pointer to the information about the usim fields belonging to that phonebook.

@return
  PBM_SUCCESS -- fields information was retrieved successfully.\n
  PBM_ERROR -- fields information was not retrieved successfully.

@dependencies
  None.
*/
extern	pbm_return_type pbm_get_adn_pbset_capibilities(pbm_phonebook_type pb_id,
												   pbm_adn_capabilites_pbset_s_type *info);


/*~ FUNCTION pbm_get_adn_pbset_capibilities */
/*~ PARAM OUT info POINTER */

/*===========================================================================
FUNCTION: PBM_SESSION_EXTENDED_FILE_INFO_ASYNC
===========================================================================*/
/**
  Through a GSDI, returns the total number of records, the number of records
  in use, the text length, and the maximum phone number length for the specified
  phone book.

@note1hang This function is to be used only by modem clients in dual processors,
  since the callback is in the same function.

@vertspace
@param[in] pb_id       Phone book ID.
@param[in] proc_func   Callback function that returns a pointer to the file data.

@return
  PBM_SUCCESS -- The file information was retrieved successfully.\n
  PBM_ERROR -- The file information was not retrieved successfully.

@dependencies
  None.
*/
pbm_return_type pbm_session_extended_file_info_async(pbm_phonebook_type pb_id,
                                        PBM_SESSION_EXTENDED_FILE_INFO_CB_FUNC proc_func);
/*~ FUNCTION pbm_session_extended_file_info_async */

/*===========================================================================
FUNCTION: PBM_SESSION_LOCATION_TO_RECORD_ID
===========================================================================*/
/**
  Converts a phone book ID and a location to a record ID.

@param[in] pb_id      Phone book ID.
@param[in] location   Location of the record.

@return
  Record ID corresponding to the location being passed.

@dependencies
  None.
*/
pbm_record_id_type pbm_session_location_to_record_id(pbm_phonebook_type pb_id, int location);

/*===========================================================================
FUNCTION: pbm_uim_device_type_to_phonebook_type
===========================================================================*/
/**
  Converts a PBM UIM device type to a phone book type.

@param[in] uim_type      UIM type.
@param[in] device_type   UIM device to be converted.

@return
  Phone book corresponding to the device_type and prov_type being passed.

@dependencies
  None.
*/
pbm_phonebook_type pbm_uim_device_type_to_phonebook_type(pbm_uim_app_enum_type uim_type,
                                                         pbm_device_type device_type);

/*===========================================================================
FUNCTION: pbm_device_id_to_pb_id
===========================================================================*/
/**
  Converts a PBM device ID to a phone book ID.

@param[in] session_type   Session type.
@param[in] device_id      Device to be converted.

@return
  Phone book corresponding to the device_type and session_type being passed.

@dependencies
  None.
*/
pbm_phonebook_type pbm_device_id_to_pb_id(pbm_session_enum_type session_type,
                                          pbm_device_type device_id);

/*===========================================================================
FUNCTION: pbm_session_type_to_pb_category
===========================================================================*/
/**
  Converts a session type to a phone book ID.

@param[in] session_type   Session type to be converted.

@return
  Category corresponding to the session_type being passed.

@dependencies
  None.
*/
pbm_pb_category_enum_type pbm_session_type_to_pb_category(pbm_session_enum_type session_type);


/*===========================================================================
FUNCTION: pbm_session_type_to_provision_type
===========================================================================*/
/**
  Converts a session type to a provision type.

@param[in] session_type   Session type to be converted.

@return
  Provisioning type corresponding to the session_type being passed.

@dependencies
  None.
*/
pbm_provision_enum_type pbm_session_type_to_provision_type(pbm_session_enum_type session_type);


/*===========================================================================
  FUNCTION pbm_get_slot_info()

  DESCRIPTION
    Get the slot info for the given PBM session
    
  PARAMETERS
    pbm_session_enum_type pbm_session
    
  RETURN VALUE

    mmgsdi_slot_id_enum_type
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16  pbm_get_slot_info (pbm_session_enum_type pbm_session);

/*===========================================================================
  FUNCTION pbm_is_card_present_in_slot()

  DESCRIPTION
    Checks if card is present in the given slot or not
    
  PARAMETERS
    Slot
    
  RETURN VALUE
    boolean 
    - TRUE if card is present in that slot 
    - FALSE if card is not present in that slot 
                     
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean pbm_is_card_present_in_slot ( uint8 slot );


/** @} */ /* end_addtogroup pbmlib */

#endif /* PBMLIB_H */
