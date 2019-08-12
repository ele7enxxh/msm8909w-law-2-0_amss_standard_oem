#ifndef DL_TIMELINE_H
#define DL_TIMELINE_H

/*===========================================================================
        D O W N L I N K   T I M E L I N E   D E C L A R A T I O N S

DESCRIPTION
  This file contains declarations and function prototypes for the
  downlink timeline event handlers.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  


Copyright (c) 2000 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dltimeline.h_v   1.3   22 May 2001 17:13:50   skeshava  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dltimeline.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
01/25/07    vb      featurized to bring in support for MCAL/dl enhanced
08/13/07    vp      Moved event handlers to individual modules.
04/12/06    mc      Added support for DL DCH maintenance event.
03/21/06    vip      Added RxD state update event on RX timeline's stack
12/30/04    vb      defined dl_tx_gen_mod_timing_post_cb to handle the n-pccpch 
                    config occur before UL mod timing is written to f/w
08/30/04    gs      Added combiner channel event handler to Rx events
                    Removed code for decoder unit test
05/22/2001  sk      Added TX SetModTiming event handler.
02/21/2001  mdl     removed all references to the ACQ timeline and associated
                    events and handlers.
01/17/2001  mdl     replaced RX_GEN_PHYCHAN_CFG_EVT and
                    RX_GEN_PHYCHAN_FRM_BDRY_EVENT with the 4
                    RX_GEN_PHYCHAN_HWCH#_EVENT names in 
                    rx_gen_tl_event_enum_type
                    
11/01/00    gs      Added decoder unit test code. Changed dec event names.
09/27/00    mdl     Created File

===========================================================================*/
#endif

