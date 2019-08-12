/*===========================================================================
                              rbcpr_task.cpp

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rbcpr_task.h"
#include "CoreVerify.h"
#include "time_service.h"
#include "cortex-m3.h"
#include "swevent.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
extern "C" void rbcpr_handler(rbcpr_bsp_rail_info*);

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
RBCPRTask::RBCPRTask (rbcpr_bsp_rail_info* client_rail, uint32_t client_interrupt_id, uint8_t priority)
    : Task (priority),
      state_ (IDLE), interrupt_id(client_interrupt_id), staleCount (0), rail(client_rail)
{
}

RBCPRTask::~RBCPRTask ()
{
}

uint64_t RBCPRTask::get_length() const
{
  return 30;
}

uint64_t RBCPRTask::get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut)
{
  systemStateOut = 0;
  return get_length ();
}

void RBCPRTask::reestimate()
{
}

bool RBCPRTask::hasImmediateWork() const
{
  return (state_ != IDLE);
}

void RBCPRTask::execute_until(volatile bool &preempt, uint64_t stop_time)
{
  if (preempt)
  { 
    return;
  }

  if (state_ == ACTIVE)
  {
    uint64_t start = time_service_now();

    if ((start + get_length ()) > stop_time)
    {
      return;
    }

    rbcpr_handler(rail);
  }

  state_ = IDLE;
  interrupt_enable_no_clear(interrupt_id);
  }

void RBCPRTask::enqueue ()
{
  CORE_VERIFY (state_ == IDLE);
  state_ = ACTIVE;
  
  interrupt_disable(interrupt_id);
  
  schedule_me(IGNORE_DEADLINE);
}

void RBCPRTask::cornerChange ()
{
  if (state_ == ACTIVE)
  {
    state_ = ACTIVE_STALE;
    staleCount ++;
  }
}

extern "C" void RBCPRTaskISR(void* task)
{
  SWEVENT(RBCPR_ISR, task);
  
  RBCPRTask *the_task = (RBCPRTask*)task;
  CORE_VERIFY (the_task != NULL);
  the_task->enqueue ();
}

extern "C" void* RBCPRTaskInit(rbcpr_bsp_rail_info* rail, uint32_t client_interrupt_id)
{
  return (void*) new RBCPRTask(rail, client_interrupt_id, 1);
}

extern "C" void RBCPRTaskCornerChange(void* task)
{
    RBCPRTask *the_task = (RBCPRTask*)task;
    CORE_VERIFY (the_task != NULL);
    the_task->cornerChange ();
}
