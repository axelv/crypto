function [refSig] = genRefSig()
%GENREFSIG Generate check references
% Generate reference signals to compare the results of the analysis,
% synthesis, and codec checks to.

% ============================== PARAMETERS ============================== 
matlab_path = '../../../MATLAB';    % Path to the folder with the matlab
                                    % implemenation of the speech codec
% ========================================================================

addMatlabPath('defaultParam.mat','analysis.m','synthesis.m','main.m');
load('defaultParam','h0','g0','nSubBands','bufferSize');

% List all files in WAVdb
fileNames = dir('../WAVdb/*.wav');
fileNames = {fileNames(:).name};
nFiles = length(fileNames);

% Compute reference results to check results to 
refSig = struct('Name',{},'subBandsRef',{},'synthSigRef',{},'fullCodecRef',{});
h = waitbar(0,sprintf('Processing file %d/%d ...',1,nFiles));
for k=1:nFiles
    waitbar((k-1)/nFiles,h,sprintf('Processing file %d/%d ...',k,nFiles));

    refSig(k).Name = fileNames{k};

    % Read input signal
    signal = audioread(['../WAVdb/',fileNames{k}],'native');
    if size(signal,2)==1
        signal = repmat(signal,1,2);
    end

    % Compute reference for analysis check
    % (conversion to int16 to save space)
    refSig(k).subBandsRef = ...
        analysis(h0,signal,nSubBands,bufferSize,true);

    % Compute reference for synthesis check
    refSig(k).synthSigRef = ...
        synthesis(g0,refSig(k).subBandsRef,bufferSize,true);

    % Compute reference for full codec check
    [outputSignal,~,PESQ] = main(signal,8e3);
    refSig(k).fullCodecRef = ...
        struct('outputSignal',outputSignal,'PESQ',PESQ);
end
waitbar(1,h); close(h);

% Save result to mat file
save('refSig','refSig');
    
    
% ------------------------------------------------------------------------

% If matlab implentation is not already in path, add it to path.
% If path does not seem to exist, throw error.
function addMatlabPath(varargin)
    alreadyInPath = true;
    for ii = 1:nargin
        alreadyInPath = alreadyInPath && exist(varargin{ii},'file');
    end
    if ~alreadyInPath
        if exist(matlab_path,'dir')~=7
            error(sprintf( ...
                ['Path to matlab implementation does not exist.\n', ...
                 'Please change the matlab_path parameter, or add the path manually.']));
        else
            fileList = dir(matlab_path);
            fileList = {fileList.name};
            validPath = true;
            for ii = 1:nargin
                validPath = validPath && ismember(varargin{ii},fileList);
            end
            if ~validPath
                error(sprintf( ...
                    ['Path to matlab implementation exists, but does not contain the right files.\n', ...
                    'Please change the matlab_path parameter, or add the path manually.']));
            else
                addpath(matlab_path);
            end
        end
    end
end
    
end


