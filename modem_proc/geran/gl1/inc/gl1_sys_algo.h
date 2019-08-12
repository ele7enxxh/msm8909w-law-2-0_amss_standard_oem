#ifndef GL1_SYS_ALGO_H
#define GL1_SYS_ALGO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                G S M   L 1   S Y S T E M S   A L G O R I T H M S

DESCRIPTION
   GSM/GPRS Layer 1 systems algorihms type declarations.


Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1_sys_algo.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
16/12/14   jk       CR753423:LIF for Idle,voice and data
10/07/14   jk      CR688587:GL1 changes to support LIF feature
19/12/13   jk       CR503359 - GL1 changes to support Rx diversity
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
24/07/09   ws       CR 180476 - Improve performance of Dedicated AGC
01/19/09   rc       Moved INVALID_ARFCN and ARFCNS_EQUAL in to gprsdef.h
06/25/04   gfr      Remove obselete code.
11/05/03   gfr      Support for quad-band.
08/16/02   JC       Added ARFCN to AGC_T structure to ease debug.
05/21/02   gw       Added parameter to sys_algo_agc_get_next_index
                    to indicate when all ARFCNs are done.
05/10/02   JC       Added sys_algo_agc_configure_max_attempts_per_MP
                    API so that the number of attempts can be cfg
                    dynamically.
03/27/02   JC       Modified API to sys_algo_agc_get_next_index
                    so that it accepts a list of pointers to AGC_T
                    vs a pointer to a list of AGC_T's.  This removes
                    the constraint of having all of the AGC_T's
                    being contigous in memory (scattered). Also,
                    added SRL flag in AGC_T struct so that ncell doesn't
                    interfere with serving cell. Also, added select fcn
                    headers.
02/15/02   JC       Added slew_rate_enable/disable interfaces.
02/04/02   JC       Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_type.h"
#include "gprsdef.h"
#include "gprsdef_g.h"


typedef struct
{
  boolean    first_visit;   /* Is this the first time that the AGC is invoked for some freq */
  dBx16_T    pwr_dBm_x16;   /* (1/16 dB)/lsb */
  boolean    valid;         /* in range result that can be trusted? */
  boolean    srl_enabled;   /* is slew rate limiting enabled on this AGC */
  uint32     time_stamp;    /* used to determine interval between visits */

  /* The next two fields are used for debug only */
  uint32     FN;            /* frame number of last update */
  ARFCN_T    arfcn;         /* channel of last update */
} sys_algo_agc_T;

/* Defines for AGC burst Rxlev averaging */
#define TWO_BURST_AVG 2
#define FOUR_BURST_AVG 4


/* Declare a null arfcn for situations when one is needed */
extern ARFCN_T NULL_ARFCN;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  sys_algo_agc_init

DESCRIPTION
  This function initializes the AGC data structure.

===========================================================================*/
void sys_algo_agc_init(sys_algo_agc_T *agc_ptr, dBx16_T init_value);


/*===========================================================================

FUNCTION  sys_algo_agc_update

DESCRIPTION
  This function provides update information to the power predictor control
  algorithm.

===========================================================================*/
void sys_algo_agc_update
(
   sys_algo_agc_T     *agc_ptr,
   dBx16_T            pwr_dBm_x16,
   boolean            in_range,
   uint32             time_stamp,
   gas_id_t           gas_id
);


/*===========================================================================

FUNCTION  sys_algo_agc_enable_slew_rate_limiting

DESCRIPTION
  This function enables the slew rate limiting function of the power
  predictor algorithm.

===========================================================================*/
void sys_algo_agc_enable_slew_rate_limiting( sys_algo_agc_T *agc_ptr );


/*===========================================================================

FUNCTION  sys_algo_agc_disable_slew_rate_limiting

DESCRIPTION
  This function disables the slew rate limiting function of the power
  predictor algorithm.

===========================================================================*/
void sys_algo_agc_disable_slew_rate_limiting( sys_algo_agc_T *agc_ptr );

/*===========================================================================

FUNCTION  SYS_ALGO_AGC_AVERAGE_OUTPUT

DESCRIPTION
  This function averages the output of the AGC over MAX_AVG_BURST_LEN bursts

DEPENDENCIES
  None

RETURN VALUE
  int16 rxlev

SIDE EFFECTS
  None

===========================================================================*/
dBx16_T sys_algo_agc_average_output(dBx16_T agc_output, gas_id_t gas_id);

/*===========================================================================

FUNCTION  SYS_ALGO_AGC_AVG_INIT

DESCRIPTION
  This function initializes the AGC running average data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sys_algo_agc_avg_init(uint16 avg_len, gas_id_t gas_id );

#ifdef FEATURE_GSM_RX_DIVERSITY
dBx16_T sys_algo_agc_average_output_divrx(dBx16_T agc_output,gas_id_t gas_id);
#endif /*FEATURE_GSM_RX_DIVERSITY*/

#endif /* GL1_SYS_ALGO_H */
