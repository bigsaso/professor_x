[preprocessed_signal, total_peaks] = clean_and_preprocess("S001R03.edf","Baseline, eyes open");
close all;
csvwrite('preprocessed_signal.csv', preprocessed_signal);