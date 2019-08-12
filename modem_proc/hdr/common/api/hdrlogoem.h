#ifndef HDRLOGOEM_H
#define HDRLOGOEM_H


/** 
  @file hdrlogoem.h
  @brief
  Contains function prototypes to copy and pass the debug display
  structure to the UI.
*/


/*===========================================================================

Copyright (c) 2010-2012 by Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrlogoem.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/12   smd     Moved it from modem/hdr/api.
09/10/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
07/19/10   cnx     Documented the public interfaces
02/18/09   wsh     Made hdrlogoem an ext-AU module
05/18/04   ds      Added the DEBUG_DISPLAY_OEM, INFO function prototypes
05/14/04   ds      Created this file

===========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "comdef.h"

/** @addtogroup hdr_api High Data Rate API
    @{ */

/** Maximum number of application streams supported by the 
    access terminal.
*/
#define HDRLOG_NUM_STREAMS 4

/**
  @brief Sends HDR debug screen information to the UI.
 
  This structure contains the exact same entries as those in
  hdrlog_debug_display_logging_struct_subtype, but with bit fields removed.
  Reason for creation: Bit fields cannot be exported over Remote Procedure
  Call (RPC).
*/
typedef struct
{

  struct
  {
    uint16 chan_num;            /**< Channel number. */
    uint8  band_class;          /**< Band class. */

  } hdr_freq;

  int8 rx_agc0;                 /**< Receive signal strength chain0 in dBm. */
  int8 rx_agc1;                 /**< Receive signal strength chain1 in dBm. */
  int8 tx_agc;                  /**< Connected state Tx signal strength 
                                     in dBm. */

  uint16 serving_pn;            /**< PN offset of serving ASET pilot. */
  uint16 aset_pn[6];            /**< ASET pilot PN offsets. */
  uint8  sleep_mode;            /**< HDR Sleep mode. Possible values are:
                                     - 0 -- Synchronous Control Channel (SCC)
                                     - 1 -- Common Control Channel (CCC)
                                     - 2 -- Long
                                     - 3 -- Until reacquisition handoff
                                     - 4 -- Release A idle */

  struct
  {
    uint8 rel0_sci;             /**< Release 0 Slot Cycle Index. Possible values
                                     are:
                                     - 0 -- 5.12s
                                     - 1 -- 10.24s
                                     - 2 -- 20.48s
                                     - 3 -- 40.96s
                                     - 4 -- Release A SCI */
    uint8 relA_sci;             /**< Release A Slot Cycle Index. */

  } sci;

  uint8 srch_state;             /**< HDR searcher state. */
  int8 rx_div;                  /**< HDR receive diversity:   
                                     - 0 -- Rx0 only 
                                     - 1 -- Diversity enabled 
                                            (both Rx0 and Rx1) */

  uint8 prot_state;             /**< HDR protocol state:
                                     - 0 -- Inactive
                                     - 1 -- Acquistion        
                                     - 2 -- Sync
                                     - 3 -- Idle
                                     - 4 -- Access
                                     - 5 -- Connected */

  uint8 hdr_session_state;      /**< HDR session state:                  
                                     - 0 -- Inactive
                                     - 1 -- Address Management Protocol (AMP)
                                            setup state
                                     - 2 -- AT initiated
                                     - 3 -- AN initiated
                                     - 4 -- Open state */

  struct
  {
    uint32 uati24;              /**< The 24 least significant bits of 
                                     the Unicast Access Terminal 
                                     Identifier (UATI). */
    uint8  color_code;          /**< Color code associated with the 
                                     subnet to which the UATI belongs. */
  } uati_info;

  uint16  stream_config[HDRLOG_NUM_STREAMS]; /**< Stream configuration. */

}
hdrlog_debug_display_struct_subtype;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


/*=========================================================================

FUNCTION     : HDRLOGOEM_DEBUG_DISPLAY_OEM

=========================================================================*/

/** 
  Sends the debug display record to the UI. It copies the latest debug
  display structure information into a structure to be used by the UI. 
	
  @param[in] ptr Pointer to the structure for the latest debug information.

  @return
      None.

  @dependencies
      None.
*/ 
void hdrlogoem_debug_display_oem 
( 
  hdrlog_debug_display_struct_subtype *ptr 
);

/** @} */ /* end_addtogroup hdr_api */

#endif /* HDRLOGOEM_H */
