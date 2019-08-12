#ifndef RBCPRTASK_H
#define RBCPRTASK_H
/*===========================================================================
                             rbcpr_task.h

SERVICES:

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the classes, macros, etc. defined in
  this file.

PUBLIC CLASSES AND FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rpm_task.h"
#include <stdint.h>
#include "rbcpr_comdef.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
struct rbcpr_bsp_rail_info;

//===========================================================================
//                             Class Definitions
//===========================================================================
class RBCPRTask : public Task
{
 public:
  virtual uint64_t get_length() const;
  virtual uint64_t get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut);
  virtual void reestimate();
  virtual bool hasImmediateWork() const;
  
  void enqueue ();
  void cornerChange ();
  enum ExecutionState
  {
    IDLE,
    ACTIVE,
    ACTIVE_STALE
  };
  
  RBCPRTask (rbcpr_bsp_rail_info* client_rail, uint32_t client_interrupt_id, uint8_t priority);

 private:
  virtual ~RBCPRTask ();

  virtual void execute_until(volatile bool &preempt, uint64_t stop_time);
  
  ExecutionState state_;
  uint16_t interrupt_id;
  uint32_t staleCount;
  rbcpr_bsp_rail_info* rail;
};

#endif // RBCPRTASK_H
