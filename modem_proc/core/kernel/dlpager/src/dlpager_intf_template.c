
#include <dlpager_main.h>

#define SWAP_POOL_SIZE  (4*1024*1024)
#define PAGE_SIZE       (4*1024)

int dlpager_get_attr (dlpager_attr_t *pAttr)
{

    /* Actual interface need to configure the attributes */

    pAttr->swap_pool[0].size = SWAP_POOL_SIZE;
    pAttr->swap_pool[0].page_size = PAGE_SIZE;
    pAttr->num_swap_pools = 1;

    return 0;
}

void dlpager_load_virtual_page (unsigned int src_addr,
		               unsigned int dest_addr, unsigned int page_size)
{
    /* Actual interface need to provide the mechanism to load the virtual page */

}
