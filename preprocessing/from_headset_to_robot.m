,

% Set up UDP connection
u = udpport;
u.OutputDatagramSize = 8192;
% Preallocate buffer
data = 0;
data_arr = [];
data_arr_total = [];
% COM port number Selection
portnum1 = 3;
comPortName1 = sprintf('\\\\.\\COM%d', portnum1);
% Baud rate for use with TG_Connect() and TG_SetBaudrate().
TG_BAUD_115200 = 115200;
% Data format for use with TG_Connect() and TG_SetDataFormat().
TG_STREAM_PACKETS = 0;
% Data type that can be requested from TG_GetValue().
TG_DATA = 4;
% Load thinkgear64 dll
if ~libisloaded('thinkgear64')
    loadlibrary('thinkgear64.dll');
else
    unloadlibrary thinkgear64
    loadlibrary('thinkgear64.dll');
end
% To display in Command Window
fprintf('thinkgear64.dll loaded\n');
% Get a connection ID handle to thinkgear64
connectionId1 = calllib('thinkgear64', 'TG_GetNewConnectionId');
if ( connectionId1 < 0 )
error( sprintf( 'ERROR: TG_GetNewConnectionId() returned %d.\n', connectionId1 ) );
end;
% Attempt to connect the connection ID handle to serial port "COM3"
errCode = calllib('thinkgear64', 'TG_Connect', connectionId1,comPortName1,TG_BAUD_115200,TG_STREAM_PACKETS );
if ( errCode < 0 )
error( sprintf( 'ERROR: TG_Connect() returned %d.\n', errCode ) );
end
fprintf( 'Connected. Reading Packets...\n' );
i=0;
j=0;
% To display in Command Window
disp('Reading Brainwaves');
%figure;
while i < 60000
if (calllib('thinkgear64','TG_ReadPackets',connectionId1,1) == 1) %if a packet was read...
if (calllib('thinkgear64','TG_GetValueStatus',connectionId1,TG_DATA) ~= 0)
j = j + 1;
i = i + 1;
% Read EEG Values from thinkgear64 packets
data = calllib('thinkgear64','TG_GetValue',connectionId1,TG_DATA);
if length(data_arr) < 1024
    data_arr = [data_arr, data];
else
    data_arr = [];
end
data_arr_total = [data_arr_total, data];
if length(data_arr) == 1024
    [data_filtered, numPeaks] = clean_and_preprocess(data_arr);
    write(u, data_arr, "double", "172.20.10.10", 8080);
    disp(numPeaks);
end
% To display in Command Window
%disp();
% Plot Graph
%plot(data_med);
%title('THETA');
% Delay to display graph
%pause(0.00195312);
end
end
end

% Rotating data array
Rotated_data = rot90(data_arr_total);
Rotated_data = rot90(Rotated_data);
Rotated_data = rot90(Rotated_data);

% To display in Command Window
disp('Loop Completed')
% Release the COM port
calllib('thinkgear64', 'TG_FreeConnection', connectionId1);
%save data to csv file
%csvwrite('EEGdataTrialR.csv', Rotated_data);%dlmwrite(path + 'data.csv', data_arr,'precision', '%.6f');
% Close figures
close all;