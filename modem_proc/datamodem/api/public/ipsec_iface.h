#ifndef IPSEC_IFACE_H
#define IPSEC_IFACE_H

/*===========================================================================
                        I P S E C _ I F A C E . H

DESCRIPTION
  IPsec IFACE MACROS.

Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ipsec_iface.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/26/10    pp     IPSEC IFACEs tuning.
03/25/09    sp     Added support for DSCP marking.
02/26/09    sp     Created module - Moved IPsec Mode Handler to Data
===========================================================================*/

#define IPSEC_IFACE_HDLR_MAX_IFACES 16
#define IPSEC_IFACE_MAX_FLOWS  4
#define IPSEC_IFACE_HDLR_MAX_TUNNELS 2
#define IPSEC_IFACE_HDLR_MAX_IFACE_LIST ( IPSEC_IFACE_HDLR_MAX_TUNNELS + 2 )

#ifdef FEATURE_GAN
#error code not present
#endif /* FEATURE_GAN */

#endif /* IPSEC_IFACE_H */
