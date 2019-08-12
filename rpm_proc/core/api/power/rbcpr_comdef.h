#ifndef RBCPR_COMDEF_H
#define RBCPR_COMDEF_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "comdef.h"

/* Temporary location for Features */
// Feature to enable RBCPR driver
#define FEATURE_POWER_RBCPR_ENABLE
// Feature to keep RBCPR "disabled" but read efuse voltage information

/* ============================================
FUNCTION RBCPR_INIT
DESCRIPTION
  Performs initialization of the RBCPR module

DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None
============================================ */

void rbcpr_init(void);

/* ============================================
FUNCTION RBCPR_SETTLE_CX_VOLTAGE_CORNERS
DESCRIPTION
  Steps through the SVS, Nominal and Super Turbo corners on Cx
  and allow CPR to settle the voltage at each corner.

DEPENDENCIES
  Requires that the Cx voltage can be stepped down to SVS.
  In order to do this, the clock driver must be first forced
  down to run clocks only at voltages that require SVS and also
  remove any votes it has for Cx to be higher than SVS.

RETURN VALUE
  Void

SIDE EFFECTS
  None
============================================ */

void rbcpr_settle_cx_voltage_corners(void);

/* ============================================
FUNCTION RBCPR_CX_CPR_ENABLED
DESCRIPTION
  Returns whether or not Cx CPR is enabled.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
============================================ */

bool rbcpr_cx_cpr_enabled(void);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif /* RBCPR_COMDEF_H */
