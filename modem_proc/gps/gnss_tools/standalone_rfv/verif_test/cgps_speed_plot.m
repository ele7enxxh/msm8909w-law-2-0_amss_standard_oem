% 
%$Header: //depot/tools/mgp/FtmScripts/ftm_gps_speed_plot.m#2 $ $DateTime: 2003/12/11 17:31:00 $ $Author: trowland $
%*****************************************************************************
%  Copyright (C) 2003 Qualcomm, Inc.
%
%                  Qualcomm, Inc.
%                  675 Campbell Technology Parkway
%                  Campbell, CA  95008
%
% This program is confidential and a trade secret of SnapTrack, Inc.  The
% receipt or possession of this program does not convey any rights to reproduce 
% or disclose its contents or to manufacture, use or sell anything that this 
% program describes in whole or in part, without the express written consent of
% SnapTrack, Inc.  The recipient and/or possessor of this program shall not 
% reproduce or adapt or disclose or use this program except as expressly 
% allowed by a written authorization from SnapTrack, Inc.
%
% *****************************************************************************
% Description:
%
% Version 1.00
% CGPS speed plot generates a plot of the
% measured SV speed as a function of time. This
% allows the oscillator frequency vs temperature
% characteristic to be examined
% 
%*****************************************************************************
%*/

%
% Usage : cgps_speed_plot( 'FileName', SvId )
%

function cgps_speed_plot( FileName, SvId )
if(isdeployed)
    SvId=str2num(SvId);
end

Samples = load( FileName );

x = Samples( find( Samples(:,1) == SvId ), : );

FCountStart = x(1,2);
CNo = x(:,3);
Speed = x(:,5);
Secs = (x(:,2) - FCountStart) / 1000;

hold off;
plot( Secs, Speed );
grid;
zoom on;

title( sprintf( 'Sv Speed vs Time - File: %s', FileName ) );
ylabel( 'Speed [m/s]');
handl = xlabel( 'Time [secs]' );


% Save figure for test documentation
saveas(handl,'SpeedPlotFig.bmp','bmp');



