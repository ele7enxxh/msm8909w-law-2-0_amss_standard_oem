#include "platform_libs_list.h"
#include "platform_libs.h"

PL_DEP(fastrpc_smd)
PL_DEP(dlw)
PL_DEP(svfs)

//TODO: move these into proper places
int QDI_SVFS_Sys_Init() {
   return 0;
}

int StaticHash_Verify(const char* key, void* buf, int nlen) {
   return 0;
}

struct platform_lib* (*pl_list[])(void) = {
   PL_ENTRY(dlw),
   PL_ENTRY(svfs),
   PL_ENTRY(fastrpc_smd),
   0
};


