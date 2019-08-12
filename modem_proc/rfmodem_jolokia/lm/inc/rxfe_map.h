#ifndef RXFE_MAP_H
#define RXFE_MAP_H

#include "comdef.h"

typedef enum
{
    QSIZE_6 = 8,
    QSIZE_8 = 4,
    QSIZE_10 = 12,
    QSIZE_12 = 9,
    QSIZE_16 = 5,
    QSIZE_20 = 13,
    QSIZE_24 = 10,
    QSIZE_32 = 6
} tMemplBufferQsizeType;

typedef union
{
    struct
    {
        uint32 vbuf_len : 16; /* W0 [15:0] */
        uint32 min_bank : 3; /* W0 [18:16] */
        uint32 max_bank : 3; /* W0 [21:19] */
        uint32 init_bank : 3; /* W0 [24:22] */
        tMemplBufferQsizeType q_size : 4; /* W0 [28:25] */
        uint32 : 3;
        uint32 start_line : 15; /* W1 [14:0] */
        uint32 : 17;
    };
    uint32 cfg[2];
} tMemplBufferCfg;

#define NUM_RXFE_BUF 4

#define RXFE_WRTR_BUF_0 0
#define RXFE_WRTR_BUF_1 1
#define RXFE_WRTR_BUF_2 2
#define RXFE_WRTR_BUF_3 3

#define MEMPL_CHAIN_OFFSET 0x1000   //mempool start line offset between chains

#define MEMPL_NB_OFFSET 0x200   //mempool start line offset between NBs

#endif