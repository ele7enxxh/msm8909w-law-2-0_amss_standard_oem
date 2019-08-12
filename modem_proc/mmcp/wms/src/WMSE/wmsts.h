#ifndef WMSTS_H
#define WMSTS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsts.h -- Translation Services Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the internal translation functions.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsts.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"
#include "wms.h"
#include "wmspriv.h"

#ifdef __cplusplus
#define QC_EXTERN EXTERN
extern "C" {
#else
#define QC_EXTERN
#endif

#ifdef FEATURE_GWSMS

/*=========================================================================
FUNCTION
  wms_ts_pack_sim_sms

DESCRIPTION
  Pack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN boolean wms_ts_pack_sim_sms
(
  const wms_msg_sim_sms_unpacked_s_type   * sim_sms_unpacked,  // INPUT
  wms_msg_sim_sms_s_type                  * sim_sms            // OUTPUT
);

/*=========================================================================
FUNCTION
  wms_ts_unpack_sim_sms

DESCRIPTION
  Unpack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN boolean wms_ts_unpack_sim_sms
(
  const wms_msg_sim_sms_s_type            * sim_sms,           // INPUT
  wms_msg_sim_sms_unpacked_s_type         * sim_sms_unpacked   // OUTPUT
);

/*=========================================================================
FUNCTION
  wms_ts_pack_sim_smsr

DESCRIPTION
  Pack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN boolean wms_ts_pack_sim_smsr
(
  const wms_raw_ts_data_s_type   *raw_ts_data,  // INPUT
  wms_msg_sim_smsr_s_type        * sim_smsr    // OUTPUT
);
/*=========================================================================
FUNCTION
  wms_ts_unpack_sim_smsr

DESCRIPTION
  Unpack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN boolean wms_ts_unpack_sim_smsr
(
  const wms_msg_sim_smsr_s_type           * sim_smsr,           // INPUT
  wms_raw_ts_data_s_type                  * raw_ts_data        // OUTPUT
);

/*=========================================================================
FUNCTION
  wms_ts_int_to_bcd

DESCRIPTION
  Convert an Integer to a BCD.
  Valid Integer values are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN uint8 wms_ts_int_to_bcd
(
  const uint8 i
);

/*=========================================================================
FUNCTION
  wms_ts_encode_deliver_report_ack

DESCRIPTION
  Encode Deliver-Report-Ack structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_encode_deliver_report_ack
(
  const wms_gw_deliver_report_ack_s_type    *deliver_report_ack,
  wms_raw_ts_data_s_type                    *raw_ts_data_ptr
);

/*=========================================================================
FUNCTION
  wms_ts_encode_deliver_report_error

DESCRIPTION
  Encode Deliver-Report-Error structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_encode_deliver_report_error
(
  const wms_gw_deliver_report_error_s_type    *deliver_report_error,
  wms_raw_ts_data_s_type                      *raw_ts_data_ptr
);


/*=========================================================================
FUNCTION
  wms_ts_dncode_smsp

DESCRIPTION
  Decode SMSP (SMS Parameter - Template) from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN boolean wms_ts_decode_smsp
(
  const uint8               * data,
  uint8                     rec_len,
  wms_gw_template_s_type  * tptr
);

/*=========================================================================
FUNCTION
  wms_ts_encode_smsp

DESCRIPTION
  Encode SMSP (SMS Parameter - Template) into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN boolean wms_ts_encode_smsp
(
  const wms_gw_template_s_type  * tptr,
  uint8                           rec_len,
  uint8                           * data
);

/*=========================================================================
FUNCTION
  wms_ts_decode_deliver

DESCRIPTION
  Decode Deliver TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_decode_deliver
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_deliver_s_type                   * deliver,
  uint32                                  * nRawTsLen
);

/*=========================================================================
FUNCTION
  wms_ts_decode_submit_report_ack

DESCRIPTION
  Decode Submit-Report-Ack TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_decode_submit_report_ack
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_submit_report_ack_s_type         * submit_report_ack,
  uint32                                  * nRawTsLen
);

/*=========================================================================
FUNCTION
  wms_ts_decode_submit_report_error

DESCRIPTION
  Decode Submit-Report-Error TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_decode_submit_report_error
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_submit_report_error_s_type       * submit_report_error,
  uint32                                  * nRawTsLen
);

/*=========================================================================
FUNCTION
  wms_ts_decode_deliver_report_ack

DESCRIPTION
  Decode Deliver-Report-Ack TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_decode_deliver_report_ack
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_deliver_report_ack_s_type        * deliver_report_ack,
  uint32                                  * nRawTsLen
);


/*=========================================================================
FUNCTION
  wms_ts_decode_deliver_report_error

DESCRIPTION
  Decode Deliver-Report-Error TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_decode_deliver_report_error
(
  const wms_raw_ts_data_s_type              * raw_ts_data_ptr,
  wms_gw_deliver_report_error_s_type        * deliver_report_error,
  uint32                                    * nRawTsLen
);

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS
/*=========================================================================
FUNCTION
  wms_ts_decode_user_status_report

DESCRIPTION
  Decode Status Report from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Decoding status

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN wms_status_e_type wms_ts_decode_status_report
(
  const wms_raw_ts_data_s_type              * raw_ts_data_ptr,
  wms_gw_status_report_s_type               * status_report,
  uint32                                    * nRawTsLen
);
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST

/*=========================================================================
FUNCTION
  wms_ts_decode_gw_cb_page_header

DESCRIPTION
  Decode GSM Cell Broadcast Page Header from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN uint8 wms_ts_decode_gw_cb_page_header
(
  const uint8                   * data,
  wms_gw_cb_page_header_s_type  * page_header_ptr
);

/*=========================================================================
FUNCTION
  wms_ts_decode_gw_cb_dcs

DESCRIPTION
  Decode GSM Cell Broadcast Data Coding Scheme.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN uint8 wms_ts_decode_gw_cb_dcs
(
  const uint8                  data,            /* INPUT */
  const uint8                  * user_data_ptr, /* INPUT */
  wms_gw_cb_dcs_s_type         * dcs_ptr        /* OUTPUT */
);

#endif /* FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION
  wms_ts_compute_user_data_header_length

DESCRIPTION
  Computes the User Data Header Length

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Length in bytes

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN uint32 wms_ts_compute_user_data_header_length
(
  const uint8           num_headers,
  const wms_udh_s_type *headers
);

#ifdef FEATURE_GWSMS
/*=========================================================================
FUNCTION
  wms_ts_compute_gw_user_data_length

DESCRIPTION
  Computes the GW User Data Length

DEPENDENCIES
  None

RETURN VALUE
  GW User Data Length in bytes

SIDE EFFECTS
  None
=========================================================================*/
QC_EXTERN uint32 wms_ts_compute_gw_user_data_length
(
  const wms_gw_dcs_s_type         *dcs,
  const wms_gw_user_data_s_type   *user_data
);
#endif /* FEATURE_GWSMS */

void wms_ts_init
(
  void
);

#endif /* WMSTS_H */

#ifdef __cplusplus
}
#endif

