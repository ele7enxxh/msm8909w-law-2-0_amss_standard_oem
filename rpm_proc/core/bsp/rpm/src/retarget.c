
// The RPM shall not make use of RVCT's semihosting.
#pragma import(__use_no_semihosting) 
#pragma import(__use_no_semihosting_swi) 

#include <stdlib.h>
#include <string.h>
#include <rt_sys.h>
#include <rt_heap.h>
#include "comdef.h"
#include "coredump.h"
#include <stdbool.h>
#include "image_layout.h"
#include "swevent.h"

const char __stdin_name[] ="STDIN";
const char __stdout_name[]="STDOUT";
const char __stderr_name[]="STDERR";

static int heap_free_count;

char** stack_base_address = (char**)&rpm_image_section_array[RPM_STACK].section_end;

__asm void __user_setup_stackheap()
{
	IMPORT stack_base_address

	ldr r0, =stack_base_address
	ldr r0, [r0]
	ldr sp, [r0]
    mov r0, #0
    mov r2, #0
    bx lr
    ALIGN
}

FILEHANDLE _sys_open(const char *name, int openmode)
{
  return -1;
}

FILEHANDLE _sys_close(FILEHANDLE fh)
{
  return -1;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
  return -1;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
  return -1;
}

int _sys_ensure(FILEHANDLE fh)
{
  return -1;
}

long _sys_flen(FILEHANDLE fh)
{
  return -1;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
  return -1;
}

int _sys_istty(FILEHANDLE fh)
{
  return -1;
}

int _sys_tmpnam(char *name, int fileno, unsigned maxlength)
{
  return 0;
}

char *_sys_command_string(char *cmd, int len)
{
  return 0;
}

void _ttywrch(int ch)
{
}

void _sys_exit(int return_code)
{
  abort();
}

typedef struct __Heap_Descriptor
{
    char *heapPtr;
    char *heapLimit;
} __Heap_Descriptor;

static __Heap_Descriptor theHeap;
static bool heap_free_disabled;

void prevent_further_heap_frees(void)
{
    heap_free_disabled = true;
}

extern int __Heap_DescSize(int zero)
{
    return sizeof(__Heap_Descriptor);
}

extern void __Heap_Initialize(struct __Heap_Descriptor *h)
{
    rpm_free_heap_space_init();
    unsigned unaligned = (unsigned)rpm_image_section_array[RPM_HEAP].section_base;
    theHeap.heapPtr = (char *)((unaligned + 7) & ~7);
    theHeap.heapLimit = (char *)rpm_image_section_array[RPM_HEAP].section_end;
	rpm_adjust_free_heap_space(theHeap.heapLimit - theHeap.heapPtr);
}

extern void __Heap_Finalize(struct __Heap_Descriptor *h) {}
extern void __Heap_ProvideMemory(struct __Heap_Descriptor *h, void *mem, size_t size) {}

extern void *__Heap_Alloc(struct __Heap_Descriptor *h, size_t size)
{
    INTLOCK();

    size = (size + 7) & ~7; // ARM docs say to 8-byte align allocated blocks.

    char *mem = theHeap.heapPtr;

    if((theHeap.heapPtr + size) < theHeap.heapLimit)
    {
        theHeap.heapPtr += size;
		rpm_adjust_free_heap_space(-size);
        INTFREE();
        return mem;
    }
    SWEVENT(RPM_OUT_OF_HEAP);
    abort();
}

extern void __Heap_Free(struct __Heap_Descriptor *h, void *ptr)
{
    // On the RPM we don't allow people to repeatedly alloc and free. That pattern leads to
    // heap fragmentation which will eventually cause the RPM to fail with OOM.
    // Better to just prevent the pattern in the first place.

    if(heap_free_disabled)
    {
        abort();
    }
    heap_free_count++;
}

extern void *__Heap_Realloc(struct __Heap_Descriptor *h, void *old_mem, size_t size)
{
    void *new_mem = __Heap_Alloc(h, size);
    memcpy(new_mem, old_mem, size);
    return new_mem;
}

extern void *__Heap_Stats(struct __Heap_Descriptor *h,
                          int (*print)(void *, char const *format, ...),
                          void *printparam)
{
    return NULL;
}

extern int __Heap_Valid(struct __Heap_Descriptor *h,
                        int (*print)(void *, char const *format, ...),
                        void *printparam, int verbose)
{
    return 1;
}

// The chip's PBL currently smashes certain regions after the RPM is loaded.
// We need to avoid putting anything useful in those areas.
//const unsigned __attribute__((at(0x107448))) smashed_by_pbl_on_msm8974v1;
//const unsigned __attribute__((at(0x108444))) smashed_by_pbl_on_msm8974v2_mdm9x25_and_later_chips;

