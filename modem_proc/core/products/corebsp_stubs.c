
#include "PrngML.h"
#include "PrngEL.h" 
#include "PrngCL.h"
#include "smp2p.h"
#include "smsm.h"
#include "smd_lite.h"
#include "smd.h"
#include "sio.h"
#include "npa_resource.h"

/**Enable this flag to allow run of standalone builds (without TZ) */
uint32 g_prng_config_enable = 0;

PrngCL_Result_Type PrngCL_init(void)
{
   return PRNGCL_ERROR_NONE;
}

PrngCL_Result_Type PrngCL_getdata(uint8*  random_ptr,  uint16  random_len)
{
  PrngCL_Result_Type ret_val = PRNGCL_ERROR_NONE;

   
  if(!random_ptr || !random_len){
      return PRNGCL_ERROR_INVALID_PARAM; 
   }

  return ret_val;
}

PrngCL_Result_Type PrngCL_deinit(void)
{
   return PRNGCL_ERROR_NONE;
}

PrngEL_Result_Type PrngEL_ClkEnable(void) 
{ 
 PrngEL_Result_Type stat = PRNGEL_ERROR_NONE; 
 return stat; 
} 

/** 
* @brief This function disable PRNG Engine. 
* 
* @param None 
* 
* @return None 
* 
* @see PrngEL_ClkEnable 
* 
*/ 

PrngEL_Result_Type PrngEL_ClkDisable(void) 
{ 
 PrngEL_Result_Type stat = PRNGEL_ERROR_NONE; 
 return stat; 
}



PrngCL_Result_Type PrngCL_lite_init(void)
{

  return PRNGCL_ERROR_NONE;
}



PrngCL_Result_Type PrngCL_lite_deinit(void)
{
  return PRNGCL_ERROR_NONE;
}


PrngML_Result_Type PrngML_getdata_lite
(
  uint8*  random_ptr,
  uint16  random_len
)
{
  PrngML_Result_Type ret_val = PRNGML_ERROR_NONE;
  
  /* Input Sanity check */
  if(!random_ptr || !random_len)
  {
    return PRNGML_ERROR_INVALID_PARAM; 
  }


  return ret_val;
} /* PrngML_getdata_lite() */

PrngCL_Result_Type PrngCL_DAL_Clock_Disable( void )
{
   PrngCL_Result_Type    ePrngStatus = PRNGCL_ERROR_NONE;
 
   return ePrngStatus;
}

PrngCL_Result_Type PrngCL_DAL_Clock_Enable( void )
{
  PrngCL_Result_Type    ePrngStatus = PRNGCL_ERROR_NONE;
  
  return ePrngStatus;
}


PrngML_Result_Type PrngML_getdata(uint8 * random_ptr, uint16 random_len)
{
  PrngML_Result_Type ret_val = PRNGML_ERROR_NONE;
  
  if(!random_ptr || !random_len){
  	return PRNGML_ERROR_INVALID_PARAM; 
  }
  
  return ret_val;

}

