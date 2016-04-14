function [varargout] = checkAnalysis(subBandsRef)
%CHECKANALYSIS check if analysis results of C equal those of MATLAB.
% If not equal, CHECKANALYSIS plots both subBands and their difference.
%
% SYNTAX:
%   CHECKANALYSIS();
%   CHECKANALYSIS(subBandsRef);
%   isEqual = CHECKANALYSIS(___);
%
% If subBandsRef is not specified, CHECKANALYSIS computes the subBands
% for comparing the C results with directly from input.wav.
% !!! DON'T FORGET TO RUN THE C CODE BEFORE EXECUTNG THIS FUNCTION !!!
%
% INPUTS (optional):
%   -   subBandsRef: subBand signals computed in MATLAB
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

addMatlabPath('defaultParam.mat','analysis.m');
load('defaultParam','bufferSize','nSubBands');

% Check number of output arguments
nargoutchk(0,1);

% Read input signal
input = audioread('../input.wav','native');
if size(input,2)==1
    input = repmat(input,1,2);
end
nSamples = size(input,1);

% Read subbands computed by C
downSampleFactors = [8 8 4 4 4].';
lengths = bufferSize./downSampleFactors;
rightSub = int16(dlmread('subBands.txt'));
leftSub = rightSub(:,1:2:end);
rightSub = rightSub(:,2:2:end);
subBands = cell(5,1);
for k=1:nSubBands
    tmpLeft = leftSub(:,sum(lengths(1:k-1))+(1:lengths(k))).';
    tmpRight = rightSub(:,sum(lengths(1:k-1))+(1:lengths(k))).';
    subBands{k} = [tmpLeft(:),tmpRight(:)];
end

% Compute reference signal if not specified
if nargin<1
    % Compute subbands
    load('defaultParam','h0');
    subBandsRef = analysis(h0,input,nSubBands,bufferSize,true);
end

% Trim subbands. Necessary because C does NOT zeropadd signal when its
% length is not an integer multiple of bufferSize, whereas MATLAB does.
subBands = cellfun(@(x,y)x(1:nSamples/y,:),subBands, ...
    num2cell(downSampleFactors),'UniformOutput',false);
subBandsRef = cellfun(@(x,y)x(1:nSamples/y,:),subBandsRef, ...
    num2cell(downSampleFactors),'UniformOutput',false);

% Check if both are equal
isEqual = isequal(subBands,subBandsRef);
display(['Subband signals from C are equal to MATLAB results: ', ...
    num2str(isEqual)]);
if nargout>=1
    varargout{1} = isEqual;
end

% If not equal, plot the difference
if ~isEqual
    figure('units','normalized','outerposition',[0 0 1 1]);
    for k=1:nSubBands
        subplot(nSubBands,4,4*k-3)
        plot(subBandsRef{k}(:,1));
        hold on; plot(subBands{k}(:,1)); hold off;
        axis tight;
        ylabel(sprintf('Band %d',k));
        legend('MATLAB','C');
        if k==1
            title('Left channel');
        end
        subplot(nSubBands,4,4*k-2);
        plot(subBandsRef{k}(:,1)-subBands{k}(:,1));
        xlim([1 size(subBands{k},1)]);
        ylabel(sprintf('Band %d',k));
        if k==1
            title('Left channel difference');
        end
        
        subplot(nSubBands,4,4*k-1)
        plot(subBandsRef{k}(:,2));
        hold on; plot(subBands{k}(:,2)); hold off;
        axis tight;
        ylabel(sprintf('Band %d',k));
        legend('MATLAB','C');
        if k==1
            title('Right channel');
        end
        subplot(nSubBands,4,4*k);
        plot(subBandsRef{k}(:,2)-subBands{k}(:,2));
        xlim([1 size(subBands{k},1)]);
        ylabel(sprintf('Band %d',k));
        if k==1
            title('Right channel difference');
        end
    end
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

