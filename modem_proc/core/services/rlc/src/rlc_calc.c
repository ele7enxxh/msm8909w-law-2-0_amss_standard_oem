#include "rlc.h"
#include "rlc_v.h"
#include "rlc_calc.h"


#if 0 
rlc_boolean rlc_calc_uint32( uint32                   *var1, 
                                     uint32                   *var2, 
                                     rlc_expression_type  exp)
{
  switch(exp)
  {
  case RLC_EXPRESSION_LESS_THAN:
    if(*var1 < *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_LESS_THAN_OR_EQUAL_TO:
    if(*var1 <= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_EQUAL_TO:
    if(*var1 == *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN:
    if(*var1 > *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN_OR_EQUAL_TO:
    if(*var1 >= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_NOT_EQUAL_TO:
    if(*var1 != *var2)
      return M_B_TRUE;
    break;
  default:
    break;
  }
  return M_B_FALSE;    
}

rlc_boolean rlc_calc_uint64( uint64                   *var1, 
                                     uint64                   *var2, 
                                     rlc_expression_type  exp)
{
  switch(exp)
  {
  case RLC_EXPRESSION_LESS_THAN:
    if(*var1 < *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_LESS_THAN_OR_EQUAL_TO:
    if(*var1 <= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_EQUAL_TO:
    if(*var1 == *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN:
    if(*var1 > *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN_OR_EQUAL_TO:
    if(*var1 >= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_NOT_EQUAL_TO:
    if(*var1 != *var2)
      return M_B_TRUE;
    break;
  default:
    break;
  }
  return M_B_FALSE;    
}

rlc_boolean rlc_calc_int32( int32                   *var1, 
                                    int32                   *var2, 
                                    rlc_expression_type exp)
{
  switch(exp)
  {
  case RLC_EXPRESSION_LESS_THAN:
    if(*var1 < *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_LESS_THAN_OR_EQUAL_TO:
    if(*var1 <= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_EQUAL_TO:
    if(*var1 == *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN:
    if(*var1 > *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN_OR_EQUAL_TO:
    if(*var1 >= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_NOT_EQUAL_TO:
    if(*var1 != *var2)
      return M_B_TRUE;
    break;
  default:
    break;
  }
  return M_B_FALSE;    
}

rlc_boolean rlc_calc_int64( int64                   *var1, 
                                    int64                   *var2, 
                                    rlc_expression_type exp)
{
  switch(exp)
  {
  case RLC_EXPRESSION_LESS_THAN:
    if(*var1 < *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_LESS_THAN_OR_EQUAL_TO:
    if(*var1 <= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_EQUAL_TO:
    if(*var1 == *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN:
    if(*var1 > *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_GREATER_THAN_OR_EQUAL_TO:
    if(*var1 >= *var2)
      return M_B_TRUE;
    break;
  case RLC_EXPRESSION_NOT_EQUAL_TO:
    if(*var1 != *var2)
      return M_B_TRUE;
    break;
  default:
    break;
  }
  return M_B_FALSE;    
}

#endif
