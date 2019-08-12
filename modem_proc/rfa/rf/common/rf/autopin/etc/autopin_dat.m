clear all;
as_version = 2;
fname = 'autopin_dat_settings.txt';
fid = fopen(fname);
raw_txt =  fscanf(fid,'%s');
fclose(fid);

parse_list = {  'revision_key', ...
                'autopin_enable', ...
                'loop_speed_acq', ...
                'loop_speed_trk', ...
                'pin_step', ...
                'trk_thresh'};

numMatch = [];
for k = 1:size(parse_list,2)
    expression = ['' parse_list{k} '(-?\d+)'];
    [data,match,idx] = regexp(raw_txt,expression,'tokens', 'match');
    data = cellfun(@str2num, [data{:}]);
    data = data(:);
    out.(parse_list{k}) = data;
    numMatch(k) = length(data);
end

% write the dat file in big endian mode
fid = fopen('autopin.dat','w');
fwrite(fid,out.revision_key,'uchar','l');    
for k = 1:1:4
  fwrite(fid,out.autopin_enable(k),'uchar','l');
  fwrite(fid,out.loop_speed_acq(k),'ushort','l');
  fwrite(fid,out.loop_speed_trk(k),'ushort','l');
  fwrite(fid,out.pin_step(k),'uchar','l');
  fwrite(fid,out.trk_thresh(k),'ushort','l');
end

fclose(fid);