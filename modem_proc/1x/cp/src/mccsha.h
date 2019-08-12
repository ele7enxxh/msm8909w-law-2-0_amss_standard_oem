/* "mccsha.h" */
#ifndef MCCSHA_H
#define MCCSHA_H

/*===========================================================================

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccsha.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"

#if defined(FEATURE_IS2000_REL_A_AES)

/****************************************************************************
*   
*  Copyright (c) 1994 - 2013 Qualcomm Technologies, Inc. 
*  All Rights Reserved Qualcomm Proprietary
*  
*  Export of this technology or software is regulated by the U.S. Government.
*  Diversion contrary to U.S. law prohibited.
*
*  All ideas, data and information contained in or disclosed by
*  this document are confidential and proprietary information of
*  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
*  By accepting this material the recipient agrees that this material
*  and the information contained therein are held in confidence and in
*  trust and will not be used, copied, reproduced in whole or in part,
*  nor its contents revealed in any manner to others without the express
*  written permission of Qualcomm Technologies, Inc.
*   
*****************************************************************************
*
*   File:           mccsha.h
*
*   Description:    Header for SHA-1 functions implemented for ESA
*
*****************************************************************************/

/* header for SHA and related procedures */
#define WORD unsigned long
#define DIGEST_LENGTH 20

typedef struct {
  unsigned char digest[DIGEST_LENGTH]; /* Message digest */
  WORD   count[2];    /* count of bits */
  WORD   data[16];    /* data buffer */
  } SHA_INFO;
	
void shaInitial(SHA_INFO *sha_info);
void shaUpdate(SHA_INFO *sha_info, 
        unsigned char *buffer,
        unsigned long offset,
        unsigned long count);
void shaFinal(SHA_INFO *sha_info);

#endif /* FEATURE_IS2000_REL_A_AES */
#endif /* MCCSHA_H */
