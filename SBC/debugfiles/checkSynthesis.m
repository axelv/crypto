function [varargout] = checkSynthesis(synthSigRef)
%CHECKSYNTHESIS check if synthesis results of C equal those of MATLAB
% If not equal, CHECKSYNTHESIS plots both signals and their difference.
%
% SYNTAX:
%   CHECKSYNTHESIS();
%   CHECKSYNTHESIS(synthSigRef);
%   isEqual = CHECKSYNTHESIS(___);
%
% If synthSigRef is not specified, CHECKSYNTHESIS computes the synthesized
% signal for comparing the C result with directly from input.wav.
% !!! DON'T FORGET TO RUN THE C CODE BEFORE EXECUTNG THIS FUNCTION !!!
%
% INPUTS (optional):
%   -   synthSigRef: synthesized signal computed in MATLAB
%
% OUTPUTS (optional):
%   -   isEqual: true if MATLAB and C results are equal
%
% Written by Gert Milis, 2016-03-27 (version 1)

% -------------------------------------------------------------------------

% ============================== PARAMETERS ============================== 
matlab_path = '../../../MATLAB';    % Path to the folder with the matlab
                                    % implemenation of the speech codec
% ========================================================================

% Check number of output arguments
nargoutchk(0,1);

% Read synthesised signal computed by C
synthSig = audioread('../output.wav','native');

% Read input
input = audioread('../input.wav','native');
if size(input,2)==1
    input = repmat(input,1,2);
end

% Compute reference signal if not specified
if nargin<1
    addMatlabPath('defaultParam.mat','analysis.m','synthesis.m');
    load('defaultParam','h0','g0','nSubBands','bufferSize');
    subBands = analysis(h0,input,nSubBands,bufferSize,true);
    synthSigRef = synthesis(g0,subBands,bufferSize,true);
end
synthSigRef = synthSigRef(1:size(input,1),:);

% Check if both are equal
isEqual = isequal(synthSig,synthSigRef);
display(['Reconstructed signal from C is equal to MATLAB results: ', ...
    num2str(isEqual)]);
if nargout>=1
    varargout{1} = isEqual;
end

% If not equal, plot the difference
if ~isEqual
    figure;
    subplot(221);
    plot(synthSigRef(:,1));
    hold on; plot(synthSig(:,1)); hold off;
    ylabel('Left channel');
    subplot(223);
    plot(synthSigRef(:,2));
    hold on; plot(synthSig(:,2)); hold off;
    ylabel('Right channel');
    subplot(222);
    plot(synthSig(:,1)-synthSigRef(:,1));
    ylabel('Difference');
    subplot(224);
    plot(synthSig(:,2)-synthSigRef(:,2));
    ylabel('Difference');
end


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

