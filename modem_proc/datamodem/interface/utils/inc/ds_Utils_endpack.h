#ifndef AEEBEGINPACK_H
#error "Improper inclusion of AEEendpack.h - see AEEendpack.h for usage"
#endif /* #ifdef AEEBEGIN_H */

#undef AEEBEGINPACK_H
/*
==========================================================================

File:         AEEendpack.h

Description:  Structure packing pragma's and declarations

Usage:
              See AEEbeginpack.h

===========================================================================
             Copyright 2006 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */
#if defined(_MSC_VER)

# pragma pack(pop)

#elif defined(__ARMCC_VERSION)

#elif defined(__GNUC__)

  __attribute((packed))

#else
#error "Unsupported compiler"
#endif

/*
==========================================================================
  DATA STRUCTURES DOCUMENTATION
==========================================================================

==========================================================================
CONSTANTS DOCUMENTATION
==========================================================================

==========================================================================
*/

