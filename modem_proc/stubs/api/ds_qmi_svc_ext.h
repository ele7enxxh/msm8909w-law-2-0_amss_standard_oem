#ifndef _DS_QMI_SVC_EXT_H
#define _DS_QMI_SVC_EXT_H
/*===========================================================================

                         D S _ Q M I _ S V C _ E X T . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/ds_qmi_svc_ext.h#1 $ $DateTime: 2016/12/13 07:57:47 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/02/11    ua     Enabling Autoconnect to working with first instance across
                   all targets. 
02/09/11    ua     Added support for LTE Data rates.
10/23/09    ar     Added IS_SERVICE_VALID() test marco.
10/22/09    kk     Windows7 compliance - WDS changes and BER/PER/FER support.
12/12/06    ks     Created module
===========================================================================*/

#include "comdef.h"

typedef enum
{
  QMI_INSTANCE_MIN     = 0, 

#if defined(FEATURE_DATA_NIKEL)
  /*-------------------------------------------------------------------------
    This tier is for NikeL architecture.
      Require 8 instances to support 4 dual-IP connections from apps
        Optional 2 more instances for dual-IP connections on 5th PDN
      Require 1 instance to support 1 dual-IP connection TE 
  -------------------------------------------------------------------------*/
  QMI_INSTANCE_RMNET1  = QMI_INSTANCE_MIN,  //Apps proc 1
  QMI_INSTANCE_RMNET2,                      //Apps proc 2
  QMI_INSTANCE_RMNET3,                      //Apps proc 3
  QMI_INSTANCE_RMNET4,                      //Apps proc 4
  QMI_INSTANCE_RMNET5,                      //Apps proc 5
  QMI_INSTANCE_RMNET6,                      //Apps proc 6
  QMI_INSTANCE_RMNET7,                      //Apps proc 7
  QMI_INSTANCE_RMNET8,                      //Apps proc 8
  #if defined(FEATURE_DATA_5_PDN)
    QMI_INSTANCE_RMNET9,                      //Apps proc 9
    QMI_INSTANCE_RMNET10,                     //Apps proc 10
    QMI_INSTANCE_APPS_PROC_MAX = QMI_INSTANCE_RMNET10,
    QMI_INSTANCE_RMNET11,                     //laptop 1
  #else
    QMI_INSTANCE_APPS_PROC_MAX = QMI_INSTANCE_RMNET8,
    QMI_INSTANCE_RMNET9,                      //laptop 1
  #endif /* FEATURE_DATA_5_PDN */
#elif defined(FEATURE_DATA_FUSION_MDM)
  /*-------------------------------------------------------------------------
    This tier is for Fusion architecture (MSM7x30 - MDM9K velcro) - MDM Tier.
      Require 8 instances to support 4 dual-IP connections from 7x30 Apps
      Require 1 instances to support 1 dual-IP connection over single rmnet from TE 
        Optional 1 more tethered instance for CSFB Fusion
  -------------------------------------------------------------------------*/
  QMI_INSTANCE_RMNET1  = QMI_INSTANCE_MIN,  //Apps proc 1
  QMI_INSTANCE_RMNET2,                      //Apps proc 2
  QMI_INSTANCE_RMNET3,                      //Apps proc 3
  QMI_INSTANCE_RMNET4,                      //Apps proc 4
  QMI_INSTANCE_RMNET5,                      //Apps proc 5
  QMI_INSTANCE_RMNET6,                      //Apps proc 6
  QMI_INSTANCE_RMNET7,                      //Apps proc 7
  QMI_INSTANCE_RMNET8,                      //Apps proc 8
  QMI_INSTANCE_APPS_PROC_MAX = QMI_INSTANCE_RMNET8,
  #ifndef FEATURE_DATA_FUSION_CSFB
    QMI_INSTANCE_RMNET9,                      //laptop 1
    QMI_INSTANCE_RMNET10,                     //laptop 2
  #else 
    #error code not present
#endif
#elif defined(FEATURE_DATA_FUSION_MSM)
  /*-------------------------------------------------------------------------
    This tier is for Fusion architecture (MSM7x30 - MDM9K velcro) - MSM Tier.
      Require 3 QMI instances to support embedded calls from 7x30 Apps.
        Optional 2 instances for embedded MCAST calls from 7x30 Apps.
        Optional 2 instances for laptop calls using SMD-USB port bridge.
      Require 1 QMI instance to support 1X proxy calls from MDM9K.
  -------------------------------------------------------------------------*/
  QMI_INSTANCE_RMNET1  = QMI_INSTANCE_MIN,  //Apps proc 1 
  QMI_INSTANCE_RMNET2,                      //Apps proc 2
  QMI_INSTANCE_RMNET3,                      //Apps proc 3
  QMI_INSTANCE_RMNET4,                    //Apps proc 4  
  QMI_INSTANCE_RMNET5,                    //Apps proc 5 
  QMI_INSTANCE_APPS_PROC_MAX = QMI_INSTANCE_RMNET5,
  #ifdef FEATURE_DATA_RM_NET_USES_SM_LAPTOP_INST
    QMI_INSTANCE_RMNET6,                    //laptop 1 
    #ifdef FEATURE_DATA_RM_NET_MULTI_USB
      QMI_INSTANCE_RMNET7,                  //laptop 2 
    #endif
  #endif /* FEATURE_DATA_RM_NET_USES_SM_LAPTOP_INST */
  QMI_INSTANCE_RMNET8,                      //MDM proc
#elif defined(FEATURE_DATA_RM_NET_USES_SM)
  /*-------------------------------------------------------------------------
    This tier is for regular multi-proc (7x30 or 8x60) architecture.
      Require 8 instances to support 4 dual-IP connections from apps
      Require 1 instances to support 1 dual-IP connection over single rmnet from TE 
        Optional 1 more tethered instance for multi-RmNet over USB
  -------------------------------------------------------------------------*/
  QMI_INSTANCE_RMNET1  = QMI_INSTANCE_MIN,  //Apps proc 1 
  QMI_INSTANCE_RMNET2,                      //Apps proc 2
  QMI_INSTANCE_RMNET3,                      //Apps proc 3
  QMI_INSTANCE_RMNET4,                    //Apps proc 4  
  QMI_INSTANCE_RMNET5,                    //Apps proc 5 
  QMI_INSTANCE_RMNET6,                      //Apps proc 6
  QMI_INSTANCE_RMNET7,                      //Apps proc 7
  QMI_INSTANCE_RMNET8,                      //Apps proc 8
  QMI_INSTANCE_APPS_PROC_MAX = QMI_INSTANCE_RMNET8,
  #ifdef FEATURE_DATA_RM_NET_USES_SM_LAPTOP_INST
    QMI_INSTANCE_RMNET9,                    //laptop 1 
    #ifdef FEATURE_DATA_RM_NET_MULTI_USB
      QMI_INSTANCE_RMNET10,                  //laptop 2 
    #endif /* FEATURE_DATA_RM_NET_MULTI_USB */
  #endif /* FEATURE_DATA_RM_NET_USES_SM_LAPTOP_INST */
#else
  /*-------------------------------------------------------------------------
    This tier is for regular single-proc (6xxx) architecture.
      Require 5 instances to support 5 dual-IP connections over USB/MUX
  -------------------------------------------------------------------------*/
  QMI_INSTANCE_RMNET1  = QMI_INSTANCE_MIN,  //Apps proc 1 
  QMI_INSTANCE_RMNET2,                      //Apps proc 2
  QMI_INSTANCE_RMNET3,                      //Apps proc 3
  QMI_INSTANCE_RMNET4,                    //Apps proc 4  
  QMI_INSTANCE_RMNET5,                    //Apps proc 5 
  QMI_INSTANCE_APPS_PROC_MAX = QMI_INSTANCE_RMNET5,
#endif 

  QMI_INSTANCE_MAX
} qmi_instance_e_type;
#endif /* _DS_QMI_SVC_EXT_H */
