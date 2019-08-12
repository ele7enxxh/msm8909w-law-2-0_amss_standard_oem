#ifndef PS_QSL_H
#define PS_QSL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               P S _ Q O S _ S P E C _ L O G G I N G . H


GENERAL DESCRIPTION
  This file provides an API to log QOS specification. Requested QOS,
  granted QOS and QOS status transitions are logged

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c)2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface.c_v   1.27   24 Feb 2003 10:55:32   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_qos_spec_logging.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/12/06    msr    Logging info_code along with QoS state
11/30/05    msr    Logging QOS_MODIFY as well and lint fixes
08/15/05    msr    Created file
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_PS_QOS
#include "ps_qos_defs.h"
#include "ps_iface_defs.h"



/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Opcodes used in logging requested QOS spec
---------------------------------------------------------------------------*/
typedef enum
{
  QSL_QOS_REQUEST    = 0,
  QSL_QOS_CONFIGURE  = 1
} qsl_qos_opcode_enum_type;

/*---------------------------------------------------------------------------
  Various QOS states that are logged in ps_qsl_log_qos_state()
---------------------------------------------------------------------------*/
typedef enum
{
  QSL_QOS_STATE_MIN       = 0,
  QSL_QOS_GRANTED         = QSL_QOS_STATE_MIN,
  QSL_QOS_NW_MODIFIED     = 1,
  QSL_QOS_SUSPENDED       = 2,
  QSL_QOS_RELEASED        = 3,
  QSL_QOS_MODIFY_ACCEPTED = 4,
  QSL_QOS_MODIFY_REJECTED = 5,
  QSL_QOS_STATE_MAX
} qsl_qos_state_enum_type;



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_QSL_LOG_REQUESTED_QOS_SPEC()

DESCRIPTION
  Logs requested QoS specification

PARAMETERS
  qos_spec_ptr : QoS specification to be logged
  iface_ptr    : iface on which QoS is requested
  flow_ptr     : flow which manages this QoS instance's state
  opcode       : QoS operation (QOS_REQUEST/QOS_CONFIGURE)

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_requested_qos_spec
(
  const qos_spec_type        * qos_spec_ptr,
  const ps_iface_type        * iface_ptr,
  const ps_flow_type         * flow_ptr,
  qsl_qos_opcode_enum_type     opcode
);

/*===========================================================================
FUNCTION PS_QSL_LOG_MODIFIED_QOS_SPEC()

DESCRIPTION
  Logs QoS spec specified in QOS_MODIFY ioctl

PARAMETERS
  qos_spec_ptr : QoS specification to be logged
  flow_ptr     : flow which manages this QoS instance's state

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_modified_qos_spec
(
  const qos_spec_type  * qos_spec_ptr,
  const ps_flow_type   * flow_ptr
);

/*===========================================================================
FUNCTION PS_QSL_LOG_GRANTED_QOS_SPEC()

DESCRIPTION
  Logs granted QoS specification

PARAMETERS
  flow_ptr : flow which manages a QoS instance's state

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_granted_qos_spec
(
  const ps_flow_type  * flow_ptr
);

/*===========================================================================
FUNCTION PS_QSL_LOG_QOS_STATE()

DESCRIPTION
  Logs QoS instance's state

PARAMETERS
  flow_ptr : flow which manages a QoS instance's state
  qos_state : QoS instance's state
  info_code : additional info

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_qos_state
(
  const ps_flow_type               * flow_ptr,
  qsl_qos_state_enum_type            qos_state,
  ps_extended_info_code_enum_type    info_code
);

/*===========================================================================
FUNCTION PS_QSL_LOG_QOS_INFO_CODE_UPDATE()

DESCRIPTION
  Logs info code, which is newly updated on a QoS instance

PARAMETERS
  flow_ptr  : flow which manages a QoS instance's state
  info_code : additional info available for this QoS instance

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_qos_info_code_update
(
  const ps_flow_type               * flow_ptr,
  ps_extended_info_code_enum_type    info_code
);

#endif /* FEATURE_DATA_PS_QOS */
#endif /* PS_QSL_H */
