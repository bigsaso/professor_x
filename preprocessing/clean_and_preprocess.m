function [data_filtered, num_peaks] = clean_and_preprocess(csv)
    table = readtable(csv);
    data = table2array(table);
    data = normalize(data,"range"); % min-max normalization
    Fs = 512; % Will become 512Hz once working with data coming from the actual headset.
    % Design low pass filter and filter signal
    digFilt = designfilt('bandpassiir','FilterOrder', 4,'HalfPowerFrequency1', mu_band(1),'HalfPowerFrequency2', beta_band(2),'SampleRate', fs);
    % Filtering data and Compensating for the delay
    data_filtered = filtfilt(digFilt, data);
    num_peaks = numel(findpeaks(data_filtered, Fs));
end
