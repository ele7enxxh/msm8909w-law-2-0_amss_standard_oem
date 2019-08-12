#ifndef FTM_WCDMA_GOERTZEL_H
#define FTM_WCDMA_GOERTZEL_H

/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                     
                    F T M   G O E R T Z E L  

GENERAL DESCRIPTION
   This module contains function definitions pertaining to Goertzel DFT
	 algorithm.

EXTERNALIZED FUNCTIONS
   Functions in ftm_gmsa.s

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None

Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/ftm_goertzel.h#1 $ 
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/07   jfc     Mainlined T_MSM6280 feature
03/15/06   jac     Made scale factor change conditional.
03/14/06   jac     Added GSM prototypes - changed scale factor.
03/11/04   xw      Created.
===========================================================================*/
							 
#include "rfa_variation.h"
#include "ftm.h"
								 
#ifdef FTM_HAS_IM2_CAL

#include "comdef.h"

#define FTM_2PI             6.28318530717959  /* 2*pi */
#define MAX_NUM_OF_DFT_BIN  20
#define SCALERADIX          12
						
typedef struct
{
  uint16  dft_len;			   /* DFT length */
  uint16  dft_start_bin; 	 /* The index of the starting DFT bins of interest     */
  uint16  num_of_dft_bin;	 /* Number of DFT bins                                 */
  int32   *coeff_tbl;		   /* A pointer to store the cosine and sine coefficient. 
								              The length should be 2*numOfDftBin                 */
  uint8   radix;			 /* The radix for scaling the cosine and sine coefficient. */
} ftm_dft_im2_type;


/*===========================================================================
FUNCTION FTM_COMPUTE_GOERTZEL_COEFF

DESCRIPTION  
   
DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_compute_goertzel_coeff(ftm_dft_im2_type dft_param);	 

/*===========================================================================
FUNCTION FTM_GOERTZEL_DFT

DESCRIPTION  
   
DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_goertzel_dft(ftm_dft_im2_type dft_param, int8 *data, double *result);

/*===========================================================================
FUNCTION FTM_GOERTZEL_ASM

DESCRIPTION  
   
DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_goertzel_asm(int32 cos_data, int32 sin_data, int16 dft_length, int8 *data);

/*===========================================================================
FUNCTION FTM_GSM_GOERTZEL_ASM

DESCRIPTION  
   
DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_gsm_goertzel_asm(int32 cos_data, int32 sin_data, int16 dft_length, int8 *data);

void ftm_gsm_goertzel_dft(ftm_dft_im2_type dft_param, int8 *data, double *result);

extern void ftm_gsm_multi_prt_dft(uint16 dft_start_bin, uint16 num_of_dft_bins, uint16 dft_len, int8 *data);

/*===========================================================================
FUNCTION FTM_MULTI_PTR_DFT

DESCRIPTION  
   This function performs a Discrete Fourier transform (DFT) using the 
   Goertzel Algorithm.  It is very effeicient for tone detection such as
   DTMF or IM2 calibration. It does not requires the entire cosine table.
	 For speed optimization, it calls FTM_COMPUTE_GOERTZEL_COEFF() to pre-compute 
	 the required DFT bin coefficents.

DEPENDENCIES
   * Make sure    FTM_COMPUTE_GOERTZEL_COEFF() is called first.
   * dft_param    Contains information for performing DFT.
   * data         A pointer to IQ data (I,Q,I,Q,....)
   * result       A pointer to store the magnitude of the DFT results.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_multi_prt_dft(uint16 dft_start_bin, uint16 num_of_dft_bins, uint16 dft_len, int8 *data);

#endif /* FTM_HAS_IM2_CAL */

#endif /* FTM_WCDMA_GOERTZEL_H */
