#ifndef PS_PHYS_LINK_IOCTL_H
#define PS_PHYS_LINK_IOCTL_H

/*===========================================================================

                          P S _ P H Y S _ L I N K _I O C T L. H

DESCRIPTION

  The Protocol Services interface file. This contains variables and
  definitions used by the various modules to access interface definitions.

Copyright (c) 2004-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_phys_link_ioctl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/14/08    pp     Common Modem Interface: Public/Private API split.
07/18/08    dm     Fixed compiler warnings
05/03/05    msr    Changed signature of ps_phys_link_ioctl() and moved all
                   phys link IOCTLs here
04/17/05    msr    Removed QOS definitions.
06/29/04    usb    Created file
===========================================================================*/

#include "comdef.h"

#include "ps_iface_defs.h"



/*===========================================================================

                         PHYS LINK IOCTL Definitions

===========================================================================*/
/*---------------------------------------------------------------------------
  Definition of various ps_phys_link control operations. Please do NOT change
  the values of the existing enumerated types when adding new ones (to ensure
  that object code released with previous versions of the code do not break).
  Each of the following control operations have associated tyepdef (with
  corresponding names) that are passed in or returned for get/set operations.

  The most significant bit of the enum value MUST be set for all these
  IOCTLs. This is used as a distinguishing factor between ps_iface, ps_flow
  and phys_link releated IOCTLs.

  Note:
    GET operations are assigned even numbers
    SET operations are assigned odd numbers.

    Also ensure that symetric operations are grouped.
---------------------------------------------------------------------------*/
#define PS_PHYS_LINK_IOCTL_MASK      0x20000000

typedef enum
{
  PS_PHYS_LINK_IOCTL_GO_ACTIVE              = 0x20000013, /* Go active from
                                                                  dormant    */
  PS_PHYS_LINK_IOCTL_GO_DORMANT             = 0x20000014, /* Go dormant      */
  PS_PHYS_LINK_IOCTL_GET_STATE              = 0x20000018, /* Get physical link
                                                           state             */
  PS_PHYS_LINK_IOCTL_GET_DORMANCY_INFO_CODE = 0x20000020, /* Get dormancy info
                                                 code when phys link is down */
  PS_PHYS_LINK_IOCTL_GET_CALL_ID            = 0x20000022,   /*Get call id */
  PS_PHYS_LINK_IOCTL_ABORT_GO_DORMANT       = 0x20000023,   /*Abort previously
                                                     issued dormancy request */ 
} ps_phys_link_ioctl_type;


#ifdef __cplusplus
extern "C" {
#endif
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
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
);

#ifdef __cplusplus
}
#endif

#endif /* PS_PHYS_LINK_IOCTL_H */
