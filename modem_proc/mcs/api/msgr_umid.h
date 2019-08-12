/*!
  @file msgr_umid.h

  @brief
  MSGR's common system wide UMID related definitions

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/msgr_umid.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/11   ckk     Define MSGR_RESERVED_NO_UMID
08/18/11   ckk     Add MSGR_TECH_QMI
07/27/11   ckk     Add MSGR_TECH_IMS
03/21/11   ejv     Add MSGR_TECH_CNE.
03/14/11   ejv     Add MSGR_MCS_TCXOMGR.
02/15/11   jg      Add MSGR_IS_INTERNAL.
11/09/10   zm      Moved file into mcs/api folder - SU API effort.
10/29/10   ejv     Add APP1/2 back in.
09/02/10   zm      Add MSGR_TECH_WMS
08/06/10   ckk     Replace MSGR_MCS_APP* tech modules
07/29/10   ckk     Addd MSGR_TECH_TDSCDMA
05/06/10   ax      Add MSGR_TECH_UTILS
04/19/10   ejv     Add MSGR_TECH_UIM.
04/01/10   ejv     Add MSGR_MCS_FW_APP.
02/07/10   ejv     Remove MSGR_MODULE_x.
10/08/09   ejv     Add MSGR_TECH_NAS.
10/07/09   ejv     Add MSGR_TECH_C2K.
10/02/09   ejv     Add MSGR_TECH_ONEX.
08/07/09   ejv     Add MSGR_TECH_DS.
02/11/09   ejv     Update the list of modem technologies.
11/24/08   ejv     Add tech/module defines from mcs_app_msg.h.
07/09/08   ejv     Update available types.
05/19/08   pl      adding OTA and TMR type
05/08/08   djm     add macros for pulling tech/module out of tech_mod type
05/06/08   djm     add umid TECH_MOD_ID_TYPE variant, keeping tech_mod_type_id
01/31/08   bmg     Added common RF services modules.
10/16/07   enj     Split umb_app into umb_app and appmgr
           enj     Initial Revision

===========================================================================*/

#ifndef __MSGR_UMID_H__
#define __MSGR_UMID_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/***************************************************
 * Technology Definitions (bits 31:24 of the UMID) *
 * This list of techs must be matched in umid.py   *
 * (also update umid.qbd while qbuild is used)     *
 ***************************************************/

/*! @brief Modem Common Services technology area */
#define MSGR_TECH_MCS           0x00

/*! @brief UMB technology definition. */
#define MSGR_TECH_UMB           0x01

/*! @brief Multimode technology definition. */
#define MSGR_TECH_MM            0x02

/*! @brief WMX technology definition. */
#define MSGR_TECH_WMX           0x03

/*! @brief LTE technology definition. */
#define MSGR_TECH_LTE           0x04

/*! @brief Factory Test Mode technology area. */
#define MSGR_TECH_FTM           0x05

/*! @brief RF and Analog technology area. */
#define MSGR_TECH_RFA           0x06

/*! @brief Legacy tech area to be phased out? */
#define MSGR_TECH_CDMA          0x07

/*! @brief HDR technology definition. */
#define MSGR_TECH_HDR           0x08

/*! @brief GERAN technology definition. */
#define MSGR_TECH_GERAN         0x09

/*! @brief GPS technology definition. */
#define MSGR_TECH_GPS           0x0a

/*! @brief WCDMA technology definition. */
#define MSGR_TECH_WCDMA         0x0b

/*! @brief Data Services technology definition. */
#define MSGR_TECH_DS            0x0c

/*! @brief 1X technology definition. */
#define MSGR_TECH_ONEX          0x0d

/*! @brief CDMA2000 technology definition - common for 1X & HDR. */
#define MSGR_TECH_C2K           0x0e

/*! @brief NAS technology definition. */
#define MSGR_TECH_NAS           0x0f

/*! @brief UIM technology definition. */
#define MSGR_TECH_UIM           0x10

/*! @brief Utilities technology definition. */
#define MSGR_TECH_UTILS         0x11

/*! @brief TD-SCDMA technology definition. */
#define MSGR_TECH_TDSCDMA       0x12

/*! @brief WMS technology definition. */
#define MSGR_TECH_WMS           0x13

/*! @brief CNE technology definition. */
#define MSGR_TECH_CNE           0x14

/*! @brief IMS technology definition. */
#define MSGR_TECH_IMS           0x15

/*! @brief QMI technology definition. */
#define MSGR_TECH_QMI           0x16

/*! @brief ECALL technology definition. */
#define MSGR_TECH_ECALL         0x17

/*! @brief Policy Manager technology definition. */
#define MSGR_TECH_POLICYMAN     0x18

/*! @brief CORE technology definition. */
#define MSGR_TECH_CORE     0x19

/*! @brief RFLM technology definition. */
#define MSGR_TECH_RFLM     0x1A

/* Remember to also update as needed,
     -- modem\mcs\build\mbs\scons\site_tools\umid.py for modem-scons
     -- modem\build\msgr_builder.py for qc-scons
     -- modem\mcs\build\qbd\system\*\umid.qbd for qbuild */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/***************************************************
 * Tech-Module Definitions (bits 31:16 of the UMID)*
 * 31:24 are tech, while 23:16 are module          *
 ***************************************************/

/* modules no longer need to be #define'd, so are listed explicitly
   in the MSGR_TECH_MODULE macro call (to help avoid collisions) */

/*! @brief MSGR tech_module */
#define MSGR_MCS_MSGR       MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x01 )
/*! @brief AppMgr tech_module */
#define MSGR_MCS_APPMGR     MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x02 )
/*! @brief Advanced Timer Services tech_module */
#define MSGR_MCS_ATS        MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x03 )
/*! @brief External Test module */
#define MSGR_MCS_EXT_TEST   MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x04 )
/*! @brief Test Framework tech_module */
#define MSGR_MCS_QTF        MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x05 )
/*! @brief Lua tech_module */
#define MSGR_MCS_LUA        MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x06 )
/*! @brief Modem firmware services tech_module */
#define MSGR_MCS_FWS        MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x07 )
/*! @brief MCS firmware app tech_module */
#define MSGR_MCS_FW_APP     MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x08 )
/*! @brief MCS TCXO Mgr tech_module */
#define MSGR_MCS_TCXOMGR    MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x09 )
/*! @brief MCS MMAL tech_module */
#define MSGR_MCS_MMAL       MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x0a )
/*! @brief MCS CXM tech_module */
#define MSGR_MCS_CXM        MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x0b )
/*! @brief MCS CFCM tech_module */
#define MSGR_MCS_CFCM       MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x0c )


/*! @brief Tech-modules used for multi-app builds */
#define MSGR_MCS_APP_DSPSW  MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x10 )
#define MSGR_MCS_APP_DSPFW  MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x11 )
#define MSGR_MCS_THREAD1    MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x12 )
#define MSGR_MCS_THREAD2    MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x13 )
#define MSGR_MCS_APP1       MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x14 )
#define MSGR_MCS_APP2       MSGR_TECH_MODULE( MSGR_TECH_MCS, 0x15 )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/***************************************************
 * Type Definitions (bits 15:8 of the UMID)        *
 ***************************************************/

/* External Types */

/*! @brief Supervisory type definition */
#define MSGR_TYPE_SPR      0x00
/*! @brief Commands type definition */
#define MSGR_TYPE_CMD      0x01
/*! @brief Requests type definition */
#define MSGR_TYPE_REQ      0x02
/*! @brief Responses (out) type definition */
#define MSGR_TYPE_RSP      0x03
/*! @brief Indications (out) type definition */
#define MSGR_TYPE_IND      0x04
/*! @brief FL Messages type definition */
#define MSGR_TYPE_FLM      0x05
/*! @brief Over The Air (OTA) Message type definition */
#define MSGR_TYPE_OTA      0x06
/*! @brief Downlink Message type definition */
#define MSGR_TYPE_DLM      0x07
/*! @brief Confirm type definition */
#define MSGR_TYPE_CNF      0x08
/*! @brief Timer events type definition */
#define MSGR_TYPE_TMR      0x09

/*! @brief Minimum value for external message types */
#define MSGR_MIN_EXT_TYPE  MSGR_TYPE_SPR
/*! @brief Maximum value for external message types */
#define MSGR_MAX_EXT_TYPE  MSGR_TYPE_TMR
/*! @brief Number of external message command types */
#define MSGR_NUM_EXT_TYPES (MSGR_MAX_EXT_TYPE - MSGR_MIN_EXT_TYPE + 1)

/* Internal Types */
/* No Supervisory internal messages */
/*! @brief Internal commands type definition */
#define MSGR_TYPE_CMDI     0x11
/*! @brief Internal requests type definition */
#define MSGR_TYPE_REQI     0x12
/*! @brief Internal responses (out) type definition */
#define MSGR_TYPE_RSPI     0x13
/*! @brief Internal indications (out) type definition */
#define MSGR_TYPE_INDI     0x14
/*! @brief Internal Confirm type definition */
#define MSGR_TYPE_CNFI     0x15
/*! @brief Internal timers type definition */
#define MSGR_TYPE_TMRI     0x16
/*! @brief Internal interrupt type definition */
#define MSGR_TYPE_INTI     0x17

/*! @brief Minimum value for internal message types */
#define MSGR_MIN_INT_TYPE  MSGR_TYPE_CMDI
/*! @brief Maximum value for internal message types */
#define MSGR_MAX_INT_TYPE  MSGR_TYPE_INTI
/*! @brief Number of internal message command types */
#define MSGR_NUM_INT_TYPES (MSGR_MAX_INT_TYPE - MSGR_MIN_INT_TYPE + 1)

/*! @brief Number of total message types */
#define MSGR_NUM_TYPES  (MSGR_NUM_INT_TYPES + MSGR_NUM_EXT_TYPES)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define MSGR_RESERVED_NO_UMID     0x00000000
#define MSGR_RESERVED_UMID_START  0xFF000000
#define MSGR_RESERVED_UMID_END    0xFFFFFFFF

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Type for a 32-bit Unique Message ID (UMID).
*/
typedef   uint32   msgr_umid_type;

/*! @brief Type for a TECH/MODULE pair
 */
typedef   uint16   msgr_tech_module_type;

/*! @deprecated This type is deprecated in lieu of msgr_tech_module_type
 */
typedef   uint8    msgr_mdl_type;

/*===========================================================================

                    EXTERNAL MACRO DEFINITIONS

===========================================================================*/

/*! @brief Macro to define a UMID
 */
#define MSGR_DEFINE_UMID( tech, mod, type, id, idnum, payload, ... ) \
  tech##_##mod##_##id##_##type =                                \
    MSGR_UMID( MSGR_##tech##_##mod, MSGR_TYPE_##type, idnum ),  \
  tech##_##mod##_##type##_##id =                                \
    MSGR_UMID( MSGR_##tech##_##mod, MSGR_TYPE_##type, idnum )

/*! @brief Macro to create a new UMID
 *  @note If this macro changes, the corresponding regex in umid_gen.pl must
 *        be changed
 */
#define MSGR_UMID( tech_mod, type, id )            \
  ((msgr_umid_type)                                \
   (((tech_mod) & 0xFFFF) << 16) |                 \
   (((type) & 0x1F) << 8) |                        \
   ((id) & 0xFF))

/*! @brief Macro to create a new UMID base
 */
#define MSGR_UMID_BASE( tech_mod, type )           \
  ((msgr_umid_type)                                \
   (((tech_mod) & 0xFFFF) << 16) |                 \
   (((type) & 0x1F) << 8))

/*! @brief Macro to create a TECH/MODULE pair
 */
#define MSGR_TECH_MODULE( tech, mod )              \
  ((uint16) (((tech) & 0xFF) << 8) | ((mod) & 0xFF))

/*! @brief Macro to extract the technology out of a tech_mod
 */
#define MSGR_TECH_MOD_TECH_VAL( tech_mod )      \
  ((uint8) (((tech_mod) >> 8) & 0xFF))

/*! @brief Macro to extract the module out of a tech_mod
 */
#define MSGR_TECH_MOD_MODULE_VAL( tech_mod )   \
  ((uint8) ((tech_mod) & 0xFF))

/*! @brief Macro to extract the technology out of a UMID.
*/
#define MSGR_TECH_VAL(umid)  ((uint8) ( ( (umid) >> 24) & 0xFF ) )

/*! @brief Macro to extract the module out of a UMID.
*/
#define MSGR_MODULE_VAL(umid)  ((uint8) ( ( (umid) >> 16) & 0xFF ) )

/*! @brief Macro to extract the tech/module pair out of a UMID.
 */
#define MSGR_TECH_MODULE_VAL(umid)  ((uint16) ( ( (umid) >> 16 ) & 0xFFFF ) )

/*! @brief Macro to extract the message-type out of a UMID.
*/
#define MSGR_TYPE_VAL(umid)  ((uint8) ( ( ( umid ) >> 8 ) & 0x1F ) )

/*! @brief Macro to extract the message-ID out of a UMID.
*/
#define MSGR_ID_VAL(umid)      ((uint8) ( (umid) & 0xFF ) )

/*! @brief Macro to extract the MSGR Variant out of a UMID.
*/
#define MSGR_VARIANT_VAL(umid)    ((uint8) ( ( ( umid ) >> 13 ) & MSGR_MAX_VARIANT ) )

/*! @brief Macro to determine if a UMID is internal type or external type.
*/
#define MSGR_IS_INTERNAL(umid)  \
  ((boolean) ( MSGR_TYPE_VAL(umid) >= MSGR_MIN_INT_TYPE ) ? TRUE : FALSE )

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* __MSGR_UMID_H__ */
