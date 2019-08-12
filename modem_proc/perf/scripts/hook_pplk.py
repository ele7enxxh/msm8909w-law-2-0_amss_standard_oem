# =============================================================================
# myps builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder definitions
#
# Copyright (c) 2009-2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# =============================================================================

import os
import sys
import inspect

# add current directory, for import
sys.path.insert(
  0,
  os.path.dirname(inspect.getframeinfo(inspect.currentframe()).filename))

import pplkcmd


# -----------------------------------------------------------------------------
# Hooks for SCons
# -----------------------------------------------------------------------------
def exists(env):
  return env.Detect('pplkcmd.main')


def generate(env):
  env.Append(BUILDERS={
    'Pplk': env.Builder(
      action=env.GetBuilderAction(pplkcmd.main),
    ),
  })
