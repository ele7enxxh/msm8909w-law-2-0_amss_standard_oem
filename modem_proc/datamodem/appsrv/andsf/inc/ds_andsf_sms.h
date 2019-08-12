/*===========================================================================
                      DS_ANDSF_SMS.H

DESCRIPTION
  sms header file for ANDSF.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
06/09/14    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_SMS_H
#define DS_ANDSF_SMS_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_POLICY

/*===========================================================================
                    EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/
void  ds_andsf_sms_init(void);


#endif /* FEATURE_DATA_ANDSF_POLICY */
#endif /* DS_ANDSF_SMS_H */
