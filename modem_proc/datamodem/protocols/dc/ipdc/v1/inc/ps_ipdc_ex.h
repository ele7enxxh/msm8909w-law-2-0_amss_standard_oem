#ifndef PS_IPDC_EX_H
#define PS_IPDC_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                     P S _ I P D C . H

DESCRIPTION
  Header file for the IP Data Compression (IPDC) protocol.
 

Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE


when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/15/15    cx    Added support for optimized UDC v1
01/07/13    pc    Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_ipdc_platform_defs.h"

#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB) || defined(IPDC_ON_Q6SIM) || defined(IPDC_ON_OFFTARGET_TOOL)
#include "dsm.h"
#endif

/*===========================================================================

                Q2E DATA ACCELERATION (DACC) PREFILL DATA

===========================================================================*/
/*===========================================================================

                          UDC PREFILL DATA
                      For LTE PDCP UDC Interface

===========================================================================*/
static const byte ipdc_udc_prefill_1[] =
{
  0x0a,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x1c,0x00,0x01,0x0a,0x41,0x63,0x63,0x65,0x70,0x74,0x3a,0x20,0x41,0x63,0x63,0x65,0x70,0x74,0x2d,0x43,0x68,0x61,
  0x72,0x73,0x65,0x74,0x3a,0x20,0x41,0x63,0x63,0x65,0x70,0x74,0x2d,0x45,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3a,0x20,0x41,0x63,0x63,0x65,0x70,0x74,0x2d,0x4c,0x61,0x6e,0x67,0x75,0x61,0x67,0x65,0x3a,0x20,
  0x41,0x63,0x63,0x65,0x70,0x74,0x2d,0x44,0x61,0x74,0x65,0x74,0x69,0x6d,0x65,0x3a,0x20,0x41,0x75,0x74,0x68,0x6f,0x72,0x69,0x7a,0x61,0x74,0x69,0x6f,0x6e,0x3a,0x20,0x43,0x61,0x63,0x68,0x65,0x2d,0x43,0x6f,
  0x6e,0x74,0x72,0x6f,0x6c,0x3a,0x20,0x43,0x6f,0x6e,0x6e,0x65,0x63,0x74,0x69,0x6f,0x6e,0x3a,0x20,0x43,0x6f,0x6f,0x6b,0x69,0x65,0x3a,0x20,0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x4c,0x65,0x6e,0x67,0x74,
  0x68,0x3a,0x20,0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x4d,0x44,0x35,0x3a,0x20,0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x3a,0x20,0x44,0x61,0x74,0x65,0x3a,0x20,0x45,0x78,0x70,0x65,
  0x63,0x74,0x3a,0x20,0x46,0x72,0x6f,0x6d,0x3a,0x20,0x48,0x6f,0x73,0x74,0x3a,0x20,0x49,0x66,0x2d,0x4d,0x61,0x74,0x63,0x68,0x3a,0x20,0x49,0x66,0x2d,0x4d,0x6f,0x64,0x69,0x66,0x69,0x65,0x64,0x2d,0x53,0x69,
  0x6e,0x63,0x65,0x3a,0x20,0x49,0x66,0x2d,0x4e,0x6f,0x6e,0x65,0x2d,0x4d,0x61,0x74,0x63,0x68,0x3a,0x20,0x49,0x66,0x2d,0x52,0x61,0x6e,0x67,0x65,0x3a,0x20,0x49,0x66,0x2d,0x55,0x6e,0x6d,0x6f,0x64,0x69,0x66,
  0x69,0x65,0x64,0x2d,0x53,0x69,0x6e,0x63,0x65,0x3a,0x20,0x4d,0x61,0x78,0x2d,0x46,0x6f,0x72,0x77,0x61,0x72,0x64,0x73,0x3a,0x20,0x50,0x72,0x61,0x67,0x6d,0x61,0x3a,0x20,0x50,0x72,0x6f,0x78,0x79,0x2d,0x41,
  0x75,0x74,0x68,0x6f,0x72,0x69,0x7a,0x61,0x74,0x69,0x6f,0x6e,0x3a,0x20,0x52,0x61,0x6e,0x67,0x65,0x3a,0x20,0x52,0x65,0x66,0x65,0x72,0x65,0x72,0x3a,0x20,0x54,0x45,0x3a,0x20,0x55,0x70,0x67,0x72,0x61,0x64,
  0x65,0x3a,0x20,0x55,0x73,0x65,0x72,0x2d,0x41,0x67,0x65,0x6e,0x74,0x3a,0x20,0x56,0x69,0x61,0x3a,0x20,0x57,0x61,0x72,0x6e,0x69,0x6e,0x67,0x3a,0x20,0x58,0x2d,0x52,0x65,0x71,0x75,0x65,0x73,0x74,0x65,0x64,
  0x2d,0x57,0x69,0x74,0x68,0x3a,0x20,0x44,0x4e,0x54,0x3a,0x20,0x58,0x2d,0x46,0x6f,0x72,0x77,0x61,0x72,0x64,0x65,0x64,0x2d,0x46,0x6f,0x72,0x3a,0x20,0x58,0x2d,0x46,0x6f,0x72,0x77,0x61,0x72,0x64,0x65,0x64,
  0x2d,0x50,0x72,0x6f,0x74,0x6f,0x3a,0x20,0x46,0x72,0x6f,0x6e,0x74,0x2d,0x45,0x6e,0x64,0x2d,0x48,0x74,0x74,0x70,0x73,0x3a,0x20,0x58,0x2d,0x41,0x31,0x54,0x44,0x65,0x76,0x69,0x63,0x65,0x49,0x64,0x3a,0x20,
  0x58,0x2d,0x57,0x61,0x70,0x2d,0x50,0x72,0x6f,0x66,0x69,0x6c,0x65,0x3a,0x20,0x50,0x72,0x6f,0x78,0x79,0x2d,0x43,0x6f,0x6e,0x6e,0x65,0x63,0x74,0x69,0x6f,0x6e,0x3a,0x20,0x47,0x45,0x54,0x20,0x2f,0x48,0x54,
  0x54,0x50,0x2f,0x31,0x2e,0x31,0x69,0x6e,0x64,0x65,0x78,0x2e,0x68,0x74,0x6d,0x6c,0x67,0x7a,0x69,0x70,0x2c,0x20,0x64,0x65,0x66,0x6c,0x61,0x74,0x65,0x65,0x6e,0x2d,0x55,0x53,0x2c,0x20,0x65,0x6e,0x69,0x6d,
  0x61,0x67,0x65,0x2f,0x6a,0x70,0x67,0x69,0x6d,0x61,0x67,0x65,0x2f,0x70,0x6e,0x67,0x69,0x6d,0x61,0x67,0x65,0x2f,0x2a,0x74,0x65,0x78,0x74,0x2f,0x63,0x73,0x73,0x74,0x65,0x78,0x74,0x2f,0x68,0x74,0x6d,0x6c,
  0x61,0x70,0x70,0x6c,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x2f,0x78,0x68,0x74,0x6d,0x6c,0x2b,0x78,0x6d,0x6c,0x61,0x70,0x70,0x6c,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x2f,0x78,0x6d,0x6c,0x6b,0x65,0x65,0x70,
  0x2d,0x61,0x6c,0x69,0x76,0x65,0x4d,0x6f,0x7a,0x69,0x6c,0x6c,0x61,0x57,0x69,0x6e,0x64,0x6f,0x77,0x73,0x47,0x65,0x63,0x6b,0x6f,0x46,0x69,0x72,0x65,0x66,0x6f,0x78,0x57,0x65,0x62,0x4b,0x69,0x74,0x58,0x4d,
  0x4c,0x48,0x74,0x74,0x70,0x52,0x65,0x71,0x75,0x65,0x73,0x74,0x0a,0x0a,0x69,0x6e,0x73,0x74,0x61,0x67,0x72,0x61,0x6d,0x0a,0x77,0x68,0x61,0x74,0x73,0x61,0x70,0x70,0x0a,0x74,0x75,0x6d,0x62,0x6c,0x72,0x0a,
  0x66,0x6c,0x69,0x63,0x6b,0x72,0x0a,0x75,0x62,0x65,0x72,0x0a,0x71,0x71,0x0a,0x61,0x6c,0x69,0x70,0x61,0x79,0x0a,0x77,0x65,0x63,0x68,0x61,0x74,0x0a,0x74,0x61,0x6f,0x62,0x61,0x6f,0x0a,0x77,0x65,0x69,0x62,
  0x6f,0x0a,0x62,0x61,0x69,0x64,0x75,0x0a,0x69,0x71,0x69,0x79,0x69,0x0a,0x71,0x71,0x20,0x6d,0x61,0x69,0x6c,0x0a,0x71,0x71,0x20,0x6d,0x75,0x73,0x69,0x63,0x0a,0x6b,0x75,0x67,0x6f,0x75,0x20,0x6d,0x75,0x73,
  0x69,0x63,0x0a,0x74,0x65,0x6e,0x63,0x65,0x6e,0x74,0x20,0x76,0x69,0x64,0x65,0x6f,0x0a,0x74,0x65,0x6e,0x63,0x65,0x6e,0x74,0x20,0x6e,0x65,0x77,0x73,0x0a,0x03,0x77,0x77,0x77,0x06,0x74,0x61,0x6f,0x62,0x61,
  0x6f,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x74,0x61,0x6f,0x62,0x61,0x6f,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x33,0x36,0x30,0x62,0x75,0x79,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x33,
  0x36,0x30,0x62,0x75,0x79,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x05,0x62,0x61,0x69,0x64,0x75,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x62,0x61,0x69,0x64,0x75,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,
  0x77,0x77,0x05,0x77,0x65,0x69,0x62,0x6f,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x77,0x65,0x69,0x62,0x6f,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x04,0x73,0x69,0x6e,0x61,0x03,0x63,0x6f,0x6d,0x02,
  0x63,0x6e,0x77,0x77,0x77,0x2e,0x73,0x69,0x6e,0x61,0x2e,0x63,0x6f,0x6d,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x05,0x79,0x6f,0x75,0x6b,0x75,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x79,0x6f,0x75,0x6b,
  0x75,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x05,0x74,0x75,0x64,0x6f,0x75,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x74,0x75,0x64,0x6f,0x75,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x02,0x71,
  0x71,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x71,0x71,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x04,0x73,0x6f,0x68,0x75,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x73,0x6f,0x68,0x75,0x2e,0x63,0x6f,
  0x6d,0x0a,0x03,0x77,0x77,0x77,0x05,0x74,0x6d,0x61,0x6c,0x6c,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x74,0x6d,0x61,0x6c,0x6c,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x03,0x31,0x36,0x33,0x03,0x63,
  0x6f,0x6d,0x77,0x77,0x77,0x2e,0x31,0x36,0x33,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x09,0x78,0x69,0x6e,0x68,0x75,0x61,0x6e,0x65,0x74,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x78,0x69,0x6e,0x68,
  0x75,0x61,0x6e,0x65,0x74,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x70,0x65,0x6f,0x70,0x6c,0x65,0x03,0x63,0x6f,0x6d,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,0x70,0x65,0x6f,0x70,0x6c,0x65,0x2e,0x63,
  0x6f,0x6d,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x05,0x69,0x66,0x65,0x6e,0x67,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x69,0x66,0x65,0x6e,0x67,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x03,0x7a,
  0x6f,0x6c,0x03,0x63,0x6f,0x6d,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,0x7a,0x6f,0x6c,0x2e,0x63,0x6f,0x6d,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x06,0x72,0x65,0x6e,0x72,0x65,0x6e,0x03,0x63,0x6f,0x6d,0x77,
  0x77,0x77,0x2e,0x72,0x65,0x6e,0x72,0x65,0x6e,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x64,0x6f,0x75,0x62,0x61,0x6e,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x64,0x6f,0x75,0x62,0x61,0x6e,0x2e,
  0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x02,0x35,0x38,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x35,0x38,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x74,0x69,0x61,0x6e,0x79,0x61,0x02,0x63,0x6e,
  0x77,0x77,0x77,0x2e,0x74,0x69,0x61,0x6e,0x79,0x61,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x05,0x69,0x71,0x69,0x79,0x69,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x69,0x71,0x69,0x79,0x69,0x2e,0x63,0x6f,
  0x6d,0x0a,0x03,0x77,0x77,0x77,0x05,0x68,0x65,0x78,0x75,0x6e,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x68,0x65,0x78,0x75,0x6e,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x05,0x74,0x67,0x62,0x75,0x73,
  0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x74,0x67,0x62,0x75,0x73,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x05,0x70,0x63,0x70,0x6f,0x70,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x70,0x63,0x70,0x6f,
  0x70,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x08,0x70,0x63,0x6f,0x6e,0x6c,0x69,0x6e,0x65,0x03,0x63,0x6f,0x6d,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,0x70,0x63,0x6f,0x6e,0x6c,0x69,0x6e,0x65,0x2e,0x63,
  0x6f,0x6d,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x06,0x77,0x65,0x63,0x68,0x61,0x74,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x77,0x65,0x63,0x68,0x61,0x74,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,
  0x03,0x6b,0x75,0x36,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x6b,0x75,0x36,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x04,0x70,0x70,0x74,0x76,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x70,0x70,0x74,
  0x76,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x31,0x30,0x6a,0x71,0x6b,0x61,0x03,0x63,0x6f,0x6d,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,0x31,0x30,0x6a,0x71,0x6b,0x61,0x2e,0x63,0x6f,0x6d,0x2e,0x63,
  0x6e,0x0a,0x03,0x77,0x77,0x77,0x11,0x6d,0x65,0x69,0x73,0x68,0x69,0x63,0x68,0x69,0x6e,0x61,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x6d,0x65,0x69,0x73,0x68,0x69,0x63,0x68,0x69,0x6e,0x61,0x2e,0x63,0x6f,
  0x6d,0x0a,0x03,0x77,0x77,0x77,0x07,0x74,0x77,0x69,0x74,0x74,0x65,0x72,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x74,0x77,0x69,0x74,0x74,0x65,0x72,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x67,
  0x6f,0x6f,0x67,0x6c,0x65,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x67,0x6f,0x6f,0x67,0x6c,0x65,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x08,0x66,0x61,0x63,0x65,0x62,0x6f,0x6f,0x6b,0x03,0x63,0x6f,
  0x6d,0x77,0x77,0x77,0x2e,0x66,0x61,0x63,0x65,0x62,0x6f,0x6f,0x6b,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x68,0x61,0x6f,0x31,0x32,0x33,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x68,0x61,0x6f,
  0x31,0x32,0x33,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x03,0x33,0x36,0x30,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,0x33,0x36,0x30,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x03,0x67,0x6d,0x77,0x02,0x63,
  0x6e,0x77,0x77,0x77,0x2e,0x67,0x6d,0x77,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x04,0x63,0x6e,0x74,0x76,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,0x63,0x6e,0x74,0x76,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,
  0x04,0x73,0x6f,0x73,0x6f,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x73,0x6f,0x73,0x6f,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x06,0x61,0x6d,0x61,0x7a,0x6f,0x6e,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,
  0x61,0x6d,0x61,0x7a,0x6f,0x6e,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,0x02,0x6a,0x64,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x6a,0x64,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x10,0x63,0x68,0x69,
  0x6e,0x61,0x64,0x61,0x69,0x6c,0x79,0x03,0x63,0x6f,0x6d,0x02,0x63,0x6e,0x77,0x77,0x77,0x2e,0x63,0x68,0x69,0x6e,0x61,0x64,0x61,0x69,0x6c,0x79,0x2e,0x63,0x6f,0x6d,0x2e,0x63,0x6e,0x0a,0x03,0x77,0x77,0x77,
  0x06,0x61,0x6c,0x69,0x70,0x61,0x79,0x03,0x63,0x6f,0x6d,0x77,0x77,0x77,0x2e,0x61,0x6c,0x69,0x70,0x61,0x79,0x2e,0x63,0x6f,0x6d,0x0a,0x03,0x77,0x77,0x77,0x05,0x63,0x68,0x69,0x6e,0x61,0x03,0x63,0x6f,0x6d,
  0x77,0x77,0x77,0x2e,0x63,0x68,0x69,0x6e,0x61,0x2e,0x63,0x6f,0x6d,0x45,0x00,0x00,0xa5,0x38,0x51,0x40,0x00,0x40,0x06,0xa0,0x3e,0x0a,0x12,0xf0,0x09,0x75,0x90,0xf2,0x17,0x98,0x61,0x1f,0x90,0x9a,0x90,0xa4,
  0x9c,0x04,0x8a,0xfa,0x56,0x80,0x18,0x01,0xca,0x7a,0x8e,0x00,0x00,0x01,0x01,0x08,0x0a,0x00,0x03,0x32,0xfd,0x6b,0xe1,0x9f,0x1f,0x60,0x00,0x00,0x00,0x00,0x20,0x06,0x40,0x20,0x02,0xc0,0x23,0x9c,0x17,0x31,
  0x1e,0x9b,0xab,0x8e,0x67,0xd9,0xfa,0x9a,0x60,0x26,0x07,0xf8,0xb0,0x40,0x0a,0x08,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x0a,0x98,0xf1,0x01,0xbb,0xc3,0x07,0xd1,0x80,0x6a,0xb5,0x0c,0xe6,0x80,0x10,0x04,
  0xde,0x4a,0xcc,0x00,0x00,0x01,0x01,0x08,0x0a,0x00,0x02,0x7f,0x07,0x5a,0x2d,0xd4,0x6e,0x45,0x00,0x00,0x3c,0xd8,0xd9,0x40,0x00,0x40,0x11,0xe1,0x5c,0x0a,0x2e,0xb4,0xe7,0x81,0x2e,0x40,0x37,0x4c,0x42,0x00,
  0x35,0x00,0x28,0xbc,0x27,0xe7,0xdc,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x06,0x67,0x6f,0x6f,0x67,0x6c,0x65,0x03,0x63,0x6f,0x6d,0x00,0x00,0x01,0x00,0x01,0x60,0x00,0x00,
  0x00,0x00,0x5d,0x11,0x40,0xfe,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x00,0x27,0xff,0xfe,0xd4,0x10,0xbb,0xfe,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x00,0x27,0xff,0xfe,0xfe,0x8f,0x95,0x02,0x23,0x02,
  0x22,0x00,0x5d,0x8d,0x2a,0x07,0x49
};

/* This is the prefill buffer 2 byte 2's complement checksum for
   ipdc_udc_prefill_1 */
#define IPDC_PREFILL_CKSUM_UDC_PREFILL_1  0xd0c8

/* This is the ULCM 2 byte 2's complement checksum when prefilled with 
   ipdc_udc_prefill_1 */
#define IPDC_ULCM_CKSUM_UDC_PREFILL_1     0x485e


/*===========================================================================

                              CONSTANTS

===========================================================================*/
/*--------------------------------------------------------------------------
  Maximum number of matches in a packet.
  i.e. Maximum number of compressed blocks in a compressed packet.
---------------------------------------------------------------------------*/
#define PS_IPDC_MAX_NUM_MATCHES          255

/*--------------------------------------------------------------------------
  Minimum match length.
  Only if at least these many byte sequence is present in compressor 
  memory, will it be compressed.
---------------------------------------------------------------------------*/
#define PS_IPDC_MIN_MATCH_IN_BYTES        5

/* 2^IPDC_META_SH_MATCH_LEN - 1 */
#define PS_IPDC_SH_LONGEST_MATCH_IN_BYTES    31
/* 2^IPDC_META_SH_MISMATCH_LEN - 1 */
#define PS_IPDC_SH_LONGEST_MISMATCH_IN_BYTES 7
#define PS_IPDC_SH_MAX_MISMATCH_IN_BYTES     9
/* maximum num of metadata supported for SH compression */
#define PS_IPDC_SH_MAX_NUM_MISMATCHES        15
/* maximum num of bytes that can be copied */
#define PS_IPDC_SH_MAX_BYTES_COPY            127
#define PS_IPDC_SH_ALIGNMENT_BYTES           4
/* maximum lookback length is 1021 instead of 1023 to allow us to
 move back 8 bytes for the SH checksum computation */
#define PS_IPDC_SH_MAX_LOOKBACK_LEN          1023
#define PS_IPDC_SH_CKSUM_START_IDX_OFFSET    8

/*--------------------------------------------------------------------------
  Longest match length.
  If more than these many bytes match, we ignore and assume the match to be
  of this max length.

  !! Do not increase beyond 132 as there is only 7 bits in the Ptr Meta 
     Data block to indicate the match length 
    (132 max char match length - 5 char minimum match length = 127) !!
---------------------------------------------------------------------------*/
#define PS_IPDC_LONGEST_MATCH_IN_BYTES (127 + PS_IPDC_MIN_MATCH_IN_BYTES) 

/*--------------------------------------------------------------------------
  Lower and upper bound on the compression gain threshold
---------------------------------------------------------------------------*/
#define IPDC_COMPRESSION_THRESHOLD_MIN      0
#define IPDC_COMPRESSION_THRESHOLD_MAX     99

/*--------------------------------------------------------------------------
  Lower and upper bound on the packet header size
---------------------------------------------------------------------------*/
#define IPDC_NUM_HEADER_BYTES_2_COMPRESS_DECOMPRESS_MIN    20
#define IPDC_NUM_HEADER_BYTES_2_COMPRESS_DECOMPRESS_MAX   160

/*--------------------------------------------------------------------------
  Maximum compression memory size, in KB
---------------------------------------------------------------------------*/
#define IPDC_COMPRESSION_MEM_MAX  16384


/*===========================================================================

                             DATA TYPES

===========================================================================*/
/*--------------------------------------------------------------------------
  IPDC Library Versions Available
---------------------------------------------------------------------------*/
typedef enum
{
  /* Initial library version */
  IPDC_LIB_VERSION_0       = 0x00,

  /* Header format updated, additional packet types added.
     - Packet type added to compress without pushing any data to memory 
       buffer
     - Packet type added to flush memory buffer and indicate the same to peer
     - Capability to query checksum for memory buffer content at any point of
       time
     - Capability to compress variable number of bytes for each packet 
       compression */
  IPDC_LIB_VERSION_1       = 0x01,
  
  IPDC_LIB_VERSION_MAX
  
} ipdc_lib_version_e_type;

/*--------------------------------------------------------------------------
  Enums for IP Data Compression (IPDC) packet types 
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_PKT_TYPE_UNKNOWN,
  IPDC_PKT_IPV4_PKT_UNCOMPRESSED,
  IPDC_PKT_IPV4_PKT_COMPRESSED,
  IPDC_PKT_IPV4_PKT_COMPRESSION_FAILED,
  IPDC_PKT_IPV4_PKT_DECOMPRESSION_FAILED,
  IPDC_PKT_MAX
} ipdc_pkt_e_type;

/*--------------------------------------------------------------------------
  IPDC Instance Handle
---------------------------------------------------------------------------*/
typedef void* ipdc_handle_type;

/*--------------------------------------------------------------------------
  IPDC Hash Algo List
  Different algorithms to create hash values for compressor memory.
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_HASH_ALG_1 = 1,
  IPDC_HASH_ALG_2,
  IPDC_HASH_ALG_3,
  IPDC_HASH_ALG_4,
  IPDC_HASH_ALG_5,
  IPDC_HASH_ALG_6
  
}ipdc_hash_algorithm_e_type;

/*--------------------------------------------------------------------------
  IPDC Extended Compression Memory (ECM) Size options
  Same is the size of the Hash Key Array
  ECM holds both the compressor memory and the packet bytes to compress
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_HK_ECM_PREALLOC_4096 = 0,
  IPDC_HK_ECM_PREALLOC_8192,
  IPDC_HK_ECM_PREALLOC_16384,
  IPDC_HK_ECM_PREALLOC_32768,
  IPDC_HK_ECM_PREALLOC_65536
  
}ipdc_hk_ecm_prealloc_size_e_type;

/*--------------------------------------------------------------------------
  IPDC compression configuration
  Whether to compress only header or the entire packet.
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_COMP_CFG_COMPRESS_ENTIRE_PKT = 0,
  IPDC_COMP_CFG_COMPRESS_HEADER_ONLY,

  IPDC_COMP_CFG_COMPRESS_MAX
  
}ipdc_compression_config_e_type;

/*--------------------------------------------------------------------------
  Allowed Compression Memory Size List
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_COMPRESSION_MEM_SIZE_2048 = 0,
  IPDC_COMPRESSION_MEM_SIZE_4096,
  IPDC_COMPRESSION_MEM_SIZE_8192,
  IPDC_COMPRESSION_MEM_SIZE_16384
  
}ipdc_compression_mem_size_e_type;

/*--------------------------------------------------------------------------
  IPDC Hash Bucket Size
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_HB_SIZE_256 = 0,
  IPDC_HB_SIZE_512,
  IPDC_HB_SIZE_1024,
  IPDC_HB_SIZE_2048,
  IPDC_HB_SIZE_4096,
  IPDC_HB_SIZE_8192,
  IPDC_HB_SIZE_16384,
  IPDC_HB_SIZE_32768
  
}ipdc_hb_size_e_type;

/*--------------------------------------------------------------------------
  IPDC Compressor Configuration
---------------------------------------------------------------------------*/
typedef struct
{
  /* Compression memory size (ULCM) */
  ipdc_compression_mem_size_e_type  cmp_mem_size;
  
  /* Number of bytes to compress in case of header only compression.
     Can be considered as approximate header length for traffic that will be
     compressed by this compressor instance. */
  uint8                             num_hdr_bytes_2_comp;

  /* Whether to compress header only or entire packet */
  ipdc_compression_config_e_type    comp_cfg_type;

  /* Compression gain threshold
     Once gain goes below this, IPDC dials down to header only compression */
  uint8                             comp_thld;

  /* Hash Bucket Size
     These many hash values can be generated.
     Smaller hash bucket would result in more collisions in hash values.
     Its a trade off between memory vs unique string comparison */
  ipdc_hb_size_e_type               hb_size;

  /* Extended Compressor Memory Size (ECM)
     Size of the memory buffer allocated to hold the Uplink Compression 
     Memory (ULCM) and the area to hold the packet being compressed */
  ipdc_hk_ecm_prealloc_size_e_type  hk_and_ecm_rsc_prealloc_size;

  /* Choice of the hashing algorithm to use */
  ipdc_hash_algorithm_e_type        hash_alg;

  /* IPDC client SUB ID.
     Which subscription is this IPDC instance being used on.
     Used only for logging purposes to fetch DPL iface from mode handler */
  uint8                             client_sub_id;

  /* IPDC client channel identifier.
     Used only for logging purposes to fetch DPL iface from mode handler */
  uint8                             client_channel_id;

  /* Number of packets to test compression gain.
     Even if compression gain has fallen below the threshold (or gone above),
     these many packets will be compressed in the current mode. */
  uint8                             num_pkts_to_test_comp;

  /* Multiplier to derive number of initial packets which are not monitored
     for compression gain.
     The number is derived by multiplying this number with the parameter
     num_pkts_to_test_comp */
  uint8                             num_pkts_to_not_test_init_mult;

  /* Maximum number of packets to ignore for compression gain calculation */
  uint16                            max_num_pkts_to_not_test;
  
} ipdc_comp_config_options_s_type;

/*--------------------------------------------------------------------------
  IPDC Decompressor Configuration
---------------------------------------------------------------------------*/
typedef struct
{
  /* Memory Buffer size : should be same as configured in compressor */
  ipdc_compression_mem_size_e_type decmp_mem_size;

  /* Header Length : should be same as configured in compressor */
  uint8                            num_hdr_bytes_2_decomp;
  
} ipdc_decomp_config_options_s_type;

/*--------------------------------------------------------------------------
  Info passed in by client per packet compression request
---------------------------------------------------------------------------*/
typedef struct
{
  /* IPDC instance handle */
  ipdc_handle_type          handle;

  /* Source packet to be compressed */
  dsm_item_type           **src_dsm_item;

  /* DSM Item to hold the compressed packet.
     DSM new buffer should have been allocated with enough space to hold the 
     compressed packet.
     IPDC directly operates on the DSM data pointer.
     DSM Pushdown API is not used. */
  dsm_item_type           **target_dsm_item;

  /* Whether to log per packet metrics or not */
  boolean                   log_metrics;

  /* Whether to log compressor statistics */
  boolean                   log_stats; 

  /* Whether to log compressed packet */
  boolean                   log_comp_pkt;

  /* Whether to log compressor memory buffer */
  boolean                   log_cmp_mem;

  /* Number of bytes in packet client wishes to compress.
     This parameter internally override the DTC length : Data To Compress
     DTC is usually derived automatically by compressor.
     In case below parameter bytes_to_comp is > 0 its acts as 
     overriden_dtc_len */
  uint16                    bytes_to_comp;

  /* Number of bytes in src pkt on which compression was attempted
     Output argument 
     IPDC -> Client */
  uint16                    bytes_comp_attempted;

  /* Length of the compressed packet
     This is same as the length of target DSM item
     IPDC is returning so that client doesn't need to perform DSM length
     call : MIPs Optimization */
  uint16                    target_dsm_len;

} ipdc_req_pkt_comp_info_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPDC_POWERUP_INIT

DESCRIPTION
  Initializes the IPDC layer at powerup.  Sets up the IPDC memory heap.

  THIS FUNCTION MUST BE CALLED BEFORE ANY IPDC INSTANCE IS ALLOCATED.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ipdc_powerup_init_ex
(
  void
);

/*===========================================================================
FUNCTION IPDC_ALLOC_EX

DESCRIPTION
  Allocates a IPDC Instance and returns a handle to it.  

  If only compressor parameters are passed, only a Compressor Engine will
  be allocated for the IPDC Instance.  Similarly if only decompressor 
  parameters are passed only a Decompressor Engine will be allocated for 
  the IPDC Instance.

  A NULL return indicates failure to allocate an IPDC instance.
  The handle is passed to all IPDC compression and decompression
  functions and is freed with a call to ipdc_free().

  
DEPENDENCIES
  The IPDC memheap must have been initialized prior to calling ipdc_alloc_ex()

RETURN VALUE
  non-NULL   IPDC instance handle on success
  NULL       on failure 
  

SIDE EFFECTS
  None
===========================================================================*/
ipdc_handle_type ipdc_alloc_ex
(
  ipdc_lib_version_e_type                         lib_version,
  const ipdc_comp_config_options_s_type    *const comp_channel_params,
  const ipdc_decomp_config_options_s_type  *const decomp_channel_params,
  const byte                               *const mem_prefill_buffer,
  uint16                                          mem_prefill_size
);

/*===========================================================================
FUNCTION IPDC_COMPRESS_EX

DESCRIPTION
  The IPDC compression function.  It takes the IPDC instance handle along
  with the packet to compress and returns the compressed packet in pkt along
  with the compressed packet type in pkt_type argument.

DEPENDENCIES
  A valid handle must be passed.

RETURN VALUE
   0   if successful.
  -1   in case of error

SIDE EFFECTS
  None
===========================================================================*/
int ipdc_compress_ex
(
  ipdc_req_pkt_comp_info_type  *comp_pkt_info_ptr
);

/*===========================================================================
FUNCTION IPDC_GET_UNCOMPRESSED_PKT

DESCRIPTION
  This function creates an uncompressed packet from an original packet.
  It takes the IPDC instance handle along with the original packet and
  adds the IPDC Header byte indicating that nothing was compressed in the
  packet.  Nothing Compressed packet is returned in pkt argument.


DEPENDENCIES
  A valid handle must be passed.

RETURN VALUE
   0   if successful.
  -1   in case of error

SIDE EFFECTS
  None
===========================================================================*/
int ipdc_get_uncompressed_pkt_ex
(
  ipdc_req_pkt_comp_info_type   *comp_pkt_info_ptr,
  boolean                        chain_src_dsm_to_targer_dsm
);

/*===========================================================================
FUNCTION IPDC_GET_RESET_PKT

DESCRIPTION
  This function creates an IPDC RESET packet from an original packet.
  It takes the IPDC instance handle along with the original packet and
  adds the IPDC Header byte indicating that memory reset is desired.
  Nothing is compressed in the packet, and the IPDC header indicates RESET 
  packet type.
  To process this API, compressor resets its memory and then proceeds with 
  uncompressed packet processing.

DEPENDENCIES
  A valid handle must be passed.
  
  Client has to ensure delivery of this packet to the peer.
  If delivery fails, decompressor memory will be out of sync and 
  decompression will fail henceforth.

RETURN VALUE
   0   if successful.
  -1   in case of error

SIDE EFFECTS
  None
===========================================================================*/
int ipdc_get_reset_pkt
(
  ipdc_req_pkt_comp_info_type   *comp_pkt_info_ptr,
  boolean                        chain_src_dsm_to_targer_dsm
);

/*===========================================================================
FUNCTION IPDC_DECOMPRESS_EX

DESCRIPTION
  The IPDC decompression function.  It takes the IPDC instance handle along
  with the packet to compress and returns the decompressed packet in pkt along
  with the decompressed packet type in pkt_type argument.

DEPENDENCIES
  A valid handle must be passed.

RETURN VALUE
   0   if successful.
  -1   in case of error

SIDE EFFECTS
  None
===========================================================================*/
int ipdc_decompress_ex
(
  ipdc_handle_type               handle,  /* IPDC instance                 */
  dsm_item_type                **pkt      /* Packet to decompress          */
);

/*===========================================================================
FUNCTION IPDC_FREE

DESCRIPTION
  Frees the IPDC instance.  The handle should not be used after a call to
  ipdc_free().

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_free_ex
(
  ipdc_handle_type          handle     /* Instance of IPDC                 */
);

/*===========================================================================
FUNCTION IPDC_UPDATE_COMP_PARAM_CLIENT_SUB_ID

DESCRIPTION
  Update compressor configuration parameter client SUB id.
  Internally this is used by IPDC for fetching iface for DPL logging.

DEPENDENCIES
  A valid handle must be passed

ARGUMENTS
  handle : IPDC Instance Handle
  sub_id : Subscription ID for which IPDC compression is being done

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int32 ipdc_update_comp_param_client_sub_id
(
  ipdc_handle_type  handle,
  uint8             sub_id 
);

/*===========================================================================
FUNCTION IPDC_UPDATE_COMP_PARAM_CLIENT_SUB_ID

DESCRIPTION
  Update compressor configuration parameter client Channel ID.
  Internally this is used by IPDC for fetching iface for DPL logging.

DEPENDENCIES
  A valid handle must be passed

ARGUMENTS
  handle     : IPDC Instance Handle
  channel_id : Channel ID for which this IPDC instance is being used

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int32 ipdc_update_comp_param_client_channel_id
(
  ipdc_handle_type  handle,
  uint8             channel_id 
);

/*===========================================================================
FUNCTION IPDC_UPDATE_COMP_PARAM_COMP_CFG_TYPE

DESCRIPTION
  Update compressor configuration parameter comp_cfg_type
  This parameter specifies whether to perform header only compression or 
  full packet compression

DEPENDENCIES
  A valid handle must be passed

ARGUMENTS
  handle        : IPDC Instance Handle
  comp_cfg_type : What kind of compression is desired

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int32 ipdc_update_comp_param_comp_cfg_type
(
  ipdc_handle_type                handle,
  ipdc_compression_config_e_type  comp_cfg_type
);

/*===========================================================================
FUNCTION IPDC_FLUSH_COMP_MEM_EX

DESCRIPTION
  Flushes the IPDC instance's Compressor Memory.  

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_flush_comp_mem_ex
(
  ipdc_handle_type          handle,     /* Instance of IPDC                 */
  const byte              * const mem_prefill_buffer_ptr,
  uint16                    mem_prefill_size
);

/*===========================================================================
FUNCTION IPDC_RESET_COMP_STATS_EX

DESCRIPTION
  Resets the IPDC instance's Compressor Engine's Statistics.  

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_reset_comp_stats_ex
(
  ipdc_handle_type          handle     /* Instance of IPDC                 */
);

/*===========================================================================
FUNCTION IPDC_FLUSH_DECOMP_MEM_EX

DESCRIPTION
  Flushes the IPDC instance's Decompressor Memory.  

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_flush_decomp_mem_ex
(
  ipdc_handle_type          handle,     /* Instance of IPDC                 */
  const byte              * const mem_prefill_buffer_ptr,
  uint16                    mem_prefill_size
);

/*===========================================================================
FUNCTION IPDC_RESET_DECOMP_STATS_EX

DESCRIPTION
  Resets the IPDC instance's Decompressor Engine's Statistics.  

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_reset_decomp_stats_ex
(
  ipdc_handle_type          handle /* IPDC Instance Handle */
);

/*===========================================================================
FUNCTION IPDC_GET_COMP_MEM_CKSUM

DESCRIPTION
  Compute the compressor memory checksum.

DEPENDENCIES
  A valid handle must be passed

ARGUMENTS 
  [in]  handle : IPDC instance handle
  [out] cksum_ptr : To hold the cksum generated.

RETURN VALUE
  -1 for failure
  0 for successful checksum computed.

SIDE EFFECTS
  None
===========================================================================*/
int32 ipdc_get_comp_mem_cksum
(
  ipdc_handle_type  handle, /* IPDC Instance Handle */
  uint16*           cksum_ptr /* Checksum returned via this argument */
);

/*===========================================================================
FUNCTION IPDC_GET_DECOMP_MEM_CKSUM

DESCRIPTION
  Compute the decompressor memory checksum.

DEPENDENCIES
  A valid handle must be passed

ARGUMENTS 
  [in]  handle : IPDC instance handle
  [out] cksum_ptr : To hold the cksum generated.

RETURN VALUE
  -1 for failure
  0 for successful checksum computed.

SIDE EFFECTS
  None
===========================================================================*/
int32 ipdc_get_decomp_mem_cksum
(
  ipdc_handle_type  handle, /* IPDC Instance Handle */
  uint16*           cksum_ptr /* Checksum returned via this argument */
);

/*===========================================================================
FUNCTION IPDC_IS_COMP_MEM_ALL_ZEROES

DESCRIPTION
  Check if the compressor memory is all zeroes.

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  TRUE for all zeroes in memory
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ipdc_is_comp_mem_all_zeroes
(
  ipdc_handle_type          handle /* IPDC Instance Handle */
);

/*===========================================================================
FUNCTION IPDC_IS_DECOMP_MEM_ALL_ZEROES

DESCRIPTION
  Check if the decompressor memory is all zeroes.

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  TRUE for all zeroes in memory
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ipdc_is_decomp_mem_all_zeroes
(
  ipdc_handle_type          handle /* IPDC Instance Handle */
);

/*===========================================================================
FUNCTION IPDC_FETCH_INST_AT_IDX

DESCRIPTION
  Fetches the IPDC instance at specified index in the global instance
  list.
  Primarily added to allow posting commands via DIAG for specific IPDC 
  instance.

PARAMETERS
  inst_idx:  Index for required IPDC instance
  
DEPENDENCIES
  Specified index should be less than number of existing allocated IPDC
  instances

RETURN VALUE
  non-NULL   IPDC instance handle on success
  NULL       on failure 

SIDE EFFECTS
  None
===========================================================================*/
ipdc_handle_type ipdc_fetch_inst_at_idx
(
  uint8  inst_idx
);

#ifdef __cplusplus
}
#endif

#endif /* PS_IPDC_EX_H */