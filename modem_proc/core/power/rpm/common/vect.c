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

#include <stdlib.h>
#include <string.h>

#include "vect.h"
#include "CoreVerify.h"

struct vect_s
{
    unsigned size;
    // TODO: we'll want a notion of 'reserved' size to minimize realloc calls,
    // but I just need this to work for now; optimize later.
    
    void   **elements;
};

vect_t *vect_create()
{
    vect_t *self = malloc(sizeof(vect_t));
    CORE_VERIFY_PTR(self);
    memset(self, 0, sizeof(vect_t));
    return self;
}

void vect_destroy(vect_t *self)
{
    free(self->elements);
    free(self);
}

unsigned vect_size(vect_t *self)
{
    return self->size;
}

void vect_resize(vect_t *self, unsigned new_size)
{
    self->elements = realloc(self->elements, new_size * sizeof(void *));

    // Initialize any new pointers to NULL.
    if(new_size > self->size)
        memset(self->elements + self->size, 0, (new_size - self->size) * sizeof(void *));

    self->size = new_size;
}

void vect_push(vect_t *self, void *element)
{
    vect_resize(self, self->size + 1);
    vect_set(self, self->size - 1, element);
}

void *vect_at(vect_t *self, unsigned idx)
{
    if(idx >= self->size)
        return 0;

    return self->elements[idx];
}

void vect_set(vect_t *self, unsigned idx, void *element)
{
    if(idx >= self->size)
        return;

    self->elements[idx] = element;
}

