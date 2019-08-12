% =============================================================================================
% IQ TEST PROCESSING SCRIPT FOR GPSONE GEN 8 CALLISTO ENGINE FAMILY
% =============================================================================================
%
% Copyright (C) 2003-2014 QUALCOMM Incorporated.
% All Rights Reserved.
% Qualcomm Confidential and Proprietary.
%  Copyright (c) 2015 Qualcomm Technologies, Inc.
%  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
%
% =============================================================================================
%
% REVISION HISTORY
% ---------------------------------------------------------------------------------------------
% Version   Date        Who       Changes
% ----------------------------------------------------------- ----------------------------------
% 2.4.6     01/15/2014 tianqiw  Added WBIQ support for GAL
% 2.4.5      12/13/2013 tianqiw   Added the support of version 53 of CGPQ I/Q log packet format
% 2.4.4	    10/31/2013  tianqiw   Updated the NBIQ pass/fail limits for 50 Hz samples: +/- 3 Hz in FTM/LPM and +/- 10 Hz in OoS/Idle/Traffic
%			          Added the NBIQ 1KHz sample frequency jump pass/fail: the number of large frequency jumps
%				  (> 200 Hz) is less than 10 with the 60 seconds collection window
% 2.4.3     10/07/2013  tianqiw   Modified the number of frequency bins to 8 for BDS CW tone power calculation
% 2.4.2     10/02/2013  tianqiw   Add the BDS equivalent noise bandwidth adjustment parameter
% 2.4.1     08/05/2013  tianqiw   Add Pass/fail criteria for DSDA/DSDS/TSTS
%                                 revise the pass/fail limits of CW tone jammer
% 2.3.12    04/08/2013  tianqiw   Revise WBIQ pass/fail criteria for BDS
% 2.3.11    03/19/2013  jiew      Ignore band edge jammer check (2kHz)
%                                 during TDD LTE blanking
% 2.3.10    01/28/2013  jiew      Revise GSM+TDSCDMA pass/fail limits
%                                 (TDSCDMA 2 uplink slots for data)
% 2.3.9     12/19/2012  tianqiw   Add WBIQ support for BDS
% 2.3.8     10/26/2012  jiew      Add GSM+TDSCDMA and GSM+TDDLTE dual-sys
%                                 blanking for NBIQ test processing
% 2.3.7     10/08/2012  jiew      Add support for SGLTE where GSM+TDSCDMA
%                                 and GSM+TDDLTE dual-sys blanking occur
% 2.3.6     09/18/2012  jiew      Add support for band-dependent blanking
%                                 for TDD-LTE and TDS-CDMA and GSM
% 2.3.5     05/24/2012  tpals     Changed units of noise floor measurement to dBm/Hz
%                                 Including spurs with BW < 1kHz in NF estimate
% 2.3.4     05/15/2012  tpals     Improved GLO NF estimate
%                                 Eliminated C/No metric to avoid confusion with the C/No test
% 2.3.3     04/02/2012  tpals     Increased PowTol to reduce chance of blanking sidelobes
%                                 being classified as jammers
% 2.3.2     03/29/2012  tpals     Integrated print_to_error.m as a subfunction
%                                 In MATLAB environment, send error to command window
% 2.3.1     02/29/2012  jiew      Add support for TD-SCDMA
% 2.3.0     02/22/2012  tpals     New gnss_IqTest_JammerProc with new call syntax
%                                 Added input arguments for engine version and AGC type
%                                 Adjusted P/F limits according to engine and AGC type
% 2.2.7     02/02/2012  tpals     Widened BP3 FBIQ signal detection window (SigUncBins)
% 2.2.6     11/09/2011  jiew      Fix the Noise Figure estimation bias
%                                 (1)use -174dBm/Hz; (2)revise noise mainlobe BW
% 2.2.5     10/28/2011  jiew      Skip Freq and Freq Drift estimate when
%                                 there is large phase jump in estimation window
% 2.2.4     10/14/2011  jiew      Added Freq and Freq Drift estimate for NBIQ
% 2.2.3     9/8/2011    tpals     Changed BP3 FBIQ freq error and IQ std limits
% 2.2.2     8/23/2011   jiew      Fixed bug to support GPS FBIQ
% 2.2.1     6/23/2011   tpals     Created a sinc subfunction
% 2.2.0     6/15/2011   jiew      Added GLO WBIQ
%                                 Added Integration-based NF estimate method
%                                 for GLO BP3 FBIQ and GLO BP4 FBIQ (for Ch0)
%                                 Updated NF and I/Q std limits
% 2.1.0     6/3/2011    tpals     Added BP3 FBIQ
%                                 Fixed GLO passband definition
%                                 Added GLO passband limit lines to the FBIQ graph
%                                 Fixed Hanning window definition
%                                 Fixed weighted average signal frequency calculation
%                                 Removed red limit lines in NBIQ spectrum
% =============================================================================================
function [ret_result] = gnss_IqTest_IqFileProc( PhoneState, SigFreqKhz, SigLevelDbm, FileName, ...
    OutputFolder, AutoClose, StdFolder, Engine, AgcType )
% Version number
VersionNum = '2.4.6';
ret_result = 0;
% moved the try to the beginning to catch and log all possible issues.
try
    %====================================================================
    %   NEW PHONE STATE DEFINITIONS:
    %   PhoneState =10000000 * nwType3 + 1000000 * nwCond3 +...
    %                           100000*nwType2+10000*nwCond2+...
    %                           1000*blankingFlag + 100*gnssSig + ...
    %                           10*nwType + nwCond
    %   nwType3: non-zero for TSTS device only, for the network type for the
    %   voice funcion (GSM support only)
    %             (1) GSM
    %   nwCond3: non-zero for TSTS device only, GSM Idle only
    %            (1) Idle
    %   nwType2: non-zero for SG-LTE/SG-TDSCDMA/DSDS/DSDA/TSTS device, for the network
    %   type for the voice function
    %            (1) 1x  (2) WCDMA (3) GSM
    %   nwCond2: (1) nwType2 is in OOS or UnConnected;
    %            (2) nwType2 is in Idle
    %            (3) nwType2 is in Traffic
    %   blankingFlag: (0) no blanking; (1) GSM blanking, and DSDA UUTs GSM blanking only(2) TDSCDMA blanking; (3) TDDLTE blanking;
    %                 (4) GSM blanking + TDSCDMA blanking, and DSDA UUTs GSM+GSM blanking
    %                 (5) GSM blanking + TDDLTE blanking;
    %	gnssSig:(1) GPS / (2) GLO/ (3) BDS (4) GAL
    %   nwType: (1) 1X / (2) WCDMA / (3) GSM / (4) Unconnected (5) EV-DO / (6) LTE /
    %           (7) SV-DO / (8) SV-LTE (9) TD-SCDMA
    %   Note: when blankingFlag indicate blanking, it overrides the corresponding network condition to be Traffic
    %   nwCond: (1) OoS/UnConnected (for SV-DO/SV-LTE devices, means both 1x and DO/LTE are OOS)
    %           (2) Idle (for SV-DO/SV-LTE devices, means both 1x and DO/LTE are Idle)
    %           (3) Traffic (for SV-DO/SV-LTE devices, means both 1x and DO/LTE are traffic)
    %           (4) 1x OOS, DO/LTE Idle (for SV-DO/SV-LTE devices only)
    %           (5) 1x OOS, DO/LTE Traffic (for SV-DO/SV-LTE devices only)
    %           (6) 1x Idle, DO/LTE OOS (for SV-DO/SV-LTE devices only)
    %           (7) 1x Idle, DO/LTE Traffic (for SV-DO/SV-LTE devices only)
    %           (8) 1x Traffic, DO/LTE OOS (for SV-DO/SV-LTE devices only)
    %           (9) 1x Traffic, DO/LTE Idle (for SV-DO/SV-LTE devices only)
    %   Note: Multimode OoS should be tested with nwType = 2 or 3. 
    %   Note: DSDS/DSDA/TSTS should be tested with nwType = 1, 2, 3 or 9.
    %====================================================================
    if( isdeployed ) %close all figures, including the hidden ones, if this script runs in the deployed mode in QDART.
        close all hidden;
    end %do not close any figure if this script runs in the standalone mode in a regular Matlab window.
    %--------------------------------------------------------------------------
    %           SETTING INPUT PARAMETERS
    %--------------------------------------------------------------------------
    % Set the default parameters
    PhoneStateDefault = 141;
    SigFreqKhzDefault = 100;
    SigLevelDbmDefault = -120;      % -120 dBm is the default CW tone power level for all WBIQ tests
    FileNameDefault = 'gnss_iq_collect.bin';
    OutputFolderDefault = '.\';
    AutoCloseDefault = 0;
    if isdeployed
        StdFolderDefault = 'C:\';
    else
        StdFolderDefault = OutputFolderDefault;
    end
    %nargin_monitor = nargin;
    
    % Set default AGC type
    if ~exist('AgcType','var') || isempty(AgcType)
        AgcType = 'HW AGC';
    end
    
    % Set default engine version
    if ~exist('Engine','var') || isempty(Engine)
        Engine = 'Gen 8A';
    end
    
    % Check standard output folder. Use default if no StdFolder given
    if (nargin <7), StdFolder = StdFolderDefault; end
    
    % Check AutoClose setting. Use default if no AutoClose given
    if( nargin >= 6 )
        % Convert string to number if compiled
        if( isdeployed )
            AutoClose=str2num(AutoClose);
        end
    else
        % No argument means use default
        AutoClose = AutoCloseDefault;
    end
    
    % Check output folder. Use default if no OutputFolder given
    if nargin < 5, OutputFolder = OutputFolderDefault; end;
    
    % Check FileName. Use default if no Filename given
    if nargin < 4, FileName = FileNameDefault; end;
    
    % Check Signal Level. Use default if no SigLevelDbm given
    if( nargin >= 3 )
        % Convert string to number if compiled
        if( isdeployed )
            SigLevelDbm=str2num(SigLevelDbm);
        end
        % Argument zero means use default
        if( SigLevelDbm == 0 )
            SigLevelDbm = SigLevelDbmDefault;
        end
    else
        % No argument means use default
        SigLevelDbm = SigLevelDbmDefault;
    end
    
    % Check Signale Freq. Use default if no SigFreqKhz given
    if( nargin >= 2 )
        % Convert string to number if compiled
        if( isdeployed )
            SigFreqKhz=str2num(SigFreqKhz);
        end
        % Argument zero means use default
        if( SigFreqKhz == 0 )
            SigFreqKhz = SigFreqKhzDefault;
        end
    else
        % No argument means use default
        SigFreqKhz = SigFreqKhzDefault;
    end
    
    % Default PhoneState
    if( nargin >= 1 )
        % Convert string to number if compiled
        if( isdeployed )
            PhoneState=str2num(PhoneState);
        end
        % Argument zero means use default
        if( PhoneState == 0 )
            PhoneState = PhoneStateDefault;
        end
    else
        % No argument means use default
        PhoneState = PhoneStateDefault;
    end
    
    %--------------------------------------------------------------------------
    %           INITIALIZATIONS
    %--------------------------------------------------------------------------
    % Split Up PhoneState to get nwType3/nwCond3/nwType2/nwCond2/blankingFlag/gnssSig/nwType/nwCond
    % Note that nw3Type
    nw3Type = floor(PhoneState / 10000000);
    nw3Cond =floor((PhoneState - nw3Type * 10000000) / 1000000);
    nw2Type = floor((PhoneState - nw3Type * 10000000 - nw3Cond * 1000000)/ 100000);
    nw2Cond = floor((PhoneState - nw3Type * 10000000 - nw3Cond * 1000000 - nw2Type * 100000) / 10000);
    blankingFlag = floor((PhoneState - nw3Type * 10000000 - nw3Cond * 1000000 - nw2Type * 100000 - nw2Cond * 10000)/1000);
    gnssSig = floor((PhoneState - nw3Type * 10000000 - nw3Cond * 1000000 - nw2Type * 100000 - nw2Cond * 10000 - blankingFlag * 1000) / 100);
    nwType = floor((PhoneState - nw3Type * 10000000 - nw3Cond * 1000000 - nw2Type * 100000 - nw2Cond * 10000 - blankingFlag * 1000 - gnssSig * 100) / 10);
    nwCond = PhoneState - nw3Type * 10000000 - nw3Cond * 1000000 - nw2Type * 100000 - nw2Cond * 10000 - blankingFlag * 1000 - gnssSig * 100 - nwType * 10;
    
    MarkType = {'r.','r+','rx','r*'};      % markers for various failing jammers
    
    % Generate string for network condition string nwStr - e.g., 'GSM-Traffic'
    nwStr1 = {'1X','WCDMA','GSM','Unconnected','EV-DO','LTE','SV-DO','SV-LTE', 'TD-SCDMA', 'SG-LTE or Dual-Sim UUTs'...
        'Tri-Sim UUTs'};
    nwStr2 = {'OoS','Idle','Traffic', ...
        '(1X OoS, DO/LTE Idle)', '(1X OoS, DO/LTE Traffic)', ...
        '(1X Idle, DO/LTE OoS)', '(1X Idle, DO/LTE Traffic)', ...
        '(1X Traffic, DO/LTE OoS)', '(1X Traffic, DO/LTE Idle)'};
    % Define the network condition display according to the network setup;
    if nw3Type ==0 % If not TSTS UUTs
        if nw2Type == 0
            if nwType ~= 4
                nwStr = strcat(nwStr1{nwType}, '-', nwStr2{nwCond});
            else  % 'Unconnected UUT'
                nwStr = nwStr1{nwType};
            end
        else
            nwStr = strcat(nwStr1{10}, '(', nwStr1{nw2Type}, '-', nwStr2{nw2Cond}, ',', nwStr1{nwType}, '-', nwStr2{nwCond}, ')');
        end
    else                    % If TSTS UUTs
        nwStr = strcat(nwStr1{11}, '(', nwStr1{nw3Type}, '-', nwStr2{nw3Cond}, ',', nwStr1{nw2Type}, '-', nwStr2{nw2Cond}, ',', nwStr1{nwType}, '-', nwStr2{nwCond}, ')');
    end
    %--------------------------------------------------------------------
    % Define the pass/fail limits for NBIQ based on the network condition;
    % The 50 Hz frequency difference should be within +/- 3Hz in FTM and LPM (Unconnected)
    % The 50 Hz frequency difference should be within +/- 10 Hz otherwise
    %--------------------------------------------------------------------
    if (nwType == 0 || nwType == 4) && (nw2Type == 0 || nw2Type == 4) && (nw3Type == 0 || nw3Type == 4) 
        Thres_MaxFreqDiffFrom50hz = 3;
    else 
        Thres_MaxFreqDiffFrom50hz = 10;
    end
    % The additional pass/fail on 1KHz NBIQ samples: if there are more than 10 frequency jumps from the 1KHz samples larger than 200 Hz, then fail the NBIQ;
    Thres_MaxFreqDiffFrom1khz = 200;
    Thres_num_largefreqdiff1khz = 10;
    
    if (isdeployed)
        % If deployed, to show filename as special symbols caused MCR Java exceptions.
        FileNameTitle = '';
        %set visibility to off if AutoClose.
        if (AutoClose)
            set(0,'DefaultFigureCreateFcn','set(gcbo,''Visible'',''off'')');
        else
            set(0,'DefaultFigureCreateFcn','set(gcbo,''Visible'',''on'')');
        end
    else
        % Need to <ESC> underscores with a '\' if we want to avoid incorrectly
        % subscripting these in the title.
        FileNameTitle = strrep( FileName, '_', '\_' );
        FileNameTitle = strcat('- File:',FileNameTitle);
    end      
    %--------------------------------------------------------------------------
    %           READ IN AND CLEAN UP DATA FROM IQ COLLECT FILE
    %--------------------------------------------------------------------------
    % Read the WBIQ data file;
    fid = fopen(FileName,'r'); 
    packet_version = fread(fid,1,'int32');
    if (packet_version == 54)
        header_sz = 23;
    elseif (packet_version == 53)
        header_sz = 19;
    else
        header_sz = 14;
    end
    ConfigTemp = fread(fid, header_sz-1, 'int32'); % Read the first header_sz -1 32-bit symbols;
    Config = [packet_version; ConfigTemp];
    BinFileVersion = Config(1);
    Flag = Config(2);
    PeakFFTPower = Config(3);
    FFTCenterFreq = Config(4);
    CollectMode = Config(5);
    GloChan = Config(6);
    GloHWChan = Config(7);
    SamplesPerInteg = Config(8) * 1024;
    NumCollections = Config(9);
    NBCenterFreqHz = Config(10);
    BpOffsetFreqHz = Config(11);
    TotalRelativeGain = Config(12);
    FBIqMode = Config(13);
    Reserved = Config(14);
    
    %--------------------------------------------------------------------------
    %           SET (FB/WB/NB), GPS/GLO/BDS, NW COND. DEPENDENT PARAMETERS
    %--------------------------------------------------------------------------    
    % Determine the test type according to the collection mode;
    switch(CollectMode),
        case {0, 1, 2}
            testType = 'WB-IQ: GPS';
        case {3}
            testType = 'WB-IQ: GLO';
        case {4}
            if gnssSig == 1,
                testType = 'FB-IQ: GPS';
            else
                testType = 'FB-IQ: BP4 GLO';
            end
        case {5, 6, 8, 9, 11}
            testType = 'NB-IQ: GPS';
        case {7, 10}
            testType = 'NB-IQ: GLO';
        case {15}
            testType = 'FB-IQ: BP3 GLO';
        case {17} % BDS WBIQ Collect mode;
            testType = 'WB-IQ: BDS';
        case {18} % GAL WBIQ Collect mode;
            testType = 'WB-IQ: GAL';
        otherwise
            error('Unsupported collect mode.')
    end
    % Setup the parameters according to the test type;
    switch (testType)
        case 'FB-IQ: GPS',
            SampsToDiscard = 4*20;                                                     % Discard 4 micro-second worth of data for the hardware to settle;  
            FSample = 20.46e6;                                                             % sample rate;
            FScale = 1e-6;                                                                       % frequency scale (MHz);
            FUnits = 'MHz';                                                                     % frequency unit 
            BinSize = FSample / SamplesPerInteg;                              % Bin size in Hz;
            MedFiltBW = 50e3;                                                               % in Hz; the median filter bandwidth is 50 KHz
            fftFreqPts = ([0:(SamplesPerInteg-1)]/SamplesPerInteg - 0.5) * FSample;
            inBandFreqIdx = find(abs(fftFreqPts) <= 100e3);             % generate the inband (100 KHz) frequency indices
            numSigSamps = 15;                                                             % +/- numSigSamps is the number of frequency bins that are considered as covered by the CW tone;
            noFlFreqPts = linspace(-FSample/2,FSample/2,FSample/MedFiltBW);
            OnePpmKHz = 1.57542;                                                       % 1 ppm in KHz w.r.t. the carrier frequency;
            SampleOffset = 0.5;                                                               % Sample memory sums need to be adjusted by +0.5 to correct the DC offset;
            SampleRotateHz = 0;                                                            % The additional rotation for the samples (used in the channelization of GLONASS)
            SampleOrder = 1:4;                                                               % The order of sample stored in the memory: I1 Q1 I2 Q2 I3 Q3 I4 Q4 is the desired order;
        case 'WB-IQ: GPS',
            SampsToDiscard = 4*2;
            FSample = 2.046e6;
            FScale = 1e-3;
            FUnits = 'KHz';
            BinSize = FSample / SamplesPerInteg;
            MedFiltBW = 50e3;                                                               % in Hz
            fftFreqPts = ([0:(SamplesPerInteg-1)]/SamplesPerInteg - 0.5) * FSample;
            inBandFreqIdx = find(abs(fftFreqPts) <= 100e3);
            numSigSamps = 15;
            noFlFreqPts = linspace(-FSample/2,FSample/2,FSample/MedFiltBW);
            OnePpmKHz = 1.57542;                        
            SampleOffset = 0.5;                                                             % The Sample Offset is the DC offset caused by ADC;
            SampleRotateHz = 0;
            SampleOrder = 1:4;
        case 'FB-IQ: BP3 GLO',
            SampsToDiscard = 0;
            FSample = 10.22e6;
            FScale = 1e-6;
            FUnits = 'MHz';
            BinSize = FSample / SamplesPerInteg;
            MedFiltBW = 25e3;                           % in Hz
            fftFreqPts = ([0:(SamplesPerInteg-1)]/SamplesPerInteg - 0.5) * FSample;
            inBandFreqIdx = find(abs(fftFreqPts) <= 50e3);
            numSigSamps = 15;
            noFlFreqPts = linspace(-FSample/2,FSample/2,FSample/MedFiltBW);
            OnePpmKHz = 1.60200;
            SampleOffset = 0;
            SampleRotateHz = -281.25e3;
            SampleOrder = [4 3 2 1];
        case 'FB-IQ: BP4 GLO',
            SampsToDiscard = 4*20;
            FSample = 20.46e6;
            FScale = 1e-6;
            FUnits = 'MHz';
            BinSize = FSample / SamplesPerInteg;
            MedFiltBW = 25e3;                           % in Hz
            fftFreqPts = ([0:(SamplesPerInteg-1)]/SamplesPerInteg - 0.5) * FSample;
            inBandFreqIdx = find(abs(fftFreqPts) <= 50e3);
            numSigSamps = 15;
            noFlFreqPts = linspace(-FSample/2,FSample/2,FSample/MedFiltBW);
            OnePpmKHz = 1.60200;
            SampleOffset = 0.5;
            SampleRotateHz = 0;
            SampleOrder = 1:4;
        case 'WB-IQ: GLO',
            SampsToDiscard = 4*2;
            FSample = 1.022e6;
            FScale = 1e-3;
            FUnits = 'KHz';
            BinSize = FSample / SamplesPerInteg;
            MedFiltBW = 25e3;                           % in Hz
            fftFreqPts = ([0:(SamplesPerInteg-1)]/SamplesPerInteg - 0.5) * FSample;
            inBandFreqIdx = find(abs(fftFreqPts) <= 50e3);
            numSigSamps = 15;
            noFlFreqPts = linspace(-FSample/2,FSample/2,FSample/MedFiltBW);
            OnePpmKHz = 1.60200;
            SampleOffset = 0.5;
            SampleRotateHz = 0;
            SampleOrder = 1:4;
        case 'WB-IQ: BDS',                            % BDS WBIQ Header file configuration;
            SampsToDiscard = 8 * 2;
            FSample = 4.092e6;
            FScale = 1e-3;
            FUnits = 'KHz';
            BinSize = FSample / SamplesPerInteg;
            MedFiltBW = 50e3;                           % in Hz
            fftFreqPts = ([0:(SamplesPerInteg-1)]/SamplesPerInteg - 0.5) * FSample;
            inBandFreqIdx = find(abs(fftFreqPts) <= 100e3);
            numSigSamps = 8;
            noFlFreqPts = linspace(-FSample/2,FSample/2,FSample/MedFiltBW);
            OnePpmKHz = 1.561098; 
            SampleOffset = 0.5;
            SampleRotateHz = 0;
            SampleOrder = 1:4;
        case 'WB-IQ: GAL',                            % GAL WBIQ Header file configuration;
            SampsToDiscard = 8 * 2;
            FSample = 4.092e6;
            FScale = 1e-3;
            FUnits = 'KHz';
            BinSize = FSample / SamplesPerInteg;
            MedFiltBW = 50e3;                           % in Hz
            fftFreqPts = ([0:(SamplesPerInteg-1)]/SamplesPerInteg - 0.5) * FSample;
            inBandFreqIdx = find(abs(fftFreqPts - SigFreqKhz * 1e3) <= 100e3);
            numSigSamps = 8;
            noFlFreqPts = linspace(-FSample/2,FSample/2,FSample/MedFiltBW);
            OnePpmKHz = 1.57542; 
            SampleOffset = 0.5;
            SampleRotateHz = 0;
            SampleOrder = 1:4; 
    end
    % Indicate the current LTE traffic condition;
    LteTraffic = (nwType == 6 ...               % LTE
        || nwType == 8)...                                  % SV-LTE
        && (nwCond == 3 ...                              % Traffic (for SV-DO/SV-LTE devices, means both 1x and DO/LTE are traffic)      
        || nwCond == 5 ...                                    % 1x OOS, DO/LTE Traffic (for SV-DO/SV-LTE devices only)
        || nwCond == 7);                                     % 1x Idle, DO/LTE Traffic (for SV-DO/SV-LTE devices only)
    GsmBlanking = (blankingFlag == 1);                    % Also used in DSDA UUTs with GSM + WCDMA and GSM + 1x
    TdscdmaBlanking = (blankingFlag == 2);
    TddlteBlanking = (blankingFlag == 3);
    GsmTdscdmaBlanking = (blankingFlag == 4);     % Also used in DSDA UUTs with GSM + GSM and GSM + TDSCDMA
    GsmTddlteBlanking = (blankingFlag == 5);
    Gen8 = isequal(lower(Engine),'gen 8') || isequal(lower(Engine),'gen8');    
    %--------------------------------------------------------------------------
    %       WBIQ and FBIQ TEST
    %--------------------------------------------------------------------------
    if (CollectMode <= 4 || CollectMode == 15 || CollectMode == 17 || CollectMode == 18),
        %--------------------------------------------------------------------------
        %           SET PASS/FAIL PARAMETERS for WBIQ and FBIQ
        %--------------------------------------------------------------------------        
        % --- Signal frequency bias ---%
        if (CollectMode == 15) % FB-IQ: BP3 GLO;
            freqPpmLim = 16;  % XO error on carrier is uncompensated in BP3 FBIQ
        elseif (nwType == 4 || nwCond == 1 || nwCond == 4 || nwCond == 5),
            %  for single WWAN device, OoS/Unconnected
            %  for SV-DO/SV-LTE devices, whenever 1x is in OOS, GPS does not get
            %  frequency assistance from WWAN
            freqPpmLim = 5;  % unit is ppm
        else
            freqPpmLim = 0.33; % unit is ppm, for in-service
        end
        
        % --- GPS Noise Figure ---%
        RFNoiseFigureLim = 5.0;
        if GsmBlanking
            RFNoiseFigureLim = 5.5;
        elseif TdscdmaBlanking
            RFNoiseFigureLim = 5.5;
        elseif GsmTdscdmaBlanking  
            RFNoiseFigureLim = 7.0; % 2.0dB max loss
        elseif GsmTddlteBlanking
            RFNoiseFigureLim = 8.5; % 3.5dB max loss
        elseif LteTraffic
            if Gen8
                RFNoiseFigureLim = 10;
            else % Gen 8A
                RFNoiseFigureLim = 6.5;
                if(TddlteBlanking)
                    RFNoiseFigureLim = 7.5;
                end
            end
        end
        
        % --- BDS Noise Figure ---%
        if isequal(testType, 'WB-IQ: BDS')
            RFNoiseFigureLim = 10.0;
            if GsmBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 0.5;
            elseif TdscdmaBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 0.5;
            elseif TddlteBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 2.5;
            elseif GsmTdscdmaBlanking 
                RFNoiseFigureLim = RFNoiseFigureLim + 2.0; % 2.0dB max loss
            elseif GsmTddlteBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 3.5; % 3.5dB max loss
            end
        end
        
          % --- GAL Noise Figure ---%
        if isequal(testType, 'WB-IQ: GAL')
            RFNoiseFigureLim = 10.0;
            if GsmBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 0.5;
            elseif TdscdmaBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 0.5;
            elseif TddlteBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 2.5;
            elseif GsmTdscdmaBlanking 
                RFNoiseFigureLim = RFNoiseFigureLim + 2.0; % 2.0dB max loss
            elseif GsmTddlteBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 3.5; % 3.5dB max loss
            end
        end
        
        % --- GLO Noise Figure ---%
        if isequal(testType, 'FB-IQ: BP3 GLO') || ...
                isequal(testType, 'FB-IQ: BP4 GLO') || ...
                isequal(testType, 'WB-IQ: GLO')
            if Gen8
                RFNoiseFigureLim = 10.0;
            else % Gen 8A
                RFNoiseFigureLim = 7.0;
            end
            if GsmBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 0.5;
            elseif TdscdmaBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 0.5;
            elseif TddlteBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 2.5;
            elseif GsmTdscdmaBlanking 
                RFNoiseFigureLim = RFNoiseFigureLim + 2.0; % 2.0dB max loss
            elseif GsmTddlteBlanking
                RFNoiseFigureLim = RFNoiseFigureLim + 3.5; % 3.5dB max loss
            end
        end
        
        % --- GPS IQ Standard Deviation ---%
        SwAgc = Gen8 && (isequal(lower(AgcType),'sw') || isequal(lower(AgcType),'swagc'));
        if SwAgc
            if GsmBlanking
                % sqrt(1.67^2*7/8+0.5^2*1/8) = 1.57
                % sqrt(1.87^2*7/8+0.5^2*1/8) = 1.76
                stdLoLim = 1.56; stdHiLim = 1.75;
            else
                stdLoLim = 1.67; stdHiLim = 1.87;
            end
        else % HW AGC
            if GsmBlanking
                %sqrt(2.72^2*7/8+0.5^2*1/8) = 2.55
                %sqrt(3.05^2*7/8+0.5^2*1/8) = 2.86
                stdLoLim = 2.54; stdHiLim = 2.85;
            elseif TdscdmaBlanking  % TD-SCMDA enabled targets always have HW-AGC
                % sqrt(2.72^2*(5000-675)/5000+0.5^2*675/5000) = 2.54
                % sqrt(3.05^2*(5000-675)/5000+0.5^2*675/5000) = 2.84
                stdLoLim = 2.54; stdHiLim = 2.84;
            elseif TddlteBlanking  % TDD-LTE enabled targets always have HW-AGC
                % sqrt(2.72^2*(5000-2143)/5000+0.5^2*2143/5000) = 2.08
                % sqrt(3.05^2*(5000-2143)/5000+0.5^2*2143/5000) = 2.33
                stdLoLim = 2.08; stdHiLim = 2.33;
            elseif GsmTdscdmaBlanking
                % GSM + TDSCDMA: max blanking duty cycle = 0.37, min duty cycle = 0.35
                % sqrt(2.72^2*0.63+0.5^2*0.37) = 2.18
                % sqrt(3.05^2*0.65+0.5^2*0.35) = 2.48
                %                 stdLoLim = 2.18; stdHiLim = 2.48;
                % GSM + GSM: max blanking duty cycle = 0.38, min duty cycle
                % = 0.25
                % sqrt(2.72^2*0.62+0.5^2*0.38) = 2.18
                % sqrt(3.05^2*0.75+0.5^2*0.25) = 2.66
                stdLoLim = 2.16; stdHiLim = 2.66;                
            elseif GsmTddlteBlanking
                % max blanking duty cycle = 0.52, min duty cycle = 0.48
                % sqrt(2.72^2*0.48+0.5^2*0.52) = 1.91
                % sqrt(3.05^2*0.52+0.5^2*0.48) = 2.23
                stdLoLim = 1.91; stdHiLim = 2.23;
            else
                stdLoLim = 2.72; stdHiLim = 3.05;
            end
        end
        
        % --- BDS IQ Standard Deviation ---%
        if isequal(testType, 'WB-IQ: BDS')
            if GsmBlanking
                stdLoLim = 2.52; stdHiLim = 2.95;
            elseif TdscdmaBlanking  % TD-SCMDA enabled targets always have HW-AGC
                stdLoLim = 2.52; stdHiLim = 2.95;
            elseif TddlteBlanking  % TDD-LTE enabled targets always have HW-AGC
                stdLoLim = 2.07; stdHiLim = 2.40;
            elseif GsmTdscdmaBlanking
                % GSM + TDSCDMA : max blanking duty cycle = 0.37, min duty cycle = 0.35
                % stdLoLim = 2.16; stdHiLim = 2.48;
                % GSM + GSM : max blanking duty cycle = 0.38, min duty
                % cycle = 0.25
                stdLoLim = 2.15; stdHiLim = 2.74;                
            elseif GsmTddlteBlanking
                % max blanking duty cycle = 0.52, min duty cycle = 0.48
                stdLoLim = 1.90; stdHiLim = 2.30;            
            else
                stdLoLim = 2.70; stdHiLim = 3.15;
            end
        end
        
        % --- GAL IQ Standard Deviation ---%
        if isequal(testType, 'WB-IQ: GAL')
            if GsmBlanking
                stdLoLim = 2.52; stdHiLim = 2.95;
            elseif TdscdmaBlanking  % TD-SCMDA enabled targets always have HW-AGC
                stdLoLim = 2.52; stdHiLim = 2.95;
            elseif TddlteBlanking  % TDD-LTE enabled targets always have HW-AGC
                stdLoLim = 2.07; stdHiLim = 2.40;
            elseif GsmTdscdmaBlanking
                % GSM + TDSCDMA : max blanking duty cycle = 0.37, min duty cycle = 0.35
                % stdLoLim = 2.16; stdHiLim = 2.48;
                % GSM + GSM : max blanking duty cycle = 0.38, min duty
                % cycle = 0.25
                stdLoLim = 2.15; stdHiLim = 2.74;                
            elseif GsmTddlteBlanking
                % max blanking duty cycle = 0.52, min duty cycle = 0.48
                stdLoLim = 1.90; stdHiLim = 2.30;            
            else
                % The measured standard deviation is 2.8887
                stdLoLim = 2.70; stdHiLim = 3.15;
            end
        end
        
        % --- GLO IQ Standard Deviation ---%
        if isequal(testType, 'FB-IQ: BP3 GLO')
            if GsmBlanking
                stdLoLim = 9.5; stdHiLim = 38.0;
            elseif TdscdmaBlanking
                stdLoLim = 9.5; stdHiLim = 38.0;
            elseif TddlteBlanking
                stdLoLim = 7.6; stdHiLim = 30.5;
            elseif GsmTdscdmaBlanking
                stdLoLim = 8.0; stdHiLim = 32.0;
            elseif GsmTddlteBlanking
                stdLoLim = 7.0; stdHiLim = 29.1;
            else
                stdLoLim = 10.1; stdHiLim = 40.4;
            end
        elseif isequal(testType, 'FB-IQ: BP4 GLO')
            if GsmBlanking
                % sqrt(2.78^2*7/8+0.5^2*1/8) = 2.61
                % sqrt(3.11^2*7/8+0.5^2*1/8) = 2.91
                stdLoLim = 2.61; stdHiLim = 2.91;
            elseif TdscdmaBlanking
                stdLoLim = 2.59; stdHiLim = 2.90;
            elseif TddlteBlanking
                stdLoLim = 2.13; stdHiLim = 2.37;
            elseif GsmTdscdmaBlanking
                % max blanking duty cycle = 0.37, min duty cycle = 0.35
                % sqrt(2.78^2*0.63+0.5^2*0.37) = 2.23
                % sqrt(3.11^2*0.65+0.5^2*0.35) = 2.52
                % max blanking duty cycle = 0.38, min duty cycle = 0.25
                % sqrt(2.78^2*0.62+0.5^2*0.38) = 2.21
                % sqrt(3.11^2*0.75+0.5^2*0.25) = 2.71
                stdLoLim = 2.21; stdHiLim = 2.71;
                %                 stdLoLim = 2.23; stdHiLim = 2.52;
            elseif GsmTddlteBlanking
                % max blanking duty cycle = 0.52, min duty cycle = 0.48
                % sqrt(2.78^2*0.48+0.5^2*0.52) = 1.96
                % sqrt(3.11^2*0.52+0.5^2*0.48) = 2.27
                stdLoLim = 1.96; stdHiLim = 2.27;
            else
                stdLoLim = 2.78; stdHiLim = 3.11;
            end
        elseif isequal(testType, 'WB-IQ: GLO')
            if GsmBlanking
                stdLoLim = 2.54; stdHiLim = 2.85;
            elseif TdscdmaBlanking
                stdLoLim = 2.54; stdHiLim = 2.84;
            elseif TddlteBlanking  % TDD-LTE enabled targets always have HW-AGC
                stdLoLim = 2.08; stdHiLim = 2.33;
            elseif GsmTdscdmaBlanking
                % GSM + TDSCDMA Blanking
                % stdLoLim = 2.18; stdHiLim = 2.48;
                % GSM + GSM Blanking: 
                % max blanking duty cycle = 0.38, min duty cycle = 0.25
                % sqrt(2.72^2*0.62+0.5^2*0.38) = 2.16
                % sqrt(3.05^2*0.75+0.5^2*0.25) = 2.66
                stdLoLim = 2.16; stdHiLim = 2.66;
            elseif GsmTddlteBlanking
                stdLoLim = 1.91; stdHiLim = 2.23;
            else
                stdLoLim = 2.72; stdHiLim = 3.05;
            end
        end
        %--------------------------------------------------------------------------
        %           GLO WB-IQ NOISE FIGURE ADJUSTMENT DATA; 
        %--------------------------------------------------------------------------
        GloChanIdx = GloChan + 8;
        GloExcessGain = zeros(1,14);
        if Gen8
            GloExcessBn = [0.212 0.035 -0.056 -0.276 -0.222 -0.119 -0.262 -0.265 -0.119 -0.217 -0.293 -0.110 -0.129 -0.253];
            if SigFreqKhz == 50
                GloExcessGain = [-0.030 -0.223 -0.049 -0.074 -0.212 -0.080 -0.048 -0.187 -0.126 -0.011 -0.163 -0.163 -0.003 -0.229];
            elseif SigFreqKhz == 100
                GloExcessGain = [-0.226 -0.573 -0.216 -0.295 -0.555 -0.277 -0.236 -0.514 -0.364 -0.160 -0.470 -0.442 -0.138 -0.631];
            end
        else % Gen8A
            GloExcessBn = [0.743 0.894 0.714 0.849 0.889 0.723 0.865 0.864 0.725 0.891 0.847 0.712 0.891 0.738];
            if SigFreqKhz == 50
                GloExcessGain = [0.095 0.186 0.137 0.021 0.186 0.120 0.052 0.195 0.107 0.070 0.227 0.089 0.061 0.116];
            elseif SigFreqKhz == 100
                GloExcessGain = [0.369 0.569 0.443 0.237 0.567 0.412 0.299 0.581 0.386 0.342 0.640 0.348 0.322 0.401];
            end
        end        
        %--------------------------------------------------------------------------
        %           BDS WB-IQ NOISE FIGURE ADJUSTMENT DATA;
        %--------------------------------------------------------------------------
        BdsExcessBn = 0.343;
        %--------------------------------------------------------------------------
        %           GAL WB-IQ NOISE FIGURE ADJUSTMENT DATA;
        %--------------------------------------------------------------------------
        GalExcessBn = -0.0802;
        %--------------------------------------------------------------------------
        %           WB-IQ AND FB-IQ TEST POST PROCESSING
        %--------------------------------------------------------------------------
        
        % ----------- OPEN THE SUMMARY FILE ----------------------------------
        SUMMARY = fopen(strcat(OutputFolder, 'Summary_IQWB.txt'),'w');
        fprintf( SUMMARY, 'Summary IQ Wideband -- gnss_IqTest_IqFileProc Version %s\r\n\r\n', VersionNum );
                
        % ------------ GET REQUIRED SAMPLES FROM FILE -------------------------
        iqSamps = zeros(NumCollections*SamplesPerInteg,1);
        SampsPer4ms = 4e-3 * FSample;
        % Jasper: book mark;
        LowSampCntWarning = 0;
        for indxnc = 1:NumCollections,
            % Get "out of memory error" for FB-IQ when reading I/Q outside of the loop, so do in loop
            x = fread( fid, 2 * SamplesPerInteg, 'int8');
            if (length(x) < 2 * SamplesPerInteg),
                if ~ LowSampCntWarning,
                    fprintf( SUMMARY, '\r\nW A R N I N G: FEWER THAN EXPECTED SAMPLES IN LOG FILE !!! \r\n');
                end
                x = [x; zeros(2*SamplesPerInteg-length(x),1)];
            end
            % Permute byte order so that X = [I1 Q1 I2 Q2 I3 Q3 ...]'
            if ~isequal(SampleOrder, 1:4)
                x = reshape(x, 4, []);
                x = x(SampleOrder,:);
                x = x(:);
            end
            % Deinterleave I and Q
            x = x(1:2:end) + sqrt(-1)*x(2:2:end);
            for indx_d = 1:SampsToDiscard,
                x((SampsPer4ms + 1):(SampsPer4ms + SampsToDiscard + 1 - indx_d):end) = [];
            end            
            iqSamps((indxnc-1)*SamplesPerInteg + [1:SamplesPerInteg]) = ...
                [x; zeros(SamplesPerInteg - length(x),1)];            
        end
        
        % Sample memory sums need to be adjusted by +0.5 to correctly
        % map to the correct signed number representation.
        % Samples written by sample log should not be adjusted.
        iqSamps = iqSamps + SampleOffset*(1 + sqrt(-1));
        % Calculate the I&Q standard deviation after correction;
        stdReal = std(real(iqSamps)); stdImag = std(imag(iqSamps));
        
        % ----------- PROCESS SAMPLES AND EXTRACT RELEVANT INFO -------------------
        
        % Apply a frequency shift to recenter the spectrum as desired
        if (SampleRotateHz ~= 0)
            iqSamps = iqSamps .* exp(sqrt(-1)*2*pi*SampleRotateHz/FSample*(0:length(iqSamps)-1)');
        end
        % Use Hanning window
        HanningWindow = hanning( SamplesPerInteg );
        
        % SamplesPerInteg-point FFT for each collection
        linFftData = zeros(SamplesPerInteg,1);
        for indxNonCohAdd = 1:NumCollections,
            sampIdx = ( indxNonCohAdd - 1 ) * SamplesPerInteg + ( 1:SamplesPerInteg );
            linFftData = linFftData + abs(fft(HanningWindow .* iqSamps(sampIdx))).^2;
        end
        linFftData = fftshift(linFftData);
        
        % Locate the signal using input signal frequency
        SigUncBins = ceil(max(7, 2 * freqPpmLim) * OnePpmKHz * 1e3 / BinSize);
        expectedSigIdx = find(abs(fftFreqPts-SigFreqKhz*1e3)==min(abs(fftFreqPts-SigFreqKhz*1e3)));
        [SigPkVal SigPkIdx] = max(linFftData(expectedSigIdx + [-SigUncBins:SigUncBins]));
        SigPkIdx = expectedSigIdx - SigUncBins + SigPkIdx - 1;
        sigPower = sum( linFftData( SigPkIdx + [-numSigSamps:numSigSamps] ) );
        
        % Normalize spectrum w.r.t. sigPower
        linFftData = linFftData / sigPower;
        
        % In WBIQ test, the sigPower of the CW ref tone has been
        % attenuated due to chip match filter, need to compensate that
        if isequal(testType, 'WB-IQ: GPS')
            linFftData = linFftData * (sinc(SigFreqKhz/1023))^2;
        elseif isequal(testType, 'WB-IQ: BDS')
            linFftData = linFftData * (sinc(SigFreqKhz/2046))^2;
        elseif isequal(testType, 'WB-IQ: GLO')
            linFftData = linFftData * (sinc(SigFreqKhz/511))^2;
            linFftData = linFftData * 10^(GloExcessGain(GloChanIdx)/10);
        elseif isequal(testType, 'WB-IQ: GAL')
            % GAL ref tone is chosen such that the attenuation is normalized to 1;
            linFftData = linFftData;            
        end
        % Calculate the noise floor using median filtering
        noiseFloor = zeros(size(noFlFreqPts));
        for indxNF = 1:length(noFlFreqPts)
            % find freq. points within Median Filter bandwidth
            FiltFreqIndx = find(abs(fftFreqPts-noFlFreqPts(indxNF)) < MedFiltBW);
            % use a median filter to get the mean noise level
            noiseFloor(indxNF) = median(linFftData(FiltFreqIndx));
        end
        
        % Adjust noiseFloor due to the median vs. mean estimate bias.
        noiseFloor = noiseFloor / (1-1/(9*NumCollections))^3;
        
        noiseFloor = lininterp(noFlFreqPts,noiseFloor,fftFreqPts);
        
        % Mean noise floor only considers within +/-100kHz for GPS WBIQ,
        % and +/-50kHz for GLO WBIQ and FBIQ
        meanNoiseFloor = mean(noiseFloor(inBandFreqIdx));
        noiseFloor = 10*log10(abs(noiseFloor));
        
        % calculate the C/No
        sigCNo =  (1 - meanNoiseFloor) / meanNoiseFloor...
            * BinSize;
        
        % Orig NF estimation method (Method 1): average within +/-100kHz
        % for GPS WBIQ, +/-50kHz for GLO WBIQ and FBIQ
        measuredRFNoiseFigure1 = (SigLevelDbm + 174 - 10*log10(sigCNo));
        
        % Convert from linear scale to log (dB)
        logFftData = 10*log10(linFftData);
        
        % Find the "weighted average" signal position
        SigPos_Hz = sum((linFftData( SigPkIdx + [-numSigSamps:numSigSamps] ).')...
            .* fftFreqPts(SigPkIdx + [-numSigSamps:numSigSamps]))/...
            sum(linFftData( SigPkIdx + [-numSigSamps:numSigSamps] ));
        
        % Remove signal and run the jammer test
        SigLoLim = SigPkIdx - numSigSamps - 1;
        SigHiLim = SigPkIdx + numSigSamps + 1;
        % when GSM + TDSCDMA/TDDLTE dual system blanking, ignore jammer
        % check within CW signal +/- 2000Hz
        if GsmTdscdmaBlanking || GsmTddlteBlanking
            SigLoLim = SigPkIdx - ceil(2000/BinSize) - 1;
            SigHiLim = SigPkIdx + ceil(2000/BinSize) + 1;
        end
        tempFftData = [logFftData(1:SigLoLim); logFftData(SigHiLim:end)];
        tempFftPts = [fftFreqPts(1:SigLoLim) fftFreqPts(SigHiLim:end)];
        tempNoiseFloor = [noiseFloor(1:SigLoLim) noiseFloor(SigHiLim:end)];
        
        % Ignore 2000Hz at the band-edge during jammer check when in
        % TDD-LTE Blanking.  This is to avoid flagging jammer due to
        % blanking symbols
        if TddlteBlanking
            tempFftData = tempFftData(ceil(2000/BinSize):end-ceil(2000/BinSize));
            tempFftPts = tempFftPts(ceil(2000/BinSize):end-ceil(2000/BinSize));
            tempNoiseFloor = tempNoiseFloor(ceil(2000/BinSize):end-ceil(2000/BinSize));
        end
        
        % Jammers for GLO FBIQ should only be within GLO band: [-4.4485, 3.8860] MHz
        if (isequal(testType, 'FB-IQ: BP3 GLO') || isequal(testType, 'FB-IQ: BP4 GLO'))
            GLOLoLim = find(tempFftPts > -4.4485e6, 1, 'first');
            GLOHiLim = find(tempFftPts < 3.8860e6, 1, 'last');
            tempFftData = tempFftData(GLOLoLim:GLOHiLim);
            tempFftPts = tempFftPts(GLOLoLim:GLOHiLim);
            tempNoiseFloor = tempNoiseFloor(GLOLoLim:GLOHiLim);
        end
        
        try
            % Call jammer processing function
            if GsmBlanking
                BlankingPeriod = 3/650;
                BlankingPulseWidth = BlankingPeriod/8;
            elseif TdscdmaBlanking
                BlankingPeriod = 5000e-6; % in seconds
                BlankingPulseWidth = 675e-6; % in seconds
            elseif TddlteBlanking
                BlankingPeriod = 5000e-6; % in seconds
                BlankingPulseWidth = 2143e-6; % in seconds
            else
                BlankingPeriod = 0;
                BlankingPulseWidth = 0;
            end
            [Jammers,BlankingSidelobes] = gnss_IqTest_JammerProc(...
                tempFftData, tempFftPts, tempNoiseFloor,...
                SigLevelDbm, SigPos_Hz, BlankingPeriod, BlankingPulseWidth, testType);
        catch
            % Do error logging
            print_to_error('gnss_IqTest_JammerProc',...
                VersionNum,...
                FileName,...
                StdFolder,...
                'Unknown exception caught within Matlab.');
            close all hidden;%if exceptions occur, must close all (not subject to AutoClose)
            if ~isdeployed
                rethrow(lasterror)
            end
        end
        
        % Another method (Method 2) to estimate noise figure
        tempFftDataNBJammerNotched = [linFftData(1:SigLoLim); linFftData(SigHiLim:end)];
        tempFftPtsNBJammerNotched  = [fftFreqPts(1:SigLoLim) fftFreqPts(SigHiLim:end)];
        
        % Remove the narrowband jammer (BW<1kHz) when estimating the noise floor
        % This code has been commented out because it has been found that a lot of
        % interference during band 13 LTE traffic is classified as narrowband; however
        % this is thought to be largely an artifact of insufficient data randomization
        % in the connection with an eNodeB emulator.
        %{
        for indxj = 1:length(Jammers),
            if (Jammers(indxj).Type < 3) % NB jammer <1kHz
                % Notch the jammer out
                jammerSampIdxVec = find(tempFftPtsNBJammerNotched == Jammers(indxj).Edge(1)):...
                    find(tempFftPtsNBJammerNotched == Jammers(indxj).Edge(2));
                tempFftDataNBJammerNotched(jammerSampIdxVec) = [];
                tempFftPtsNBJammerNotched(jammerSampIdxVec) = [];
            end
        end
        %}
        
        kT = -174;  % Input thermal noise density [dBm/Hz]
        
        % Estimate the equivalent noise bandwidth for mainlobe (in dBHz)
        % For WBIQ GPS,
        %>> f = linspace(-1023e3,1023e3,1e5);
        %>> p = sinc(f/1023e3).^2;
        %>> 10*log10(trapz(f,p)), we get 59.6548 in dBHz.
        if isequal(testType, 'WB-IQ: GPS')
            Bn = 59.6548;  %Equivalent Noise bandwidth [dB-Hz]
        elseif isequal(testType, 'WB-IQ: BDS')
           Bn = 62.6651 + BdsExcessBn;  %Equivalent Noise bandwidth [dB-Hz] adjusted by the filter shape distortion
        elseif isequal(testType, 'WB-IQ: GLO')
            Bn = 56.6402;
            Bn = Bn + GloExcessBn(GloChanIdx);
        elseif isequal(testType, 'WB-IQ: GAL')
            % Estimate the equivalent noise bandwidth for mainlobe (in dBHz) 
            Bn = 62.3712 + GalExcessBn; %Equivalent Noise bandwidth [dB-Hz] adjusted by the filter shape distortion
        elseif (isequal(testType, 'FB-IQ: BP3 GLO') || isequal(testType, 'FB-IQ: BP4 GLO'))
            Bn = 10*log10(511e3 * 2);
        elseif (isequal(testType, 'FB-IQ: GPS'))
            Bn = 10*log10(1.023e6 * 2);
        end
        
        % Only add the noise within the mainlobe for GPS WBIQ test
        % Currently only apply the Method 2 to the GPS NF estimate
        if (isequal(testType, 'WB-IQ: GPS') || isequal(testType, 'FB-IQ: GPS') )
            mainLobeFreqPts_leftEdge = find(tempFftPtsNBJammerNotched >= -1.023e6, 1, 'first');
            mainLobeFreqPts_rightEdge = find(tempFftPtsNBJammerNotched <= 1.023e6, 1, 'last');
        elseif (isequal(testType, 'WB-IQ: GLO') || isequal(testType, 'FB-IQ: BP3 GLO') || isequal(testType, 'FB-IQ: BP4 GLO'))
            mainLobeFreqPts_leftEdge = find(tempFftPtsNBJammerNotched >= -511e3, 1, 'first');
            mainLobeFreqPts_rightEdge = find(tempFftPtsNBJammerNotched <= 511e3, 1, 'last');
        elseif isequal(testType, 'WB-IQ: BDS') || isequal(testType, 'WB-IQ: GAL')
            mainLobeFreqPts_leftEdge = find(tempFftPtsNBJammerNotched >= -2.046e6, 1, 'first');
            mainLobeFreqPts_rightEdge = find(tempFftPtsNBJammerNotched <= 2.046e6, 1, 'last');
        end
        
        totalNoise_dbc = 10*log10(sum(tempFftDataNBJammerNotched(mainLobeFreqPts_leftEdge:mainLobeFreqPts_rightEdge)));
        
        measuredRFNoiseFigure2 = SigLevelDbm + totalNoise_dbc - (kT + Bn);
        
        % Determine the NF and delta b/w two methods
        measuredRFNoiseFigure = measuredRFNoiseFigure2;
        noiseFigureEstDelta = measuredRFNoiseFigure2 - measuredRFNoiseFigure1;
        
        % ----------- GENERATE PLOTS --------------------------------------------
        figure(13);
        
        % Plot spectrum
        plot( FScale * fftFreqPts, logFftData + SigLevelDbm, '-');
        hold on; grid on;
        if(~isdeployed)
            zoom on; %"zoom" command is instable in the deployed mode, do not use it in the deployed mode.
        end
        
        % Plot jammers failing test
        FailJamType = [];
        for indxj = 1:length(Jammers),
            if (~Jammers(indxj).Pass),
                FailJamType = [FailJamType; Jammers(indxj).Type];
                sampIdxVec = find(fftFreqPts == Jammers(indxj).Edge(1)):...
                    find(fftFreqPts == Jammers(indxj).Edge(2));
                plot(FScale * fftFreqPts(sampIdxVec), logFftData(sampIdxVec) + SigLevelDbm, 'r-');
                plot(FScale * Jammers(indxj).Freq, Jammers(indxj).Pow + SigLevelDbm,...
                    MarkType{Jammers(indxj).Type});
            end
        end
        
        %{
        % Mark blanking sidelobes
        for i = 1:length(BlankingSidelobes)
           plot(FScale*BlankingSidelobes(i).Freq,BlankingSidelobes(i).Pow + SigLevelDbm,'m.')
           plot(FScale*BlankingSidelobes(i).SpecFreq,BlankingSidelobes(i).SpecPow + SigLevelDbm,'k.')
        end
        %}
        
        % Plot and mark signal
        plot(FScale * fftFreqPts(SigPkIdx + [-numSigSamps:+numSigSamps]), logFftData(SigPkIdx + [-numSigSamps:+numSigSamps]) + SigLevelDbm, 'k-');
        plot(FScale * SigPos_Hz, SigLevelDbm, 'kd');
        
        % Plot smooth noise floor
        plot( FScale * fftFreqPts, noiseFloor + SigLevelDbm, 'g-');
        
        % Add plot title, labels, and scale axes
        title( sprintf( 'Power Spectrum %s', FileNameTitle ) );
        xlabel( sprintf( 'Frequency [%s]', FUnits ) );
        ylabel( 'Power Level [dBm]' );
        powerSpectrumMin = min(min(logFftData)+SigLevelDbm-5,-180);
        powerSpectrumMax = max([-110,max([Jammers.Pow])+SigLevelDbm,SigLevelDbm+5]);
        axis([-FScale * FSample/2, FScale * FSample/2, powerSpectrumMin, powerSpectrumMax]);
        
        % Add legend
        writeLegend(FailJamType);
        
        % Show passband for GLO FBIQ
        if (isequal(testType, 'FB-IQ: BP3 GLO') || isequal(testType, 'FB-IQ: BP4 GLO'))
            line(-4.4485e6*FScale*[1 1], [powerSpectrumMin powerSpectrumMax], 'Color', 'm');
            line(3.8860e6*FScale*[1 1], [powerSpectrumMin powerSpectrumMax], 'Color', 'm');
        end
        
        % Add text to plot
        xtext = 0.25 * (FScale*FSample) - FScale*FSample/2;
        ytext = 0.1 * (powerSpectrumMax - powerSpectrumMin) + powerSpectrumMin;
        
        measuredNoiseFloor = mean(noiseFloor(inBandFreqIdx)) + SigLevelDbm - 10*log10(BinSize);
        hndl3 = text( xtext, ytext,...
            sprintf('Est. Sig. Freq. Offset : %.3f KHz\nNoise Floor = %.2f dBm/Hz\nNF = %.2f dB', ...
            SigPos_Hz*1e-3, measuredNoiseFloor, measuredRFNoiseFigure));
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hndl3, strcat(OutputFolder, sprintf('%sIqTestFig.bmp',testType(1:2))),'bmp');
            saveas(hndl3, strcat(OutputFolder, sprintf('%sIqTestFig.fig',testType(1:2))),'fig');
        else
            print(13, '-dpng', strcat(OutputFolder, sprintf('%sIqTestFig.png',testType(1:2))));
        end
        
        % ----------- EVALUATE PASS-FAIL AND GENERATE SUMMARY  ----------------------
        % Jammer Results Header is dependent on Type of collect
        JamResHdr1 = sprintf('%13s%10s%10s%10s%10s\r\n','Frequency','Power','BW','Margin','Pass/');
        if FScale == 1e-6
            JamResHdr2 = sprintf('%13s%10s%10s%10s%10s\r\n','(MHz)','(dBm)','(Hz)','(dB)','Fail');
        else % FScale == 1e-3
            JamResHdr2 = sprintf('%13s%10s%10s%10s%10s\r\n','(kHz)','(dBm)','(Hz)','(dB)','Fail');
        end
        
        % Create Summary file
        fprintf( SUMMARY, 'Input Signal: %.0f kHz, %.0f dBm at antenna input\r\n', SigFreqKhz, SigLevelDbm );
        fprintf( SUMMARY, 'Measured Signal Frequency: %0.3f KHz\r\n', SigPos_Hz*0.001 );
        fprintf( SUMMARY, '\r\nSTD I/Q = %.4f / %.4f\r\n', stdReal, stdImag );
        fprintf( SUMMARY, 'Noise Floor = %.2f dBm/Hz\r\n', measuredNoiseFloor);
        fprintf( SUMMARY, 'Noise Figure = %.2f dB\r\n', measuredRFNoiseFigure );
        if isequal(testType, 'WB-IQ: GLO') && ~(SigFreqKhz == 50 || SigFreqKhz == 100)
            fprintf( SUMMARY, 'Warning! Noise figure accuracy is degraded if the reference tone frequency is not 50 or 100 kHz.\r\n');
        end
        
        TotalJammerPower = 10*log10(sum(10.^([Jammers.Pow]/10)));
        if isempty(Jammers)
            fprintf( SUMMARY, '\r\nNo jammers found\r\n');
        else
            fprintf( SUMMARY, '\r\nFound jammers:\r\n');
            fprintf( SUMMARY, '------------------------------------------------------------\r\n');
            fprintf( SUMMARY, JamResHdr1 );
            fprintf( SUMMARY, JamResHdr2 );
            fprintf( SUMMARY, '------------------------------------------------------------\r\n');
            TestRes = {'Fail','Pass'};
            for indxj = 1:length(Jammers),
                fprintf( SUMMARY, '%13.4f%10.1f%10.0f%10.1f%10s\r\n',...
                    FScale * Jammers(indxj).Freq, Jammers(indxj).Pow + SigLevelDbm,...
                    Jammers(indxj).BW, Jammers(indxj).Margin,...
                    TestRes{Jammers(indxj).Pass+1} );
            end
            fprintf( SUMMARY, '------------------------------------------------------------\r\n');
            fprintf( SUMMARY, 'Total jammer power = %.1f dBm\r\n', TotalJammerPower + SigLevelDbm);
        end
        
        % Print pass/fail results only if using standard (default) signal input
        FlagFail = 0;
        fprintf( SUMMARY, '\r\nPASS/FAIL CHECKS\r\n' );
        freqPpm = abs(SigPos_Hz*0.001 - SigFreqKhz) / OnePpmKHz;
        if (freqPpm < freqPpmLim),
            fprintf( SUMMARY, 'PASS  %s: CW Signal Freq     |  %.2f ppm in %s\r\n', testType, freqPpmLim, nwStr );
        else
            fprintf( SUMMARY, 'FAIL  %s: CW Signal Freq     |  %.2f ppm in %s\r\n', testType, freqPpmLim, nwStr );
            FlagFail = 1;
        end;
        
        if ( measuredRFNoiseFigure < RFNoiseFigureLim),
            fprintf( SUMMARY, 'PASS  %s: Noise Figure       |  %.1f dB in %s\r\n', testType, RFNoiseFigureLim, nwStr );
        else
            fprintf( SUMMARY, 'FAIL  %s: Noise Figure       |  %.1f dB in %s\r\n', testType, RFNoiseFigureLim, nwStr );
            FlagFail = 1;
        end;
        
        % GPS/GLO/BDS jammer pass/fail display
        
        if isequal(testType, 'WB-IQ: BDS') % BDS WBIQ
            if all([Jammers.Pass]) && (TotalJammerPower + SigLevelDbm < -112)
                fprintf( SUMMARY, 'PASS  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer Power < -128 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -128 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -115 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -112 dBm\r\n');
            else
                fprintf( SUMMARY, 'FAIL  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer power < -128 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -128 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -115 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -112 dBm\r\n');
                FlagFail = 1;
            end
        elseif (isequal(testType, 'WB-IQ: GLO') || isequal(testType, 'FB-IQ: BP3 GLO') || isequal(testType, 'FB-IQ: BP4 GLO'))  % GLONASS WBIQ
            if all([Jammers.Pass]) && (TotalJammerPower + SigLevelDbm < -120)
                fprintf( SUMMARY, 'PASS  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer Power < -136 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -136 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -123 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -120 dBm\r\n');
            else
                fprintf( SUMMARY, 'FAIL  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer power < -136 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -136 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -123 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -120 dBm\r\n');
                FlagFail = 1;
            end            
        elseif isequal(testType, 'WB-IQ: GAL') % GAL WBIQ (TBD)
            if all([Jammers.Pass]) && (TotalJammerPower + SigLevelDbm < -124)
                fprintf( SUMMARY, 'PASS  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer Power < -140 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -140 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -127 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -124 dBm\r\n');
            else
                fprintf( SUMMARY, 'FAIL  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer power < -140 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -140 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -127 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -124 dBm\r\n');
                FlagFail = 1;
            end
        else % GPS WBIQ
            if all([Jammers.Pass]) && (TotalJammerPower + SigLevelDbm < -124)
                fprintf( SUMMARY, 'PASS  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer Power < -140 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -140 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -127 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -124 dBm\r\n');
            else
                fprintf( SUMMARY, 'FAIL  %s:  Jammer Test       |  if Jammer BW < 50Hz, Jammer power < -140 dBm,\r\n', testType );
                fprintf( SUMMARY, '                                     |  else if 50Hz < Jammer BW < 1kHz, Jammer power < -140 + 10*log10(BW/50) dBm,\r\n');
                fprintf( SUMMARY, '                                     |  else if 1kHz <= Jammer BW < 10kHz, Jammer power < -127 dBm\r\n');
                fprintf( SUMMARY, '                                     |  Total jammer power < -124 dBm\r\n');
                FlagFail = 1;
            end
        end
        
        if( stdReal >= stdLoLim && stdReal <= stdHiLim && stdImag >= stdLoLim && stdImag <= stdHiLim ),
            fprintf( SUMMARY, 'PASS  %s: I/Q STD Dev.       |  [%.2f:%.2f] in %s\r\n', testType, stdLoLim, stdHiLim, nwStr );
        else
            fprintf( SUMMARY, 'FAIL  %s: I/Q STD Dev.       |  [%.2f:%.2f] in %s\r\n', testType, stdLoLim, stdHiLim, nwStr );
            FlagFail = 1;
        end
        
        % Declare the test as PASS or FAIL
        if( FlagFail == 0 )
            fprintf( SUMMARY, '\r\n-------------------------PASS-------------------------\r\n' );
        else
            fprintf( SUMMARY, '\r\n-------------------------FAIL-------------------------\r\n' );
        end
        
        fclose( SUMMARY );
        fclose('all');
        %===============================================
        %           NB-IQ TEST POST PROCESSING
        %===============================================
    else        
        % Center Frequency
        CenterFreq = NBCenterFreqHz;        
        % Load all 1kHz samples
        iqSamps = fread(fid,inf, 'int8'); fclose(fid);
        % Create summary file
        SUMMARY = fopen(strcat(OutputFolder, 'Summary_IQNB.txt'),'w');
        fprintf( SUMMARY, 'Summary IQ Narrow Band -- gnss_IqTest_IqFileProc Version %s\r\n\r\n', VersionNum );
        % Change into complex data and store in array with size (SamplesPerInteg x NumCollections).
        iqSamps = iqSamps(1:2:end) +  sqrt(-1)*iqSamps(2:2:end);
        %%
        %---------------------------------------------------------------------------------
        % Pre-processing the 1ms NBIQ samples for TDD LTE blanking case
        % for TDD LTE, the blanking period is about 2ms every 5ms. That
        % will make ~40% of the 1ms NBIQ samples close to 0.  Therefore,
        % need to pre-process the samples.  The idea is to identify the
        % location of blanked 1ms, then estimate the frequency near it, and
        % estimate it using a nearby non-blanking sample, then replace the
        % orignal blanked sample with the estimated sample
        % Process in the forward direction, ignore last 10 samples for
        % simplicity: the last 10 samples are not used in later phase or
        % freq estimation anyway
        %---------------------------------------------------------------------------------
        if (TddlteBlanking || GsmTddlteBlanking)
            % any sample with amp < bankingSampThresh is treated as blanking sample            
            sorted_amp = sort(abs(iqSamps(1:500)),'descend');
            % the threshold is set at 10dB below the median amplitude of the largest 20 samples in first 500 samples
            blankingSampThresh = median(sorted_amp(1:20)) * 10^(-10/20);
            for i = 1: length(iqSamps)-10
                if (abs(iqSamps(i)) < blankingSampThresh)
                    % search for the next non-blanking sample
                    for k = i+1: i+10
                        % require 6dB higher than blankingSampThresh to be
                        % declare as non-blanking sample
                        if (abs(iqSamps(k)) > blankingSampThresh * 2)
                            break;
                        end
                    end
                    % At this point, k is the index of the next non-blanked
                    % sample after i
                    % search for the next blanked sample after k
                    for m = k+1: k+10
                        if (abs(iqSamps(m)) < blankingSampThresh)
                            break;
                        end
                    end
                    % At this point, m is the index of the next blanked
                    % sample after k
                    % Therefore iqSamps(k) to iqSamps(m-1) is the close
                    % non-blanked segment after iqSamps(i)
                    iqSeg = iqSamps(k:m-1);
                    %Estimate the freq in that segment
                    segCarrierPh = unwrap( angle( iqSeg )) / 2/ pi;
                    segFreq = (segCarrierPh(end) - segCarrierPh(1))/((length(segCarrierPh)-1) * 1e-3);
                    % replace the orignal blanked iqSamps(i) with the estimate
                    iqSamps(i) = iqSamps(k) * exp(sqrt(-1)*2*pi*segFreq*(i-k)*1e-3);
                end
            end
        end
        % End of the pre-processing NBIQ for TDD-LTE blanking
        %%
        if (length(iqSamps) ~= SamplesPerInteg * NumCollections)
            fprintf( SUMMARY, '\r\nW A R N I N G: FEWER THAN EXPECTED SAMPLES IN LOG FILE !!! \r\n');
            iqSamps = [iqSamps; zeros(length(iqSamps) - SamplesPerInteg * NumCollections,1)];
        end
        iqSamps = reshape(iqSamps,SamplesPerInteg,NumCollections);        
        % Compute the hanning window
        HanningWindow = hanning( SamplesPerInteg );
        
        % Compute the FFT for each collection interval
        y = zeros(SamplesPerInteg,1);
        
        for idx_nc = 1:NumCollections
            % Incoherently accummulate multipleI/Q collects in the frequency
            % domain. y is initialized on the first pass
            y = y + abs( fft( iqSamps(:,idx_nc) .* HanningWindow ) ).^2;
        end
        
        % Need to <ESC> underscores with a '\' if we want to avoid incorrectly
        % subscripting these in the title.
        FileNameTitle = strrep( FileName, '_', '\_' );
        
        I_Samples = real(iqSamps); Q_Samples = imag(iqSamps);
        
        std_realx = std( I_Samples ); std_imagx = std( Q_Samples );
        
        % Plot the results only in the standalone mode; do not plot when deployed in QDART.
        if(~isdeployed)
            % FIGURE 1 and 2: The Magnitude of I/Q samples v.s. sample index;
            figure(1);
            plot(I_Samples);
            grid on; zoom on;
            
            title( sprintf( 'Time Domain (REAL) %s', FileNameTitle ) );
            xlabel( 'Sample Count'); ylabel( 'Amplitude' );
            
            axistext = axis;
            xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
            ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
            text( xtext, ytext, sprintf( 'Mean = %.3f    Std = %.3f', mean( I_Samples ), std_realx))
            
            figure(2); plot(Q_Samples); grid on; zoom on;
            
            title( sprintf( 'Time Domain (IMAGINARY) %s', FileNameTitle ) );
            xlabel( 'Sample Count'); ylabel( 'Amplitude' );
            
            axistext = axis;
            xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
            ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
            text( xtext, ytext, sprintf( 'Mean = %.3f    Std = %.3f', mean( Q_Samples ), std_imagx))
        end
        
        % Initialize parameters
        FSample = 1000;     % Hz
        FScale = 1; FUnits = 'Hz';
        %%
        % Figure 3: Plot spectrum
        f = (FSample * [0:length(y)-1] / length(y)) - (FSample / 2);
        f = f'; fftlinear = fftshift( y );
        [peak_signal, peak_index ] = max( fftlinear );
        % normalize the spectrum w.r.t. the peak power;
        fftlinear = fftlinear / peak_signal;
        % LOG domain;
        fftplot = 10*log10( fftlinear );        
        figure(3); plot( FScale * f, fftplot ); grid on;
        if(~isdeployed)
            zoom on; %"zoom" command is instable in the deployed mode, do not use it in the deployed mode.
        end        
        title( sprintf( 'Power Spectrum %s', FileNameTitle ) );
        xlabel( sprintf( 'Frequency [%s]', FUnits ) ); ylabel( 'Level [dB]' );
        Power_Spectrum_min = -100; Power_Spectrum_max = max(0,max(fftplot));
        axis([-FScale * FSample/2, FScale * FSample/2, Power_Spectrum_min, Power_Spectrum_max]);
        hold on;
        %---------------------------------------------------------------------------------
        % Compute the mean noise. For the Narrow band spectrum,
        % mean noise is computed away from the main peak (which could span multiple bins
        % due to phase noise). Remove 1000 bins from either side of the peak to compute
        % noise mean.
        %---------------------------------------------------------------------------------
        % Left-hand-side
        noise_index = peak_index - 1000;
        if( noise_index > 1 )
            sum_noise = sum( fftlinear( 1:noise_index)); n_noise = noise_index;
        else
            sum_noise = 0; n_noise = 0;
        end;
        % Right-hand-side
        noise_index = peak_index + 1000;
        if( noise_index < length(fftlinear) )
            sum_noise = sum_noise + sum( fftlinear( noise_index:length(fftlinear)));
            n_noise = n_noise + length(fftlinear) - noise_index + 1;
        end;
        % Average noise;
        mean_noise = sum_noise / n_noise;
        db_mean_noise = 10*log10( mean_noise );
        %---------------------------------------------------------------------------------
        % For Narrow-band collects, compute the Phase Noise
        % Filtered fftplot (in dB) will be used to determine the upper and
        % lower frequency points for the phase noise calculation. This
        % really could do with some optimization.
        %---------------------------------------------------------------------------------
        b = ones(1,20);
        filtered_fftplot = filter( b/sum(b), 1, fftplot );
        
        % Identify the samples that are going to be included in the
        % phase noise calculation.        
        [peak_signal, peak_index ] = max( fftlinear );        
        high_band = filtered_fftplot( peak_index: length(fftlinear)  );
        low_band = filtered_fftplot( 1: peak_index );        
        high_band_index = find( high_band < db_mean_noise );
        low_band_index = find( low_band < db_mean_noise );
        include_index = [ low_band_index( length( low_band_index )): peak_index + high_band_index(1) ];
        
        hi_signal_index = peak_index + 40;
        lo_signal_index = peak_index - 40;
        hi_noise_index = include_index(length( include_index ));
        lo_noise_index = include_index(1);
%         
%         v = axis;
%         % Plot a red box that contains the signal
%         %plot( [f(lo_signal_index), f(lo_signal_index)], [v(3), v(4)], 'r' );
%         %plot( [f(hi_signal_index), f(hi_signal_index)], [v(3), v(4)], 'r' );
%         
%         if(0)  % Skip this. Use the alternate method for phase noise
%             signal_power = sum( fftlinear( lo_signal_index : hi_signal_index ) );
%             noise_power = sum( fftlinear( lo_noise_index : hi_noise_index ) ) - signal_power;
%             
%             if noise_power > 0
%                 nb_phase_noise = 180/pi * sqrt( 2 * noise_power / signal_power );
%             else
%                 nb_phase_noise = 0;
%             end
%         end
            %---------------------------------------------------------------------------------
            %  Phase noise calculation method:
            %  The idea is (f x t + Q_1) - (f x t + Q_2) = Q_1 - Q_2, where f is the desired frequency and Q is the phase noise
            %---------------------------------------------------------------------------------
            % Calculate the carrier phase of each sample
            CarrierPhase = unwrap( angle( iqSamps )) / 2/ pi;
            % The length of the I/Q samples
            Lcf=length(CarrierPhase);
            % Freq difference between 10 Hz and 1 kHz is equivalent to phase difference
            % 10 Hz is chosen because HW applies XO compensation every 100 ms;
            DeltaFreq([1:Lcf-100])=(((CarrierPhase([101:Lcf])-CarrierPhase([1:Lcf-100]))*0.01)- ...
                (CarrierPhase([51:Lcf-50])-CarrierPhase([50:Lcf-51])))*FSample;
            % Phase noise from cycles to deg, scale by root 2,per sample (kHz)
            nb_phase_noise=std(DeltaFreq)*360/1.414/FSample;
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.2 * (axistext(4) - axistext(3)) + axistext(3);
        
        % If noise floor is too close to bottom of plot, move text to the top.
        % 0.6 is a rough threshold gotten by observing several plots.
        if( (db_mean_noise - Power_Spectrum_min) < 0.6*(Power_Spectrum_max - Power_Spectrum_min) )
            ytext = 0.8 * (axistext(4) - axistext(3)) + axistext(3);
        end
        hndl3 = text( xtext, ytext, sprintf( 'Center Frequency = %.3f %s\nPeak Energy Frequency Offset = %.3f %s\nNoise Floor = %.1f dB\nPhase Modulation = %.1f Degrees', ...
            CenterFreq, FUnits, FScale * f( peak_index ), FUnits, db_mean_noise, nb_phase_noise));
        hold off;
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hndl3,strcat(OutputFolder, 'NBIqTestFig3.bmp'),'bmp');
            saveas(hndl3,strcat(OutputFolder, 'NBIqTestFig3.fig'),'fig');
        else
            print(3, '-dpng', strcat(OutputFolder, 'NBIqTestFig3.png'));
        end
        
        % Create Summary file
        TdF = f; TdY = fftplot;
        mask = [lo_noise_index:hi_noise_index];
        mask([find(mask<1) find(mask>length(TdF))]) = [];
        TdF(mask) = []; TdY(mask) = [];
        [TdVal, TdX] = max(TdY);
        
        fprintf( SUMMARY, 'Input Signal: 100 kHz, -110 dBm at antenna input\r\n' );
        fprintf( SUMMARY, 'Center Frequency = %.3f %s\r\nPeak Energy Frequency Offset = %.3f %s\r\nNoise Floor = %.1f dB\r\nPhase Modulation = %.1f Degrees\r\n', ...
            CenterFreq, FUnits, FScale * f( peak_index ), FUnits, ...
            db_mean_noise, nb_phase_noise);
        fprintf( SUMMARY, 'STD I/Q = %.1f / %.1f\r\n', std_realx, std_imagx );
        %%
        % Figure 4: Frequency jump on 1KHz samples
        % Decimate 1 minus Decimate 100
        Lcf=length(CarrierPhase);
        % Freq difference between 10 Hz and 1 kHz is equivalent to phase
        % difference, unit is Hz
        DeltaFreq([1:Lcf-100])=((CarrierPhase([51:Lcf-50])-CarrierPhase([50:Lcf-51]))- ...
            ((CarrierPhase([101:Lcf])-CarrierPhase([1:Lcf-100]))*0.01))*FSample;
        % Plot 1kHz plot
        TimeScale = [50:length(DeltaFreq)+49] / FSample;
        figure(4); plot( TimeScale,  DeltaFreq); grid on;
        if(~isdeployed)
            zoom on; %"zoom" command is instable in the deployed mode, do not use it in the deployed mode.
        end        
        title( sprintf( 'Frequency Difference from 1 kHz Samples %s', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Freq [Hz]' );
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        % Phase noise from cycles to deg, scale by root 2, per sample (kHz)
        % This is the phase noise on 1kHz plot
        hndl4 = text( xtext, ytext, sprintf( 'Std = %.3f    Phase noise = %.1f deg', std(DeltaFreq), std(DeltaFreq)*360/1.414/FSample));
        if(~isdeployed)
            zoom on; %"zoom" command is instable in the deployed mode, do not use it in the deployed mode.
        end        
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hndl4, strcat(OutputFolder, 'NBIqTestFig4.bmp'),'bmp'); %save BMP and FIG files when in the standalone mode
            saveas(hndl4, strcat(OutputFolder, 'NBIqTestFig4.fig'),'fig');
        else
            print(4, '-dpng', strcat(OutputFolder, 'NBIqTestFig4.png')); %save PNG file when in the deployed mode (QDART)
        end
        fprintf( SUMMARY, 'Freq difference from 1 kHz samples (max/min) = %.3f/%.3f Hz\r\n', ...
            max(DeltaFreq), min(DeltaFreq) );
        %%
        % Figure 5: the 50Hz Delta Freq from the 20ms I/Q sequence with the largest Delta Freq
        % Calculate instant freq using 10Hz samples (100ms away)
        InstantFreq([1:Lcf-100]) = (CarrierPhase([101:Lcf]) - CarrierPhase([1:Lcf-100]))*0.01*FSample;
        %---------------------------------------------------------------------------------
        % Calculate the local phase. Phase is the integration of freq over time
        % don't care the absolute phase, thus can set the initial phase to
        % arbitrary InstantFreq(1) points to the instant freq at t=51 samples
        %---------------------------------------------------------------------------------
        localPhase = 0.001 * cumsum(InstantFreq(1:Lcf-100));
        %---------------------------------------------------------------------------------
        % Use the localPhase to de-rotate all 1ms (1kHz) samples
        % Note the localPhase(1) points to the instant freq at k=51
        %---------------------------------------------------------------------------------
        derotated_cmplx(51 : Lcf-50) = iqSamps(51 : Lcf-50) .* exp( -2.0 * pi * sqrt(-1) * (localPhase(1:Lcf-100))');
        %---------------------------------------------------------------------------------
        % Sum 20 of de-roated 1ms samples to construct 20ms I/Q
        % 20 different bit edges are calculated
        % Then estimate the unwrapped phase of each 20ms I/Q sequences
        % Then calculate the phase jump between consecutive 20ms I/Q in each
        % sequence, Divide by 0.02 (times 50) to get the Delta Freq
        %---------------------------------------------------------------------------------
        for bitedge = 1 : 20
            for j = 4 : (floor((Lcf-100)/20)-1)
                constructed_20ms_IQ(bitedge,j) = sum(derotated_cmplx( (20*(j-1) + bitedge) : (20*j + bitedge - 1) ));
            end
            CarrierPhase_20ms(bitedge,:) = unwrap( angle( constructed_20ms_IQ(bitedge,4:(floor((Lcf-100)/20)-1)) )) / 2/ pi;
            DeltaFreq_20ms(bitedge,:) = diff(CarrierPhase_20ms(bitedge,:)) * 50;
        end
        
        % Find the max Delta Freq and corresponding bit edge index in all of 20ms I/Q sequences
        maxDeltaFreq = 0; maxDeltaFreqBitEdgeIndex = 0;
        for bitedge = 1 : 20
            if (maxDeltaFreq < max(abs(DeltaFreq_20ms(bitedge,:))))
                maxDeltaFreq = max(abs(DeltaFreq_20ms(bitedge,:)));
                maxDeltaFreqBitEdgeIndex = bitedge;
            end
        end
        
        % Obtain the 20ms I/Q data w/ max Delta Freq
        maxDeltaFreq_20ms = DeltaFreq_20ms(maxDeltaFreqBitEdgeIndex,:);
        
        TimeScale = 0.02 * (1:length(maxDeltaFreq_20ms));
        
        % StdDeltaDeltaPhase=std(maxDeltaFreq_20ms);
        MinFreqDiffFrom50hz = min(maxDeltaFreq_20ms);
        MaxFreqDiffFrom50hz = max(maxDeltaFreq_20ms);
        
        % Plot the 50Hz Delta Freq from the 20ms I/Q sequence with the largest
        % Delta Freq
        figure(5); plot(TimeScale, maxDeltaFreq_20ms); grid on;
        title( sprintf( 'Frequency Difference from 50 Hz Samples %s', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Freq [Hz]' );
        
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        % Phase noise from cycles to deg, scale by root 2, per sample (kHz)
        hnd15 = text( xtext, ytext, sprintf( 'Std = %.3f    Phase noise = %.1f deg', std(maxDeltaFreq_20ms), std(maxDeltaFreq_20ms)*360/1.414/FSample));
        if(~isdeployed)
            zoom on; %"zoom" command is instable in the deployed mode, do not use it in the deployed mode.
        end
        
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hnd15,  strcat(OutputFolder, 'NBIqTestFig5.bmp'),'bmp');
            saveas(hnd15,  strcat(OutputFolder, 'NBIqTestFig5.fig'),'fig');
        else
            print(5, '-dpng', strcat(OutputFolder, 'NBIqTestFig5.png'));
        end
        % Figure 6
        % Plot the derotated 1kHz samples I/Q
        % Because the derotation starts from symbol index=51 and end at symbol
        % index=Lcf-50, plot that range
        figure(6)
        plot((51:Lcf-50)/FSample, real(derotated_cmplx(51:Lcf-50)))
        hold on
        plot((51:Lcf-50)/FSample, imag(derotated_cmplx(51:Lcf-50)),'r')
        hold off
        grid on;
        title( sprintf( 'Time Domain 1kHz Samples after de-rotation %s', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Amp' );
        
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        hnd16 = text( xtext, ytext, sprintf( 'blue: Real  red: Imag'));
        if(~isdeployed)
            zoom on; %"zoom" command is instable in the deployed mode, do not use it in the deployed mode.
        end
        
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hnd16,  strcat(OutputFolder, 'NBIqTestFig6.bmp'),'bmp');
            saveas(hnd16,  strcat(OutputFolder, 'NBIqTestFig6.fig'),'fig');
        else
            print(6, '-dpng', strcat(OutputFolder, 'NBIqTestFig6.png'));
        end
        % Figure 7
        % Plot the amplitude of the 1kHz samples sqrt(I*I+Q*Q)
        
        Amplitude = abs( iqSamps ); Mean_Amp = mean(Amplitude);
        TimeScale = [0:length(Amplitude)-1] / FSample;
        figure(7); plot(TimeScale(1:end-10), Amplitude(1:end-10)); grid on;
        title( sprintf( 'Time Domain 1kHz Samples Amplitude %s', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Amp' );
        
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        hnd17 = text( xtext, ytext, sprintf( 'Mean Amplitdue = %.3f ', Mean_Amp));
        if(~isdeployed)
            zoom on; %"zoom" command is instable in the deployed mode, do not use it in the deployed mode.
        end
        
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hnd17,  strcat(OutputFolder, 'NBIqTestFig7.bmp'),'bmp');
            saveas(hnd17,  strcat(OutputFolder, 'NBIqTestFig7.fig'),'fig');
        else
            print(7, '-dpng', strcat(OutputFolder, 'NBIqTestFig7.png'));
        end
        
        fprintf( SUMMARY, 'Freq difference from 50 Hz samples (max/min) = %.3f/%.3f Hz\r\n', ...
            MaxFreqDiffFrom50hz, MinFreqDiffFrom50hz );
        % output the mean amplitude
        fprintf( SUMMARY, 'Mean amplitude of 1 kHz samples = %.3f \r\n', Mean_Amp);
        
        %----------------------------------------------------------------
        %   NB-IQ FREQ and FREQ DRIFT ESTIMATION
        %----------------------------------------------------------------
        % processing
        % angle(Z) returns the phase angles, in radians, for each element of complex array Z
        % unwrap(P) corrects the radian phase angles in a vector P by adding multiples of 2 pi
        % when absolute jumps between consecutive elements of P are greater than or equal to the
        % default jump tolerance of pi radians.
        phaseVals = unwrap(angle(iqSamps)) / (2*pi);    % generate phase values
        
        lenPhVal = length(phaseVals);
        
        % estimate the freq at t by using the phase values between t-n*Ts
        % and t+n*Ts. Ts = 1ms, choose n = 10 so it is small such that the
        % frequency during this 20ms period is constant (piece-wise linear).
        % Perform linear least square estimation for the freq.  Since we
        % choose the time range to be -10ms to 10ms, the linear least
        % square is signficantly simplified
        n = 10;
        nSeq = [-n:n].'/ FSample;
        tSum = sum(nSeq.' * nSeq);
        
        % Estimate the freq at each time = idx
        for idx = n+1 : lenPhVal-n
            phSeq = phaseVals(idx + [-n:n]);
            estFreq(idx-n) = sum(nSeq.' * phSeq) / tSum;
        end
        
        freqEst1stmp = filter(ones(1,1000)/1000,1,estFreq);
        freqEst1s = freqEst1stmp(1000:length(freqEst1stmp)-1000);
        
        % Assume linear drift to get the upper bound de-sense estimate
        for k=1:length(freqEst1s)-12000
            freqDrift12s(k) = (max(freqEst1s(k:12000+k)) - min(freqEst1s(k:12000+k)))/12;
        end
        
        % Estimate the frequency drift at t by using the estimated
        % frequency values between t-m*Ts and t+m*Ts.  Ts=1ms, choose m=50
        % so it is small such that the frequency drift during this 50ms
        % period is constant (piece-wise linear).  Perform linear least
        % square estimation for the frequency drift.  Since we
        % choose the time range to be -50ms to 50ms, the linear least
        % square is signficantly simplified
        m = 50;
        mSeq = [-m:m].'/ FSample;
        mtSum = sum(mSeq.' * mSeq);
        % Find out the number of the recorded frequency values;
        lenFreqVal = length(estFreq);
        % Estimate the freq drift at each time = idx
        for idx = m+1 : lenFreqVal-m
            freqSeq = estFreq(idx + [-m:m]);
            estDrift(idx-m) = sum(mSeq.' * freqSeq') / mtSum;
        end
        
        % Prepare instantaneous freq estimate vs. time (fig.8)
        % represent in orig phase sample time scale
        Tvec = [n+1 : lenPhVal-n] / FSample;
        
        % Re-process estFreq vector and time vector here
        % don't give freq estimate if there is a phase jump during the estimation window.
        % mark these data points as very negative value (invalid value)
        for i = 1:lenPhVal-100
            % compare 50Hz freq estimate to 50Hz, pay attention to the
            % alignment (MUST have the same index arrangment as in this
            % Matlab code
            if abs(DeltaFreq(i)) > 50 % large phase jump
                estFreq(40+i-25:40+i+25) = -10000;
                Tvec(40+i-25:40+i+25) = -10000;
            end
        end
        
        % Remove the data that is marked as -10000
        estFreq(find(estFreq<-5000)) = [];
        Tvec(find(Tvec<-5000)) = [];
        % Figure 8 : estimated instantaneous frequency estiamte
        figure(8)
        % Plot instant freq vs. time, throw out small portion at the
        % beginning and the end, due to phase jump concern + time margin
        plot(Tvec(50:length(Tvec)-100), estFreq(50:length(Tvec)-100))
        grid on;
        title( sprintf( 'Tone Frequency vs. time %s', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Frequency (Hz)' );
        
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        hnd18 = text( xtext, ytext, '');
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hnd18, strcat(OutputFolder, 'NBIqTestFig8.bmp'),'bmp');
            saveas(hnd18, strcat(OutputFolder, 'NBIqTestFig8.fig'),'fig');
        else
            print(8, '-dpng', strcat(OutputFolder, 'NBIqTestFig8.png'));
        end
        
        % Prepare instantaneous freq drift estimate vs. time (fig.10)
        % represent in orig phase sample time scale
        Tvec = [m+n+1 : lenPhVal-m+n] / FSample;
        
        % Re-process estDrift vector and time vector here
        % don't give drift estimate if there is a phase jump during the estimation window.
        % mark these data points as very negative value (invalid value)
        for i = 211:lenPhVal-320
            % compare 50Hz freq estimate to 50Hz, pay attention to the
            % alignment (MUST have the same index arrangment as in this
            % Matlab code
            if abs(DeltaFreq(i)) > 50  % large phase jump
                estDrift(i-10-130:i-10+130) = -10000;
                Tvec(i-10-130:i-10+130) = -10000;
            end
        end
        
        % Remove the number that is marked as -10000
        estDrift(find(estDrift<-5000)) = [];
        Tvec(find(Tvec<-5000)) = [];
        
        % 12s drift estimate don't need re-process, plot it directly
        figure(9)
        plot((1:length(freqDrift12s))/FSample, freqDrift12s)
        grid on;
        title( sprintf( 'Tone Frequency Drift 12s vs. time %s for Worst-case De-sense Evaluation', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Frequency Drift (Hz/s)' );
        
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        hnd19 = text( xtext, ytext, '');
        
        % Save figure for test documentation
        if(~isdeployed)
            saveas(hnd19,  strcat(OutputFolder, 'NBIqTestFig9.bmp'),'bmp');
            saveas(hnd19,  strcat(OutputFolder, 'NBIqTestFig9.fig'),'fig');
        else
            print(9, '-dpng', strcat(OutputFolder, 'NBIqTestFig9.png'));
        end
        
        figure(10)
        % Plot instant freq vs. time, throw out small portion at the
        % beginning and the end, due to phase jump concern + time margin
        plot(Tvec(100:length(Tvec)-200), estDrift(100:length(Tvec)-200))
        grid on;
        title( sprintf( 'Tone Frequency Drift vs. time %s', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Frequency Drift (Hz/s)' );
        
        maxDrift = max(abs(estDrift));
        maxDrift12s = max(freqDrift12s);
        fprintf( SUMMARY, 'Max instantaneous frequency drift = %.3f \r\n', maxDrift);
        fprintf( SUMMARY, 'Max 12 frequency drift = %.3f \r\n', maxDrift12s);
        
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        hnd20 = text( xtext, ytext, '');
        if(~isdeployed)
            saveas(hnd20,  strcat(OutputFolder, 'NBIqTestFig10.bmp'),'bmp');
            saveas(hnd20,  strcat(OutputFolder, 'NBIqTestFig10.fig'),'fig');
        else
            print(10, '-dpng', strcat(OutputFolder, 'NBIqTestFig10.png'));
        end
        
        % plot and save the unwrapped carrier phase (cycles) vs. time plot
        Tvec1 = [1 : lenPhVal] / FSample;
        figure(11)
        plot(Tvec1, phaseVals ,'.')
        grid on;
        title( sprintf( 'Tone Carrier Phase vs. time %s', FileNameTitle ));
        xlabel( 'Secs'); ylabel( 'Carrier Phase (cycle)' );
        
        axistext = axis;
        xtext = 0.05 * (axistext(2) - axistext(1)) + axistext(1);
        ytext = 0.05 * (axistext(4) - axistext(3)) + axistext(3);
        hnd21 = text( xtext, ytext, '');
        if(~isdeployed)
            saveas(hnd21,  strcat(OutputFolder, 'NBIqTestFig11.bmp'),'bmp');
            saveas(hnd21,  strcat(OutputFolder, 'NBIqTestFig11.fig'),'fig');
        else
            print(11, '-dpng', strcat(OutputFolder, 'NBIqTestFig11.png'));
        end
        
        %----------------------------------------------------------------------
        FlagFail = 0;
        fprintf( SUMMARY, '\r\nPASS/FAIL CHECKS\r\n' );
        
        if GsmBlanking || GsmTdscdmaBlanking || GsmTddlteBlanking % GSM Traffic
            nb_phase_noise_mask = 15;
        elseif TdscdmaBlanking % TD-SCDMA blanking
            nb_phase_noise_mask = 15;
        elseif TddlteBlanking % TDD-LTE blanking
            nb_phase_noise_mask = 10;
        else
            nb_phase_noise_mask = 6;
        end
        % Pass/Fail check on phase noise;
        if( nb_phase_noise < nb_phase_noise_mask )
            fprintf( SUMMARY, 'PASS  NB Phase Noise Modulation           |  < %2u deg\r\n', nb_phase_noise_mask );
        else
            fprintf( SUMMARY, 'FAIL  NB Phase Noise Modulation           |  < %2u deg\r\n', nb_phase_noise_mask );
            FlagFail = 1;
        end
        % Pass/Fail check on 1Khz      
        num_largefreqdiff1khz = sum(abs(DeltaFreq) > Thres_MaxFreqDiffFrom1khz);
        fmt_pass = sprintf('PASS  %4.0f instances with absolute freq difference larger than %2.1f Hz from 1KHz samples  |  Limit: < %2.0f instances within 60 seconds \r\n', num_largefreqdiff1khz,...
            Thres_MaxFreqDiffFrom1khz, Thres_num_largefreqdiff1khz);
        fmt_fail = sprintf('FAIL  %4.0f instances with absolute freq difference larger than %2.1f Hz from 1KHz samples  | Limit: < %2.0f instances within 60 seconds \r\n',  num_largefreqdiff1khz,...
            Thres_MaxFreqDiffFrom1khz, Thres_num_largefreqdiff1khz);
        if num_largefreqdiff1khz < Thres_num_largefreqdiff1khz
            fprintf( SUMMARY, fmt_pass);
        else
            fprintf( SUMMARY, fmt_fail);
            FlagFail = 1;
        end
        % Pass/Fail check on 50hz
        fmt_pass = sprintf('PASS  Freq difference from 50 Hz samples  |  < +/-  %2.1f Hz \r\n', Thres_MaxFreqDiffFrom50hz);
        fmt_fail = sprintf('FAIL  Freq difference from 50 Hz samples  |  < +/-  %2.1f Hz \r\n', Thres_MaxFreqDiffFrom50hz);
        if( MaxFreqDiffFrom50hz < Thres_MaxFreqDiffFrom50hz & MinFreqDiffFrom50hz > -Thres_MaxFreqDiffFrom50hz)
            fprintf( SUMMARY, fmt_pass);
        else
            fprintf( SUMMARY, fmt_fail);
            FlagFail = 1;
        end        
        %--------------------------------------------------------------------
        % NEW DRIFT ESTIMATION PASS FAIL (currently not enabled)
        %--------------------------------------------------------------------
        if( max(abs(estDrift)) < 150)
            fprintf( SUMMARY, 'PASS  Instantaneous Drift                 |  < +/- 150 Hz/s \r\n' );
        else
            fprintf( SUMMARY, 'FAIL  Instantaneous Drift                 |  < +/- 150 Hz/s \r\n' );
            FlagFail = 1;
        end
        
        %
        
        %---------------------------------------------------------------------
        
        if( FlagFail == 0 )
            fprintf( SUMMARY, '\r\n-------------------------PASS-------------------------\r\n' );
        else
            fprintf( SUMMARY, '\r\n-------------------------FAIL-------------------------\r\n' );
        end
        fclose( SUMMARY );
    end;
    
    hold off;
    %--------------------------------------------------------------------------
    %           PERFORM AUTOCLOSE
    %--------------------------------------------------------------------------
    if (AutoClose == 1)
        close all;
    end;
    fclose('all');
    ret_result = 1;
catch
    %do error logging and handling...
    ret_result = 0;
    print_to_error('gnss_IqTest_IqFileProc',...
        VersionNum,...
        FileName,...
        StdFolder,...
        'Unknown exception caught within Matlab.');
    close all hidden;%if exceptions occur, must close all (not subject to AutoClose)
    if ~isdeployed
        rethrow(lasterror)
    end
end

% ---------------------------------------------------------------------------------------------
%    LINEAR INTERPOLATION
% ---------------------------------------------------------------------------------------------
function yp = lininterp(x,y,xp)
% SYNTAX        yp = lininterp(x,y,xp)
% CALCULATES THE VALUE YP USING LINEAR INTERPOLATION BETWEEN (X1,Y1) AND
% (X2,Y2), WHERE X1 AND X2 ARE THE CLOSEST POINTS IN VECTOR X SUCH THAT
% X1 <= X <= X2. IF XP IS A VECTOR, THIS PROCESS IS REPEATED FOR EACH
% ELEMENT IN XP.
% NOTE: X SHOULD BE SORTED IN *ASCENDING* ORDER
LoIdx = zeros(size(xp));

for idx = 1:length(xp),
    x1_idx = max(find(x < xp(idx)));
    
    if (isempty(x1_idx) || x1_idx == length(x)),
        if xp(idx) == x(1),
            LoIdx(idx) = 1;
        else
            error('xp value outside range of x\n');
        end
    else
        LoIdx(idx) = x1_idx;
    end
end

HiIdx = LoIdx + 1;
yp = y(LoIdx) + ((y(HiIdx) - y(LoIdx))./(x(HiIdx) - x(LoIdx))).*(xp - x(LoIdx));

% ---------------------------------------------------------------------------------------------
%    ADD LEGEND TO SPECTRUM PLOT
% ---------------------------------------------------------------------------------------------
function dum = writeLegend(FailJamType)
% get all the line handles
plotObjHand = flipud(get(gca,'Children'));
legendStr = cell(5+length(FailJamType),1);
handAr = [];

%Spectrum
legendStr{length(handAr)+1} = 'Spectrum';
handAr = [handAr; 1];

% NB Jammers
Temp = find(FailJamType == 1);
if (~isempty(Temp)),
    legendStr{length(handAr) + 1} = 'Jammer: BW \leq 50Hz';
    handAr = [handAr; 2 * min(Temp) + 1];
end;

% less than 1kHz Jammers
Temp = find(FailJamType == 2);
if (~isempty(Temp)),
    legendStr{length(handAr) + 1} = 'Jammer: 50Hz < BW \leq 1 KHz';
    handAr = [handAr; 2 * min(Temp) + 1];
end;

% 1k to 10k jammers
Temp = find(FailJamType == 3);
if (~isempty(Temp)),
    legendStr{length(handAr) + 1} = 'Jammer: 1 KHz < BW \leq 10 KHz';
    handAr = [handAr; 2 * min(Temp) + 1];
end;

% WB Jammers
Temp = find(FailJamType == 4);
if (~isempty(Temp)),
    legendStr{length(handAr) + 1} = 'Jammer: BW > 10 KHz';
    handAr = [handAr; 2 * min(Temp) + 1];
end;

% Signal Tone
legendStr{length(handAr) + 1} = 'Input Tone';
handAr = [handAr; length(plotObjHand)-2];

legendStr{length(handAr) + 1} = 'Effective CW Input';
handAr = [handAr; length(plotObjHand)-1];

legendStr{length(handAr) + 1} = 'Noise Floor Estimate';
handAr = [handAr; length(plotObjHand)];

legStrFinal = cell(length(handAr),1);
for indxl = 1:length(handAr), legStrFinal{indxl} = legendStr{indxl}; end;

legend(plotObjHand(handAr),legStrFinal,'Location','NorthWest','Fontsize',6);

% =============================================================================================
% WBIQ/FBIQ TEST JAMMER PROCESSING SCRIPT FOR GPSONE GEN 8 CALLISTO ENGINE
% =============================================================================================
%
% [Jammers,BlankingSidelobes] = gnss_IqTest_JammerProc(SpurY,SpurF,NoiseFlr,CWTonePow,CWToneFreq,BlankingPeriod,BlankingPulseWidth)
%
% INPUTS:
%
%   SpurY              : Power spectrum in dB/bin, scaled so the CW reference tone is 0 dB.
%   SpurF              : Power spectrum bin centers in Hz.
%   NoiseFlr           : Smooth noise floor in dB/bin.
%   CWTonePow          : Power of CW reference tone in dBm.
%   CWToneFreq         : Frequency of CW reference tone in Hz.
%   BlankingPeriod     : Period in seconds of blanking signal. Set to 0 if there is no blanking.
%   BlankingPulseWidth : Duration in seconds of each blanking pulse.
%
% OUTPUTS:
%
%   Jammers            : Structure array of jammer data.
%   BlankingSidelobes  : Structure array of blanking sidelobe data.
%
%   Both structures have the following fields in common:
%
%   Pow  : spur power in dBc
%   Freq : spur frequency in Hz
%   BW   : spur bandwidth in Hz
%   Edge : spur frequency end-points in Hz
%
%   The Jammers structure contains the following additional fields:
%
%   Type   : jammer type, classified by bandwidth
%   Margin : margin in dB to the max power allowed for this jammer type
%   Pass   : flag to indicate if the jammer passes or fails
%
% =============================================================================================
%
% REVISION HISTORY
% ---------------------------------------------------------------------------------------------
% When        Who       What
% ---------------------------------------------------------------------------------------------
% 2013/08/15    tianqiw New jammer mask for BDS ang GLO
% 2012/02/21  tpals     New blanking sidelobe mask
%                       Report blanking sidelobes separately from jammers
%                       Report passing margin for all jammers
%                       Apply -124 dBm test to each wideband jammer individually
%                       Move check on total jammer power into gnss_IqTest_IqFileProc
% 2008/12/02  sibasish  New jammer BW classification and associated pass/fail criteria
%
% =============================================================================================

function [Jammers,BlankingSidelobes] = gnss_IqTest_JammerProc(SpurY,SpurF,NoiseFlr,CWTonePow,CWToneFreq,BlankingPeriod,BlankingPulseWidth, testType)
%--------------- PARAMETERS -----------------------------------
MaxNoJammers = 20;                  % max no. of jammers reported
NoiseBandTol = 0;                        % one-sided width of noise band in dB
FreqTol = 16;                                 % frequency tolerance (in Hz) for blanking sidelobe check
PowTol = 4;                                    % power tolerance (in dB) for blanking sidelobe check
if isequal(testType, 'WB-IQ: BDS')
    NBJammerSpec_dBm = -128;         % NB jammer spec for BDS signals
    MBJammerSpec_dBm = -121;        % MB jammer spec; the MB jammer based on the bin spread is -115 then tightened by 6 dB to align with BDS WB jammer
    WBJammerSpec_dBm = -121;        % WB jammer spec; this WB jammer targets at lowering the noise figure by 0.2 dB (0.2 dB is the legacy desensitization spec from CDMA noise emission)
elseif isequal(testType, 'WB-IQ: GLO') ||  isequal(testType, 'FB-IQ: BP3 GLO') || isequal(testType,  'FB-IQ: BP4 GLO')
    NBJammerSpec_dBm = -136;         % NB jammer spec for GLO signals
    MBJammerSpec_dBm = -127;        % MB jammer spec; the MB jammer based on the bin spread is -123 then tightened by 4 dB to align with GLO WB jammer
    WBJammerSpec_dBm = -127;        % WB jammer spec; this WB jammer targets at lowering the noise figure by 0.2 dB
elseif isequal(testType, 'WB-IQ: GAL') % TBD
    NBJammerSpec_dBm = -140;         % NB jammer spec for GAL signals
    MBJammerSpec_dBm = -127;        % MB jammer spec
    WBJammerSpec_dBm = -124;        % WB jammer spec; this WB jammer targets at lowering the noise figure by 0.2 dB
else
    NBJammerSpec_dBm = -140;         % NB jammer spec for GPS signals
    MBJammerSpec_dBm = -127;        % MB jammer spec
    WBJammerSpec_dBm = -124;        % WB jammer spec; this WB jammer targets at lowering the noise figure by 0.2 dB
end
MinJammerPower_dBm = -156;        % minimum jammer power to be detected
%--------------- INITIALIZATIONS ------------------------------
Jammers = struct('Pow',[],'Freq',[],'BW',[],'Edge',[],'Type',[],'Margin',[],'Pass',[]);
BlankingSidelobes = struct('Pow',[],'Freq',[],'BW',[],'Edge',[]);
NoJammers = 0;
NoSidelobes = 0;

%--------------- BEGIN TEST -----------------------------------

% the CW tone and a few neighboring bins are removed from input spectrum;
% calculate edge bins for the removed signal bins
CWToneHiEdge = find(SpurF > CWToneFreq,1,'first');
CWToneLoEdge = find(SpurF < CWToneFreq,1,'last');

% find all jammers above threshold
while NoJammers < MaxNoJammers
    
    % find max spur among remaining bins
    [SpVal,SpPos] = max(SpurY);
    
    % calculate the spur bandwidth using noise-floor method
    [LoLimPt,HiLimPt] = CalcSpurBW(SpurY,SpPos,NoiseFlr + NoiseBandTol);
    
    % if jammer BW includes the missing signal bins, consider only portion of
    % jammer on the side of signal which has the peak energy bin
    if (LoLimPt <= CWToneLoEdge) && (HiLimPt >= CWToneHiEdge)
        if SpPos >= CWToneHiEdge, LoLimPt = CWToneHiEdge; end
        if SpPos <= CWToneLoEdge, HiLimPt = CWToneLoEdge; end
    end
    SpBW = SpurF(HiLimPt) - SpurF(LoLimPt);
    
    % calculate jammer power and frequency
    SpLinPowVec = 10.^(SpurY(LoLimPt:HiLimPt)'/10) - 10.^(NoiseFlr(LoLimPt:HiLimPt)/10);
    SpLinPowVec(SpLinPowVec < 0) = 0;
    SpPow = 10*log10(sum(SpLinPowVec));
    SpFreqVec = SpurF(LoLimPt:HiLimPt);
    SpFreq = sum(SpLinPowVec.*SpFreqVec)/10^(SpPow/10);
    
    % exit loop if jammer power is below threshold
    if SpPow + CWTonePow < MinJammerPower_dBm
        break
    end
    
    % remove jammer from FFT data
    SpurY(LoLimPt:HiLimPt) = -inf;
    
    % check if jammer is a blanking sidelobe
    IsBlankingSidelobe = 0;
    if BlankingPeriod > 0
        FreqDiff = SpFreq - CWToneFreq;
        BlankingFreq = 1/BlankingPeriod;
        BlankingFreqTest = mod(FreqDiff,BlankingFreq) < FreqTol || mod(FreqDiff,BlankingFreq) > BlankingFreq-FreqTol;
        SidelobePowSpec = 20*log10(abs(sinc(FreqDiff*(BlankingPeriod-BlankingPulseWidth)))) + PowTol;
        BlankingPowTest = SpPow < SidelobePowSpec;
        IsBlankingSidelobe = BlankingFreqTest && BlankingPowTest;
    end
    
    % save measurements into either the sidelobe struct or the jammer struct
    if IsBlankingSidelobe
        NoSidelobes = NoSidelobes + 1;
        BlankingSidelobes(NoSidelobes).Pow = SpPow;
        BlankingSidelobes(NoSidelobes).Freq = SpFreq;
        BlankingSidelobes(NoSidelobes).BW = SpBW;
        BlankingSidelobes(NoSidelobes).Edge = [SpurF(LoLimPt) SpurF(HiLimPt)];
        %BlankingSidelobes(NoSidelobes).SpecFreq = CWToneFreq + round(FreqDiff/BlankingFreq)*BlankingFreq;
        %BlankingSidelobes(NoSidelobes).SpecPow = SidelobePowSpec;
    else
        NoJammers = NoJammers + 1;
        % classify jammer according to BW and evaluate against P/F criteria
        if SpBW >= 10000
            SpType = 4;
            SpMargin = WBJammerSpec_dBm - (SpPow + CWTonePow);
        elseif SpBW >= 1000
            SpType = 3;
            SpMargin = MBJammerSpec_dBm - (SpPow + CWTonePow);
        elseif SpBW >= 50
            SpType = 2;
            % Spread the jammer power evenly into multiple bins; 50 Hz
            % corresponds to the worst case bin seperation, 10 ms
            SpMargin = (NBJammerSpec_dBm + 10*log10(SpBW/50)) - (SpPow + CWTonePow); 
        else
            SpType = 1;
            SpMargin = NBJammerSpec_dBm - (SpPow + CWTonePow);
        end
        Jammers(NoJammers).Pow = SpPow;
        Jammers(NoJammers).Freq = SpFreq;
        Jammers(NoJammers).BW = SpBW;
        Jammers(NoJammers).Edge = [SpurF(LoLimPt) SpurF(HiLimPt)];
        Jammers(NoJammers).Type = SpType;
        Jammers(NoJammers).Margin = SpMargin;
        Jammers(NoJammers).Pass = SpMargin >= 0;
    end
    
end

% sort jammers in order of increasing margin
[margin_vec,order] = sort([Jammers.Margin]);
Jammers = Jammers(order);


% ---------------------------------------------------------------------------------------------
%    FUNCTION TO FIND JAMMER BANDWIDTH
% ---------------------------------------------------------------------------------------------
function [LoLimPt,HiLimPt] = CalcSpurBW(SpurY,SpPos,NoiseFlr)

LastPt = length(SpurY);

% find lower limit
LimPt = SpPos;
FlagFound = 0;
while ~FlagFound && (LimPt > 1)
    if SpurY(LimPt) > NoiseFlr(LimPt)
        LimPt = LimPt - 1;
    else
        FlagFound = 1;
    end
end
LoLimPt = LimPt;

% find upper limit
LimPt = SpPos;
FlagFound = 0;
while ~FlagFound && (LimPt < LastPt)
    if SpurY(LimPt) > NoiseFlr(LimPt)
        LimPt = LimPt + 1;
    else
        FlagFound = 1;
    end
end
HiLimPt = LimPt;

% ---------------------------------------------------------------------------------------------
%    SINC FUNCTION
% ---------------------------------------------------------------------------------------------
function y = sinc(x)

izero = find(x == 0);
x(izero) = 1;
y = sin(pi*x)./(pi*x);
y(izero) = 1;


% ---------------------------------------------------------------------------------------------
%    HANNING WINDOW
% ---------------------------------------------------------------------------------------------
function w = hanning(n)
w = 0.5*(1 - cos(2*pi*(0:(n-1))'/(n-1)));

% ---------------------------------------------------------------------------------------------
%    DUMP ERROR INFO DURING EMBEDDED OPERATION
% ---------------------------------------------------------------------------------------------
%
% Description:
% Print to error logs. e.g., when unhandled exceptions occur.
%
% Usage:
% param1: FunctionName = the function calling this sub-func.
% param2: VersionNum = the version num of calling func.
% param3: output_folder = standard output folder, where to put log file.
% param4: UserDefinedMsg = caller can put a msg to define err conditions.
%
% Revision history.
% 02/23/2009 hyang  v1.0.2 Reset last error and warning msg after handling.
% 05/23/2008 hyang  v1.0.1 Added logging last warning msg, and check for
%                           input parameters.
% 05/01/2008 hyang  v1.0.0 Initial draft

function print_to_error(FunctionName, VersionNum, DataFileName, output_folder, UserDefinedMsg)

try
    err_msg = lasterr;
    warn_msg = lastwarn;
    err_time = datestr(now);
    if (nargin < 1)
        FunctionName = 'Unknown';
    end
    if (nargin < 2)
        VersionNum = 'Unknown';
    end
    if (nargin <3)
        DataFileName = '';
    end
    if (nargin <4)
        output_folder = 'C:\';
    end
    if (nargin <5)
        UserDefinedMsg = 'Unknown';
    end
    output_file = strcat(output_folder, 'McrErrLogs.txt');
    [McrErrLog,msg] = fopen(output_file,'a');
    if McrErrLog == -1
        error('Unable to open file %s: %s.',output_file,msg)
    end
    fprintf( McrErrLog, '--------------------------------------');
    fprintf( McrErrLog, '\r\nException unhandled:\r\n');
    fprintf( McrErrLog, '    Function info: %s\r\n', FunctionName);
    fprintf( McrErrLog, '    Version number:%s\r\n', VersionNum);
    fprintf( McrErrLog, '    Time:%s\r\n', err_time);
    fprintf( McrErrLog, '    MCR last system error msg: %s\r\n', err_msg);
    fprintf( McrErrLog, '    MCR last system warning msg: %s\r\n', warn_msg);
    fprintf( McrErrLog, '    User defined msg: %s\r\n', UserDefinedMsg);
    fprintf( McrErrLog, '    Data filename:%s\r\n', DataFileName);
    fprintf( McrErrLog, '--------------------------------------\r\n');
    fclose( McrErrLog);
    %clear last error and warning msg
    %lasterr('');
    %lastwarn('');
catch
    if ~isdeployed
        rethrow(lasterror)
    end
end
