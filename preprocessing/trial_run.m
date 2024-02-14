% [preprocessed_signal, total_peaks] = old_clean_and_preprocess("S001R01.edf","Baseline, eyes open");
% close all;
% csvwrite('preprocessed_signal_S001R14.csv', preprocessed_signal);

% [original_signal,normalized_signal] = save_signal_to_csv("S001R15.edf");
% csvwrite('original_signal_S001R15.csv', original_signal);
% csvwrite('normalized_signal_S001R15.csv', normalized_signal);

[data_filtered, numPeaks] = clean_and_preprocess("original_signal_S001R01");