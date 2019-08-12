/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  U M T S ( W C D M A / G P R S )  P A C K E T - S W I T C H E D   D A T A

                  Q O S   F U N C T I O N   H A N D L E R 

  
GENERAL DESCRIPTION

  This file contains QOS data and function definitions for UMTS (wcdma/gprs)
  Packet-Switched DATA for PDP Type IP.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/dsumtspsqos.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/13/11    ttv    Changes to add src_stat_desc as profile parameter.
07/27/11    ttv    Added changes required for source statistics descriptor
                   addition.
05/05/11    sa     Fixed dsumtsps_conv_umts_qos_to_app_qos() for sig_ind.
04/12/11    dvk    Fixed Klocwork errors 
07/29/10    asn    correct handling of QoS objects 
06/09/10    asn    set sig_ind to app requested value during S-PDP modification
03/13/09    ua     Correcting the enum mis-match between PS and MH. 
05/29/09    ars    Fixed lint warnings
04/08/08    vrk    Lint Fixes
01/02/08    asn    Fix bug in conversion of max SDU size for storage
05/01/07    aw     Added support for Signalling Indication Flag.
02/11/07    vk     Fixed RVCT 2.1 compiler warnings.
09/21/06    rr     Fixed dsumtsps_conv_net_allocated_qos_to_app_qos to
                   populate correct traffic class. 
09/06/06    asn    P-PDP QoS Modify support
08/01/06    rr     Fixed Lint errors/warnings.
03/23/06    rr     Changed PS_FLOW_SET_TX_GRANTED_FLOW and PS_FLOW_SET_RX_
                   GRANTED_FLOW to PS_FLOW_SET_CURRENT_FLOW.
02/27/06    rr     Changed PS_FLOW_SET_CURRENT_FLOW to PS_FLOW_SET_TX_GRANTED
                   _FLOW and PS_FLOW_SET_RX_GRANTED_FLOW
01/11/05    rr     Changed PS_FLOW_GET_RX_CURRENT_FLOW/PS_FLOW_GET_TX_
                   CURRENT_FLOW to PS_FLOW_GET_RX_REQUESTED_FLOW/PS_FLOW_GET_
                   TX_REQUESTED_FLOW in dsumtsps_sec_qos_modify_flow_param_
                   process
11/30/05    rc     Removed dsumtsps_filter_validation() function. Added
                   validation for maximum and guaranteed bit rates in
                   dsumtsps_flow_validation().
09/02/05    rc     Added support for Mobile Initiated QOS Modification.
06/15/05    rc     Fixed bug where UL min qos parameters were being assigned
                   to UL requested QOS.
06/14/05    rc     Removed addition of 1 to the max and guaranteed bit rate
                   if it is not a multiple of 1000.
05/12/05    rr     Added IPV6 code.
05/11/05    rr     Fixed Lint errors/warnings.
04/19/05    rc     Using PS_FLOW_SET_CURRENT_FLOW() instead of
                   PS_PHYS_LINK_SET_CURRENT_FLOW().
11/21/04    rc     Added comdef.h and customer.h
11/21/04    rc     Created File.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined (FEATURE_DATA_WCDMA_PS) ||  defined (FEATURE_GSM_GPRS) 
#include "dsumtspsqos.h"
#include "msg.h"
#include "ds_3gppi_utils.h"

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Traffic class values are offset by 1 w.r.t 24.008 and 
    ds_umts_qos_tclass_enum_type. Add by 1 and send it to network. 
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( field_mask & (int)IPFLOW_MASK_TRF_CLASS )
  {
    umts_qos->traffic_class = (ds_umts_qos_tclass_enum_type)((uint32)trf_class + 1);
  }
  else
  {
    /* Not specified, default to SUBSCRIBED */
    umts_qos->traffic_class = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
  }

  return;
} /* dsumtsps_process_traffic_class */


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
)
{
  uint32 temp_bitrate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !(field_mask & (int)IPFLOW_MASK_DATA_RATE) )
  {
    /* Not specified, default to SUBSCRIBED */
    *max_bitrate = 0;
    *gtd_bitrate = 0;
    return;
  }

  if ( ip_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE )
  {
    /*----------------------------------------------------------------------- 
       Option 1 was specified by the application (UMTS specific values)
       take the minimum value between the what was specified and the
       theoritical maximum of UMTS RT
    -----------------------------------------------------------------------*/
    if (ip_flow->data_rate.format.min_max.max_rate > QOS_MAX_BIT_RATE )
    {
      *max_bitrate = QOS_MAX_KBIT_RATE;
    }
    else
    {
      temp_bitrate=0;
      temp_bitrate = (ip_flow->data_rate.format.min_max.max_rate / 1000 );

      *max_bitrate = temp_bitrate;
    }

    if (ip_flow->data_rate.format.min_max.guaranteed_rate > QOS_MAX_BIT_RATE )
    {
      *gtd_bitrate = QOS_MAX_KBIT_RATE;
    }
    else
    {
      temp_bitrate=0;
      temp_bitrate = (ip_flow->data_rate.format.min_max.guaranteed_rate/1000 );

      *gtd_bitrate = temp_bitrate;
    }
  }
  else if (ip_flow->data_rate.format_type == DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE)
  {
    /*----------------------------------------------------------------------- 
      Option 2 was specified by the application (1X specific values)
      use the following equation :                                 
      If L>0 MIN( MAX(B/(L-K), Rp), C )                            
      else   MIN(Rp, C)                                            
      Where L is latency, B is Token Bucket Size, K is latency     
      factor constant, Rp is peak rate, C is maximum theoritcal    
      channel rate supported                                        
    -----------------------------------------------------------------------*/
    if ( ( field_mask & (int)IPFLOW_MASK_LATENCY_VAR ) && ( latency > 0 ) )
    {
      uint32 rt_max;
      uint32 latency_factor_const = 0;
      temp_bitrate = 0;

      rt_max = (uint32)((ip_flow->data_rate.format.token_bucket.size)\
                         /(latency - latency_factor_const));

       
      temp_bitrate = MIN( MAX ( rt_max, 
                                ip_flow->data_rate.format.token_bucket.\
                                peak_rate), 
                                QOS_MAX_BIT_RATE );

      *max_bitrate = temp_bitrate/1000;      
    }
    else
    {
      temp_bitrate =0;

      temp_bitrate = MIN ( ip_flow->data_rate.format.token_bucket.peak_rate, 
                              QOS_MAX_BIT_RATE );

      *max_bitrate = temp_bitrate/1000;      
    }

    temp_bitrate = 0;

    temp_bitrate =MIN(ip_flow->data_rate.format.token_bucket.token_rate,
                             QOS_MAX_BIT_RATE );

    *gtd_bitrate = temp_bitrate/1000;      
  }
  else
  {
    /* Subscribed */
    *max_bitrate = 0;
    *gtd_bitrate = 0;
  }

  /*------------------------------------------------------------
    24.008: Table 10.5.156
    
    The Guaranteed bit rate for uplink/downlink value is ignored if the 
    Traffic Class is Interactive class or Background class, or
    Maximum bit rate for uplink/downlink is set to 0 kbps.
  --------------------------------------------------------------*/
  if ( ( ip_flow->trf_class == IP_TRF_CLASS_INTERACTIVE ) ||
       ( ip_flow->trf_class == IP_TRF_CLASS_BACKGROUND ) ||
       ( *max_bitrate == 0 ))
  {
    *gtd_bitrate = 0;
  }

  return;
} /* dsumtsps_process_data_rate */

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_umts_qos_sdu_error_enum_type sdu_array_for_1E[6] =
  {  DS_UMTS_QOS_SDU_ERR_1E1,     /* 1E-1 */
     DS_UMTS_QOS_SDU_ERR_1E2,     /* 1E-2 */
     DS_UMTS_QOS_SDU_ERR_1E3,     /* 1E-3 */
     DS_UMTS_QOS_SDU_ERR_1E4,     /* 1E-4 */
     DS_UMTS_QOS_SDU_ERR_1E5,     /* 1E-5 */
     DS_UMTS_QOS_SDU_ERR_1E6,     /* 1E-6 */
  };

  ds_umts_qos_sdu_error_enum_type temp_sdu_err = DS_UMTS_QOS_SDU_ERR_RESERVED;

  /*------------------------------------------------------------------------- 
     Pkt Err rate (E) we get from application is governed by the following
     equation : E = M*10**(-P). We are given the M and P values(from the app)
     Need to convert that into ds_umts_qos_sdu_error_enum_type format.
     If we have no match, mode specific handlers need to use an alternate pkt
     err rate that is the nearest supported value. In making that decision,
     we are going to prefer lower limit
     Algorithm: Choose the a nearest P value then choose nearest M value.
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------- 
     Implementation description: If the exponent (P) is 3 and multiplier is 7
     we have a match to 7E3. Else based on the exponent, choose one of the
     1EP value defined in UMTS. For anything else choose 1E6.
   ------------------------------------------------------------------------*/
  if ( (!(field_mask & (int)IPFLOW_MASK_PKT_ERR_RATE) ) )
  {
    umts_qos->sdu_err = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
    return;
  }


  if ( ( ip_flow->pkt_err_rate.exponent == 0 ) ||
       ( ip_flow->pkt_err_rate.multiplier == 0 ) )
  {
    temp_sdu_err = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
  }
  else if ( ( ip_flow->pkt_err_rate.exponent >= 1 ) ||
            ( ip_flow->pkt_err_rate.exponent <= 6 ) )
  { 
    if ( ( ip_flow->pkt_err_rate.multiplier == 7 ) &&
         ( ip_flow->pkt_err_rate.exponent == 3 ) )
    {
      temp_sdu_err = DS_UMTS_QOS_SDU_ERR_7E3;
    }
    else
    {
      if ( ip_flow->pkt_err_rate.exponent < 6 ) /*6 is the size of sdu_array */
      {
        temp_sdu_err = sdu_array_for_1E[ip_flow->pkt_err_rate.exponent-1];
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Invalid exponent value recived. setting SDU ERR to subscribe");
        temp_sdu_err = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
      }
    }
  }
  else
  {
    temp_sdu_err = DS_UMTS_QOS_SDU_ERR_1E6;
  }

  if ( temp_sdu_err == DS_UMTS_QOS_SDU_ERR_RESERVED )
  {
    DS_3GPP_MSG1_ERROR("Invalid SDU ERR parameter : %d",temp_sdu_err);
    umts_qos->sdu_err = DS_UMTS_QOS_SDU_ERR_7E3;
  }
  else
  {
    umts_qos->sdu_err = temp_sdu_err;
  }

  return;
} /* dsumtsps_process_pkt_err_rate */

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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    One on one mapping between the application residual ber and UMTS  
    residual ber, just copy the field 

    values are offset by 1 w.r.t 24.008 and 
    ds_umts_qos_res_ber_enum_type. Add by 1 and send it to network. 
  -------------------------------------------------------------------------*/
  if ( field_mask & (int)IPFLOW_MASK_UMTS_RES_BER )
  {
    umts_qos->res_biterr = (ds_umts_qos_res_ber_enum_type)
                                    ( (uint8) ip_flow->umts_params.res_ber + 1 );
  }
  else
  {
    /* Not specified, default to SUBSCRIBED */
    umts_qos->res_biterr = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
  }
  return;
} /* dsumtsps_process_residual_ber */

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
)
{
 
  ASSERT ( ip_flow != NULL );
  ASSERT ( umts_qos != NULL );
  
  /*------------------------------------------------------------------------- 
    Table 10.5.156/3GPP TS 24.008: Quality of service information element

    The Traffic handling priority value is ignored if the Traffic Class 
    is Conversational class, Streaming class or Background class.
    
    One on one mapping between the application traffic handling priority 
    and UMTS taffic handling priority, just copy the field               
  -------------------------------------------------------------------------*/
  if ( (field_mask & (int)IPFLOW_MASK_UMTS_TRF_PRI ) &&
       ( umts_qos->traffic_class == DS_UMTS_QOS_TCLASS_INTR ))
  {
    umts_qos->thandle_prio = (int)ip_flow->umts_params.trf_pri + 1;
  }
  else
  {
    /* Not specified, default to SUBSCRIBED */
    umts_qos->thandle_prio = 0;
  }

  return;
} /* dsumtsps_process_trf_hdl_prio */


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
)
{
  ASSERT ( umts_qos != NULL);
  ASSERT ( ip_flow  != NULL);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    One on one mapping between the application signalling indication bit 
    and UMTS signalling indication bit. Copy the field if traffic class is
    interactive and Traffic Handling Priority is 1. 
  -------------------------------------------------------------------------*/
  if ( (field_mask & (int)IPFLOW_MASK_UMTS_SIG_IND ) &&
     (umts_qos->traffic_class == DS_UMTS_QOS_TCLASS_INTR) &&
     (umts_qos->thandle_prio == 1))
  {
    umts_qos->sig_ind = (int)ip_flow->umts_params.sig_ind;
  }
  else
  {
    /* Not specified, default to SUBSCRIBED */
    umts_qos->sig_ind = 0;
  }
  DS_3GPP_MSG1_HIGH( "sig_ind: %1x", umts_qos->sig_ind);
  return;
} /* dsumtsps_process_sig_ind_prio */

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
)
{
  ASSERT ( ip_flow  != NULL);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    One on one mapping between the application signalling indication bit 
    and UMTS signalling indication bit. Copy the field if traffic class is
    interactive and Traffic Handling Priority is 1. 
  -------------------------------------------------------------------------*/
  if ( (field_mask & (int)IPFLOW_MASK_UMTS_SRC_STAT_DESC ) &&
       ( (traffic_class == DS_UMTS_QOS_TCLASS_CONV) ||
         (traffic_class == DS_UMTS_QOS_TCLASS_STRM) ) )
  {
    *src_stat_desc = (int)ip_flow->umts_params.src_stat_desc;
  }
  else
  {
    /* Not specified, default to SUBSCRIBED */
    *src_stat_desc = 0;
  }
  DATA_MSG1_HIGH("src_stat_desc: %d", *src_stat_desc );
  return;
} /* dsumtsps_process_src_stat_desc */



/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_MODIFY_FLOW_PARAM_PROCESS

DESCRIPTION
  Process the application supplied QoS for modification. 
  Convert them into UMTS technology specific parameters. Sort the QoS to 
  find the minimum and requested sets
  In UMTS, except data rates and guaranteed data rates, other parameters
  are common across UL and DL. The table below deascribes how we process
  the parameters given by the application

  Required UL Data Rate            : Taken from TX Required set
  Required UL Guaranteed Data Rate : Taken from TX Required set
  Minimum UL Data Rate             : Taken from TX Minimum set
  Minimum UL Guaranteed Data Rate  : Taken from TX Minimum set

  
  Required DL Data Rate            : Taken from RX Required set
  Required DL Guaranteed Data Rate : Taken from RX Required set
  Minimum DL Data Rate             : Taken from RX Minimum set
  Minimum DL Guaranteed Data Rate  : Taken from RX Minimum set
  
  Required Traffic Class (common)             :Taken from RX or Tx Required set
  Required Traffic Handling Priority (common) :Taken from RX or Tx Required set
  Required RES BER (common)                   :Taken from RX or Tx Required set
  Required SDU Err Ratio (common)             :Taken from RX or Tx Required set

  In this function, process each QOS field individually. If the field is
  not set, set it to default (subscribed). The out come of the process
  is two sets of QOS.

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
  const ps_flow_type                 *flow_ptr,
  const qos_spec_type                *app_qos,
  ds_umts_umts_qos_params_type       *req_qos,
  ds_umts_umts_qos_params_type       *min_qos
)
{
  uint32 temp_max_bitrate, temp_gtd_bitrate;
  ip_flow_type *tx_flow_params, *rx_flow_params;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e641 suppressing error 641*/
  /*-------------------------------------------------------------------------
    Initialize the Required and Minimum sets to defaults
  -------------------------------------------------------------------------*/
  req_qos->traffic_class        = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
  req_qos->dlvry_order          = DS_UMTS_QOS_DORDER_SUBSCRIBE;
  req_qos->max_sdu_size         = 0x0;
  req_qos->sdu_err              = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
  req_qos->res_biterr           = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
  req_qos->dlvr_err_sdu         = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;
  req_qos->trans_delay          = 0x0;

  min_qos->traffic_class        = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
  min_qos->dlvry_order          = DS_UMTS_QOS_DORDER_SUBSCRIBE;
  min_qos->max_sdu_size         = 0x0;
  min_qos->sdu_err              = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
  min_qos->res_biterr           = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
  min_qos->dlvr_err_sdu         = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;
  min_qos->trans_delay          = 0x0;
  min_qos->thandle_prio         = 0x0;
  min_qos->sig_ind              = FALSE;

  /*-------------------------------------------------------------------------
    If either the requested Rx or Tx flow has been specified, set the valid 
    flag to TRUE.
  -------------------------------------------------------------------------*/
  if ((app_qos->field_mask & (int)QOS_MASK_RX_FLOW) ||
      (app_qos->field_mask & (int)QOS_MASK_TX_FLOW))
  {
    req_qos->valid_flg = TRUE;
  }
  else
  {
    req_qos->valid_flg = FALSE;
    return;
  }
  /*-------------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -------------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_RX_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary Modification : Processing RX Req QOS Flow Spec");
    /*-----------------------------------------------------------------------
      Convert the Traffic Class parameter of required set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_traffic_class (
                                app_qos->rx.flow_template.req_flow.trf_class,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos );

    /*-----------------------------------------------------------------------
      Convert the Data Rate parameter of required set into UMTS specific:
      DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.latency,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                &temp_max_bitrate,
                                &temp_gtd_bitrate );
    req_qos->max_dl_bitrate =  temp_max_bitrate;
    req_qos->gtd_dl_bitrate =  temp_gtd_bitrate;

    DS_3GPP_MSG2_HIGH("Req QOS max_dl_bitrate %d, gtd_dl_bitrate %d",req_qos->max_dl_bitrate,
                                               req_qos->gtd_dl_bitrate);

    /*-----------------------------------------------------------------------
      Convert the Packet Error Rate parameter of required set into SDU Error.
    -----------------------------------------------------------------------*/
    dsumtsps_process_pkt_err_rate(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos);

    /*-----------------------------------------------------------------------
      Convert the Residual Bit Error Rate of required set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_residual_ber(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos);

    /*-----------------------------------------------------------------------
      Convert the Traffic Handling Priority parameter of required set
    -----------------------------------------------------------------------*/
    dsumtsps_process_trf_hdl_prio(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos);

    /*-------------------------------------------------------------------------
      Fill the signalling Indication bit if the traffic class is interactive.
      and the Traffic Handling priority is 1.

      Sig Ind Flag should only be set if the traffic class is interactive
      and the traffic handling priority is 1 (This is as per spec 
      23.107 Sec 6.4.3.1).
    -------------------------------------------------------------------------*/
    dsumtsps_process_sig_ind_prio(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos);
  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Rx Flow Parameters.
      Get the current flow parameters and populate the required set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("Secondary Modification :No Change in RX Req QOS Flow Spec");
    rx_flow_params = PS_FLOW_GET_RX_REQUESTED_FLOW(flow_ptr);

    if (rx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
                                  rx_flow_params,
                                  rx_flow_params->latency,
                                  rx_flow_params->field_mask,
                                  &temp_max_bitrate,
                                  &temp_gtd_bitrate );

      req_qos->max_dl_bitrate =  temp_max_bitrate;
      req_qos->gtd_dl_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Rx flow, set the values to
        subscribed.
      ---------------------------------------------------------------------*/
      req_qos->max_dl_bitrate       = 0x0;
      req_qos->gtd_dl_bitrate       = 0x0;
    }
  }
  /*-------------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -------------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_TX_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary Modification : Processing TX Req QOS Flow Spec");
    /*-----------------------------------------------------------------------
      Convert the Data Rate parameter of required set into UMTS specific:
      DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                                &app_qos->tx.flow_template.req_flow,
                                app_qos->tx.flow_template.req_flow.latency,
                                app_qos->tx.flow_template.req_flow.field_mask,
                                &temp_max_bitrate,
                                &temp_gtd_bitrate );

    req_qos->max_ul_bitrate =  temp_max_bitrate;
    req_qos->gtd_ul_bitrate =  temp_gtd_bitrate;

    DS_3GPP_MSG2_HIGH("Req QOS max_ul_bitrate %d, gtd_ul_bitrate %d",
                                              req_qos->max_ul_bitrate,
                                              req_qos->gtd_ul_bitrate);

    if (!(app_qos->field_mask & (int)QOS_MASK_RX_FLOW))
    {
      dsumtsps_process_traffic_class (
                              app_qos->tx.flow_template.req_flow.trf_class,
                              app_qos->tx.flow_template.req_flow.field_mask,
                              req_qos );

      /*---------------------------------------------------------------------
       Convert the Packet Error Rate parameter of required set into SDU Error.
      ---------------------------------------------------------------------*/
      dsumtsps_process_pkt_err_rate(
                              &app_qos->tx.flow_template.req_flow,
                              app_qos->tx.flow_template.req_flow.field_mask,
                              req_qos);

      /*---------------------------------------------------------------------
       Convert the Residual Bit Error Rate of required set into UMTS specific.
      ---------------------------------------------------------------------*/
      dsumtsps_process_residual_ber(
                              &app_qos->tx.flow_template.req_flow,
                              app_qos->tx.flow_template.req_flow.field_mask,
                              req_qos);

      /*---------------------------------------------------------------------
        Convert the Traffic Handling Priority parameter of required set
      ---------------------------------------------------------------------*/
      dsumtsps_process_trf_hdl_prio(
                              &app_qos->tx.flow_template.req_flow,
                              app_qos->tx.flow_template.req_flow.field_mask,
                              req_qos);
      /*-------------------------------------------------------------------------
        Fill the signalling Indication bit if the traffic class is interactive.
        and the Traffic Handling priority is 1.
        
        Sig Ind Flag should only be set if the traffic class is interactive
        and the traffic handling priority is 1 (This is as per spec 
        23.107 Sec 6.4.3.1).
      -------------------------------------------------------------------------*/
      dsumtsps_process_sig_ind_prio(
                                &app_qos->tx.flow_template.req_flow,
                                app_qos->tx.flow_template.req_flow.field_mask,
                                req_qos);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Tx Requested Flow Parameters.
      Get the current flow parameters and populate the required set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("Secondary Modification :No Change in TX Req QOS Flow Spec");
    tx_flow_params = PS_FLOW_GET_TX_REQUESTED_FLOW(flow_ptr);

    if (tx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
                                    tx_flow_params,
                                    tx_flow_params->latency,
                                    tx_flow_params->field_mask,
                                    &temp_max_bitrate,
                                    &temp_gtd_bitrate );

      req_qos->max_ul_bitrate =  temp_max_bitrate;
      req_qos->gtd_ul_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Tx Requested flow, 
        set the values to subscribed.
      ---------------------------------------------------------------------*/
      req_qos->max_ul_bitrate       = 0x0;
      req_qos->gtd_ul_bitrate       = 0x0;
    }
  }
  /*-------------------------------------------------------------------------
    If either the minimum Rx or Tx flow has been specified, set the valid 
    flag to TRUE.
  -------------------------------------------------------------------------*/
  if ((app_qos->field_mask & (int)QOS_MASK_RX_MIN_FLOW) ||
      ( app_qos->field_mask & (int)QOS_MASK_TX_MIN_FLOW))
  {
    min_qos->valid_flg = TRUE;
  }
  else
  {
    min_qos->valid_flg = FALSE;
    return;
  }
  /*-------------------------------------------------------------------------
    DL Minimum QOS parameters have been specified by the application
    Convert the DL Data Rate, DL GTD Data Rate, Traffic class, Traffic
    handling priority, Residual Bit Error Rate, SDU Error Ratio
  -------------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_RX_MIN_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary Modification : Processing RX Min QOS Flow Spec");
    /*-----------------------------------------------------------------------
      Convert the Traffic Class parameter of minimum set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_traffic_class (
                            app_qos->rx.flow_template.min_req_flow.trf_class,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos );

    /*-----------------------------------------------------------------------
      Convert the Data Rate parameter of minimum set into UMTS specific:
      DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                             &app_qos->rx.flow_template.min_req_flow,
                             app_qos->rx.flow_template.min_req_flow.latency,
                             app_qos->rx.flow_template.min_req_flow.field_mask,
                             &temp_max_bitrate,
                             &temp_gtd_bitrate );

    min_qos->max_dl_bitrate =  temp_max_bitrate;
    min_qos->gtd_dl_bitrate =  temp_gtd_bitrate;

    /*-----------------------------------------------------------------------
      Convert the Packet Error Rate parameter of minimum set into SDU Error.
    -----------------------------------------------------------------------*/
    dsumtsps_process_pkt_err_rate(
                            &app_qos->rx.flow_template.min_req_flow,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos);

    /*-----------------------------------------------------------------------
       Convert the Residual Bit Error Rate of minimum set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_residual_ber(
                            &app_qos->rx.flow_template.min_req_flow,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos);

    /*-----------------------------------------------------------------------
       Convert the Traffic Handling Priority parameter of minimum set
    -----------------------------------------------------------------------*/
    dsumtsps_process_trf_hdl_prio(
                            &app_qos->rx.flow_template.min_req_flow,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos);

    /*-------------------------------------------------------------------------
      Fill the signalling Indication bit if the traffic class is interactive.
      and the Traffic Handling priority is 1.

      Sig Ind Flag should only be set if the traffic class is interactive
      and the traffic handling priority is 1 (This is as per spec 
      23.107 Sec 6.4.3.1).
    -------------------------------------------------------------------------*/
    dsumtsps_process_sig_ind_prio(
                                &app_qos->rx.flow_template.min_req_flow,
                                app_qos->rx.flow_template.min_req_flow.field_mask,
                                min_qos);

  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Rx Minimum Flow Parameters.
      Get the current flow parameters and populate the minimum set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("Secondary Modification :No Change in RX Min QOS Flow Spec");
    rx_flow_params = PS_FLOW_GET_RX_MIN_FLOW(flow_ptr);

    if (rx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
                                  rx_flow_params,
                                  rx_flow_params->latency,
                                  rx_flow_params->field_mask,
                                  &temp_max_bitrate,
                                  &temp_gtd_bitrate );

      min_qos->max_dl_bitrate =  temp_max_bitrate;
      min_qos->gtd_dl_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Rx Minimum flow, set the 
        values to subscribed.
      ---------------------------------------------------------------------*/
      min_qos->max_dl_bitrate       = 0x0;
      min_qos->gtd_dl_bitrate       = 0x0;
    }
  }

  /*-----------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -----------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_TX_MIN_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary Modification : Processing TX Min QOS Flow Spec");

    /*-----------------------------------------------------------------------
       Convert the Data Rate parameter of required set into UMTS specific:
       DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                             &app_qos->tx.flow_template.min_req_flow,
                             app_qos->tx.flow_template.min_req_flow.latency,
                             app_qos->tx.flow_template.min_req_flow.field_mask,
                             &temp_max_bitrate,
                             &temp_gtd_bitrate );

    min_qos->max_ul_bitrate =  temp_max_bitrate;
    min_qos->gtd_ul_bitrate =  temp_gtd_bitrate;

    if (!(app_qos->field_mask & (int)QOS_MASK_RX_MIN_FLOW))
    {
      dsumtsps_process_traffic_class (
                              app_qos->tx.flow_template.min_req_flow.trf_class,
                              app_qos->tx.flow_template.min_req_flow.field_mask,
                              min_qos );

      /*-----------------------------------------------------------------------
        Convert the Packet Error Rate parameter of required set into SDU Error.
      -----------------------------------------------------------------------*/
      dsumtsps_process_pkt_err_rate(
                              &app_qos->tx.flow_template.min_req_flow,
                              app_qos->tx.flow_template.min_req_flow.field_mask,
                              min_qos);

      /*-----------------------------------------------------------------------
        Convert the Residual Bit Error Rate of required set into UMTS specific.
      -----------------------------------------------------------------------*/
      dsumtsps_process_residual_ber(
                              &app_qos->tx.flow_template.min_req_flow,
                              app_qos->tx.flow_template.min_req_flow.field_mask,
                              min_qos);

      /*-----------------------------------------------------------------------
        Convert the Traffic Handling Priority parameter of required set
      -----------------------------------------------------------------------*/
      dsumtsps_process_trf_hdl_prio(
                              &app_qos->tx.flow_template.min_req_flow,
                              app_qos->tx.flow_template.min_req_flow.field_mask,
                              min_qos);

      /*-------------------------------------------------------------------------
        Fill the signalling Indication bit if the traffic class is interactive.
        and the Traffic Handling priority is 1.

        Sig Ind Flag should only be set if the traffic class is interactive
        and the traffic handling priority is 1 (This is as per spec 
        23.107 Sec 6.4.3.1).
      -------------------------------------------------------------------------*/
      dsumtsps_process_sig_ind_prio(
                                &app_qos->tx.flow_template.min_req_flow,
                                app_qos->tx.flow_template.min_req_flow.field_mask,
                                min_qos);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Tx Minimum Flow Parameters.
      Get the current flow parameters and populate the minimum set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("Secondary Modification : No Change in TX Min QOS Flow Spec");
    tx_flow_params = PS_FLOW_GET_TX_MIN_FLOW(flow_ptr);

    if (tx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
          tx_flow_params,
          tx_flow_params->latency,
          tx_flow_params->field_mask,
          &temp_max_bitrate,
          &temp_gtd_bitrate );
      
      min_qos->max_ul_bitrate =  temp_max_bitrate;
      min_qos->gtd_ul_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Tx Minimum flow, set the 
        values to subscribed.
      ---------------------------------------------------------------------*/
      min_qos->max_ul_bitrate       = 0x0;
      min_qos->gtd_ul_bitrate       = 0x0;
    }
  }
  /*lint -restore Restore lint error 641*/
  return;
} /* dsumtsps_sec_qos_modify_flow_param_process() */


/*===========================================================================
FUNCTION DSUMTSPS_PRI_QOS_MODIFY_FLOW_PARAM_PROCESS

DESCRIPTION
  Process the application supplied QoS for modification. 
  Convert them into UMTS technology specific parameters. Sort the QoS to 
  find the minimum and requested sets
  In UMTS, except data rates and guaranteed data rates, other parameters
  are common across UL and DL. The table below deascribes how we process
  the parameters given by the application

  Required UL Data Rate            : Taken from TX Required set
  Required UL Guaranteed Data Rate : Taken from TX Required set
  Minimum UL Data Rate             : Taken from TX Minimum set
  Minimum UL Guaranteed Data Rate  : Taken from TX Minimum set

  
  Required DL Data Rate            : Taken from RX Required set
  Required DL Guaranteed Data Rate : Taken from RX Required set
  Minimum DL Data Rate             : Taken from RX Minimum set
  Minimum DL Guaranteed Data Rate  : Taken from RX Minimum set
  
  Required Traffic Class (common)             :Taken from RX or Tx Required set
  Required Traffic Handling Priority (common) :Taken from RX or Tx Required set
  Required RES BER (common)                   :Taken from RX or Tx Required set
  Required SDU Err Ratio (common)             :Taken from RX or Tx Required set

  In this function, process each QOS field individually. If the field is
  not set, set it to default (subscribed). The out come of the process
  is two sets of QOS.

PARAMETERS
  *app_qos_ptr    - Ptr to the application specified QoS parameters for P-PDP
  *umts_qos       - Ptr to the UMTS specific QoS parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
void dsumtsps_pri_qos_modify_flow_param_process
(
  const ps_flow_type            *flow_ptr,
  const primary_qos_spec_type   *app_qos_ptr,
  ds_umts_umts_qos_params_type  *req_qos,
  ds_umts_umts_qos_params_type  *min_qos
)
{
  uint32 temp_max_bitrate, temp_gtd_bitrate;
  ip_flow_type *tx_flow_params, *rx_flow_params;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -save -e641 suppressing error 641*/
  /*-------------------------------------------------------------------------
    Initialize the Required and Minimum sets to defaults
  -------------------------------------------------------------------------*/
  req_qos->traffic_class        = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
  req_qos->dlvry_order          = DS_UMTS_QOS_DORDER_SUBSCRIBE;
  req_qos->max_sdu_size         = 0x0;
  req_qos->sdu_err              = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
  req_qos->res_biterr           = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
  req_qos->dlvr_err_sdu         = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;
  req_qos->trans_delay          = 0x0;

  min_qos->traffic_class        = DS_UMTS_QOS_TCLASS_SUBSCRIBE;
  min_qos->dlvry_order          = DS_UMTS_QOS_DORDER_SUBSCRIBE;
  min_qos->max_sdu_size         = 0x0;
  min_qos->sdu_err              = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
  min_qos->res_biterr           = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
  min_qos->dlvr_err_sdu         = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;
  min_qos->trans_delay          = 0x0;
  min_qos->thandle_prio         = 0x0;
  min_qos->sig_ind              = FALSE;

  /*-------------------------------------------------------------------------
    If either the requested Rx or Tx flow has been specified, set the valid 
    flag to TRUE.
  -------------------------------------------------------------------------*/
  if ((app_qos_ptr->field_mask & (int)QOS_MASK_RX_FLOW) ||
      (app_qos_ptr->field_mask & (int)QOS_MASK_TX_FLOW))
  {
    req_qos->valid_flg = TRUE;
  }
  else
  {
    req_qos->valid_flg = FALSE;
    return;
  }
  /*-------------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -------------------------------------------------------------------------*/
  if ( app_qos_ptr->field_mask & (int)QOS_MASK_RX_FLOW )
  {
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS:Processing RX Req QOS Flow ");
    /*-----------------------------------------------------------------------
      Convert the Traffic Class parameter of required set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_traffic_class (
                              app_qos_ptr->rx_flow_template.req_flow.trf_class,
                              app_qos_ptr->rx_flow_template.req_flow.field_mask,
                              req_qos );

    /*-----------------------------------------------------------------------
      Convert the Data Rate parameter of required set into UMTS specific:
      DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                             &app_qos_ptr->rx_flow_template.req_flow,
                             app_qos_ptr->rx_flow_template.req_flow.latency,
                             app_qos_ptr->rx_flow_template.req_flow.field_mask,
                             &temp_max_bitrate,
                             &temp_gtd_bitrate );
    req_qos->max_dl_bitrate =  temp_max_bitrate;
    req_qos->gtd_dl_bitrate =  temp_gtd_bitrate;

    DS_3GPP_MSG2_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS: Req QOS max_dl_bitrate %d, gtd_dl_bitrate %d",
             req_qos->max_dl_bitrate, req_qos->gtd_dl_bitrate);

    /*-----------------------------------------------------------------------
      Convert the Packet Error Rate parameter of required set into SDU Error.
    -----------------------------------------------------------------------*/
    dsumtsps_process_pkt_err_rate(
                             &app_qos_ptr->rx_flow_template.req_flow,
                             app_qos_ptr->rx_flow_template.req_flow.field_mask,
                             req_qos);

    /*-----------------------------------------------------------------------
      Convert the Residual Bit Error Rate of required set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_residual_ber(
                            &app_qos_ptr->rx_flow_template.req_flow,
                            app_qos_ptr->rx_flow_template.req_flow.field_mask,
                            req_qos);

    /*-----------------------------------------------------------------------
      Convert the Traffic Handling Priority parameter of required set
    -----------------------------------------------------------------------*/
    dsumtsps_process_trf_hdl_prio(
                            &app_qos_ptr->rx_flow_template.req_flow,
                            app_qos_ptr->rx_flow_template.req_flow.field_mask,
                            req_qos);

    /*-------------------------------------------------------------------------
      Fill the signalling Indication bit if the traffic class is interactive.
      and the Traffic Handling priority is 1.

      Sig Ind Flag should only be set if the traffic class is interactive
      and the traffic handling priority is 1 (This is as per spec 
      23.107 Sec 6.4.3.1).
    -------------------------------------------------------------------------*/
    dsumtsps_process_sig_ind_prio(
                                &app_qos_ptr->rx_flow_template.req_flow,
                                app_qos_ptr->rx_flow_template.req_flow.field_mask,
                                req_qos);
  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Rx Flow Parameters.
      Get the current flow parameters and populate the required set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS:No Change in RX Req QOS Flow ");
    rx_flow_params = PS_FLOW_GET_RX_REQUESTED_FLOW(flow_ptr);

    if (rx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
                                  rx_flow_params,
                                  rx_flow_params->latency,
                                  rx_flow_params->field_mask,
                                  &temp_max_bitrate,
                                  &temp_gtd_bitrate );

      req_qos->max_dl_bitrate =  temp_max_bitrate;
      req_qos->gtd_dl_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Rx flow, set the values to
        subscribed.
      ---------------------------------------------------------------------*/
      req_qos->max_dl_bitrate       = 0x0;
      req_qos->gtd_dl_bitrate       = 0x0;
    }
  }
  /*-------------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -------------------------------------------------------------------------*/
  if ( app_qos_ptr->field_mask & (int)QOS_MASK_TX_FLOW )
  {
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS: Processing TX Req QOS Flow ");
    /*-----------------------------------------------------------------------
      Convert the Data Rate parameter of required set into UMTS specific:
      DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                             &app_qos_ptr->tx_flow_template.req_flow,
                             app_qos_ptr->tx_flow_template.req_flow.latency,
                             app_qos_ptr->tx_flow_template.req_flow.field_mask,
                             &temp_max_bitrate,
                             &temp_gtd_bitrate );

    req_qos->max_ul_bitrate =  temp_max_bitrate;
    req_qos->gtd_ul_bitrate =  temp_gtd_bitrate;

    DS_3GPP_MSG2_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS: Req QOS max_ul_bitrate %d, gtd_ul_bitrate %d",req_qos->max_ul_bitrate,
               req_qos->gtd_ul_bitrate);

    if (!(app_qos_ptr->field_mask & (int)QOS_MASK_RX_FLOW))
    {
      dsumtsps_process_traffic_class (
                              app_qos_ptr->tx_flow_template.req_flow.trf_class,
                              app_qos_ptr->tx_flow_template.req_flow.field_mask,
                              req_qos );

      /*---------------------------------------------------------------------
       Convert the Packet Error Rate parameter of required set into SDU Error.
      ---------------------------------------------------------------------*/
      dsumtsps_process_pkt_err_rate(
                              &app_qos_ptr->tx_flow_template.req_flow,
                              app_qos_ptr->tx_flow_template.req_flow.field_mask,
                              req_qos);

      /*---------------------------------------------------------------------
       Convert the Residual Bit Error Rate of required set into UMTS specific.
      ---------------------------------------------------------------------*/
      dsumtsps_process_residual_ber(
                              &app_qos_ptr->tx_flow_template.req_flow,
                              app_qos_ptr->tx_flow_template.req_flow.field_mask,
                              req_qos);

      /*---------------------------------------------------------------------
        Convert the Traffic Handling Priority parameter of required set
      ---------------------------------------------------------------------*/
      dsumtsps_process_trf_hdl_prio(
                              &app_qos_ptr->tx_flow_template.req_flow,
                              app_qos_ptr->tx_flow_template.req_flow.field_mask,
                              req_qos);

      /*-------------------------------------------------------------------------
        Fill the signalling Indication bit if the traffic class is interactive.
        and the Traffic Handling priority is 1.

        Sig Ind Flag should only be set if the traffic class is interactive
        and the traffic handling priority is 1 (This is as per spec 
        23.107 Sec 6.4.3.1).
      -------------------------------------------------------------------------*/
      dsumtsps_process_sig_ind_prio(
                                &app_qos_ptr->tx_flow_template.req_flow,
                                app_qos_ptr->tx_flow_template.req_flow.field_mask,
                                req_qos);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Tx Requested Flow Parameters.
      Get the current flow parameters and populate the required set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS:No Change in TX Req QOS Flow");

    tx_flow_params = PS_FLOW_GET_TX_REQUESTED_FLOW(flow_ptr);

    if (tx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
                                    tx_flow_params,
                                    tx_flow_params->latency,
                                    tx_flow_params->field_mask,
                                    &temp_max_bitrate,
                                    &temp_gtd_bitrate );

      req_qos->max_ul_bitrate =  temp_max_bitrate;
      req_qos->gtd_ul_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Tx Requested flow, 
        set the values to subscribed.
      ---------------------------------------------------------------------*/
      req_qos->max_ul_bitrate       = 0x0;
      req_qos->gtd_ul_bitrate       = 0x0;
    }
  }
  /*-------------------------------------------------------------------------
    If either the minimum Rx or Tx flow has been specified, set the valid 
    flag to TRUE.
  -------------------------------------------------------------------------*/
  if ((app_qos_ptr->field_mask & (int)QOS_MASK_RX_MIN_FLOW) ||
      ( app_qos_ptr->field_mask & (int)QOS_MASK_TX_MIN_FLOW))
  {
    min_qos->valid_flg = TRUE;
  }
  else
  {
    min_qos->valid_flg = FALSE;
    return;
  }
  /*-------------------------------------------------------------------------
    DL Minimum QOS parameters have been specified by the application
    Convert the DL Data Rate, DL GTD Data Rate, Traffic class, Traffic
    handling priority, Residual Bit Error Rate, SDU Error Ratio
  -------------------------------------------------------------------------*/
  if ( app_qos_ptr->field_mask & (int)QOS_MASK_RX_MIN_FLOW )
  {
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS: Processing RX Min QOS Flow");
    /*-----------------------------------------------------------------------
      Convert the Traffic Class parameter of minimum set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_traffic_class (
                         app_qos_ptr->rx_flow_template.min_req_flow.trf_class,
                         app_qos_ptr->rx_flow_template.min_req_flow.field_mask,
                         min_qos );

    /*-----------------------------------------------------------------------
      Convert the Data Rate parameter of minimum set into UMTS specific:
      DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                          &app_qos_ptr->rx_flow_template.min_req_flow,
                          app_qos_ptr->rx_flow_template.min_req_flow.latency,
                          app_qos_ptr->rx_flow_template.min_req_flow.field_mask,
                          &temp_max_bitrate,
                          &temp_gtd_bitrate );

    min_qos->max_dl_bitrate =  temp_max_bitrate;
    min_qos->gtd_dl_bitrate =  temp_gtd_bitrate;

    /*-----------------------------------------------------------------------
      Convert the Packet Error Rate parameter of minimum set into SDU Error.
    -----------------------------------------------------------------------*/
    dsumtsps_process_pkt_err_rate(
                        &app_qos_ptr->rx_flow_template.min_req_flow,
                        app_qos_ptr->rx_flow_template.min_req_flow.field_mask,
                        min_qos);

    /*-----------------------------------------------------------------------
       Convert the Residual Bit Error Rate of minimum set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_residual_ber(
                         &app_qos_ptr->rx_flow_template.min_req_flow,
                         app_qos_ptr->rx_flow_template.min_req_flow.field_mask,
                         min_qos);

    /*-----------------------------------------------------------------------
       Convert the Traffic Handling Priority parameter of minimum set
    -----------------------------------------------------------------------*/
    dsumtsps_process_trf_hdl_prio(
                        &app_qos_ptr->rx_flow_template.min_req_flow,
                        app_qos_ptr->rx_flow_template.min_req_flow.field_mask,
                        min_qos);
    /*-------------------------------------------------------------------------
      Fill the signalling Indication bit if the traffic class is interactive.
      and the Traffic Handling priority is 1.

      Sig Ind Flag should only be set if the traffic class is interactive
      and the traffic handling priority is 1 (This is as per spec 
      23.107 Sec 6.4.3.1).
    -------------------------------------------------------------------------*/
    dsumtsps_process_sig_ind_prio(
                                &app_qos_ptr->rx_flow_template.min_req_flow,
                                app_qos_ptr->rx_flow_template.min_req_flow.field_mask,
                                min_qos);

  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Rx Minimum Flow Parameters.
      Get the current flow parameters and populate the minimum set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS: No Change in RX Min QOS Flow");

    rx_flow_params = PS_FLOW_GET_RX_MIN_FLOW(flow_ptr);

    if (rx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
                                  rx_flow_params,
                                  rx_flow_params->latency,
                                  rx_flow_params->field_mask,
                                  &temp_max_bitrate,
                                  &temp_gtd_bitrate );

      min_qos->max_dl_bitrate =  temp_max_bitrate;
      min_qos->gtd_dl_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Rx Minimum flow, set the 
        values to subscribed.
      ---------------------------------------------------------------------*/
      min_qos->max_dl_bitrate       = 0x0;
      min_qos->gtd_dl_bitrate       = 0x0;
    }
  }

  /*-----------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -----------------------------------------------------------------------*/
  if ( app_qos_ptr->field_mask & (int)QOS_MASK_TX_MIN_FLOW )
  {
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS: Processing TX Min QOS Flow");

    /*-----------------------------------------------------------------------
       Convert the Data Rate parameter of required set into UMTS specific:
       DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                         &app_qos_ptr->tx_flow_template.min_req_flow,
                         app_qos_ptr->tx_flow_template.min_req_flow.latency,
                         app_qos_ptr->tx_flow_template.min_req_flow.field_mask,
                         &temp_max_bitrate,
                         &temp_gtd_bitrate );

    min_qos->max_ul_bitrate =  temp_max_bitrate;
    min_qos->gtd_ul_bitrate =  temp_gtd_bitrate;

    if (!(app_qos_ptr->field_mask & (int)QOS_MASK_RX_MIN_FLOW))
    {
      dsumtsps_process_traffic_class (
                          app_qos_ptr->tx_flow_template.min_req_flow.trf_class,
                          app_qos_ptr->tx_flow_template.min_req_flow.field_mask,
                          min_qos );

      /*-----------------------------------------------------------------------
        Convert the Packet Error Rate parameter of required set into SDU Error.
      -----------------------------------------------------------------------*/
      dsumtsps_process_pkt_err_rate(
                           &app_qos_ptr->tx_flow_template.min_req_flow,
                           app_qos_ptr->tx_flow_template.min_req_flow.field_mask,
                           min_qos);

      /*-----------------------------------------------------------------------
        Convert the Residual Bit Error Rate of required set into UMTS specific.
      -----------------------------------------------------------------------*/
      dsumtsps_process_residual_ber(
                          &app_qos_ptr->tx_flow_template.min_req_flow,
                          app_qos_ptr->tx_flow_template.min_req_flow.field_mask,
                          min_qos);

      /*-----------------------------------------------------------------------
        Convert the Traffic Handling Priority parameter of required set
      -----------------------------------------------------------------------*/
      dsumtsps_process_trf_hdl_prio(
                          &app_qos_ptr->tx_flow_template.min_req_flow,
                          app_qos_ptr->tx_flow_template.min_req_flow.field_mask,
                          min_qos);

      /*-------------------------------------------------------------------------
        Fill the signalling Indication bit if the traffic class is interactive.
        and the Traffic Handling priority is 1.

        Sig Ind Flag should only be set if the traffic class is interactive
        and the traffic handling priority is 1 (This is as per spec 
        23.107 Sec 6.4.3.1).
      -------------------------------------------------------------------------*/
      dsumtsps_process_sig_ind_prio(
                                &app_qos_ptr->tx_flow_template.min_req_flow,
                                app_qos_ptr->tx_flow_template.min_req_flow.field_mask,
                                min_qos);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      There was no change requested in the Tx Minimum Flow Parameters.
      Get the current flow parameters and populate the minimum set.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("PRI_QOS_MODIFY_FLOW_PARAM_PROCESS: No Change in TX Min QOS Flow");

    tx_flow_params = PS_FLOW_GET_TX_MIN_FLOW(flow_ptr);

    if (tx_flow_params != NULL)
    {
      dsumtsps_process_data_rate(
          tx_flow_params,
          tx_flow_params->latency,
          tx_flow_params->field_mask,
          &temp_max_bitrate,
          &temp_gtd_bitrate );
      
      min_qos->max_ul_bitrate =  temp_max_bitrate;
      min_qos->gtd_ul_bitrate =  temp_gtd_bitrate;
    }
    else
    {
      /*---------------------------------------------------------------------
        If for some reason, there was no current Tx Minimum flow, set the 
        values to subscribed.
      ---------------------------------------------------------------------*/
      min_qos->max_ul_bitrate       = 0x0;
      min_qos->gtd_ul_bitrate       = 0x0;
    }
  }
  /*lint -restore Restore lint error 641*/
  return;
} /* dsumtsps_pri_qos_modify_flow_param_process() */


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
  /*lint -esym(715,rx_or_tx) */
)
{
  boolean result = TRUE;

  /*-----------------------------------------------------------------------
    Make sure that the maximum and guaranteed rates are not greater than the 
    theoretical maximum.
  -----------------------------------------------------------------------*/
  if ((flow_ptr->field_mask & (int)IPFLOW_MASK_DATA_RATE) &&
      flow_ptr->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
  {
    if (flow_ptr->data_rate.format.min_max.max_rate > QOS_MAX_BIT_RATE )
    {
      /* Error if the rate specified is more than the theoritical max */
      DS_3GPP_MSG1_ERROR("max data rate exceeds theoritical max: %d",
                 flow_ptr->data_rate.format.min_max.max_rate);
      flow_ptr->err_mask |= (int)IPFLOW_MASK_DATA_RATE;
      result = FALSE;
    }

    if (flow_ptr->data_rate.format.min_max.guaranteed_rate > QOS_MAX_BIT_RATE)
    {
      /* Error if the rate specified is more than the theoritical max */
      DS_3GPP_MSG1_ERROR("max data rate exceeds theoritical max: %d",
                 flow_ptr->data_rate.format.min_max.guaranteed_rate);
      flow_ptr->err_mask |= (int)IPFLOW_MASK_DATA_RATE;
      result = FALSE;
    }
  }

  if ( result)
  {
    DS_3GPP_MSG0_HIGH("Flow Spec validation : NO Errors");
  }
  return result;
}


/*==========================================================================
FUNCTION DSUMTSPS_CONV_NET_ALLOCATED_QOS_TO_APP_QOS

DESCRIPTION
  Given a set of network allocated QoS, converts them into application
  specific QoS. Applies to Secondary context and modify context. 

  If it is for MODIFY, accept_flow should be FALSE. 

PARAMETERS
  *mode_info_ptr    - Ptr to the network allocated QOS.
  *app_qos          - Ptr to App QOS in Call table
  accept_flow       - This should be TRUE for secondary only which would
                      then accept the flow. 
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
  boolean                         accept_flow  /* call_instance should be valid
                                                  when accept_flow is TRUE*/
)
{
  ip_flow_type rx_flow, tx_flow;
  int sdu_exp_lookup[8] = { 0, 2, 3, 3, 4, 5, 6, 1 };
  int sdu_mult_lookup[8] = { 0, 1, 7, 1, 1, 1, 1, 1 };

  /*------------------------------------------------------------------------
    Application requires two kind of flow specs (rx_spec and tx_spec).
    In UMTS, except data rates and guaranteed data rates, other parameters
    are common across UL and DL. The table below describes how we convert
    the network allocated parameters into application specific parameters.

    RX Traffic Class             : Common Traffic Class parameter
    RX Traffic Handling Priority : Common Traffic Handling Priority
    RX RES BER                   : Common RES BER
    RX Packet Err Ratio          : Common SDU Error Ratio

    TX Traffic Class             : Common Traffic Class parameter
    TX Traffic Handling Priority : Common Traffic Handling Priority
    TX RES BER                   : Common RES BER
    TX Packet Err Ratio          : Common SDU Error Ratio

    RX Max Data Rate          :  DL Data Rate
    RX guaranteed Data Rate   :  DL Gauranteed Data Rate


    TX Max Data Rate          :  UL Data Rate
    TX guaranteed Data Rate   :  UL Gauranteed Data Rate

    Note that Data Rate format type will always be
    DATA_RATE_FORMAT_MIN_MAX_TYPE
  ------------------------------------------------------------------------*/
  if(mode_info_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("Cannot convert Net Allocated QOS into App QOS since it is NULL");
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Converting Net Allocated QOS into App QOS");

    DS_3GPP_MSG9_HIGH("delay class:%lx , rel class :%lx, peak :%lx,   \
	               precd class:%lx, mean :%lx, traff class:%lx,   \
		       del order :%lx, del err sdu:%lx, max sdu :%lx",  \
					   mode_info_ptr->info.gw_ps_call.qos.delay_class,
					   mode_info_ptr->info.gw_ps_call.qos.reliability_class,
					   mode_info_ptr->info.gw_ps_call.qos.peak_throughput,
					   mode_info_ptr->info.gw_ps_call.qos.precedence_class,
					   mode_info_ptr->info.gw_ps_call.qos.mean_throughput,
					   mode_info_ptr->info.gw_ps_call.qos.traffic_class,
					   mode_info_ptr->info.gw_ps_call.qos.deliver_order,
					   mode_info_ptr->info.gw_ps_call.qos.deliver_err_sdu ,
					   mode_info_ptr->info.gw_ps_call.qos.max_sdu_size);
    		 
    DS_3GPP_MSG9_HIGH("Max UL rate:%lx, Max DL rate:%lx, Res BER :%lx,     \
	               sdu err rat:%lx, txfer delay:%lx, traff prio :%lx,  \
	               guar UL rate:%lx, guar DL rate:%lx, Sig Ind Bit: %d", \
                       mode_info_ptr->info.gw_ps_call.qos.max_bit_rare_uplink,
					   mode_info_ptr->info.gw_ps_call.qos.max_bit_rare_downlink,
					   mode_info_ptr->info.gw_ps_call.qos.residual_ber ,
					   mode_info_ptr->info.gw_ps_call.qos.sdu_err_ratio,
					   mode_info_ptr->info.gw_ps_call.qos.tranfer_delay,
					   mode_info_ptr->info.gw_ps_call.qos.traffic_priority,
					   mode_info_ptr->info.gw_ps_call.qos.guarantee_ber_uplink,
					   mode_info_ptr->info.gw_ps_call.qos.guarantee_ber_dwnlink,
					   (int)mode_info_ptr->info.gw_ps_call.qos.sig_ind);

    DS_3GPP_MSG1_HIGH("Src Stat Desc: %d",
             (int)mode_info_ptr->info.gw_ps_call.qos.src_stat_desc);

    memset(&rx_flow, 0, sizeof(ip_flow_type));
    memset(&tx_flow, 0, sizeof(ip_flow_type));

    /*------------------------------------------------------------------------
      RX FLOW SPEC: Convert RX RES BER. RES BER is offset by 1.
    ------------------------------------------------------------------------*/
    rx_flow.umts_params.res_ber = (umts_residual_ber_enum_type)
                         (mode_info_ptr->info.gw_ps_call.qos.residual_ber - 1);
    rx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;

    /*------------------------------------------------------------------------
      RX FLOW SPEC: Convert Traffic Class. IP traffic class if ofsset by 1
      as there is no subscribed class. 
    ------------------------------------------------------------------------*/
    rx_flow.trf_class = (ip_traffic_class_enum_type)
                       (mode_info_ptr->info.gw_ps_call.qos.traffic_class - 1);
    rx_flow.field_mask |= (int)IPFLOW_MASK_TRF_CLASS;

    /*------------------------------------------------------------------------
      RX FLOW SPEC: Convert Traffic Handling Priority
    ------------------------------------------------------------------------*/
    if ( rx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE )
    {
      rx_flow.umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
                    (mode_info_ptr->info.gw_ps_call.qos.traffic_priority - 1);
      rx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
    }
    /*------------------------------------------------------------------------
      RX FLOW SPEC: Convert RX Data Rate
    ------------------------------------------------------------------------*/
    rx_flow.data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    rx_flow.data_rate.format.min_max.max_rate =
      dsumtsps_conv_netw_data_rate_to_umts_data_rate(
            mode_info_ptr->info.gw_ps_call.qos.max_bit_rare_downlink)
                                                 * 1000;

    /*------------------------------------------------------------------------
      RX FLOW SPEC: Convert RX Guaranteed Data Rate
    ------------------------------------------------------------------------*/
    if ( !(( rx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE )||
         ( rx_flow.trf_class == IP_TRF_CLASS_BACKGROUND )   ||
         (rx_flow.data_rate.format.min_max.max_rate == 0)))
    {
      rx_flow.data_rate.format.min_max.guaranteed_rate =
        dsumtsps_conv_netw_data_rate_to_umts_data_rate(
          mode_info_ptr->info.gw_ps_call.qos.guarantee_ber_dwnlink) * 1000;
    }
    rx_flow.field_mask |= (int)IPFLOW_MASK_DATA_RATE;

    /*------------------------------------------------------------------------
      RX FLOW SPEC: Convert Packet Error Rate
    ------------------------------------------------------------------------*/
    rx_flow.pkt_err_rate.multiplier =
      (uint16)sdu_mult_lookup[mode_info_ptr->info.gw_ps_call.qos.sdu_err_ratio];
    rx_flow.pkt_err_rate.exponent =
      (uint16)sdu_exp_lookup[mode_info_ptr->info.gw_ps_call.qos.sdu_err_ratio];
    rx_flow.field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;

    /*------------------------------------------------------------------------
      TX FLOW SPEC: Convert RES BER
    ------------------------------------------------------------------------*/
    tx_flow.umts_params.res_ber = (umts_residual_ber_enum_type)
                        (mode_info_ptr->info.gw_ps_call.qos.residual_ber - 1);
    tx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;

    /*------------------------------------------------------------------------
      TX FLOW SPEC: Convert Traffic Class. IP traffic class if ofsset by 1
      as there is no subscribed class. 
    ------------------------------------------------------------------------*/
    tx_flow.trf_class = (ip_traffic_class_enum_type)
                        (mode_info_ptr->info.gw_ps_call.qos.traffic_class - 1);
    tx_flow.field_mask |= (int)IPFLOW_MASK_TRF_CLASS;

    /*------------------------------------------------------------------------
      TX FLOW SPEC: Convert Traffic Handling Priority
    ------------------------------------------------------------------------*/
    if ( tx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE )
    {
      tx_flow.umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
                    ( mode_info_ptr->info.gw_ps_call.qos.traffic_priority -1 );
      tx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
    }
    /*------------------------------------------------------------------------
      TX FLOW SPEC: Convert RX Data Rate
    ------------------------------------------------------------------------*/
    tx_flow.data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    tx_flow.data_rate.format.min_max.max_rate =
      dsumtsps_conv_netw_data_rate_to_umts_data_rate(
        mode_info_ptr->info.gw_ps_call.qos.max_bit_rare_uplink) * 1000;

    /*------------------------------------------------------------------------
      TX FLOW SPEC: Convert RX Guaranteed Data Rate
    ------------------------------------------------------------------------*/
    if ( !(( tx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE )||
           ( tx_flow.trf_class == IP_TRF_CLASS_BACKGROUND )   ||
           (tx_flow.data_rate.format.min_max.max_rate == 0)))
    {
      tx_flow.data_rate.format.min_max.guaranteed_rate =
        dsumtsps_conv_netw_data_rate_to_umts_data_rate(
           mode_info_ptr->info.gw_ps_call.qos.guarantee_ber_uplink) * 1000;
    }
    tx_flow.field_mask |= (int)IPFLOW_MASK_DATA_RATE;


    /*------------------------------------------------------------------------
      TX FLOW SPEC: Convert Packet Error Rate
    ------------------------------------------------------------------------*/
    tx_flow.pkt_err_rate.multiplier =
      (uint16)sdu_mult_lookup[mode_info_ptr->info.gw_ps_call.qos.sdu_err_ratio];
    tx_flow.pkt_err_rate.exponent =
      (uint16)sdu_exp_lookup[mode_info_ptr->info.gw_ps_call.qos.sdu_err_ratio];
    tx_flow.field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;

    /* Source Statistics Descriptor is valid when Traffic Class is conventional
       or Streaming */
    if ( (tx_flow.trf_class == IP_TRF_CLASS_CONVERSATIONAL) ||
         (tx_flow.trf_class == IP_TRF_CLASS_STREAMING) )
    {
      /*------------------------------------------------------------------------
          Convert Signalling Indication Flag
      ------------------------------------------------------------------------*/
      tx_flow.umts_params.src_stat_desc =
        mode_info_ptr->info.gw_ps_call.qos.src_stat_desc;
      tx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SRC_STAT_DESC);
    }
  
  if ( (tx_flow.trf_class == IP_TRF_CLASS_CONVERSATIONAL) ||
       (tx_flow.trf_class == IP_TRF_CLASS_STREAMING) )
    {
      /*------------------------------------------------------------------------
        Convert Signalling Indication Flag
      ------------------------------------------------------------------------*/
      rx_flow.umts_params.src_stat_desc =
        mode_info_ptr->info.gw_ps_call.qos.src_stat_desc;
      rx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SRC_STAT_DESC);
    }

    /* Signal Bit is valid when Traffic Class is Interactive and
       traffic priority is 1 */
    if ( (tx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE ) &&
         (tx_flow.umts_params.trf_pri == UMTS_TRF_HANDLING_PRI1))
    {
      /*------------------------------------------------------------------------
          Convert Signalling Indication Flag
      ------------------------------------------------------------------------*/
      tx_flow.umts_params.sig_ind =
        mode_info_ptr->info.gw_ps_call.qos.sig_ind;
      tx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SIG_IND);
    }
  
    if ( (rx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE ) &&
         (rx_flow.umts_params.trf_pri == UMTS_TRF_HANDLING_PRI1))
    {
      /*------------------------------------------------------------------------
        Convert Signalling Indication Flag
      ------------------------------------------------------------------------*/
      rx_flow.umts_params.sig_ind =
        mode_info_ptr->info.gw_ps_call.qos.sig_ind;
      rx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SIG_IND);
    }

    /*lint -e506 -e774 */
    if ( ( TRUE == accept_flow ) 
       )
    {
    }
    else
    {
      ASSERT ( qos_ptr != NULL );
      qos_ptr->rx.flow_template.req_flow = rx_flow;
      qos_ptr->rx.flow_template.min_req_flow.field_mask = 
                                 (ipflow_field_mask_type)IPFLOW_MASK_NONE;
      qos_ptr->rx.flow_template.num_aux_flows = 0;
      qos_ptr->rx.flow_template.aux_flow_list_ptr = NULL;
    
      qos_ptr->tx.flow_template.req_flow = tx_flow;
      qos_ptr->tx.flow_template.min_req_flow.field_mask = 
                                 (ipflow_field_mask_type)IPFLOW_MASK_NONE;
      qos_ptr->tx.flow_template.num_aux_flows = 0;
      qos_ptr->tx.flow_template.aux_flow_list_ptr = NULL;
  
      qos_ptr->field_mask =  (qos_spec_field_mask_type)
                        ((uint16)QOS_MASK_TX_FLOW | (uint16)QOS_MASK_RX_FLOW);
    }
  }
  /*lint +e506 +e774 */
  return;
} /* dsumtsps_conv_net_allocated_qos_to_app_qos */


/*==========================================================================
FUNCTION DSUMTSPS_CONV_NETW_QOS_TO_UMTS_QOS

DESCRIPTION
  Given a set of network allocated QoS, converts them into UMTS
  specific QoS. 

PARAMETERS
  (->) *netw_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *umts_qos_ptr    - Ptr to App QOS in Call table

DEPENDENCIES
  None.

RETURN VALUE
  TRUE on SUCCESS
  FALSE on FAILURE

SIDE EFFECTS


===========================================================================*/

boolean dsumtsps_conv_netw_qos_to_umts_qos
(
  const qos_T                  *netw_qos_ptr,
  ds_umts_umts_qos_params_type *umts_qos_ptr
)
{   
    byte tx_delay_msb;
    byte tx_delay_val;

    if ( !DSUMTPS_QOS_IS_CM_PROV_NETW_QOS_VALID( netw_qos_ptr ) )
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_UMTS_QOS: Invalid Netw QoS Ptr");
      return FALSE;
    }

    if ( umts_qos_ptr == NULL )
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_UMTS_QOS: Invalid UMTS QoS Blob Ptr");
      return FALSE;
    }

    DS_3GPP_MSG0_HIGH("CONV_NETW_QOS_TO_UMTS_QOS: Starting QoS conversion routine");

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Unset the valid flag
     ----------------------------------------------------------------------*/
    umts_qos_ptr->valid_flg = FALSE;

    /*-----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Traffic Class, same for UL and DL
    -----------------------------------------------------------------------*/
    umts_qos_ptr->traffic_class = (ds_umts_qos_tclass_enum_type)
                                              netw_qos_ptr->traffic_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Max and Guaranteed Data Rates (kbps), 
      diff for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->max_ul_bitrate = 
        dsumtsps_conv_netw_data_rate_to_umts_data_rate( 
                                 netw_qos_ptr->max_bit_rare_uplink );
    umts_qos_ptr->max_dl_bitrate = 
        dsumtsps_conv_netw_data_rate_to_umts_data_rate( 
                                 netw_qos_ptr->max_bit_rare_downlink );

    umts_qos_ptr->gtd_ul_bitrate = 
        dsumtsps_conv_netw_data_rate_to_umts_data_rate( 
                                  netw_qos_ptr->guarantee_ber_uplink );
    umts_qos_ptr->gtd_dl_bitrate = 
        dsumtsps_conv_netw_data_rate_to_umts_data_rate( 
                                 netw_qos_ptr->guarantee_ber_dwnlink );

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Residual Bit Error Rate, 
      same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->dlvry_order = (ds_umts_qos_dorder_enum_type)
                                            netw_qos_ptr->deliver_order;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Max SDU Size, same for UL and DL
     ----------------------------------------------------------------------*/
    if ( netw_qos_ptr->max_sdu_size <= QOS_MAX_SDU_1500_BYTES ) 
    {
     umts_qos_ptr->max_sdu_size = netw_qos_ptr->max_sdu_size * 10;
    }
    else
    {
        switch ( netw_qos_ptr->max_sdu_size ) 
        {
        case QOS_MAX_SDU_1502_BYTES:
            umts_qos_ptr->max_sdu_size = 1502;
            break;
        case QOS_MAX_SDU_1510_BYTES:
            umts_qos_ptr->max_sdu_size = 1510;
            break;
        case QOS_MAX_SDU_1520_BYTES:
            umts_qos_ptr->max_sdu_size = 1520;
            break;
        default:
            DS_3GPP_MSG1_HIGH("CONV_NETW_QOS_TO_UMTS_QOS: Max SDU Size is out of range:%d, \
                     setting to RESERVED",netw_qos_ptr->max_sdu_size);
            umts_qos_ptr->max_sdu_size = QOS_MAX_SDU_RESERVED;
            break;
        }
    }

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Packet Error Rate, same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->sdu_err = (ds_umts_qos_sdu_error_enum_type)
                                             netw_qos_ptr->sdu_err_ratio;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Residual Bit Error Rate, same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->res_biterr = (ds_umts_qos_res_ber_enum_type)
                                              netw_qos_ptr->residual_ber;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Delivery of err SDU, same for UL and DL
     ----------------------------------------------------------------------*/
    umts_qos_ptr->dlvr_err_sdu = (ds_umts_qos_sdu_dlvr_enum_type)
                                            netw_qos_ptr->deliver_err_sdu;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store transfer delay, same for UL and DL 
     ----------------------------------------------------------------------*/
    umts_qos_ptr->trans_delay = 0;

    /*----------------------------------------------------------------------
      Table 10.5.156/3GPP TS 24.008: Quality of service information element
        
      0 0 0 0 0 1 The Transfer delay is binary coded in 6 bits, using a 
                  granularity of 10 ms giving a range of values from 10 ms 
      0 0 1 1 1 1 to 150 ms in 10 ms increments.

      0 1 0 0 0 0 The transfer delay is 200 ms + 
                  ((the binary coded value in 6 bits - 010000) * 50 ms)
      0 1 1 1 1 1 giving a range of values from 200 ms to 950 ms in 50ms 
                  increments.

      1 0 0 0 0 0 The transfer delay is 1000 ms + 
                  ((the binary coded value in 6 bits - 100000) * 100 ms)
      1 1 1 1 1 0 giving a range of values from 1000 ms to 4000 ms in 100ms 
                  increments.
      1 1 1 1 1 1 Reserved.

      The Transfer delay value is ignored if the Traffic Class is 
      Interactive class or Background class.
    ----------------------------------------------------------------------*/
    if ( ( umts_qos_ptr->traffic_class != DS_UMTS_QOS_TCLASS_INTR ) &&
         ( umts_qos_ptr->traffic_class != DS_UMTS_QOS_TCLASS_BACK )
        ) 
    {
      /* Extract 2 MSBs */
      tx_delay_msb = (netw_qos_ptr->tranfer_delay >> 4) & 0x3; 
      tx_delay_val = (netw_qos_ptr->tranfer_delay & 0xF);

      if (tx_delay_msb == 0) 
      {
        umts_qos_ptr->trans_delay = tx_delay_val * 10;
      }
      else if (tx_delay_msb == 1 )
      {
        umts_qos_ptr->trans_delay = 200 + tx_delay_val * 50;
      }
      else if (tx_delay_msb > 1) 
      {
        tx_delay_val = (netw_qos_ptr->tranfer_delay & 0x1F);
        umts_qos_ptr->trans_delay = 1000 + tx_delay_val * 100;
      }
    }
        
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert & Store Traffic Handling Priority, 
      same for UL and DL 
     ----------------------------------------------------------------------*/
    umts_qos_ptr->thandle_prio = netw_qos_ptr->traffic_priority;

    if(umts_qos_ptr->traffic_class == DS_UMTS_QOS_TCLASS_INTR)
    {
      /*----------------------------------------------------------------------
        SIG_IND: Convert & Store Signalling Indication Bit,same for UL and DL 
      ----------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("converting the signalling indication parameter: sig_ind - %d", 
             netw_qos_ptr->sig_ind);
      umts_qos_ptr->sig_ind = netw_qos_ptr->sig_ind;

    }

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Set the valid flag
     ----------------------------------------------------------------------*/
    umts_qos_ptr->valid_flg = TRUE;

    DS_3GPP_MSG0_HIGH("CONV_NETW_QOS_TO_UMTS_QOS: End of QoS conversion routine");

    return TRUE;
}/*dsumtsps_conv_netw_qos_to_umts_qos*/


/*==========================================================================
FUNCTION DSUMTSPS_CONV_UMTS_QOS_TO_APP_QOS

DESCRIPTION
  Given a set of UMTS QoS, converts them into application specific QoS.

  In UMTS, except data rates and guaranteed data rates, other parameters
  are common across UL and DL. The table below describes how we process
  the parameters given by the application
  
  RX Traffic Class             : Common Traffic Class parameter
  RX Traffic Handling Priority : Common Traffic Handling Priority
  RX RES BER                   : Common RES BER
  RX Packet Err Ratio          : Common SDU Error Ratio
  TX Traffic Class             : Common Traffic Class parameter
  TX Traffic Handling Priority : Common Traffic Handling Priority
  TX RES BER                   : Common RES BER
  TX Packet Err Ratio          : Common SDU Error Ratio
  RX Max Data Rate          :  DL Data Rate
  RX guaranteed Data Rate   :  DL Gauranteed Data Rate

  TX Max Data Rate          :  UL Data Rate
  TX guaranteed Data Rate   :  UL Gauranteed Data Rate
  Note that Data Rate format type will always be
  DATA_RATE_FORMAT_MIN_MAX_TYPE

PARAMETERS
  (->) *umts_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *app_qos_ptr     - Ptr to App QOS in Call table

DEPENDENCIES
  None.

RETURN VALUE
  boolean

LIMITATIONS
  This routine is internal to DS and thus basic input arg validation is done
===========================================================================*/

boolean dsumtsps_conv_umts_qos_to_app_qos
(
  const ds_umts_umts_qos_params_type                    *umts_qos_ptr,
  ps_flow_ioctl_primary_qos_get_granted_flow_spec_type  *app_qos_ptr
)
{   
    ip_flow_type *app_rx_flow_spec, *app_tx_flow_spec;
    int sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_MAX] =
        { 0, 2, 3, 3, 4, 5, 6, 1 };
    int sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_MAX] = 
        { 0, 1, 7, 1, 1, 1, 1, 1 };

    app_rx_flow_spec = &app_qos_ptr->rx_ip_flow;
    app_tx_flow_spec = &app_qos_ptr->tx_ip_flow;

    if ( !DSUMTSPS_QOSI_IS_QOS_BLOB_VALID( umts_qos_ptr ))
    {
      DS_3GPP_MSG0_ERROR("CONV_UMTS_QOS_TO_APP_QOS: UMTS QoS Blob Ptr is not valid");
      return FALSE;
    }

    if ( app_qos_ptr == NULL ) 
    {
      DS_3GPP_MSG0_ERROR("CONV_UMTS_QOS_TO_APP_QOS: App Qos Ptr is not valid");
      return FALSE;
    }
        
    DS_3GPP_MSG0_HIGH("CONV_UMTS_QOS_TO_APP_QOS: Starting QoS conversion routine");

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Traffic Class, same for UL and DL
    -----------------------------------------------------------------------*/
    if( umts_qos_ptr->traffic_class > 0 )
    {
      app_rx_flow_spec->trf_class = (ip_traffic_class_enum_type)
      ((uint32)umts_qos_ptr->traffic_class - 1);
      app_rx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_TRF_CLASS;

      app_tx_flow_spec->trf_class = (ip_traffic_class_enum_type)
      ((uint32)umts_qos_ptr->traffic_class - 1);
      app_tx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_TRF_CLASS;
    }
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Traffic Handling Priority, same for UL and DL 
    -----------------------------------------------------------------------*/
    if ( ( umts_qos_ptr->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
	 (umts_qos_ptr->thandle_prio > 0 ) )
    {
      app_rx_flow_spec->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
        ( umts_qos_ptr->thandle_prio - 1 );
      app_rx_flow_spec->field_mask |= ((uint32) IPFLOW_MASK_UMTS_TRF_PRI);
  
      app_tx_flow_spec->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
       ( umts_qos_ptr->thandle_prio - 1 );
      app_tx_flow_spec->field_mask |= ((uint32) IPFLOW_MASK_UMTS_TRF_PRI);

      /*----------------------------------------------------------------------
       Signal Bit is valid when Traffic Class is Interactive and
       traffic priority is 1 
      -----------------------------------------------------------------------*/
      if(umts_qos_ptr->thandle_prio == 1)
      {
        app_rx_flow_spec->umts_params.sig_ind = umts_qos_ptr->sig_ind;
        app_rx_flow_spec->field_mask |= ((uint32) IPFLOW_MASK_UMTS_SIG_IND);

        app_tx_flow_spec->umts_params.sig_ind = umts_qos_ptr->sig_ind;
        app_tx_flow_spec->field_mask |= ((uint32) IPFLOW_MASK_UMTS_SIG_IND);
      }
    }
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Max and Guaranteed Data Rates (bps), 
      diff for UL and DL
    -----------------------------------------------------------------------*/
    app_rx_flow_spec->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    app_rx_flow_spec->data_rate.format.min_max.max_rate = 
                                    umts_qos_ptr->max_dl_bitrate * 1000;
    if ( !( ( umts_qos_ptr->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) ||
            ( umts_qos_ptr->traffic_class == DS_UMTS_QOS_TCLASS_BACK ) ||
            ( umts_qos_ptr->max_dl_bitrate == 0 )) )
    {
      app_rx_flow_spec->data_rate.format.min_max.guaranteed_rate = 
                                    umts_qos_ptr->gtd_dl_bitrate * 1000;
    }
    if ( (app_rx_flow_spec->data_rate.format.min_max.max_rate != 0)||
         (app_rx_flow_spec->data_rate.format.min_max.guaranteed_rate !=0 ) )
    {
      app_rx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_DATA_RATE;
    }

    app_tx_flow_spec->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    app_tx_flow_spec->data_rate.format.min_max.max_rate = 
                                    umts_qos_ptr->max_ul_bitrate * 1000;
    if ( !( ( umts_qos_ptr->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) ||
            ( umts_qos_ptr->traffic_class == DS_UMTS_QOS_TCLASS_BACK ) ||
            ( umts_qos_ptr->max_ul_bitrate == 0 )) )
    {
      app_tx_flow_spec->data_rate.format.min_max.guaranteed_rate = 
                                    umts_qos_ptr->gtd_ul_bitrate * 1000;
    }

    if ( (app_tx_flow_spec->data_rate.format.min_max.max_rate != 0)||
         (app_tx_flow_spec->data_rate.format.min_max.guaranteed_rate !=0 ) )
    {
      app_tx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_DATA_RATE;
    }
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Packet Error Rate, same for UL and DL
    -----------------------------------------------------------------------*/
    if(umts_qos_ptr->sdu_err < DS_UMTS_QOS_SDU_ERR_MAX)
    {
      app_rx_flow_spec->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[umts_qos_ptr->sdu_err];
      app_rx_flow_spec->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[umts_qos_ptr->sdu_err];
      app_rx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_PKT_ERR_RATE;

      app_tx_flow_spec->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[umts_qos_ptr->sdu_err];
      app_tx_flow_spec->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[umts_qos_ptr->sdu_err];
      app_tx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_PKT_ERR_RATE;
    }

    if( (umts_qos_ptr->res_biterr != DS_UMTS_QOS_RES_BER_SUBSCRIBE) && 
        (umts_qos_ptr->res_biterr <= DS_UMTS_QOS_RES_BER_6E8 ) &&
        (umts_qos_ptr->res_biterr > 0 ) )
    {
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Residual Bit Error Rate, same for UL and DL
    -----------------------------------------------------------------------*/
    app_rx_flow_spec->umts_params.res_ber = (umts_residual_ber_enum_type)
    ((uint8) umts_qos_ptr->res_biterr - 1);
    app_rx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_UMTS_RES_BER;

    app_tx_flow_spec->umts_params.res_ber = (umts_residual_ber_enum_type)
     ( (uint8) umts_qos_ptr->res_biterr - 1);
    app_tx_flow_spec->field_mask |= (uint32)IPFLOW_MASK_UMTS_RES_BER;
   }

    DS_3GPP_MSG0_HIGH("CONV_UMTS_QOS_TO_APP_QOS: End of QoS conversion routine");

    return TRUE;
}/*dsumtsps_conv_umts_qos_to_app_qos*/

/*==========================================================================
FUNCTION DSUMTSPS_CONV_GPRS_QOS_TO_APP_QOS

DESCRIPTION
  Given a set of UMTS QoS, converts them into application specific QoS.

  Comments are provided before each parameter conversion

PARAMETERS
  (->) *umts_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *app_qos_ptr     - Ptr to App QOS in Call table

DEPENDENCIES
  None.

RETURN VALUE
  boolean

LIMITATIONS

===========================================================================*/

boolean dsumtsps_conv_gprs_qos_to_app_qos
(
  const ds_umts_gprs_qos_params_type                    *gprs_qos_ptr,
  ps_flow_ioctl_primary_qos_get_granted_flow_spec_type  *app_qos_ptr
)
{   
  ip_flow_type *app_rx_ip_flow_ptr, *app_tx_ip_flow_ptr;
  int sdu_exp_lookup_tbl[8] = { 0, 2, 3, 3, 4, 5, 6, 1 };
  int sdu_mul_lookup_tbl[8] = { 0, 1, 7, 1, 1, 1, 1, 1 };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  app_rx_ip_flow_ptr = &app_qos_ptr->rx_ip_flow;
  app_tx_ip_flow_ptr = &app_qos_ptr->tx_ip_flow;

    if ( !DSUMTSPS_QOSI_IS_QOS_BLOB_VALID( gprs_qos_ptr ))
    {
      DS_3GPP_MSG0_ERROR("CONV_UMTS_QOS_TO_APP_QOS: UMTS QoS Blob Ptr is not valid");
      return FALSE;
    }

    if ( app_qos_ptr == NULL ) 
    {
      DS_3GPP_MSG0_ERROR("CONV_UMTS_QOS_TO_APP_QOS: App Qos Ptr is not valid");
      return FALSE;
    }
        
    DS_3GPP_MSG0_HIGH("CONV_UMTS_QOS_TO_APP_QOS: Starting QoS conversion routine");

   
    /*-------------------------------------------------------------------------
      R99 parameters

      Traffic class
      Interactive (0x3): delay class = 1/2/3
      Bakground  (0x4) : delay class = 4
      Traffic handling priority
      1 : delay class = 1
      2 : delay class = 2
      3 : delay class = 3
    -------------------------------------------------------------------------*/
    if( gprs_qos_ptr->delay == QOS_DELAY_CLASS_SUBSCRIBED )
    {
      app_rx_ip_flow_ptr->trf_class = (ip_traffic_class_enum_type)
      DS_UMTS_QOS_TCLASS_SUBSCRIBE;
      app_tx_ip_flow_ptr->trf_class = (ip_traffic_class_enum_type)
      DS_UMTS_QOS_TCLASS_SUBSCRIBE;

      app_rx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      QOS_TH_PRIO_SUBSCRIBED;
      app_tx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      QOS_TH_PRIO_SUBSCRIBED;
    }
    else if( gprs_qos_ptr->delay <= QOS_DELAY_CLASS_3 )
    {
      app_rx_ip_flow_ptr->trf_class = IP_TRF_CLASS_INTERACTIVE;
      app_tx_ip_flow_ptr->trf_class = IP_TRF_CLASS_INTERACTIVE;

      app_rx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      gprs_qos_ptr->delay;
      app_tx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      gprs_qos_ptr->delay;
    }
    else if ( gprs_qos_ptr->delay == QOS_DELAY_CLASS_4 )
    {
      app_rx_ip_flow_ptr->trf_class = IP_TRF_CLASS_BACKGROUND;
      app_tx_ip_flow_ptr->trf_class = IP_TRF_CLASS_BACKGROUND;

      app_rx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      gprs_qos_ptr->delay;
      app_tx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      gprs_qos_ptr->delay;
    }
    else
    {
      app_rx_ip_flow_ptr->trf_class = (ip_traffic_class_enum_type)
      DS_UMTS_QOS_TCLASS_SUBSCRIBE;
      app_tx_ip_flow_ptr->trf_class = (ip_traffic_class_enum_type)
      DS_UMTS_QOS_TCLASS_SUBSCRIBE;

      app_rx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      QOS_TH_PRIO_SUBSCRIBED;
      app_tx_ip_flow_ptr->umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
      QOS_TH_PRIO_SUBSCRIBED;
    }

    app_rx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_TRF_CLASS;
    app_tx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_TRF_CLASS;

    app_rx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
    app_tx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;

    /*-------------------------------------------------------------------------
      SDU error ratio,
      1*10e-6 : rel. class = 1/2
      1*10e-4 : rel. class = 3
      1*10e-3 : rel. class = 4/5
    -------------------------------------------------------------------------*/
    if( gprs_qos_ptr->reliability == QOS_REL_CLASS_SUBSCRIBED)
    {
      app_rx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];
      app_rx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];

      app_tx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];
      app_tx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];
    }
    else if( gprs_qos_ptr->reliability <= QOS_REL_CLASS_2)
    {
      app_rx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E6];
      app_rx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E6];

      app_tx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E6];
      app_tx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E6];
    }
    else if( gprs_qos_ptr->reliability == QOS_REL_CLASS_3 )
    {
      app_rx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E4];
      app_rx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E4];

      app_tx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E4];
      app_tx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E4];
    }
    else if( gprs_qos_ptr->reliability <= QOS_REL_CLASS_5 )
    {
      app_rx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E3];
      app_rx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E3];

      app_tx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E3];
      app_tx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_1E3];
    }
    else
    {
      app_rx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];
      app_rx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];

      app_tx_ip_flow_ptr->pkt_err_rate.multiplier = 
          (uint16)sdu_mul_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];
      app_tx_ip_flow_ptr->pkt_err_rate.exponent = 
          (uint16)sdu_exp_lookup_tbl[DS_UMTS_QOS_SDU_ERR_SUBSCRIBE];
    }

    app_rx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
    app_tx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;

    /*-------------------------------------------------------------------------
      Res BER , Delivery of err SDU
      reliablity class = 1,2,3,4 : Res BER = 1*10e-5, Err SDU = NO
      reliablity class = 5       : Res BER = 4*10e-3, Err SDU = yes
    -------------------------------------------------------------------------*/
    if( gprs_qos_ptr->reliability == QOS_REL_CLASS_SUBSCRIBED )
    {
      app_rx_ip_flow_ptr->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE_MIN;
      app_tx_ip_flow_ptr->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE_MIN;
    }
    else if( gprs_qos_ptr->reliability <= QOS_REL_CLASS_4 )
    {
      app_rx_ip_flow_ptr->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE7;
      app_tx_ip_flow_ptr->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE7;
    }
    else if( gprs_qos_ptr->reliability == QOS_REL_CLASS_5 )
    {
      app_rx_ip_flow_ptr->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE4;
      app_tx_ip_flow_ptr->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE4;
    }
    else
    {
      app_rx_ip_flow_ptr->umts_params.res_ber = (umts_residual_ber_enum_type)
      DS_UMTS_QOS_RES_BER_SUBSCRIBE;
      app_tx_ip_flow_ptr->umts_params.res_ber = (umts_residual_ber_enum_type)
      DS_UMTS_QOS_RES_BER_SUBSCRIBE;
    }

    app_rx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;
    app_tx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;

    /*-------------------------------------------------------------------------
      Max Bit rate:
      8    kbps : peak throughput class = 1  ( 1k bytes )
      16   kbps : peak throughput class = 2  ( 2k bytes )
      32   kbps : peak throughput class = 3  ( 4k bytes )
      64   kbps : peak throughput class = 4  (8 k bytes )
      128  kbps : peak throughput class = 5  (16 k bytes )
      256  kbps : peak throughput class = 6  (32 k bytes )
      512  kbps : peak throughput class = 7  (64 k bytes )
      1024 kbps : peak throughput class = 8  (128 k bytes )
      2048 kbps : peak throughput class = 9  (256 k bytes )
    -------------------------------------------------------------------------*/
    app_rx_ip_flow_ptr->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    app_tx_ip_flow_ptr->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;

    switch ( gprs_qos_ptr->peak )
    {
      case QOS_PEAK_TPUT_1KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                  QOS_BIT_RATE_8KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                  QOS_BIT_RATE_8KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_2KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                 QOS_BIT_RATE_16KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                 QOS_BIT_RATE_16KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_4KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                 QOS_BIT_RATE_32KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                 QOS_BIT_RATE_32KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_8KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                 QOS_BIT_RATE_64KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                 QOS_BIT_RATE_64KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_16KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                QOS_BIT_RATE_128KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                QOS_BIT_RATE_128KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_32BYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                QOS_BIT_RATE_256KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                QOS_BIT_RATE_256KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_64KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                QOS_BIT_RATE_512KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                                QOS_BIT_RATE_512KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_128KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                               QOS_BIT_RATE_1024KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                               QOS_BIT_RATE_1024KBPS * 1024;
        break;

      case QOS_PEAK_TPUT_256KBYTES:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                               QOS_BIT_RATE_2048KBPS * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                               QOS_BIT_RATE_2048KBPS * 1024;
        break;

      default:
        app_rx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                             QOS_BIT_RATE_SUBSCRIBED * 1024;
        app_tx_ip_flow_ptr->data_rate.format.min_max.max_rate = 
                                             QOS_BIT_RATE_SUBSCRIBED * 1024;
    }
    if(gprs_qos_ptr->peak != QOS_BIT_RATE_SUBSCRIBED)
    {
    app_rx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_DATA_RATE;
    app_tx_ip_flow_ptr->field_mask |= (int)IPFLOW_MASK_DATA_RATE;
    }

    return TRUE;
}/*dsumtsps_conv_gprs_qos_to_app_qos*/


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
)
{

    if ( !DSUMTPS_QOS_IS_CM_PROV_NETW_QOS_VALID( netw_qos_ptr ) )
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_GPRS_QOS: Invalid Netw QoS Ptr");
      return FALSE;
    }

    if ( gprs_qos_ptr == NULL )
    {
      DS_3GPP_MSG0_ERROR("CONV_NETW_QOS_TO_GPRS_QOS: Invalid UMTS QoS Blob Ptr");
      return FALSE;
    }

    DS_3GPP_MSG0_HIGH("CONV_NETW_QOS_TO_GPRS_QOS: Starting QoS conversion routine");

    /*----------------------------------------------------------------------
      Unset the valid flag
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->valid_flg = FALSE;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Precedence Class, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->precedence = netw_qos_ptr->precedence_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Delay Class, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->delay = netw_qos_ptr->delay_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Reliability Class, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->reliability = netw_qos_ptr->reliability_class;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert peak thruput, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->peak = netw_qos_ptr->peak_throughput;

    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert mean thruput, same for UL and DL
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->mean = netw_qos_ptr->mean_throughput;

    /*----------------------------------------------------------------------
      Set the valid flag
     ----------------------------------------------------------------------*/
    gprs_qos_ptr->valid_flg = TRUE;

    return TRUE;

}

/*==========================================================================
FUNCTION DSUMTSPS_CONV_NETW_DATA_RATE_TO_UMTS_DATA_RATE

DESCRIPTION
  Converts Netw encoded data rate to UMTS data rate 

PARAMETERS
  (->) byte netw_encoded_data_rate;

DEPENDENCIES
  None.

RETURN VALUE
  uint32 decoded umts_data_rate

LIMITATIONS
  Internal DS routine. Can decode max data rate of 8640 kbps
===========================================================================*/
uint32 dsumtsps_conv_netw_data_rate_to_umts_data_rate
(
    byte netw_encoded_data_rate
)
{
        if ( ( netw_encoded_data_rate > 0 ) && 
             ( netw_encoded_data_rate <= 0x3F ) ) 
        {
            return ( netw_encoded_data_rate );
        }
        else if ( ( netw_encoded_data_rate > 0x3F ) && 
                  ( netw_encoded_data_rate <= 0x7F ) )
        {
            return ( 64 + (netw_encoded_data_rate - 0x40)*8 );
        }
        else if ( ( netw_encoded_data_rate > 0x7F ) && 
                  ( netw_encoded_data_rate <= 0xFE ) ) 
        {
            return ( 576 + (netw_encoded_data_rate - 0x80)*64 );
        }
        return 0;
}

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
)
{
  uint32 decoded_data_rate=0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* According to Spec 3GPP TS 24.008 version 10.7.0 ETSI TS 124 008 V10.7.0 (2012-07)*/
  if(encoded_data_rate == 0)
  {
    decoded_data_rate = QOS_BIT_RATE_SUBSCRIBED;
  }
  else if (encoded_data_rate >= 1 && encoded_data_rate <= 0x4A)
  {
    /*-----------------------------------------------------------------------
      Data rate = 8600kbps + ( ( 8 bit binary encoded value) * 100kbps)
      giving a range of 8700kbps -16000kbps in 100kbps increments
    -----------------------------------------------------------------------*/
    decoded_data_rate = (uint32)(8600+(encoded_data_rate)*100);
  } 
  else if (encoded_data_rate >= 0x4B && encoded_data_rate <= 0xBA) 
  {
    /*-----------------------------------------------------------------------
      Data rate = 16 Mbps + ((the binary coded value in 8 bits - 01001010) * 1 Mbps)
      giving a range of values from 17 Mbps to 128 Mbps in 1 Mbps increments
    -----------------------------------------------------------------------*/
    decoded_data_rate = (uint32)(16000+(encoded_data_rate-0x4A)*1000);
  } 
  else if (encoded_data_rate >= 0xBB && encoded_data_rate <= 0xFA) 
  {
    /*-----------------------------------------------------------------------
      Data rate = 128 Mbps + ((the binary coded value in 8 bits - 10111010) * 2 Mbps)
      giving a range of values from 130 Mbps to 256 Mbps in 2 Mbps increments
    -----------------------------------------------------------------------*/
    decoded_data_rate = (uint32) (128000+(encoded_data_rate-0xBA)*2000);
  }
  
 return decoded_data_rate;
} /* dsumtsps_convert_data_rate_to_r7_ext_decode_val */

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS  */
