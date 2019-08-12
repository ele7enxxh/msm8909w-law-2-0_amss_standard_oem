#ifndef PS_QOS_DEFS_H
#define PS_QOS_DEFS_H

/*===========================================================================

                          P S _ Q O S _ D E F S . H

DESCRIPTION

Copyright (c) 2002-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_qos_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/30/11    aa     Added ioctl for Network Initiated QoS 
07/21/11    ttv    Added changes required for source statistics descriptor.
02/05/07    sv     Added support for IM_CN flag.
11/08/06    msr    Added support for flow priority
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
02/14/06    msr    Added support for QoS over WLAN
02/13/06    mct    Added MAX_NETWORK_SUPPORTED_PROFILE_COUNT definition.
01/09/06    msr    Conforming to new QOS_MODIFY API
10/31/05    mct    Renamed MAX_QOS_FLOWS to better suit its purpose.
08/15/05    mct    Added definition for MAX_QOS_FLOWS supported.
08/15/05    msr    Renamed variables in ip_flow_spec_type
08/10/05    msr    Added new masks to support QOS_MODIFY
06/10/05    mct    Added support for profile ids and multiple flow specs.
04/17/05    msr    Added QOS_MASK_INVALID.
11/10/04    mct    Added more enums to check ranges for QOS.
06/22/04    usb    Initial version
===========================================================================*/

#include "comdef.h"
#include "ps_ipfltr_defs.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        QOS FLOW VALIDATION RULES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  1. A bit in field_mask shall be set for each parameter specified.  If a
     bit is not set, a default  value for the corresponding parameter is
     used. The default value is picked up by the technology handler and could
     be based on the network type and service subscription type.
  2. If one or more parameters are incorrrectly specified, the bits in
     err_mask indicate those parameters.  Hence in case of errors err_mask
     should be checked to see if a parameter validation error has occurred
     which will be cleared otherwise.
  3. If a parameter consisting of multiple sub fields is specified, each sub
     field shall also be correctly specified.  Such parameters are,
     a. data_rate opt1
     b. data_rate opt2
     c. pkt_err_rate
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_3GPP2_ULT
#define MAX_QOS_OPERATIONS_PER_IOCTL 1
#else
#define MAX_QOS_OPERATIONS_PER_IOCTL 10
#endif /*FEATURE_DATA_3GPP2_ULT*/
#define MAX_NETWORK_SUPPORTED_PROFILE_COUNT 255


/*---------------------------------------------------------------------------
  Checking will need to be re-added in ps_flow validation functions for the
  MIN values if they are increased to anything above 0.
---------------------------------------------------------------------------*/
typedef enum
{
  IP_TRF_CLASS_MIN                   = 0,
  IP_TRF_CLASS_CONVERSATIONAL        = 0,
  IP_TRF_CLASS_STREAMING             = 1,
  IP_TRF_CLASS_INTERACTIVE           = 2,
  IP_TRF_CLASS_BACKGROUND            = 3,
  IP_TRF_CLASS_MAX
} ip_traffic_class_enum_type;

typedef enum
{
  UMTS_RES_BIT_ERR_RATE_MIN     = 0,
  UMTS_RES_BIT_ERR_RATE1        = 0,        /* 5*10-2 */
  UMTS_RES_BIT_ERR_RATE2        = 1,        /* 1*10-2 */
  UMTS_RES_BIT_ERR_RATE3        = 2,        /* 5*10-3 */
  UMTS_RES_BIT_ERR_RATE4        = 3,        /* 4*10-3 */
  UMTS_RES_BIT_ERR_RATE5        = 4,        /* 1*10-3 */
  UMTS_RES_BIT_ERR_RATE6        = 5,        /* 1*10-4 */
  UMTS_RES_BIT_ERR_RATE7        = 6,        /* 1*10-5 */
  UMTS_RES_BIT_ERR_RATE8        = 7,        /* 1*10-6 */
  UMTS_RES_BIT_ERR_RATE9        = 8,        /* 6*10-8 */
  UMTS_RES_BIT_ERR_RATE10       = 9,       /* Reserved */
  UMTS_RES_BIT_ERR_RATE_MAX
} umts_residual_ber_enum_type;

typedef enum
{
  UMTS_TRF_HANDLING_PRI_MIN     = 0,
  UMTS_TRF_HANDLING_PRI1        = 0,              /* Priority Level 1 */
  UMTS_TRF_HANDLING_PRI2        = 1,              /* Priority Level 2 */
  UMTS_TRF_HANDLING_PRI3        = 2,              /* Priority Level 3 */
  UMTS_TRF_HANDLING_PRI_MAX
} umts_trf_handling_pri_enum_type;

typedef enum
{
  LTE_QCI_0                     = 0,
  LTE_QCI_1                     = 1,
  LTE_QCI_2                     = 2,
  LTE_QCI_3                     = 3,
  LTE_QCI_4                     = 4,
  LTE_QCI_5                     = 5,
  LTE_QCI_6                     = 6,
  LTE_QCI_7                     = 7,
  LTE_QCI_8                     = 8,
  LTE_QCI_9                     = 9,
  LTE_QCI_MAX
} lte_qci_enum_type;

typedef uint16 cdma_flow_spec_profile_id_type;
typedef uint8  cdma_flow_priority_type;

typedef enum
{
  WLAN_USER_PRI_MIN               = 0,
  WLAN_USER_PRI_BEST_EFFORT       = 0,
  WLAN_USER_PRI_BACKGROUND        = 1,
  WLAN_USER_PRI_RESERVED          = 2,
  WLAN_USER_PRI_EXCELLENT_EFFORT  = 3,
  WLAN_USER_PRI_CONTROLLED_LOAD   = 4,
  WLAN_USER_PRI_VIDEO             = 5,
  WLAN_USER_PRI_VOICE             = 6,
  WLAN_USER_PRI_NETWORK_CONTROL   = 7,
  WLAN_USER_PRI_MAX
} wlan_user_pri_enum_type;

/* If a new bit is added update the ALL mask as well */
typedef enum
{
  IPFLOW_MASK_NONE                      = 0x00000000,
  IPFLOW_MASK_TRF_CLASS                 = 0x00000001,
  IPFLOW_MASK_DATA_RATE                 = 0x00000002,
  IPFLOW_MASK_LATENCY                   = 0x00000004,
  IPFLOW_MASK_LATENCY_VAR               = 0x00000008,
  IPFLOW_MASK_PKT_ERR_RATE              = 0x00000010,
  IPFLOW_MASK_MIN_POLICED_PKT_SIZE      = 0x00000020,
  IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE      = 0x00000040,
  IPFLOW_MASK_UMTS_RES_BER              = 0x00000080,
  IPFLOW_MASK_UMTS_TRF_PRI              = 0x00000100,
  IPFLOW_MASK_CDMA_PROFILE_ID           = 0x00000200,
  IPFLOW_MASK_WLAN_USER_PRI             = 0x00000400,
  IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL = 0x00000800,
  IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL = 0x00001000,
  IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL  = 0x00002000,
  IPFLOW_MASK_NOMINAL_SDU_SIZE          = 0x00004000,
  IPFLOW_MASK_CDMA_FLOW_PRIORITY        = 0x00008000,
  IPFLOW_MASK_UMTS_IM_CN_FLAG           = 0x00010000,
  IPFLOW_MASK_UMTS_SIG_IND              = 0x00020000,
  IPFLOW_MASK_LTE_QCI                   = 0x00040000,
  IPFLOW_MASK_UMTS_SRC_STAT_DESC        = 0x00080000,
  IPFLOW_MASK_ALL                       = 0x000FFFFF
} ipflow_field_mask_enum_type;

typedef uint32 ipflow_field_mask_type;

typedef enum
{
  DATA_RATE_FORMAT_MIN_MAX_TYPE,
  DATA_RATE_FORMAT_MIN_MAX_EX_TYPE,
  DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE
} ipflow_data_rate_format_type;

typedef struct
{
  ipflow_field_mask_type          field_mask;
  ipflow_field_mask_type          err_mask;

  ip_traffic_class_enum_type      trf_class;          /* TC */

  struct
  {
    ipflow_data_rate_format_type  format_type;

    union
    {
      struct
      {
        uint32      max_rate;           /* bps, Rmax */
        uint32      guaranteed_rate;    /* bps, Rg */
      } min_max;

      struct
      {
        uint64      max_rate;           /* bps, Rmax */
        uint64      guaranteed_rate;    /* bps, Rg */
      } min_max_ex;

      struct
      {
        uint32      peak_rate;          /* bps, Rp */
        uint32      token_rate;         /* bps, Rb */
        uint32      size;               /* bytes, B */
      } token_bucket;
    } format;
  } data_rate;

  uint32        latency;            /* msec, L */
  uint32        latency_var;        /* msec, Lvar */

  struct                            /* ratio, Eper = m * 10 ** (-p) */
  {
    uint16      multiplier;         // Multiplication factor, m
    uint16      exponent;           // -ve of Base 10 exponent, p
  } pkt_err_rate;

  uint32      min_policed_pkt_size;   /* bytes, Smin */
  uint32      max_allowed_pkt_size;   /* bytes, Smax */

  struct
  {
    boolean  is_fixed;
    uint32   size;
  } nominal_sdu_size;

  /* 3GPP only QOS parameters */
  struct
  {
    umts_residual_ber_enum_type      res_ber;        /* Erber */
    umts_trf_handling_pri_enum_type  trf_pri;        /* Ptrf */
    boolean                          im_cn_flag;
    boolean                          sig_ind;
    uint32                           src_stat_desc;
  } umts_params;

  struct
  {
    lte_qci_enum_type                     lte_qci; /* QoS Class ID*/
  } lte_params;

  /* 3GPP2 only QOS parameters */
  struct
  {
    cdma_flow_spec_profile_id_type  profile_id;		    /* I */
    cdma_flow_priority_type         flow_priority;
  } cdma_params;

  /* WLAN only QOS parameters */
  struct
  {
    wlan_user_pri_enum_type     user_priority;        /* User Priority */
    uint32                      min_service_interval; /* In micro sec  */
    uint32                      max_service_interval; /* In micro sec  */
    uint32                      inactivity_interval;  /* In micro sec  */
  } wlan_params;
} ip_flow_type;

typedef struct
{
  ip_flow_type    req_flow;            /*  Required flow                   */
  ip_flow_type    min_req_flow;        /*  Minimum required flow           */

  /* To allow auxiliary flow specs */
  uint8           num_aux_flows;       /* # of auxiliary flows in the list */
  ip_flow_type  * aux_flow_list_ptr;   /* Ptr to array of auxiliary flows  */

} ip_flow_spec_type;

typedef enum
{
  QOS_MASK_INVALID                = 0x00,
  QOS_MASK_RX_FLOW                = 0x01,
  QOS_MASK_RX_MIN_FLOW            = 0x02,
  QOS_MASK_RX_AUXILIARY_FLOWS     = 0x04,
  QOS_MASK_TX_FLOW                = 0x08,
  QOS_MASK_TX_MIN_FLOW            = 0x10,
  QOS_MASK_TX_AUXILIARY_FLOWS     = 0x20,
  QOS_MASK_ALL                    = 0x3F,
  QOS_MODIFY_MASK_RX_FLTR_MODIFY  = 0x40,
  QOS_MODIFY_MASK_TX_FLTR_MODIFY  = 0x80,
  QOS_MODIFY_MASK_ALL             = 0xFF
} qos_spec_field_mask_enum_type;

typedef uint16 qos_spec_field_mask_type;

typedef struct
{
  qos_spec_field_mask_type    field_mask;

  struct
  {
    ip_flow_spec_type       flow_template;    /* traffic flow template   */
    ip_filter_spec_type     fltr_template;    /* traffic filter template */
  } rx;

  struct
  {
    ip_flow_spec_type       flow_template;    /* traffic flow template   */
    ip_filter_spec_type     fltr_template;    /* traffic filter template */
  } tx;
} qos_spec_type;

/*---------------------------------------------------------------------------
  QoS structure used to modify QoS on primary
---------------------------------------------------------------------------*/
typedef struct
{
  qos_spec_field_mask_type   field_mask;         /* How is QoS modified?   */
  ip_flow_spec_type          rx_flow_template;   /* QoS in Rx direction    */
  ip_flow_spec_type          tx_flow_template;   /* QoS in Tx direction    */
} primary_qos_spec_type;

/**
  @brief QoS structure used to register for Network Initiated QoS notifications
*/
typedef struct
{
  ip_filter_spec_type  rx_fltr_template;   /**< Rx traffic filter template */
  ip_filter_spec_type  tx_fltr_template;   /**< Tx traffic filter template */
} net_initiated_qos_spec_type;

#endif /* PS_QOS_DEFS_H */

