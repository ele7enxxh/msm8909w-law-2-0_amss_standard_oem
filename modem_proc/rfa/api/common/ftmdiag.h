#ifndef FTMDIAG_H
#define FTMDIAG_H
/*==========================================================================

           Diagnostic Packet Definitions 

  Description: Contains Packet definitions for the diagnostic subsystem, 
               dispatcher function for the subsystem specific commands and
               also the subsystem code dispatch table.

  !!! NOTE: All member structures of diag packets must be PACKED.
  

Copyright (c) 2002, 2003 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/ftmdiag.h#1 $ $DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/10/12   sb      Removed ftm_cmd.h  
05/09/11   sar     Removed ftm_rf_cmd.h and ftm_gsm.h for rfa/api cleanup.
03/31/11   sar     Remove #def's to comply with CMI-4.
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
10/12/04   jac     Mainlined ASYNC packet code.
10/01/04   wd      Changes to ftm header structure.
08/19/04   ra      Merged 1x and UMTS targets
10/02/03   ka      Added support for variable length response packet.
08/21/02   rjr     Created file.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "diagpkt.h" 

#include "ftm.h"


/*===========================================================================
 
                            DATA DECLARATIONS

===========================================================================*/


typedef enum
{
  FTM_RSP_DO_LEGACY = 0,
  FTM_RSP_NO_RSP,
  FTM_RSP_DO_NEW_PKT,
  FTM_RSP_BAD_CMD,
  FTM_RSP_BAD_PARM,
  FTM_RSP_BAD_LEN,
  FTM_RSP_BAD_MODE,
  FTM_RSP_NOP,
  FTM_RSP_MAX
} ftm_rsp_cmd_type;

typedef struct {
  ftm_rsp_cmd_type  cmd;
  dword             pkt_len;
  void             *pkt_payload;
  boolean           delete_payload;
} ftm_rsp_pkt_type;

typedef struct {
  dword             req_pkt_len;   /* req_packet length */
  dword             rsp_pkt_len;   /* req_packet length */
  void             *ftm_data_ptr;  /* pointer to requesting ftm or hwtc data */
  void             *data_rsp_ptr;  /* pointer to memory allocated for respond packet */
} ftm_pkt_data_type;



typedef PACK(struct)
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
}
ftm_msg_get_ssid_ranges_req_type;


typedef PACK(struct)
{
  uint8 cmd_code;		/* Command code */
  uint8 sub_cmd;		/* Message Sub-command */
  uint16 ssid_start;	/* Start of subsystem ID range */
  uint16 ssid_end;      /* End of subsystem ID range */

  uint16 pad;           /* unused */

  uint32 rt_mask[1];	/* Array of runtime masks for subsystems in 
                                range.  Array size is:
                                'ssid_end - ssid_start + 1' */
}
ftm_msg_set_rt_mask_req_type;




/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION FTMDIAG_INIT

DESCRIPTION
  Register the subsystem function with diag.

============================================================================*/
void ftmdiag_init( void);

/*===========================================================================

FUNCTION TYPE FTMDIAG_IDLE_PROCESSING_CB

DESCRIPTION
  A function that when registered with diag, is called when the DIAG task has 
  idle processing time.

===========================================================================*/
void ftmdiag_idle_processing_cb (void *param);


/*===========================================================================

FUNCTION FTMDIAG_CREATE_NEW_PKT

DESCRIPTION
  This procedure creates a new paket in the ftm heap.

============================================================================*/
ftm_rsp_pkt_type ftmdiag_create_new_pkt (dword len);

/*===========================================================================

FUNCTION FTMDIAG_CREATE_NEW_PKT_WITH_DATA

DESCRIPTION
  This procedure creates a new paket in the ftm heap from given information 
  about header size and data size.

============================================================================*/
ftm_rsp_pkt_type ftmdiag_create_new_pkt_with_data (byte *hdr, dword hdr_len, byte *data, dword data_len);

/*===========================================================================

FUNCTION FTMDIAG_PROCESS_RSP_PKT

DESCRIPTION
  This procedure will send asyc pkts to diag.

============================================================================*/
boolean ftmdiag_process_async_rsp_pkt (ftm_rsp_pkt_type rsp_pkt);

/*===========================================================================

FUNCTION FTMDIAG_SEND_ASYNC_PKT

DESCRIPTION
  This procedure sets signals diag_task so it will send an asyc pkt.

============================================================================*/
void ftmdiag_send_async_pkt(ftm_msg_type *ftm_msg);

/*===========================================================================

FUNCTION FTM_ALLOCATE_DELAYED_RSP_BYTES

DESCRIPTION
   This function requests delayed response bytes from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

byte* ftm_allocate_delayed_rsp_bytes(
                           diagpkt_subsys_delayed_rsp_id_type delay_rsp_id,
                           diagpkt_subsys_cmd_code_type subsys_cmd_code,
                           uint16 rsp_cnt,
                           uint32 status, 
                           uint32 length);



#endif /* FTMDIAG_H */
