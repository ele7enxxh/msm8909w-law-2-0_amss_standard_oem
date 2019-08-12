#ifndef INC_GLLC_H
#define INC_GLLC_H
/*****************************************************************************

TITLE  gllc.h

 GPRS LLC Layer Service Interface


DESCRIPTION

 Provides the interface functions and type definitions which form the
 GPRS LLC Layer Service Provision.


Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
All Rights Reserved. QUALCOMM Proprietary.

Export of this technology or software is regulated by the U.S.
Government. Diversion contrary to U.S. law prohibited.

**************************************************************************/


/*****************************************************************************

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gllc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------
09-02-19   tjw     Automatically split from original header file and then tidied.

**************************************************************************/

/*****************************************************************************

    Public Defines & Constants

**************************************************************************/

/* GPRS L3/LL PDU Sizes */

#define GPRS_LLC_L3_PDU_MAX_OCTETS      1520


#define GPRS_LLC_LL_PDU_HDR_MAX_OCTETS  36  /* I+S Frame Header */
#define GPRS_LLC_LL_PDU_FCS_OCTETS      3

#define GPRS_LLC_LL_PDU_MAX_OCTETS \
        ( \
          GPRS_LLC_LL_PDU_HDR_MAX_OCTETS +  \
          GPRS_LLC_L3_PDU_MAX_OCTETS     +  \
          GPRS_LLC_LL_PDU_FCS_OCTETS        \
        )

#endif /* INC_GLLC_H   */
