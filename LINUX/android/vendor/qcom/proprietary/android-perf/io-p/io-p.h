/******************************************************************************
  @file    io-p.h
  @brief   Header file for communication and actions for iop

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2011,2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __IO_P_H__
#define __IO_P_H__

/* Server functions */
int  iop_server_init(void);
void iop_server_exit(void);

#define IOP_CMD_PERFLOCK_IOPREFETCH_START 1
#define IOP_CMD_PERFLOCK_IOPREFETCH_STOP 2

int iop_send(int control /* IOP_CMD_* */, ...);

#endif /* __IO_P_H__ */
