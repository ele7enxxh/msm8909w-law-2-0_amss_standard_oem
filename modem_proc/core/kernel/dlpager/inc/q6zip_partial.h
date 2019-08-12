#ifndef __Q6ZIP_PARTIAL_H__
#define __Q6ZIP_PARTIAL_H__

/*===========================================================================
 * FILE:         q6zip_partial.h
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  q6zip partial decompressions support
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

void dlpager_partial_uncompress(unsigned out_page_start, unsigned compressed_block_addr,
                                void* dict, int delta, int deferred);

#endif
