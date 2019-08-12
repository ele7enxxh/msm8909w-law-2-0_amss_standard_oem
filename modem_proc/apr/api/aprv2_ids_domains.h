#ifndef __APRV2_IDS_DOMAINS_H__
#define __APRV2_IDS_DOMAINS_H__

/** @file aprv2_ids_domains.h
*/

/*===========================================================================
Copyright (c) 2010-2012 Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/api/aprv2_ids_domains.h#1 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
===========================================================================*/

/** @addtogroup aprv2_data_types
@{ */

/** @name Domain ID Definitions
    @xreflabel{hdr:DomainIdDefinitions}
  The transport mechanism for the published domains varies per target. New
  Domain ID aliases can be defined to lock down the actual transport mechanism,
  e.g., APRV2_IDS_DOMAIN_ID_PC_DIAG_V or APRV2_IDS_DOMAIN_ID_MODEM_SMD1_V.
@{ */

#define APRV2_IDS_DOMAIN_ID_RESERVED_V ( 0 ) /**< Reserved domain. */
#define APRV2_IDS_DOMAIN_ID_SIM_V      ( 1 ) /**< Simulation domain. */
#define APRV2_IDS_DOMAIN_ID_PC_V       ( 2 ) /**< PC domain. */
#define APRV2_IDS_DOMAIN_ID_MODEM_V    ( 3 ) /**< Modem domain. */
#define APRV2_IDS_DOMAIN_ID_ADSP_V     ( 4 ) /**< aDSP domain. */
#define APRV2_IDS_DOMAIN_ID_APPS_V     ( 5 ) /**< Application domain. */
#define APRV2_IDS_DOMAIN_ID_MODEM2_V   ( 6 ) /**< Second modem domain. */
#define APRV2_IDS_DOMAIN_ID_APPS2_V    ( 7 ) /**< Second application domain. */
#define APRV2_IDS_DOMAIN_ID_ADSP2_V    ( 8 ) /**< Second ADSP domain for 8926 . */

/** @} */ /* end_name Domain ID Definitions */
/** @} */ /* end_addtogroup aprv2_data_types */

#endif /* __APRV2_IDS_DOMAINS_H__ */

