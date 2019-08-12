/*===========================================================================
  @file nf_sensors_log_test_001.cpp

DESCRIPTION
  This is a QTF unit test for the nf_sensors_log module

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

 This program is confidential and a trade secret of Qualcomm Technologies, Inc.  The
 receipt or possession of this program does not convey any rights to
 reproduce or disclose its contents or to manufacture, use or sell anything
 that this program describes in whole or in part, without the express written
 consent of Qualcomm Technologies, Inc.  The recipient and/or possessor of this program
 shall not reproduce or adapt or disclose or use this program except as
 expressly allowed by a written authorization from Qualcomm Technologies, Inc.

===========================================================================*/

/*=============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/mgp/pe/nf/test/nf_sensors_log_test_001.cpp#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/10/10   kgj     Original

=============================================================================*/

#include "qtf.h"
#include "qtf_stub.h"

/* --------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
extern "C" {
#include "comdef.h"
#include "customer.h"

#include "nf_sensors_log.h"
#include "log_codes.h"
#include "log.h"
#include "msg.h"
#include "mgp_pe_common.h"
}

extern "C"
{
TF_DECLARE_STUB(PACK(void *), log_alloc, log_code_type, unsigned int);
TF_DECLARE_STUB(void, log_commit, PACK(void *) );
}



/* --------------------------------------------------------------------------
 * PRIVATE Macros
 * --------------------------------------------------------------------------*/
#define LOG_HDR_SZ    (sizeof(log_hdr_type))


/* --------------------------------------------------------------------------
 * PRIVATE Data Types
 * --------------------------------------------------------------------------*/
typedef struct {
  uint8           d[256];
  uint32          alignment;
} logPayloadT;

typedef struct {
  log_hdr_type  hdr;
  logPayloadT   payload;
} logT;


/* --------------------------------------------------------------------------
 * PRIVATE Module Data
 * --------------------------------------------------------------------------*/
static size_t           q_packetSz;
static logT             z_inputBuffer;
static logT             z_outputBuffer;
static const uint8 *    p_payload;
static boolean          b_forceNoMem;


/* --------------------------------------------------------------------------
 * PRIVATE Functions and Function Prototypes
 * --------------------------------------------------------------------------*/
static void nf_logUnpack(
              void *        p_dst,
              size_t        sz )
{
  uint8  *  p_d = (uint8 *) p_dst;

  q_packetSz += sz;
  
  while( sz > 0 )
  {
    *p_d++ = *p_payload++;
    sz--;
  }
} 

#define NF_LOG_UNPACK(dst)                nf_logUnpack( &dst, sizeof(dst) )


static boolean hdrIsOK( log_hdr_type    hdr,
                        log_code_type   code,
                        unsigned int    length )
{
  if( hdr.code != code )
    return FALSE;

  if( hdr.len != length )
    return FALSE;

  if( (hdr.ts[0] != 0x12345678) || (hdr.ts[1] != 0x87654321) )
    return FALSE;

  return TRUE;
}

/* --------------------------------------------------------------------------
 * PRIVATE Functions and Function Prototypes
 * --------------------------------------------------------------------------*/
static boolean nf_logHeadingFilterTest( void )
{
  int                     d;
  uint8                   version;
  nf_headingFilterLogT    z_logInput;
  nf_headingFilterLogT    z_logOutput;
  nf_headingFilterLogT *  p_logOutput = &z_logOutput;
  FLT                     f;
  boolean                 ok;

  /*
   *  Zero out the Input and Output buffers
   */
  memset( &z_inputBuffer, 0, sizeof(z_inputBuffer) );
  memset( &z_outputBuffer, 0, sizeof(z_outputBuffer) );

  /*
   *  Zero out the input and output log data structures so they can
   *  be compared to each other at the end of the test
   */
  memset( &z_logInput, 0, sizeof(z_logInput) );
  memset( &z_logOutput, 0, sizeof(z_logOutput) );

  /*
   *  Create an input log data structure with test data 
   */ 
  z_logInput.w_gpsWeek = 111;
  z_logInput.q_gpsMsec = 222;
  z_logInput.b_hfEnabled = TRUE;
  z_logInput.f_filtHeading = 1.234;
  z_logInput.f_filtHeadingUnc = 2.345;
  z_logInput.f_gnssOnlyHeading = 3.456;
  z_logInput.f_gnssOnlyHeadingUnc = 4.567;

  /*
   *  Force log_alloc to fail
   */
  b_forceNoMem = TRUE;
  nf_logHeadingFilter( &z_logInput );
  b_forceNoMem = FALSE;

  if( z_inputBuffer.payload.d[0] != 0 )
    return FALSE;

  /*
   *  Invoke the log function
   */
  nf_logHeadingFilter( &z_logInput );

  /*
   *  Set the module-global p_packet pointer equal to the
   *  base address of the output buffer that was created by the
   *  log_commit call
   */
  p_payload = z_outputBuffer.payload.d;

  /*
   *  Unpack the output buffer into a local (unpacked) structure
   */
  q_packetSz = 0;
  NF_LOG_UNPACK( version );
  NF_LOG_UNPACK( p_logOutput->w_gpsWeek );
  NF_LOG_UNPACK( p_logOutput->q_gpsMsec );
  NF_LOG_UNPACK( p_logOutput->b_hfEnabled );
  NF_LOG_UNPACK( p_logOutput->f_filtHeading );
  NF_LOG_UNPACK( p_logOutput->f_filtHeadingUnc );
  NF_LOG_UNPACK( p_logOutput->f_gnssOnlyHeading );
  NF_LOG_UNPACK( p_logOutput->f_gnssOnlyHeadingUnc );

  /*
   *  Check for bad values at the beginning and end of
   *  the output buffer
   */
  if( z_outputBuffer.payload.d[0] != 0 )
    return FALSE;

  memscpy( &f, sizeof(f), (z_outputBuffer.payload.d + 20), 4);

  if( (f > 4.56701 ) || (f < 4.5669) )
    return FALSE;

  if( z_outputBuffer.payload.d[24] != 0 )
    return FALSE;

  ok = hdrIsOK( z_outputBuffer.hdr,
                LOG_GNSS_PE_HEADING_FILTER,
                24 + LOG_HDR_SZ );
  if( !ok )
    return FALSE;

  /*
   *  Check for invalid packet size and invalid version number
   */
  if( 24 != q_packetSz )
  {
    return FALSE;
  }

  if( version != 0 )
  {
    return FALSE;
  }

  /*
   *  Compare the local unpacked INPUT structure to the local 
   *  unpacked OUTPUT structure
   */
  d = memcmp( &z_logInput, &z_logOutput, sizeof(z_logInput) );
  if( d )
    return FALSE;

  return TRUE;
}

static boolean nf_logNhcTest( void )
{
  int           d;
  uint8         version;
  nf_nhcLogT    z_logInput;
  nf_nhcLogT    z_logOutput;
  nf_nhcLogT *  p_logOutput = &z_logOutput;
  FLT           f;
  boolean       ok;

  /*
   *  Zero out the Input and Output buffers
   */
  memset( &z_inputBuffer, 0, sizeof(z_inputBuffer) );
  memset( &z_outputBuffer, 0, sizeof(z_outputBuffer) );

  /*
   *  Zero out the input and output log data structures so they can
   *  be compared to each other at the end of the test
   */
  memset( &z_logInput, 0, sizeof(z_logInput) );
  memset( &z_logOutput, 0, sizeof(z_logOutput) );

  /*
   *  Create an input log data structure with test data 
   */ 
  z_logInput.w_gpsWeek = 111;
  z_logInput.q_gpsMsec = 222;
  z_logInput.b_nhcEnabled = TRUE;
  z_logInput.f_velEnuPreNHC[0] = 1.111;
  z_logInput.f_velEnuPreNHC[1] = 2.222;
  z_logInput.f_velEnuPreNHC[2] = 3.333;
  z_logInput.f_velEnuUncPreNHC[0] = 11.11;
  z_logInput.f_velEnuUncPreNHC[1] = 22.22;
  z_logInput.f_velEnuUncPreNHC[2] = 33.33;
  z_logInput.f_velEnuPostNHC[0] = 4.444;
  z_logInput.f_velEnuPostNHC[1] = 5.555;
  z_logInput.f_velEnuPostNHC[2] = 6.666;
  z_logInput.f_velEnuUncPostNHC[0] = 44.44;
  z_logInput.f_velEnuUncPostNHC[1] = 55.55;
  z_logInput.f_velEnuUncPostNHC[2] = 66.66;
  z_logInput.d_posLlaPreNHC[0] = 7.777;
  z_logInput.d_posLlaPreNHC[1] = 8.888;
  z_logInput.d_posLlaPreNHC[2] = 9.999;
  z_logInput.f_posLlaUncPreNHC[0] = 77.77;
  z_logInput.f_posLlaUncPreNHC[1] = 88.88;
  z_logInput.f_posLlaUncPreNHC[2] = 99.99;
  z_logInput.d_posLlaPostNHC[0] = 111.1;
  z_logInput.d_posLlaPostNHC[1] = 222.2;
  z_logInput.d_posLlaPostNHC[2] = 333.3;
  z_logInput.f_posLlaUncPostNHC[0] = 444.4;
  z_logInput.f_posLlaUncPostNHC[1] = 555.5;
  z_logInput.f_posLlaUncPostNHC[2] = 666.6;
  z_logInput.f_headingPostNHC = 777.7;
  z_logInput.f_headingUncPostNHC = 888.8;

  /*
   *  Force log_alloc to fail
   */
  b_forceNoMem = TRUE;
  nf_logNHC( &z_logInput );
  b_forceNoMem = FALSE;

  if( z_inputBuffer.payload.d[0] != 0 )
    return FALSE;

  /*
   *  Invoke the log function
   */
  nf_logNHC( &z_logInput );

  /*
   *  Set the module-global p_packet pointer equal to the
   *  base address of the output buffer that was created by the
   *  log_commit call
   */
  p_payload = z_outputBuffer.payload.d;

  /*
   *  Unpack the output buffer into a local (unpacked) structure
   */
  q_packetSz = 0;
  NF_LOG_UNPACK( version );
  NF_LOG_UNPACK( p_logOutput->w_gpsWeek );
  NF_LOG_UNPACK( p_logOutput->q_gpsMsec );
  NF_LOG_UNPACK( p_logOutput->b_nhcEnabled );
  NF_LOG_UNPACK( p_logOutput->f_velEnuPreNHC[0] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuPreNHC[1] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuPreNHC[2] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuUncPreNHC[0] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuUncPreNHC[1] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuUncPreNHC[2] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuPostNHC[0] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuPostNHC[1] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuPostNHC[2] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuUncPostNHC[0] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuUncPostNHC[1] );
  NF_LOG_UNPACK( p_logOutput->f_velEnuUncPostNHC[2] );
  NF_LOG_UNPACK( p_logOutput->d_posLlaPreNHC[0] );
  NF_LOG_UNPACK( p_logOutput->d_posLlaPreNHC[1] );
  NF_LOG_UNPACK( p_logOutput->d_posLlaPreNHC[2] );
  NF_LOG_UNPACK( p_logOutput->f_posLlaUncPreNHC[0] );
  NF_LOG_UNPACK( p_logOutput->f_posLlaUncPreNHC[1] );
  NF_LOG_UNPACK( p_logOutput->f_posLlaUncPreNHC[2] );
  NF_LOG_UNPACK( p_logOutput->d_posLlaPostNHC[0] );
  NF_LOG_UNPACK( p_logOutput->d_posLlaPostNHC[1] );
  NF_LOG_UNPACK( p_logOutput->d_posLlaPostNHC[2] );
  NF_LOG_UNPACK( p_logOutput->f_posLlaUncPostNHC[0] );
  NF_LOG_UNPACK( p_logOutput->f_posLlaUncPostNHC[1] );
  NF_LOG_UNPACK( p_logOutput->f_posLlaUncPostNHC[2] );
  NF_LOG_UNPACK( p_logOutput->f_headingPostNHC );
  NF_LOG_UNPACK( p_logOutput->f_headingUncPostNHC );

  /*
   *  Check for bad values at the beginning and end of
   *  the output buffer
   */
  if( z_outputBuffer.payload.d[0] != 0 )
    return FALSE;

  memscpy( &f, sizeof(f), (z_outputBuffer.payload.d + 132), 4);

  if( (f > 888.801 ) || (f < 888.799) )
    return FALSE;

  if( z_outputBuffer.payload.d[136] != 0 )
    return FALSE;

  ok = hdrIsOK( z_outputBuffer.hdr,
                LOG_GNSS_PE_NHC,
                136 + LOG_HDR_SZ );
  if( !ok )
    return FALSE;

  /*
   *  Check for invalid packet size and invalid version number
   */
  if( 136 != q_packetSz )
  {
    return FALSE;
  }

  if( version != 0 )
  {
    return FALSE;
  }

  /*
   *  Compare the local unpacked INPUT structure to the local 
   *  unpacked OUTPUT structure
   */
  d = memcmp( &z_logInput, &z_logOutput, sizeof(z_logInput) );
  if( d )
    return FALSE;

  return TRUE;
}

static boolean nf_logCrdTest( void )
{
  int           d;
  uint8         version;
  nf_crdLogT    z_logInput;
  nf_crdLogT    z_logOutput;
  nf_crdLogT *  p_logOutput = &z_logOutput;
  FLT           f;
  boolean       ok;

  /*
   *  Zero out the Input and Output buffers
   */
  memset( &z_inputBuffer, 0, sizeof(z_inputBuffer) );
  memset( &z_outputBuffer, 0, sizeof(z_outputBuffer) );

  /*
   *  Zero out the input and output log data structures so they can
   *  be compared to each other at the end of the test
   */
  memset( &z_logInput, 0, sizeof(z_logInput) );
  memset( &z_logOutput, 0, sizeof(z_logOutput) );

  /*
   *  Create an input log data structure with test data 
   */ 
  z_logInput.w_gpsWeek = 111;
  z_logInput.q_gpsMsec = 222;
  z_logInput.w_crd2StartGpsWeek = 333;
  z_logInput.q_crd2StartGpsMsec = 444;
  z_logInput.w_crd2EndGpsWeek = 555;
  z_logInput.q_crd2EndGpsMsec = 666;
  z_logInput.q_crd2Count = 777;
  z_logInput.f_rotationAngle = 0.888;

  /*
   *  Force log_alloc to fail
   */
  b_forceNoMem = TRUE;
  nf_logCRD( &z_logInput );
  b_forceNoMem = FALSE;

  if( z_inputBuffer.payload.d[0] != 0 )
    return FALSE;

  /*
   *  Invoke the log function
   */
  nf_logCRD( &z_logInput );

  /*
   *  Set the module-global p_packet pointer equal to the
   *  base address of the output buffer that was created by the
   *  log_commit call
   */
  p_payload = z_outputBuffer.payload.d;

  /*
   *  Unpack the output buffer into a local (unpacked) structure
   */
  q_packetSz = 0;
  NF_LOG_UNPACK( version );
  NF_LOG_UNPACK( p_logOutput->w_gpsWeek );
  NF_LOG_UNPACK( p_logOutput->q_gpsMsec );
  NF_LOG_UNPACK( p_logOutput->w_crd2StartGpsWeek );
  NF_LOG_UNPACK( p_logOutput->q_crd2StartGpsMsec );
  NF_LOG_UNPACK( p_logOutput->w_crd2EndGpsWeek );
  NF_LOG_UNPACK( p_logOutput->q_crd2EndGpsMsec );
  NF_LOG_UNPACK( p_logOutput->q_crd2Count );
  NF_LOG_UNPACK( p_logOutput->f_rotationAngle );

  /*
   *  Check for bad values at the beginning and end of
   *  the output buffer
   */
  if( z_outputBuffer.payload.d[0] != 0 )
    return FALSE;

  memscpy( &f, sizeof(f), (z_outputBuffer.payload.d + 23), 4);

  if( (f > 0.88801 ) || (f < 0.88799) )
    return FALSE;

  if( z_outputBuffer.payload.d[27] != 0 )
    return FALSE;

  ok = hdrIsOK( z_outputBuffer.hdr,
                LOG_GNSS_PE_CRD,
                27 + LOG_HDR_SZ);
  if( !ok )
    return FALSE;

  /*
   *  Check for invalid packet size and invalid version number
   */
  if( 27 != q_packetSz )
  {
    return FALSE;
  }

  if( version != 0 )
  {
    return FALSE;
  }

  /*
   *  Compare the local unpacked INPUT structure to the local 
   *  unpacked OUTPUT structure
   */
  d = memcmp( &z_logInput, &z_logOutput, sizeof(z_logInput) );
  if( d )
    return FALSE;

  return TRUE;
}

static boolean nf_logYawGyroTest( void )
{
  int                   d;
  uint8                 version;
  nf_yawGyroCalLogT     z_logInput;
  nf_yawGyroCalLogT     z_logOutput;
  nf_yawGyroCalLogT *   p_logOutput = &z_logOutput;
  FLT                   f;
  boolean               ok;

  /*
   *  Zero out the Input and Output buffers
   */
  memset( &z_inputBuffer, 0, sizeof(z_inputBuffer) );
  memset( &z_outputBuffer, 0, sizeof(z_outputBuffer) );

  /*
   *  Zero out the input and output log data structures so they can
   *  be compared to each other at the end of the test
   */
  memset( &z_logInput, 0, sizeof(z_logInput) );
  memset( &z_logOutput, 0, sizeof(z_logOutput) );

  /*
   *  Create an input log data structure with test data 
   */ 
  z_logInput.w_gpsWeek = 111;
  z_logInput.q_gpsMsec = 222;
  z_logInput.f_virtualGyroRate = 1.111;
  z_logInput.f_virtualGyroRateUnc = 2.222;
  z_logInput.f_virtualGyroRateOTFBias = 3.333;
  z_logInput.f_virtualGyroRateOTFBiasUnc = 4.444;

  /*
   *  Force log_alloc to fail
   */
  b_forceNoMem = TRUE;
  nf_logYawGyroCal( &z_logInput );
  b_forceNoMem = FALSE;

  if( z_inputBuffer.payload.d[0] != 0 )
    return FALSE;

  /*
   *  Invoke the log function
   */
  nf_logYawGyroCal( &z_logInput );

  /*
   *  Set the module-global p_packet pointer equal to the
   *  base address of the output buffer that was created by the
   *  log_commit call
   */
  p_payload = z_outputBuffer.payload.d;

  /*
   *  Unpack the output buffer into a local (unpacked) structure
   */
  q_packetSz = 0;
  NF_LOG_UNPACK( version );
  NF_LOG_UNPACK( p_logOutput->w_gpsWeek );
  NF_LOG_UNPACK( p_logOutput->q_gpsMsec );
  NF_LOG_UNPACK( p_logOutput->f_virtualGyroRate );
  NF_LOG_UNPACK( p_logOutput->f_virtualGyroRateUnc );
  NF_LOG_UNPACK( p_logOutput->f_virtualGyroRateOTFBias );
  NF_LOG_UNPACK( p_logOutput->f_virtualGyroRateOTFBiasUnc );

  /*
   *  Check for bad values at the beginning and end of
   *  the output buffer
   */
  if( z_outputBuffer.payload.d[0] != 0 )
    return FALSE;

  memscpy( &f, sizeof(f), (z_outputBuffer.payload.d + 19), 4);

  if( (f > 4.44401 ) || (f < 4.44399) )
    return FALSE;

  if( z_outputBuffer.payload.d[27] != 0 )
    return FALSE;

  ok = hdrIsOK( z_outputBuffer.hdr,
                LOG_GNSS_YAW_GYRO_CALIBRATION,
                23 + LOG_HDR_SZ );
  if( !ok )
    return FALSE;

  /*
   *  Check for invalid packet size and invalid version number
   */
  if( 23 != q_packetSz )
  {
    return FALSE;
  }

  if( version != 0 )
  {
    return FALSE;
  }

  /*
   *  Compare the local unpacked INPUT structure to the local 
   *  unpacked OUTPUT structure
   */
  d = memcmp( &z_logInput, &z_logOutput, sizeof(z_logInput) );
  if( d )
    return FALSE;

  return TRUE;
}

boolean nf_logSensorsTest( void )
{
  boolean   test_passed[4];

  test_passed[0] = nf_logHeadingFilterTest( );
  test_passed[1] = nf_logNhcTest( );
  test_passed[2] = nf_logCrdTest( );
  test_passed[3] = nf_logYawGyroTest( );

  return (  test_passed[0] &&
            test_passed[1] &&
            test_passed[2] &&
            test_passed[3] );
}



/* --------------------------------------------------------------------------
 * Stubbed log_alloc and log_commit
 * --------------------------------------------------------------------------*/
PACK(void *) nfslt_log_alloc( log_code_type   code, unsigned int length )
{
  if( b_forceNoMem )
    return NULL;

  z_inputBuffer.hdr.code = code;
  z_inputBuffer.hdr.len = length;
  z_inputBuffer.hdr.ts[0] = 0x12345678;
  z_inputBuffer.hdr.ts[1] = 0x87654321;
 
  return &z_inputBuffer;
}

void nfslt_log_commit( PACK(void *) ptr )
{
  uint8 *   p = (uint8 *) ptr;
  logT *    p_log;

  p_log = (logT *) p;

  z_outputBuffer.hdr = p_log->hdr;

  memscpy(z_outputBuffer.payload.d, sizeof(z_outputBuffer.payload.d), p_log->payload.d, p_log->hdr.len );
}


/* --------------------------------------------------------------------------
 * QTF Framework
 * --------------------------------------------------------------------------*/
TF_DEFINE_TEST_CASE(COMPONENT, NF_SENSORS_LOG_TEST_001);

void NF_SENSORS_LOG_TEST_001::Setup()
{
  // Put setup code here
  TF_MSG( "Hello world!" );
  TF_MSG( "TF_MSG takes var args (%d,%d)",
          3, 16 );
}

void NF_SENSORS_LOG_TEST_001::Test()
{
  TF_STUB(log_alloc) = nfslt_log_alloc;
  TF_STUB(log_commit) = nfslt_log_commit;

  TF_ASSERT( 0 == 0 );  

  if( nf_logSensorsTest( ) )
    TF_MSG( "NF_LOG_SENSORS_TEST_001 PASS\r\n" );
  else
    TF_MSG( "NF_LOG_SENSORS_TEST_001 FAIL\r\n" );
}

void NF_SENSORS_LOG_TEST_001::Teardown()
{
  // Put teardown code here
  TF_MSG( "Goodbye cruel world!" );
}
