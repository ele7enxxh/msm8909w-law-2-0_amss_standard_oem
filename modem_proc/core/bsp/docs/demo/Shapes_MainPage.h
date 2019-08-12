#ifndef __SHAPES_MAINPAGE_H__
#define __SHAPES_MAINPAGE_H__

/*  
  ====================================================================
                             Edit History 
  
  when      who   what, where, why 
  --------  ----  ----------------------------------------------------
  05/04/10  MK    Created
  
  ====================================================================
 
  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential
 
  ==================================================================== 
*/ 

/**
@mainpage Shapes API Documentation

@section S1 Introduction

[This section should talk briefly about the API itself. Should give a brief 
overview of what the API enables.] Shapes API provides a framework to 
manipulate various geometrical shapes like Rectangles,circles etc. It also has 
the ability to use the underlying GPU to accelerate the processing of the shapes. 

@section S2 Another Section
Multiple sections can be added using the "section" doxygen tag.

@subsection SS21 A Sub Section 
To better document the introduction, one can also use subsection tags. This will 
appear under the "Another Section" Section. 

*/

/** 
@defgroup shapes_apis Shapes APIs 
This group groups all the APIs in the Shapes API module. 
 
@defgroup shapes_mandatory_apis Mandatory APIs 
@addtogroup shapes_apis 
This is an example group which groups mandatory APIs.
 
@defgroup shapes_cfg_apis Configuration APIs  
@addtogroup shapes_apis 
This example section groups all the APIs used to configure the Shapes API
 
@defgroup shapes_oper_apis Operational APIs
@addtogroup shapes_apis 
This example section groups all the APIs that manipulate and draw shapes 
supported by the Shapes API
 
@defgroup shapes_enums Enumerated Constants  
@addtogroup shapes_apis 
This section defines the various enumerated constants. Please note that all the 
enums defined here are actually typedef'ed enums and can be used without the 
explicit enum keyword. 
 
@defgroup shapes_datastructs Data Structures 
@addtogroup shapes_apis 
This section defines the various data structures published in the Shapes API. 
Please note that all the structs defined here are actually typedef'ed struct and 
can be used without the explicit struct keyword. 
 
*/ 

#endif /* #ifndef __SHAPES_MAINPAGE_H__ */
