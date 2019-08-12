#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

'''
Created on Feb 26, 2014

@author: francisg
'''


from . import BaseValidator
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.parsegen.secdat import SecdatParseGen


class SecDatValidator(BaseValidator):
    '''
    secDatValidator
    '''
    def __init__(self, oem_config_path, qc_config_path,
                 user_config_path, ui_config_path,sec_dat_path, config, feature):
        '''
        Constructor
        '''
        self.oem_config_path = oem_config_path
        self.qc_config_path = qc_config_path
        self.user_config_path = user_config_path
        self.ui_config_path = ui_config_path
        self.sec_dat_path = sec_dat_path

        self.config_parser = config.ConfigParser()
        self.secdat_parser = SecdatParseGen(feature)

    def val_type(self):
        return 'secDatValidator'

    def validate(self):

        logger.info( 'Validating... \n' )

        retVal = True

        self.generateConfigModel()
        self.generateSecDatModel()

        try:
            if self.config_parser.data_model.compare(self.secdat_parser.data_model, 'config files', 'sec.dat'):
                logger.info( 'Validate Successful!' )
        except Exception as e:
            append_str = ('Validation of the secdat file against the config files failed.' + '\n'
                          '\n'
                          'Differences Details: ' )
            raise RuntimeError(append_str + str(e))

        return retVal

    def generateConfigModel(self):
        '''
        generates model from both oem and qc config xml's and combines them into single data model
        '''
        self.config_parser.read(self.oem_config_path, self.qc_config_path, self.ui_config_path, self.user_config_path)

    def generateSecDatModel(self):
        '''
        generates model from sec.dat binary
        '''
        self.secdat_parser.read(self.sec_dat_path)