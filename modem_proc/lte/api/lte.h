/*!
  @file
  lte.h

  @brief
  This file contains definitions for the Common type and constants required
  by different modules within LTE Access Stratum(AS) as well as LTE Non-Access
  Stratum(NAS)

*/

/*=============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/05/14   mg      CR 591605: eMBMS Klockwork fixes
01/10/13   sb      CR439568: Change macro LTE_MAX_EPS from 32 to 16
04/19/12   kp      Updated lte_3gpp_release_ver_e with 3gpp Rel-10 version.
05/16/11   ae      Moved a2_common.h and a2 algo to lte_security.h.
03/23/10   sureshs Moved UE category to lte.h from lte_as.h
08/25/09   mm      Added a structure for RB-EPS bearer mapping info
06/17/09   ar      fixed featurization of integrity enum for LTE
04/23/09   sm      set cipher algo enum values to match with A2 enumeration
04/17/09   dd      Added the security related stuff
07/31/08   sm      initial version
=============================================================================*/

#ifndef LTE_H
#define LTE_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <comdef.h>
#include <IxErrno.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief Maximum Number of EPS Ids
*/
#define LTE_MAX_EPS 16

/*! @brief Size of ciphering/integrity key in bytes
*/
#define LTE_CIPHER_INTEGRITY_KEY_LEN 16

/*! @brief Size of the generator key in bytes
*/
#define LTE_GENERATOR_KEY_LEN 32

/*! @brief Size of the MAC-I (Integrity message digest) in bytes
*/
#define LTE_MACI_LEN 4

/*!
** REX signals used by LTE.
*/
#define LTE_NV_READ_SIG 0x0001

#ifdef SENSORS_COMPONENT
#define LTE_UE_CATEGORY 3
#else
#define LTE_UE_CATEGORY 4
#endif

/*! @brief Maximum active eMBMS sessions
*/
#define LTE_EMBMS_MAX_ACTIVE_SESSIONS 32

/*! @brief Maximum available eMBMS sessions for now
  (Thereotical max sessions are 3472 -> (8 MBSFN areas * 15 PMCHs * 29 LC IDs) - 8 MCCHs)
*/
#define LTE_EMBMS_MAX_AVAIL_SESSIONS 128

/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*! @brief Enumeration for LTE 3GPP Spec version
*/
typedef enum
{
  /*Rel8 versions*/
  LTE_3GPP_REL8 = 0x10,
  LTE_3GPP_REL8_DEC08 = 0x11,
  LTE_3GPP_REL8_MAR09 = 0x12,
  LTE_3GPP_REL8_JUN09 = 0x13,
  LTE_3GPP_REL8_SEP09 = 0x14,
  LTE_3GPP_REL8_DEC09 = 0x15,
  LTE_3GPP_REL8_MAR10 = 0x16,
  LTE_3GPP_REL8_JUN10 = 0x17,
  LTE_3GPP_REL8_SEP10 = 0x18,
  LTE_3GPP_REL8_DEC10 = 0x19,
  LTE_3GPP_REL8_MAX,
  /*Rel 9 versions*/
  LTE_3GPP_REL9 = 0x20,
  LTE_3GPP_REL9_MAR10 = 0x21,
  LTE_3GPP_REL9_JUN10 = 0x22,
  LTE_3GPP_REL9_SEP10 = 0x23,
  LTE_3GPP_REL9_MAX,
  /*Rel 10 versions*/
  LTE_3GPP_REL10 = 0x40, /*June 2011 release*/
  LTE_3GPP_REL10_SEP11 = 0x41,
  LTE_3GPP_REL10_DEC11 = 0x42,
  LTE_3GPP_REL10_MAR12 = 0x43,
  LTE_3GPP_REL10_JUN12 = 0x44,
  LTE_3GPP_REL10_SEP12 = 0x45,
  LTE_3GPP_REL10_DEC12 = 0x46, /*! v10.8.0 */
  LTE_3GPP_REL10_MAR13 = 0x47, /*! v10.9.0 */
  LTE_3GPP_REL10_JUN13 = 0x48, /*! v10.10.0 */
  LTE_3GPP_REL10_MAX,
  /*Rel 11 versions*/
  LTE_3GPP_REL11 = 0x80, /*! Dec 2012 release*/
  LTE_3GPP_REL11_JUN13 = 0x81,
  LTE_3GPP_REL11_MAX,
} lte_3gpp_release_ver_e;

/*=============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

=============================================================================*/



/*=============================================================================

                   DATA TYPES

=============================================================================*/

/*! @brief error codes used within LTE Access Stratum
*/
typedef errno_enum_type lte_errno_e;

/*! @brief This defines the EPS identity as described in 36.331(RRC)
*/
typedef uint8 lte_eps_id_t;

/*! @brief REQUIRED brief description of this structure typedef
  This type is used for the active EPS bearer indication sent from
  RRC to NAS and for loopback testing
*/
typedef struct
{
  lte_eps_id_t eps_id;   /*!< EPS bearer ID */
  uint8 rb_id;   /*!< RB ID */

} lte_rb_eps_bearer_map_info_s;


#endif /* LTE_H */

