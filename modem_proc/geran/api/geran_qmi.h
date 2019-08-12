#ifndef GRLCQMI_H
#define GRLCQMI_H

/*============================================================================
  @file GRLCQMI.H

  GRLC QMI Interface Definition

  FUNCTION SUMMARY

    geran_get_bearer_data_rates()

      Returns data structure comprising the maximum and current GERAN data
      bearer rates

  Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_qmi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
01/09/08   hv      Created header file
           abz?    Added ...

===========================================================================*/



/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "sys.h"


/*****************************************************************************
***
***     Public type defines & functions
***
*****************************************************************************/

typedef struct
{
  /* Max theoretical tx and rx dara rates in bps - assuming the highest coding
  ** scheme and/or maximum timeslot allocation from NW, given the MS multislot
  ** class capability
  */
  int32 max_tx_bearer_data_rate;
  int32 max_rx_bearer_data_rate;

  /* Current tx and rx data rates in bps based on the last NW allocations.
  ** Zeroes are returned if transfer is not active
  */
  int32 current_tx_bearer_data_rate;
  int32 current_rx_bearer_data_rate;

} geran_bearer_data_rate_t;


typedef struct
{
  uint8   gprs_msc;
  uint8   egprs_msc;
  uint8   dtm_msc;
  uint8   gprs_hmsc;
  uint8   egprs_hsmc;
} geran_multislot_classes_t;

/* Cost of modem power designations
*/
typedef enum
{
  LOW_POWER,
  MED_POWER,
  HIGH_POWER,
  NA_POWER
} geran_cost_of_modem_power_t;


/*****************************************************************************
***
***     Public Functions
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

extern void geran_get_bearer_data_rates
(
  geran_bearer_data_rate_t *p_data_rates
);

extern void geran_get_bearer_data_rates_as_id
(
  geran_bearer_data_rate_t *p_data_rates,
  sys_modem_as_id_e_type as_id
);

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
extern void geran_get_multislot_class_info
(
  geran_multislot_classes_t *p_multislot_classes
);

extern void geran_get_multislot_class_info_as_id
(
  geran_multislot_classes_t *p_multislot_classes,
  sys_modem_as_id_e_type as_id
);

extern geran_cost_of_modem_power_t  geran_get_cost_of_modem_power (void);

#endif /* GERAN_QMI_H */
