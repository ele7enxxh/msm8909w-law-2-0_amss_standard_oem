#ifndef DS_QMUX_MSG_H
#define DS_QMUX_MSG_H

/*===========================================================================

                         D S _ Q M U X _ M S G . H

DESCRIPTION

  The Data Services Qualcomm MSM Interface control channel message 
  multiplexing protocol message definition header file.

EXTERNALIZED FUNCTIONS
  None

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmux_msg.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/31/05   jd/ks   Code review updates
05/16/05    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  QMUX PDU header constants
  - interface type
  - control flag bit definitions and values
---------------------------------------------------------------------------*/
#define QMUXI_IFTYPE_QMUX                 (0x01)
#define QMUXI_IFTYPE_QMUX_FRAMED          (0x02)

#define QMUXI_CFLAG_MASK_SENDER           (1 << 7)
#define QMUXI_CFLAG_MASK_RESERVED         (0x7F)

#define QMUXI_CFLAG_SENDER_CONTROL_POINT  (0x00)
#define QMUXI_CFLAG_SENDER_SERVICE        (0x80)


/*===========================================================================

                                DATA TYPES

===========================================================================*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif


/*---------------------------------------------------------------------------
  QMUX PDU header type
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16  len;
  uint8   ctl_flags;
  uint8   svc_type;
  uint8   clid;
} qmux_hdr_type;


#ifdef TEST_FRAMEWORK
#error code not present
#endif

#endif /* DS_QMUX_MSG_H */
