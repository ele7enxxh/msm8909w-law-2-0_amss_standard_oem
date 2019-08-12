/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "bytearray.h"
#include "CoreVerify.h"

struct bytearray_s
{
    bool     static_buffer;
    unsigned size;
    unsigned reserved;
    void    *bytes;
};

unsigned bigger(const unsigned a, const unsigned b)
{
    if(a > b)
        return a;
    else
        return b;
}

bytearray_t *bytearray_create(unsigned buffer_size)
{
    bytearray_t *self = (bytearray_t *)malloc(sizeof(bytearray_t));
    CORE_VERIFY_PTR(self);
    memset(self, 0, sizeof(*self));

    if(buffer_size)
    {
        self->bytes = malloc(buffer_size);
        CORE_VERIFY_PTR(self->bytes);
		
        self->reserved = buffer_size;
    }

    return self;
}

bytearray_t *bytearray_frombuffer(const void *bytes, unsigned buffer_size)
{
    bytearray_t *self = (bytearray_t *)malloc(sizeof(bytearray_t));
    CORE_VERIFY_PTR(self);
	
    memset(self, 0, sizeof(*self));
    self->static_buffer = true;

    //bytearray_swapbuffer(self, bytes, buffer_size);
    self->bytes    = (void *)bytes;
    self->size     = buffer_size;
    self->reserved = buffer_size;
    return self;
}

void bytearray_swapbuffer(bytearray_t *self, const void *bytes, unsigned buffer_size)
{
    if(!self->static_buffer && self->bytes)
    {
        free(self->bytes);
        self->static_buffer = true;
    }

    self->bytes         = (void *)bytes;
    self->size          = buffer_size;
    self->reserved      = buffer_size;
    self->static_buffer = true;
}

void bytearray_destroy(bytearray_t *self)
{
    if(!self->static_buffer && self->bytes)
        free(self->bytes);
    free(self);
}

unsigned bytearray_size(bytearray_t *self)
{
    return self->size;
}

unsigned bytearray_reserved(bytearray_t *self)
{
    return self->reserved;
}

void bytearray_reserve(bytearray_t *self, unsigned minimum)
{
    if(self->reserved >= minimum)
        return;

    self->reserved = minimum;
    self->bytes = realloc(self->bytes, minimum);
    CORE_VERIFY(self->bytes);
}

static void bytearray_extend(bytearray_t *self, unsigned num_bytes)
{
    unsigned exp_size, new_size, required_size;
    if(self->size & 0x80000000)
        exp_size = 0xFFFFFFFFu;
    else
        exp_size = self->size * 2;
		
    required_size = self->size + num_bytes;
    CORE_VERIFY(required_size >= self->size);
    new_size      = bigger(exp_size, required_size);

    self->bytes = realloc(self->bytes, new_size);
    CORE_VERIFY(self->bytes);
	
    self->reserved = new_size;
}

void bytearray_put(bytearray_t *self, const void *bytes, unsigned num_bytes)
{
    unsigned new_size;
    if(self->static_buffer)
        return;

    new_size = self->size + num_bytes;
    CORE_VERIFY(new_size >= self->size);
	
    if(new_size > self->reserved)
        bytearray_extend(self, num_bytes);

    memcpy(((char *)self->bytes) + self->size, bytes, num_bytes);
    self->size += num_bytes;
}

void bytearray_clear(bytearray_t *self)
{
    self->size = 0;
}

void *bytearray_at(bytearray_t *self, unsigned index)
{
    if(index < self->size)
        return ((char *)self->bytes) + index;

    return 0;
}

void *bytearray_slice(bytearray_t *self, unsigned start, unsigned end)
{
    if(start < self->size && start <= end && end <= self->size)
        return ((char *)self->bytes) + start;

    return 0;
}

