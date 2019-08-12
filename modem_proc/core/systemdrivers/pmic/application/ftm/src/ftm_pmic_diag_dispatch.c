/*! \file 
 *  \n
 *  \brief  ftm_pmic_diag_dispatch.c
 *  \details  This file contains function definitions for diag dispatchers
 *			  to register PMIC FTM interfaces.
 *
 *  \n &copy; Copyright (c) 2012  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/ftm/src/ftm_pmic_diag_dispatch.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#include "ftm_pmic.h"
#include "ftm_pmic_diag_dispatch.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "pm.h"

/*============================================================================
                        INTERNAL VARIABLES DEFINITIONS
============================================================================*/
static const diagpkt_user_table_entry_type 
ftm_pmic_diag_func_table [] =
{
  {                  
	FTM_PMIC_C ,
    FTM_PMIC_C ,
    ftm_pmic_dispatch 
  }
};

static boolean ftm_pmic_diag_registered = FALSE;

/*============================================================================
                        EXTERNAL API DEFINITIONS
============================================================================*/
/*===========================================================================

FUNCTION FTM_PMIC_DIAG_INIT

DESCRIPTION
This function is called to register the PMIC diagnostic message type.  
It will associate the PMIC diagnostic message with a function call.  
Each time a PMIC diagnostic message is encountered the specified function 
will be called.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS

   
===========================================================================*/
void ftm_pmic_diag_init()
{
  if (!ftm_pmic_diag_registered)
  {
    ftm_pmic_diag_registered = TRUE;
    DIAGPKT_DISPATCH_TABLE_REGISTER ( DIAG_SUBSYS_FTM ,
                                      ftm_pmic_diag_func_table
                                    );
  }
}


/*===========================================================================

FUNCTION FTM_PMIC_DISPATCH

DESCRIPTION
This function dispatches the appropriate PMIC function for the requested
operation.  In this case the PMIC sub command is treated as 
diagnostic payload.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
PACKED void *  ftm_pmic_dispatch ( PACKED void * request , uint16 length )
{
	ftm_pmic_pkt_type* req_pkt = (ftm_pmic_pkt_type *) request;

    switch ( req_pkt->ftm_cmd_header.cmd_id )
    {
        case FTM_PMIC_TEST_RTC:
            return ftm_pmic_process_rtc_commands(request, length);
            break;
        case FTM_PMIC_TEST_MPP:
            return ftm_pmic_process_mpp_commands(request, length);
            break;
		case FTM_PMIC_TEST_GPIO:
			return ftm_pmic_process_gpio_commands(request, length);
			break;
		default:
		   break;
    }

    return NULL;
} 
