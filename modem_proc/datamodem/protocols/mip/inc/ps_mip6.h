#ifndef PS_MIP6_H
#define PS_MIP6_H
/*===========================================================================

                              P S _ M I P 6 . H
                   
DESCRIPTION
  The header file for MIP6 external/common declarations.

EXTERNALIZED FUNCTIONS

 Copyright (c)2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/ps_mip6.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#ifdef FEATURE_DATA_PS_MIPV6
#include "dhcp_client_common.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_FAIL_REASON_TYPE

DESCRIPTION
  Enum of MIP6 fail reason code
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_FAIL_REASON_NONE        = 0,
  MIP6_FAIL_REASON_REG_FAILED  = 1,
  MIP6_FAIL_REASON_UNKNOWN     = 2,
  MIP6_FAIL_REASON_PROC_ERROR  = 3,
  MIP6_FAIL_REASON_MAX
} ps_mip6_fail_reason_type;

/*---------------------------------------------------------------------------
TYPEDEF MIP6_STATUS_E_TYPE

DESCRIPTION
  Enum describing MIP6 status
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_MIN_EV       = 0,
  MIP6_SUCCESS_EV   = 0,
  MIP6_FAILURE_EV   = 1,
  MIP6_MAX_EV,
} mip6_status_e_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION  PS_MIP6_IS_ENABLED()

DESCRIPTION
  Determines whether MIP6 is turned ON or not.

DEPENDENCIES

RETURN VALUE
  TRUE  on Success.
  FALSE on Failure.

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_mip6_is_enabled
(
  void
);
#endif /* FEATURE_DATA_PS_MIPV6 */
#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6*/
#endif /* PS_MIP6_H */
