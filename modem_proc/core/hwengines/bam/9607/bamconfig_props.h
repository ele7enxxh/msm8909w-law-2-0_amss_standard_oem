#ifndef BAMCONFIG_PROPS_H_
#define BAMCONFIG_PROPS_H_

/**
  @file bamconfig_props.h
  @brief
  This file contains definitions of constants, data structures and
  interfaces of base address and irq vector of BAM hardware.

*/
/*
===============================================================================

                             Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/18/15   SA      Created (from 8909).

-----------------
Previous history
-----------------
07/10/14   PR      Branched from config/8916 and updated the 8909 supported BAMs.

===============================================================================
                   Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "DALStdDef.h"

/* Module base address info  */
#define USB2_HSIC_BASE       0x078C0000
#define BLSP1_BASE           0x07880000
#define QDSS_TRACE_BASE      0x06000000
#define MSS_TOP_BASE         0x04000000
#define CRYPTO_TOP_BASE      0x00700000


/* Bam Module base address info */
#define BAM_USB2_HSIC_BASE        (USB2_HSIC_BASE  + 0x00004000)
#define BAM_LITE_BLSP1_BASE       (BLSP1_BASE      + 0x00004000)
#define BAM_NDP_QDSS_BASE         (QDSS_TRACE_BASE + 0x00084000)
#define BAM_NDP_MSS_A2_BASE       (MSS_TOP_BASE    + 0x00044000)
#define BAM_NDP_MSS_CRYPTO_BASE   (MSS_TOP_BASE    + 0x001C4000)
#define BAM_NDP_CRYPTO1_BASE      (CRYPTO_TOP_BASE + 0x00004000)
#define BAM_QPIC_BASE             0x7984000

/* BAM module irq vector in Apps image */
#ifdef BAM_APPS_CONFIG
#define BAM_NDP_CRYPTO1_IRQVECTOR           239
#define BAM_LITE_BLSP1_IRQVECTOR            270
#define BAM_USB2_IRQVECTOR                  167
#define BAM_NDP_QDSS_IRQVECTOR              199
#define BAM_QPIC_IRQVECTOR                  164
#endif/* BAM_APPS_CONFIG */

/* BAM module irq vector in mpss image */
#ifdef BAM_MODEM_CONFIG
#define BAM_LITE_BLSP1_IRQVECTOR            244
#define BAM_NDP_MSS_CRYPTO_IRQVECTOR         74
#define BAM_MSS_A2_IRQVECTOR                 61
#define BAM_QPIC_IRQVECTOR                   77
#endif /* BAM_MODEM_CONFIG */

/* BAM module irq vector in TZOS image */
#ifdef BAM_TZOS_CONFIG
#define BAM_NDP_CRYPTO1_IRQVECTOR           238
#define BAM_LITE_BLSP1_IRQVECTOR            270
#define BAM_IRQVECTOR_NONE                    0
#endif/* BAM_TZOS_CONFIG */

/* Bam Cilent config info structure */
typedef struct
{
    uint32  base_pa;
    uint32  irq_vector;
}bamclient_config_info_type;

#endif /* BAMCONFIG_PROPS_H_ */


