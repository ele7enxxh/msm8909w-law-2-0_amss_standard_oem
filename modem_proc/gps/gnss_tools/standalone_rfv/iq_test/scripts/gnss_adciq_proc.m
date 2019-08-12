% =============================================================================================
% ADC-IQ TEST PROCESSING SCRIPT FOR GPSONE GEN 8 CALLISTO ENGINE FAMILY
% =============================================================================================
% This MATLAB post process script is compatible with ALL Gen 8 family flavors: Gen 8/8A/8B/8C
%=============================================================================================
% Copyright (C) 2003-2012 QUALCOMM Incorporated.
% All Rights Reserved.
% Qualcomm Confidential and Proprietary.
% Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
% Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
%
% =============================================================================================
%
% REVISION HISTORY
% ----------|----------------|-----------------|-------------------------------------------------------
% Version |Date                  |  Who                  |Changes
%------------------------------------------------------------------------------------------------
% 2.1.2       |04/15/2015 |tianqiw              | Add the automated noise power spectrum density increase
% 2.1.1       |12/16/2014 |tianqiw              | Add the I/Q saturation
%                  |                            |                               |           rate on the I/Q CDF plot;
% 2.1           |11/13/2014 |tianqiw              | Add the support of GAL
% 2.04        |11/04/2014 |tianqiw              | Added Gen8B/8C flag for ver53 parsing
% 2.03        |10/29/2014 |tianqiw              | Added error handling
%                  |                             |                              | codes for ver54 parsing issue
% ----------|----------------|-----------------|--------------- ----------------------------------
% 2.0.2       |08/12/2014 |tianqiw              | Add the support for B27
%                  |                            |                               | for 2H analysis
% ----------|----------------|-----------------|--------------- ----------------------------------
% 2.0.1       |07/29/2014  |tianqiw             |  Fixed minor bugs for
%                  |                             |                              |  backward compatibility
% ----------|----------------|-----------------|--------------- ----------------------------------
% 2.0           |06/12/2014 |tianqiw             | merged the support of
%                  |                            |                              | packet version 54 for the log file;
% ----------|----------------|-----------------|--------------- ----------------------------------
% 1.06        |03/03/2014 |tianqiw             | add the support of B13 RB45
% ----------|----------------|-----------------|--------------- ----------------------------------
% 1.05        |02/14/2014 |tianqiw             | add the output of the RMS voltage/power of I+Q/ I / Q components
% ----------|----------------|-----------------|--------------- ----------------------------------
% 1.04        |01/01/2014 |tianqiw             | add the support of B13 RB46, B14 all RB, and B14 RB 0
% ----------|----------------|-----------------|--------------- ----------------------------------
% 1.03        |11/26/2013 |tianqiw             | change the RF reference tone from IF to RF frequency;
% ----------|----------------|-----------------|--------------- ----------------------------------
% 1.02        |11/15/2013 |tianqiw             |adds Gen 8C support
% ----------|----------------|-----------------|--------------- ----------------------------------
% 1.01        |08/09/2013 |tianqiw             |adds input parameter:  pass/fail flag and BDS support
% ----------|----------------|-----------------|--------------- ----------------------------------
% 1.0           |01/01/2013 | tpals                  |creates
% ----------|----------------|-----------------|--------------- ----------------------------------
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [total_rms,...
    total_rms_isamp,...
    total_rms_qsamp,...
    noise_pdsdelta_gpsglo_dB, ...
    signal_power,...
    signal_freq,...
    mean_iq_amp_mv,...
    cdf_adc_amp,...
    adc_amp...
    ] = ...
    gnss_adciq_proc(...
    iq_file,...                                                         % The ACD IQ collection bin file;
    save_results,...                                            % 1 : save the generated figure; 0: do not save the figure;
    widejam_band,...                                       % [lower_frequency_boud_in_Hz upper_frequency_bound_in_Hz] defines a frequency range where a wide band jammer is needed;
    flag_ref_tone,...                                           %  1: means there is reference tone; 0 no reference tone;
    flag_mpr_totalpower,...                          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the total power; 0 turn off;
    flag_mpr_cwjammer,...                           % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the CW tone jammer; 0 turn off;
    flag_mpr_gpsglo_powerdelta,...          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the GPS/GLO band noise power sepctrum density (PSD) comparison; 0 turn off;
    flag_mpr_meaniq,...                                  % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the mean IQ (DC) component; 0 turn off;
    flag_mpr_highiq,...                                    % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the high IQ (DC) component; 0 turn off;
    SigFreqKhz_RF,...                                      % Reference tone frequency (kHz) in RF;
    SigLevelDbm,...                                          % Reference tone power level in dBm;
    b13TxOn)                                                    % 1: means the ADC IQ is collected during the B13 Tx traffic; 0: means no B13 Tx traffic during IQ collection;
% Version number
VersionNum = '2.1.2';
%====================================================================
%   THE USAGE OF THE MATLAB POST PROCESSING SCRIPTS
%====================================================================
% Initial check if deployment options;
if( isdeployed ) %close all figures, including the hidden ones, if this script runs in the deployed mode in QDART.
    close all hidden;
else
    close all;
end %do not close any figure if this script runs in the standalone mode in a regular Matlab window.

%--------------------------------------------------------------------------
%           SETTING INPUT PARAMETERS
%--------------------------------------------------------------------------
adc_config_flags_default = 0;
adc_bitwidth_default = 8;
vref_volt_default = 0.4;

flag_mpr_totalpower_default = 0;
flag_mpr_cwjammer_default = 0;
flag_mpr_gpsglo_powerdelta_default = 0;
flag_mpr_meaniq_default = 0;
flag_mpr_highiq_default = 1;
flag_ref_tone_default = 0;
SigFreqKhz_rf_default = 1575.52e3;
SigLevelDbm_default = -120;
b13TxOn_default = 0;
skipNumCheck = 0;                                % skip the number of samples check in the cases when the bin file is not parsed properly
isTargetGen8B = 1;
%--------------------------------------------------------------------------
% Choose RF core for Gen8A targets. This variable is ignored for Gen8B targets.
% Use 'magellan' for RTR8600
% Use 'eureka' for WTR1605
% Use 'eura' for WTR2605
% Use 'euton' for WTR3925/WTR3905/WTR3605/WTR3205
%--------------------------------------------------------------------------
freqPpmLim = 15;                                                              % unit is ppm
numSigSamps = 1;                                                             % +/- numSigSamps is the number of frequency bins that are considered as covered by the CW tone;
% FFT window
win_type = 'hann'; % Choose 'hann' or 'rect'
% Maximum number of narrowband jammers to detect
max_num_jammers = 20;
% Narrowband jammer detector sensitivity (smaller is more sensitive)
det_sens = 2;
% Inline function that gives the center frequency of a GLO channel
glo_freq = @(ch) 1602e6 + ch*562.5e3;  % Hz
% Inline function that calculates RMS
rms = @(v) sqrt(mean(abs(v).^2));
% Input argument checks: 0 <= nargin <= 6;
narginchk(0,12)
% Handle the input parameters
if nargin < 1 || isempty(iq_file)
    iq_file = 'gnss_iq_collect.bin';
    flag_mpr_totalpower = flag_mpr_totalpower_default;
    flag_mpr_cwjammer = flag_mpr_cwjammer_default;
    flag_mpr_gpsglo_powerdelta = flag_mpr_gpsglo_powerdelta_default;
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    flag_ref_tone = flag_ref_tone_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 2 || isempty(save_results)
    save_results = false;
    flag_mpr_totalpower = flag_mpr_totalpower_default;
    flag_mpr_cwjammer = flag_mpr_cwjammer_default;
    flag_mpr_gpsglo_powerdelta = flag_mpr_gpsglo_powerdelta_default;
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    flag_ref_tone = flag_ref_tone_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 3
    widejam_band = [];
    flag_mpr_totalpower = flag_mpr_totalpower_default;
    flag_mpr_cwjammer = flag_mpr_cwjammer_default;
    flag_mpr_gpsglo_powerdelta = flag_mpr_gpsglo_powerdelta_default;
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    flag_ref_tone = flag_ref_tone_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 4
    flag_mpr_totalpower = flag_mpr_totalpower_default;
    flag_mpr_cwjammer = flag_mpr_cwjammer_default;
    flag_mpr_gpsglo_powerdelta = flag_mpr_gpsglo_powerdelta_default;
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    flag_ref_tone = flag_ref_tone_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 5
    flag_mpr_totalpower = flag_mpr_totalpower_default;
    flag_mpr_cwjammer = flag_mpr_cwjammer_default;
    flag_mpr_gpsglo_powerdelta = flag_mpr_gpsglo_powerdelta_default;
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 6
    flag_mpr_cwjammer = flag_mpr_cwjammer_default;
    flag_mpr_gpsglo_powerdelta = flag_mpr_gpsglo_powerdelta_default;
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 7
    flag_mpr_gpsglo_powerdelta = flag_mpr_gpsglo_powerdelta_default;
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 8
    flag_mpr_meaniq = flag_mpr_meaniq_default;
    flag_mpr_highiq = flag_mpr_highiq_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 9
    flag_mpr_highiq = flag_mpr_highiq_default;
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 10
    SigFreqKhz_RF = SigFreqKhz_rf_default;
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 11
    SigLevelDbm = SigLevelDbm_default;
    b13TxOn = b13TxOn_default;
elseif nargin < 12
    b13TxOn = b13TxOn_default;
elseif nargin == 12
    fprintf('All %d inputs are provided! \n',nargin)
else
    error(' Too many inputs are provided! This script requires %d number of inputs or less! \n.',nargin)
end
% Find out how many bin files the customer wants to process;
numOfBinFiles = numel(strfind(iq_file,'.bin'));
switch numOfBinFiles
    case 1
        % The legacy 1-bin file ADC IQ process
        gen8_adc_iq_proc_core(...
            iq_file,...                                                         % The ACD IQ collection bin file;
            save_results,...                                            % 1 : save the generated figure; 0: do not save the figure;
            widejam_band,...                                       % [lower_frequency_boud_in_Hz upper_frequency_bound_in_Hz] defines a frequency range where a wide band jammer is needed;
            flag_ref_tone,...                                           %  1: means there is reference tone; 0 no reference tone;
            flag_mpr_totalpower,...                          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the total power; 0 turn off;
            flag_mpr_cwjammer,...                           % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the CW tone jammer; 0 turn off;
            flag_mpr_gpsglo_powerdelta,...          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the GPS/GLO band noise power sepctrum density (PSD) comparison; 0 turn off;
            flag_mpr_meaniq,...                                  % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the mean IQ (DC) component; 0 turn off;
            flag_mpr_highiq,...                                    % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the high IQ (DC) component; 0 turn off;
            SigFreqKhz_RF,...                                      % Reference tone frequency (kHz) in RF;
            SigLevelDbm,...                                          % Reference tone power level in dBm;
            b13TxOn);
    case 2
        % 2-bin file ADC IQ process to calculate the noise floor increase
        % due to jammer
        idxOfDelimiter = strfind(iq_file, ',');
        % Number of files check;
        if numel(idxOfDelimiter) ~= 1
            error(' If 2 bin files are provided, the filenames must be separated by and only by the comma sign ! \n.');
        end
        % Reference tone check;
        if ~flag_ref_tone
            error(' For automated noise power spectrum density calculation, the reference tone must be provided in both bin files! \n.');
        end
        iqFile1 = iq_file(1:idxOfDelimiter-1);
        iqFile2 = iq_file(idxOfDelimiter+1:end);
        gpsBandTotalNoisedBmVect = [];
        gloBandTotalNoisedBmVect = [];
        bdsBandTotalNoisedBmVect = [];
        galBandTotalNoisedBmVect = [];
        fprintf('\r\n Now processing file %s \r\r', iqFile1);
        % Calculate IQ FILE 1
        [total_rms,...
            total_rms_isamp,...
            total_rms_qsamp,...
            noise_pdsdelta_gpsglo_dB, ...
            signal_power,...
            signal_freq,...
            mean_iq_amp_mv,...
            cdf_adc_amp,...
            adc_amp,...
            gpsBandTotalNoisedBm,...
            gloBandTotalNoisedBm,...
            bdsBandTotalNoisedBm,...
            galBandTotalNoisedBm...
            ]  = gen8_adc_iq_proc_core(...
            iqFile1,...                                                         % The ACD IQ collection bin file;
            save_results,...                                            % 1 : save the generated figure; 0: do not save the figure;
            widejam_band,...                                       % [lower_frequency_boud_in_Hz upper_frequency_bound_in_Hz] defines a frequency range where a wide band jammer is needed;
            flag_ref_tone,...                                           %  1: means there is reference tone; 0 no reference tone;
            flag_mpr_totalpower,...                          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the total power; 0 turn off;
            flag_mpr_cwjammer,...                           % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the CW tone jammer; 0 turn off;
            flag_mpr_gpsglo_powerdelta,...          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the GPS/GLO band noise power sepctrum density (PSD) comparison; 0 turn off;
            flag_mpr_meaniq,...                                  % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the mean IQ (DC) component; 0 turn off;
            flag_mpr_highiq,...                                    % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the high IQ (DC) component; 0 turn off;
            SigFreqKhz_RF,...                                      % Reference tone frequency (kHz) in RF;
            SigLevelDbm,...                                          % Reference tone power level in dBm;
            b13TxOn);                                                    % 1: means the ADC IQ is collected during the B13 Tx traffic; 0: means no B13 Tx traffic during IQ collection;
        % Add to the noise calculation vector;
        gpsBandTotalNoisedBmVect = [gpsBandTotalNoisedBmVect gpsBandTotalNoisedBm];
        gloBandTotalNoisedBmVect = [gloBandTotalNoisedBmVect gloBandTotalNoisedBm];
        bdsBandTotalNoisedBmVect = [bdsBandTotalNoisedBmVect bdsBandTotalNoisedBm];
        galBandTotalNoisedBmVect = [galBandTotalNoisedBmVect galBandTotalNoisedBm];
        fprintf('\r\n Now processing file %s \r\r', iqFile2);
        % Calculate IQ FILE 2
        [total_rms,...
            total_rms_isamp,...
            total_rms_qsamp,...
            noise_pdsdelta_gpsglo_dB, ...
            signal_power,...
            signal_freq,...
            mean_iq_amp_mv,...
            cdf_adc_amp,...
            adc_amp,...
            gpsBandTotalNoisedBm,...
            gloBandTotalNoisedBm,...
            bdsBandTotalNoisedBm,...
            galBandTotalNoisedBm...
            ]  = gen8_adc_iq_proc_core(...
            iqFile2,...                                                         % The ACD IQ collection bin file;
            save_results,...                                            % 1 : save the generated figure; 0: do not save the figure;
            widejam_band,...                                       % [lower_frequency_boud_in_Hz upper_frequency_bound_in_Hz] defines a frequency range where a wide band jammer is needed;
            flag_ref_tone,...                                           %  1: means there is reference tone; 0 no reference tone;
            flag_mpr_totalpower,...                          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the total power; 0 turn off;
            flag_mpr_cwjammer,...                           % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the CW tone jammer; 0 turn off;
            flag_mpr_gpsglo_powerdelta,...          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the GPS/GLO band noise power sepctrum density (PSD) comparison; 0 turn off;
            flag_mpr_meaniq,...                                  % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the mean IQ (DC) component; 0 turn off;
            flag_mpr_highiq,...                                    % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the high IQ (DC) component; 0 turn off;
            SigFreqKhz_RF,...                                      % Reference tone frequency (kHz) in RF;
            SigLevelDbm,...                                          % Reference tone power level in dBm;
            b13TxOn);                                                    % 1: means the ADC IQ is collected during the B13 Tx traffic; 0: means no B13 Tx traffic during IQ collection;
        % Add to the noise calculation vector;
        gpsBandTotalNoisedBmVect = [gpsBandTotalNoisedBmVect gpsBandTotalNoisedBm];
        gloBandTotalNoisedBmVect = [gloBandTotalNoisedBmVect gloBandTotalNoisedBm];
        bdsBandTotalNoisedBmVect = [bdsBandTotalNoisedBmVect bdsBandTotalNoisedBm];
        galBandTotalNoisedBmVect = [galBandTotalNoisedBmVect galBandTotalNoisedBm];
        % GNSS signal bandwidth
        gps_bandwidth_Hz =2 * 1023e3;
        bds_bandwidth_Hz = 2 * 2046e3;
        glo_bandwidth_Hz = 8334500;
        gal_bandwidth_Hz =  2 * 2046e3;
        % Calculate the Tx noise power increase
        TxNoisePwrDensitydBmHzGps = 10*log10(abs(10^(gpsBandTotalNoisedBmVect(2)/10) - 10^(gpsBandTotalNoisedBmVect(1)/10))) - 10*log10(gps_bandwidth_Hz);
        TxNoisePwrDensitydBmHzGlo = 10*log10(abs(10^(gloBandTotalNoisedBmVect(2)/10) - 10^(gloBandTotalNoisedBmVect(1)/10))) - 10*log10(glo_bandwidth_Hz);
        TxNoisePwrDensitydBmHzBds = 10*log10(abs(10^(bdsBandTotalNoisedBmVect(2)/10) - 10^(bdsBandTotalNoisedBmVect(1)/10))) - 10*log10(bds_bandwidth_Hz);
        TxNoisePwrDensitydBmHzGal = 10*log10(abs(10^(galBandTotalNoisedBmVect(2)/10) - 10^(galBandTotalNoisedBmVect(1)/10))) - 10*log10(gal_bandwidth_Hz);
        % Log writing
        OutputFolder = strcat(pwd, '\');
        % Generate the file name properly;
        diary_file = strcat(OutputFolder, 'Summary_NoisePSD.txt');
        if exist(diary_file,'file')
            delete(diary_file)
        end
        diary(diary_file)
        % Display the results
        fprintf('\r\n=========================\n')
        fprintf('NOISE POWER SPECTRUM DENSITY INCREASE\n')
        fprintf('=========================\r\n')
        fprintf('Noise power spectrum density increase in GPS band = %.2f dBm/Hz\n',TxNoisePwrDensitydBmHzGps)
        fprintf('Noise power spectrum density increase in GLO band = %.2f dBm/Hz\n',TxNoisePwrDensitydBmHzGlo)
        fprintf('Noise power spectrum density increase in BDS band = %.2f dBm/Hz\n',TxNoisePwrDensitydBmHzBds)
        fprintf('Noise power spectrum density increase in GAL band = %.2f dBm/Hz\n',TxNoisePwrDensitydBmHzGal)
        diary off
    otherwise
        error(' The number of bin files is not supported! This script only supports either 1 or 2 bin files! \n.');
end
    is_iq_swapped = bitand(adc_config_flags, 1);
%%
%====================================================================
%   THE CORE ADC IQ FUNCTION
%====================================================================
    function  [total_rms,...
            total_rms_isamp,...
            total_rms_qsamp,...
            noise_pdsdelta_gpsglo_dB, ...
            signal_power,...
            signal_freq,...
            mean_iq_amp_mv,...
            cdf_adc_amp,...
            adc_amp,...
            gpsBandTotalNoisedBm,...
            gloBandTotalNoisedBm,...
            bdsBandTotalNoisedBm,...
            galBandTotalNoisedBm...
            ]  = gen8_adc_iq_proc_core(...
            iq_file,...                                                         % The ACD IQ collection bin file;
            save_results,...                                            % 1 : save the generated figure; 0: do not save the figure;
            widejam_band,...                                       % [lower_frequency_boud_in_Hz upper_frequency_bound_in_Hz] defines a frequency range where a wide band jammer is needed;
            flag_ref_tone,...                                           %  1: means there is reference tone; 0 no reference tone;
            flag_mpr_totalpower,...                          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the total power; 0 turn off;
            flag_mpr_cwjammer,...                           % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the CW tone jammer; 0 turn off;
            flag_mpr_gpsglo_powerdelta,...          % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the GPS/GLO band noise power sepctrum density (PSD) comparison; 0 turn off;
            flag_mpr_meaniq,...                                  % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the mean IQ (DC) component; 0 turn off;
            flag_mpr_highiq,...                                    % 1: flip on the pass/fail test on the minimum performance recommendation (MPR) on the high IQ (DC) component; 0 turn off;
            SigFreqKhz_RF,...                                      % Reference tone frequency (kHz) in RF;
            SigLevelDbm,...                                          % Reference tone power level in dBm;
            b13TxOn)                                                    % 1: means the ADC IQ is collected during the B13 Tx traffic; 0: means no B13 Tx traffic during IQ collection;
        % Save command window output
        if save_results
            % Extract the IQ file name;
            [pathstr,filename] = fileparts(iq_file);
            % Generate the file name properly;
            diary_file = fullfile(pathstr,[filename '.txt']);
            if exist(diary_file,'file')
                delete(diary_file)
            end
            diary(diary_file)
        end
        %--------------------------------------------------------------------------
        %           READ IN AND CLEAN UP DATA FROM IQ COLLECT FILE
        %--------------------------------------------------------------------------
        % H/W config;
        C_NAVRF_ADC_6_TO_8_BIT = 0;   % This is a Gen8C feather feature where it maps the H/W 6-bit ADC to a 8-bit number;
        % GPS L1 center frequency; GAL E1 center frequency;
        fL1 = 1575.42e6;
        % BDS B1 center frequency;
        fB1 = 1561.098e6;
        % Open the data file
        fid = fopen(iq_file,'r');
        if fid == -1
            error('Unable to open file %s.',iq_file)
        end
        % Read packet version to determine the header information;
        packet_version = fread(fid,1,'int32');
        %----------------------------------------------------
        % Different packet version corresponds to different header information;
        %----------------------------------------------------
        is_withheaderhelp = 0;
        if packet_version <53
            % Define header size;
            header_sz = 14;
            % Read the rest of the header
            header = [packet_version; fread(fid,header_sz-1,'int32')];
            % Find the collect mode;
            collect_mode = header(5); % collect mode = 16 means it is an ADC IQ collection.
            % Find the number of collection;
            num_collect = header(9);
            if collect_mode ~= 16
                error('Incorrect collection mode. This is not ADC IQ data.')
            end
            if num_collect < 20
                fprintf('Warning: Number of collections = %d\n',num_collect)
            end
            % No XO compensation info in the header so cannot rely on the reference tone.
            flag_ref_tone = 0;
            % The default support for Gen 8 and Gen 8A family;
            rf_core = 'eureka';
            % ADC reference voltage;
            % eureka: 200 mv;
            vref = 0.2;  % Volts ;
            % ADC set point;
            adc_setpoint_mv = 9.1;
            % ADC bit width;
            adc_bitwidth = 6;
            fsample = 81.84e6;
            if isequal(rf_core,'magellan')
                fLO = 1588.71e6;
            elseif isequal(rf_core,'eureka')
                fLO = 1590e6;
            else
                error('Invalid RF core.')
            end
            is_iq_swapped = 0;
        elseif packet_version == 53
            is_withheaderhelp =1;
            % define header size;
            header_sz = 19;
            % Read the rest of the header
            header = [packet_version; fread(fid,header_sz-1,'int32')];
            % Find the collect mode;
            collect_mode = header(5); % collect mode = 16 means it is an ADC IQ collection.
            % Find the number of collection;
            num_collect = header(9);
            if collect_mode ~= 16
                error('Incorrect collection mode. This is not ADC IQ data.')
            end
            if num_collect < 20
                fprintf('Warning: Number of collections = %d\n',num_collect)
            end
            % RFIC core name;
            rf_core = 'eureka';
            if isTargetGen8B
                %--------------------------------------
                %   The Gen8B specific paramters:
                %   6-bit adc;
                %   200 mv Vref;
                %   IQ swapped = 0;
                %--------------------------------------
                % ADC bit width;
                adc_bitwidth = 6;
                % ADC reference voltage;
                % eureka: 200 mv;
                vref = 0.2;  % Volts ;
                % ADC set point;
                adc_setpoint_mv = 9.1;
                % Flag to indicate if IQ swap is enabled in ADC;
                is_iq_swapped = 0;
            else
                %--------------------------------------
                %   The Gen8C specific paramters:
                %   8-bit adc;
                %   400 mv Vref;
                %   IQ swapped = 1;
                %--------------------------------------
                % ADC bit width;
                adc_bitwidth = 8;
                % ADC reference voltage;
                vref = 0.4;  % Volts ;
                % ADC set point;
                adc_setpoint_mv = 12;
                % Flag to indicate if IQ swap is enabled in ADC;
                is_iq_swapped = 1;
            end
            % Read in the rest of the header information;
            fsample_nom = header(14);                        % Hz
            samp_mem_sz = header(15);                      % bytes
            xo_error_hwu = header(16);                        % 2^-20 ppm
            gps_if_freq = header(17);                              % Hz
            pga_gain = header(18);                                  % dB
            xo_error_ppm = xo_error_hwu*2^-20;
            fsample = fsample_nom*(1 + 1e-6*xo_error_ppm);
            fLO = (fL1 - gps_if_freq)*(1 + 1e-6*xo_error_ppm);
            fprintf('XO error    = %.2f ppm\n',xo_error_ppm)
            fprintf('Sample rate = %.0f Hz\n',fsample)
            fprintf('PGA gain    = %d dB\n\n',pga_gain)
        elseif packet_version >= 53
            is_withheaderhelp =1;
            % define header size;
            header_sz = 23;
            % Read the rest of the header
            header = [packet_version; fread(fid,header_sz-1,'int32')];
            % Find the collect mode;
            collect_mode = header(5); % collect mode = 16 means it is an ADC IQ collection.
            % Find the number of collection;
            num_collect = header(9);
            if collect_mode ~= 16
                error('Incorrect collection mode. This is not ADC IQ data.')
            end
            if num_collect < 20
                fprintf('Warning: Number of collections = %d\n',num_collect)
            end
            % This is Gen 8C flavor
            rf_core = 'euton';
            % Read in the rest of the header information;
            fsample_nom = header(14);                        % Hz
            samp_mem_sz = header(15);                      % bytes
            xo_error_hwu = header(16);                        % 2^-20 ppm
            gps_if_freq = header(17);                               % Hz
            pga_gain = header(18);                                   % dB
            adc_setpoint_mv = header(19)/10;          % mV (ADC set point;)
            vref = header(20)/1000;                                % Vref in volt;
            % Error handling for the vref information
            if abs(vref ) ~= 0.2 &&  abs(vref ) ~= 0.4
                fprintf('Warning:  Vref value in the header is incorrect: header value = %f V \n',vref)
                fprintf('Warning:  Vref value is overwritten by the default value 0.4 V \n')
                vref = vref_volt_default;
                skipNumCheck = 1;
            end
            adc_bitwidth = header(21);                           % ADC bit width
            % Error handling for the adc bit width information
            if adc_bitwidth ~= 6 &&  adc_bitwidth ~= 8
                fprintf('Warning:  Bit width in the header is incorrect: header value = %f bit \n',adc_bitwidth)
                fprintf('Warning:  Bit width value is overwritten by the default value: 8 \n')
                adc_bitwidth = adc_bitwidth_default;
                skipNumCheck = 1;
            end
            
            adc_config_flags = header(22);
            % Error handling for the adc bit width information
            if abs(adc_config_flags) > 2
                fprintf('Warning:  ADC config flag in the header is incorrect: header value = %f  \n',adc_config_flags)
                fprintf('Warning:  ADC config flag value is overwritten by the default value: 1 \n')
                adc_config_flags = adc_config_flags_default;
                skipNumCheck = 1;
            elseif adc_config_flags == 2
                C_NAVRF_ADC_6_TO_8_BIT = 1;  
                fprintf('Note:  The Nav Core 6-8 bit conversion is ON! \n')
            end
            % Flag to indicate if IQ swap is enabled in ADC;
            is_iq_swapped = bitand(adc_config_flags, 1);
            xo_error_ppm = xo_error_hwu*2^-20;
            fsample = fsample_nom*(1 + 1e-6*xo_error_ppm);
            fLO = (fL1 - gps_if_freq)*(1 + 1e-6*xo_error_ppm);
            fprintf('XO error    = %.2f ppm\n',xo_error_ppm)
            fprintf('Sample rate = %.0f Hz\n',fsample)
            fprintf('PGA gain    = %d dB\n\n',pga_gain)
            fprintf('ADC set point = %f mV\n', adc_setpoint_mv)
            fprintf('ADC vref = %f V\n', vref)
            fprintf('ADC bit width = %d bits\n', adc_bitwidth)
            fprintf('ADC config flags = %d\n\n', adc_config_flags)
        else
            % Undefined flavor;
            error('Unsupported log packet format! Version # %d', packet_version)
        end
        % Calculate 1 PPM value;
        LO_freq_oneppm_Hz = fLO * 1e-6;
        OnePpmKHz = LO_freq_oneppm_Hz / 1e3;
        % Read the samples
        [dat,cnt] = fread(fid,inf,'uint8');
        fclose(fid);
        % Error handling
        if mod(cnt,2) ~= 0
            error('Unequal number of I and Q samples.')
        end
        if (packet_version > 52) && cnt ~= num_collect*samp_mem_sz && ~skipNumCheck
            error('Number of collected samples (%d) does not match header (%d).',cnt/2,num_collect*samp_mem_sz/2)
        end
        if skipNumCheck
            num_samp = num_collect*samp_mem_sz/2;
            numZerosToPad = num_collect*samp_mem_sz - cnt;
            cnt = num_collect*samp_mem_sz;
        else
            num_samp = cnt/2;
        end
        if mod(num_samp,num_collect) ~= 0 &&~skipNumCheck
            error('Total number of samples is not divisible by the number of collections.')
        end
        
        %--------------------------------------------------------------------------
        % The pass/fail criterion of the key performance indicator;
        %--------------------------------------------------------------------------
        totalVolLim_mv        =  3 * sqrt(2) *...
            adc_setpoint_mv;                                             % mV;   the total voltage mask is set as 3 times higher than the setpoint voltage
        totalVolLim_B13_mv = 5 * sqrt(2) *...
            adc_setpoint_mv;                                             % mV;   the total voltage mask is set as 5 times higher than the setpoint voltage where the B13 tx indicator is on;
        nb_bwlimit_KHz = 10;                                       % KHz; BW < 10 KHz spur is deemed as narrowband; otherwise it is wideband
        nbjammerLim_dBm =  -90;                            % dBm; the narrowband pass/fail applies to all bands; definition of narrowband is BW < 10 KHz; per discussion with RF system team on 08-23-2013
        wbjammerLim_dBm = -85;                            % dBm; the wideband pass/fail applies to all bands;  definition of wideband is BW >= 10 KHz; per discussion with RF system team on 08-23-2013
        gpsglo_noisedensitiydelta_dB = 10;          % dB;     the GPS/GLO band noise density delta - 10 dB per discussion with RF system team on 08-23-2013
        mean_IQ_voltage_mv  = vref * 0.1;             % mV;   the mean value of I/Q voltages => DC offset if > xx% (e.g. 10%) of Vref
        target_percentage = 80;                                 % percentage of Vref for the definition of high-valued IQ
        percentage_high_IQ_tile = 20;                     % percentage of near-Vref I/Q voltage absolute values (vref * target_percentage%)

        % dat = [Q2 I2 Q1 I1 Q4 I4 Q3 I3 ...]. Callisto HW writes the samples in
        % big-endian byte order, so the first word in sample memory is [I1 Q1 I2
        % Q2], the second word is [I3 Q3 I4 Q4], etc.
        
        % Convert to doubles in the range [-1 1]
        % Eureka: 6-bit ADC: Dat is 6-bit [00XXXXXX - byte] unsigned integer now normalized to the range of [-1 1]
        % Eurton: 8-bit ADC: Dat is 8-bit [XXXXXXXX - byte] unsigned integer now
        % normalized to the range of [-1 1] TBD
        % Calculate the full range based on the given ADC bit width info
        if (adc_bitwidth == 6) || (C_NAVRF_ADC_6_TO_8_BIT)
            full_range = 2^6 - 1;
        elseif adc_bitwidth == 8 
            full_range = 2^(adc_bitwidth) - 1;
        else 
            fprintf('Warning: the ADC bit width information is incorrect! The ADC bit width logged in the header is : %d\n', adc_bitwidth);
            full_range = 2^(adc_bitwidth) - 1; % Set the full range as default;
        end
        % Convert binary offset to 2's complementary
        if adc_bitwidth == 6 || adc_bitwidth == 8
            dat = dat - 2^(adc_bitwidth - 1);
        else
            fprintf('Warning: the ADC bit width information is incorrect! The ADC bit width logged in the header is : %d\n', adc_bitwidth);
            dat = dat - 2^(adc_bitwidth - 1);
        end
        % Scale the range of dat to [-1, +1]
        dat = dat/full_range; 
        %         dat = 2*dat/full_range - 1;
        if skipNumCheck
            if numZerosToPad > 0
                dat(end+1:end+numZerosToPad)  = 0;
            end
        end
        %%
        % Error handling
        if max(abs(dat)) > 1
            error('Invalid data format. Two MSBs of every byte should be zero.')
        end
        % Deinterleave the IQ samples
        perm = reshape(1:cnt,4,[]);
        perm = perm(4:-1:1,:);
        % Reorder data to retrieve IQ samples sequentially
        dat = dat(perm(:));
        % Arrange IQ data based on if the I / Q swap is enabled in ADC proc;
        if is_iq_swapped
            % Gather the I/Q samples from data
            qsamples = dat(1:2:end);
            isamples = dat(2:2:end);
        else
            % Gather the I/Q samples from data
            isamples = dat(1:2:end);
            qsamples = dat(2:2:end);
        end
        % Bin the data
        bin_edges = linspace(-1-1/full_range,1+1/full_range,full_range + 2);
        % bin_edges = linspace(-1-1/half_range,1+1/half_range,64 + 2);
        ibins = histc(isamples,bin_edges)/num_samp;
        qbins = histc(qsamples,bin_edges)/num_samp;
        %----------------------------------------
        % Graph histogram
        %----------------------------------------
        figure
        fig.hist = gcf;
        h(1) = subplot(2,1,1);
        bin_edges = 1e3*vref*bin_edges;
        bar(bin_edges,ibins,'histc')
        ylim = max([ibins; qbins]);
        ylim = ceil(20*ylim)/20;
        ax = [bin_edges(1) bin_edges(end) 0 ylim];
        axis(ax)
        xlabel('I samples (mV)')
        text(0.95*ax(1),0.85*ax(4),sprintf('mean = %.2f mV\nrms = %.2f mV',1e3*vref*mean(isamples),1e3*vref*rms(isamples)))
        ht = title(sprintf('Histogram: %s',iq_file));
        set(ht,'Interpreter','none')
        h(2) = subplot(2,1,2);
        bar(bin_edges,qbins,'histc')
        axis(ax)
        xlabel('Q samples (mV)')
        text(0.95*ax(1),0.85*ax(4),sprintf('mean = %.2f mV\nrms = %.2f mV',1e3*vref*mean(qsamples),1e3*vref*rms(qsamples)))
        linkaxes(h,'x')
        % Percentage of saturation;
        iSat = sum(abs(isamples) == 1);
        qSat = sum(abs(qsamples) == 1);
        perSat = (iSat + qSat)/(2 * numel(isamples)) * 100;
        perSatI = iSat/numel(isamples)* 100;
        perSatQ = qSat/numel(qsamples)* 100;
        % zoom xon
        %----------------------------------------
        % Graph samples in time domain
        %----------------------------------------
        figure
        fig.time = gcf;
        t = 1/fsample*(0:num_samp-1);
        tcoh = 1/fsample*num_samp/num_collect;
        ylim = 1e3*vref;
        for i = 2:num_collect
            line(1e3*(i-1)*tcoh*[1 1],[-ylim ylim],'Color','y')
        end
        hold on
        hl = plot(1e3*t,1e3*vref*isamples,1e3*t,1e3*vref*qsamples);
        hold off
        axis([1e3*t(1) 1e3*t(end) -ylim ylim])
        xlabel('Time (ms)')
        ylabel('Sample value (mV)')
        ht = title(sprintf('Time domain: %s',iq_file));
        set(ht,'Interpreter','none')
        legend(hl,'I','Q')
        % grid on, zoom xon
        
        % Calculate total RMS voltage
        total_rms = vref*sqrt(mean(isamples.^2 + qsamples.^2));
        % Calculate the I RMS voltage
        total_rms_isamp = vref*sqrt(mean(isamples.^2));
        % Calculate the Q RMS voltage
        total_rms_qsamp = vref*sqrt(mean(qsamples.^2));
        
        % Define the template of displaying the voltage
        fmt = '%15s = %8.3f mV\n';
        fmt_dBm = '%15s = %8.3f dBm\n';
        %---------------------------------------------
        % Compute power spectrum. Note: Applying a window function can reduce the
        % spectral leakage of a CW tone, but it changes the total RMS voltage.
        %---------------------------------------------
        nfft = num_samp/num_collect;
        isamples = reshape(isamples,nfft,num_collect);
        qsamples = reshape(qsamples,nfft,num_collect);
        linFftData = zeros(nfft,1);
        if isequal(win_type,'hann')
            win = 0.5*(1 - cos(2*pi/(nfft-1)*(0:nfft-1)'));
        elseif isequal(win_type,'rect')
            win = ones(nfft,1);
        else
            error('Unrecognized window type.')
        end
        % Calculate the averaged power sepctrum;
        for i = 1:num_collect
            cmplx = complex(isamples(:,i),qsamples(:,i));
            linFftData = linFftData + abs(fft(win.*cmplx)).^2;
        end
        % linFftData is the averaged power of the collected IQ in frequency domain;
        linFftData = linFftData/num_collect;
        
        % Scale to units of V^2/bin
        %  For length N input vector x, the DFT is a length N vector X,  with elements
        %                                       N
        %       X(k) = fft(x) =       sum  x(n)*exp(-j*2*pi*(k-1)*(n-1)/N), 1 <= k <= N.
        %                                     n=1
        %       X(k) is the total amplitude of  x(n) modulated to carrier exp(-j*2*pi*(k-1)*/N);
        %       Thus, X(k)/N is the averaged amplitude of x(n) modulated to kth carrier
        %       Thus, (X(k)/N)^2 is the averaged power of x(n) modulated to kth carrier
        %       In fact, this follows the Parseval's theorem:
        %                     N                                         N
        %       1/N x  sum |x(n)|^2 = 1/(N^2) x sum |X(k)|^2
        %                  n= 1                                     n= 1
        
        linFftData = linFftData...                                        % The unit of y is still the sume of N discrete values quantized to [-1 +1] at this step
            *(vref/nfft)^2;                       % This unit is now converted to actual power (volt)^2;
        
        % Define frequency vector
        fftFreqPts = fsample/nfft*(0:nfft-1)';
        ineg = find(fftFreqPts >= fsample/2);
        fftFreqPts(ineg) = fftFreqPts(ineg) - fsample;
        BinSize = fsample / nfft;
        % Reorder the spectrum to put DC in the middle
        fftFreqPts = fftshift(fftFreqPts);
        linFftData = fftshift(linFftData);
        
        % Calculate total RMS voltage
        if ~isequal(win_type,'rect')
            % scale the power spectrum density of y to make equal the
            % (1) time domain estimated power ;
            % (2) frequency domain estimated power;
            linFftData = total_rms^2/sum(linFftData)*linFftData;
        end
        %---------------------------------------------
        % Noise floor calculation
        %---------------------------------------------
        % Apply a median filter to estimate noise floor
        num_pts = 401;
        % Equally seperate the frequency space (BW = 81.84 MHz) into "num_pts - 1"
        % spaces
        noise_flr_freq = linspace(-fsample/2,fsample/2,num_pts);
        
        % calculate the frequency range of the subbands;
        df = mean(diff(noise_flr_freq));
        % initialize the noise floor vector;
        noise_flr = zeros(1,num_pts);
        % calculate the noise floor for each subbands using median filter
        numNoisebands = 3;
        for i = 1:num_pts
            yi = linFftData(noise_flr_freq(i)-numNoisebands*df < fftFreqPts & fftFreqPts < noise_flr_freq(i)+numNoisebands*df);
            noise_flr(i) = median(yi);
        end
        % Linear interp on log scale to propogate the noise floor estimation over
        % the entire 81.84 MHz bandwidth
        noise_flr = 10.^interp1(noise_flr_freq,log10(noise_flr),fftFreqPts,'linear');
        % Median to mean adjustment
        med_to_mean_dB = 10/(3*log(10)*num_collect);
        noise_flr = 10^(med_to_mean_dB/10)*noise_flr;
        % Calculate RMS noise voltage
        % calculate the left & right edges of the GPS/GLO/BDS/GAL bands
        gps_band = fL1 - fLO + 1023e3*[-1 1];
        bds_band = fB1 - fLO + 2046e3*[-1 1];
        glo_band = glo_freq([-7 6]) - fLO + 511e3*[-1 1];
        gal_band =  fL1 - fLO + 2046e3*[-1 1];
        rbw = fsample/nfft;                                         % frequency bin size in the DFT
        peak_noise_tot = sqrt(max(noise_flr)/rbw); % this is the noise DENSITY;
        % Calculate the total noise voltage and peak noise voltage within GPS band;
        noise_gps = sqrt(sum(noise_flr(gps_band(1) <= fftFreqPts & fftFreqPts <= gps_band(2))));
        peak_noise_gps = sqrt(max(noise_flr(gps_band(1) <= fftFreqPts & fftFreqPts <= gps_band(2)))/rbw);
        average_noise_gps = sqrt(mean(noise_flr(gps_band(1) <= fftFreqPts & fftFreqPts <= gps_band(2)))/rbw);
        % Calculate the total noise voltage and peak noise voltage within GLO band;
        noise_glo = sqrt(sum(noise_flr(glo_band(1) <= fftFreqPts & fftFreqPts <= glo_band(2))));
        peak_noise_glo = sqrt(max(noise_flr(glo_band(1) <= fftFreqPts & fftFreqPts <= glo_band(2)))/rbw);
        average_noise_glo = sqrt(mean(noise_flr(glo_band(1) <= fftFreqPts & fftFreqPts <= glo_band(2)))/rbw);
        % Calculate the total noise voltage and peak noise voltage within BDS band;
        noise_bds = sqrt(sum(noise_flr(bds_band(1) <= fftFreqPts & fftFreqPts <= bds_band(2))));
        peak_noise_bds = sqrt(max(noise_flr(bds_band(1) <= fftFreqPts & fftFreqPts <= bds_band(2)))/rbw);
        average_noise_bds = sqrt(mean(noise_flr(bds_band(1) <= fftFreqPts & fftFreqPts <= bds_band(2)))/rbw);
        % Calculate the total noise voltage and peak noise voltage within GAL band;
        noise_gal = sqrt(sum(noise_flr(gal_band(1) <= fftFreqPts & fftFreqPts <= gal_band(2))));
        peak_noise_gal = sqrt(max(noise_flr(gal_band(1) <= fftFreqPts & fftFreqPts <= gal_band(2)))/rbw);
        average_noise_gal = sqrt(mean(noise_flr(gal_band(1) <= fftFreqPts & fftFreqPts <= gal_band(2)))/rbw);
        %---------------------------------------------
        % Wideband jammer calculation
        %---------------------------------------------
        % Integrate wideband jammer power
        if ~isempty(widejam_band)
            if isequal(widejam_band,'b13 all rb')
                widejam_band = [1555e6 1573e6] - fLO;
            elseif isequal(widejam_band,'b13 rb49')
                widejam_band = [1572.64e6 1573e6] - fLO;
            elseif isequal(widejam_band,'b13 rb46')
                widejam_band = [1571e6 1572.5e6] - fLO;
            elseif isequal(widejam_band,'b13 rb45')
                widejam_band = [1571.2e6 1571.56e6] - fLO;
            elseif isequal(widejam_band,'b14 all rb')
                widejam_band = [1577e6 1595e6] - fLO;
            elseif isequal(widejam_band,'b14 rb0')
                widejam_band = [1577e6 1577.36e6] - fLO;
            elseif isequal(widejam_band,'b27 all rb')
                widejam_band = [1615e6 1633e6] - fLO;
            elseif isequal(widejam_band,'b26 all rb')
                widejam_band = [1629.5e6 1656.5e6] - fLO;
            elseif isequal(widejam_band,'b18 all rb')
                widejam_band = [1631.5e6 1658.5e6] - fLO;
            elseif ischar(widejam_band)
                error('Unrecognized wideband jammer.')
            end
            widejam_band(2) = min(max(fftFreqPts),  widejam_band(2));
            widejam_band(1) = max(min(fftFreqPts),  widejam_band(1));
            jammer_pow = sum(linFftData(widejam_band(1) <= fftFreqPts & fftFreqPts <= widejam_band(2)));
        end
        %---------------------------------------------
        % Narrowband jammer calculation
        %---------------------------------------------
        % Integrate the power in narrowband jammers
        y_notched = linFftData;
        signal_power = [];
        signal_freq = [];
        signal_bw = [];
        % Run jammer detection in the whole band excepting wideband jammer
        if ~isempty(widejam_band)
            y_notched(widejam_band(1) <= fftFreqPts & fftFreqPts <= widejam_band(2)) = 0;
        end
        % Run jammer once;
        run_jammer_detector
        % Repeat jammer detection in signal passbands only
        y_notched(~((gps_band(1) <= fftFreqPts & fftFreqPts <= gps_band(2)) | ...
            (glo_band(1) <= fftFreqPts & fftFreqPts <= glo_band(2)) | ...
            (bds_band(1) <= fftFreqPts & fftFreqPts <= bds_band(2)))) = 0;
        % Run jammer twice;
        run_jammer_detector
        [not_used,psort] = sort(signal_power,2,'descend');
        [not_used,fsort] = sort(signal_freq,2,'ascend');
        
        % Optional: If reference tone is provided; the dBm values of spurs can be
        % provide;
        % fprintf('The ADC saturation percentage is %2.2f %%. \n', perSat);
        % fprintf('\n');
        if flag_ref_tone
            % Convert RF frequency to IF frequency;
            SigFreqKhz = SigFreqKhz_RF - fLO/1e3;
            fprintf('A reference tone is place at %4.4f KHz at power level %4.0f dBm. \n', SigFreqKhz, SigLevelDbm);
            % Locate the signal using input signal frequency
            SigUncBins = ceil(max(7, 2 * freqPpmLim) * OnePpmKHz * 1e3 / BinSize);
            expectedSigIdx = find(abs(fftFreqPts-SigFreqKhz*1e3)==min(abs(fftFreqPts-SigFreqKhz*1e3)));
            [no_use SigPkIdx] = max(linFftData(expectedSigIdx + [-SigUncBins:SigUncBins]));
            SigPkIdx = expectedSigIdx - SigUncBins + SigPkIdx - 1;
            % Get the estimated mv^2 => dBm mapping
            sigPower = sum(linFftData( SigPkIdx + [-numSigSamps:numSigSamps]) - noise_flr( SigPkIdx + [-numSigSamps:numSigSamps]) );
            % Find the "weighted average" signal position
            SigPos_Hz = sum((linFftData( SigPkIdx + [-numSigSamps:numSigSamps] ))...
                .* fftFreqPts(SigPkIdx + [-numSigSamps:numSigSamps]))/...
                sum(linFftData( SigPkIdx + [-numSigSamps:numSigSamps] ));
            inv_R = 10^(SigLevelDbm/10) ./ sigPower;   % Unit is mw/V^2
            % Display the wideband jammer power;
            if ~isempty(widejam_band)
                fprintf('%15s = %8.3f dBm in band [%.4f %.4f] MHz\n','Wideband jammer', 10*log10(jammer_pow * inv_R),1e-6*widejam_band(1),1e-6*widejam_band(2))
            end
            % Display the power distribution;
            fprintf(fmt_dBm,'Total (I+Q)',10*log10(total_rms^2 * inv_R))
            fprintf(fmt_dBm,'Total (I)',10*log10(total_rms_isamp^2 * inv_R))
            fprintf(fmt_dBm,'Total (Q)',10*log10(total_rms_qsamp^2 * inv_R))
            fprintf('*** Power estimates below are for I + jQ ***\n')
            fprintf('Power breakdown:\n')
            % Calculate the total in-gps-band noise;
            gpsBandTotalNoisedBm = 10*log10(noise_gps^2 * inv_R);
            % Calculate the total in-glo-band noise;
            gloBandTotalNoisedBm = 10*log10(noise_glo^2 * inv_R);
            % Calculate the total in-glo-band noise;
            bdsBandTotalNoisedBm = 10*log10(noise_bds^2 * inv_R);
            % Calculate the total in-glo-band noise;
            galBandTotalNoisedBm = 10*log10(noise_gal^2 * inv_R);
            
            fprintf(fmt_dBm,'GPS noise', 10*log10(noise_gps^2 * inv_R))
            fprintf(fmt_dBm,'GLONASS noise',10*log10(noise_glo^2 * inv_R))
            fprintf(fmt_dBm,'BDS noise', 10*log10(noise_bds^2 * inv_R))
            fprintf(fmt_dBm,'GAL noise', 10*log10(noise_gal^2 * inv_R))
            fprintf(fmt_dBm,'Spurious',10*log10(sum(signal_power) * inv_R))
            
            % Report peak noise voltage density
            fprintf('\nPeak noise power density:\n')
            fmt_dBm = '%15s = %8.3f dBm/Hz\n';
            fprintf(fmt_dBm,'Overall', 10*log10(peak_noise_tot^2 * inv_R))
            fprintf(fmt_dBm,'In GPS band',10*log10(peak_noise_gps^2 * inv_R))
            fprintf(fmt_dBm,'In GLONASS band',10*log10(peak_noise_glo^2 * inv_R))
            fprintf(fmt_dBm,'In BDS band',10*log10(peak_noise_bds^2*inv_R))
            fprintf(fmt_dBm,'In GAL band',10*log10(peak_noise_gal^2*inv_R))
            
            % Print out spur table
            if ~isempty(signal_power)
                fprintf('\nSpur table:\n')
                fprintf('%8s \t\t %10s \t %10s \n ','dBm','center frequency (MHz)', 'bandwidth (KHz)')
            end
            for i = 1:length(signal_power)
                fprintf('%8.3f \t\t %10.4f \t\t %10.4f \n',10*log10(signal_power(psort(i)) * inv_R),1e-6*signal_freq(psort(i)), 1e-3*signal_bw(psort(i)))
            end
        else
            if ~isempty(widejam_band)
                fprintf('%15s = %8.3f mV in band [%.4f %.4f] MHz\n','Wideband jammer',1e3*sqrt(jammer_pow),1e-6*widejam_band(1),1e-6*widejam_band(2))
            end
            fprintf(fmt,'Total (I + Q)',1e3*total_rms)
            fprintf(fmt,'Total (I)',1e3*total_rms_isamp)
            fprintf(fmt,'Total (Q)',1e3*total_rms_qsamp)
            fprintf('*** Voltages below are for I + jQ ***\n')
            fprintf('Voltage breakdown:\n')
            % Display the results of noise and spurious
            fprintf(fmt,'GPS noise',1e3*noise_gps)
            fprintf(fmt,'GLONASS noise',1e3*noise_glo)
            fprintf(fmt,'BDS noise', 1e3*noise_bds)
            fprintf(fmt,'GAL noise', 1e3*noise_gal)
            fprintf(fmt,'Spurious',1e3*sqrt(sum(signal_power)))
            
            % Report peak noise voltage density
            fprintf('\nPeak noise voltage density:\n')
            fmt = '%15s = %8.3f uV/sqrtHz\n';
            fprintf(fmt,'Overall',1e6*peak_noise_tot)
            fprintf(fmt,'In GPS band',1e6*peak_noise_gps)
            fprintf(fmt,'In GLONASS band',1e6*peak_noise_glo)
            fprintf(fmt,'In BDS band',1e6*peak_noise_bds)
            fprintf(fmt,'In GAL band',1e6*peak_noise_gal)
            % Print out spur table
            if ~isempty(signal_power)
                fprintf('\nSpur table:\n')
                fprintf('%8s \t\t %10s \t %10s \n ','mV','center frequency (MHz)', 'bandwidth (KHz)')
            end
            for i = 1:length(signal_power)
                fprintf('%8.3f \t\t %10.4f \t\t %10.4f \n',1e3*sqrt(signal_power(psort(i))),1e-6*signal_freq(psort(i)), 1e-3*signal_bw(psort(i)))
            end
        end
        %%
        %---------------------------------------------
        % Check Pass and Fail
        %---------------------------------------------
        TestRes = {'FAIL','PASS'};
        if 0
            fprintf('----------------------------------------------\n');
            fprintf('	Tentative Pass/Fail mask examination      \n');
            fprintf('----------------------------------------------\n');
        end
        % 1. Check if the total voltage check is enabled
        % non-B13 case;
        if flag_mpr_totalpower && ~b13TxOn
            fprintf('\n 1. The total voltage check (no B13 Tx traffic)\n');
            % cprintf('_red', '\n 1. The total voltage check (no B13 Tx traffic)\n');
            if 1e3 * total_rms > totalVolLim_mv
                fprintf('The total voltage : %2.2f mv\n',1e3 * total_rms);
                fprintf('The total voltage limit : %2.2f mv\n', totalVolLim_mv);
                fprintf('%10s: the total voltage is larger than the limit! \n', TestRes{1});
            else
                fprintf('The total voltage : %2.2f mv\n',1e3 * total_rms);
                fprintf('The total voltage limit : %2.2f mv\n', totalVolLim_mv);
                fprintf('%10s: the total voltage is smaller than the limit! \n', TestRes{2});
            end
        end
        % B13 case;
        if flag_mpr_totalpower && b13TxOn
            fprintf('\n 1. The total voltage check (coexist with B13 Tx traffic)\n');
            %     fprintf('\n 1. The total voltage check\n');
            %     cprintf('_red', '\n 1. The total voltage check (coexist with B13 Tx traffic)\n');
            if 1e3 * total_rms > totalVolLim_B13_mv
                fprintf('The total voltage : %2.2f mv\n',1e3 * total_rms);
                fprintf('The total voltage limit : %2.2f mv\n', totalVolLim_B13_mv);
                fprintf('%10s: the total voltage is larger than the limit! \n', TestRes{1});
            else
                fprintf('The total voltage : %2.2f mv\n',1e3 * total_rms);
                fprintf('The total voltage limit : %2.2f mv\n', totalVolLim_B13_mv);
                fprintf('%10s: the total voltage is smaller than the limit! \n', TestRes{2});
            end
        end
        %2. Check if the GPS/GLO noise PSD delta check is enabled;
        noise_pdsdelta_gpsglo_dB = 20*log10(abs(average_noise_gps/average_noise_glo));
        if flag_mpr_gpsglo_powerdelta
            noise_pdsdelta_gpsglo_dB = 20*log10(abs(average_noise_gps/average_noise_glo));
            fprintf('\n 2. GPS/GLO gain delta check \n');
            %cprintf('_Magenta', '\n 2. GPS/GLO noise PSD check \n')
            if abs(noise_pdsdelta_gpsglo_dB) > gpsglo_noisedensitiydelta_dB
                fprintf('The GPS/GLO band absolute  average gain delta : %2.2f dB\n',noise_pdsdelta_gpsglo_dB);
                fprintf('The GPS/GLO band absolute average gain delta limit (delta within) : %2.2f dB \n', gpsglo_noisedensitiydelta_dB);
                fprintf('%10s: the GPS/GLO absolute  average gain delta is larger than the limit! \n', TestRes{1});
            else
                fprintf('The GPS/GLO band absolute average gain delta : %2.2f dB\n',noise_pdsdelta_gpsglo_dB);
                fprintf('The GPS/GLO band absolute average gain delta limit (delta within) : %2.2f dB \n', gpsglo_noisedensitiydelta_dB);
                fprintf('%10s: the GPS/GLO  average gain delta is smaller than the limit! \n', TestRes{2});
            end
        end
        %3. Check if the CW tone jammer check is enabled;
        % note that this check is only enabled when there is a reference tone
        if flag_mpr_cwjammer
            fprintf('\n 3. Spur table (with spur limit check) \n')
            %cprintf('_cyan', '\n 3. Spur table (with spur limit check) \n')
            
            if flag_ref_tone
                % Convert to dBm
                signal_power = 10*log10(signal_power * inv_R);
                % Print out spur table
                if ~isempty(signal_power)
                    fprintf('%8s \t %10s \t %10s \t %15s \t %10s\n','dBm','center frequency (MHz)', 'bandwidth (KHz)', 'Jammer limit (dBm)', 'Pass/Fail')
                else
                    fprintf('\nNo spur above noise floor!\n')
                end
                % Exclude the DC jammer;
                for i = 1:length(signal_power)
                    if abs(signal_freq(psort(i)) ) > 5 * LO_freq_oneppm_Hz
                        % chech if the jammer is wideband or narrowband;
                        if signal_bw(psort(i)) > nb_bwlimit_KHz * 1e3
                            jammerLim_dBm = wbjammerLim_dBm;
                        else
                            jammerLim_dBm = nbjammerLim_dBm;
                        end
                        if signal_power(psort(i)) > jammerLim_dBm
                            fprintf('%8.3f  \t %10.4f \t\t %10.4f \t\t %10.4f \t\t %10s \n',signal_power(psort(i)),1e-6*signal_freq(psort(i)), 1e-3*signal_bw(psort(i)), jammerLim_dBm, TestRes{1});
                        else
                            fprintf('%8.3f  \t %10.4f \t\t  %10.4f \t\t %10.4f \t\t %10s \n',signal_power(psort(i)),1e-6*signal_freq(psort(i)), 1e-3*signal_bw(psort(i)), jammerLim_dBm, TestRes{2});
                        end
                    else
                        fprintf('%8.3f  \t %10.4f \t\t  %10.4f \t\t %10s \t\t %10s \n',signal_power(psort(i)),1e-6*signal_freq(psort(i)), 1e-3*signal_bw(psort(i)), 'N/A', 'N/A (DC SPUR)');
                    end
                end
            else
                fprintf('Spur pass/fail test is only valid when a reference tone is provided! \n')
            end
        end
        % 4. Mean IQ sample check;
        isamples_vector = reshape(isamples, 1, []);     % I component samples
        qsamples_vector = reshape(qsamples, 1, []);    % Q component samples;
        cmp_iq_vector = complex(isamples_vector, qsamples_vector);
        mean_iq_amp_mv = 0;
        if flag_mpr_meaniq
            mean_iq_amp_mv = 1e3 * vref * mean(cmp_iq_vector);
            % Process I/Q separately;
            mean_iq_amp_mv = abs(0.5 * real(mean_iq_amp_mv) + 0.5 * imag(mean_iq_amp_mv));
            fprintf('\n 4. Mean ADC amp value check \n')
            %cprintf('_green', '\n 4. Mean ADC amp value check \n')
            if mean_iq_amp_mv > 1e3 * mean_IQ_voltage_mv
                fprintf('The measured mean absolute ADC amp: %2.2f mv\n',mean_iq_amp_mv);
                fprintf('The mean absolute  ADC amp limit (Vref x 10%%): %2.2f mv \n', 1e3 * mean_IQ_voltage_mv);
                fprintf('%10s: the measured ADC amp is larger than the limit! \n', TestRes{1});
            else
                fprintf('The measured mean absolute ADC amp: %2.2f mv\n',mean_iq_amp_mv);
                fprintf('The mean absolute ADC amp limit (Vref x 10%%): %2.2f mv \n', 1e3 * mean_IQ_voltage_mv);
                fprintf('%10s: the measured ADC amp is smaller than the limit! \n', TestRes{2});
            end
        end
        % 5. High IQ sample check;
        if flag_mpr_highiq
            % process the I/Q separately;
            iq_abs_volt_vector = abs([isamples_vector qsamples_vector]);
            [cdf_adc_amp, adc_amp] = func_cdf(iq_abs_volt_vector);
            [cdf_i_amp, i_amp] = func_cdf(isamples_vector);
            [cdf_q_amp, q_amp] = func_cdf(qsamples_vector);
            if 0
                %----------------------------------------
                % plot the CDF curve;
                %----------------------------------------
                figure
                fig.cdf = gcf;
                plot(1e3 * vref .* adc_amp, cdf_adc_amp, 'LineWidth', 2);
                ht = title(sprintf('Absolute ADC amp CDF: %s',iq_file));
                set(ht,'Interpreter','none')
                grid on
                xlabel('Absolute voltage (mV) at ADC quantizer');
                ylabel('Cumulative distribution function');
                ax = axis;
                h = text(ax(2) * 0.6,ax(4)-0.75,sprintf(' mean absolute ADC amp = %.1f mV\n min absolute ADC amp = %.1f mV\n max absolute ADC amp = %.1f mV\n',1e3 *vref .* mean(adc_amp),...
                    1e3 *vref .* min(adc_amp), 1e3 * vref .* max(adc_amp)));
                set(h,'FontSize',8,'VerticalAlignment','top')
            end
            %----------------------------------------
            % plot the CDF of i/q samples;
            %----------------------------------------
            figure
            fig.cdfiq = gcf;
            plot(1e3 * vref .* i_amp, cdf_i_amp, 'LineWidth', 2, 'DisplayName', 'I amp'); hold on;
            plot(1e3 * vref .* q_amp, cdf_q_amp, 'r-', 'LineWidth', 2, 'DisplayName', 'Q amp');
            legend('show');
            %     hleg = legend('I amp','Q amp',...
            %         'Location','NorthEastOutside');
            %     % Make the text of the legend italic and color it brown
            %     set(hleg,'FontAngle','italic','TextColor',[.3,.2,.1])
            grid on;
            ht = title(sprintf('I Saturation Rate: %2.2f %%; Q Saturation Rate: %2.2f %%',perSatI, perSatQ));
            
            set(ht,'Interpreter','none')
            xlabel('I/Q voltage (mV) at ADC quantizer');
            ylabel('Cumulative distribution function');
            ax = axis;
            h = text(ax(2) * 0.45,ax(4)-0.75,sprintf(' mean I amp = %.1f mV\n min I amp = %.1f mV\n max I amp = %.1f mV\n',1e3 *vref .* mean(i_amp),...
                1e3 *vref .* min(i_amp), 1e3 * vref .* max(i_amp)));
            h1= text(ax(2) * 0.45,ax(4)-0.25,sprintf(' mean Q amp = %.1f mV\n min Q amp = %.1f mV\n max Q amp = %.1f mV\n',1e3 *vref .* mean(q_amp),...
                1e3 *vref .* min(q_amp), 1e3 * vref .* max(q_amp)));
            set(h,'FontSize',8,'VerticalAlignment','top')
            set(h1,'FontSize',8,'VerticalAlignment','top')
            %     % evaluate the pass/fail;
            %     targer_percentage = int8(target_percentage);
            %     idx_temp = find(adc_amp > target_percentage/100);
            %     fprintf('\n 5. High-valued ADC amp disbribution check \n')
            %     %cprintf('_blue', '\n 5. High-valued ADC amp disbribution check \n')
            %     if numel(idx_temp)
            %         cdf_adc_amp_ninety_tile = 1 - cdf_adc_amp(idx_temp(1));
            %     else
            %         cdf_adc_amp_ninety_tile = 0;
            %     end
            %     if 100 * cdf_adc_amp_ninety_tile > percentage_high_IQ_tile
            %         fprintf('The percentage of the measured ADC amp higher than Vref x %2.0f%%: %2.2f %%\n',targer_percentage, 100 * cdf_adc_amp_ninety_tile);
            %         fprintf('The ADC amp > Vref x %2.0f%% limit : %2.2f %% \n', targer_percentage, percentage_high_IQ_tile);
            %         fprintf('%10s: the high-valued ADC amp percentage is larger than the limit! \n', TestRes{1});
            %     else
            %         fprintf('The percentage of the measured ADC amp higher than Vref x %2.0f%%: %2.2f %%\n',targer_percentage, 100 * cdf_adc_amp_ninety_tile);
            %         fprintf('The ADC amp > Vref x %2.0f%% limit : %2.2f %% \n', targer_percentage, percentage_high_IQ_tile);
            %         fprintf('%10s: the high-valued ADC amp percentage is lower than the limit! \n', TestRes{2});
            %     end
        end
        %==========================
        % Graph the spectrum
        %==========================
        FScale = 1e-6;
        if flag_ref_tone
            figure
            fig.spec = gcf;
            logFftData = 10*log10(linFftData .* inv_R);
            hspec = plot(FScale*fftFreqPts,logFftData,...
                FScale*fftFreqPts,10*log10(noise_flr .* inv_R),...
                FScale*signal_freq(fsort),10*log10(signal_power(fsort).* inv_R),'b.',...
                FScale * fftFreqPts(SigPkIdx + [-numSigSamps:+numSigSamps]), logFftData(SigPkIdx + [-numSigSamps:+numSigSamps]), 'k-', ...
                FScale * SigPos_Hz, SigLevelDbm, 'kd');
            hold on;
            grid on
            xlabel('Frequency (MHz)')
            ylabel('Signal Power (dBm) at ADC quantizer')
            ht = title(sprintf('Frequency domain: %s',iq_file));
            set(ht,'Interpreter','none')
            ax = axis;
            ax(1:2) = FScale*fsample*[-0.5 0.5];
            axis(ax)
            if packet_version > 52
                h = text(ax(1),ax(4)-0.5,sprintf(' RBW = %.1f Hz\n PGA = %d dB',fsample/nfft,pga_gain));
            else
                h = text(ax(1),ax(4)-0.5,sprintf(' RBW = %.1f Hz',fsample/nfft));
            end
            set(h,'FontSize',8,'VerticalAlignment','top')
            % Highlight the wideband jammer band
            if ~isempty(widejam_band)
                xpatch = FScale*widejam_band([1 2 2 1]);
                ypatch = ax([3 3 4 4]);
                h = patch(xpatch,ypatch,'c');
                set(h,'LineStyle','none','FaceAlpha',0.25)
                line(xpatch([1 1]),ypatch([1 3]),'Color','c')
                line(xpatch([2 2]),ypatch([1 3]),'Color','c')
            end
            %---------------------------------------------
            % Highlight the signal bands
            %---------------------------------------------
            % Highlight the GPS band;
            xpatch = FScale*gps_band([1 2 2 1]);
            ypatch = ax([3 3 4 4]);
            h = patch(xpatch,ypatch,'y');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            % Highlight the GLONASS band;
            xpatch = FScale*glo_band([1 2 2 1]);
            h = patch(xpatch,ypatch,'c');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            % Highlight the BDS band;
            xpatch = FScale *bds_band([1 2 2 1]);
            h = patch(xpatch,ypatch,'m');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            % Highlight the GAL band;
            xpatch = FScale *gal_band([1 2 2 1]);
            h = patch(xpatch,ypatch,'k');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            
            % Replot the spectrum in the foreground
            copyobj(hspec(end:-1:1),gca)
            delete(hspec)
            %     zoom xon
        else
            figure
            fig.spec = gcf;
            hspec = plot(1e-6*fftFreqPts,10*log10(linFftData),1e-6*fftFreqPts,10*log10(noise_flr),1e-6*signal_freq(fsort),10*log10(signal_power(fsort)),'b.');
            grid on
            xlabel('Frequency (MHz)')
            ylabel('RMS voltage (dBV) at ADC quantizer')
            ht = title(sprintf('Frequency domain: %s',iq_file));
            set(ht,'Interpreter','none')
            ax = axis;
            ax(1:2) = 1e-6*fsample*[-0.5 0.5];
            axis(ax)
            if is_withheaderhelp
                h = text(ax(1),ax(4)-0.5,sprintf(' RBW = %.1f Hz\n PGA = %d dB',fsample/nfft,pga_gain));
            else
                h = text(ax(1),ax(4)-0.5,sprintf(' RBW = %.1f Hz',fsample/nfft));
            end
            set(h,'FontSize',8,'VerticalAlignment','top')
            
            % Highlight the wideband jammer band
            if ~isempty(widejam_band)
                xpatch = 1e-6*widejam_band([1 2 2 1]);
                ypatch = ax([3 3 4 4]);
                h = patch(xpatch,ypatch,'c');
                set(h,'LineStyle','none','FaceAlpha',0.25)
                line(xpatch([1 1]),ypatch([1 3]),'Color','c')
                line(xpatch([2 2]),ypatch([1 3]),'Color','c')
            end
            %---------------------------------------------
            % Highlight the signal bands
            %---------------------------------------------
            % Highlight the GPS band;
            xpatch = 1e-6*gps_band([1 2 2 1]);
            ypatch = ax([3 3 4 4]);
            h = patch(xpatch,ypatch,'y');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            % Highlight the GLONASS band;
            xpatch = 1e-6*glo_band([1 2 2 1]);
            h = patch(xpatch,ypatch,'c');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            % Highlight the BDS band;
            xpatch = 1e-6 *bds_band([1 2 2 1]);
            h = patch(xpatch,ypatch,'m');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            % Highlight the BDS band;
            xpatch = 1e-6 *gal_band([1 2 2 1]);
            h = patch(xpatch,ypatch,'k');
            set(h,'LineStyle','none','FaceAlpha',0.25)
            line(xpatch([1 1]),ypatch([1 3]),'Color','y')
            line(xpatch([2 2]),ypatch([1 3]),'Color','y')
            % Replot the spectrum in the foreground
            copyobj(hspec(end:-1:1),gca)
            delete(hspec)
            %     zoom xon
        end
        % Save graphs
        if save_results
            diary off
            fftFreqPts = fieldnames(fig);
            for i = 1:length(fftFreqPts)
                name = sprintf('%s_%s',filename,fftFreqPts{i});
                if ispc
                    bmp_file = fullfile(pathstr,[name '.bmp']);
                    saveas(fig.(fftFreqPts{i}),bmp_file,'bmp')
                else
                    png_file = fullfile(pathstr,[name '.png']);
                    saveas(fig.(fftFreqPts{i}),png_file,'png')
                end
            end
        end
        %%%%%%%%%%%%%%%%%%%%%%%%
        %---- Nested functions
        %%%%%%%%%%%%%%%%%%%%%%%%
        %%
        %---------------------------------------------
        % Function 1: detect narrowband jammers;
        %---------------------------------------------
        function run_jammer_detector
            % Loop over max peaks
            while length(signal_power) < max_num_jammers
                [maxval,maxidx] = max(y_notched);
                sigidx = index_window(maxidx);
                sigidx = sigidx(2:end-1);
                y_signal = y_notched(sigidx) - noise_flr(sigidx);
                % Integrate jammer power
                signal_pow = sum(y_signal);
                noise_pow = sum(noise_flr(sigidx));
                % break out the loop if the SNR is less than the detection
                % sensitivity;
                if signal_pow/noise_pow < det_sens
                    break
                end
                signal_power = [signal_power signal_pow];
                signal_freq = [signal_freq sum(fftFreqPts(sigidx).*y_signal)/signal_pow];
                signal_bw = [signal_bw fftFreqPts(sigidx(end))-fftFreqPts(sigidx(1))];
                % Notch out the jammer;
                y_notched(sigidx) = 0;
            end
        end
        %---------------------------------------------
        % Function 2: Automatic measurement of spectral line width
        %---------------------------------------------
        function ivec = index_window(imid)
            % Automatic measurement of spectral line width
            % Find the left edge that is above the noise floor;
            istart = imid;
            while linFftData(istart) > noise_flr(istart)
                istart = istart-1;
                if istart < 1
                    break
                end
            end
            istart = max(1,istart);
            % Find the right edge that is above the noise floor;
            istop = imid;
            while linFftData(istop) > noise_flr(istop)
                istop = istop+1;
                if istop > nfft
                    break
                end
            end
            istop = min(nfft,istop);
            ivec = istart:istop;
        end
        %---------------------------------------------
        % Function 3: generate the cumulative distribution function given a sample group "x"
        %----------------------------------------------
        function [p y] = func_cdf(x)
            y = sort(x, 'ascend');
            p = zeros(size(y));
            for i = 1 : length(p)
                p(i) = i/length(p);
            end
        end
    end
end