#ifndef QMI_BIT_H
#define QMI_BIT_H
/*============================================================================
  @file qmi_bit.h

  @brief
    This module binds the QMI-BIT to the QCSI framework.  

    
    
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/inc/qmi_bit.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  -----------------------------------------------------------
2012-07-15  spn  Initial revision.


=============================================================================*/

/*****************************************************************************
 * Include Files
 * *************************************************************************/

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * *************************************************************************/



/*****************************************************************************
 * Function Declarations and Documentation
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to initialize the module, so it can attach itself
  with QCSI Framework and initialize the QMI-BIT Service.

@retval  TRUE  - Successful initialization of the module
@retval  FALSE - Module initialization was not successful
*//* ------------------------------------------------------------------------*/
boolean qmiBitInit
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to reset and disconnect the shim layer from the QCSI
  framework and de-register the QMI-BIT Service. After this function call, 
  the PDComms task will need to call qmiBitInit() to be able to 
  communicate over the QMI-framework. 
*//* ------------------------------------------------------------------------*/
void qmiBitCleanup 
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by the Service owner indicating that there is a 
  QMI-Event that is queued up in QCSI that needs to be handled.
*//* ------------------------------------------------------------------------*/
void qmiBitHandleEvent 
( 
  void 
);


#endif /* #ifndef QMI_BIT_H */


