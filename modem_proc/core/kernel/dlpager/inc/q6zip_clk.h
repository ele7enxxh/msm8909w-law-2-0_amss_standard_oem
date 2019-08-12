#ifndef __Q6ZIP_CLK_H__
#define __Q6ZIP_CLK_H__
/*===========================================================================
 * FILE:         q6zip_clk.h
 *
 * DESCRIPTION:  Q6zip clock voting management
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
 ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/inc/q6zip_clk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/14   ao      Initial revision
===========================================================================*/

void q6zip_clk_init(void);
void q6zip_clk_check_vote(void);
unsigned  q6zip_clk_query(void);

#endif
