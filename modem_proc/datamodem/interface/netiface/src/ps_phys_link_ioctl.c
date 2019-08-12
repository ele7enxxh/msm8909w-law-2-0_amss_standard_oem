/*===========================================================================

                      P S _ P H Y S _ L I N K _ I O C T L. C

DESCRIPTION
  This file contains functions used by various modules to access
  network interface.

EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ioctl.c_v   1.8   13 Feb 2003 14:15:20   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_phys_link_ioctl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/29/05    msr    Lint changes.
05/12/05    mct    Lint changes.
05/03/05    msr    Changed signature of ps_phys_link_ioctl()
06/29/04    usb    Initial version.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ps_phys_link_ioctl.h"
#include "ps_phys_link.h"
#include "msg.h"
#include "err.h"
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_crit_sect.h"


/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_PHYS_LINK_IOCTL()

DESCRIPTION
  This function performs various operations on the given phys link.
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS

  phys_link_ptr             - Phys link on which the specified operations
                              is to be performed

  ioctl_name                - The operation name

  argval_ptr                - Pointer to operation specific structure

  ps_errno                   - Error code returned in case of failure (Error
                               values are those defined in dserrno.h)

                              DS_EINVAL - Returned when the specified IOCTL
                              does not belong to the common set of IOCTLs
                              and there is no IOCTL mode handler registered
                              for the specified interface.

                              DS_EOPNOTSUPP - Returned by the lower level
                              IOCTL mode handler when specified IOCTL is not
                              supported by the interface. For instance, this
                              would be returned by interfaces that do not
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the
                              implementation is mode specific)

                              DS_EFAULT - This error code is returned if the
                              specified arguments for the IOCTL are correct
                              but an error is encountered while executing
                              the IOCTL.

                              DS_NOMEMORY - This error code is returned if we
                              run out of mempory buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
int ps_phys_link_ioctl
(
  ps_phys_link_type        *phys_link_ptr,
  ps_phys_link_ioctl_type   ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  phys_link_state_type *    phys_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *ps_errno = 0;

  if(!(PS_PHYS_LINK_IS_VALID(phys_link_ptr)))
  {
    LOG_MSG_ERROR_2("ps_phys_link_ioctl(): "
                    "IOCTL 0x%x failed, invalid phys link 0x%p",
                    ioctl_name, phys_link_ptr);
    *ps_errno = DS_EBADF;
    return -1;
  }

  if (PS_PHYS_LINK_IOCTL_GET_STATE == ioctl_name)
  {
    if(argval_ptr == NULL)
    {
      *ps_errno = DS_EFAULT;
      return -1;
    }
    phys_state = (phys_link_state_type *)argval_ptr;
    *phys_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);
    return 0;
  }
  else if (PS_PHYS_LINK_IOCTL_GET_DORMANCY_INFO_CODE == ioctl_name)
  {
    if(argval_ptr == NULL)
    {
      *ps_errno = DS_EFAULT;
      return -1;
    }
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    (*(ps_extended_info_code_enum_type *)argval_ptr) =
      phys_link_ptr->dormancy_info_code;

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    return 0;
  }
  /*---------------------------------------------------------------------
    Phys link specific operation. Call the registered callback.
  ---------------------------------------------------------------------*/
  if(phys_link_ptr->ioctl_f_ptr != NULL)
  {
    LOG_MSG_INFO2_1("ps_phys_link_ioctl(): "
                    "Interface specific ioctl=0x%x, calling callback",
                    ioctl_name);
    return phys_link_ptr->ioctl_f_ptr(phys_link_ptr,
                                      ioctl_name,
                                      argval_ptr,
                                      ps_errno);
  }
  else
  {
    LOG_MSG_INFO1_2("ps_phys_link_ioctl(): "
                    "Unsupported IOCTL 0x%x on phys link 0x%p",
                    ioctl_name, phys_link_ptr);
    *ps_errno = DS_EINVAL;
    return -1;
  }
} /* ps_phys_link_ioctl() */

