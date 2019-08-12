/* memload_handler.h */

#ifndef  _MEMLOAD_HANDLER_H_
#define  _MEMLOAD_HANDLER_H_

/* includes */

/* constants-defines */
#define PAGE_SHIFT_BITS 		12
#define NUM_SWAP_POOLS		        2
#define MAPPING_TABLE_SIZE	        4096
#define MI_PBT_FLAG_POOL_INDEX_SHIFT    0x1B
#define SIZE_4K                         0x1000
#define MAX_FAULTING_THREADS 8


extern unsigned int __swapped_segments_start__;
extern unsigned int __swapped_segments_end__;
extern int memcpyK (void *dest, void *src, size_t n);
static unsigned int logical_start = 0xe0000000;

/* data structures */
struct mapping_table_entry {
  unsigned int virtAddr;
  unsigned int physAddr;
};
typedef struct mapping_table_entry mapping_table_entry_t;

struct swap_region {
  int pool_index;
  int flags;
};
typedef struct swap_region swap_region_t;

/* swap pool */
struct active_page_list_entry {
  uintptr_t addr;
  uintptr_t paddr;
} active_page_list_entry;

struct load_log_list_entry {
  uintptr_t loaded_vaddr;
  unsigned int thread; //thread_id
  uintptr_t evicted_vaddr;
} load_log_list_entry;

struct active_page {
  char size[4096];
} active_page;

struct swap_page_pool{
  //struct active_page * active_page_pool; //this is the base addr of the phys pool
  unsigned int active_page_pool; //base addr of phys pool
  int active_page_pool_size;
  struct active_page_list_entry *active_page_list;
  int active_page_list_count;
  int active_page_list_index;
  int page_replacement_count; 

  /* log victims */ 
  struct load_log_list_entry* load_log_list;
  int load_log_list_count;
  int load_log_list_index;
};
typedef struct swap_page_pool swap_page_pool_t;

//kernel side:

typedef struct _pagefault_t
{
    unsigned int thread_id;
    unsigned int faulting_addr;
    unsigned int pad[4];
} pagefault_t ;

typedef struct _swap_pools_t
{
    unsigned int swap_pool0_addr;
    unsigned int swap_pool0_size;
    unsigned int swap_pool1_addr;
    unsigned int swap_pool1_size;
    unsigned int pad[2];
} swap_pools_t ;

extern void dl_segment_int(swap_pools_t *dl_pools_configs);
extern void init_active_page_pool (int, unsigned int, int);
extern void dump_mapping_table (void);
extern void dump_pool (int);


#endif /* _MEMLOAD_HANDLER_H_ */
