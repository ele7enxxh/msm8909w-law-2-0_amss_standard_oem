#include <comdef.h>
#include <string.h>
#include <stdarg.h>

#include "qsee_cipher.h"
#include "qsee_crypto.h"
#include "qsee_hash.h"
#include "qsee_hmac.h"
#include "qsee_prng.h"
#include "qsee_core.h"
#include "qsee_fuse.h"
#include "qsee_securechannel.h"

void app_export_init_info_to_qsee(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void qsee_wait_default_signal_syscall(void);
void qsee_printf_syscall(const char* fmt, va_list ap);
unsigned int qsee_generic_syscall(unsigned int id, unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5);
