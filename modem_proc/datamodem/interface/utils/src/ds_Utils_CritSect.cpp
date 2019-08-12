/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           D S S O C K . C P P

GENERAL DESCRIPTION
  This header file defines the class which implements IDSSock interface

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_CritSect.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CritSect.h"
#include "ps_crit_sect.h"

using namespace ds::Utils;
using namespace ds::Error;

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
CritSect::CritSect
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("CritSect::CritSect(): "
                   "Initing crit sect 0x%p", this);
  PS_INIT_CRIT_SECTION(&mutex);
  refCnt = 1;

} /* CritSect() */

CritSect::~CritSect
(
  void
)
throw()
{

  LOG_MSG_INFO3_1 ("CritSect::~CritSect(): "
                   "Destroying crit sect 0x%p", this);
  PS_DESTROY_CRIT_SECTION(&mutex);

} /* ~CritSect() */

void CritSect::Enter
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION(&mutex);

} /* Enter() */

int CritSect::TryEnter
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1  ("CritSect::TryEnter(): "
                    "Crit sect 0x%p", this);
  return QDS_EOPNOTSUPP;

} /* Enter() */

void CritSect::Leave
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_LEAVE_CRIT_SECTION(&mutex);

} /* Leave() */



