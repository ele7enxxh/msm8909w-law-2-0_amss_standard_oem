#ifndef TDS_RRC_EXT_IF_H
#define TDS_RRC_EXT_IF_H


/*===========================================================================
                      RRC EXT UMIDs and Interface Structures

DESCRIPTION

  This module contains the UMIDs and Interface Structures for
  TDSCDMA RRC procedures

  !!!SHOULD NOT INCLUDE ANY RRC HEADER HERE!!!


Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tds_rrc_ext_if.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#include "tds_rrc_umid.h"


/*============================================================
        FR 3803: One NV settings configuration data types
=============================================================*/
typedef enum
{
  TDS_CONFIG_FIELD,
  TDS_CONFIG_LAB,
  TDS_CONFIG_USER,
} tds_rrc_config_e_type;

/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_GET_CONFIG_REQ
This primitive is sent to TRRC to request for current configuration.
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr; 

  /* No payload */
} tds_rrc_get_config_req_type;


/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_GET_CONFIG_RSP
This primitive is sent from TRRC to return current configuration.
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr; 

  /* Current configuration in SW */
  tds_rrc_config_e_type  active_config;
  
  /* Configuration set by user */
  tds_rrc_config_e_type  desired_config;

  /* If active_config and desired_config are different
   * values, user is required to power cycle the phone.
   */

} tds_rrc_get_config_rsp_type;


/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_SET_CONFIG_REQ
This primitive is sent to TRRC to set current configuration.
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr; 

  /* New configuration */
  tds_rrc_config_e_type  config;
} tds_rrc_set_config_req_type;



/*----------------------------------------------------------------------------
                         TDSCDMA_RRC_SET_CONFIG_RSP
This primitive is sent from TRRC to return operation status.
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr; 

  /* Operation status: TRUE success, FALSE failure */
  boolean  status;
} tds_rrc_set_config_rsp_type;


#endif /*TDS_RRC_EXT_IF_H*/


