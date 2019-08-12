/*! 
 @file rflte_core_iq_capture.h 
 
 @brief 
  This is the header file for core functions for LTE IQ capture. 
 
 @details 
 
*/ 
  
/*=========================================================================== 
  
Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved. 
  
                           EDIT HISTORY FOR FILE 
  
This section contains comments describing changes made to this file. 
Notice that changes are listed in reverse chronological order. 

$Header: 
  
When       who     what, where, why 
------------------------------------------------------------------------------- 
08/31/13   bsh     Initial version. 
  
============================================================================*/ 
#ifndef RFLTE_CORE_IQ_CAPTURE_H
#define RFLTE_CORE_IQ_CAPTURE_H

#include "rflte_msg.h"

/*----------------------------------------------------------------------------*/
/* @TODO move to LTE FTM */
void  rflte_core_iq_capture_func_ptr_init(void);

/*----------------------------------------------------------------------------*/
/* @TODO move to LTE FTM */
void rflte_core_iq_capture (void *msg_ptr);

#endif 
