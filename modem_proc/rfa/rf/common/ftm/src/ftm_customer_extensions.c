/*===========================================================================

   F T M    C U S T O M E R    E X T E N S I O N S    

DESCRIPTION
   This file contains a sample function protocal

REFERENCES
   None

Copyright (c) 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

Procedures on how to add ftm_customer_extensions.c/.h to project
- Add ftm_customer_extensions.c to ftm.min. (If the extension file is not included)
- Define FEATURE_FTM_CUSTOMER_EXTENSIONS in the build configuration
- Add FTM ENUM to ftm_custext_cmd_type in ftm_customer_extensions.h
- Add function to the ftm_customer_extensions.c
- Add function to the switch statement in ftm_customer_extensions_dispatch()

$Header: 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/07   jf      Provide more framework and comments
06/01/05   jf      Initial check in with sample function protocal
===========================================================================*/
#include "rfa_variation.h"
#include "ftm_customer_extensions.h"

#ifdef FEATURE_FTM_CUSTOMER_EXTENSIONS

#include "ftm_task.h" //FTM_MSG  
/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
void ftm_customer_extensions_sample( word packetLength, byte *data );
/*===========================================================================

FUNCTION FTM_CUSTOMER_EXTENSIONS_DISPATCH

DESCRIPTION
    This function handles Customer Extension FTM requests to run tests and
    other primitives by dispatching the appropriate functions.

DEPENDENCIES
    None.

RETURN VALUE
    Response packet.

SIDE EFFECTS
    None.

===========================================================================*/
ftm_rsp_pkt_type ftm_customer_extensions_dispatch(ftm_custext_pkt_type *req_pkt)
{     
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  switch (req_pkt->ftm_hdr.cmd_id)
	{
  //FTM sample function
  case FTM_CUSTEXT_SAMPLE:
    //Display messages to QXDM using FTM_MSG
    FTM_MSG_HIGH("Receive customer extension cmd_id: %d, len: %d, rsp_pkt_size: %d",
                req_pkt->ftm_hdr.cmd_id,req_pkt->ftm_hdr.cmd_data_len,req_pkt->ftm_hdr.cmd_rsp_pkt_size);

    //Call FTM sample function
    ftm_customer_extensions_sample( req_pkt->ftm_hdr.cmd_data_len, (byte*)req_pkt->data );
    break;

  default:    
    FTM_MSG_ERROR("Unknown FTM Customer Extension Command",0,0,0);
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
  break;
	}
  return rsp_pkt;
}
/*===========================================================================

FUNCTION FTM_CUSTOMER_EXTENSIONS_DISPATCH

DESCRIPTION
    This sample function display the data in byte format using FTM_MSG,
    and the FTM_MSG can be seen in the QXDM F3 messages. (FTM message has
    to be turned on in the QXDM)

DEPENDENCIES
    None.

RETURN VALUE
    Response packet.

SIDE EFFECTS
    None.

===========================================================================*/
void ftm_customer_extensions_sample( word packetLength, byte *data )
{
 //A data structure can be casted to *data here.
 //Examples of data structure casting can be found in
 //ftm_common_dispatch.c
}
#endif
