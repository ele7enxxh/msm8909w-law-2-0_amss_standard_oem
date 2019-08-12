#ifndef QMI_SVC_UTILS_H
#define QMI_SVC_UTILS_H
/*===========================================================================

                      Q M I _ S V C _ U T I L S . H

DESCRIPTION

  The QMI Services external interface utils file.

EXTERNALIZED UTIL FUNCTIONS
  qmi_svc_put_param_tlv()
    Construct a message option TLV from given type, length, and value in
    the provided DSM item.

  qmi_svc_put_result_tlv()
    Construct a result option TLV from given result and error code in
    the provided DSM item.

  qmi_svc_get_tl()
    Extract the type and length from the message.

  qmi_svc_prepend_msg_hdr(): Append the message header type and length to
    DSM Item.

Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/20/12    sa      New QMI utils API for SPC checks.
08/30/11    rk      Added bundle message support to QMI Framework
01/20/11    rk      Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dsm.h"
#include "qmi_svc_defs.h"

/*===========================================================================

                               DATA TYPES

===========================================================================*/
/**
  @brief Enumeration for the result code to SPC check routine.
*/
typedef enum
{
  QMI_SVC_SPC_OK           = 0x00, /**< SPC code check succeeded. */
  QMI_SVC_SPC_FAIL         = 0x01, /**< SPC code check failed. */
  QMI_SVC_SPC_LOCKED       = 0x02, /**< SPC code locked. */
  QMI_SVC_SPC_ERR_INTERNAL = 0xFF  /**< SPC check failed due
                                        to internal error. */
} qmi_svc_spc_result_e_type;

/*===========================================================================

                   EXTERNAL QMI SVC UTIL MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO QMI_SVC_HDLR()

DESCRIPTION
  Returns a qmi_svc_handler initializer given command type and handler
  function.

PARAMETERS
  cmdval : The command type value of the message
  hdlr   : The name of the handler function

RETURNS
  An initializer of qmi_svc_handler type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define QMI_SVC_HDLR(cmdval,hdlr)  { cmdval, #hdlr, hdlr }

/*===========================================================================
MACRO QMI_SVC_PKT_PUSH()

DESCRIPTION
  To push down the given value, val of length len, onto pkt.

PARAMETERS
  pkt : packet
  val : value
  len : length

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define QMI_SVC_PKT_PUSH(pkt,val,len)  ( len == dsm_pushdown_packed(pkt,\
                                                  val,\
                                                  len,\
                                                  DSM_DS_SMALL_ITEM_POOL ) )

/*===========================================================================

                   EXTERNAL QMI SVC UTIL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_SVC_PUT_PARAM_TLV()

DESCRIPTION
  Construct a TLV using the input type, length and value

PARAMETERS
  pkt   : message to append the TLV to
  type  : value to be put in type field
  len   : value to be put in length field
  value : contents of the value field of TLV

RETURN VALUE
  TRUE  - Input tlv construction is success.
  FALSE - Input tlv construction is failed.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean  qmi_svc_put_param_tlv
(
  dsm_item_type **  pkt,
  byte              type,
  uint16            len,
  void *            value
);

/*===========================================================================
FUNCTION QMI_SVC_PUT_RESULT_TLV()

DESCRIPTION
  Construct a Result Code TLV using the input result and error code

PARAMETERS
  response : response message to append the TLV to
  result   : result code
  error    : error code


RETURN VALUE
  TRUE - Result code tlv construction is success.
  FALSE - Result code tlv construction is failed.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean  qmi_svc_put_result_tlv
(
  dsm_item_type **   response,
  qmi_result_e_type  result,
  qmi_error_e_type   error
);

/*===========================================================================
FUNCTION QMI_SVC_GET_TL()

DESCRIPTION
  Extract the type and length from the message

PARAMETERS
  pkt  : message
  type : value in type field
  len  : value in length field

RETURN VALUE
  TRUE   - extracted success.
  FALSE  - extraction failed.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean  qmi_svc_get_tl
(
  dsm_item_type **  pkt,
  uint8 *           type,
  uint16 *          len
);

/*===========================================================================
  FUNCTION QMI_SVC_PREPEND_MSG_HDR()

  DESCRIPTION
    Append the message header type and length to the provided QMI service
    message payload

  PARAMETERS
    pkt     : message payload
    cmd_val : message type value

  RETURN VALUE
    TRUE
    FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_prepend_msg_hdr
(
  dsm_item_type **  pkt,
  uint16            cmd_val
);

/*===========================================================================

FUNCTION QMI_SVC_CHECK_SPC()

===========================================================================*/
/** @ingroup qmi_svc_check_spc
  This function checks the SPC and OTKSL. The SPC code itself is used to
  enforce read/write access to protected NV items like ACCOLC etc.

  @param[in] spc code         SPC code to be verified.
  @param[in] otksl_check_flag Flag to specify whether OTKSL needs to be
                              checked if SPC check fails.

  @return
  SPC Check Result.

  @dependencies
  none
  @newpage
*/
qmi_svc_spc_result_e_type qmi_svc_check_spc
(
  uint8 * spc_code,
  boolean otksl_check_flag
);
#endif /* QMI_SVC_UTIL_H */
