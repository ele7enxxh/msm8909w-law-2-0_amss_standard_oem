#ifndef __DDITEMPLATE_H__
#define __DDITEMPLATE_H__
/**
  @file <Filename> 
  @brief A one line description that will act as a brief description.
  Anything that follows the first line is treated as a detailed 
  description. Blank lines serve as paragraph markers.
   
*/ 
/*  
  ====================================================================
                             Edit History 
  
  when      who   what, where, why 
  --------  ----  ----------------------------------------------------
  01/23/45  EP    Created
  
  ====================================================================
 
  Copyright (c) <Year> Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential
  
  ==================================================================== 
*/ 


/** 
   @name [Constants group name] 
   The name tag doesnt have a separate brief and detailed description segment. 
   Anything that goes in here will be documented along with the defines list
   @addtogroup GroupName

   @{ 
*/ 

#define DEFINE0 0x0   /**< -- Description of define 0 */
#define DEFINE1 0X1   /**< -- Description of define 1 */
/** 
   @}
*/


/*----------------------------------------------------------------------------
  Start of Enumerations
----------------------------------------------------------------------------*/
/**
   @addtogroup <SubGroupName> 

   @{ */
/**
   Description for EnumType
  */

typedef enum
{
   ENUM0      = 0,      /**< -- Description of enum 0 */ 
   ENUM1        = 1,    /**< -- Description of enum 1 */    
}EnumType;

/** 
   @}
*/
 /*  End of enumerations */


/*----------------------------------------------------------------------------
  Start of Structures
----------------------------------------------------------------------------*/

/**
   @addtogroup <SubGroupName> 
    
   @{ 
*/ 

/**
    Brief Description for StructType
 */
typedef struct 
{
    uint32 dwField0;  /**< -- Description for field 0  */
    uint32 dwField1;  /**< -- Description for field 1  */
}StructType;

/** 
  @}  
*/ 
/*  End of Structures */


/*----------------------------------------------------------------------------
  Start of public APIs
----------------------------------------------------------------------------*/
/**
   @addtogroup <SubGroupName> 

   @{ */
    
/* ============================================================================
**  Function : Foobar
** ============================================================================
*/
/**
    @brief
    Provide a brief one-line description
    
    @detail
    Provide detailed description on what the API does. It can span multiple lines.
    Blank lines are treated as paragraph breaks.
    
    @param[in    ]  *p1    Input to the API                               
    @param[out   ]  *p2    Param modified/updated by the API              
    @param[in,out]  *p3    Param is both input as well output for the API 

    @dependencies
    If there are any dependencies for this API 
    
    @sideeffects
    Any side effects that may occur
    
    @return
    One of the following error codes 
    <ul> 
        <li> Error Code 1 – Description for Error code 1
        <li> Error Code n – Description for Error code n
    </ul> 

    
    @sa
    None
*/
uint32 Foobar(void *p1, void *p2, void *p3);
    

/** 
   @}
 */
 /*  End of public APIs */

#endif /*#ifndef __DDITEMPLATE_H__*/
