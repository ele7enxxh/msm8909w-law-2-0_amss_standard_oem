/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "mmpm.h"
#include "adsppm_qdi.h"
/**
@struct Adsppmcb_Elem_Type_Srv
call back info queue for each registered client
*/
typedef struct Adsppmcb_Elem_Type_Srv{
   struct Adsppmcb_Elem_Type_Srv *next;
   QDI_Adsppm_cbinfo_t arg;
}Adsppmcb_Elem_Type_Srv;

/**
@fn Adsppmcb_Init_Server  -- init adsppm qdi server
*/

void Adsppmcb_Init_Server(void);

/**
@fn Adsppmcb_register_callback_Srv -- register call back funciton for QDI clients
*/
void Adsppmcb_register_callback_Srv( QDI_Adsppm_cbinfo_t *arg);

/**
@fn Adsppmcb_deRegister_callback_Srv --deregister call back function for QDI clients
*/
void Adsppmcb_deRegister_callback_Srv(int clientId);

/**
@fn Adsppmcb_notify_callbacks_Srv  -- call back function from asynmgr
*/
uint32 Adsppmcb_notify_callbacks_Srv(MmpmCallbackParamType* pCallbackValue);

/**
@fn Adsppmcb_Deinit_Srv -- deinit adsppm qdi server
*/
void Adsppmcb_Deinit_Srv(void);

/**
 * @fn QDI_Adsppm_do_callback - Add callback data to the callback queue and signal user to pick it up
 */
void QDI_Adsppm_do_callback(QDI_Adsppm_cbinfo_t *ptr, MmpmCallbackParamType* pValue);
