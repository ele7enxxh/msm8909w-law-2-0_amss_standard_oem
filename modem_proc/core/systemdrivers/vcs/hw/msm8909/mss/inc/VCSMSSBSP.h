#ifndef __VCSMSSBSP_H__
#define __VCSMSSBSP_H__
/*
===========================================================================
*/
/**
  @file VCSMSSBSP.h 
  
  Header description for the MSS VCS driver BSP format.
*/
/*  
  ====================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/hw/msm8909/mss/inc/VCSMSSBSP.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/22/14   lil     Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include "VCSBSP.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * VCSImageBSPConfigType
 *
 * BSP data structure for describing the image configuration.
 *
 *  bEnableMVC - MVC enable switch.
 */
typedef struct VCSImageBSPConfigType
{
  boolean bEnableMVC;
} VCSImageBSPConfigType;


#endif  /* __VCSMSSBSP_H__ */ 

