#ifndef HDRSRCHMCPM_H
#define HDRSRCHMCPM_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R   S R C H    M C P M    F U N C T I O N S

GENERAL DESCRIPTION
  This module acts as a thin layer between HDRSRCH and Modem Clock and Power
  Manager ( MCPM ).  It consolidates SRCH state information, peak UL/DL data
  rates and diversity information and conveys it to MCPM.

EXTERNALIZED FUNCTIONS
  hdrsrchmcpm_init
    Initializes SRCH-MCPM struct.
 
  hdrsrchmcpm_set_state
    Sets HDR state to be conveyed to MCPM.
 
  hdrsrchmcpm_set_diversity_status
    Sets diversity on/off status to be conveyed to MCPM.
 
  hdrsrchmcpm_set_equalizer_status
    Sets equalizer on/off status to be conveyed to MCPM.
 
  hdrsrchmcpm_set_subtype
    Sets peak UL/DL data rates to be conveyed to MCPM.
 
  hdrsrchmcpm_set_mcdo_status
    Sets BW info to be conveyed to MCPM.
 
  hdrsrchmcpm_set_enh_rates_status
    Sets DL rates to be conveyed to MCPM based on enhanced rates
    enable/disable status.
 
  hdrsrchmcpm_do2lte_measurement
    Sets DO to LTE measurement status to be conveyed to MCPM.
 
  Copyright (c) 2011 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchmcpm.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
06/05/14   rmv     Delay setting MCPM state to HDRSRCHMCPM_STOP_TX_REQ, until 
                   searcher traffic exit in MCDO case 
02/04/14   ljl     Requested MCPM_DO_PARMS_UPDATE_REQ instead of 
                   MCPM_DO_GO_TO_SLEEP_REQ when HDR was in sleep already.
                   Requested MCPM_DO_PARMS_UPDATE_REQ instead of MCPM_DO_NOP_REQ.
06/05/12   kss     Removed hdrsrchmcpm_reset_tx().
04/26/12   rmg     Added hdrsrchmcpm_reset_tx().
07/06/11   rmg     Set MCPM state change req field to NOP for param updates.
05/19/11   rmg     Updated HDR MCPM interface.
04/27/11   rmg     Created.

==========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "mcpm_api.h"
#include "sys.h"


/* DO MCPM states type */
typedef enum
{
  HDRSRCHMCPM_POWERDOWN_ST = 0,
  HDRSRCHMCPM_IDLE_RX_ST,
  HDRSRCHMCPM_SLEEP_ST,
  HDRSRCHMCPM_DATA_ST,
  HDRSRCHMCPM_MAX_ST = HDRSRCHMCPM_DATA_ST,
  HDRSRCHMCPM_INVALID_ST

} hdrsrchmcpm_state_type;


/* DO MCPM request type */
typedef enum
{
  HDRSRCHMCPM_START_REQ = 0,
  HDRSRCHMCPM_STOP_REQ,
  HDRSRCHMCPM_WAKE_UP_REQ,
  HDRSRCHMCPM_GO_TO_SLEEP_REQ,
  HDRSRCHMCPM_START_TX_REQ,
  HDRSRCHMCPM_STOP_TX_REQ,
  HDRSRCHMCPM_PARMS_UPDATE_REQ,
  HDRSRCHMCPM_MAX_REQ = HDRSRCHMCPM_PARMS_UPDATE_REQ,
  HDRSRCHMCPM_INVALID_REQ

} hdrsrchmcpm_request_type;

/*============================================================================

FUNCTION HDRSRCHMCPM_INIT

DESCRIPTION
  Initializes SRCH-MCPM structure.  To be done at power up.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_init( void );



/*============================================================================

FUNCTION HDRSRCHMCPM_SET_STATE

DESCRIPTION
  Conveys DO state change request to MCPM and stores it internally.
 
PARAMETERS 
  req  - DO state change request to be stored and conveyed to MCPM module

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_set_state
(
  hdrsrchmcpm_request_type   req
    /* SRCH-MCPM state change request */
);



/*============================================================================

FUNCTION HDRSRCHMCPM_SET_DIVERSITY_STATUS

DESCRIPTION
  Conveys DO Rx diversity on/off status to MCPM and stores it for future MCPM
  communication.
 
PARAMETERS 
  div_status  -  TRUE  => Diversity ON
                 FALSE => Diversity OFF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_set_diversity_status
(
  boolean        div_status
    /* Diversity status */
);



/*============================================================================

FUNCTION HDRSRCHMCPM_SET_EQUALIZER_STATUS

DESCRIPTION
  Conveys equalizer on/off status to MCPM and stores it for future MCPM
  communication.
 
PARAMETERS 
  eq_status  -  TRUE  => Equalizer ON
                FALSE => Equalizer OFF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_set_equalizer_status
(
  boolean        eq_status
    /* Equalizer status */
);



/*============================================================================

FUNCTION HDRSRCHMCPM_SET_SUBTYPE

DESCRIPTION
  Stores DO subtype and notifies peak up-link/down-link rates for the subtype
  to MCPM.
 
PARAMETERS 
  subtype  -  DO protocol subtype

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_set_subtype
(
  sys_active_prot_e_type subtype
    /* DO protocol subtype */
);



/*============================================================================

FUNCTION HDRSRCHMCPM_SET_MCDO_STATUS

DESCRIPTION
  Conveys if DO is operating in multicarrier mode to MCPM and stores it for
  future MCPM communication.
 
PARAMETERS 
  mcdo_status  -  TRUE  => DO is operating in multicarrier mode
                  FALSE => DO is operating in single carrier mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_set_mcdo_status
(
  boolean        mcdo_status
    /* TRUE if more than one carriers are assigned */
);

/*============================================================================

FUNCTION HDRSRCHMCPM_GET_MCDO_STATUS

DESCRIPTION
  Get if DO is operating in multicarrier mode.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE 
  TRUE  => DO is operating in multicarrier mode
  FALSE => DO is operating in single carrier mode 

SIDE EFFECTS
  None.

============================================================================*/

boolean hdrsrchmcpm_get_mcdo_status( void );


/*============================================================================

FUNCTION HDRSRCHMCPM_SET_ENH_RATES_STATUS

DESCRIPTION
  Sets enhanced rates status and conveys updated FL data rates to MCPM.
 
PARAMETERS 
  enh_rates_enabled  -  TRUE  => Enhanced rates enabled
                        FALSE => Enhanced rates disabled

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_set_enh_rates_status
(
  boolean        enh_rates_enabled
    /* TRUE if enhanced rates are enabled */
);



/*============================================================================

FUNCTION HDRSRCHMCPM_DO2LTE_MEASUREMENT

DESCRIPTION
  Conveys DO to LTE measurement start / stop status to MCPM and stores it
  for future MCPM communication.
 
PARAMETERS 
  status  -  TRUE  => DO to LTE measurement start
             FALSE => DO to LTE measurement stop

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_do2lte_measurement
(
  boolean        status
    /* DO to LTE measurement status */
);

/*============================================================================

FUNCTION HDRSRCHMCPM_EXTEND_SLEEP

DESCRIPTION
  Update MCPM that sleep yim has changed.
 
PARAMETERS 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/

void hdrsrchmcpm_extend_sleep( void );

#endif /* HDRSRCHMCPM_H */
