/*==============================================================================
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/afe/drivers/hw/lpasshwio/devcfg/src/lpasshwio_devcfg_8916.c#1 $
$DateTime: 2016/12/13 07:58:24 $
$Author: mplcsds1 $
$Change: 11985135 $
$Revision: #1 $

FILE: lpasshwio_devcfg_8x16.c 

DESCRIPTION: 8x16 Device Config

Copyright 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "lpasshwio_devcfg.h"

/* LPASS_HW_VERSION register address */
HwdLpassPropertyType lpass_prop ={0x7700000};

/* LPAIF */
HwdAudioIfPropertyType audioif_prop = 
{ 
   0x7708000,		/* Base Address */  
   (0x1000*16),		/* Register page size, 16 4 KB granules */
   0,				/* HDMI base Address */  
   0,				/* HDMI reg page size */  
   4,				/* DMA word size in bytes */				
   2, 				/* Read DMA channel count */  
   2,				/* Write DMA channel Address */  
   4,				/* Num of LPAIF interfaces  */
   {0, 3, 4, 5},	/* Interface type list */  
   {4, 2, 4, 4},	/* Max num channel for audio IF */  
   {1, 1, 2, 3},	/* Interface direction type */  
   46,				/* LPAIF L2VIC # */  
   1,				/* LPAIF interrupt line # */  
   0x30030000,		/* LPAIF h/w revision  */  
   0				/* AV Timer h/w latched */  
};

/* AV Timer */
HwdAvtimerPropertyType avtimer_prop = 
{
   {0x7706000, 0, 0},	/* AV timer reg Base Address */  
   {0x1000*2, 0, 0},	/* AV timer reg Page size */  
   {19200000, 0, 0},	/* AV timer root clock */
   45,					/* L2VIC #*/
   0x30040000			/* AV Timer h/w revision */
};

/* VFR */
lpasshwio_vfr_prop_t vfr_prop = 
{
   0x7701000,			/* VFR reg Base Address */  
   0x1000,				/* VFR reg page size */  
   0x10000000,			/* VFR h/w revision */
   0x1,					/* H/w latching version */
   {{59, 0x0, 0x1},		/* VFR Source 1 prop (L2VIC#, VFR_MUX_IN#, is_supported) */
   {60, 0x7, 0x1},		/* VFR Source 2 prop (L2VIC#, VFR_MUX_IN#, is_supported) */
   {0, 0x0, 0x0},		/* VFR Source 3 prop (L2VIC#, VFR_MUX_IN#, is_supported) */
   {0, 0x0, 0x0}}		/* VFR Source 4 prop (L2VIC#, VFR_MUX_IN#, is_supported) */
};

/* BT_FM */
lpasshwio_prop_riva_struct_t afe_riva_prop =
{
   1,			/* H/w revision */
   0xA206008,	/* CCU MB0 physical address */
   0x4105000, /* Q6SS IPC register physical address*/
   8, /*Q6ss IPC acknowledge interrupt bit position*/
   0x20,		/* Mail box offset form CCU MB0 */
   47,			/* Internal BT Tx L2VIC# */
   44,			/* Internal BT Rx L2VIC# */
   56,			/* Internal FM Tx/Rx L2VIC# */
   40			/* Internal BTFM IPC L2VIC# */
};

HwdDmlitePropertyType dmlite_prop = {0, 0, 0, 0};
HwdResamplerPropertyType resampler_prop = {0, 0, 0, 0};
lpasshwio_prop_slimbus_struct_t afe_slimbus_prop = {0};
HwdMidiPropertyType midi_prop = {0, 0, 0, 0, 0, 0};
lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};
lpasshwio_prop_hdmi_input_struct_t hdmi_input_prop = {0, 0, 0, 0};
HwdMmpmPropertyType mmpm_prop = 
{
   {100, 200, 300, 400, 500, 600, 700, 800},/* This BW table is irrelevant for
                                               Bear family. This must be in Bytes
                                               per second.*/
   {100, 100, 100, 100, 100, 100, 100, 100}, /* CPP or MCPS scaling factors, same
                                                for all AXI bus bandwidth. ACPM
                                                will do this scaling.*/
};


