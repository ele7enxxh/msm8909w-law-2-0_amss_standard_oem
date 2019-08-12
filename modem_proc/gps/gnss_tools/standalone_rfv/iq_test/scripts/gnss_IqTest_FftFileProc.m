%
%$Header:
%*****************************************************************************
%  Copyright (C) 2003-2012 QUALCOMM Incorporated.
%  Copyright (c) 2013-2015 Qualcomm Technologies, Inc. 
%  All Rights Reserved.
%  Confidential and Proprietary - Qualcomm Technologies, Inc.
%
% =============================================================================================
% *****************************************************************************
% Description:
%
% Version 1.0.1
%
%*****************************************************************************
%*/
%
% Usage : cgps_IqTest_FftFileProc( 'FileName' )
%
function fftplot = gnss_IqTest_FftFileProc(FileName, ...
    widejam_band...                                       % [lower_frequency_boud_in_Hz upper_frequency_bound_in_Hz] defines a frequency range where a wide band jammer is needed;
    )
VersionNum = '1.0.1';
% enum Enum1
% {
%   COLLECT_MODE_WBIQ_BP1_BP2_2MHZ,
%   COLLECT_MODE_WBIQ_BP1_2MHZ,
%   COLLECT_MODE_WBIQ_BP2_2MHZ,
%   COLLECT_MODE_WBIQ_BP3_1MHZ,
%   COLLECT_MODE_FBIQ_BP4_20MHZ,
%   COLLECT_MODE_NBIQ_BP1_AUTO_1KHZ,
%   COLLECT_MODE_NBIQ_BP2_AUTO_1KHZ,
%   COLLECT_MODE_NBIQ_BP3_AUTO_1KHZ,
%   COLLECT_MODE_NBIQ_BP1_MANUAL_1KHZ,
%   COLLECT_MODE_NBIQ_BP2_MANUAL_1KHZ,
%   COLLECT_MODE_NBIQ_BP3_MANUAL_1KHZ,
%   COLLECT_MODE_NBIQ_BP4_MANUAL_1KHZ,
%   COLLECT_MODE_ADC1_16_POINT_8_MHZ,
%   COLLECT_MODE_ADC2_16_POINT_8_MHZ,
%   COLLECT_MODE_ADC1_ADC2_21_MHZ,
%   COLLECT_MODE_MAX
% } TEnum1;

% Default filename cgps_fft_collect.txt
if nargin < 1, FileName = 'gnss_fft_collect.bin'; end;
% Default for wideband jammer
if nargin < 2, widejam_band = ''; end;

fid = fopen(FileName,'r');
SigFreqKhz_RF = 1575.52e3;
SigLevelDbm = -120;
% GPS L1 center frequency; GAL E1 center frequency;
fL1 = 1575.42e6;
% BDS B1 center frequency;
fB1 = 1561.098e6;
% Inline function that gives the center frequency of a GLO channel
glo_freq = @(ch) 1602e6 + ch*562.5e3;  % Hz
% Inline function that calculates RMS
rms = @(v) sqrt(mean(abs(v).^2));

% Status Flag
% Peak FFT Power
% FFT Center Freq (Hz)
% Collect mode (0~4=>WBIQ (BP1&BP2, BP1, BP2, BP3, BP4)
%              (5~7=>NBIQ Autocenter (BP1, BP2, BP3)
%              (8-11=>NBIQ Manual (BP1, BP2, BP3, BP4)
%              (12-14)=>ADC1 Only, ADC1 Only, ADC1 & ADC2
% GLO R1 Freq (k value)
% GLO HW Chan (1 of 12)
% Number of 1k (1024) Samples
% Total requested integrations
%----------------------------------------
%   HEADER
%----------------------------------------
Config = fread(fid, 23, 'int32');
BinFileVersion = Config(1);
Flag = Config(2);
PeakFFTPower = Config(3);
FFTCenterFreq = Config(4);
CollectMode = Config(5);
GloR1FreqK = Config(6);
GloHWChan = Config(7);
SamplesPerInteg = Config(8);
SamplesPerInteg = SamplesPerInteg * 1024;
NumCollections = Config(9);
NBCenterFreqHz = Config(10);
BpOffsetFreqHz = Config(11);
TotalRelativeGain = Config(12);
FBIqMode = Config(13);
% Read in the rest of the header information;
fsample_nom = Config(14);                        % Hz
samp_mem_sz = Config(15);                      % bytes
xo_error_hwu = Config(16);                        % 2^-20 ppm
gps_if_freq = Config(17);                               % Hz
pga_gain = Config(18);                                   % dB
adc_setpoint_mv = Config(19)/10;          % mV (ADC set point;)
vref = Config(20)/1000;                                % Vref in volt;
adc_bitwidth = Config(21);                           % ADC bit width
adc_config_flags = Config(22);
% Flag to indicate if IQ swap is enabled in ADC;
is_iq_swapped = bitand(adc_config_flags, 1);
xo_error_ppm = xo_error_hwu*2^-20;
fsample = fsample_nom*(1 + 1e-6*xo_error_ppm);
fLO = (fL1 - gps_if_freq)*(1 + 1e-6*xo_error_ppm);
%----------------------------------------
%   Diaplay the key RF info
%----------------------------------------
fprintf('XO error    = %.2f ppm\n',xo_error_ppm)
fprintf('Sample rate = %.0f Hz\n',fsample)
fprintf('PGA gain    = %d dB\n\n',pga_gain)
fprintf('ADC set point = %f mV\n', adc_setpoint_mv)
fprintf('ADC vref = %f V\n', vref)
fprintf('ADC bit width = %d bits\n', adc_bitwidth)
fprintf('ADC config flags = %d\n\n', adc_config_flags)
%----------------------------------------
%   Read in the 32k-FFT output
%----------------------------------------
Samples=fread(fid,inf, 'uint16');
% The file contains the log value of the spectrum;
logFftData = Samples * (-0.002);
% Linear value of the spectrum is needed for power summation;
linFftData = 10.^(logFftData/10);
fclose(fid);
% Retrieve the length of FFT;
nfft=length(Samples);
% Fix
switch CollectMode
    case {0, 1, 2}
        FSample = 2046000;  % Hz
        FScale = 1/1000;
        FUnits = 'kHz';
    case 3
        FSample = 1022000;  % Hz
        FScale = 1/1000;
        FUnits = 'kHz';
    case 4
        FSample = 20460000;  % Hz
        FScale = 1/1000;
        FUnits = 'kHz';
    case {5, 6, 7 , 8, 9, 10, 11}
        FSample = 1000;     % Hz
        FScale = 1;
        FUnits = 'Hz';
    case 16 % ADC I/Q
        FSample = 576000000/7;  % Hz
        FScale = 1e-6;
        FUnits = 'MHz';
    otherwise
        fprintf(1, 'Invalid Collect Mode %u\n', CollectMode);
        return;
end
% Calculate 1 PPM value;
LO_freq_oneppm_Hz = fLO * 1e-6;
OnePpmKHz = LO_freq_oneppm_Hz / 1e3;
freqPpmLim = 15;                                                              % unit is ppm
numSigSamps = 1;                                                             % +/- numSigSamps is the number of frequency bins that are considered as covered by the CW tone;
% Calculate the left & right edges of the GPS/GLO/BDS/GAL bands
gps_band = fL1 - fLO + 1023e3*[-1 1];
bds_band = fB1 - fLO + 2046e3*[-1 1];
glo_band = glo_freq([-7 6]) - fLO + 511e3*[-1 1];
gal_band =  fL1 - fLO + 2046e3*[-1 1];
rbw = fsample/nfft;                                         % frequency bin size in the DFT
% Get the Center Frequency
CenterFreq = NBCenterFreqHz;
f = FSample / nfft * ([0:nfft-1] - nfft/2);
% Define frequency vector
fftFreqPts = fsample/nfft*(0:nfft-1)';
ineg = find(fftFreqPts >= fsample/2);
fftFreqPts(ineg) = fftFreqPts(ineg) - fsample;
BinSize = fsample / nfft;
% Reorder the spectrum to put DC in the middle
fftFreqPts = fftshift(fftFreqPts);
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
%----------------------------------------
% Noise floor calculation
%----------------------------------------
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
med_to_mean_dB = 10/(3*log(10)*NumCollections);
noise_flr = 10^(med_to_mean_dB/10)*noise_flr;
%----------------------------------------
% Ref tone calculation
%----------------------------------------
% Convert RF frequency to IF frequency;
SigFreqKhz = SigFreqKhz_RF - fLO/1e3;
fprintf('A reference tone is place at %4.4f KHz at power level %4.0f dBm. \n', SigFreqKhz, SigLevelDbm);
% Locate the signal using input signal frequency
SigUncBins = ceil(max(7, 2 * freqPpmLim) * OnePpmKHz * 1e3 / BinSize);
expectedSigIdx = find(abs(fftFreqPts-SigFreqKhz*1e3)==min(abs(fftFreqPts-SigFreqKhz*1e3)));
[no_use SigPkIdx] = max(linFftData(expectedSigIdx + [-SigUncBins:SigUncBins]));
SigPkIdx = expectedSigIdx - SigUncBins + SigPkIdx - 1;
% Get the estimated mv^2 => dBm mapping
sigPower = sum(linFftData( SigPkIdx + [-numSigSamps:numSigSamps]));% - noise_flr( SigPkIdx + [-numSigSamps:numSigSamps]) );
% Find the "weighted average" signal position
SigPos_Hz = sum((linFftData( SigPkIdx + [-numSigSamps:numSigSamps] ))...
    .* fftFreqPts(SigPkIdx + [-numSigSamps:numSigSamps]))/...
    sum(linFftData( SigPkIdx + [-numSigSamps:numSigSamps] ));
inv_R = 10^(SigLevelDbm/10) ./ sigPower;   % Unit is mw/V^2
%----------------------------------------
%   Figure 1 - Spectrum plot
%----------------------------------------
figure
fig.spec = gcf;
% Generate the dBm value;
logFftData = 10*log10(linFftData .* inv_R);
hspec = plot(FScale*fftFreqPts,logFftData,...
    FScale*fftFreqPts,10*log10(noise_flr .* inv_R),...
    FScale * fftFreqPts(SigPkIdx + [-numSigSamps:+numSigSamps]), logFftData(SigPkIdx + [-numSigSamps:+numSigSamps]), 'k-', ...
    FScale * SigPos_Hz, SigLevelDbm, 'kd');
hold on;
grid on
xlabel('Frequency (MHz)')
ylabel('Signal Power (dBm) at ADC quantizer')
FileNameTitle = strrep( FileName, '_', '\_' );
ht = title(sprintf('Frequency domain: %s',FileNameTitle));
set(ht,'Interpreter','none')
ax = axis;
ax(1:2) = FScale*fsample*[-0.5 0.5];
axis(ax)
h = text(ax(1),ax(4)-0.5,sprintf(' RBW = %.1f Hz\n PGA = %d dB',fsample/nfft,pga_gain));
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
%----------------------------------------
%   WB jammer calculation disp
%----------------------------------------
% Display the wideband jammer power;
if ~isempty(widejam_band)
    fprintf('%15s = %8.3f dBm in band [%.4f %.4f] MHz\n','Wideband jammer', 10*log10(jammer_pow * inv_R),1e-6*widejam_band(1),1e-6*widejam_band(2))
end