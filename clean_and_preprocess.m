function [preprocessed_signal,total_peaks] = clean_and_preprocess(edf, title_text)
    % Read .edf and extract Fpz_ values and normalize them
    task = edfread(edf);
    task_fpz = vertcat(task.Fpz_{:});
    task_fpz = normalize(task_fpz);
    Fs = 160; % Will become 512Hz once working with data coming from the actual headset.
    % Plot signal
    figure;
    plot(task_fpz, 'k');
    title(title_text);
    xlabel("time"); ylabel("mV");
    grid on;
    % Plot spectograms
    figure;
    spectrogram(task_fpz,[],[],[],Fs,'yaxis');
    title(title_text);
    xlabel("time"); ylabel("mV");
    grid on;
    % Design low pass filter and filter signal
    digFilt = designfilt('lowpassiir','PassbandFrequency',8,'StopbandFrequency',30,'PassbandRipple',1,'StopbandAttenuation',60,'SampleRate',Fs);
    task_fpz_filtered = filter(digFilt, task_fpz);
    % Plotting the delay of the lowpass filter
    grpdelay(digFilt,500,Fs)
    % Compensating for the delay
    task_fpz_delayCompensated = filtfilt(digFilt, task_fpz);
    % Plotting the differences between original, filtered, and delay compensated signals
    figure;
    hold on;
    % Plot the original signal in red
    plot(task_fpz, 'r');
    % Plot the filtered signal in blue
    plot(task_fpz_filtered, 'b');
    % Plot the filtered and delay-compensated signal in black
    plot(task_fpz_delayCompensated, 'k');
    title(title_text);
    legend('Original Signal', 'Filtered Signal', 'Filtered and Delay Accounted Signal');
    grid on;
    hold off;
    % Plotting the peaks
    figure;
    num_peaks = numel(findpeaks(task_fpz_delayCompensated, Fs));
    disp(['Peaks of ', title_text, ': ', num2str(num_peaks)]);
    findpeaks(task_fpz_delayCompensated, Fs);
    title(['Vertical Component - ', title_text]);
    xlabel('Time (sec)'); ylabel('Amplitude');
    % Return
    preprocessed_signal = task_fpz_delayCompensated;
    total_peaks = num_peaks;
end