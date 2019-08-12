
#include <string.h>
#include <windows.h>
#include "rex.h"
#include "rte_ipc.h"
#include "task.h"
#include "ipc_dispatch.h"
#include "ipcsvc.h"
#define BREW_PROXY_IPC_SIG      0x100

#define BREW_PROXY_RECEIVE_SIG 0x400

static HANDLE brew_proxy_thread_event;
static HANDLE brew_proxy_task_event;

static void *returned_msg_ptr;

static unsigned int requested_msg_id = IPC_MSG_ANY;
static int brew_proxy_initialized;

int ipc_is_brew_thread (void)
{
	unsigned long curr_thread_id = GetCurrentThreadId();

	if ( curr_thread_id == rex_self()->thread_id )
		return 0;
	else
		return 1;
}

rex_tcb_type *ipc_brew_get_tcb (void)
{
	return &brew_proxy_tcb;
}

union ipc_msg_type* ipc_receive_for_brew (unsigned int msg_id)
{

	requested_msg_id = msg_id;

	SetEvent (brew_proxy_task_event);

	WaitForSingleObject (brew_proxy_thread_event, INFINITE);

	return returned_msg_ptr;
}

void ipc_brew_proxy_task (unsigned long param)
{
	rex_sigs_type wait_sigs = BREW_PROXY_IPC_SIG;
	rex_sigs_type set_sigs;
	unsigned int returned_msg_id = IPC_MSG_ANY;
	(void) param;
	
	ixipc_init (IPCSVC_IXCONTENTHANDLER, BREW_PROXY_IPC_SIG);

	while ( 1 )
	{
		set_sigs = rex_wait (wait_sigs);

		if ( (set_sigs & BREW_PROXY_IPC_SIG) )
		{
			//returned_msg_ptr = ipc_receive (requested_msg_id);
			//returned_msg_id  = *((ipc_msg_id_type*)(returned_msg_ptr));

			if( requested_msg_id != IPC_MSG_ANY )
			{
				returned_msg_ptr = ipc_receive (requested_msg_id);
				requested_msg_id = IPC_MSG_ANY;

				WaitForSingleObject (brew_proxy_task_event, INFINITE);

				INTLOCK ();
				SetEvent (brew_proxy_thread_event);
				INTFREE ();         
			} else
			{
				ipc_dispatch_msg();
			}
		}
	}
}

int ipc_brew_proxy_init (void)
{
	if ( brew_proxy_initialized == FALSE )
	{
		brew_proxy_thread_event = CreateEvent(NULL, FALSE, FALSE, NULL );
		brew_proxy_task_event   = CreateEvent(NULL, FALSE, FALSE, NULL );

		if ( (brew_proxy_thread_event == NULL) || (brew_proxy_task_event == NULL) )
			return -1;

		strcpy (brew_proxy_tcb.task_name, "BREW Proxy");
		rex_def_task_ext (  &brew_proxy_tcb,  
							brew_proxy_stack,  
							8192,
							(rex_priority_type) BREWPROXY_PRI, 
							ipc_brew_proxy_task, 0, "BREW Proxy", 0);
		brew_proxy_initialized = TRUE;
		return 0;
	} else
	{
		/* BREW Proxy task already started */
		return 0;
	}
}

