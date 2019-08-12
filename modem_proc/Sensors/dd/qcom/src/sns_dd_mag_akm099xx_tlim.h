/******************************************************************************
 *
 *  $Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm099xx_tlim.h#1 $
 *
 * -- Copyright Notice --
 *
 * Copyright (c) 2004 Asahi Kasei Microdevices Corporation, Japan
 * All Rights Reserved.
 *
 * -- End Asahi Kasei Microdevices Copyright Notice --
 *
 ******************************************************************************/
#ifndef _SNSD_MAG_AKM099XX_TLIM_H
#define _SNSD_MAG_AKM099XX_TLIM_H

// Limit of factory shipment test
#define TLIMIT_NO_ASAX          0x101
#define TLIMIT_LO_ASAX          1
#define TLIMIT_HI_ASAX          254

#define TLIMIT_NO_ASAY          0x102
#define TLIMIT_LO_ASAY          1
#define TLIMIT_HI_ASAY          254

#define TLIMIT_NO_ASAZ          0x103
#define TLIMIT_LO_ASAZ          1
#define TLIMIT_HI_ASAZ          254

#define TLIMIT_NO_SLF_RVHX      0x201
#define TLIMIT_NO_SLF_RVHY      0x202
#define TLIMIT_NO_SLF_RVHZ      0x203
#define TLIMIT_NO_SLF_ST2       0x204
#define TLIMIT_LO_SLF_ST2       0
#define TLIMIT_HI_SLF_ST2       0

/*******************************
 * AK09912 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_09912    -200
#define TLIMIT_HI_SLF_RVHX_09912    200

#define TLIMIT_LO_SLF_RVHY_09912    -200
#define TLIMIT_HI_SLF_RVHY_09912    200

#define TLIMIT_LO_SLF_RVHZ_09912    -1600
#define TLIMIT_HI_SLF_RVHZ_09912    -400

/*******************************
 * AK09911 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_09911    -30
#define TLIMIT_HI_SLF_RVHX_09911    30

#define TLIMIT_LO_SLF_RVHY_09911    -30
#define TLIMIT_HI_SLF_RVHY_09911    30

#define TLIMIT_LO_SLF_RVHZ_09911    -400
#define TLIMIT_HI_SLF_RVHZ_09911    -50

#endif /* End include guard  _SNSD_MAG_AKM099XX_TLIM_H */
