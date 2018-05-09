function [x,y] = fft_plot(datafft)
    P2 = abs(datafft/99);
    y = P2(1:99/2+1);
    y(2:end - 1) = abs(2*2*y(2:end-1));
    y = y/16000.0;
    x = 1000*(0:(99/2))/99;
end

