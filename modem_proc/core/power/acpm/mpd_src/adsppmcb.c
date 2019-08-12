/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include <stdlib.h>
#include "qurt_rmutex.h"
#include "adsppmcb.h"
#include "adsppm.h"
#include "mmpm.h"
#include "adsppm_qdi.h"



static qurt_mutex_t Adsppmcb_mutex_Srv;
static Adsppmcb_Elem_Type_Srv *pfnlist = NULL;

void Adsppmcb_Init_Server(void)
{
    qurt_rmutex_init(&Adsppmcb_mutex_Srv);
    pfnlist = NULL;
}

void Adsppmcb_register_callback_Srv( QDI_Adsppm_cbinfo_t *arg)
{
    Adsppmcb_Elem_Type_Srv *p;

    p = malloc(sizeof(*p));
    if (p) {
        memcpy((void*)&p->arg, arg, sizeof(QDI_Adsppm_cbinfo_t));
        qurt_rmutex_lock(&Adsppmcb_mutex_Srv);
        p->next = pfnlist;
        pfnlist = p;
        qurt_rmutex_unlock(&Adsppmcb_mutex_Srv);
    }
}

void Adsppmcb_deRegister_callback_Srv(int clientId)
{
    Adsppmcb_Elem_Type_Srv *p;
    Adsppmcb_Elem_Type_Srv *preP = NULL;
    qurt_rmutex_lock(&Adsppmcb_mutex_Srv);
    for (p=pfnlist; p; p=p->next)
    {
        if( clientId == (p->arg.clientId))
        {
            if(preP)
            {
                preP->next = p->next;
            }
            else
            {
                //Deleting last element
                pfnlist = NULL;
            }
            free(p);
            break;
        }
        preP = p;
    }
    qurt_rmutex_unlock(&Adsppmcb_mutex_Srv);
}

uint32 Adsppmcb_notify_callbacks_Srv(MmpmCallbackParamType* pCallbackValue)
{
    Adsppmcb_Elem_Type_Srv *p;

    for (p=pfnlist; p; p=p->next)
    {
        if(pCallbackValue->clientId == p->arg.clientId)
        {
            QDI_Adsppm_do_callback(&p->arg, pCallbackValue);
            break;
        }
    }
    return 0;
}

void Adsppmcb_Deinit_Srv(void)
{
    Adsppmcb_Elem_Type_Srv *p;
    //go through pfnlist, release mem
	if(pfnlist)
	{
		while(pfnlist->next)
		{
			for (p=pfnlist; p; p=p->next)
			{
				if(NULL != p->next)
				{
					if(NULL == p->next->next)
					{
						free(p->next);
						p->next = NULL;
					}
				}
			}
		}
	free(pfnlist);
	pfnlist = NULL;	
    }	
    qurt_rmutex_destroy(&Adsppmcb_mutex_Srv);
}
