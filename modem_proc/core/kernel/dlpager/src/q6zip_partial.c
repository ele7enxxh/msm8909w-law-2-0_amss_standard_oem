/*===========================================================================
 * FILE:         q6zip_partial.c
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  q6zip partial decompressions support
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

#include <stdlib.h>
#include <assert.h>
#include "q6zip_partial.h"
#include "q6zip_uncompress.h"
#include "dlpager_constants.h"
#include "memload_handler.h"
#include "dlpager_q6zip.h"

void dlpager_partial_uncompress(unsigned out_page_start, unsigned compressed_block_addr,
                                void* dict, int delta, int secondary)
{
    typedef struct {
        signed int last_sequencial:10;
        unsigned int bits_left:6;
        unsigned int in_delta:10;
        signed int out_delta_from_chunk_size:6;
    } chunk_metadata_t;

    /* For N_CHUNKS=4 there are 5 chunks, first is special added to have first few words
       available regardless of what "real chunks" (others, not the special one) are loaded

          +-------------+----------------+----------------+----------------+----------------+
          | small chunk |   real chunk   |   real chunk   |   real chunk   |   real chunk   |
          +-------------+----------------+----------------+----------------+----------------+
       border[0]    border[1]        border[2]        border[3]        border[4]         border[5]
         (0)     chunk_meta[0]     chunk_meta[1]    chunk_meta[2]    chunk_meta[3]      (PAGE_SIZE)

    */

    typedef struct {
        chunk_metadata_t chunk[N_CHUNKS]; /* only N_CHUNKS metadata entries are needed */
    } block_metadata_t;

    block_metadata_t* block_metadata = (block_metadata_t*)compressed_block_addr;

    unsigned borders[N_CHUNKS + 2];
    borders[0] = 0;
    borders[1] = (block_metadata->chunk[0].out_delta_from_chunk_size) * 4; /* first chunk is small */
    int i;
    for (i = 2; i < (N_CHUNKS + 1); ++i)
    {
        borders[i] = borders[i-1] + PAGE_SIZE/N_CHUNKS +
            ((int)block_metadata->chunk[i-1].out_delta_from_chunk_size) * 4;
    }
    borders[N_CHUNKS + 1] = PAGE_SIZE;

    unsigned in    = compressed_block_addr + sizeof(block_metadata_t);
    unsigned out   = out_page_start;
    int last_seq   = -1; /* -1 is the same as no previous seq decompression to continue */
    int bits_left  = 32; /* any value 32 or less is handled as a left over from previous load */
    int loop_count = 0;
#ifdef DLPAGER_QURT_SUPPORTS_SSR
    int do_all_chunks = dlpager_osam_debug_enabled; /* if debugger present do all chunks */
#else
    int do_all_chunks = 1;
#endif

    while (loop_count < (N_CHUNKS + 1))
    {
        /* Update state, add 32 bits to current hold */
        uint64_t hold = *((unsigned*)(in));
        hold >>= (32 - bits_left);
        hold |= (uint64_t)(*((unsigned*)(in + 4))) << bits_left;
        int output_size_meta = ((int)block_metadata->chunk[loop_count].out_delta_from_chunk_size) * 4;
        if (loop_count != 0) output_size_meta += PAGE_SIZE/N_CHUNKS;

        if (do_all_chunks ||
            (*((unsigned *)out) == 0 &&  /* load only if area is filled with 0s */
             ((loop_count == 0) ||             /* always load first chunk for border case */
              (borders[loop_count] <= delta && delta < borders[loop_count + 1]))))
        {
#define ESTIMATED_CHUNK_INPUT_BYTES (((PAGE_SIZE/N_CHUNKS) * 5) >> 3)
            if (loop_count != 0)
                dlpager_l2fetch_buffer((void *)(in + CACHE_LINE_SZ), ESTIMATED_CHUNK_INPUT_BYTES);

            q6zip_state_t state;
            state.hold_low = hold;
            state.hold_high = hold >> 32;
            state.bits_in_hold = bits_left + 32;
            state.last_sequencial_offset = last_seq;

            unsigned output_size = q6zip_uncompress( /* in+8 because 2 words already in state's hold */
                (void*)out, (void*)(in + 8), (void*)dict, &state);

            DL_DEBUG("output_size %d  output_size_meta %d  out - out_page_start %d\n", output_size, output_size_meta, out - out_page_start);

            assert(output_size == output_size_meta ||
                   (output_size + (out - out_page_start)) == PAGE_SIZE);

            if (secondary)
            {
                /* update caches for decompressed area
                   note that I-cache also needs to be invalidated to ensure
                   only whole packets are filled with zeros */
                int size = (output_size & ~(CACHE_LINE_SZ - 1)) + CACHE_LINE_SZ;
                int delta = out - out_page_start;
                if (delta + size > PAGE_SIZE)
                    size = PAGE_SIZE - delta;
                dlpager_memload_cache_update(delta, size);
            }
        }
        if (loop_count < N_CHUNKS)
        {
            out        += output_size_meta;
            in         += block_metadata->chunk[loop_count].in_delta * 4;
            bits_left  = block_metadata->chunk[loop_count].bits_left;
            last_seq   = block_metadata->chunk[loop_count].last_sequencial;

            DL_DEBUG("output_size_meta %d  in_delta %d  bits_left %d  last_seq %d\n",
                     output_size_meta, block_metadata->chunk[loop_count].in_delta * 4,
                     bits_left, last_seq);
        }
        loop_count++;
    }
    assert(N_CHUNKS == N_COMPRESSION_BLOCKS);
}
