:: https://github.com/niirmaaltwaatii/WinNet
REM N11rm44L 7w44711

echo {
set /p ssid="Name : "
set ./p pwd="Password : "
netsh wlan set hostednetwork mode=allow ssid=%ssid% key=%pwd%
echo }

pause

winnet.bat