#ifndef ADCDIAGCOMMON_H
#define ADCDIAGCOMMON_H

/*============================================================================
  @file AdcDiagCommon.h
 
  Declarations common to both the legacy and DAL ADC diagnostic systems.
  This file also contains all diagnostic packet definitions which must not change
  after they have been published.
 
  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/diag/protected/AdcDiagCommon.h#1 $
 
                Copyright (c) 2009-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/


#include "diagcmd.h"
#include "diagpkt.h"

/*============================================================================
 *  
 * Core Constants
 *  
 *============================================================================*/

 #define ADC_DIAG_DAL_VER_MAJOR       1
 #define ADC_DIAG_DAL_VER_MINOR       0

/*============================================================================
 *  
 * Command codes
 *  
 *============================================================================*/

/* This corresponds to the DIAG_SUBSYS_COREBSP */
#define ADC_DIAG_SUBSYS_ID            75

/*----------------------------------------------------------------------------
 * DAL Command Codes
 * -------------------------------------------------------------------------*/

#define ADC_DIAG_CMDDAL_VERSION       30
#define ADC_DIAG_CMDDAL_CHAN_INFO     31

#define ADC_DIAG_CMDDAL_STARTREAD     35
#define ADC_DIAG_CMDDAL_GETREADINGS   36
#define ADC_DIAG_CMDDAL_RECALIBRATE   37

/*============================================================================
 *  
 * Macros
 *  
 *============================================================================*/
/* TODO: replace this macro with an inline function */
#define ADC_DIAG_SET_RESERVED_FIELDS(field, cnt) do { \
  uint16 byteNum; \
  for(byteNum = 0; byteNum < (cnt); byteNum++) \
  { \
    field[byteNum] = 0xFF; \
  }} while(0)

/*============================================================================
 *  
 * DAL packets
 *  
 *============================================================================*/

/*----------------------------------------------------------------------------
 * Common Structures
 * -------------------------------------------------------------------------*/

typedef struct
{
  uint32 nDeviceIdx;
  uint32 nChannelIdx;
} AdcDiagDAL_ChannelSpecification;

typedef struct
{
  AdcDiagDAL_ChannelSpecification channel;
  uint16 result;
  uint8 reserved[2]; //For Alignment
} AdcDiagDAL_ErrorResult;

typedef struct
{
  AdcDiagDAL_ChannelSpecification channel;

  uint32 nReadTime;

  uint16 eErrors;
  uint8 reserved[2];  //For alignment

  //ADC Results
  int32 nPhysical;     
  uint32 nPercent;     
  uint32 nMicrovolts; 
  uint32 nCode; 

  uint8 reserved2[4]; //Potential extra field

} AdcDiagDAL_ReadResult;

/*----------------------------------------------------------------------------
 * Associated with: ADC_DIAG_CMDDAL_VERSION
 * -------------------------------------------------------------------------*/

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint8 reserved[4];

} AdcDiagDAL_Version_ReqPktType;

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint32 nMajorVersion;
  uint32 nMinorVersion;

  uint8 reserved[4];

} AdcDiagDAL_Version_RespPktType;

/*----------------------------------------------------------------------------
 * Associated with: ADC_DIAG_CMDDAL_CHAN_INFO
 * -------------------------------------------------------------------------*/

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint16 nChannelIdLength;
  uint8 reserved;       //For alignment

  char zsChannelId[1];

} AdcDiagDAL_ChanInfo_ReqPktType;

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint16 Error;
  uint8 reserved[2]; //For alignment

  AdcDiagDAL_ChannelSpecification info;

} AdcDiagDAL_ChanInfo_RespPktType;

/*----------------------------------------------------------------------------
 * Associated with: ADC_DIAG_CMDDAL_STARTREAD
 * -------------------------------------------------------------------------*/

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint16 nChannelCount;
  uint8 reserved[2];  //For alignment

  AdcDiagDAL_ChannelSpecification channels[1];

} AdcDiagDAL_StartRead_ReqPktType;

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint32 nStartReadTime;

  uint16 nResultCount;
  uint8 reserved[2];

  AdcDiagDAL_ErrorResult results[1];
  
} AdcDiagDAL_StartRead_RespPktType;

/*----------------------------------------------------------------------------
 * Associated with: ADC_DIAG_CMDDAL_GETREADINGS 
 * Returns results started by ADC_DIAG_CMDDAL_STARTREAD
 * -------------------------------------------------------------------------*/

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint16 nChannelCount;
  uint8 reserved[2];  //For alignment

  AdcDiagDAL_ChannelSpecification channels[1];

} AdcDiagDAL_GetReadings_ReqPktType;

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint16 nResultCount;
  uint8 reserved[2]; //For alignment

  AdcDiagDAL_ReadResult results[1];

} AdcDiagDAL_GetReadings_RespPktType;

/*----------------------------------------------------------------------------
 * Associated with: ADC_DIAG_CMDDAL_RECALIBRATE
 * -------------------------------------------------------------------------*/

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint16 nChannelCount;
  uint8 reserved[2];  //For alignment

  AdcDiagDAL_ChannelSpecification channels[1];

} AdcDiagDAL_Recalibrate_ReqPktType;

typedef struct
{
  diagpkt_subsys_header_type hdr;

  uint16 nResultCount;
  uint8 reserved[2];

  AdcDiagDAL_ErrorResult results[1];
} AdcDiagDAL_Recalibrate_RespPktType;

#endif /* ADCDIAGCOMMON_H */

