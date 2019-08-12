#ifndef __SPMICOREHWIO_H__
#define __SPMICOREHWIO_H__

#include "SpmiCoreHwioGen.h"

// Compatibility with newer targets which have both master and slave bits
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_M_GENI_DISABLE_BMSK HWIO_PMIC_ARB_SPMI_GENI_CTRL_GENI_DISABLE_BMSK
#define HWIO_PMIC_ARB_SPMI_GENI_CTRL_M_GENI_DISABLE_SHFT HWIO_PMIC_ARB_SPMI_GENI_CTRL_GENI_DISABLE_SHFT

#endif
