table = readtable("short_signal_S001R01.csv");
data = table2array(table);
u = udpport;
u.OutputDatagramSize = 8192; % 1024 double values
write(u, data, "double", "localhost", 8080);