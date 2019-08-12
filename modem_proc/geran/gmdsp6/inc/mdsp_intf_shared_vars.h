#ifndef MDSP_INTF_SHARED_VARS_H
#define MDSP_INTF_SHARED_VARS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L A Y E R  1  - M D S P   I N T E R F A C E
                    mDSP Shared Variables Access Functions

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


  Copyright (c) 2012 - 2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmdsp6/inc/mdsp_intf_shared_vars.h#1 $

when         who     what, where, why
----------   ------- ------------------------------------------------------
23/01/15     jk      CR788012:x2gRxfeConfigAlpha set to 30 for T2G and for other X2G set to 0
27/03/13     cja     CR468678 Set cmCleanupAlpha shared variable for FW for T2G
12-10-22     tjw     Initial version.
==========================================================================*/



/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "geran_dual_sim.h"

typedef enum
{
  GFWSHAREDVARIABLES_RX_RXALPHA,
  GFWSHAREDVARIABLES_RX_MONALPHA,
  GFWSHAREDVARIABLES_RX_X2GSTARTALPHA,
  GFWSHAREDVARIABLES_RX_CMCLEANUPALPHA,
  GFWSHAREDVARIABLES_EDGE_MSC,
  GFWSHAREDVARIABLES_EDGE_COVERITHRESH,
  GFWSHAREDVARIABLES_EDGE_COVERISIGMA,
  GFWREADVARIABLESSTRUCT_TCHNUMZEROSOFTDECS,
  GFWREADVARIABLESSTRUCT_GFWSLEEPSTATE,
  GFWREADVARIABLESSTRUCT_GSISRPROCDELAY,
  GFWREADVARIABLESSTRUCT_PWRMSRBURSTSIZE,
  GFWMETRICSBUFFERS,
  GFWSHAREDVARIABLES_RX_X2GRXFECONFIGALPHA
} MdspGeranSharedVariableType;

void mdsp_intf_shared_var_write(MdspGeranSharedVariableType sharedVariableType,  uint16 value , gas_id_t gas_id );
void * mdsp_intf_shared_var_read(MdspGeranSharedVariableType sharedVariableType , gas_id_t gas_id );

 #endif /* MDSP_INTF_SHARED_VARS_H */