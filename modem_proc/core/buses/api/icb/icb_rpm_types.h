#ifndef ICB_RPM_TYPES_H
#define ICB_RPM_TYPES_H
/*
===========================================================================

FILE:         icb_rpm_types.h

DESCRIPTION:  Header file for the ICB interface to the ICB RPM resources.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/icb/icb_rpm_types.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/06/20   sds     Extend request types to add latency request in addition to BW.
2012/01/19   sds     Initial revision.

===========================================================================
             Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "icbid.h" /* for external ids */

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/* This enum describes the available fields that can be sent to the RPM
** for RPM_BUS_SLAVE_REQ.
*/
typedef enum
{
  ICB_SLAVE_FIELD_BW = 0x00007762, /**< 'bw\0\0' little endian */
} icb_rpm_slave_fields_type;

/* This enum describes the available fields that can be sent to the RPM
** for RPM_BUS_MASTER_REQ.
*/
typedef enum
{
  ICB_MASTER_FIELD_BW            = 0x00007762, /**< 'bw\0\0' little endian */
  ICB_MASTER_FIELD_BW_THRESHOLD0 = 0x30747762, /**< 'bwt0' little endian */
  ICB_MASTER_FIELD_BW_THRESHOLD1 = 0x31747762, /**< 'bwt1' little endian */
  ICB_MASTER_FIELD_BW_THRESHOLD2 = 0x32747762, /**< 'bwt2' little endian */
} icb_rpm_master_bw_fields_type;

/* This enum describes the available fields that can be sent to the RPM
** for RPM_BUS_SPDM_CLK_REQ.
*/
typedef enum
{
  ICB_SPDM_CLK_FIELD_EN = 0x00006e65, /**< 'en\0\0' little endian */
} icb_rpm_spdm_clk_fields_type;

/* This enum describes the available fields that can be sent to the RPM
** for RPM_BUS_MASTER_LATENCY_REQ.
*/
typedef enum
{
  ICB_LATENCY_MASTER_FIELD_NS = 0x736e746c, /**< 'ltns' little endian */
} icb_rpm_latency_master_fields_type;
#endif /* ICB_RPM_TYPES_H */

