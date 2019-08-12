#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#=============================================================================== 

"""
:Authors: sectools

Main interface to the secimage functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""


import os
import re
import sys
import traceback

# Append current directory in the path
sys.path.append(os.path.dirname(os.path.realpath(__file__)))

from cfgparser import ConfigDir, ConfigParser
import defines
from imageinfo import ImageInfo, StatusInfo
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import store_data_to_file, load_data_from_file, PROGRESS_CB_PYPASS, ProgressNotifier
import sectools.common.utils.c_path as c_path
from sectools.features.isc.encryption_service import get_encdec
from signer.signer import Signer
import stager


PROGRESS_TOTAL_STAGES = 6


class SecImageCore(object):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform various operations (signing, encryption).


    User can select the config file using:

    - :meth:`chipset`
    - :meth:`config_path`

    User can specify images for processing using:

    - :meth:`set_image_path`
    - :meth:`set_meta_build_path`

    User can specify output using:

    - :meth:`output_dir`
    - :meth:`mini_build_path`

    User can finally perform the following operations:

    - :meth:`verify_inputs`
    - :meth:`sign`
    - :meth:`encrypt`
    - :meth:`validate`

    User can also perform multiple operations using :meth:`process`

    :param str config_dir: Path to the directory containing the config files
        for the supported chipsets.
    :param bool debug: If all the underlying modules should work in debug mode
        (save debug logs, etc).

    .. data:: debug

        (bool) Is debug mode enabled. The debug logs will be stored in :meth:`output_dir`

    """

    def __init__(self, config_dir=defines.DEFAULT_CONFIG_DIR, debug=False):
        # Initialize the internal properties
        self._config_dir_obj = ConfigDir(config_dir)
        self._img_config_parser = None
        self._stager = None

        # Initialize the public properties
        self.debug = debug
        self.l1_key = None

    @property
    def config_dir(self):
        """(str) Path to the config directory provided in params. The config_dir
        path cannot be set directly.
        """
        return self._config_dir_obj.config_dir

    @config_dir.setter
    def config_dir(self, config_dir):
        raise RuntimeError('Config dir cannot be set.')

    @property
    def available_chipsets(self):
        """(list[str]) A list of chipsets for which config files are available
        in the :meth:`config_dir`. The list of available chipsets cannot be set
        directly.
        """
        return self._config_dir_obj.chipsets

    @available_chipsets.setter
    def available_chipsets(self, chipset_list):
        raise RuntimeError('Available chipsets cannot be set.')

    @property
    def chipset(self):
        """(str) Chipset that is selected for the current run. The chipset
        selection will update the :meth:`config_path`
        """
        if self._img_config_parser is not None:
            return self._img_config_parser.chipset
        else:
            raise RuntimeError('Chipset/Config Path is not set.')

    @chipset.setter
    def chipset(self, chipset):
        # Log if the chipset is changed
        try:
            selected_chipset = self.chipset
            if selected_chipset:
                logger.note('Switching chipset from "' + selected_chipset + '" to "' + chipset + '"')
        except Exception:
            pass

        # Update the config path based on the given chipset
        self.config_path = self._config_dir_obj.get_chipset_config_path(chipset)
        logger.info('Chipset is set to: ' + chipset)

    @property
    def config_path(self):
        """(str) Config path that is selected for the current run. The
        config_path selection will update the :meth:`chipset`
        """
        if self._img_config_parser is not None:
            return self._img_config_parser.config_path
        else:
            raise RuntimeError('Chipset/Config Path is not set.')

    @config_path.setter
    def config_path(self, config_path):
        # Log if the config_path is changed
        try:
            selected_config_path = self.config_path
            if selected_config_path:
                logger.note('Switching config_path: ' + '\n'
                            '    ' + 'from: ' + selected_config_path + '\n'
                            '    ' + ' to: ' + config_path)
        except Exception:
            pass

        # Update the config_path
        self._img_config_parser = ConfigParser(config_path)
        logger.info('Config path is set to: ' + config_path)

        # Reset the stager at this point
        if self._stager is not None:
            logger.note('Resetting image list due to config path change.')
            self._stager = None

    @property
    def available_sign_ids(self):
        """(list[str]) A list of sign_ids available from the selected
        :meth:`config_path`/:meth:`chipset`.
        """
        if self._img_config_parser is not None:
            return self._img_config_parser.sign_id_list
        else:
            raise RuntimeError('Set chipset or config Path before querying the available sign ids.')

    @available_sign_ids.setter
    def available_sign_ids(self, chipset_list):
        raise RuntimeError('Available sign ids cannot be set.')

    def set_image_path(self, image_path, sign_id):
        """Sets the image that will be processed.

        :param str image_path: Path to the image to be processed
        :param str sign_id: sign_id corresponding to the image_path. This must
            be one of the :meth:`available_sign_ids`
        """
        if self._img_config_parser is not None:
            self._stager = stager.ImagePathsStager(image_path, self._img_config_parser, sign_id)
        else:
            raise RuntimeError('Set chipset or config Path before setting the image path.')

    def set_meta_build_path(self, meta_build_path):
        """Sets the meta build path from which the images to be processed will
        be queried.

        :param str meta_build_path: Path to the meta build.
        """
        if self.meta_supports_sign_id(meta_build_path):
            self._stager = stager.MetaBuildStager(meta_build_path, self._config_dir_obj)
        else:
            self._stager = stager.MetaBuildTmpStager(meta_build_path, self._config_dir_obj, self._img_config_parser)

    @property
    def image_path_list(self):
        """(list[str]) List of source image paths that are being operated on.
        This is updated once one of :meth:`set_image_path`,
        :meth:`set_meta_build_path` is used to specify the images to be
        processed.
        """
        if self._stager is not None:
            return self._stager.image_path_list
        else:
            raise RuntimeError('Image list is not set. '
                               'Set one of image_path or meta_build_path.')

    @image_path_list.setter
    def image_path_list(self, image_path_list):
        raise RuntimeError('Image path list cannot be set directly. '
                           'Use set_image_path or set_meta_build to set the image paths.')

    @property
    def image_info_list(self):
        """(list[ImageInfo]) A list of :class:`imageinfo.ImageInfo` objects that are being
        operated on.
        """
        if self._stager is not None:
            return self._stager.image_info_list
        else:
            raise RuntimeError('Image Info list is not set. '
                               'Set one of image_path or meta_build_path.')

    @image_info_list.setter
    def image_info_list(self, image_info_list):
        raise RuntimeError('Image Info list cannot be set directly.')

    @property
    def output_dir(self):
        """(str) Path to the output directory to use for saving the logs, output
        files.
        """
        if self._stager is not None:
            return self._stager.output_dir
        else:
            raise RuntimeError('Image list is not set. '
                               'Set one of image_path or meta_build_path. '
                               'Then retry setting output directory.')

    @output_dir.setter
    def output_dir(self, output_dir):
        # Log if the output_dir is changed
        try:
            selected_output_dir = self.output_dir
            if selected_output_dir:
                logger.note('Switching output_dir: ' + '\n'
                            '    ' + 'from: ' + selected_output_dir + '\n'
                            '    ' + ' to: ' + output_dir)
        except Exception:
            pass

        # Update the output_dir
        self._stager.output_dir = output_dir
        logger.info('Output dir is set to: ' + output_dir)
        
        # Clear the minimized build setting
        try:
            self._stager.mini_build_path = ''
        except Exception:
            pass

    @property
    def mini_build_path(self):
        """(str) Path to the minimized build directory to use for saving the
        logs, output files.
        """
        if self._stager is not None:
            return self._stager.mini_build_path
        else:
            raise RuntimeError('Image list is not set. '
                               'Set one of image_path or meta_build_path. '
                               'Then retry querying/setting minimized build.')

    @mini_build_path.setter
    def mini_build_path(self, mini_build_path):
        # Log if the mini_build_path is changed
        selected_mini_build_path = self.mini_build_path
        if selected_mini_build_path:
            logger.note('Switching mini_build_path: ' + '\n'
                        '    ' + 'from: ' + selected_mini_build_path + '\n'
                        '    ' + ' to: ' + mini_build_path)

        # Update the mini_build_path
        self._stager.mini_build_path = mini_build_path
        logger.info('Minimized Build is set to: ' + mini_build_path)
    
        # Clear the output dir setting
        try:
            self._stager.output_dir = ''
        except Exception:
            pass

    @classmethod
    def meta_supports_sign_id(self, meta_build_path):
        return stager.meta_build_stager.MetaBuildStager.meta_supports_sign_id(meta_build_path)

    def process(self, verify_setup=False, sign=False, encrypt=False, decrypt=False,
                val_image=False, val_sign=False, val_encrypt=False,
                progress_cb=PROGRESS_CB_PYPASS):
        """Performs the secure-image related operations specified from the params.

        :param bool verify_setup: Verify that the configuration of the object
            is correct and return. This will ignore any other params.
        :param bool sign: Sign the images. (Re-sign if image is already signed)
        :param bool encrypt: Encrypt the images. (Re-encrypt if image is already
            encrypted)
        :param bool val_image: Validate the integrity of the image against the
            config file.
        :param bool val_sign: Validate that the image is signed and validate the
            integrity of the signing related data.
        :param bool val_encrypt: Validate that the image is encrypted and
            validate the integrity of the encryption related data.
        :param cb progress_cb: Callback method to get a status update during
            processing.

            Callback method should have this format:
            ::
                def progress_cb(status_string, progress_percent):
                    \"""
                    :param str status_string: the current status.
                    :param int progress_percent: the progress (in percent)
                    \"""
                    ...

        """
        # Ensure that one or more image files is provided for processing
        if self._stager is None or not self.image_path_list:
            raise RuntimeError('Please specify one or more images for processing')

        # Ensure the correct set of operations is provided
        if not (verify_setup or sign or encrypt or val_image or val_sign or
                val_encrypt or decrypt):
            raise RuntimeError('Please specify one or more operations to perform.')

        if verify_setup:
            logger.note('The inputs provided (config, cmd args) are valid.')
            return

        # Start processing images
        total_images = len(self.image_info_list)
        progress = ProgressNotifier(total_images, progress_cb, PROGRESS_TOTAL_STAGES)

        for idx, image in enumerate(self.image_info_list):
            assert isinstance(image, ImageInfo)

            logger.info('------------------------------------------------------')
            status_string = ('Processing ' + str(idx + 1) + '/' + str(total_images) + ': ' + image.image_under_operation)
            logger.info(status_string + '\n')

            # Send a progress notification to the toplevel
            progress.status = status_string
            progress.cur = idx
            progress.cur_stage = 0

            try:
                # Create the required directory structure for this image
                image_output_dir = image.dest_image.image_dir
                try:
                    c_path.create_dir(image_output_dir)
                except Exception as e:
                    raise RuntimeError('Could not create output directory: ' + image_output_dir + '\n'
                                       '    ' + 'Error: ' + str(e))

                # Enable/Disable debug
                image.dest_image.debug_enable = self.debug
                c_path.create_debug_dir(image.dest_image.debug_dir)

                # Create the encryptor object
                if encrypt or decrypt or val_encrypt:
                    try:
                        c_path.create_debug_dir(image.dest_image.debug_dir_encdec)
                        encdec = get_encdec(image)
                    except Exception:
                        if not val_encrypt:
                            raise
                else:
                    encdec = None
    
                # Create the parsegen object
                image.encdec = encdec
                parsegen = self._status_updater(self._create_parsegen_obj,
                                                image.status.parsegen,
                                                image)
                progress.push()
                
                # Set the security mechanisms
                parsegen.sign = parsegen.is_signed() or sign
                parsegen.encrypt = parsegen.is_encrypted() or encrypt
                
                # If encrypt:
                if encrypt:
                    parsegen.encryption_params = encdec.get_encryption_parameters_blob()
                
                # Dump any debug data
                self.dump_parsegen_debug_data(image, parsegen)
                
                # Sign the image
                if sign:
                    self._status_updater(self._sign_image,
                                         image.status.sign,
                                         image, parsegen)
                progress.push()
                
                # Package and generate the output file
                if sign or encrypt:
                    store_data_to_file(image.dest_image.image_path, parsegen.get_data())
                    if encrypt:
                        image.status.encrypt.state = StatusInfo.SUCCESS
                        
                    logger.info(('Signed ' if sign else '') +
                                ('& ' if sign and encrypt else '') +
                                ('Encrypted ' if encrypt else '') +
                                'image is stored at ' + image.dest_image.image_path)
                    image.image_under_operation = image.dest_image.image_path

                    # Do any post processing
                    self._status_updater(self._post_process,
                                         image.status.postprocess,
                                         image, image.config.post_process.pil_splitter,
                                         getattr(self._stager, '_meta_build_path', None))
                    progress.push()

                # Print the image data
                logger.info('\n' + str(parsegen))
                
                # Decrypt the image
                if decrypt:
                    store_data_to_file(image.dest_image.decrypted_file, parsegen.get_data(encrypt=False))

                # Validate parsegen
                if val_image:
                    self._validate_parsegen(image, parsegen)
                progress.push()

                # Validate sign
                if val_sign:
                    self._validate_sign(image, parsegen)
                progress.push()

                # Validate encrypt
                if val_encrypt:
                    self._validate_encrypt(image, parsegen)
                progress.push()
                
                # Set overall processing to true
                image.status.overall.state = StatusInfo.SUCCESS
                
            except Exception:
                logger.debug(traceback.format_exc())
                logger.error(sys.exc_info()[1])

            logger.info('------------------------------------------------------\n')
        progress.complete()

    def _create_parsegen_obj(self, image):
        c_path.create_debug_dir(image.dest_image.debug_dir_parsegen)
        from sectools.features.isc.parsegen import get_parser
        parsegen = get_parser(image)
        return parsegen

    def _sign_image(self, image, parsegen):
        logger.info('Resigning signed' if parsegen.is_signed() else 'Signing'
                    ' image: ' + image.image_under_operation)

        c_path.create_debug_dir(image.dest_image.debug_dir_signer)
        signer = Signer(image.config)
        sign_assets = signer.sign(parsegen.data_to_sign, image)

        # Dump any debug information
        self.dump_signer_debug_data(image, sign_assets)

        parsegen.data_signature = sign_assets.signature
        parsegen.cert_chain = sign_assets.cert_chain

    def _post_process(self, image, pil_splitter_path, meta_build_path):
        ''' Replacement tags in postsign commands for images. '''
        TAG_OUTPUT_DIR = '$(OUTPUT_DIR)'
        TAG_PIL_SPLITTER = '$(PIL_SPLITTER)'
        TAG_IMAGE_FILE = '$(IMAGE_FILE)'
        REPL_META_PATH = '$(META_BUILD)'

        if image.post_process_commands:
            # Ensure pil splitter is available
            if pil_splitter_path.find(REPL_META_PATH) != -1:
                if not meta_build_path:
                    raise RuntimeError('Metabuild path is not available for pil splitter')
                pil_splitter_path = pil_splitter_path.replace(REPL_META_PATH, meta_build_path)

            if not c_path.validate_file(pil_splitter_path):
                raise RuntimeError('Cannot access pil splitter at: ' + pil_splitter_path)

            # Run all the commands
            for cmd in image.post_process_commands:
                # Perform any needed replacements
                cmd = cmd.replace(TAG_OUTPUT_DIR, os.path.dirname(image.image_under_operation))
                cmd = cmd.replace(TAG_PIL_SPLITTER, pil_splitter_path)
                cmd = cmd.replace(TAG_IMAGE_FILE, image.image_under_operation)

                logger.info('Running postsign command: ' + cmd)
                err = os.system(cmd)
                logger.info('Result: ' + str(err))

    def _validate_parsegen(self, image, parsegen):
        #logger.warning('Validating parsegen is not yet supported')
        image.status.validate_parsegen.state = StatusInfo.SUCCESS

    def _validate_sign(self, image, parsegen):
        if parsegen.is_signed():
            try:
                signer = Signer(image.config)
                if signer.validate(parsegen):
                    logger.info('Image ' + image.image_under_operation + ' is valid')
                    image.status.validate_sign.state = StatusInfo.SUCCESS
                else:
                    logger.info('Image ' + image.image_under_operation + ' is not valid')
                    image.status.validate_sign.state = StatusInfo.ERROR
            except Exception as e:
                image.status.validate_sign.state = StatusInfo.ERROR
                image.status.validate_sign.error = str(e)
                raise
        else:
            logger.info('Image ' + image.image_under_operation + ' is not signed')
            image.status.validate_sign.state = StatusInfo.ERROR

    def _validate_encrypt(self, image, parsegen):
        #logger.warning('Validating encryption is not yet supported')
        image.status.validate_encrypt.state = StatusInfo.SUCCESS

    def _status_updater(self, method, status, *args, **kwargs):
        try:
            retval = method(*args, **kwargs)
        except Exception as e:
            status.state = StatusInfo.ERROR
            status.error = str(e)
            raise
        status.state = StatusInfo.SUCCESS
        return retval

    def dump_parsegen_debug_data(self, image, parsegen):
        if image.dest_image.debug_dir_parsegen is None:
            return

        so = parsegen
        fp = image.dest_image
        from imageinfo import DestImagePath
        assert isinstance(fp, DestImagePath)

        debug_logs = []
        try: debug_logs.append((so.get_data(False, False), fp.debug_file_parsegen_unsigned))
        except Exception: pass
        try: debug_logs.append((so.data_to_sign, fp.debug_file_parsegen_tosign))
        except Exception: pass
        try: debug_logs.append((so.cert_chain, fp.debug_file_parsegen_cert_chain))
        except Exception: pass
        try: debug_logs.append((so.data_signature, fp.debug_file_parsegen_signature))
        except Exception: pass
        try: debug_logs.append((so.get_data(True, False), fp.debug_file_parsegen_signed))
        except Exception: pass
        try: debug_logs.append((so.get_data(True, True), fp.debug_file_parsegen_encrypted))
        except Exception: pass
        
        for data, debug_file in debug_logs:
            try: store_data_to_file(debug_file, data)
            except Exception: logger.debug2('Failed to save debug file ' + debug_file + '\n'
                                            '    ' + str(sys.exc_info()[1]))

    def dump_signer_debug_data(self, image, sign_assets):
        if image.dest_image.debug_dir_signer is None:
            return

        sa = sign_assets
        fp = image.dest_image
        from imageinfo import DestImagePath
        assert isinstance(fp, DestImagePath)

        debug_logs = [(sa.root_cert, fp.debug_file_signer_root_cert),
                      (sa.attestation_ca_cert, fp.debug_file_signer_attestation_ca_cert),
                      (sa.attestation_cert, fp.debug_file_signer_attestation_cert),
                      (sa.attestation_key, fp.debug_file_signer_attestation_key),
                      (sa.signature, fp.debug_file_signer_signature),
                      (sa.cert_chain, fp.debug_file_signer_cert_chain)]

        for data, debug_file in debug_logs:
            try: store_data_to_file(debug_file, data)
            except Exception: logger.debug2('Failed to save debug file ' + debug_file + '\n'
                                            '    ' + str(sys.exc_info()[1]))


#------------------------------------------------------------------------------
# Restrict all import
#------------------------------------------------------------------------------
__all__ = ['SecImageCore',
           'cfgparser',
           'cryptoutils',
           'parsegen', ]

