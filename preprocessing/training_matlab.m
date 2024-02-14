% This is the code that will pre-process data and send to Raspberry Pi for processing
% Will only be used for training purposes. The actual script to take data from
%   headset will be another one.

% Delay to allow signal processing of big data collected by Alex
delay = 2;

% Read signal
table = readtable("Trial_EEGs\original_signal_S001R14.csv");
data = table2array(table);

% Define the frequency bands
mu_band = [8, 13]; % Mu band (8-13 Hz)
beta_band = [13, 30]; % Beta band (13-30 Hz)

% Design a bandpass filter to capture the mu and beta bands
bpFilt = designfilt('bandpassiir', ...
                    'FilterOrder', 4, ...
                    'HalfPowerFrequency1', mu_band(1), ...
                    'HalfPowerFrequency2', beta_band(2), ...
                    'SampleRate', 512);

data_filtered = filtfilt(bpFilt, data);

% Dividing data points in 1024 mini arrays
totalDatapoints = numel(data_filtered);
datapointsPerSend = 1024;
totalSends = totalDatapoints / datapointsPerSend;

% UDP part: send to raspberry pi
u = udpport;
u.OutputDatagramSize = 8192; % 1024 double values

for i = 1:totalSends
    startIndex = (i-1)*datapointsPerSend + 1;
    endIndex = i*datapointsPerSend;
    dataChunk = data_filtered(startIndex:endIndex);
    write(u, dataChunk, "double", "192.168.1.11", 8080);
    pause(delay);
end

%write(u, data_filtered, "double", "172.20.10.10", 8080); % At school
%write(u, data_filtered, "double", "192.168.1.11", 8080); % At home