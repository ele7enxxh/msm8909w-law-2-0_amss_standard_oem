#include "dlpager_q6zip.h"
#include "dlpager_constants.h"
#include "dlpager_main.h"
#include "dlpager_log.h"
#include "dlpager_cache.h"
#include "q6zip_uncompress.h"
#include "rw_compress.h"
#include "q6zip_clk.h"
#include "q6zip_partial.h"
#include "memload_handler.h"

/* if USE_DLPAGER_HEAP is defined, this module uses its own heap.
   else use AMSS heap defined in modem_proc\config\8974\cust_config.xml.
   If using AMSS heap, the AMSS heap size needs to be increased.
*/
#define USE_DLPAGER_HEAP

#include <stdlib.h>
#include <stringl.h>
#include <hexagon_protos.h>
#include "assert.h"
#include "qurt_cycles.h"
#include "qurt_memory.h"
#include <qurt_cycles.h>
#ifdef USE_DLPAGER_HEAP
#include "memheap.h"
#endif
#include <assert.h>

#define MAX_LONG  (0xFFFFFFFF)
#define RW_CIRC_BUF_SIZE 256

/* Type declarations */

/* element of rw table */
typedef struct
{
    void *ptr;       /* pointer to memory containing a compressed block*/
    uint32_t size;   /* size of allocated block */
} dlpager_rw_table_t;

/* structure used for rw v->p translation and decompression */
typedef struct
{
    unsigned int page_size;       /* size of rw page */
    unsigned int num_elements;    /* number of elements in table */
    dlpager_rw_table_t *table;    /* array of pointers to compressed blocks */
} dlpager_rw_info_t;

typedef struct
{
  unsigned short int total_primary_count;
  unsigned short int total_secondary_count;
  unsigned char last_secondary_count;
}rx_stats_t;

typedef struct
{
  unsigned short int decompress_count;
  unsigned short int recompress_count;
}rw_stats_t;


static struct {
    unsigned total_text_secondary_chunk[N_CHUNKS];
    int total_text_secondary;
    int total_text_faults;
    int total_rodata_faults;
    int total_rwdata_faults;
    int total_bss_faults;
} dlpager_stats;


/* Symbol definitions */

/* Extern symbols defied in dlpager_main.c */
extern unsigned int __attribute__ ((weak)) __swapped_segments_start__;

extern unsigned int __attribute__ ((weak)) __swapped_range_rx_start__;
extern unsigned int __attribute__ ((weak)) __swapped_range_text_start__;
extern unsigned int __attribute__ ((weak)) __swapped_range_text_partial_start__;
extern unsigned int __attribute__ ((weak)) __swapped_range_text_partial_end__;
extern unsigned int __attribute__ ((weak)) __swapped_range_text_end__;
extern unsigned int __attribute__ ((weak)) __swapped_range_rodata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_range_rodata_end__;
extern unsigned int __attribute__ ((weak)) __swapped_range_rx_end__;

extern unsigned int __attribute__ ((weak)) __swapped_range_rw_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_rw_start__;
extern unsigned int __attribute__ ((weak)) __swapped_range_rwdata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_range_rwdata_end__;
extern unsigned int __attribute__ ((weak)) __swapped_range_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_range_bss_end__;
extern unsigned int __attribute__ ((weak)) __swapped_range_rw_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_end__;

extern unsigned int __attribute__ ((weak)) __dlpager_heap_start__;
extern unsigned int __attribute__ ((weak)) __dlpager_heap_end__;

extern uint32_t __attribute__((section (".data"))) start_va_uncompressed_text;
extern uint32_t __attribute__((section (".data"))) end_va_uncompressed_text ;
extern uint32_t __attribute__((section (".data"))) start_va_compressed_text;
extern uint32_t __attribute__((section (".data"))) end_va_compressed_text;

extern uint32_t __attribute__((section (".data"))) start_va_uncompressed_rw;
extern uint32_t __attribute__((section (".data"))) end_va_uncompressed_rw ;
extern uint32_t __attribute__((section (".data"))) start_va_compressed_rw;
extern uint32_t __attribute__((section (".data"))) end_va_compressed_rw;

int dlpager_osam_debug_enabled = 0;

/* rx metadata used for v->p translation and decompression */
static dlpager_rx_metadata_t* dlpager_rx_metadata;
static dlpager_rw_metadata_t* dlpager_rw_metadata;

/* rw info used for v->p translation and decompression */
static dlpager_rw_info_t dlpager_rw_info;

static volatile uint32_t q6zip_swap_pool_size = 2 * 1024 * 1024;

#ifdef USE_DLPAGER_HEAP
/* dlpager heap structure */
static mem_heap_type dlpager_heap;
/* dlpager heap buffer */
static byte *dlpager_heap_buffer;
#endif

static unsigned int dlpager_rw_temp_buf[(PAGE_SIZE>>2) + (PAGE_SIZE>>4)] __attribute__((aligned(32)));

/* Stats */

static rx_stats_t* rx_stats;
static rw_stats_t* rw_stats;

static unsigned long long int rx_total_decompressions = 0;
static unsigned long long int rw_total_decompressions = 0;
static unsigned long long int rw_total_recompressions = 0;
static long long int rw_initial_compressed_size = 0;

//tracks malloced size with optimization
static unsigned int rw_circ_buf_idx = 0;
static long long int rw_malloced_size[RW_CIRC_BUF_SIZE];

//tracks malloced size with/without optimization
static unsigned int rw_circ_buf_idx_always = 0;
static long long int rw_malloced_size_always[RW_CIRC_BUF_SIZE];


#define DLPAGER_NUM_MAX_OUTSTANDING_WRITES_CODE 32
#define DLPAGER_NUM_MAX_OUTSTANDING_WRITES_RO 32
#define DLPAGER_NUM_MAX_OUTSTANDING_WRITES_RW 32

unsigned int dlpager_num_outstanding_writes_code = 0;
unsigned int dlpager_num_outstanding_writes_ro = 0;
unsigned int dlpager_num_outstanding_writes_rw = 0;
unsigned int dlpager_debug_is_busy = 0; //T32 DLL should return error of user tries to set BP or change data values for address in the
unsigned int dlpager_debug_address = 0; //uncompressed virtual address of page if the flag above is 1
typedef struct
{
  unsigned int address; //uncompressed virtual address of compressed data/text, 0 means the entry is empty
  unsigned int value; //to be written supplied by user of T32
  unsigned int mask; //size to write at address, word=0xFFFFFFFF, short = 0xFFFF, byte = 0xFF - should be always 0xFFFFFFFF here
  unsigned int state; //1: brkpt is set and active, 2: brkpt is set but not active
  unsigned int orig_value; //original value at address
} dlpager_debug_rx_elem_s;
typedef struct
{
  unsigned int address; //uncompressed virtual address of compressed data/text, 0 means the entry is empty
  unsigned int value; //to be written supplied by user of T32
  unsigned int mask; //size to write at address, word=0xFFFFFFFF, short = 0xFFFF, byte = 0xFF
} dlpager_debug_data_elem_s;
dlpager_debug_rx_elem_s dlpager_outstanding_writes_code[DLPAGER_NUM_MAX_OUTSTANDING_WRITES_CODE] = {{0}};
dlpager_debug_data_elem_s dlpager_outstanding_writes_ro[DLPAGER_NUM_MAX_OUTSTANDING_WRITES_RO] = {{0}};
dlpager_debug_data_elem_s dlpager_outstanding_writes_rw[DLPAGER_NUM_MAX_OUTSTANDING_WRITES_RW] = {{0}};
/* Function definitions*/

/* returns 1 if addr belongs to text or ro data */
unsigned int dlpager_is_addr_in_rx_range(unsigned int addr)
{
    /* new linker symbols exist */
    if ( (unsigned)&__swapped_range_rx_start__ != 0 && (unsigned)&__swapped_range_rx_end__ != 0 )
    {
        return ((unsigned)&__swapped_range_rx_start__ <= addr && addr < (unsigned)&__swapped_range_rx_end__);
    }
    else /* or backward compatible symbol check */
    {
        return (start_va_uncompressed_text <= addr && addr < end_va_uncompressed_text );
    }
}

unsigned int dlpager_is_addr_in_rodata_range(unsigned int addr)
{
    return ( (unsigned)&__swapped_range_rodata_start__ <= addr && addr < (unsigned)&__swapped_range_rodata_end__ );
}

/*
   Text means code only. Interested in text vs. RO data for I$ optimization.
   RO data does not require I$ invalidation because it goes through data cache.
   Live target T32 debugging also takes advantage of this function, as well as 
   dlpager_log.
*/
unsigned int dlpager_is_addr_in_text_range(unsigned int addr)
{
    /* new linker symbols exist */
    if ( (unsigned)&__swapped_range_text_start__ != 0 && (unsigned)&__swapped_range_text_end__ != 0 )
    {
        return ( (unsigned)&__swapped_range_text_start__ <= addr && addr < (unsigned)&__swapped_range_text_end__ );
    }
    else /* or backward compatible symbol check */
    {
        if ( (unsigned int)&__swapped_range_rodata_start__ > 0 )
        {   /* Means we are in text only, and not RO Data range. */
            return ( start_va_uncompressed_text <= addr && addr < (unsigned int)&__swapped_range_rodata_start__ );
        }
        else
        {   /* This range includes both code and RO data. */
            return ( start_va_uncompressed_text <= addr && addr < end_va_uncompressed_text );
        }
    }
}

unsigned int dlpager_is_addr_in_text_partial_range(unsigned addr)
{
    return ( (unsigned)&__swapped_range_text_partial_start__ <= addr && addr < (unsigned)&__swapped_range_text_partial_end__ );
}

/* returns 1 if addr belongs to rw data (bss or rwdata) */
unsigned int dlpager_is_addr_in_rw_range(unsigned int addr)
{
    /* new linker symbols exist */
    if ( (unsigned)&__swapped_range_rw_start__ != 0 && (unsigned)&__swapped_range_rw_end__ != 0 )
    {
        return ( (unsigned)&__swapped_range_rw_start__ <= addr && addr < (unsigned)&__swapped_range_rw_end__ );
    }
    else /* or backward compatible symbols check */
    {
        return ( start_va_uncompressed_rw <= addr && addr < end_va_uncompressed_rw );
    }
}

/* returns 1 if addr belongs to rwdata */
unsigned int dlpager_is_addr_in_rwdata_range(unsigned int addr)
{
    /* new linker symbols exist */
    if ( (unsigned)&__swapped_range_rwdata_start__ != 0 && (unsigned)&__swapped_range_rwdata_end__ != 0 )
    {
        return ( (unsigned)&__swapped_range_rwdata_start__ <= addr && addr < (unsigned)&__swapped_range_rwdata_end__ );
    }
    else /* or backward compatible symbol check */
    {
        return ( start_va_uncompressed_rw <= addr && addr < (unsigned int)&__swapped_segments_bss_start__ );
    }
}

/* returns 1 if addr belongs to bss */
unsigned int dlpager_is_addr_in_bss_range(unsigned int addr)
{
    /* new linker symbols exist */
    if ( (unsigned)&__swapped_range_bss_start__ != 0 && (unsigned)&__swapped_range_bss_end__ != 0 )
    {
        return ( (unsigned)&__swapped_range_bss_start__ <= addr && addr < (unsigned)&__swapped_range_bss_end__ );
    }
    else /* or backward compatible symbol check */
    {
        return ( (unsigned int)&__swapped_segments_bss_start__ <= addr && addr < end_va_uncompressed_rw );
    }
}


static inline void dlpager_get_compressed_block_info_rx
(
    uint32_t uncompressed_page_addr,
    uint32_t *compressed_block_addr,
    unsigned long *compressed_block_size
    )
{
    uint32_t block_num;

    block_num = (uncompressed_page_addr - start_va_uncompressed_text) / PAGE_SIZE ;

    if (block_num < dlpager_rx_metadata->n_blocks)
    {
        *compressed_block_addr = dlpager_rx_metadata->compressed_va[block_num];
        *compressed_block_size = (block_num + 1 == dlpager_rx_metadata->n_blocks)?
            (end_va_compressed_text
             - dlpager_rx_metadata->compressed_va[block_num]):
            (dlpager_rx_metadata->compressed_va[block_num + 1]
             - dlpager_rx_metadata->compressed_va[block_num]);
    }
}

static inline void dlpager_get_compressed_block_info_rw
(
    uint32_t uncompressed_page_addr,
    uint32_t *compressed_block_addr_ptr,
    unsigned long *compressed_block_size_ptr
    )
{
    uint32_t block_num = (uncompressed_page_addr - start_va_uncompressed_rw) / PAGE_SIZE;

    *compressed_block_addr_ptr = (uint32_t) dlpager_rw_info.table[block_num].ptr;
    *compressed_block_size_ptr = dlpager_rw_info.table[block_num].size;
}

void dlpager_l2fetch_buffer(void *addr, uint32_t len)
{
    /* Cache-align starting address and length. */
    uint32_t ofs = ((uint32_t) addr) & (CACHE_LINE_SZ-1);
    addr = (void *) ((uint32_t) addr - ofs);
    len  = (len+ofs+CACHE_LINE_SZ-1) / CACHE_LINE_SZ;

    /* Width=cache line, height=# cache lines, stride=cache line */
    asm volatile ("l2fetch(%[addr],%[dim])" : :
                  [addr] "r" (addr),
                  [dim] "r" ( Q6_P_combine_IR(CACHE_LINE_SZ, Q6_R_combine_RlRl(CACHE_LINE_SZ, len)) )
                  : "memory");
}

void dlpager_store_virtual_page(unsigned int page_to_store, unsigned int fault_addr_evicted_page)
{
    //Uncompressed_page_addr = location in swap pool to write to
    //Mapped_page_addr = linked location in uncompressed virtual space (10 million range)
    uint32_t compressed_size;
    uint32_t block_num;
    unsigned int rw_circ_buf_next_idx;

    block_num = (fault_addr_evicted_page - start_va_uncompressed_rw) / PAGE_SIZE;	

    DL_DEBUG("Recompressing RW page for at mapped addr = 0x%x, uncomp addr = 0x%x\n", fault_addr_evicted_page, page_to_store);

    dlpager_l2fetch_buffer((void *)page_to_store, PAGE_SIZE);
    dlpager_dczero((uint32_t)dlpager_rw_temp_buf, PAGE_SIZE);
    compressed_size = deltaCompress((unsigned int *)page_to_store,(unsigned int*) dlpager_rw_temp_buf,PAGE_SIZE>>2);
    assert(compressed_size != 0);

    //free and malloc only for first time bss or when new compressed size is larger than original malloced size
    if ((dlpager_rw_info.table[block_num].ptr == NULL) || (dlpager_rw_info.table[block_num].size < compressed_size))
    {
        if (dlpager_rw_info.table[block_num].ptr != NULL)
        {
#ifdef USE_DLPAGER_HEAP
            mem_free(&dlpager_heap,dlpager_rw_info.table[block_num].ptr);
#else
            free(dlpager_rw_info.table[block_num].ptr);
#endif
        }

#ifdef USE_DLPAGER_HEAP
        dlpager_rw_info.table[block_num].ptr = mem_malloc(&dlpager_heap,compressed_size+4);
#else
        dlpager_rw_info.table[block_num].ptr = malloc(compressed_size+4);
#endif

        //make delta memory change on previous entry
        rw_circ_buf_next_idx = (rw_circ_buf_idx + 1) % RW_CIRC_BUF_SIZE;
        rw_malloced_size[rw_circ_buf_next_idx] = rw_malloced_size[rw_circ_buf_idx]
            + (compressed_size - dlpager_rw_info.table[block_num].size);
        rw_circ_buf_idx = rw_circ_buf_next_idx;

        //do this for this stats buffer also to track the above
        rw_circ_buf_next_idx = (rw_circ_buf_idx_always + 1) % RW_CIRC_BUF_SIZE;
        rw_malloced_size_always[rw_circ_buf_next_idx] = rw_malloced_size_always[rw_circ_buf_idx_always]
            + (compressed_size - dlpager_rw_info.table[block_num].size);
        rw_circ_buf_idx_always = rw_circ_buf_next_idx;

        dlpager_rw_info.table[block_num].size = compressed_size;
    }
    else
    {
        rw_circ_buf_next_idx = (rw_circ_buf_idx_always + 1) % RW_CIRC_BUF_SIZE;
        rw_malloced_size_always[rw_circ_buf_next_idx] = rw_malloced_size_always[rw_circ_buf_idx_always]
            - (dlpager_rw_info.table[block_num].size - compressed_size);
        rw_circ_buf_idx_always = rw_circ_buf_next_idx;
    }

    memscpy(dlpager_rw_info.table[block_num].ptr,compressed_size,(void*)dlpager_rw_temp_buf,compressed_size);

    rw_stats_t * rw_stat = &rw_stats[block_num];
    rw_stat->recompress_count++;
    rw_total_recompressions++;
}

static void dlpager_load_page_rx(unsigned src_addr_fault, unsigned out_page_start, int secondary)
{
    uint32_t compressed_block_addr = 0;
    unsigned long compressed_block_size = 0;
    int ret_val;
    unsigned int src_addr = DLPAGER_ALIGN_DOWN(src_addr_fault, PAGE_SIZE);
    unsigned src_addr_delta = src_addr_fault - src_addr;

    dlpager_get_compressed_block_info_rx(src_addr, &compressed_block_addr, &compressed_block_size);
    assert( compressed_block_addr != 0 && compressed_block_size != 0 )

    if (dlpager_is_addr_in_rodata_range(src_addr) || !dlpager_is_addr_in_text_partial_range(src_addr))
    {
        q6zip_state_t state;
        state.hold_low = *((uint32_t*)compressed_block_addr);
        state.hold_high = *((unsigned*)(compressed_block_addr + 4));
        state.bits_in_hold = 64;
        state.last_sequencial_offset = -1; /* none, just last word */
        dlpager_l2fetch_buffer((void *)compressed_block_addr, compressed_block_size);
        int output_size = q6zip_uncompress((void*)out_page_start, (void*)(compressed_block_addr + 8),
                                           (void*)(dlpager_rx_metadata->dictionary), &state);
        assert(output_size == PAGE_SIZE);

        return;
    }
    else
    {
        dlpager_partial_uncompress(out_page_start, compressed_block_addr,
                                   (void*)dlpager_rx_metadata->dictionary, src_addr_delta, secondary);
    }
}


void dlpager_load_secondary_page(unsigned int src_addr, unsigned int dest_addr)
{
    unsigned block_num;
    rx_stats_t* stat;
	
	block_num = (src_addr - start_va_uncompressed_text) / PAGE_SIZE;
	stat = &rx_stats[block_num];

    if (stat->last_secondary_count >= N_CHUNKS)
    {
        stat->last_secondary_count = N_CHUNKS - 1;
    }

    dlpager_stats.total_text_secondary_chunk[stat->last_secondary_count]++;
    dlpager_stats.total_text_secondary++;

    stat->last_secondary_count++;
    stat->total_secondary_count++;

    dlpager_load_page_rx(src_addr, dest_addr, TRUE);
}

void dlpager_load_primary_page_rx(unsigned int src_addr, unsigned int dest_addr)
{

    uint32_t compressed_block_addr = 0;
    unsigned long compressed_block_size = 0;
    int ret_val;
    unsigned int i,debug_count,debug_addr,debug_mask;
    unsigned int j = 0;

    unsigned block_num = (src_addr - start_va_uncompressed_text) / PAGE_SIZE;
    rx_stats_t* stat = &rx_stats[block_num];
    stat->last_secondary_count = 0;
    stat->total_primary_count++;

    if (dlpager_is_addr_in_rodata_range(src_addr))
    {
        dlpager_stats.total_rodata_faults++;
    }
    else
    {
        dlpager_stats.total_text_faults++;
    }

    dlpager_dczero(dest_addr, PAGE_SIZE);

    dlpager_load_page_rx(src_addr, dest_addr, FALSE);

    if (dlpager_osam_debug_enabled)
    {
        if (dlpager_is_addr_in_text_range(src_addr))
        {
            i = 0;
            debug_count = 0;
            while ((debug_count < dlpager_num_outstanding_writes_code) && (i < DLPAGER_NUM_MAX_OUTSTANDING_WRITES_CODE))
            {
                debug_addr = dlpager_outstanding_writes_code[i].address;
                if (debug_addr != 0) //used entry
                {
                    //address must be in the compressible area
                    j = dlpager_is_addr_in_text_range(debug_addr);
                    assert(j);
                    if ((src_addr <= debug_addr) && (debug_addr < (src_addr + PAGE_SIZE))) //is address in this page
                    {
                        assert(dlpager_outstanding_writes_code[i].state == 2);
                        //store original word present at offset
                        dlpager_outstanding_writes_code[i].orig_value = *(unsigned int *)(dest_addr + (debug_addr - src_addr));
                        //write value to address offset in uncompressed page
                        debug_mask = dlpager_outstanding_writes_code[i].mask;
                        if (debug_mask == 0xFFFFFFFF) //this should be the only case for breakpoints
                            *(unsigned int *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_code[i].value;
                        else if (debug_mask == 0xFFFF)
                            *(unsigned short *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_code[i].value & debug_mask;
                        else if (debug_mask == 0xFF)
                            *(unsigned char *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_code[i].value & debug_mask;
                        else
                            assert(0);
                        //move state to 1
                        dlpager_outstanding_writes_code[i].state = 1;
                    }
                    debug_count++;
                }
                i++;
            }
        }
        else if (dlpager_is_addr_in_rodata_range(src_addr))
        {
            i = 0;
            debug_count = 0;
            while ((debug_count < dlpager_num_outstanding_writes_ro) && (i < DLPAGER_NUM_MAX_OUTSTANDING_WRITES_RO))
            {
                debug_addr = dlpager_outstanding_writes_ro[i].address;
                if (debug_addr != 0) //used entry
                {
                    //address must be in the compressible area
                    j = dlpager_is_addr_in_rodata_range(debug_addr);
                    assert(j);
                    if ((src_addr <= debug_addr) && (debug_addr < (src_addr + PAGE_SIZE))) //is address in this page
                    {
                        //write value to address offset in uncompressed page
                        debug_mask = dlpager_outstanding_writes_ro[i].mask;
                        if (debug_mask == 0xFFFFFFFF)
                            *(unsigned int *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_ro[i].value;
                        else if (debug_mask == 0xFFFF)
                            *(unsigned short *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_ro[i].value & debug_mask;
                        else if (debug_mask == 0xFF)
                            *(unsigned char *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_ro[i].value & debug_mask;
                        else
                            assert(0);
                    }
                    debug_count++;
                }
                i++;
            }
        }
        else assert(0);
    }

    rx_total_decompressions++;
}

void dlpager_load_page_rw (unsigned int src_addr_fault,
                                   unsigned int dest_addr, unsigned int page_size)
{
    unsigned int src_addr = DLPAGER_ALIGN_DOWN(src_addr_fault, PAGE_SIZE);
    uint32_t compressed_block_addr = 0;
    unsigned long compressed_block_size = 0;
    int ret_val, block_num;
    unsigned int i = 0,j = 0,debug_count = 0,debug_write_count = 0,debug_addr,debug_mask;

    DL_DEBUG("Handling RW page for decompression at addr = 0x%x\n",src_addr);

    if (dlpager_is_addr_in_bss_range(src_addr_fault))
        dlpager_stats.total_bss_faults++;
    else
        dlpager_stats.total_rwdata_faults++;


    dlpager_get_compressed_block_info_rw(src_addr,
                                         &compressed_block_addr,
                                         &compressed_block_size);

    dlpager_dczero(dest_addr, page_size);
    if (compressed_block_addr == 0) //bss first time
    {
        /* dczero made this page all 0s */
    }
    else
    {
#if defined L2_PREFETCH
        dlpager_l2fetch_buffer((void *)compressed_block_addr, compressed_block_size);
#endif
        ret_val = deltaUncompress((unsigned int*)compressed_block_addr, (unsigned int*)dest_addr, PAGE_SIZE >> 2);
        assert(ret_val == PAGE_SIZE);
    }

    if (dlpager_osam_debug_enabled)
    {
        while ((debug_count < dlpager_num_outstanding_writes_rw) && (i < DLPAGER_NUM_MAX_OUTSTANDING_WRITES_RW))
        {
            debug_addr = dlpager_outstanding_writes_rw[i].address;
            if (debug_addr != 0) //used entry
            {
                //address must be in the compressible area
                j = dlpager_is_addr_in_rw_range(debug_addr);
                assert(j);
                if ((src_addr <= debug_addr) && (debug_addr < (src_addr + page_size))) //is address in this page
                {
                    //write value to address offset in uncompressed page
                    debug_mask = dlpager_outstanding_writes_rw[i].mask;
                    if (debug_mask == 0xFFFFFFFF)
                        *(unsigned int *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_rw[i].value;
                    else if (debug_mask == 0xFFFF)
                        *(unsigned short *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_rw[i].value & debug_mask;
                    else if (debug_mask == 0xFF)
                        *(unsigned char *)(dest_addr + (debug_addr - src_addr)) = dlpager_outstanding_writes_rw[i].value & debug_mask;
                    else
                        assert(0);
                    //mark entry as free
                    dlpager_outstanding_writes_rw[i].address = 0;
                    dlpager_outstanding_writes_rw[i].value = 0;
                    dlpager_outstanding_writes_rw[i].mask = 0;
                    debug_write_count++;
                }
                debug_count++;
            }
            i++;
        }
        //adjust the outstanding write count for deletions
        dlpager_num_outstanding_writes_rw -= debug_write_count;
    }
    block_num = (src_addr - start_va_uncompressed_rw) >> 12 ;
    rw_stats_t * rw_stat = &rw_stats[block_num];
    rw_stat->decompress_count++;
    rw_total_decompressions++;
}


void dlpager_load_primary_page(unsigned int fault_addr, unsigned int load_addr)
{
    q6zip_clk_check_vote();

    if (dlpager_is_addr_in_rx_range(fault_addr))
        dlpager_load_primary_page_rx(fault_addr, load_addr);
    else if (dlpager_is_addr_in_rw_range(fault_addr))
        dlpager_load_page_rw(fault_addr, load_addr, PAGE_SIZE);
    else
    {
        DL_DEBUG("fault_addr_align does not fall within text or rw sections \n");
        assert(0);
    }
}

/* returns total number of blocks in rw data and bss */
static unsigned int dlpager_make_rw_compress_table(dlpager_rw_metadata_t* rw_metadata_ptr)
{
    uint32_t i,num_rw_blocks,compressed_block_size,num_bss_blocks,bss_size;

    DL_DEBUG("Making RW compress table\n");

    num_rw_blocks = rw_metadata_ptr->n_blocks;

    bss_size = end_va_uncompressed_rw - (unsigned int) &__swapped_segments_bss_start__;
    assert((bss_size & (PAGE_SIZE-1)) == 0); //must be a multiple of page size
    num_bss_blocks = bss_size/PAGE_SIZE;

#ifdef USE_DLPAGER_HEAP
    //initialize DL pager heap
    dlpager_heap_buffer = (byte*) &__dlpager_heap_start__;
#ifdef DLPAGER_CBSP_SUPPORTS_HEAP_LITE
    mem_init_heap_lite
#else
    mem_init_heap
#endif
        (&dlpager_heap,
         dlpager_heap_buffer,
         (unsigned int)&__dlpager_heap_end__ - (unsigned int)&__dlpager_heap_start__,
         NULL);
#endif

    //allocate array of pointers
#ifdef USE_DLPAGER_HEAP
    dlpager_rw_info.table = mem_malloc(&dlpager_heap,sizeof(dlpager_rw_table_t) * (num_rw_blocks + num_bss_blocks));
#else
    dlpager_rw_info.table = malloc(sizeof(dlpager_rw_table_t) * (num_rw_blocks + num_bss_blocks));
#endif

    if ( dlpager_rw_info.table != NULL ) 
    {
        //allocate memory for each compressed block
        for (i = 0; i < num_rw_blocks; i++)
        {
              compressed_block_size = (i + 1 == num_rw_blocks)?
                (end_va_compressed_rw
                - rw_metadata_ptr->compressed_va[i]):
                (rw_metadata_ptr->compressed_va[i+1]
                - rw_metadata_ptr->compressed_va[i]);

#ifdef USE_DLPAGER_HEAP
            dlpager_rw_info.table[i].ptr = mem_malloc(&dlpager_heap,compressed_block_size+4); //1 more for extra 32 bit read into 64 bit word
#else
            dlpager_rw_info.table[i].ptr = malloc(compressed_block_size+4);
#endif
            if ( dlpager_rw_info.table[i].ptr != NULL ) 
            {
                dlpager_rw_info.table[i].size = compressed_block_size;
                memscpy(dlpager_rw_info.table[i].ptr, compressed_block_size,
                (void*)rw_metadata_ptr->compressed_va[i], compressed_block_size );
                rw_initial_compressed_size += compressed_block_size;
            }
            else
            {
                assert(0);
            }
        }

        //loop over bss blocks
       //continue i from above loop
        while (i < (num_rw_blocks + num_bss_blocks))
        {
          dlpager_rw_info.table[i].ptr = NULL;
          dlpager_rw_info.table[i].size = 0;
          i++;
        }
    }
    else 
   {
      assert(0);
   }
     rw_malloced_size[rw_circ_buf_idx] = rw_initial_compressed_size; //idx will be 0
     rw_malloced_size_always[rw_circ_buf_idx_always] = rw_initial_compressed_size; //idx will be 0

     dlpager_rw_info.num_elements = num_rw_blocks + num_bss_blocks;
     return (num_rw_blocks + num_bss_blocks);
}

//Needs to be implemented once we can reclaim
static void dlpager_free_rw_metadata(void)
{
    DL_DEBUG("Freeing compressed RW memory\n");
}

int dlpager_get_attr (dlpager_attr_t *pAttr)
{
    /* Actual interface need to configure the attributes */
    pAttr->swap_pool[0].size = q6zip_swap_pool_size;
    pAttr->swap_pool[0].page_size = PAGE_SIZE;
    pAttr->num_swap_pools = 1;

    return 0;
}

#ifdef DLPAGER_CBSP_SUPPORTS_HEAP_LITE
__attribute__ ((__constructor__))
#endif
void dlpager_q6zip_init2(void)
{
    unsigned int nblocks;

    if (&__swapped_segments_start__ != &__swapped_segments_end__)
    {
        /* rx init */
        dlpager_rx_metadata = (dlpager_rx_metadata_t*)start_va_compressed_text;
        nblocks = dlpager_rx_metadata->n_blocks;
        rx_stats = (rx_stats_t *)malloc(sizeof(rx_stats_t) * nblocks);
        if ( rx_stats != NULL )
        {
           memset( rx_stats, 0, sizeof(rx_stats_t) * nblocks );
        }
        else
        {
          assert(0);
        }

        /* rw init */
        if ( (&__swapped_segments_rw_start__ != 0) && (&__swapped_segments_rw_start__ != &__swapped_segments_end__) )
        {
            q6zip_swap_pool_size += 1024 * 1024;
            dlpager_rw_metadata = (dlpager_rw_metadata_t*)start_va_compressed_rw;
            nblocks = dlpager_make_rw_compress_table( dlpager_rw_metadata );
            dlpager_free_rw_metadata();
            rw_stats = (rw_stats_t *)malloc( sizeof(rw_stats_t) * nblocks );
            if ( rw_stats != NULL )
            {
               memset( rw_stats, 0, sizeof(rw_stats_t) * nblocks );
            }
            else 
            {
               assert(0);
            }
        }
    }
}

int dlpager_q6zip_init(void)
{
    if (&__swapped_segments_start__ != &__swapped_segments_end__)
    {
        q6zip_clk_init();
#ifndef DLPAGER_CBSP_SUPPORTS_HEAP_LITE
        dlpager_q6zip_init2();
#endif
    }
    return 0;
}

