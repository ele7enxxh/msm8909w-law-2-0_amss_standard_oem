#ifndef UIMDRV_MSG_H
#define UIMDRV_MSG_H
/*===========================================================================


           U I M DRIVER  L O G   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
07/22/16   ks      Correcting the MSG MACROs
05/23/16   na      Support for additional arg
07/31/14   ak      Feature flags cleanup
29/01/14   sam     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_msg.h"

/*=============================================================================

                          UIM MSG MACROS
               MACRO Definitions used for logging throughout UIM

=============================================================================*/

#define UIMDRV_MSG_HIGH_0(sub, str) \
  UIM_MSG_HIGH_1("UIM_%d: " str , (sub+1))

#define UIMDRV_MSG_HIGH_1(sub, str, p1) \
  UIM_MSG_HIGH_2("UIM_%d: " str , (sub+1), (p1))

#define UIMDRV_MSG_HIGH_2(sub, str, p1, p2) \
  UIM_MSG_HIGH_3("UIM_%d: " str , (sub+1), (p1), (p2))

#define UIMDRV_MSG_HIGH_3(sub, str, p1, p2, p3) \
  UIM_MSG_HIGH_4("UIM_%d: " str , (sub+1), (p1), (p2), (p3))

#define UIMDRV_MSG_HIGH_4(sub, str, p1, p2, p3, p4) \
  UIM_MSG_HIGH_5("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4))

#define UIMDRV_MSG_HIGH_5(sub, str, p1, p2, p3, p4, p5) \
  UIM_MSG_HIGH_6("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5))

#define UIMDRV_MSG_HIGH_6(sub, str, p1, p2, p3, p4, p5, p6) \
  UIM_MSG_HIGH_7("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6))

#define UIMDRV_MSG_HIGH_7(sub, str, p1, p2, p3, p4, p5, p6, p7) \
  UIM_MSG_HIGH_8("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7))


/*Slot specific UIMDRV MED message macro definitions*/
#define UIMDRV_MSG_MED_0(sub, str) \
  UIM_MSG_MED_1("UIM_%d: " str , (sub+1))

#define UIMDRV_MSG_MED_1(sub, str, p1) \
  UIM_MSG_MED_2("UIM_%d: " str , (sub+1), (p1))

#define UIMDRV_MSG_MED_2(sub, str, p1, p2) \
  UIM_MSG_MED_3("UIM_%d: " str , (sub+1), (p1), (p2))

#define UIMDRV_MSG_MED_3(sub, str, p1, p2, p3) \
  UIM_MSG_MED_4("UIM_%d: " str , (sub+1), (p1), (p2), (p3))

#define UIMDRV_MSG_MED_4(sub, str, p1, p2, p3, p4) \
  UIM_MSG_MED_5("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4))

#define UIMDRV_MSG_MED_5(sub, str, p1, p2, p3, p4, p5) \
  UIM_MSG_MED_6("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5))

#define UIMDRV_MSG_MED_6(sub, str, p1, p2, p3, p4, p5, p6) \
  UIM_MSG_MED_7("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6))

#define UIMDRV_MSG_MED_7(sub, str, p1, p2, p3, p4, p5, p6, p7) \
  UIM_MSG_MED_8("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7))


/*Slot specific UIMDRV LOW message macro definitions*/
#define UIMDRV_MSG_LOW_0(sub, str) \
  UIM_MSG_LOW_1("UIM_%d: " str , (sub+1))

#define UIMDRV_MSG_LOW_1(sub, str, p1) \
  UIM_MSG_LOW_2("UIM_%d: " str , (sub+1), (p1))

#define UIMDRV_MSG_LOW_2(sub, str, p1, p2) \
  UIM_MSG_LOW_3("UIM_%d: " str , (sub+1), (p1), (p2))

#define UIMDRV_MSG_LOW_3(sub, str, p1, p2, p3) \
  UIM_MSG_LOW_4("UIM_%d: " str , (sub+1), (p1), (p2), (p3))

#define UIMDRV_MSG_LOW_4(sub, str, p1, p2, p3, p4) \
  UIM_MSG_LOW_5("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4))

#define UIMDRV_MSG_LOW_5(sub, str, p1, p2, p3, p4, p5) \
  UIM_MSG_LOW_6("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5))

#define UIMDRV_MSG_LOW_6(sub, str, p1, p2, p3, p4, p5, p6) \
  UIM_MSG_LOW_7("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6))

#define UIMDRV_MSG_LOW_7(sub, str, p1, p2, p3, p4, p5, p6, p7) \
  UIM_MSG_LOW_8("UIM_%d: " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7))
  

/*Slot specific UIMDRV ERROR message macro definitions*/
#define UIMDRV_MSG_ERR_0(sub, str) \
  UIM_MSG_ERR_1("UIM_%d: " str , (sub+1))

#define UIMDRV_MSG_ERR_1(sub, str, p1) \
  UIM_MSG_ERR_2("UIM_%d: " str , (sub+1), (p1))

#define UIMDRV_MSG_ERR_2(sub, str, p1, p2) \
  UIM_MSG_ERR_3("UIM_%d: " str , (sub+1), (p1), (p2))

#define UIMDRV_MSG_ERR_3(sub, str, p1, p2, p3) \
  UIM_MSG_ERR_4("UIM_%d: " str , (sub+1), (p1), (p2), (p3))

#define MSG_FATAL_UIM( sub , str, p1, p2, p3)\
MSG_4 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, "UIM_%d" str , (sub+1), (p1), (p2), (p3))


#define UIM_ERROR_MSG_0(str) UIM_MSG_ERR_0(str)
#define UIM_ERROR_MSG_1(str,a) UIM_MSG_ERR_1(str,a)
#define UIM_ERROR_MSG_2(str,a,b) UIM_MSG_ERR_2(str,a,b)
#define UIM_ERROR_MSG_3(str,a,b,c) UIM_MSG_ERR_3(str,a,b,c)


#define UIMDRV_USB_MSG_ERR_0(str)   UIM_MSG_ERR_0("UIM USB:" str)
#define UIMDRV_USB_MSG_ERR_1(str,a)   UIM_MSG_ERR_1("UIM USB:" str,a,0)
#define UIMDRV_USB_MSG_ERR_2(str,a,b)   UIM_MSG_ERR_2("UIM USB:" str,a,b)
#define UIMDRV_USB_MSG_ERR_3(str,a,b,c)   UIM_MSG_ERR_3("UIM USB:" str,a,b,c)


#endif /* UIMDRV_MSG_H */
