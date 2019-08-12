#ifndef ADCDIAGDAL_H
#define ADCDIAGDAL_H

/*============================================================================
  @file AdcDiagDal.h
 
  External declarations for the DAL ADC diagnostic system.
 
  For more information, please see the diagnostic tools wiki page at
    http://qwiki.qualcomm.com/qct-drvsw/ADC_Diagnostic_Tools
 
  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/test/protected/AdcTest.h#1 $
 
                Copyright (c) 2009 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "AdcDiagCommon.h"

DALResult AdcDiagDAL_Init(void);

DALResult AdcDiagDAL_DeInit(void);

#endif /* ADCDIAGDAL_H */
