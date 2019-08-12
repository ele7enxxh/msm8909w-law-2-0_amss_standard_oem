/*===========================================================================

  rpm_ee.h - data model for an execution environment

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_EE_H
#define RPM_EE_H

#include <stdint.h>

#include "rpmserver.h"
#include "../common/inq.h"
#include "../common/inmap.h"
#include "../common/vect.h"

class EE
{
public:
    EE(unsigned id);

private:
};

class EEManager
{
public:
    EEManager();

    void addEE(const EEDescription &eed);

    EE &operator[](unsigned ee_id);
    EE *get(unsigned ee_id);

private:
    unsigned numEEs_;
    EE      *ees_;
};


#endif // RPM_EE_H

