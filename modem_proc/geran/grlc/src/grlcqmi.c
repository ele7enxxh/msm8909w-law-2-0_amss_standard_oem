/*============================================================================
  FILE:         GRLCQMI.C

  OVERVIEW:     GRLC QMI Interface Definition

                Provides APIs for querying bearer data rates from the GSM/GPRS Access
                Stratum

  FUNCTION SUMMARY

                geran_get_bearer_data_rates()
                  Returns data structure comprising the maximum and current GERAN data
                  bearer rates


  DEPENDENCIES: Requires RLC uplink and downlink data structures, current NW
                allocations and various

                Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcqmi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/09/08   hv      Created
           abz     Added ...

===========================================================================*/

#include "geran_variation.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "rr_public_store.h"
#include "geran_qmi.h"
#include "grlci.h"
#include "grlcultypes.h"
#include "grlcmaccmn.h"
#include "gmacrlc.h"
#include "grlcdli.h"
#include "grlculproc.h"
#include "grlcdl.h"

/*****************************************************************************
***
***     Private Data
***
*****************************************************************************/

/*---------------------------------------------------------------------------
** Table of multislot class versus maximum timeslot (on dl or ul).
** This is an array with multislotclass value 1-45 as index. The values
** contained in each element are maximum number of rx and tx timeslots
**---------------------------------------------------------------------------
*/
#define MAX_MULTISLOT_SLOT_CLASS  45

/* max_data_rate [Rx][Tx]
*/
static const uint16 max_data_rate[MAX_MULTISLOT_SLOT_CLASS + 1][2] =
{
/*
  Rx,Tx    Multislot Class
*/
  { 0,0 },     /*  not used */
  { 1,1 },     /*  1 */
  { 2,1 },     /*  2 */
  { 2,2 },     /*  3 */
  { 3,1 },     /*  4 */
  { 2,2 },     /*  5 */
  { 3,2 },     /*  6 */
  { 3,3 },     /*  7 */
  { 4,1 },     /*  8 */
  { 3,2 },     /*  9 */
  { 4,2 },     /* 10 */
  { 4,3 },     /* 11 */
  { 4,4 },     /* 12 */
  { 3,3 },     /* 13 */
  { 4,4 },     /* 14 */
  { 5,5 },     /* 15 */
  { 6,6 },     /* 16 */
  { 7,7 },     /* 17 */
  { 8,8 },     /* 18 */
  { 6,2 },     /* 19 */
  { 6,3 },     /* 20 */
  { 6,4 },     /* 21 */
  { 6,4 },     /* 22 */
  { 6,6 },     /* 23 */
  { 8,2 },     /* 24 */
  { 8,4 },     /* 25 */
  { 8,4 },     /* 26 */
  { 8,4 },     /* 27 */
  { 8,6 },     /* 28 */
  { 8,8 },     /* 29 */
  { 5,1 },     /* 30 */
  { 5,2 },     /* 31 */
  { 5,3 },     /* 32 */
  { 5,4 },     /* 33 */
  { 5,5 },     /* 34 */
  { 5,1 },     /* 35 */
  { 5,2 },     /* 36 */
  { 5,3 },     /* 37 */
  { 5,4 },     /* 38 */
  { 5,5 },     /* 39 */
  { 6,1 },     /* 40 */
  { 6,2 },     /* 41 */
  { 6,3 },     /* 42 */
  { 6,4 },     /* 43 */
  { 6,5 },     /* 44 */
  { 6,6 },     /* 45 */
};


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      grlc_get_data_rate
===
===  DESCRIPTION
===
===    calculate the  data rate based on num_timeslots,payload assigned.
===
===
===  PARAMS
===
===    number_of_ts and payload_len
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    data_rate in kbps ( 50 radio blocks / sec , 1 byte = 8 bits)
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
static uint32 grlc_get_data_rate
(
  uint16 num_timeslots,
  uint16 payload_len
)
{
  return (num_timeslots * payload_len  * 50 * 8);

} /* grlc_get_data_rate */

/*===========================================================================
===
===  FUNCTION      grlc_current_get_tx_data_rate
===
===  DESCRIPTION
===
===    calculate the current uplink tx data rate based on num_timeslots,
===    coding scheme assigned.
===
===
===  PARAMS
===
===    none
===
===  DEPENDENCIES
===
===
===    ul,dl structures from GRLC module
===
===  RETURN VALUE
===
===    data_rate in kbps ( 50 radio blocks / sec , 1 byte = 8 bits)
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
static uint32 grlc_current_get_tx_data_rate ( gas_id_t gas_id )
{
  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    return
      grlc_get_data_rate(ul[gas_id].cur_res.num_timeslots,
                         (ul[gas_id].cur_res.cs > RM_MCS_6) ?
                         (uint16)(2 * E_GRLC_DATA_BLOCK_LEN[ul[gas_id].cur_res.cs]) :
                         (uint16)(E_GRLC_DATA_BLOCK_LEN[ul[gas_id].cur_res.cs])
                        );
  }
  else
  {
    return
      grlc_get_data_rate(ul[gas_id].cur_res.num_timeslots,
                         (GRLC_DATA_BLOCK_LEN[ul[gas_id].cur_res.cs] - RM_RLC_HDR_LEN)
                        );
  }

} /* grlc_current_get_tx_data_rate */

/*===========================================================================
===
===  FUNCTION      grlc_current_get_rx_data_rate
===
===  DESCRIPTION
===
===    calculate the current downlink rx data rate based on num_timeslots,
===    coding scheme assigned.
===
===
===  PARAMS
===
===    none
===
===  DEPENDENCIES
===
===
===    ul,dl structures from GRLC module
===
===  RETURN VALUE
===
===    data_rate in kbps ( 50 radio blocks / sec , 1 byte = 8 bits)
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
static uint32 grlc_current_get_rx_data_rate( gas_id_t gas_id )
{
  if ( dl[gas_id].tbf_mode == TBF_MODE_EGPRS)
  {
    return
      grlc_get_data_rate(gmac_access_func_get_num_dl_timeslots(gas_id),
                         (dl[gas_id].cs > RM_MCS_6) ?
                         (uint16)(2 * E_GRLC_DATA_BLOCK_LEN[dl[gas_id].cs]) :
                         (uint16)(E_GRLC_DATA_BLOCK_LEN[dl[gas_id].cs])
                        );
  }
  else
  {
    return
      grlc_get_data_rate(gmac_access_func_get_num_dl_timeslots(gas_id),
                         (GRLC_DATA_BLOCK_LEN[dl[gas_id].cs] - RM_RLC_HDR_LEN)
                        );
  }

} /* grlc_current_get_rx_data_rate */


/*****************************************************************************
***
***     Public Function
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GERAN_GET_BEARER_DATA_RATES
===
===  DESCRIPTION
===
===    Provides APIs for querying bearer data rates from the GSM/GPRS Access Stratum.
===    The maximum data bearer rates are based on the maximum MS multislot class
===    capability and these values do not reflect the current channel conditions
===
===    The current data bearer rates are based on the last uplink/downlink allocations
===    received from the NW. Zeroes are returned when transfer is not active.
===
===    This function only return values in the parameter p_data_rates when invoked but
===    it provides no mechanism for reporting changes
===
===    data_rate in kbps ( 50 radio blocks / sec , 1 byte = 8 bits)
===
===  PARAMS
===
===    p_data_rates: Pointer to geran_bearer_data_rate_t containing returning values
===
===  DEPENDENCIES
===
===    RR/GRR Public Store services
===    ul,dl structures from GRLC module
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void geran_get_bearer_data_rates
(
  geran_bearer_data_rate_t  *p_data_rates
)
{
  geran_get_bearer_data_rates_as_id(p_data_rates, SYS_MODEM_AS_ID_1);
}

void geran_get_bearer_data_rates_as_id
(
  geran_bearer_data_rate_t  *p_data_rates,
  sys_modem_as_id_e_type as_id
)
{
  boolean             egprs_capable_cell = FALSE;
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id); 

  /* Read the NW TBF MODE cell capability using the RR public store.
  */
  rr_read_public_store(RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id);

  /* Read the MAX multislot class for EGPRS
  */
  rr_read_public_store(RRPS_MS_EGPRS_MULTISLOT_CLASS, &sysinfo[gas_id].max_egprs_multislot_class, gas_id);

  /* calculating max tx and rx data rate;
  ** current uplink/downlink data rate based on cs,ts,nw support for gprs/egprs.
  */
  if (egprs_capable_cell == TRUE)
  {
    p_data_rates->max_rx_bearer_data_rate = (int32)
      grlc_get_data_rate(max_data_rate[sysinfo[gas_id].max_egprs_multislot_class][0],
                          (uint16)(E_GRLC_DATA_BLOCK_LEN[RM_MCS_9]*2) );

    p_data_rates->max_tx_bearer_data_rate = (int32)
      grlc_get_data_rate(max_data_rate[sysinfo[gas_id].max_egprs_multislot_class][1],
                          (uint16)(E_GRLC_DATA_BLOCK_LEN[RM_MCS_9]*2) );
  }
  else
  {
    /* Read the MAX multislot class for GPRS
    */
    rr_read_public_store(RRPS_MS_MULTISLOT_CLASS, &sysinfo[gas_id].max_gprs_multislot_class, gas_id);

    p_data_rates->max_rx_bearer_data_rate = (int32)
      grlc_get_data_rate(max_data_rate[sysinfo[gas_id].max_gprs_multislot_class][0],
                          (uint16)(GRLC_DATA_BLOCK_LEN[RM_CS_4] - RM_RLC_HDR_LEN));

    p_data_rates->max_tx_bearer_data_rate = (int32)
      grlc_get_data_rate(max_data_rate[sysinfo[gas_id].max_gprs_multislot_class][1],
                          (uint16)(GRLC_DATA_BLOCK_LEN[RM_CS_4] - RM_RLC_HDR_LEN));
  }


  if ( ul[gas_id].state == US_ACK_XFER || ul[gas_id].state == US_UNACK_XFER )
  {
    p_data_rates->current_tx_bearer_data_rate =
      (int32)grlc_current_get_tx_data_rate(gas_id);
  }
  else
  {
    p_data_rates->current_tx_bearer_data_rate = 0;
  }

  /* current downlink data rate bases on cs,ts */
  if ( dl[gas_id].state == DS_RX_ACK || dl[gas_id].state == DS_RX_UNACK )
  {
    p_data_rates->current_rx_bearer_data_rate =
      (int32)grlc_current_get_rx_data_rate(gas_id);
  }
  else
  {
    p_data_rates->current_rx_bearer_data_rate = 0;
  }

} /* geran_get_bearer_data_rates */


/*===========================================================================
===
===  FUNCTION      GERAN_GET_MULTISLOT_CLASS_INFO
===
===  DESCRIPTION
===
===    This function provides an API for querying supported GPRS and EGPRS
===    multislot classes.
===
===  PARAMS
===
===    p_multislot_classes: Pointer to a geran_multislot_classes_t containing
===    returning values.
===
===    IMPORTANT:
===    Return value is invalid if parameters gprs_msc, egprs_msc, dtm_msc,
===    gprs_hmsc, or egprs_hsmc of structure pointed to by p_multislot_classes
===    contain value 0xFF.
===
===  DEPENDENCIES
===
===    RR/GRR Public Store services
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void geran_get_multislot_class_info
(
  geran_multislot_classes_t *p_multislot_classes
)
{
  geran_get_multislot_class_info_as_id(p_multislot_classes, SYS_MODEM_AS_ID_1);
}

void geran_get_multislot_class_info_as_id
(
  geran_multislot_classes_t *p_multislot_classes,
  sys_modem_as_id_e_type as_id
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id); 

  /* Initialise input parameters with invalid values. */
  p_multislot_classes->gprs_msc   = 0xFF;
  p_multislot_classes->egprs_msc  = 0xFF;
  p_multislot_classes->dtm_msc    = 0xFF;
  p_multislot_classes->gprs_hmsc  = 0xFF;
  p_multislot_classes->egprs_hsmc = 0xFF;


  /* Read the MAX multislot class for GPRS
  */
  rr_read_public_store(RRPS_MS_MULTISLOT_CLASS,
                       &sysinfo[gas_id].max_gprs_multislot_class, gas_id);

  p_multislot_classes->gprs_msc =
    (uint8) sysinfo[gas_id].max_gprs_multislot_class;


  /* Read the MAX multislot class for EGPRS
  */
  rr_read_public_store(RRPS_MS_EGPRS_MULTISLOT_CLASS,
                       &sysinfo[gas_id].max_egprs_multislot_class, gas_id);
  p_multislot_classes->egprs_msc =
    (uint8) sysinfo[gas_id].max_gprs_multislot_class;


#ifdef FEATURE_GSM_DTM
  /* Read the MAX multislot sub class for DTM
  */
  rr_read_public_store(RRPS_MS_DTM_EGPRS_MULTISLOT_SUBCLASS,
                       &sysinfo[gas_id].max_dtm_egprs_multislot_subclass, gas_id);
  p_multislot_classes->dtm_msc =
    (uint8) sysinfo[gas_id].max_dtm_egprs_multislot_subclass;
#endif /* FEATURE_GSM_DTM */


#ifdef FEATURE_GSM_GPRS_MSC33
  /* Read the MAX multislot class for GPRS HMSC
  */
  rr_read_public_store(RRPS_MS_HMSC_GPRS_CODED_MS_CLASS,
                       &sysinfo[gas_id].max_hmsc_gprs_multislot_class, gas_id);
  p_multislot_classes->gprs_hmsc =
    (uint8) sysinfo[gas_id].max_hmsc_gprs_multislot_class;

  /* Read the MAX multislot class for EGPRS HMSC
  */
  rr_read_public_store(RRPS_MS_HMSC_EGPRS_CODED_MS_CLASS,
                       &sysinfo[gas_id].max_hmsc_egprs_multislot_class, gas_id);
  p_multislot_classes->gprs_hmsc =
    (uint8) sysinfo[gas_id].max_hmsc_gprs_multislot_class;
#endif /* FEATURE_GSM_GPRS_MSC33 */

} /* geran_get_multislot_class_info */

/*===========================================================================
===
===  FUNCTION      GERAN_GET_cost_of_modem_power
===
===  DESCRIPTION
===
===    This function provides an API for querying cost of modem power
===
===  PARAMS
===
===    NONE
===
===    IMPORTANT:
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    geran_cost_of_modem_power_t enum
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
geran_cost_of_modem_power_t  geran_get_cost_of_modem_power (void)
{
  return NA_POWER;
}
