#ifndef __TEMPLATE_MAINPAGE_H__
#define __TEMPLATE_MAINPAGE_H__

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
  @mainpage <API Name> Documentation
  
  @section <SectionLabel> <Section Name>

  [This section should talk briefly about the API itself. Should give a brief 
  overview of what the API enables.] SectionLabel is useful when a particular 
  section needs to be referenced.
 
  Multiple sections can be added using the "section" doxygen tag.
  
  @subsection <SubSectionLabel> <SubSection Name> 
  To better document the introduction, one can also use subsection tags. This will 
  appear under the Section defined above the subsection. 
  
*/

/** 
  @defgroup <GroupName> <Group Title> 
  This group groups all the APIs in the Shapes API module. 
 
  @defgroup <SubgroupName> <SubGroupTitle> 
  @addtogroup <GroupName> 
  This is an example Sub group which groups a certain set of code blocks. It is 
  also grouped under the GroupName using the ingroup tag. 
   
*/ 

#endif /* #ifndef __TEMPLATE_MAINPAGE_H__ */
