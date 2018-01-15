%Comparin model spectrum with power spectral density

fname = input('Enter speech filename (testm.raw):  ','s');
if isempty(fname)
    fname = 'testm.raw';
end

fid = fopen(fname,'r','b');
speech = fread(fid, inf, 'int16');
[psd,freq] = pwelch(speech,[],[],1024,8000); % input, [],[], window size,
                                             % sampling frequency
P = 10*log10(psd);
figure

% order 3
order = 3;
model3 = aryule(speech,order); % input, AR model size
[H3,W] = freqz(1,model3,1024,8000);
subplot(2,2,1)
h=plot(freq,P,'k-',W,7+3*10*log10(abs(H3)),'k-');
set(h,{'LineWidth'},{1;2})
title('Model Order 3');
xlabel('Frequency (Hz)');
ylabel('dB');
%order 5
order = 5;
model5 = aryule(speech,order); % input, AR model size
[H5,W] = freqz(1,model5,1024,8000);
subplot(2,2,2)
h=plot(freq,P,'k-',W,7+3*10*log10(abs(H5)),'k-');
set(h,{'LineWidth'},{1;2})
title('Model Order 5');
xlabel('Frequency (Hz)');
ylabel('dB');

%order 11
order = 11;
model11 = aryule(speech,order); % input, AR model size
[H11,W] = freqz(1,model11,1024,8000);
subplot(2,2,3)
h=plot(freq,P,'k-',W,7+3*10*log10(abs(H11)),'k-');
set(h,{'LineWidth'},{1;2})
title('Model Order 11');
xlabel('Frequency (Hz)');
ylabel('dB');

%order 21
order = 21;
model21 = aryule(speech,order); % input, AR model size
[H21,W] = freqz(1,model21,1024,8000);
subplot(2,2,4)
h=plot(freq,P,'k-',W,7+3*10*log10(abs(H21)),'k-');
set(h,{'LineWidth'},{1;2})
title('Model Order 21');
xlabel('Frequency (Hz)');
ylabel('dB');


