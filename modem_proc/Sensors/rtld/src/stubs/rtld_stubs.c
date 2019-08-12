/*==============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "dlfcn.h"
/*
void* dlopen(const char* name, int mode)
{
  return 0;
}

int dlclose(void* handle)
{
  return -1;
}

void* dlsym(void* __restrict handle, const char* __restrict name)
{
  return 0;
}

int dladdr(const void* __restrict addr, Dl_info* __restrict info)
{
  return 0;
}

char* dlerror(void)
{
  return "stubbed out loader";
}
*/
int dlinfo(void* __restrict handle, int request, void* __restrict p)
{
  return -1;
}
