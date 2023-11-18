function [original_signal,normalized_signal] = save_signal_to_csv(edf)
    % Read .edf and extract Fpz_ values and normalize them
    signal = edfread(edf);
    original_signal = vertcat(signal.Fpz_{:});
    normalized_signal = normalize(original_signal,"range"); % min-max
end