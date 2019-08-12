#ifndef RFLM_TEST_H
#define RFLM_TEST_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    F T M  C O M M O N  D I S P A T C H

GENERAL DESCRIPTION
  This is the header file for the embedded FTM Commands that are common
  between 1x and UMTS.


Copyright (c) 2005 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/lm/inc/rxlm_test.h#1 $ $DateTime:

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
10/09/13   cvd     Initial Version
===========================================================================*/


#include "rxfe_map.h"
#include "lm_types.h"

void cfgRxfeBrdg(const tMemplBufferCfg *vbuf, uint32 wrtr, uint32 MemplOffset);

void configure_rxfe_bridge(lm_tech_type tech, uint32 nb0, uint32 nb1, uint32 nb2, uint32 nb3);

void enableRxfeBrdg(uint32 numWrtrs, uint32 nb0, uint32 nb1, uint32 nb2, uint32 nb3);

void cfgFbrxBrdg(const tMemplBufferCfg* vbuf, uint32 wrtr, uint32 MemplOffset);

boolean activate_chain(uint8 adc, uint8 wb, uint8 nb0, uint8 nb1, uint8 nb2, uint8 nb3);

#endif
