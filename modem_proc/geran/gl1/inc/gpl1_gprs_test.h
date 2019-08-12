/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      GPL1 LOOPBACK TEST MODE UTILITY FUNCTIONS

GENERAL DESCRIPTION

   This module provides utility functions for LOOPBACK TEST MODE B for GPRS and
   EGPRS as well as Switched Radio Block LoopBack mode (SRB)

EXTERNALIZED FUNCTIONS




INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_gprs_test.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
26/02/13   ws        CR455036 - X+G DSDA: Crash in GL1 when testing Test mode B loopback mode 
10-12-08   tjw       Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
#include "geran_variation.h"
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "grlculfifo.h"


/*===========================================================================

FUNCTION  GPL1_GPRS_GET_RLC_LOOP_DATA

DESCRIPTION
  This function Reads the loopback data out from the RLC UL FIFO for
  Transmission on the Uplink. It then deletes the data from the FIFO



DEPENDENCIES
  None

RETURN VALUE
  boolean - Success/faiure of retrieval of pointer for RLC UL FIFO
SIDE EFFECTS
  None

===========================================================================*/
extern boolean gpl1_gprs_get_rlc_loop_data(grlc_ul_fifo_access_block_t   *rlc_config_params, gas_id_t gas_id);

/*===========================================================================

FUNCTION  GPL1_GPRS_GET_LOOP_UL_DATA

DESCRIPTION
  This function copies the appropriate data to be transmitted on the uplink
  into a local structure. Transmitting RRBP's or receiving control data will
  result in either loopback data being deleted or previous data being resent.


DEPENDENCIES
  None

RETURN VALUE
  NULL

SIDE EFFECTS
  None

===========================================================================*/
extern void gpl1_gprs_get_loop_ul_data(gas_id_t gas_id);

/*===========================================================================

FUNCTION  GPL1_GPRS_SET_LOOP_B_DATA

DESCRIPTION

  The function Assigns a pointer to the data to be transmitted from the local
  strucure written to via gpl1_gprs_get_loop_ul_data().

  It also picks up the approprate MCS, PS or CS to use for the transmission

DEPENDENCIES
  None

RETURN VALUE
  pointer to the message to transmit

SIDE EFFECTS
  None

===========================================================================*/
extern void gpl1_gprs_set_loop_b_data(gl1_defs_coding_type *coding_scheme,
  gl1_defs_puncturing_type puncturing_scheme[],
  uint8 *msg_data[], gas_id_t gas_id);

/*===========================================================================

FUNCTION  GPL1_GPRS_IS_RLC_DL_DATA_DISC

DESCRIPTION

  The function determines what received RLC data needs to be discarded and what data
  should be passed up to RLC  in terms up number of blocks to be looped back on the Uplink
  The function considers various UL/DL Ts allocations.

DEPENDENCIES
  None

RETURN VALUE
  0: Discard RLC DL data
  num : Pass RLC data block into DL FIFO

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 gpl1_gprs_is_rlc_dl_data_disc(gl1_defs_tn_type rx_tn, gas_id_t gas_id);

/*===========================================================================

FUNCTION  GPL1_GPRS_SETUP_TEST_B

DESCRIPTION
  This function Determines which downlink timeslots need to be looped back
  on the uplink. This is done based on test_mode_tn which indicates the
  first burst to loopback.
DEPENDENCIES
  None

RETURN VALUE
  NULL
SIDE EFFECTS
  None

===========================================================================*/
extern void gpl1_gprs_setup_test_B(gas_id_t gas_id);

/*===========================================================================

FUNCTION  gpl1_l1_test_mode_start_ind( )

DESCRIPTION
  Posts MPH_L1_TEST_MODE_START_IND to RR.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void  gpl1_l1_test_mode_start_ind( gas_id_t gas_id );

/*===========================================================================

FUNCTION  gpl1_l1_test_mode_stop_ind( )

DESCRIPTION
  Posts MPH_L1_TEST_MODE_STOP_IND to RR.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void  gpl1_l1_test_mode_stop_ind( gas_id_t gas_id );
