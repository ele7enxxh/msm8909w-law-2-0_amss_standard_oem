#ifndef DL_DEC_EVT_H
#define DL_DEC_EVT_H

/*===========================================================================
                        D L _ D E C _ E V T . H

GENERAL DESCRIPTION
  This file contains prototypes for decoder module sequencer event handlers.

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldecevt.h_v   1.1   12 Jul 2001 11:43:24   mlevin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dldecevt.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
                    
===========================================================================*/

      
/*===========================================================================
FUNCTION        DEC_TRBLK_HEADER_READ_EVENT_HANDLER

FILE NAME       

DESCRIPTION     This function handles the decoder transport block header
                read event.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void dec_trblk_header_read_event_handler(void);



extern void dec_trblk_data_read_event_handler(void);
      
#endif
