#ifndef FTM_GSM_CTL_H
#define FTM_GSM_CTL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

												 F T M  G S M  C T L   

GENERAL DESCRIPTION
	This is the header file for the embedded FTM RF Commands 

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000 -2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

												EDIT HISTORY FOR MODULE

	This section contains comments describing changes made to the module.
	Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/ftm_gsm_ctl.h#1 $ 
$DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/13   cpm     Added EGSM support for FTM_PROGRAM_QFE_SCENARIO
07/01/14   sw      Move the calls to the geran test mode api into rftech_gsm 
04/03/13   ec	   Pass rfm_dev to NS function
04/02/13   aka     Support for band change in frame sweep functionality. 
02/26/13   tws     Make the TX frequency offset device specific.
02/21/13   tws     Fix GSM FTM variable initialisation.
01/18/13   tws     Set TX frequency offset is not device dependent.
01/3/13    av      Changes to support chain2 for GSM
12/03/12   tws     Move the FTM variable initialisation back to core.
11/27/12   tws     Add device id to FTM.
12/17/07   ad      Support ftm_gsm_toggle_modes() for EDGE only support
12/10/02   xw      Renamed it to ftm_gsm_ctl.h
08-28-02   rjr     Created 
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h" 
#include "gs.h"
  #ifdef FEATURE_GSM

    #include "ftm.h"



/*-----------------------------------------------------------------------------
                               Prototypes
-----------------------------------------------------------------------------*/

void ftm_gsm_start_geran( rfm_device_enum_type rfm_dev );

void ftm_gsm_stop_geran( rfm_device_enum_type rfm_dev );

void ftm_gsm_set_mode( rfm_device_enum_type rfm_dev,
                       ftm_mode_id_type mode );    

void ftm_gsm_frame_sweep_set_band( rfm_device_enum_type rfm_dev,
                                   ftm_mode_id_type mode );

void ftm_tdma_isr_dev0( sys_modem_as_id_e_type as_id );

void ftm_tdma_isr_dev2( sys_modem_as_id_e_type as_id );

void ftm_gsm_init_once( void );

void ftm_gsm_init_rx( rfm_device_enum_type rfm_dev );

void ftm_gsm_init_tx( rfm_device_enum_type rfm_dev );

gs_status_T ftm_send_FTM_STOP_GSM_MODE_REQ( rfm_device_enum_type rfm_dev );

extern void ftm_gsm_toggle_modes( void );

void ftm_gsm_set_tx_frequency_offset( rfm_device_enum_type rfm_dev, int32 offset );

void ftm_gsm_program_qfe_scenario(void);

#endif /* FTM_HAS_GSM */
#endif // FEATURE_FACTORY_TESTMODE
#endif  // FTM_GSM_CTL_H
