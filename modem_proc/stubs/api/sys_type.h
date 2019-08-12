#ifndef sys_type_h
#define sys_type_h

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.


===========================================================================*/

/* definition of enum rr_internal_band_T is based on definition of enum
   sys_band_T. */
typedef enum
{
  SYS_BAND_EGSM_900,
  SYS_BAND_PGSM_900,
  SYS_BAND_PCS_1900,
  SYS_BAND_DCS_1800,
  SYS_BAND_CELL_850,
  INVALID_BAND,
  SYS_BAND_NONE = 0xFF  
} sys_band_T;
//typedef  signed short      sint15;  
#ifndef FEATURE_GNSS_SA
typedef sint15                    dBx16_T;
#endif
/* ARFCN type, includes band if quadband is supported */
typedef struct{  
uint16     num;  
sys_band_T band;
} ARFCN_T;
#endif

