/******************************************************************************
 *
 *  $Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm8963_tlim.h#1 $
 *
 * -- Copyright Notice --
 *
 * Copyright (c) 2004 Asahi Kasei Microdevices Corporation, Japan
 * All Rights Reserved.
 *
 * This software program is proprietary program of Asahi Kasei Microdevices 
 * Corporation("AKM") licensed to authorized Licensee under Software License 
 * Agreement (SLA) executed between the Licensee and AKM.
 * 
 * Use of the software by unauthorized third party, or use of the software 
 * beyond the scope of the SLA is strictly prohibited.
 *
 * -- End Asahi Kasei Microdevices Copyright Notice --
 *
 ******************************************************************************/
#ifndef _SNSD_MAG_AKM8963_TLIM_H
#define _SNSD_MAG_AKM8963_TLIM_H

// Limit of factory shipment test
#define TLIMIT_NO_RST                   0x101

#define TLIMIT_NO_RST_READ              0x102

#define TLIMIT_NO_RST_WIA				0x103
#define TLIMIT_LO_RST_WIA				0x48
#define TLIMIT_HI_RST_WIA				0x48

#define TLIMIT_NO_RST_INFO				0x104
#define TLIMIT_LO_RST_INFO				0
#define TLIMIT_HI_RST_INFO				255

#define TLIMIT_NO_RST_ST1				0x105
#define TLIMIT_LO_RST_ST1				0
#define TLIMIT_HI_RST_ST1				0

#define TLIMIT_NO_RST_HXL				0x106
#define TLIMIT_LO_RST_HXL				0
#define TLIMIT_HI_RST_HXL				0

#define TLIMIT_NO_RST_HXH				0x107
#define TLIMIT_LO_RST_HXH				0
#define TLIMIT_HI_RST_HXH				0

#define TLIMIT_NO_RST_HYL				0x108
#define TLIMIT_LO_RST_HYL				0
#define TLIMIT_HI_RST_HYL				0

#define TLIMIT_NO_RST_HYH				0x109
#define TLIMIT_LO_RST_HYH				0
#define TLIMIT_HI_RST_HYH				0

#define TLIMIT_NO_RST_HZL				0x10A
#define TLIMIT_LO_RST_HZL				0
#define TLIMIT_HI_RST_HZL				0

#define TLIMIT_NO_RST_HZH				0x10B
#define TLIMIT_LO_RST_HZH				0
#define TLIMIT_HI_RST_HZH				0

#define TLIMIT_NO_RST_ST2				0x10C
#define TLIMIT_LO_RST_ST2				0
#define TLIMIT_HI_RST_ST2				0

#define TLIMIT_NO_RST_CNTL1				0x10D
#define TLIMIT_LO_RST_CNTL1				0
#define TLIMIT_HI_RST_CNTL1				0

#define TLIMIT_NO_RST_CNTL2				0x10E
#define TLIMIT_LO_RST_CNTL2				0
#define TLIMIT_HI_RST_CNTL2				0

#define TLIMIT_NO_RST_ASTC				0x10F
#define TLIMIT_LO_RST_ASTC				0
#define TLIMIT_HI_RST_ASTC				0

#define TLIMIT_NO_RST_I2CDIS			0x110
#define TLIMIT_LO_RST_I2CDIS            0
#define TLIMIT_HI_RST_I2CDIS            0

#define TLIMIT_NO_ASAX					0x112
#define TLIMIT_LO_ASAX					1
#define TLIMIT_HI_ASAX					254

#define TLIMIT_NO_ASAY					0x113
#define TLIMIT_LO_ASAY					1
#define TLIMIT_HI_ASAY					254

#define TLIMIT_NO_ASAZ					0x114
#define TLIMIT_LO_ASAZ					1
#define TLIMIT_HI_ASAZ					254

#define TLIMIT_NO_SNG_CNTL1				0x201
#define TLIMIT_NO_SNG_WAIT				0x202

#define TLIMIT_NO_SNG_ST1				0x203
#define TLIMIT_LO_SNG_ST1				1
#define TLIMIT_HI_SNG_ST1				1

#define TLIMIT_NO_SNG_HX				0x204
#define TLIMIT_LO_SNG_HX				-32759
#define TLIMIT_HI_SNG_HX				32759

#define TLIMIT_NO_SNG_HY				0x206
#define TLIMIT_LO_SNG_HY				-32759
#define TLIMIT_HI_SNG_HY				32759

#define TLIMIT_NO_SNG_HZ				0x208
#define TLIMIT_LO_SNG_HZ				-32759
#define TLIMIT_HI_SNG_HZ				32759

#define TLIMIT_NO_SNG_ST2				0x20A
#define TLIMIT_LO_SNG_ST2               16
#define TLIMIT_HI_SNG_ST2               16

#define TLIMIT_NO_SLF_ASTC				0x20B
#define TLIMIT_NO_SLF_CNTL1				0x20C
#define TLIMIT_NO_SLF_WAIT				0x20D

#define TLIMIT_NO_SLF_ST1				0x20E
#define TLIMIT_LO_SLF_ST1				1
#define TLIMIT_HI_SLF_ST1				1

#define TLIMIT_NO_SLF_RVHX				0x20F
#define TLIMIT_LO_SLF_RVHX				-5000
#define TLIMIT_HI_SLF_RVHX				5000

#define TLIMIT_NO_SLF_RVHY				0x211
#define TLIMIT_LO_SLF_RVHY				-5000
#define TLIMIT_HI_SLF_RVHY				5000

#define TLIMIT_NO_SLF_RVHZ				0x213
#define TLIMIT_LO_SLF_RVHZ				-5000
#define TLIMIT_HI_SLF_RVHZ				5000

#define TLIMIT_NO_SLF_ST2				0x215
#define TLIMIT_LO_SLF_ST2               16
#define TLIMIT_HI_SLF_ST2               16

#define TLIMIT_NO_SLF_ASTC_RST			0x216

#endif /* End include guard  _SNSD_MAG_AKM8963_TLIM_H */
