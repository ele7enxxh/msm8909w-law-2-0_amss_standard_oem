#ifndef _DS_RMNET_DEFS_H
#define _DS_RMNET_DEFS_H
/*===========================================================================

                         D S _ R M N E T _ D E F S . H

DESCRIPTION

  Rm Network device - Definitions internal header file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ds_rmnet_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
07/11/12    pgm    Added support for RNDIS agg mode.
02/02/12    cy     Added support for MBIM NTB parameters
09/18/11    wc     Nikel and MPSS.NI.1.1 port configuration
05/02/11    ua     Enabling Autoconnect to working with first instance across
                   all targets.
03/09/11    kk     Removing DCC command buffer dependency from RmNet.
06/19/10    vs     Changes for Dual IP support over single QMI instance
11/29/06    jd     Added DCTM support to RmNet
10/09/06    ks     Added rmnet cmds for Flow enable/disable.
08/23/06    ks     Created file
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "cm.h"
#include "ps_sys.h"
#include "ds_qmi_svc_ext.h"

#if defined(FEATURE_IP_CALL)
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  #include "cmipapp.h"
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif /* defined(FEATURE_IP_CALL) */


/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum RmNet dual IP instances. The first QMI_INSTANCE_MAX instances
  are for legacy non-mux calls.
---------------------------------------------------------------------------*/
#define RMNET_DUAL_IP_MAX (QMI_INSTANCE_MAX + 16) // 8 emb + 8 teth

/*---------------------------------------------------------------------------
  Net-RMSM instance enumeration.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_INSTANCE_MIN      =  0,
  RMNET_INSTANCE_MAX      =  (RMNET_DUAL_IP_MAX * 2)
} rmnet_instance_e_type;

/*---------------------------------------------------------------------------
  Data format mask used in set data format.
---------------------------------------------------------------------------*/
#define RMNET_DATA_FORMAT_MASK_QOS                (0x00000001)
#define RMNET_DATA_FORMAT_MASK_LINK_PROT          (0x00000002)
#define RMNET_DATA_FORMAT_MASK_UL_DATA_AGG        (0x00000004)
#define RMNET_DATA_FORMAT_MASK_DL_DATA_AGG        (0x00000008)
#define RMNET_DATA_FORMAT_MASK_NDP_SIGNATURE      (0x00000010)
#define RMNET_DATA_FORMAT_MASK_DL_DATA_AGG_NUM    (0x00000020)
#define RMNET_DATA_FORMAT_MASK_DL_DATA_AGG_SIZE   (0x00000040)
#define RMNET_DATA_FORMAT_MASK_QOS_FORMAT         (0x00000080)
#define RMNET_DATA_FORMAT_MASK_DL_MIN_PADDING     (0x00000100)
#define RMNET_DATA_FORMAT_MASK_FLOW_CONTROL       (0x00000200)

/*---------------------------------------------------------------------------
  Default value used in set data format.
---------------------------------------------------------------------------*/
#define RMNET_DATA_FORMAT_MINIMUM_DL_DATA_AGG_SIZE   (2048)

/*---------------------------------------------------------------------------
  Default Values used in set data format for DL Aggr params
---------------------------------------------------------------------------*/
#define RMNET_RNDIS_HDR_LEN           (44)         /*RNDIS spec defined*/
#define RMNET_ETH_PKT_LEN             (1512)       /*14 byte Eth hdr + 1498*/
#define RMNET_MIN_RNDIS_PKT_SIZE       \
        (RMNET_RNDIS_HDR_LEN + RMNET_ETH_PKT_LEN)

/*Default DL aggr params that will be set in A2*/
#define RMNET_DATA_FORMAT_MAX_RNDIS_AGGR_PKTS   (15)
#define RMNET_DATA_FORMAT_MAX_RNDIS_AGGR_SIZE   \
        (RMNET_DATA_FORMAT_MAX_RNDIS_AGGR_PKTS * RMNET_MIN_RNDIS_PKT_SIZE)
#define RMNET_DATA_FORMAT_MIN_RNDIS_DL_DATA_AGG_SIZE  RMNET_MIN_RNDIS_PKT_SIZE

#define RMNET_DATA_FORMAT_QMAP_MAX_AGGR_PKTS (20)
#define RMNET_DATA_FORMAT_QMAP_DEFAULT_AGGR_PKTS (10)
#define RMNET_DATA_FORMAT_QMAP_DEFAULT_AGGR_SIZE (1536)

typedef enum
{
  RMNET_SET_DATA_FORMAT_NONE          = 0,
  RMNET_SET_DATA_FORMAT_QMI_CTL       = 1, /* To be used by QMI CTL message when setting
                                              the data format */
  RMNET_SET_DATA_FORMAT_QMI_WDA,           /* To be used by QMI WDA message when setting
                                    the data format */
  RMNET_SET_DATA_FORMAT_DTR_HGH,     /* This will be used as part of auto connect
                                    triggering the call and setting the data
                                    format as part of it.  */
  RMNET_SET_DATA_FORMAT_DTR_LOW        /* This will be used in set data format at
                                    DTR Low, allowing flexibilty at each DTR
                                    high to pick either Driver SET CTL or the
                                    configuration in the EFS */
}rmnet_set_data_fmt_e_type;

typedef enum
{
  RMNET_SET_DATA_FORMAT_SUCCESS = 1,
  RMNET_SET_DATA_FORMAT_FAILURE,
  RMNET_SET_DATA_FORMAT_NO_EFFECT
}rmnet_set_data_fmt_ret_e_type;

#endif /* _DS_RMNET_DEFS_H */

