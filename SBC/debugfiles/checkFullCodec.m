function [varargout] = checkFullCodec(outputRef)
%CHECKFULLCODEC check if codec output of C is equal to that of MATLAB
% If not equal, CHECKFULLCODEC plots both signals and their difference.
%
% SYNTAX:
%   CHECKFULLCODEC();
%   CHECKFULLCODEC(outputRef);
%   [isEqual, PESQ_MATLAB, PESQ_C] = CHECKFULLCODEC(___);
%
% If outputRef is not specified, CHECKFULLCODEC computes the output
% signal for comparing the C results with directly from input.wav.
% !!! DON'T FORGET TO RUN THE C CODE BEFORE EXECUTNG THIS FUNCTION !!!
%
% INPUTS (optional):
%   -   outputRef: 1x1 structure containing the following two fields:
%           outputSignal: codec output signal computed in MATLAB (cf. main)
%           PESQ: PESQ score of that output signal (cf. main)
%
% OUTPUTS (optional):
%   -   isEqual: true if MATLAB and C results are equal
%   -   PESQ_MATLAB: PESQ score of MATLAB codec output signal
%   -   PESQ_C: PESQ score of C codec output signal
%
% Written by Gert Milis, 2016-03-27 (version 1)

% -------------------------------------------------------------------------

% ============================== PARAMETERS ============================== 
matlab_path = '../../../MATLAB';    % Path to the folder with the matlab
                                    % implemenation of the speech codec
% ========================================================================

% Check number of output arguments
nargoutchk(0,3);

% Read output signal computed by C
output = audioread('../output.wav','native');

% Read input
input = audioread('../input.wav','native');
if size(input,2)==1
    stereo = false;
    input = repmat(input,1,2);
else
    stereo = any(diff(input,1,2));
end

% Compute reference signal if not specified
if nargin<1
    addMatlabPath('main.m');
    [outputRefSig,delay] = main(input,8e3);
    fprintf('\n');
else
    outputRefSig = outputRef.outputSignal;
    load('defaultParam','delay');
end
outputRefSig = outputRefSig(1:size(input,1),:);

% Check if both are equal
isEqual = isequal(output,outputRefSig);
fprintf('Codec output from C is equal to MATLAB results: %d\n', isEqual);

% If not equal, plot the difference
if ~isEqual
    figure;
    t = (0:size(input,1)-1)/8e3;
    subplot(221);
    plot(t,outputRefSig(:,1));
    hold on; plot(t,output(:,1)); hold off;
    ylabel('Left channel');
    xlim([t(1) t(end)]);
    legend('Matlab','C');
    subplot(222);
    plot(t,outputRefSig(:,1)-output(:,1));
    ylabel('Difference');
    xlim([t(1) t(end)]);
    subplot(223);
    plot(outputRefSig(:,2));
    hold on; plot(t,output(:,2)); hold off;
    ylabel('Right channel');
    xlim([t(1) t(end)]);
    xlabel('Time (s)');
    legend('Matlab','C');
    subplot(224);
    plot(t,outputRefSig(:,2)-output(:,2));
    ylabel('Difference');
    xlim([t(1) t(end)]);
    xlabel('Time (s)');
end

% Compute PESQ values for MATLAB and C outputs
PESQ_MATLAB = zeros(1,2);
if nargin<1
    PESQ_MATLAB(:) = pesq(double(input(1:end-delay,1)), ...
        double(outputRefSig(delay+1:size(input,1),1)),8e3);
    if stereo
        PESQ_MATLAB(2) = pesq(double(input(1:end-delay,2)), ...
            double(outputRefSig(delay+1:size(input,1),2)),8e3);
    end
else
    PESQ_MATLAB = outputRef.PESQ;
end

if stereo
    printFormat = '[%g %g]';
    idx = 1:2;
else
    printFormat = '%g';
    idx = 1;
end

PESQ_C = zeros(1,2);
if isEqual
    PESQ_C = PESQ_MATLAB;
    fprintf(['PESQ score: ',printFormat',' \n'],PESQ_MATLAB(idx));
else
    PESQ_C(:) = pesq(double(input(1:end-delay,1)),...
        double(output(delay+1:size(input,1),1)),8e3);
    if stereo
        PESQ_C(2) = pesq(double(input(1:end-delay,2)),...
            double(output(delay+1:size(input,1),2)),8e3);
    end
    fprintf(['MATLAB PESQ score: ',printFormat,'\n'],PESQ_MATLAB(idx));
    fprintf(['C PESQ score: ',printFormat,'\n'],PESQ_C(idx));
end

% Put in outputs
varargout = {isEqual,PESQ_MATLAB,PESQ_C};
varargout = varargout(1:nargout);


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

