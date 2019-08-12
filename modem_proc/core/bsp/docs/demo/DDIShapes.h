#ifndef __DDISHAPES_H__
#define __DDISHAPES_H__

/**
  @file DDIShapes.h 
  @brief 
      This file contains the definitions of the constants, data 
  structures, and interfaces that comprise the Shapes API. 
  Only the interfaces declared shall be used by the client for
  accessing the Shapes API. 
  
  Anything that starts after a blank line is a detailed description. 
  This is also a sample header file that serves as a template as a 
  sample documentation for doxygen. Make sure you replace the 
  DDIShapes.h above with the name of the header file 
*/ 
/*  
  ====================================================================
                             Edit History 
  
  when      who   what, where, why 
  --------  ----  ----------------------------------------------------
  04/26/10  MK    Created
  
  ====================================================================
 
  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential
 
  ==================================================================== 
*/ 


/** 
   @name Return Codes 
   The following constants are grouped as Return Codes using the "name" doxygen 
   tag
   @addtogroup shapes_apis

   @{ 
*/ 

#define SHAPES_SUCCESS 0x0      /**< -- The API Succeeded */
#define SHAPES_ERROR   0x8001   /**< -- The API Failed for some reason */
/** 
   @}
*/


/*----------------------------------------------------------------------------
  Start of Enumerations
----------------------------------------------------------------------------*/


/**
   @addtogroup shapes_enums 

   @{ */
/**
  Various shapes supported by the Shapes API
  */

typedef enum
{
   SHAPE_SQUARE      = 0,      /**< -- quadrilateral with four right angles and unequal lengths   */ 
   SHAPE_RECT        = 1,      /**< -- quadrilateral with four right angles and equal lengths  */ 
   SHAPE_CIRCLE      = 2,      /**< -- Curve with equal major and minor radius   */ 
   SHAPE_ELLIPSE     = 3,      /**< -- Curve with unequal major and minor radius */ 
}Shapes_ShapeType;

/**
  Debug Levels used by the Shapes API to display messages. 
 */
typedef enum
{
    DBGLVL_ALL      = 0,   /**< -- Everything gets printed   */
    DBGLVL_INFO     = 1,   /**< -- Informative messages  */
    DBGLVL_WARN     = 2,   /**< -- Warning messages  */
    DBGLVL_ERROR    = 3,   /**< -- Non-critical error messages  */
    DBGLVL_FATAL    = 4,   /**< -- Critical error messages.  */
}Shapes_DbgLvlType;

/** 
   @}
*/
 /*  End of enumerations */


/*----------------------------------------------------------------------------
  Start of Structures
----------------------------------------------------------------------------*/

/**
   @addtogroup shapes_datastructs 
    
   @{ 
*/ 

/**
    A structure that represents a point 
 */
typedef struct 
{
    uint32 x;  /**< -- The X coordinate  */
    uint32 y;  /**< -- The Y coordinate  */
}Shapes_PtType;

/**
	A structure that represents a rectangle
*/
typedef struct 
{ 
    Shapes_PtType mTopLeft;     /**< -- Point that represents the top left corner 
                                    of the rectangle */
    Shapes_PtType mBottomRight; /**< -- Point that represents the bottom right 
                                    corner of the rectangle */
}Shapes_RectType;

/** 
  @}  
*/ 
/*  End of Structures */


/*----------------------------------------------------------------------------
  Start of Mandatory APIs
----------------------------------------------------------------------------*/
/**
   @addtogroup shapes_mandatory_apis 

   @{ */


/* ============================================================================
**  Function : Shapes_Init
** ============================================================================
*/
/**
    @brief Initializes the Shapes API
    
    @description This function initializes various internal modules that manage the shapes
    and also powers on the GPU and restores it to a known default state.
    
    @param[out] *pHandle   Pointer to the handle structure that will be updated
                           upon successful initialization
    
    @dependencies
    None
    
    @sideeffects
    None
    
    @return
    Returns one of the following values. The values can be nicely presented
    using item tags as follows:
    <ul>
        <li> SHAPES_SUCCESS </li>
        <li> SHAPES_ERROR </li>
    </ul>
             
    
    @sa
    Shapes_DeInit
    */
    
    uint32 Shapes_Init(void *handle);
    

/* ============================================================================
**  Function : Shapes_DeInit
** ============================================================================
*/
/**
    @brief
    Deinitializes the Shapes API
    
    This function de-initializes various internal modules that manage the shapes
    and also powers off the GPU.
    
    @param[in,out] *pHandle   Pointer to the handle structure that will be cleared
                              upon successful de-initialization

    @dependencies
    Shapes_Init must have been called prior to this function
    
    @sideeffects
    Any Shapes APIs that were being executed at the time of deinit will have
    undefined behavior. So care must be taken to ensure all pending operations
    were completed prior to the de-initialization.
    
    @return
    Returns one of the following values. The values can be nicely presented
    using item tags as follows:
    <ul>
        <li> SHAPES_SUCCESS </li>
        <li> SHAPES_ERROR </li>
    </ul>
             
    
    @sa
    Shapes_Init
*/
uint32 Shapes_DeInit(void *pHandle);

 /** 
   @}
 */
 /*  End of Mandatory APIs */

/*----------------------------------------------------------------------------
  Start of Configuration APIs
----------------------------------------------------------------------------*/
/**
   @addtogroup shapes_cfg_apis  

   @{ */

/* ============================================================================
**  Function : Shapes_EnableGPU
** ============================================================================
*/
/**
    @brief
    Enable/disables the GPU
    
    @detail
    Enables or disables the GPU based on the value of bEnable. If bEnable is
    TRUE GPU is enabled, disabled otherwise.
    
    @param[in]  bEnable    Boolean to enable the GPU 

    @dependencies
    Shapes_Init must have been called prior to this function
    
    @sideeffects
    None
    
    @return
    None
    
    @sa
    Shapes_GetGPUState
*/
void Shapes_EnableGPU(bool bEnable);
  
/* ============================================================================
**  Function : Shapes_GetGPUState
** ============================================================================
*/
/**
    @brief
    Get the state of the GPU
    
    @detail
    Get the Graphics Processing Unit's State
    

    @dependencies
    Shapes_Init must have been called prior to this function
    
    @sideeffects
    None
    
    @return
    Returns TRUE if GPU is enabled; FALSE Otherwise.
    
    @sa
    Shapes_EnableGPU    
*/
bool Shapes_GetGPUState();
  
/* ============================================================================
**  Function : Shapes_SetDbgLvl
** ============================================================================
*/
/**
    @brief
    Sets the current debug level
    
    @detail
    Sets the debug level for the Shapes API
    
    @param[in]  eDbgLvl    Debug level to be used by the Shapes API

    @dependencies
    Shapes_Init must have been called prior to this function
    
    @sideeffects
    None
    
    @return
    None.
    
    @sa
    Shapes_GetDbgLvl
*/
void Shapes_SetDbgLvl(Shapes_DbgLvlType eDbgLvl);

/* ============================================================================
**  Function : Shapes_GetDbgLvl
** ============================================================================
*/
/**
    @brief
    Gets the current debug level
    
    @detail
    Gets the debug level current configured with the Shapes API
    

    @dependencies
    Shapes_Init must have been called prior to this function
    
    @sideeffects
    None
    
    @return
    returns the debug level current configured with the Shapes API.
    
    @sa
    Shapes_SetDbgLvl
*/
Shapes_DbgLvlType Shapes_GetDbgLvl();

 /** 
   @}
 */
 /*  End of Configuration APIs */

/*----------------------------------------------------------------------------
  Start of Operational APIs
----------------------------------------------------------------------------*/
/**
   @addtogroup shapes_oper_apis 

   @{ */

/* ============================================================================
**  Function : Shapes_DrawPoint
** ============================================================================
*/
/**
    @brief
    Draws a point
    
    @detail
    Draws a point using mPt for its co-ordinates.
    
    @param[in]  *pHandle    Pointer to the handle from Shapes_Init 
    @param[in]  mPt         Point to be drawn                  

    @dependencies
    Shapes_Init must have been called prior to this function
       
    @sideeffects
    None
    
    @return
    None
    
    @sa
    Shapes_DrawRect
*/
void Shapes_DrawPoint(void *pHandle, Shapes_PtType mPt);

/* ============================================================================
**  Function : Shapes_DrawRect
** ============================================================================
*/
/**
    @brief
    Draws a rectangle
    
    @detail
    Draws a rectangle using mRect for its co-ordinates.
    
    @param[in]  *pHandle    Pointer to the handle from Shapes_Init
    @param[in]  mRect       Rectangle to be drawn

    @dependencies
    Shapes_Init must have been called prior to this function
    
    @sideeffects
    None
    
    @return
    None
    
    @sa
    Shapes_DrawPoint
*/
void Shapes_DrawRect(void *pHandle, Shapes_RectType mRect);

/* ============================================================================
**  Function : Shapes_GetRectPerimeter
** ============================================================================
*/
/**
    @brief
    Gets the perimeter of the rectangle.
    
    @detail
    Computes the perimeter of the given rectangle using the following formula:
    [Doxygen Allows us to use LaTeX Formula for LaTeX/PDF only outputs]
    \f$ perimeter = 2 * (length + width) \f$ where \f$ length =
    mRect.mTopLeft.x - mRect.mBottomRight.x \f$ and \f$ width =
    mRect.mTopLeft.y - mRect.mBottomRight.y \f$ 
    
    @param[in ]  *pHandle        Pointer to the handle from Shapes_Init
    @param[in ]  mRect           Rectangle whose perimeter to be computed
    @param[out]  *dwPerimeter    Pointer to store the perimeter value

    @dependencies
    Shapes_Init must have been called prior to this function
   
    @sideeffects
    None
    
    @return
    None
    
    @sa
    None
*/
void Shapes_GetRectPerimeter(void *pHandle, Shapes_RectType mRect, uint32 * dwPerimeter);

/** 
   @}
 */
 /*  End of Operational APIs */

#endif /* #ifndef __DDISHAPES_H__ */
