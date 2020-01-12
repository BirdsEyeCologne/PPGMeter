format long;
clc;
clear all;
close all;

bytes_per_entry = 33;
fname = '1.bin';

s = dir(fname);
fsize = s.bytes;
entries = floor(fsize/bytes_per_entry)

t1 = zeros(entries,1);
t2 = zeros(entries,1);
t3 = zeros(entries,1);
p = zeros(entries,1);
rpm = zeros(entries,1);
h = zeros(entries,1);
lat = zeros(entries,1);
lon = zeros(entries,1);
spd = zeros(entries,1);
alt = zeros(entries,1);
tim = zeros(entries,3);

file = fopen(fname,'r');

for cnt = 1:entries  
  t1(cnt,1) = fread(file,1, '*uint16');
  t2(cnt,1) = fread(file,1, '*uint16');
  t3(cnt,1) = fread(file,1, '*int16');
  p(cnt,1) = fread(file,1, '*uint32');
  rpm(cnt,1) = fread(file,1, '*uint16');
  h(cnt,1) = fread(file,1, '*uint32');
  lat(cnt,1) = fread(file,1, '*float');
  lon(cnt,1) = fread(file,1, '*float');
  spd(cnt,1) = fread(file,1, '*uint16');
  alt(cnt,1) = fread(file,1, '*int16');
  tim(cnt,:) = fread(file,3, '*uint8');  
end




x = 1:1:entries;

# https://de.wikipedia.org/wiki/Barometrische_H%C3%B6henformel
hm = (t3+273)./0.0065 .* (1-(p./101325).^(1/5.255));


#plot(x,alt./max(alt))
#plot(x,t1./max(t1))
#hold on

f = ones(10,1);
rpm_filt = convn(rpm, f, 'same');

#plot(x,alt)
#hold on;

figure(1,"position",get(0,"screensize"))

ax = plotyy(x,t1,x,rpm_filt./10)

xlim([1,entries])
title('Temp vs. RPM')
xlabel ("t [s]")
ylabel (ax(1), "CHT [C] ");
ylabel (ax(2), "RPM [60/s]");
legend('CHT', 'RPM',"location", "northwest")
grid on



figure(2,"position",get(0,"screensize"))
plot(x,alt)
hold on
plot(x,hm)

title('Baro vs. GPS')
legend('Baro', 'GPS',"location", "northwest")
xlabel ("t [s]")
ylabel('elev. [m]')
xlim([1,entries])
grid on




















