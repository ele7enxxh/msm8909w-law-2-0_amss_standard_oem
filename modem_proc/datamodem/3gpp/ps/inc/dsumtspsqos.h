#ifndef DSUMTSPSQOS_H
#define DSUMTSPSQOS_H
/*===========================================================================

  U M T S ( W C D M A / G P R S )  P A C K E T - S W I T C H E D   D A T A

                  Q O S   F U N C T I O N   H A N D L E R

                           H E A D E R   F I L E


DESCRIPTION
  This file contains QOS data declarations and function prototypes for UMTS
  (WCDMA/GPRS) Packet-Switched Data which are common for wcdma and gprs. 


Copyright (c) 2002-2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/dsumtspsqos.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/28/11    ttv    Made changes to support max data rates for targets that 
                   support 64QAm and Dual carrier.
09/13/11    ttv    Changes to add src_stat_desc as profile parameter.
12/29/10    ss     Removing REL5 featurization for QOS bitrate parameter as 
                   it is used under REL7 too.
09/06/06    asn    P-PDP QoS Modify support
08/01/06    rr     Fixed Lint errors/warnings.
11/30/05    rc     Removed dsumtsps_filter_validation() prototype.
09/02/05    rc     Added support for Mobile Originated QOS Modification.
11/21/04    rc     Created File.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "comdef.h"
#include "customer.h"


/*---------------------------------------------------------------------------
  QOS attribute values : defined in TS24.008 sec 10.5.6.5
  These values are used locally to encode PDP Context messages and hence 
  defined here.
  Reliability class ( octet 3 : b1-b3 )
---------------------------------------------------------------------------*/
/*lint -e750 */
#define QOS_REL_CLASS_SUBSCRIBED       0x0
#define QOS_REL_CLASS_1                0x1
#define QOS_REL_CLASS_2                0x2
#define QOS_REL_CLASS_3                0x3
#define QOS_REL_CLASS_4                0x4
#define QOS_REL_CLASS_5                0x5
       
/*---------------------------------------------------------------------------
  Delay class ( octet 3 : b4-b6 )
---------------------------------------------------------------------------*/
#define QOS_DELAY_CLASS_SUBSCRIBED     0x0
#define QOS_DELAY_CLASS_1              0x1
#define QOS_DELAY_CLASS_2              0x2
#define QOS_DELAY_CLASS_3              0x3
#define QOS_DELAY_CLASS_4              0x4

/*---------------------------------------------------------------------------
  Precedence class (octet 4 : b1-b3)
---------------------------------------------------------------------------*/
#define QOS_PRECED_CLASS_SUBSCRIBED    0x0
#define QOS_PRECED_CLASS_1             0x1
#define QOS_PRECED_CLASS_2             0x2
#define QOS_PRECED_CLASS_3             0x3

/*---------------------------------------------------------------------------
  Peak throughput (octet4 :b5-b8
---------------------------------------------------------------------------*/
#define QOS_PEAK_TPUT_SUBSCRIBED       0x0
#define QOS_PEAK_TPUT_1KBYTES          0x1
#define QOS_PEAK_TPUT_2KBYTES          0x2
#define QOS_PEAK_TPUT_4KBYTES          0x3
#define QOS_PEAK_TPUT_8KBYTES          0x4
#define QOS_PEAK_TPUT_16KBYTES         0x5
#define QOS_PEAK_TPUT_32BYTES          0x6
#define QOS_PEAK_TPUT_64KBYTES         0x7
#define QOS_PEAK_TPUT_128KBYTES        0x8
#define QOS_PEAK_TPUT_256KBYTES        0x9

/*---------------------------------------------------------------------------
  Maximum ul/dl bit rates  and gurarenteed ul/dl bit rates
  ( octets 7,8,12,13 of QOS : 1byte )
  Note: '0'kbps is encoded as 0xff as per TS24.008 sec 10.5.6.5
---------------------------------------------------------------------------*/
#define QOS_BIT_RATE_SUBSCRIBED       0x0
#define QOS_BIT_RATE_0KBPS            0xff
#define QOS_BIT_RATE_8KBPS            0x08
#define QOS_BIT_RATE_16KBPS           0x10
#define QOS_BIT_RATE_32KBPS           0x20
#define QOS_BIT_RATE_64KBPS           0x40
#define QOS_BIT_RATE_128KBPS          0x48
#define QOS_BIT_RATE_256KBPS          0x58
#define QOS_BIT_RATE_512KBPS          0x78
#define QOS_BIT_RATE_1024KBPS         0x87
#define QOS_BIT_RATE_2048KBPS         0x97
#define QOS_BIT_RATE_8640KBPS         0xFE

/*---------------------------------------------------------------------------
  Mean throughput ( octet 5 : b1-b5 )
---------------------------------------------------------------------------*/
#define QOS_MEAN_TPUT_BEST_EFFORT     0x1f

/*---------------------------------------------------------------------------
  Max SDU ( octet 7 )
---------------------------------------------------------------------------*/
#define QOS_MAX_SDU_SUBSCRIBED        0x0
#define QOS_MAX_SDU_1500_BYTES        0x96
#define QOS_MAX_SDU_1502_BYTES        0x97
#define QOS_MAX_SDU_1510_BYTES        0x98
#define QOS_MAX_SDU_1520_BYTES        0x99
#define QOS_MAX_SDU_RESERVED          0xFF

/*---------------------------------------------------------------------------
  Traffic Handling Prio (octet 11 : b1-b2 )
---------------------------------------------------------------------------*/
#define QOS_TH_PRIO_SUBSCRIBED        0x0

/*---------------------------------------------------------------------------
  Transfer delay (octet 11 : b3-b8 )
---------------------------------------------------------------------------*/
#define QOS_TXFER_DELAY_SUBSCRIBED    0x0

/*---------------------------------------------------------------------------
  Maximum ul/dl bit rates  and gaurenteed ul/dl bit rates
---------------------------------------------------------------------------*/
#define QOS_MAX_BIT_RATE  QOS_MAX_DL_BIT_RATE
#define QOS_MAX_KBIT_RATE  QOS_MAX_DL_KBIT_RATE

#if defined (FEATURE_WCDMA_DC_HSUPA) || defined (FEATURE_WCDMA_16_QAM)
#define QOS_MAX_UL_KBIT_RATE 11520
#elif defined (FEATURE_WCDMA_HSUPA_2MS_TTI)
#define QOS_MAX_UL_KBIT_RATE 5760
#elif defined (FEATURE_WCDMA_HSUPA)
#define QOS_MAX_UL_KBIT_RATE 2048
#elif defined (FEATURE_WCDMA)
#define QOS_MAX_UL_KBIT_RATE 384
#elif defined (FEATURE_TDSCDMA)
#define QOS_MAX_UL_KBIT_RATE 512
#else
#define QOS_MAX_UL_KBIT_RATE 59   /* EDGE class 8, 10 and MCS-9*/
#endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */

#ifdef FEATURE_WCDMA_3C_HSDPA
#define QOS_MAX_DL_KBIT_RATE 63000
#elif defined (FEATURE_WCDMA_DC_HSDPA)
#define QOS_MAX_DL_KBIT_RATE 42200
#elif defined (FEATURE_WCDMA_COMBINED_MIMO_AND_64QAM) 
#define QOS_MAX_DL_KBIT_RATE 42200
#elif defined (FEATURE_WCDMA_MIMO)
#define QOS_MAX_DL_KBIT_RATE 28000
#elif defined (FEATURE_WCDMA_64QAM)
#define QOS_MAX_DL_KBIT_RATE 21600
#elif defined FEATURE_WCDMA_REL7
#define QOS_MAX_DL_KBIT_RATE 14000
#elif defined (FEATURE_HSDPA)
#define QOS_MAX_DL_KBIT_RATE 7168
#elif defined (FEATURE_WCDMA)
#define QOS_MAX_DL_KBIT_RATE 384
#elif defined (FEATURE_TDSCDMA)
#define QOS_MAX_DL_KBIT_RATE 2048
#else
#define QOS_MAX_DL_KBIT_RATE 236 /* EDGE Max DL with 4+1 TS config*/
#endif /* FEATURE_HSDPA */

#define QOS_MAX_UL_BIT_RATE (QOS_MAX_UL_KBIT_RATE * 1000)
#define QOS_MAX_DL_BIT_RATE (QOS_MAX_DL_KBIT_RATE * 1000)

/* 3GPP QoS parameters as per Spec 24.008 */
#define QOS_PARAM_MAX_SDU_SIZE   1520

#define QOS_PARAM_MAX_TRANS_DELAY   4000

#define QOS_PARAM_MAX_THANDLE_PRIORITY   3

#define QOS_PARAM_MAX_RELIABILITY_CLASS   7
#define QOS_PARAM_MAX_DELAY_CLASS         7
#define QOS_PARAM_MAX_PRECEDENCE_CLASS    7

#define QOS_PARAM_MAX_PEAK_THROUGHPUT_CLASS   9
#define QOS_PARAM_MAX_MEAN_THROUGHPUT_CLASS   31

#if defined (FEATURE_DATA_WCDMA_PS) ||  defined (FEATURE_GSM_GPRS)
#include "ps_qos_defs.h"
#include "ps_flow_ioctl.h"
#include "ps_flow.h"
#include "dsumtspdpreg.h"
#include "cm.h"

/*---------------------------------------------------------------------------
  Enumeration for QoS priority
---------------------------------------------------------------------------*/
typedef enum
{
  DS_UMTS_QOS_PRIORITY_NONE   = 0x0,      /* Priority None                 */
  DS_UMTS_QOS_PRIORITY_ONE    = 0x1,      /* Priority One                  */
  DS_UMTS_QOS_PRIORITY_TWO    = 0x2,      /* Priority Two                  */
  DS_UMTS_QOS_PRIORITY_THREE  = 0x3,      /* Priority Three                */
  DS_UMTS_QOS_PRIORITY_FOUR   = 0x4,      /* Priority FOUR                 */
  DS_UMTS_QOS_PRIORITY_FIVE   = 0x5,      /* Priority FIVE                 */
  DS_UMTS_QOS_PRIORITY_SIX    = 0x6,      /* Priority SIX                  */    
  DS_UMTS_QOS_PRIORITY_MAX    = 0xFFFF    /* force max to 0xffff so that   */
                                          /* enum is defined as a word     */
} ds_umts_qos_priority_enum_type;

/*---------------------------------------------------------------------------
  The enum type for TFT operation code  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_UMTS_TFT_OPCODE_SPARE      = 0x0,  /* Spare                           */
  DS_UMTS_TFT_OPCODE_CREATE     = 0x1,  /* Create new TFT                  */
  DS_UMTS_TFT_OPCODE_DEL_TFT    = 0x2,  /* Delete existing TFT             */
  DS_UMTS_TFT_OPCODE_ADD        = 0x3,  /* Add pkt ftr to exiting TFT      */
  DS_UMTS_TFT_OPCODE_REPLACE    = 0x4,  /* Replace pkt ftr in existing TFT */
  DS_UMTS_TFT_OPCODE_DEL_FILTER = 0x5,  /* Delete pkt ftr in existing TFT  */
  DS_UMTS_TFT_OPCODE_NO_TFT     = 0x6,   /* No TFT operation */
  DS_UMTS_TFT_OPCODE_RESERVED   = 0xFF  /* Reserved                        */
} ds_umts_tft_opcode_enum_type;

/*---------------------------------------------------------------------------
  The enum type for TFT packet filter component type identifier  
  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_UMTS_TFT_IPV4_ADDR        = 0x10,  /* IP V4 Address type              */
  DS_UMTS_TFT_IPV6_ADDR        = 0x20,  /* IP V6 Address type              */
  DS_UMTS_TFT_NEXT_HDR_TYPE    = 0x30,  /* Protocol identifier             */
  DS_UMTS_TFT_SINGLE_DEST_PORT = 0x40,  /* Single destination port         */
  DS_UMTS_TFT_DEST_PORT_RANGE  = 0x41,  /* Destination port range          */
  DS_UMTS_TFT_SINGLE_SRC_PORT  = 0x50,  /* Single Source port              */
  DS_UMTS_TFT_SRC_PORT_RANGE   = 0x51,  /* Source port range               */
  DS_UMTS_TFT_SEC_PARAM_IDX    = 0x60,  /* Security parameter index        */
  DS_UMTS_TFT_TYPE_OF_SRVC     = 0x70,  /* Type of service                 */
#ifdef FEATURE_DATA_PS_IPV6
  DS_UMTS_TFT_IP6_TRAFFIC_CLASS = DS_UMTS_TFT_TYPE_OF_SRVC, 
                                        /* IPV6 traffic class              */
#endif /*FEATURE_DATA_PS_IPV6*/
  DS_UMTS_TFT_FLOW_LABEL       = 0x80   /* Flow Label                      */
} ds_umts_tft_comp_id_enum_type;

/*===========================================================================
MACRO DSUMTSPS_QOSI_IS_QOS_BLOB_VALID

DESCRIPTION   
  Given the UMTS QoS BLOB, returns validilty

PARAMETERS
  ds_umts_umts_qos_params_type *qos_blob_ptr

DEPENDENCIES  
  None.

RETURN VALUE  
  See Descritpion.
  
LIMITATIONS

===========================================================================*/
#define DSUMTSPS_QOSI_IS_QOS_BLOB_VALID( qos_blob_ptr ) \
                  ( (qos_blob_ptr) == NULL \
                    ?FALSE \
                    :(qos_blob_ptr)->valid_flg \
                   )
          
/*===========================================================================
MACRO DSUMTSPS_QOS_GET_CM_PROV_NETW_QOS_PTR

DESCRIPTION   
  Given the CM Mode Info Ptr, returns pointer to QoS information

PARAMETERS
  cm_call_mode_info_s_type *cm_mode_info_ptr

DEPENDENCIES  
  None.

RETURN VALUE  
  Pointer to CM provided Netw QoS of type qos_T
  
LIMITATIONS

===========================================================================*/

#define DSUMTSPS_QOS_GET_CM_PROV_NETW_QOS_PTR( cm_mode_info_ptr ) \
                  ( ( cm_mode_info_ptr == NULL ) \
                      ?NULL \
                      :&(cm_mode_info_ptr->info.gw_ps_call.qos) \
                   )

/*===========================================================================
MACRO DSUMTPS_QOS_IS_CM_PROV_NETW_QOS_VALID

DESCRIPTION   
  Checks validilty of Cm provided Netw Allocated QoS 

PARAMETERS
  qos_T *netw_qos_ptr;

DEPENDENCIES  
  None.

RETURN VALUE  
  boolean
  
LIMITATIONS

===========================================================================*/
#define DSUMTPS_QOS_IS_CM_PROV_NETW_QOS_VALID( netw_qos_ptr ) \
                  ( ( (netw_qos_ptr) == NULL ) \
                    ?FALSE \
                    :(netw_qos_ptr)->valid \
                   )



/*===========================================================================
FUNCTION DSUMTSPS_QOS_FLOW_VALIDATION

DESCRIPTION
   Call back funtion used by PS IFACE to validate QOS parameters.
   Optional function to perform extended validation on QOS parameters, 
   generic validaiton still done by PS IFACE
    
PARAMETERS
  *flow_ptr      - pointer to the QOS parameters
  rx_or_tx       - Boolean to indicate if it is a RX spec or TX spec
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_qos_flow_validation
(  
  ip_flow_type                   *flow_ptr,
  qos_spec_field_mask_enum_type   rx_or_tx
);

/*==========================================================================
FUNCTION DSUMTSPS_CONV_NET_ALLOCATED_QOS_TO_APP_QOS

DESCRIPTION
  Given a set of network allocated QoS, converts them into application 
  specific QoS. Applies only to Secondary context.
    
PARAMETERS
  *mode_info_ptr    - Ptr to the network allocated QOS.
  *app_qos          - Ptr to App QOS in Call table
    
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS 
  
  
===========================================================================*/
void dsumtsps_conv_net_allocated_qos_to_app_qos
(
  const cm_call_mode_info_s_type *mode_info_ptr,
  uint32                          call_instance,
  qos_spec_type                   *qos_ptr,
  boolean                         accept_flow 
);


/*===========================================================================
FUNCTION DSUMTSPS_QOS_MODIFY_FLOW_PARAM_PROCESS

DESCRIPTION
  Process the application supplied QoS. Convert them into UMTS technology
  specific parameters. Sort the QoS to find the minimum and requested sets

PARAMETERS 
  *app_qos        - Ptr to the application specified QoS parameters
  *umts_qos       - Ptr to the UMTS specific QoS parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
void dsumtsps_pri_qos_modify_flow_param_process
(
  const ps_flow_type                           *flow_ptr,
  const   primary_qos_spec_type                *app_qos_ptr,
  ds_umts_umts_qos_params_type                 *req_qos,
  ds_umts_umts_qos_params_type                 *min_qos

);

/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_MODIFY_FLOW_PARAM_PROCESS

DESCRIPTION
  Process the application supplied QoS. Convert them into UMTS technology
  specific parameters. Sort the QoS to find the minimum and requested sets

PARAMETERS 
  *app_qos        - Ptr to the application specified QoS parameters
  *umts_qos       - Ptr to the UMTS specific QoS parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
void dsumtsps_sec_qos_modify_flow_param_process
(
  const ps_flow_type           *flow_ptr,
  const qos_spec_type          *app_qos,
  ds_umts_umts_qos_params_type *req_qos,
  ds_umts_umts_qos_params_type *min_qos

);

/*==========================================================================
FUNCTION DSUMTSPS_CONV_NETW_QOS_TO_UMTS_QOS

DESCRIPTION
  Given a set of network allocated QoS, converts them into UMTS
  specific QoS. 

PARAMETERS (validated inside this routine)
  (->) *netw_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *umts_qos_ptr    - Ptr to App QOS in Call table
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS

===========================================================================*/

boolean dsumtsps_conv_netw_qos_to_umts_qos
(
  const qos_T                   *netw_qos_ptr,
  ds_umts_umts_qos_params_type  *umts_qos_ptr
);

/*==========================================================================
FUNCTION DSUMTSPS_CONV_UMTS_QOS_TO_APP_QOS

DESCRIPTION
  Given a set of UMTS QoS, converts them into application
  specific QoS. 

PARAMETERS (validated inside this routine)
  (->) *umts_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *app_qos_ptr     - Ptr to App QOS in Call table

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
 -1 on FAILURE

SIDE EFFECTS

===========================================================================*/

boolean dsumtsps_conv_umts_qos_to_app_qos
(
  const ds_umts_umts_qos_params_type                    *umts_qos_ptr,
  ps_flow_ioctl_primary_qos_get_granted_flow_spec_type  *app_qos_ptr
);

/*==========================================================================
FUNCTION DSUMTSPS_CONV_GPRS_QOS_TO_APP_QOS

DESCRIPTION
  Given a set of UMTS QoS, converts them into application
  specific QoS. 

PARAMETERS (validated inside this routine)
  (->) *umts_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *app_qos_ptr     - Ptr to App QOS in Call table

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
 -1 on FAILURE

SIDE EFFECTS


===========================================================================*/

boolean dsumtsps_conv_gprs_qos_to_app_qos
(
  const ds_umts_gprs_qos_params_type                    *gprs_qos_ptr,
  ps_flow_ioctl_primary_qos_get_granted_flow_spec_type  *app_qos_ptr
);


/*==========================================================================
FUNCTION DSUMTSPS_CONV_NETW_QOS_TO_GPRS_QOS

DESCRIPTION
  Given a set of network encoded QoS, decode GPRS QoS paramters

PARAMETERS
  (->) *netw_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *gprs_qos_ptr    - Ptr to GPRS QoS Blob in DS Call table

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
 -1 on FAILURE

SIDE EFFECTS


===========================================================================*/

boolean dsumtsps_conv_netw_qos_to_gprs_qos
(
  const qos_T                  *netw_qos_ptr,
  ds_umts_gprs_qos_params_type *gprs_qos_ptr
);

/*==========================================================================
FUNCTION DSUMTSPS_CONV_NETW_DATA_RATE_TO_UMTS_DATA_RATE

DESCRIPTION
  Converts Netw encoded data rate to UMTS data rate 

PARAMETERS
  (->) byte netw_encoded_data_rate;

DEPENDENCIES
  None.

RETURN VALUE
  uint32

LIMITATIONS
  Can decode max data rate limit of 8640 kbps
  
SIDE EFFECTS

===========================================================================*/
uint32 dsumtsps_conv_netw_data_rate_to_umts_data_rate
(
    byte netw_encoded_data_rate
);

/*==========================================================================
FUNCTION DSUMTSPSBEARERCNTXT_CONVERT_DATA_RATE_TO_R7_EXT_DECODE_VAL

DESCRIPTION
  Converts Netw encoded data rate to UMTS data rate if extended bit is set

PARAMETERS
  uint8 encoded_data_rate;

DEPENDENCIES
  None.

RETURN VALUE
  uint32 decoded umts_data_rate

LIMITATIONS
  none
===========================================================================*/
uint32 dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_decode_val
(
  uint8  encoded_data_rate
);


/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_TRAFFIC_CLASS

DESCRIPTION
  Convert the Traffic Class parameter of Application QoS into UMTS specific.  
    
PARAMETERS
  trf_class        - Application specified traffic class 
  field_mask       - Application set field mask
  *umts_qos        - Pointer to the UMTS specific qos (traffic class)
  
DEPENDENCIES
  None.
  
RETURN VALUE
  returns an int. 0 if there are no errors.

SIDE EFFECTS 
  
  
===========================================================================*/
void dsumtsps_process_traffic_class 
(
  ip_traffic_class_enum_type          trf_class, 
  ipflow_field_mask_type              field_mask,
  ds_umts_umts_qos_params_type        *umts_qos
);

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_DATA_RATE

DESCRIPTION
  Convert the Data Rate parameter of Application given QoS set into UMTS 
  specific. UL/DL Max Data Rate, UL/DL Guartanteed Data Rate are calculated.
    
PARAMETERS
  *ip_flow         - Pointer to the application specified data rate
  field_mask       - Application set field mask
  latency          - Application specified latency
  *max_bitrate     - Pointer to the max bit rate of the flow spec
  *gtd_bitrate     - Pointer to the gtd bit rate of the flow spec
  
DEPENDENCIES
  None.
  
RETURN VALUE
  If the specified value exceeds the theoritical maximum data rate, an \
  error is sent to the application.

SIDE EFFECTS 
  
  
===========================================================================*/
void dsumtsps_process_data_rate
(
  const ip_flow_type                  *ip_flow,
  uint32                              latency,
  ipflow_field_mask_type              field_mask,
  uint32                              *max_bitrate,
  uint32                              *gtd_bitrate
);

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_PKT_ERR_RATE

DESCRIPTION
  Convert the Packet Error Rate parameter of Application QoS into SDU Error. 
    
PARAMETERS
  *ip_flow         - Apointer to application specified packet error rate
  field_mask       - Application set field mask
  *umts_qos        - Pointer to the UMTS specific qos (SDU ERR enum)
  
DEPENDENCIES
  None.
  
RETURN VALUE
  returns an int. 0 if there are no errors.

SIDE EFFECTS 
  
  
===========================================================================*/
void dsumtsps_process_pkt_err_rate
(
  const ip_flow_type                  *ip_flow, 
  ipflow_field_mask_type              field_mask,
  ds_umts_umts_qos_params_type        *umts_qos
);

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_RESIDUAL_BER

DESCRIPTION
  Convert the Residual Bit Error Rate of Application into UMTS specific.

PARAMETERS
  *ip_flow         - Pointer to application specified RES BER
  field_mask       - Application set field mask
  *umts_qos        - Pointer to the UMTS specific qos (RES BER enum)

DEPENDENCIES
  None.

RETURN VALUE
  returns an int. 0 if there are no errors.

SIDE EFFECTS


===========================================================================*/
void dsumtsps_process_residual_ber
(
  const ip_flow_type                  *ip_flow,
  ipflow_field_mask_type              field_mask,
  ds_umts_umts_qos_params_type        *umts_qos
);

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_TRF_HDL_PRIO

DESCRIPTION
  Convert the Traffic Handling Priority parameter of application QoS into
  traffic handling priority of UMTS.

PARAMETERS
  trf_pri        - Application specified traffic handling priority
  field_mask     - Application set field mask
  *umts_qos      - Pointer to the UMTS specific qos(traffic handling priority)

DEPENDENCIES
  None.

RETURN VALUE
  returns an int. 0 if there are no errors.

SIDE EFFECTS


===========================================================================*/
void dsumtsps_process_trf_hdl_prio
(
  const ip_flow_type                  *ip_flow,
  ipflow_field_mask_type              field_mask,
  ds_umts_umts_qos_params_type        *umts_qos
);

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_SIG_IND_PRIO

DESCRIPTION
  Convert the Signalling Indication parameter of application QoS into
  signalling indication of UMTS.

  Sig Ind Flag should only be set if the traffic class is interactive
  and the traffic handling priority is 1 (This is as per spec 
  23.107 Sec 6.4.3.1).

  Signalling Indication (Yes/No)
  Definition: Indicates the signalling nature of the submitted SDUs.
  This attribute is additional to the other QoS attributes and does
  not over-ride them. This attribute is only defined for the
  interactive traffic class. If signalling indication is set to "Yes",
  the UE should set the traffic handling priority to "1".


PARAMETERS
  ip_flow        - Application specified ip flow
  field_mask     - Application set field mask
  *umts_qos      - Pointer to the UMTS specific qos(traffic handling priority)

DEPENDENCIES
  None.

RETURN VALUE
  returns an int. 0 if there are no errors.

SIDE EFFECTS


===========================================================================*/
void dsumtsps_process_sig_ind_prio
(
  const ip_flow_type                  *ip_flow,
  ipflow_field_mask_type              field_mask,
  ds_umts_umts_qos_params_type        *umts_qos
); 

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_SRC_STAT_DESC

DESCRIPTION
  Convert the Source Static Descriptor parameter of application QoS into
  Source Static Descriptor of UMTS.

  Source Static Descriptor should only be set if the traffic class is 
  conventional or streaming (This is as per spec 23.107 Sec 6.4.3.1).

PARAMETERS
  ip_flow        - Application specified ip flow
  field_mask     - Application set field mask
  *umts_qos      - Pointer to the UMTS specific qos(traffic handling priority)
  traffic_class  - Traffic class value

DEPENDENCIES
  None.

RETURN VALUE
  returns an int. 0 if there are no errors.

SIDE EFFECTS


===========================================================================*/
void dsumtsps_process_src_stat_desc
(
  const ip_flow_type                  *ip_flow,
  ipflow_field_mask_type              field_mask,
  uint32                              *src_stat_desc,
  ds_umts_qos_tclass_enum_type        traffic_class
);

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS  */

#endif /* FEATURE_DATA_3GPP */

#endif /* DSUMTSPSQOS_H */

