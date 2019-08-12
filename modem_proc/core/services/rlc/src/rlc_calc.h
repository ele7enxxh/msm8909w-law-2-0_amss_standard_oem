#ifndef RLC_CALC_H
#define RLC_CALC_H

#include "rlc.h"

/*
  RLC_VARIABLE_NONE = 0,
  RLC_DYNAMIC_VARIABLE_UINT32,
  RLC_DYNAMIC_VARIABLE_UINT64,
  RLC_DYNAMIC_VARIABLE_INT32,
  RLC_DYNAMIC_VARIABLE_INT64,
  RLC_DYNAMIC_VARIABLE_FROM_FN_PTR_32,
  RLC_DYNAMIC_VARIABLE_FROM_FN_PTR_64,
  RLC_FIXED_SIGNED_32,
  RLC_FIXED_SIGNED_64,
  RLC_FIXED_UNSIGNED_32,
  RLC_FIXED_UNSIGNED_64,
  RLC_VARIABLE_MAX,
  */
#if 0
rlc_boolean rlc_calc_uint32( uint32 *var1, 
                                     uint32 *var2, 
                                     rlc_expression_type exp);

rlc_boolean rlc_calc_uint64( uint64 *var1, 
                                     uint64 *var2, 
                                     rlc_expression_type exp);

rlc_boolean rlc_calc_int32( int32 *var1, 
                                    int32 *var2, 
                                    rlc_expression_type exp);

rlc_boolean rlc_calc_int64( int64 *var1, 
                                    int64 *var2, 
                                    rlc_expression_type exp);

#endif
#endif /*RLC_CALC_H*/
