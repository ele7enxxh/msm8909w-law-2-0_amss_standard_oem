#ifndef __REX_PRIVATE_H__
#define __REX_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif
/**
Checks whether REX has been initialized.

@return
TRUE     --    Indicates that REX has been initialized.
FALSE    --    Indicates that REX has not been initialized.

@dependencies
None.
*/
boolean rex_check_if_initialized(void);

/**
Routine to create the DPC task.

@return
REX_SUCCESS --    Indicates that the REX DPC task was created sucessfully.
REX_ERROR   --    Otherwise.

@dependencies
None.
*/
int rex_create_dpc_task(void);

/**
Routine to initialize REX task profiling

@return
REX_SUCCESS --    Indicates that the REX DPC task was created sucessfully.
REX_ERROR   --    Otherwise.

@dependencies
None.
*/
void rex_init_profiling(void);

#ifdef __cplusplus
}
#endif

#endif //__REX_PRIVATE_H__
