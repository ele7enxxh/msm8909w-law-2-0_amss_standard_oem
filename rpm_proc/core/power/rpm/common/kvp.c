/*===========================================================================
  kvp.c

  SERVICES:

  DESCRIPTION:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

  Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
  ===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#ifdef _MSC_VER
// enable memory leak detection under msvc
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "kvp.h"
#include "bytearray.h"
#include "CoreVerify.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define ALIGN_32(n) (((n)+3)&~3)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
struct kvp_s
{
  bytearray_t *buffer;
  unsigned     read_idx;
};

typedef struct
{
  unsigned   type;
  unsigned   length;
  const char value[];
} tlv_header;


//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
/*===========================================================================
  FUNCTION: kvp_create

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
kvp_t *kvp_create(unsigned buffer_size)
{
  kvp_t *self = (kvp_t *)malloc(sizeof(kvp_t));
  CORE_VERIFY_PTR(self);
  memset(self, 0, sizeof(kvp_t));
  self->buffer = bytearray_create(buffer_size);
  return self;
}

/*===========================================================================
  FUNCTION: kvp_frombuffer

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
kvp_t *kvp_frombuffer(unsigned size, const char *buffer)
{
  kvp_t *self = (kvp_t *)malloc(sizeof(kvp_t));
  CORE_VERIFY_PTR(self);
  memset(self, 0, sizeof(kvp_t));
  self->buffer = bytearray_frombuffer(buffer, size);
  return self;
}

/*===========================================================================
  FUNCTION: kvp_swapbuffer

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void kvp_swapbuffer(kvp_t *self, const char *bytes, unsigned buffer_size)
{
  bytearray_swapbuffer(self->buffer, bytes, buffer_size);
  self->read_idx = 0;
}

/*===========================================================================
  FUNCTION: kvp_destroy

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void kvp_destroy(kvp_t *self)
{
  bytearray_destroy(self->buffer);
  free(self);
}

/*===========================================================================
  FUNCTION: kvp_eof

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
unsigned kvp_eof(const kvp_t *self)
{
  return (self->read_idx >= bytearray_size(self->buffer));
}

/*===========================================================================
  FUNCTION: kvp_put

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void kvp_put(kvp_t *self, unsigned type, unsigned length, const void *value)
{
  const char padding[] = "\x00\x00\x00";
  unsigned required_pad = (ALIGN_32(length & 0x3)) - (length & 0x3);

  bytearray_put(self->buffer, &type,   sizeof(type));
  bytearray_put(self->buffer, &length, sizeof(length));
  bytearray_put(self->buffer,  value,  length);

  if(required_pad)
    bytearray_put(self->buffer, padding, required_pad);
}

/*===========================================================================
  FUNCTION: kvp_bytes_available

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
static unsigned kvp_bytes_available(kvp_t *self)
{
  unsigned bufsize = bytearray_size(self->buffer);
  if(self->read_idx >= bufsize)
    return 0;
  else
    return bufsize - self->read_idx;
}

/*===========================================================================
  FUNCTION: kvp_get

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
unsigned kvp_get(kvp_t *self, unsigned *type, unsigned *length, const char **value)
{
  tlv_header *head;
	
  // If the buffer doesn't hold another header, we fail.
  if(kvp_bytes_available(self) < sizeof(tlv_header))
    return 0;

  head = (tlv_header *)bytearray_at(self->buffer, self->read_idx);
  self->read_idx  += sizeof(tlv_header);

  CORE_VERIFY_PTR(head);
  *type   = head->type;
  *length = head->length;

  // We can have keys with 'null' values.
  if(!head->length)
  {
    *value = NULL;
    return 1;
  }

  // Not null, but ensure that the buffer has the data it's promising.
  if(kvp_bytes_available(self) < ALIGN_32(head->length))
    return 0;

  *value  = head->value;
  self->read_idx += ALIGN_32(head->length);
  return 1;
}

/*===========================================================================
  FUNCTION: kvp_reset

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void kvp_reset(kvp_t *self)
{
  self->read_idx = 0;
}

/*===========================================================================
  FUNCTION: kvp_clear

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void kvp_clear(kvp_t *self)
{
  bytearray_clear(self->buffer);
  kvp_reset(self);
}

/*===========================================================================
  FUNCTION: kvp_get_raw_size

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
unsigned kvp_get_raw_size(const kvp_t *self)
{
  return bytearray_size(self->buffer);
}

/*===========================================================================
  FUNCTION: kvp_get_raw_bytes

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
const char *kvp_get_raw_bytes(const kvp_t *self)
{
  return bytearray_at(self->buffer, 0);
}

