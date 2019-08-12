#ifndef WMSPRIV_H
#define WMSPRIV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wms_priv.h -- Internal inter-module definitions

  This file contains private definitions for WMS inter-module use.


  -------------

    Copyright (c) 2001-2008,2010-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/inc_deprecated/wmspriv.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "mmcp_variation.h"
#include "wms.h"

/*===========================================================================
========================  MACROS ============================================
===========================================================================*/
#define WMS_GW_TEMPLATE_MIN         28

enum { WMS_SMS_UDL_MAX_7_BIT  = 160 }; /* as in the spec */
enum { WMS_SMS_UDL_MAX_8_BIT  = 140 }; /* as in the spec */

enum { WMS_GW_CB_PAGE_HEADER_LEN = 6 };
enum { WMS_GW_CB_MAX_PAGE_USER_DATA_LEN = 93 };
enum { WMS_GW_CB_PAGE_SIZE    = 88 };

enum { WMS_TL_MAX_LEN        = 246 };

#define WMS_RUIM_SMSP_EMPTY_CHAR    0xFF
#define WMS_RUIM_SMSP_EMPTY_CHAR2   0x00

enum{ WMS_DUMMY_SEQ_NUM         = 0x80 };

enum { WMS_SIM_SMS_MAX  = 176 }; /* as in the spec */

/* Transport Layer parameter mask values:
*/
enum{ WMS_MASK_TL_NULL                = 0x00000000 };
enum{ WMS_MASK_TL_TELESERVICE_ID      = 0x00000001 };
enum{ WMS_MASK_TL_BC_SRV_CATEGORY     = 0x00000002 };
enum{ WMS_MASK_TL_ADDRESS             = 0x00000004 };
enum{ WMS_MASK_TL_SUBADDRESS          = 0x00000008 };
enum{ WMS_MASK_TL_BEARER_REPLY_OPTION = 0x00000040 };
enum{ WMS_MASK_TL_CAUSE_CODES         = 0x00000080 };
enum{ WMS_MASK_TL_BEARER_DATA         = 0x00000100 };

/* EFmwis message status mask
*/
enum{ WMS_MASK_EF_MWIS_VOICEMAIL     = 0x01 };
enum{ WMS_MASK_EF_MWIS_FAX           = 0x02 };
enum{ WMS_MASK_EF_MWIS_EMAIL         = 0x04 };
enum{ WMS_MASK_EF_MWIS_OTHERS        = 0x08 };
enum{ WMS_MASK_EF_MWIS_VIDEOMAIL     = 0x10 };

/* EFmwis message position in a record
*/
enum{ WMS_EF_MWIS_VOICEMAIL_POS  = 1};
enum{ WMS_EF_MWIS_FAX_POS        = 2};
enum{ WMS_EF_MWIS_EMAIL_POS      = 3};
enum{ WMS_EF_MWIS_OTHERS_POS     = 4};
enum{ WMS_EF_MWIS_VIDEOMAIL_POS  = 5};

/*Extended message Videomail waiting type
*/
enum{ WMS_SP_EXTD_VIDEOMAIL_MSG_WAITING_TYPE = 0x07};

/* Transport Layer parameter Ids:
*/
typedef enum
{
  WMS_TL_DUMMY          = -1,  /* dummy */
  WMS_TL_TELESERVICE_ID = 0,  /* Teleservice Identifier     */
  WMS_TL_BC_SRV_CATEGORY,     /* Broadcast Service Category */
  WMS_TL_ORIG_ADDRESS,        /* Originating Address        */
  WMS_TL_ORIG_SUBADDRESS,     /* Originating Subaddress     */
  WMS_TL_DEST_ADDRESS,        /* Destination Address        */
  WMS_TL_DEST_SUBADDRESS,     /* Destination Subaddress     */
  WMS_TL_BEARER_REPLY_OPTION, /* Bearer Reply Option        */
  WMS_TL_CAUSE_CODES,         /* Cause Codes                */
  WMS_TL_BEARER_DATA          /* Bearer Data                */

} wms_tl_parm_id_e_type;

/* Bearer Data subparameter Ids:
*/
typedef enum
{
  WMS_BD_DUMMY  = -1,   /* dummy */
  WMS_BD_MSG_ID = 0,    /* Message Identifier                */
  WMS_BD_USER_DATA,     /* User Data                         */
  WMS_BD_USER_RESP,     /* User Response Code                */
  WMS_BD_MC_TIME,       /* Message Center Time Stamp         */
  WMS_BD_VALID_ABS,     /* Validity Period - Absolute        */
  WMS_BD_VALID_REL,     /* Validity Period - Relative        */
  WMS_BD_DEFER_ABS,     /* Deferred Delivery Time - Absolute */
  WMS_BD_DEFER_REL,     /* Deferred Delivery Time - Relative */
  WMS_BD_PRIORITY,      /* Priority Indicator                */
  WMS_BD_PRIVACY,       /* Privacy Indicator                 */
  WMS_BD_REPLY_OPTION,  /* Reply Option                      */
  WMS_BD_NUM_OF_MSGS,   /* Number of Messages                */
  WMS_BD_ALERT,         /* Alert on Message Delivery         */
  WMS_BD_LANGUAGE,      /* Language Indicator                */
  WMS_BD_CALLBACK,      /* Call Back Number                  */
  WMS_BD_DISPLAY_MODE,  /* Display Mode */
  WMS_BD_MULTIPLE_USER_DATA, /* Multiple Encoding User Data  */
  WMS_BD_DEPOSIT_INDEX, /* Message Deposit Index             */
  WMS_BD_SCPT_DATA,     /* Service Category Program Data       */
  WMS_BD_SCPT_RESULT,   /* Service Category Program Result     */
  WMS_BD_DELIVERY_STATUS, /* Message Status */
  WMS_BD_IP_ADDRESS     = 85,   /* IP Address               */
  WMS_BD_RSN_NO_NOTIFY  = 86,   /* Reason for Not Notified  */
  WMS_BD_ESN            = 87   /* ESN/UIM-ID */

} wms_bd_sub_parm_id_e_type;


/*===========================================================================
========================  STRUCTURES ========================================
===========================================================================*/


/* Over-the-air (raw) message structure
*/
typedef PACKED struct wms_OTA_message_struct
{
  wms_format_e_type           format;
  uint16                      data_len;
  uint8                       data[ WMS_MAX_LEN ];
} wms_OTA_message_type;

/* Transport Layer message types
*/
typedef enum
{
  WMS_TL_TYPE_MIN               = 0,

  WMS_TL_TYPE_POINT_TO_POINT    = 0,
  WMS_TL_TYPE_BROADCAST         = 1,
  WMS_TL_TYPE_ACK               = 2,

  WMS_TL_TYPE_MAX               = 2

} wms_tl_message_type_e_type;


/* Cause code
*/
typedef struct
{
  uint8                     reply_seq_num;
  wms_error_class_e_type    error_class;

  /* If error_class is NoError, status should be ignored.
     Only the first half of the enums are used in this structure.
  */
  wms_cdma_tl_status_e_type tl_status;

} wms_cause_code_type;


/* TL fields without bearer data
*/
typedef struct
{
  boolean                      is_mo;

  wms_tl_message_type_e_type   tl_message_type;

  /* the mask indicates which fields are present in this message */
  uint16                       mask;

  wms_teleservice_e_type       teleservice;
  wms_address_s_type           address;
  wms_subaddress_s_type        subaddress;
  uint8                        bearer_reply_seq_num; /* 0..63 */
  wms_service_e_type           service;
  wms_cause_code_type          cause_code;

  /* decoded bearer data */
  wms_client_bd_s_type         cl_bd;

} wms_tl_message_type;

#ifdef FEATURE_GWSMS

typedef PACKED struct
{
  uint8       status;
  uint8       len;
  uint8       data[WMS_SIM_SMS_MAX];
} wms_msg_sim_sms_s_type;

typedef struct
{
  uint8                     status;
  wms_address_s_type        sc_address;
  wms_raw_ts_data_s_type    raw_ts_data;
} wms_msg_sim_sms_unpacked_s_type;

typedef struct
{
  uint8                        rec_id; /*EFSMS Index*/
  uint8                        len; /*TPDU Length*/
  uint8                        data[WMS_SIM_SMS_MAX];
} wms_msg_sim_smsr_s_type;

#endif /* FEATURE_GWSMS */

#endif /* WMSTSPRIV_H */

