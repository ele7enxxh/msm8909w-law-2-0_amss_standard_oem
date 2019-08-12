#ifndef __QSEE_EXTERNAL_H
#define __QSEE_EXTERNAL_H

#include "lktypes.h"
#include "tzbsp_log.h"

void qsee_process_external_commands(uint32_t command_id, void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

void qsee_load_external_image(void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

void qsee_unload_external_image(void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

void external_image_entry(void* vp0, void* vp1, void* vp2);

typedef void (*external_image_entry_t) (void* vp0, void* vp1, void* vp2);

#endif
