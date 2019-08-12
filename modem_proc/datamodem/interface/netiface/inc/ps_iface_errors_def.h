#ifndef PS_IFACE_ERRORS_DEF_H
#define PS_IFACE_ERRORS_DEF_H
/*===========================================================================

              P S    I F A C E    E R R O R N O ' S

DESCRIPTION

 The ps framework Error codes header file.  Contains #defines for the various
 errors .

Copyright (c) 1998-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
                    Error Condition Values
---------------------------------------------------------------------------*/
#define PS_IFACE_ERR_NONE                                             0                            
#define PS_IFACE_ERR_NULL_ARG                                        -1 
#define PS_IFACE_ERR_INVALID_ARG                                     -2                
#define PS_IFACE_ERR_OUT_OF_SYSTEM_HEAP_MEM                          -3             
#define PS_IFACE_ERR_INVALID_PROTOCOL                                -4 
#define PS_IFACE_ERR_INVALID_TECH_TYPE                               -5 
#define PS_IFACE_ERR_DSM_EXTRACT_FAILURE                             -6 
#define PS_IFACE_ERR_OUT_OF_DIAG_BUF                                 -7
#define PS_IFACE_ERR_ASSOC_IFACE                                     -8
#define PS_IFACE_ERR_ZERO_COUNT                                      -9
#define PS_IFACE_ERR_INVALID_IFACE                                   -10 
#define PS_IFACE_ERR_INVALID_STATE                                   -11 
#define PS_IFACE_ERR_INVALID_OPERATION                               -12 


#endif /* PS_IFACE_ERRORS_DEF_H */