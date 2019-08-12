#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

'''
Created on Feb 24, 2014

@author: francisg
'''

import abc

class BaseValidator(object):
    '''
    validator base class
    '''
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        '''
        Constructor
        '''
        print 'I am ' + self.val_type

    @abc.abstractproperty
    def val_type(self):
        return 'BASE'

    @abc.abstractproperty
    def validate(self):
        pass


__all__ = ['secdat_validator',
          ]
