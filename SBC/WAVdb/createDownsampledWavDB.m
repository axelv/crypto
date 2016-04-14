% Downsample signals in WAVdb and convert to stereo if necessary

addpath('../../../MATlAB');

% ====================== PARAMETERS ======================
fs = 8e3;       % Frequency to downsample to, in Hz
scale = false;  % If true, signals values are rescaled to
                % occupy full range of 16-bits integers
% ========================================================

% Load signal database
currDir = cd;
cd('../../../MATlAB');
sigDB = loadWavDB(scale);
cd(currDir);

% Downsample signals and write to wav file in current directory
for k=1:length(sigDB)
    data = downsample(sigDB(k).Data,sigDB(k).Fs/fs);
    if scale
        nBits = 16;
    else
        nBits = sigDB(k).Nbits;
        data = 2^(nBits-1)*data;
    end
    checkInt(data,nBits);
    audiowrite(sigDB(k).Name,cast(data,sprintf('int%d',nBits)),fs);
end

% Force refSig database to be updated
if exist('../debugfiles/refSig.mat','file')==2
    delete('../debugfiles/refSig.mat');
end
