%==============================================================================================
%			NEW JAMMER CLASSIFICATION ALGORITHM WITH NEW PASS FAIL CRITERIA
%			AUTHOR: SIBASISH DAS, ADDED ON 12/02/08
%==============================================================================================
%
% Jammers = gnss_IqTest_JammerProc(SpurY, SpurF, NoFl, CWTonePow, CWToneFreq, PhoneState)
% INPUTS: SpurY - FFT Input Samples *** _IN _dB_ SCALE_ ***
%         SpurF - FFT Input BIN CENTERS
%         NoFl - Smooth noise floor for spectrum FFT points.
%         CWTonePow - Power of CW Tone.
%         CWToneFreq - Freq. of input CW Tone.
%         GsmTrafficFlag - (1) GSM Traffic, (0) Otherwise
%
% OutPuts:  Jammers.Pow - Array of Jammer Powers in dBc
%           Jammers.BW - Bandwidth of Jammers (either 3dB or above Noise floor)
%           Jammers.Freq - Center of Mass for the Jammer in Frequency Domain
%           Jammers.Edge - Index of Edge points of Jammer on Spectrum
%           Jammers.Pass - (1) Jammer is with spec/ (0) Jammer fails spec/ (-1) WB Jammers Present       
%           Jammers.Test - (1) Jammer Test Passes / (0) Jammer Test Fails.
%==============================================================================================
%  $Header: $ 
%  $DateTime:  $ 
%  $Author:  $
%==============================================================================================

function Jammers = gnss_IqTest_JammerProc(SpurY, SpurF, NoFl, CWTonePow, CWToneFreq, GsmTrafficFlag)

%--------------- PARAMETERS -----------------------------------
	MySpurY = SpurY;
	MySpurF = SpurF;
	MaxNoJammers = 50;										% max no. of jammers reported
    
    NoiseBandTol = 0;                                       % (one-sided) width of noise band in dB
    NBJammerSpec_dBm = -140;                                % NB Jammer Spec
    MBJammerSpec_dBm = -127;                                % Medium BW (1KHz to 10KHz) Jammer Spec
    WBJammerSpec_dBm = -124;                                % WB Jammer Spec
    GSMJammerSpec_dBm = -130;                               % relaxed jammer spec for GSM (sinc mask applied separately)
    MinJammerPower_dBm = -156;                              % minimum jammer power to be detected
    
%--------------- INITIALIZATIONS ------------------------------
	Jammers.Pow = [];
	Jammers.Freq = [];
	Jammers.BW = [];
    Jammers.Edge = [];
	Jammers.Type = [];
	NoJammers = 0;

%--------------- BEGIN TEST -----------------------------------
	    
    % note that the CW-Tone and a few neighboring bins are removed from input spectrum. 
    % calculate edge bins for the removed signal bins
    CWToneHiEdge = min(find(MySpurF > CWToneFreq));
    CWToneLoEdge = max(find(MySpurF < CWToneFreq));
    
    
    % find all jammers above threshold
	for indxj = 1:MaxNoJammers,
		% find max spur amonst remaining bins
		[SpVal, SpPos] = max(MySpurY);
		
		% calculate the spur bandwidth using noise-floor method
            [LoLimPt, HiLimPt] = CalcSpurBW(MySpurY,MySpurF,SpVal,SpPos,NoFl + NoiseBandTol,MySpurF);
            
            % if jammer BW includes the missing signal bins, consider only portion of
            % Jammer on the side of signal which has the peak energy bin.
            if (LoLimPt <= CWToneLoEdge && HiLimPt >= CWToneHiEdge)
                if (SpPos >= CWToneHiEdge), LoLimPt = CWToneHiEdge; end;
                if (SpPos <= CWToneLoEdge), HiLimPt = CWToneLoEdge; end;
            end        
            SpBW = MySpurF(HiLimPt)-MySpurF(LoLimPt);
            
           
            % check if jammer is from GSM Blanking 
            if (GsmTrafficFlag)    % GSM traffic
                FreqDiffGSM = MySpurF(SpPos) - CWToneFreq;
                IsGSMJammer = ((mod(FreqDiffGSM, 216.64) < 16) ||...
                                (mod(FreqDiffGSM, 216.64) > (216.64-16 )));
            else
                IsGSMJammer = 0;
            end            
            
            %CALCULATE JAMMER POWER AND FREQUENCY.
            % if GSM Traffic Jammer, leave Freq and Pow consistent with legacy algorithm
            if IsGSMJammer,
                SpFreq = MySpurF(SpPos);
                SpPow = SpVal;
            else
                SpPow = 10*log10(sum(10.^(MySpurY(LoLimPt:HiLimPt)/10)));   % jammer power 
                SpFreq = sum((10.^(MySpurY(LoLimPt:HiLimPt).'/10)).*MySpurF(LoLimPt:HiLimPt))/...
                            (10^(SpPow/10));    % jammer frequency
            end                        
                        
			% classify into jammer types and append to Jammer Array	
            % (1) BW < 50Hz, (2) 50<=BW<1000, (3) 1000<=BW<10000, (4) BW > 10000
            
			if (SpPow + CWTonePow >= MinJammerPower_dBm)
				if (SpBW >= 10000),
					FlagWBJam = 1;
					Jammers.Pow = [Jammers.Pow; SpPow];
					Jammers.Freq = [Jammers.Freq; SpFreq];
					Jammers.BW = [Jammers.BW; SpBW];
                    Jammers.Edge = [Jammers.Edge; MySpurF(LoLimPt) MySpurF(HiLimPt)];
					Jammers.Type = [Jammers.Type; 4];
					
				elseif (SpBW >= 1000),
					Jammers.Pow = [Jammers.Pow; SpPow];
					Jammers.Freq = [Jammers.Freq; SpFreq];
					Jammers.BW = [Jammers.BW; SpBW];
                    Jammers.Edge = [Jammers.Edge; MySpurF(LoLimPt) MySpurF(HiLimPt)];
					Jammers.Type = [Jammers.Type; 3];
				
                elseif (SpBW >= 50),
					Jammers.Pow = [Jammers.Pow; SpPow];
					Jammers.Freq = [Jammers.Freq; SpFreq];
					Jammers.BW = [Jammers.BW; SpBW];
                    Jammers.Edge = [Jammers.Edge; MySpurF(LoLimPt) MySpurF(HiLimPt)];
					if IsGSMJammer,
                        Jammers.Type = [Jammers.Type; 0];
                    else
                        Jammers.Type = [Jammers.Type; 2];
                    end
                else
					Jammers.Pow = [Jammers.Pow; SpPow];
					Jammers.Freq = [Jammers.Freq; SpFreq];
					Jammers.BW = [Jammers.BW; SpBW];
                    Jammers.Edge = [Jammers.Edge; MySpurF(LoLimPt) MySpurF(HiLimPt)];
					if IsGSMJammer,
                        Jammers.Type = [Jammers.Type; 0];
                    else
                        Jammers.Type = [Jammers.Type; 1];
                    end
				end
				
                % remove Jammer from fft data
				MySpurY(LoLimPt:HiLimPt) = -inf;
				NoJammers = NoJammers + 1;
            else
                break;
            end
    end

    if isempty(Jammers.Pow),
        Jammers.Test = 1;
        return;
    end
    
    % sort according to jammer powers
    [TempVec, TempIndx] = sort(Jammers.Pow,'descend');
    Jammers.Pow = TempVec;
    Jammers.Freq = Jammers.Freq(TempIndx);
    Jammers.BW = Jammers.BW(TempIndx);
    Jammers.Edge(:,1) = Jammers.Edge(TempIndx,1);
    Jammers.Edge(:,2) = Jammers.Edge(TempIndx,2);
    Jammers.Type = Jammers.Type(TempIndx);
    
	% apply pass fail criteria and generate report
    Jammers.Test = 1;
    Jammers.Pass = zeros(NoJammers,1);
    TotalJammerPower = 10*log10(sum(10.^(Jammers.Pow/10))); % used for WB-Jammers

    for indx = 1:NoJammers,
        switch (Jammers.Type(indx))
            case 0      % NB GSM Jammer
                FreqDiffGSM = Jammers.Freq(indx) - CWToneFreq;
                SincFactor = 10*log10(abs(sinc(FreqDiffGSM/(216.64*8))));
                SincFactor = max(SincFactor,-10);
                Jammers.Pass(indx) = (Jammers.Pow(indx) + CWTonePow <= (GSMJammerSpec_dBm + SincFactor));
                Jammers.Test = Jammers.Test && Jammers.Pass(indx);

            case 1      % NON GSM NB Jammer
                Jammers.Pass(indx) = (Jammers.Pow(indx) + CWTonePow < NBJammerSpec_dBm);
                Jammers.Test = Jammers.Test && Jammers.Pass(indx);

            case 2      % Jammer BW between 50Hz and 1KHz
                Jammers.Pass(indx) =...
                    (Jammers.Pow(indx) + CWTonePow < (NBJammerSpec_dBm + 10*log10(Jammers.BW(indx)/50)));
                Jammers.Test = Jammers.Test && Jammers.Pass(indx);

            case 3      % jammer BW between 1KHz and 10KHz
                Jammers.Pass(indx) = (Jammers.Pow(indx) +CWTonePow < MBJammerSpec_dBm);
                Jammers.Test = Jammers.Test && Jammers.Pass(indx);
            case 4      % wideband jammer: spec on total jammer power
                Jammers.Pass(indx) = (TotalJammerPower + CWTonePow < WBJammerSpec_dBm);
                Jammers.Test = Jammers.Test && Jammers.Pass(indx);
        end

    end

% -------------------------------------------------------------------------
%                 FUNCTION TO FIND JAMMER BANDWIDTH
% -------------------------------------------------------------------------
function [LoLimPt, HiLimPt] = CalcSpurBW(MySpurY,MySpurF,SpVal,SpPos,NoFl,NLFreqPts)
		
			LastPt = length(MySpurY);
			
			% find lower limit
			LimPt = SpPos;
			FlagFound = 0;
			while ((~FlagFound) && (LimPt > 1)),
				 if(MySpurY(LimPt) > NoFl(LimPt)),
					LimPt = LimPt - 1;
				 else
					FlagFound = 1;
				 end
			end
			LoLimPt = LimPt;

			% find upper limit
			LimPt = SpPos;
			FlagFound = 0;
			while ((~FlagFound) && (LimPt < LastPt)),
				 if(MySpurY(LimPt) > NoFl(LimPt)),
					LimPt = LimPt + 1;
				 else
					FlagFound = 1;
				 end
			end
			HiLimPt = LimPt;
					 
return;	
