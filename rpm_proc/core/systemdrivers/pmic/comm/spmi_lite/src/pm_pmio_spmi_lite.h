#ifndef PM_PMIO_SPMI_LITE_H
#define PM_PMIO_SPMI_LITE_H

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_pmio.h"
#include "pm_comm.h"
#include "com_dtypes.h"

/*=========================================================================
     PMIO Function Implementation
==========================================================================*/

/* =========================================================================
**  Function : in_pmio
** =========================================================================*/
/**
  Reads a PMIC register.
*/
static inline pm_err_flag_type in_pmio
(
  void    *ctxt,
  uint8    pmic,
  uint32   addr,
  uint8   *val
)
{
  return pm_comm_read_byte(pmic, addr & 0xFFFF, val, 0);

} /* END in_pmio */

/* =========================================================================
**  Function : out_pmio
** =========================================================================*/
/**
  Writes a PMIC register.
*/

static inline pm_err_flag_type out_pmio
(
  void    *ctxt,
  uint8    pmic,
  uint32   addr,
  uint8    val
)
{
  return pm_comm_write_byte(pmic, addr & 0xFFFF, val, 0);

} /* END out_pmio */


/* =========================================================================
**  Function : in_pmio_field
** =========================================================================*/
/**
  Reads a field from a PMIC register.
*/
static inline pm_err_flag_type in_pmio_field
(
  void    *ctxt,
  uint8    pmic,
  uint32   addr,
  uint8    mask,
  uint8    shift,
  uint8   *val
)
{
  pm_err_flag_type err = in_pmio(ctxt, pmic, addr, val);
  *val = (*val & mask) >> shift;
  return err;

} /* END in_pmio_field */


/* =========================================================================
**  Function : out_pmio_field
** =========================================================================*/
/**
  Writes to a PMIC register field.
*/
static inline pm_err_flag_type out_pmio_field
(
  void    *ctxt,
  uint8    pmic,
  uint32   addr,
  uint8    mask,
  uint8    shift,
  uint8    val
)
{
  uint8 reg;
  pm_err_flag_type err;

  if ((err = in_pmio(ctxt, pmic, addr, &reg)) != PM_ERR_FLAG__SUCCESS)
  {
    return err;
  }

  return out_pmio(ctxt, pmic, addr, (reg & ~mask) | (val << shift));

} /* END out_pmio_field */

#endif /* PM_PMIO_SPMI_LITE_H */

