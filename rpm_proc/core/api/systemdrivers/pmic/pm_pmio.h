#ifndef PM_PMIO_H
#define PM_PMIO_H
/*
===========================================================================
*/
/**
  @file pm_pmio.h

  Public interface include file for accessing the PMIO definitions.

  The pm_pmio.h file is the public API interface to the PMIC I/O (PMIO)
  register access definitions.
*/
/*
  ====================================================================

  Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietaty - Qualcomm Technologies, Inc.

  ====================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_pmio.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*
  The following functions must be defined by the client in order to use
  the PMIO macros. Replace "ERR_TYPE" with the appropriate error type.

  static inline ERR_TYPE in_pmio (void *ctxt, uint8 pmic, uint32 addr, uint8 *val);
  static inline ERR_TYPE out_pmio (void *ctxt, uint8 pmic, uint32 addr, uint8 val);
  static inline ERR_TYPE in_pmio_field (void *ctxt, uint8 pmic, uint32 addr, uint8 mask, uint8 shift, uint8 *val);
  static inline ERR_TYPE out_pmio_field (void *ctxt, uint8 pmic, uint32 addr, uint8 mask, uint8 shift, uint8 val);
*/

/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */

/**
  @addtogroup pmio_macros
  @{
*/

/**
 * @name Address Macros
 * These macros are used to retrieve the address of a register.
 * @{
 */
/** Returns the directly accessible register address (virtual or physical based
 * on environment), PMIO_PHYS* returns the physical address. */
#define PMIO_ADDR(pmiosym)    __pmio_addr(pmiosym)
/** Retrieves the offset from the base region of a register address. */
#define PMIO_OFFS(pmiosym)    __pmio_offs(pmiosym)
/** @} */

/**
 * @name Input Macros
 * These macros are used for reading from a named PMIC register. The "M" suffix
 * indicates that the input is masked with the supplied mask.
 *
 * @note1hang Typically use either PMIO_IN or PMIO_INF (with required indexing).
 *
 * @{
 */

/** Reads from a named PMIC register. */
#define PMIO_IN(ctxt, pmic, pmiosym, val)                   __pmio_in(ctxt, pmic, pmiosym, val)

/** Reads from a named PMIC register and enables masking the input with
 *  the supplied mask.
 */
#define PMIO_INM(ctxt, pmic, pmiosym, mask, val)            __pmio_inf(ctxt, pmic, pmiosym, mask, 0, val)

/** Takes a field name, conducts the appropriate masking and shifting,
 *  and returns only the value of that field. 
 */
#define PMIO_INF(ctxt, pmic, pmiosym, field, val)           __pmio_inf(ctxt, pmic, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)

/** Reads from a named PMIC register. Used for explicit addressing. */
#define PMIO_INX(ctxt, pmic, base, pmiosym, val)            __pmio_inx(ctxt, pmic, base, pmiosym, val)

/** Reads from a named PMIC register and enables masking the input with
 *  the supplied mask. Used for explicit addressing.
 */
#define PMIO_INMX(ctxt, pmic, base, pmiosym, mask, val)     __pmio_infx(ctxt, pmic, base, pmiosym, mask, 0, val)

/** Takes a field name, conducts the appropriate masking and shifting,
 *  and returns only the value of that field. Used for explicit addressing.
 */
#define PMIO_INFX(ctxt, pmic, base, pmiosym, field, val)    __pmio_infx(ctxt, pmic, base, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)
/** @} */

/**
 * @name Output Macros
 *
 * These macros are used for writing to a named PMIC register.
 *
 * Macros with an "M" extension mask the output with the supplied
 * mask, i.e., the macros read, mask in the supplied data, then
 * write it back.
 *
 * Macros with an "X" extension are used for explicit addressing where
 * the base address of the module is provided as an argument to
 * the macro.
 *
 * @note1hang Typically use either PMIO_OUT or PMIO_OUTF (with required indexing).
 *
 * @{
 */
/** Writes to a named PMIC register. */
#define PMIO_OUT(ctxt, pmic, pmiosym, val)                  __pmio_out(ctxt, pmic, pmiosym, val)

/** The mask value for accessing a field in a register. Output is
 *  masked with the supplied mask. 
 */
#define PMIO_OUTM(ctxt, pmic, pmiosym, mask, val)           __pmio_outf(ctxt, pmic, pmiosym, mask, 0, val)

/** Takes a field name, and then masks and shifts it so the output is
 *  only the value of that field.
 */
#define PMIO_OUTF(ctxt, pmic, pmiosym, field, val)          __pmio_outf(ctxt, pmic, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)

/** Writes to a named PMIC register. Used for explicit addressing. */
#define PMIO_OUTX(ctxt, pmic, base, pmiosym, val)           __pmio_outx(ctxt, pmic, base, pmiosym, val)

/** The mask value for accessing a field in a register. Output is
  * masked with the supplied mask. Used for explicit addressing. 
  */
#define PMIO_OUTMX(ctxt, pmic, base, pmiosym, mask, val)    __pmio_outfx(ctxt, pmic, base, pmiosym, mask, val)

/** Takes a field name, and then masks and shifts it so the output is
 *  only the value of that field. Used for explicit addressing.
 */
#define PMIO_OUTFX(ctxt, pmic, base, pmiosym, field, val)   __pmio_outfx(ctxt, pmic, base, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)
/** @} */

/**
 * @name Shift and Mask Macros
 *
 * Macros for getting shift and mask values for fields and registers.
 *
 * @{
 */
/** Mask value for accessing an entire register.
 *
 *  <b>Example</b>
 *  @code
 *  PMIO_RMSK(REG) -> 0xFFFFFFFF  @endcode 
 */
#define PMIO_RMSK(pmiosym)                    __pmio_rmsk(pmiosym)

/** Right-shift value for accessing a field in a register.
 *  <b>Example</b>
 *  @code
 *  PMIO_SHFT(REG, FLD) -> 8  @endcode
 */
#define PMIO_SHFT(pmio_regsym, pmio_fldsym)   __pmio_shft(pmio_regsym, pmio_fldsym)

/** Mask value for accessing a field in a register. 
 *  
 *  <b>Example</b>
 *  @code
 *  PMIO_FMSK(REG, FLD) -> 0xFF00  @endcode
 */
#define PMIO_FMSK(pmio_regsym, pmio_fldsym)   __pmio_fmsk(pmio_regsym, pmio_fldsym)

/** Value for a field in a register.
 *  
 *  <b>Example</b>
 *  @code
 *  PMIO_VAL(REG, FLD, ON) -> 0x1  @endcode
 */
#define PMIO_VAL(pmiosym, field, val)         __pmio_val(pmiosym, field, val)

/** Takes a numerical value and shifts and masks it into the given field position.
 *
 *  <b>Example</b>
 *  @code
 *  PMIO_FVAL(REG, FLD, 0x1) -> 0x100  @endcode
 */
#define PMIO_FVAL(pmiosym, field, val)        (((uint32)(val) << PMIO_SHFT(pmiosym, field)) & PMIO_FMSK(pmiosym, field))
/** @} */

/** @} */ /* end_addtogroup pmio_macros */

/** @cond */

/*
 * Map to final symbols.  This remapping is done to allow register
 * redefinitions.  If we just define PMIO_IN(xreg) as PMIO_##xreg##_IN
 * then remappings like "#define xreg xregnew" do not work as expected.
 */
#define __pmio_in(ctxt, pmic, pmiosym, val)                         PMIO_##pmiosym##_IN(ctxt, pmic, val)
#define __pmio_inf(ctxt, pmic, pmiosym, mask, shift, val)           PMIO_##pmiosym##_INF(ctxt, pmic, mask, shift, val)
#define __pmio_out(ctxt, pmic, pmiosym, val)                        PMIO_##pmiosym##_OUT(ctxt, pmic, val)
#define __pmio_outf(ctxt, pmic, pmiosym, mask, shift, val)          PMIO_##pmiosym##_OUTF(ctxt, pmic, mask, shift, val)
#define __pmio_addr(pmiosym)                                        PMIO_##pmiosym##_ADDR
#define __pmio_offs(pmiosym)                                        PMIO_##pmiosym##_OFFS
#define __pmio_rmsk(pmiosym)                                        PMIO_##pmiosym##_RMSK
#define __pmio_fmsk(pmiosym, pmiofldsym)                            PMIO_##pmiosym##_##pmiofldsym##_BMSK
#define __pmio_shft(pmiosym, pmiofldsym)                            PMIO_##pmiosym##_##pmiofldsym##_SHFT
#define __pmio_val(pmiosym, pmiofld, pmioval)                       PMIO_##pmiosym##_##pmiofld##_##pmioval##_FVAL

#define __pmio_inx(ctxt, pmic, base, pmiosym, val)                  PMIO_##pmiosym##_IN(ctxt, pmic, base, val)
#define __pmio_infx(ctxt, pmic, base, pmiosym, mask, shift, val)    PMIO_##pmiosym##_INF(ctxt, pmic, base, mask, shift, val)
#define __pmio_outx(ctxt, pmic, base, pmiosym, val)                 PMIO_##pmiosym##_OUT(ctxt, pmic, base, val)
#define __pmio_outfx(ctxt, pmic, base, pmiosym, mask, shift, val)   PMIO_##pmiosym##_OUTF(ctxt, pmic, base, mask, shift, val)

/** @endcond */

#endif /* PM_PMIO_H */

