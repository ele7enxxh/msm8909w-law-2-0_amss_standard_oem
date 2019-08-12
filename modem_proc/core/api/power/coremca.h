/*============================================================================
@file coremca.h

Defines supported keys for vector requests to "/core/mca" resource

Copyright (c) 2009-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/api/power/coremca.h#1 $
============================================================================*/
#ifndef COREMCA_H
#define COREMCA_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Constants
 * -------------------------------------------------------------------------*/

//supported keys for mca vector requests
#define MPPS_REQ_KEY           0x4D505053 //'M' 'P' 'P' 'S'  Units: MPPS
#define CPP_REQ_KEY            0x43505000 //'C' 'P' 'P'      Units: CPP*1000
#define Q6_CLK_REQ_KEY         0x51434C4B //'Q' 'C' 'L' 'K'  Units: kHz
#define IB_REQ_KEY             0x49425700 //'I' 'B' 'W'      Units: MB/s
#define AB_REQ_KEY             0x41425700 //'A' 'B' 'W'      Units: MB/s
#define LAT_REQ_KEY            0x4C415400 //'L' 'A' 'T'      Units: ns 
#define FUDGE_FACTOR_REQ_KEY   0x46460000 //'F' 'F'          Units: (1/k(i))*1000 where, k(i) < 1
#define REQUEST_PRIORITY_KEY   0x52515052 //'R' 'Q' 'P' 'R'  Units: Low, Medium, High (see #defines below)

//request priorities for mca requests
#define REQ_PRIORITY_DEFAULT 0
#define REQ_PRIORITY_LOW     1
#define REQ_PRIORITY_MEDIUM  2
#define REQ_PRIORITY_HIGH    3

#ifdef __cplusplus
}
#endif

#endif // COREMCA_H
