clear all;
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
s = serial("COM4");
fopen(s);
fprintf(s,'r');
data = zeros(99,4);
fscanf(s);
for i = 1:99
    val = fscanf(s);
    chars = strsplit(val, ', ');
    nums = cellfun(@str2num, chars);
    data(i,:) = nums;
end
datafft = fft(data)
[x1, y1] = fft_plot(datafft(:,1));
[xfir,yfir] = fft_plot(datafft(:,3));
[xmir,ymir] = fft_plot(datafft(:,2));
[xirr,yirr] = fft_plot(datafft(:,4));
subplot(2,2,1)
bar(x1/500,y1)
title('Original Accelerometer Z-acceleration')
ylabel('Z-Acceleration (g)')
xlabel('Frequency (hz)')
subplot(2,2,2)
bar(xmir/500,ymir)
title('Accelerometer Z-acceleration with a 10th Order MIR Filter')
ylabel('Z-Acceleration (g)')
xlabel('Frequency (hz)')
subplot(2,2,3)
bar(xfir/500,yfir)
title('Accelerometer Z-acceleration with a 10th Order FIR Filter and 5hz Cutoff')
ylabel('Z-Acceleration (g)')
xlabel('Frequency (hz)')
subplot(2,2,4)
bar(xirr/500,yirr)
title('Accelerometer Z-acceleration with a 2nd order MIR filter')
ylabel('Z-Acceleration (g)')
xlabel('Frequency (hz)')
fclose(s)
delete(s)
clear s