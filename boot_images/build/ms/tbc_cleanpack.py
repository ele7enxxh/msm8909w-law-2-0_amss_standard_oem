#============================================================================
#  Name:                                                                     
#    tbc_cleanpack.py 
#
#  Description:                                                              
#    None 
#                                                                            
# Copyright (c) 2016 by QUALCOMM, Incorporated.  All Rights Reserved.        
#============================================================================
#                                                                            
# *** AUTO GENERATED FILE - DO NOT EDIT                                      
#                                                                            
# GENERATED: Thu Dec  8 04:42:18 2016 
#============================================================================
def exists(env):
   return env.Detect('tcfg_cleanpack')

def generate(env):
   env.AddUsesFlags('USES_CLEAN_PACK',  True)
