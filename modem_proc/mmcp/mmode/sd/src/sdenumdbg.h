/*===========================================================================

              S Y S T E M   D E T E R M I N A T I O N

      E N U M E R A T I O N   D E B U G G E R   M O D U L E

DESCRIPTION
  This header file contains the tools to modify original enumerations
  with ENUM/STRING macros so that at compile time, there will be two
  static variables generated, one in enum representation, and the other
  in string map representation. The string map representation is used
  exclusively to replace its corresponding integer value in order to
  enhance the message debug log. This module should be included in all
  files that the Macros are present in; note, it does not have a ifndef
  guard because it is supposed to be included twice for every file that
  requires the string message enhancement.

Copyright (c) 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdenumdbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/10   gm      SD_ENUM_UNDEFINED_ELEMENT definition changed
08/20/10   DQ      Initial release.

===========================================================================*/

/* This Macro should be applied to the name of the enumeration. */
#undef SD_BEGIN_ENUM

/* This Macro should be applied to the name of the script table. */
#undef SD_BEGIN_SCRIPT

/* This Macro should be applied to every body member of the enumeration. */
#undef SD_ENUM_ELEMENT

/* Used exclusively for tag elements of the script constructs in
** sdssscr.h, type sdssscr_con_e_type. These elements will have the
** prefix of SDSSSCR_CON_. In all other enum cases use SD_ENUM_ELEMENT */ 
#undef SD_CONSTR_ELEMENT

/* This Macro is used to surround any values that are necessary in the
** enumeration but would cause errors if they appeared in a string map.
** For example "sd_first value = 12," the = 12 would need to be tagged
** with this macro to avoid conflicts when converted to a string map. */
#undef SD_ENUM_ELEMENT_INDEX

/* This Macro should be applied to the end name of the enum. */
#undef SD_END_ENUM

/* Used to fill holes in the string map for when the enumeration skips
** to higher ranges. Used with UNDEFINED_CONSTRUCT as its parameter. */
#undef SD_ENUM_UNDEFINED_ELEMENT

/* Used as placeholder for undefined enum spots when the range changes
** in the middle of the enumeration. This is to keep consistency with
** the indexes of the string map representation. It is used in conjunction
** with SD_ENUM_UNDEFINED_ELEMENT. */
#undef UNDEFINED_CONSTRUCT
#define UNDEFINED_CONSTRUCT "" 

/*=====================================================================*/
/* 
** The following are two sets of macros used to tag enumerations in a
** header file. The featurization FEATURE_SD_ENABLE_DEBUG_STRINGS toggles
** which representation a given platform is generating. For example, if
** FEATURE_SD_ENABLE_DEBUG_STRINGS is defined, the set of macros that are
** used to interpret the code are distinctly associated with that of a
** string map. If the feature is not enabled (the default), it will be an
** enum representation.
*/ 

/* String Map Form */
#include "mmcp_variation.h"
#ifdef FEATURE_SD_ENABLE_DEBUG_STRINGS
  #define SD_BEGIN_ENUM( ENUM_NAME ) static const char* ENUM_NAME##_type_string_map [] =
  #define SD_BEGIN_SCRIPT( SCRIPT_NAME ) static const char* SCRIPT_NAME##_string_map [] =
  #define SD_ENUM_ELEMENT( element ) #element
  #define SD_CONSTR_ELEMENT( element ) #element
  #define SD_ENUM_ELEMENT_INDEX( index )  
  #define SD_ENUM_UNDEFINED_ELEMENT( element ) #element ,
  #define SD_END_ENUM( ENUM_NAME )  ;

/* Default Enumeration From*/
#else
  #define SD_BEGIN_ENUM( ENUM_NAME ) typedef enum ENUM_NAME
  #define SD_BEGIN_SCRIPT( SCRIPT_NAME ) static const sdssscr_elem_u_type * const SCRIPT_NAME [] = 
  #define SD_ENUM_ELEMENT( element ) element
  #define SD_CONSTR_ELEMENT( element ) SDSSSCR_CON_##element
  #define SD_ENUM_ELEMENT_INDEX( index ) index
  #define SD_ENUM_UNDEFINED_ELEMENT( element )
  #define SD_END_ENUM( ENUM_NAME ) ENUM_NAME ;
#endif
