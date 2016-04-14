% Run analysis, synthesis, and full codec checks for all files in WAVdb

close all;
clc;

% List all files in WAVdb
signals = dir('../WAVdb/*.wav');
signals = {signals(:).name};
nFiles = length(signals);

% Load reference signals
if exist('refSig.mat','file')==2
    load('refSig','refSig');
else
    answer = questdlg([' No refSig.mat file seems to exist. ', ...
        'Do you want to create it now?'], 'refSig database not found');
    switch answer
        case 'Yes'
            refSig = genRefSig();
            clc;
        case 'No'
            refSig = struct('Name',{},'subBandsRef',{},'synthSigRef',{},'fullCodecRef',{});
            idx = [];
        otherwise
            error('Operation terminated by user');
    end
end

% Loop over all files
analCheck = false(nFiles,1);
synthCheck = false(nFiles,1);
codecCheck = false(nFiles,1);
PESQ_MATLAB = zeros(nFiles,2);
PESQ_C = zeros(nFiles,2);

nFig = 0;
getNFig = @()length(findobj(0,'type','figure'));
h = waitbar(0,sprintf('Processing file %d/%d ...',1,nFiles));
for k=1:nFiles
    waitbar((k-1)/nFiles,h,sprintf('Processing file %d/%d ...',k,nFiles));
    
    % Check if reference signal is in database
    if ~isempty(refSig) % reference signal database is loaded
        idx = find(strcmp(signals{k},{refSig(:).Name}));
        if isempty(idx)
            warning([signals{k},' does not appear to be in refSig database. ',...
                'Consider running genRef.']);
        elseif length(idx)>2
            error(['corrupt refSig database: multiple entries for ',signals{k}]);
        end
    end
    
    % Create input.wav file
    copyfile(['../WAVdb/',signals{k}],'../input.wav');
    
    % Check analysis and synthesis
    fprintf('\n');
    cd('..'); unix('./checkAnalSynth'); cd('debugfiles');
    fprintf('\n');
    analCheck(k) = checkAnalysis(refSig(idx).subBandsRef);
    if getNFig()>nFig
        set(gcf,'Name',['Comparison of analysis output for ',signals{k}]);
        nFig = nFig+1;
    end
    synthCheck(k) = checkSynthesis(refSig(idx).synthSigRef);
    if getNFig()>nFig
        set(gcf,'Name',['Comparison of synthesis output for ',signals{k}]);
        nFig = nFig+1;
    end

    % Check full codec
    fprintf('\n'); 
    cd('..'); unix('./SBC'); cd('debugfiles');
    fprintf('\n');
    [codecCheck(k),PESQ_MATLAB(k,:),PESQ_C(k,:)] = ...
        checkFullCodec(refSig(idx).fullCodecRef);
    if getNFig()>nFig
        set(gcf,'Name',['Comparison of codec output for ',signals{k}]);
        nFig = nFig+1;
    end
    
end
waitbar(1,h); close(h);

% Save results to mat file
save('codecResults','signals','analCheck','synthCheck', ...
    'codecCheck','PESQ_MATLAB','PESQ_C');

% Clean up
delete('subBands.txt');

%% Summarize results in table
codecResults = [analCheck,synthCheck,codecCheck,PESQ_MATLAB(:,1),PESQ_C(:,1)];
rlab = strjoin(cellfun(@(x)strtok(x,'.'),signals,'UniformOutput',false));
clab = 'analysis synthesis fullCodec PESQ_MATLAB PESQ_C';
printmat(codecResults,'resultOfCodecCheck',rlab,clab);
