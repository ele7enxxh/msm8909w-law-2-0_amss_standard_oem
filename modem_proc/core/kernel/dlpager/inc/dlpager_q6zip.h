#ifndef DLPAGER_Q6ZIP_H
#define DLPAGER_Q6ZIP_H
#include "q6zip_uncompress.h"

/* Some typedefs for compatibility */
typedef unsigned char               uint8_t;
typedef unsigned short              uint16_t;
typedef unsigned long               uint32_t;
typedef unsigned long long          uint64_t;

/* Blob containing metadata used to map uncompressed text to
   compressed text*/
struct __attribute__ ((__packed__)) dlpager_rx_metadata
{
  uint16_t n_blocks;  /* number of uncompressed pages of text */
  uint16_t version;
  uint32_t dictionary[Q6ZIP_DICT_SIZE];
  uint32_t compressed_va[1]; /* contains n_blocks of uint32 addresses
                                mapping uncompressed text to compressed text */
};
typedef struct dlpager_rx_metadata dlpager_rx_metadata_t;


/* Blob containing metadata used to map uncompressed text to
   compressed text*/
struct __attribute__ ((__packed__)) dlpager_rw_metadata   
{
  uint16_t n_blocks;  /* number of uncompressed pages of text */
  uint16_t version;
  uint32_t compressed_va[1]; /* contains n_blocks of uint32 addresses
                                mapping uncompressed text to compressed text */
};
typedef struct dlpager_rw_metadata dlpager_rw_metadata_t;



int dlpager_q6zip_init(void);
extern void recompress_rw_page(unsigned int uncompressed_page_addr, unsigned int mapped_page_addr);
extern void dlpager_load_primary_page(unsigned int src_addr, unsigned int dest_addr);
extern void dlpager_load_secondary_page(unsigned int src_addr, unsigned int dest_addr);
extern void dlpager_l2fetch_buffer(void *addr, uint32_t len);

#endif /* DLPAGER_Q6ZIP_H */
