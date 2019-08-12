/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   

                ( G S M   F A X   C O M M A N D S )

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the gsm fax
  commands.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatgsmfax_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX

#include "ds3gsiolib_ex.h"
#include "dsati.h"
#include "dsatdl.h"
#include "dsatgsmfax.h"
#include "dsatparm.h"
#include "err.h"
#include "amssassert.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#include "dsatetsictab.h"
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#include "dsfps.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

#ifdef FEATURE_MULTIPROCESSOR
/*===========================================================================

FUNCTION DSATGSMFAX_SET_STACK_PARAMS

DESCRIPTION
  This function updates the fax protocol stack parameters stored in
  the T.31 layer.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void dsatgsmfax_set_stack_params( void )
{
  dsfps_params_type  fax_params;
  dsat_num_item_type temp_val ;
  /* Update the fax protocol stack with current values */
  
  temp_val =(dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_FDD_IDX,0,0,NUM_TYPE);
  fax_params.fdd = (uint16)temp_val;

  temp_val =(dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_FCL_IDX,0,0,NUM_TYPE);
  fax_params.fcl = (uint16)temp_val;
  
  temp_val= (dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_ACT_FTS_ETSI_IDX,0,0,NUM_TYPE);
  fax_params.fts = (uint16)temp_val;

  temp_val = (dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_ACT_FRS_ETSI_IDX,0,0,NUM_TYPE);
  fax_params.frs = (uint16)temp_val;
  
  temp_val =(dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_FIT_IDX,0,0,NUM_TYPE);
  fax_params.fit[0] = (uint16)temp_val;

  temp_val =(dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_FIT_IDX,0,1,NUM_TYPE);
  fax_params.fit[1] = (uint16)temp_val;
    
  /* Invoke the fax protocol stack interface */
  dsfps_set_at_stack_params( &fax_params );
} /* dsatgsmfax_set_stack_params() */
#endif /* ( FEATURE_MULTIPROCESSOR ) */

#endif  /* FEATURE_DATA_GCSD_FAX */

