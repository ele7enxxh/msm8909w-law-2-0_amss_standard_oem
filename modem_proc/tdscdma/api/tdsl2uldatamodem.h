#ifndef TDSL2ULDATAMODEM_H
#define TDSL2ULDATAMODEM_H
/*===========================================================================

                    TDS L2 UL DATAMODEM  HEADER FILE



DESCRIPTION

  This file contains L2 UL data structures, variables, enums and function prototypes

  needed to be used by the Data Modem  


Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.

QUALCOMM Proprietary.  Export of this technology or software is regulated

by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/10   ps       Initial Release
===========================================================================*/

#include "tdsl2external.h"

/*===========================================================================

                     TYPE DEFINITIONS FOR MODULE

===========================================================================*/

/* Command id for the commands that Data Modem sends to l2 UL task*/
typedef enum
{

  TDSRLC_UL_MODEMDATA_REG_DEREG_SRVC_REQ, /* Register -Deregister Uplink Watermark */

  TDSRLC_UL_DATAMODEM_OPEN_RX_WIN,            /* Open the Receiving Window. */

  TDSRLC_UL_DATAMODEM_CLOSE_RX_WIN,           /* Close the Receiving Window.  */

  TDSL2_UL_DATAMODEM_MAX_CMDS              /* Number of Ul L2 Command types. must be last entry */

} tdsl2_ul_datamodem_api_enum_type;




/* Downlink Layer 2 API data type for the API  that Data Modem calls l2 UL  */
typedef struct
{
  tdsl2_ul_datamodem_api_enum_type   api_id ;

  /* API id for the APIs that Data Modem calls to l2 UL */
   union
  {
  
    
    tdsl2_ul_service_register_type ul_reg_dereg_cmd;
  
    uint8              ul_rlc_data_id;
  
  }api_data;

}tdsl2_ul_datamodem_api_type;

/*===========================================================================

                     -FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION:        tdsl2_ul_submit_datamodem_msg

DESCRIPTION:
  This function should be called by the DATA MODEM to send a command
  to the L2 UL task
  The pointer for cmd_ptr passed  should be freed by the caller.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  boolean TRUE if succes else FALSE.

SIDE EFFECTS:
  The pointer for cmd_ptr passed  should be freed by the caller.
  
  Command will be enqueued in L2-UL task queue and a signal will be set 
  to indicate that the item has been enqueued.

===========================================================================*/


boolean tdsl2_ul_submit_datamodem_msg
(
  tdsl2_ul_datamodem_api_type * cmd_ptr
);

#endif
