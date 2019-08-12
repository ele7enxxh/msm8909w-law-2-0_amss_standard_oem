#ifndef TDSL2DLDATAMODEM_H
#define TDSL2DLDATAMODEM_H

/*===========================================================================

                    TDS L2 DL DATAMODEM  HEADER FILE



DESCRIPTION

  This file contains L2 DL data structures, variables, enums and function prototypes

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

/* Structure for the Command that PDPCP sends for WM cleanup */
typedef  struct {
  void *cleanup_wm_para;

  uint8 lc_id;

  void  (*cleanup_wm_cb)( void *call_instance);

} tdsrlci_cleanup_wm_cmd_type;
  

/* API id for the APIs that Data Modem calls to l2 DL */
typedef enum
{

  TDSRLC_DL_MODEMDATA_REG_DEREG_SRVC_REQ, /* Register DeregisterDownlink Watermark */

  TDSRLC_DL_MODEMDATA_PDCP_CLEANUP_WM,

  TDSL2_DL_MODEMDATA_MAX_CMDS            /* Number of DL L2 Command types. must be last entry */

}tdsl2_dl_modemdata_api_enum_type;





/* API structures  that Data Modem calls to l2 DL task */
typedef struct
{
  tdsl2_dl_modemdata_api_enum_type    api_id;

  /* Downlink Layer 2 API data type for the API  that Data Modem calls l2 DL  */
  union
 {
 
  tdsl2_dl_service_register_type dl_reg_dereg_cmd;
  tdsrlci_cleanup_wm_cmd_type    cleanup_wm_cmd;
  
  }api_data;

}tdsl2_dl_datamodem_api_type;

/*===========================================================================

                     -FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION:        tdsl2_dl_submit_datamodem_msg

DESCRIPTION:
  This function should be called by the DATA MODEM to send a command
  to the L2 DL task
  
The pointer for cmd_ptr passed  should be freed by the caller.

DEPENDENCIES:
  None.

RETURN VALUE:
  boolean TRUCE if succes else FALSE.

SIDE EFFECTS:
  The pointer for cmd_ptr passed  should be freed by the caller.


  Command will be enqueued in L2-DL task queue and a signal will be set 
  to indicate that the item has been enqueued.

===========================================================================*/


boolean tdsl2_dl_submit_datamodem_msg
(
  tdsl2_dl_datamodem_api_type * cmd_ptr
);
#endif/*TDSL2DLDATAMODEM_H*/
